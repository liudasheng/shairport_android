#ifndef __SHAIRPORT_LIB_H
#define __SHAIRPORT_LIB_H

#ifdef __cplusplus
extern "C" 
{
#endif

#define AIRPLAY_NAME_PROPERTY  "persist.airplay.name"
#define AIRPLAY_HOST_NAME      "O2"

typedef enum
{
    CLIENT_DISCONNECTED=0,
    CLIENT_CONNECTED,
}mConnectStatus_t;


int start_shairport(void);
int stop_shairport(void);
int set_shairport_hostname(const char *apname);
int get_shairport_hostname(char *apname);
mConnectStatus_t get_connect_status(void);


#ifdef __cplusplus
}
#endif

#endif //__SHAIRPORT_LIB_H

