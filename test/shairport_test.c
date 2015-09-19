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
#include <utils/Log.h>
#include <cutils/properties.h>

#include "airplay.h"

MetaData_t MetaData;

int notify(int msg, int ext1, int ext2)
{
    printf("notify: msg=%d, ext1=%d, ext2=%d\n", msg, ext1, ext2);
    switch(msg)
    {
        case MEDIA_INFO:
            switch(ext1)
            {
                case MEDIA_INFO_METADATA_UPDATE:
                    memset(&MetaData, 0, sizeof(MetaData_t));
                    get_metadata(&MetaData);
                    printf("META album: %s\n", MetaData.album);
                    printf("META artist: %s\n", MetaData.artist);
                    printf("META genre: %s\n", MetaData.genre);
                    printf("META title: %s\n", MetaData.title);
                    break;
                default:
                    break;
            }

        default:
            break;
    }
    return 0;    
}

int main(int argc, char **argv) 
{
    register_airplay_notify(notify);

    start_airplay();
    
    while(1)
    {
        //printf("start_shairport\n");
        //start_airplay();

        //sleep(20);

        //printf("stop_shairport\n");
        //stop_airplay();

        sleep(1);
    }
    
    return 0;
}

