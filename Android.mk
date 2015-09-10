LOCAL_PATH := $(call my-dir)

#########################


include $(CLEAR_VARS)
LOCAL_SRC_FILES := shairport.c \
                   daemon.c \
                   rtsp.c \
                   mdns.c \
                   mdns_external.c \
                   mdns_tinysvcmdns.c \
                   ifaddrs.c \
                   common.c \
                   rtp.c \
                   metadata.c \
                   player.c \
                   alac.c \
                   audio.c \
                   audio_dummy.c \
                   audio_pipe.c \
                   tinysvcmdns.c 
                  

#LOCAL_SRC_FILES +=  audio_alsa.c

LOCAL_SRC_FILES += mdns_dns_sd.c

LOCAL_C_INCLUDES := external/airplay/shairport \
                    external/openssl \
                    external/openssl/include \
                    external/mdnsresponder/mDNSShared 
                             

LOCAL_MODULE := shairport
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -O2 -Wall


#LOCAL_SYSTEM_SHARED_LIBRARIES := libc libcutils
LOCAL_SHARED_LIBRARIES :=  liblog libssl libcrypto libtinyalsa 
LOCAL_SHARED_LIBRARIES +=libmdnssd

include $(BUILD_EXECUTABLE)


