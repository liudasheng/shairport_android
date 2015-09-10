CONFIG_ALSA=yes
CONFIG_AVAHI=yes
CONFIG_HAVE_GETOPT_H=yes
CFLAGS+=   -I/usr/include/alsa   -D_REENTRANT  
LDFLAGS+= -lm -lpthread -lssl -lcrypto   -lasound   -lavahi-common -lavahi-client  
