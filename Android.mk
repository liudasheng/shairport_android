LOCAL_PATH := $(call my-dir)

#CONFIG_TINYALSA := true
CONFIG_ALSA := true
CONFIG_DNS_SD := true

CONFIG_BUILD_LIBRARY := true

#########################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := shairport.c \
                   daemon.c \
                   rtsp.c \
				   rtp.c \
                   ifaddrs.c \
                   mdns.c \
                   mdns_external.c \
                   mdns_tinysvcmdns.c \
                   tinysvcmdns.c \
                   common.c \
                   metadata.c \
                   player.c \
                   alac.c \
                   audio.c \
                   audio_dummy.c \
                   audio_pipe.c 
                   
                  
LOCAL_C_INCLUDES := external/airplay/shairport \
                    external/openssl \
                    external/openssl/include 

LOCAL_MODULE := shairport
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -O2 -Wall 

LOCAL_SHARED_LIBRARIES :=  liblog libssl libcrypto libcutils libc


ifeq ($(strip $(CONFIG_ALSA)),true)
LOCAL_CFLAGS += -D_POSIX_SOURCE -D_POSIX_C_SOURCE
LOCAL_SRC_FILES +=  audio_alsa.c
LOCAL_C_INCLUDES += external/airplay/alsa-lib/include
LOCAL_SHARED_LIBRARIES += libasound
endif 

ifeq ($(strip $(CONFIG_DNS_SD)),true)
LOCAL_C_INCLUDES += external/mdnsresponder/mDNSShared 
LOCAL_SRC_FILES += mdns_dns_sd.c
LOCAL_SHARED_LIBRARIES += libmdnssd
endif

ifeq ($(strip $(CONFIG_TINYALSA)),true)
LOCAL_SHARED_LIBRARIES += libtinyalsa
endif

include $(BUILD_EXECUTABLE)



#########################
ifeq ($(strip $(CONFIG_BUILD_LIBRARY)),true)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := shairport_lib.c \
                   daemon.c \
                   rtsp.c \
				   rtp.c \
                   ifaddrs.c \
                   mdns.c \
                   mdns_external.c \
                   mdns_tinysvcmdns.c \
                   tinysvcmdns.c \
                   common.c \
                   metadata.c \
                   player.c \
                   alac.c \
                   audio.c \
                   audio_dummy.c \
                   audio_pipe.c 
                   
                  
LOCAL_C_INCLUDES := external/airplay/shairport \
                    external/openssl \
                    external/openssl/include 

LOCAL_MODULE := libshairport
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -O2 -Wall -DBUILD_LIBRARY

LOCAL_SHARED_LIBRARIES :=  liblog libssl libcrypto libcutils libc 


ifeq ($(strip $(CONFIG_ALSA)),true)
LOCAL_CFLAGS += -D_POSIX_SOURCE -D_POSIX_C_SOURCE
LOCAL_SRC_FILES +=  audio_alsa.c
LOCAL_C_INCLUDES += external/airplay/alsa-lib/include
LOCAL_SHARED_LIBRARIES += libasound
endif 

ifeq ($(strip $(CONFIG_DNS_SD)),true)
LOCAL_C_INCLUDES += external/mdnsresponder/mDNSShared 
LOCAL_SRC_FILES += mdns_dns_sd.c
LOCAL_SHARED_LIBRARIES += libmdnssd
endif

ifeq ($(strip $(CONFIG_TINYALSA)),true)
LOCAL_SHARED_LIBRARIES += libtinyalsa
endif

include $(BUILD_SHARED_LIBRARY)

#################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := test/shairport_test.c
LOCAL_C_INCLUDES := external/airplay/shairport  
LOCAL_MODULE := shairport_test
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES += libshairport libcutils libc liblog
include $(BUILD_EXECUTABLE)

endif
