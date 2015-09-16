/*
 * Shairport, an Apple Airplay receiver
 * Copyright (c) James Laird 2013
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

//#define LOG_NDEBUG 0
//#define LOG_TAG "shairport"
//#include <utils/Log.h>
#include <cutils/properties.h>

#include "airplay.h"

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <openssl/md5.h>
#include <sys/wait.h>
#include <getopt.h>
#include "common.h"
#include "daemon.h"
#include "rtsp.h"
#include "mdns.h"
#include "getopt_long.h"
#include "metadata.h"

static const char *version =
    #include "version.h"
    ;

static void log_setup();

static int shutting_down = 0;


void shairport_shutdown(int retval) {
    ALOGD("%s in.", __func__);
    if (shutting_down)
        return;
    shutting_down = 1;
    printf("Shutting down...\n");
    mdns_unregister();
    rtsp_shutdown_stream();
    if (config.output)
        config.output->deinit();
    daemon_exit(); // This does nothing if not in daemon mode

    #ifndef BUILD_LIBRARY
    exit(retval);
    #endif
    ALOGD("%s ok.", __func__);
}

// forked daemon lets the spawner know it's up and running OK
// should be called only once!
void shairport_startup_complete(void) {
    if (config.daemonise) {
        daemon_ready();
    }
}

static void log_setup() {
    if (config.logfile) {
        int log_fd = open(config.logfile,
                O_WRONLY | O_CREAT | O_APPEND,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (log_fd < 0)
            ALOGE("Could not open logfile");

        dup2(log_fd, STDOUT_FILENO);
        setvbuf (stdout, NULL, _IOLBF, BUFSIZ);

        if (!config.errfile) {
            dup2(log_fd, STDERR_FILENO);
            setvbuf (stderr, NULL, _IOLBF, BUFSIZ);
        }
    }

    if (config.errfile) {
        int err_fd = open(config.errfile,
                O_WRONLY | O_CREAT | O_APPEND,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (err_fd < 0)
            ALOGE("Could not open logfile");

        dup2(err_fd, STDERR_FILENO);
        setvbuf (stderr, NULL, _IOLBF, BUFSIZ);
    }
}

static char airplay_apname[128];
static pthread_t rtsp_thread;
static int shairport_running = 0;

static void *rtsp_listen_thread(void *arg) 
{
    ALOGD("%s in.", __func__);
    rtsp_listen_loop();
    ALOGD("%s ok.", __func__);
    return NULL;
}

static int start_shairport(void) 
{
    ALOGD("%s in.", __func__);

    shutting_down = 0;

    if(1 == shairport_running)
    {
        ALOGI("shairport is already running!");
        return 0;
    }
    shairport_running = 1;

    memset(&config, 0, sizeof(config));

    // set defaults
    config.buffer_start_fill = 220;
    config.port = 5002;

    memset(airplay_apname, 0, sizeof(airplay_apname));
    property_get(AIRPLAY_NAME_PROPERTY, airplay_apname, AIRPLAY_HOST_NAME);
    config.apname = airplay_apname;

    // mDNS supports maximum of 63-character names (we append 13).
    if (strlen(config.apname) > 50)
    {
        ALOGE("Supplied name too long (max 50 characters)");
        return 1;
    }

    if (config.daemonise) {
        daemon_init();
    }

    log_setup();

    config.output = audio_get_output(config.output_name);
    if (!config.output) {
        audio_ls_outputs();
        ALOGE("Invalid audio output specified!");
        return 2;
    }
    char *argv[1];
    config.output->init(0, argv+1);
    
    uint8_t ap_md5[16];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, config.apname, strlen(config.apname));
    MD5_Final(ap_md5, &ctx);
    memcpy(config.hw_addr, ap_md5, sizeof(config.hw_addr));

    if (config.meta_dir)
        metadata_open();
    
    ALOGV("shairport version: %s", version);
    ALOGV("shairport config:");
    ALOGV("config.buffer_start_fill=%d", config.buffer_start_fill);
    ALOGV("config.port=%d", config.port);
    ALOGV("config.apname=%s", config.apname);

    //rtsp_listen_loop();
    ALOGD("start to creat rtsp_listen_thread...");
    
    int ret = pthread_create(&rtsp_thread, NULL, rtsp_listen_thread, NULL);
    if(0 != ret)
    {
        ALOGE("%s: pthread_create rtsp_listen_thread failed, ret=0x%x", __func__, ret);
        return ret;
    }

    send_event(MEDIA_PREPARED, 0, 0);

    ALOGD("success to creat rtsp_listen_thread!");

    ALOGD("%s ok.", __func__);
    return 0;
}

static int stop_shairport(void)
{
    ALOGD("%s in.", __func__);

    if(0 == shairport_running)
    {
        ALOGI("shairport is already stoped!");
        return 0;
    }
    shairport_running = 0;
    
    shairport_shutdown(0);

    send_event(MEDIA_STOPPED, 0, 0);
    
    #ifndef BUILD_LIBRARY
    pthread_kill(rtsp_thread, SIGUSR1);
    void *retval;
    pthread_join(rtsp_thread, &retval);
    #endif 
    
    ALOGD("%s ok.", __func__);
    
    return 0;
}

static int set_shairport_hostname(const char * apname)
{

    if(NULL == apname)
    {
        ALOGE("apname is a null poiter!");
        return 1;
    }
    
    property_set(AIRPLAY_NAME_PROPERTY, apname);
    return 0;
}

static int get_shairport_hostname(char * apname)
{

    if(NULL == apname)
    {
        ALOGE("apname is a null poiter!");
        return 1;
    }
    
    property_get(AIRPLAY_NAME_PROPERTY, apname, AIRPLAY_HOST_NAME);
    return 0;
}


static airplay_notify_cb airplay_notify = NULL;
int register_airplay_notify(const airplay_notify_cb notify_fn)
{
    if (notify_fn)
        airplay_notify = notify_fn;

    return 0;
}

int send_event(int msg, int ext1, int ext2)
{
    ALOGV("%s: msg=%d, ext1=%d, ext2=%d", __func__, msg, ext1, ext2);

    if(airplay_notify)
    {
        ALOGD("%s: msg=%d, ext1=%d, ext2=%d", __func__, msg, ext1, ext2);
        airplay_notify(msg, ext1, ext2);
    }
    return 0;
}

int start_airplay(void)
{
    return start_shairport();
}
    
int stop_airplay(void)
{
    return stop_shairport();
}

int set_airplay_hostname(const char *apname)
{
    return set_shairport_hostname(apname);
}

int get_airplay_hostname(char *apname)
{
    return get_shairport_hostname(apname);
}

