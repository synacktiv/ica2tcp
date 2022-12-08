
#ifndef __COMMON_H__
#define __COMMON_H__


#ifdef Linux
#ifdef SDK
#include <citrix/ica.h>
#include <citrix/ica-c2h.h>       /* for VD_C2H structure */
#else
#include <ica.h>
#include <ica-c2h.h>
#endif
#else
#include "ica.h"
#include "ica-c2h.h"
#endif

#ifdef DEBUG_NAME
#define VIRTUAL_CHANNEL_NAME "CTXSCKD"
#else
#define VIRTUAL_CHANNEL_NAME "CTXSCKS"
#endif
#define MAX_ICA_PACKET_DATA_SIZE 2048

#define MAX_QUEUE_SIZE 5000
#define MAX_SOCKET_LIST_INDEX 1000


#define IN
#define OUT

#define ID_SIZE 4
#define ID_TYPE unsigned int

#define PACKET_TYPE_SIZE 1
#define PACKET_TYPE_TYPE unsigned char
#define LEN_SIZE 2
#define LEN_TYPE unsigned short
#define VER_SIZE 1
#define VER_TYPE unsigned char
#define CMD_SIZE 1
#define CMD_TYPE unsigned char
#define NMETHODS_SIZE 1
#define NMETHODS_TYPE unsigned char
#define METHOD_SIZE 1
#define METHOD_TYPE unsigned char
#define IPADDR_SIZE 4
#define IPADDR_TYPE unsigned int
#define IP6ADDR_SIZE 16
#define IP6ADDR_TYPE unsigned char *
#define PORT_SIZE 2
#define PORT_TYPE unsigned short
#define RPL_SIZE 1
#define RPL_TYPE unsigned char
#define ATYP_SIZE 1
#define ATYP_TYPE unsigned char
#define DOMLEN_SIZE 1
#define DOMLEN_TYPE unsigned char
#define RSV_SIZE 1
#define RSV_TYPE unsigned char
#define MAX_DOMSTR_SIZE 256
#define DOMSTR_TYPE unsigned char *

typedef ID_TYPE ID;
typedef  LEN_TYPE LEN;
typedef PACKET_TYPE_TYPE PACKET_TYPE;
typedef VER_TYPE VER;
typedef CMD_TYPE CMD;
typedef IPADDR_TYPE IPADDR;
typedef IP6ADDR_TYPE IP6ADDR;
typedef  PORT_TYPE PORT;
typedef RPL_TYPE RPL;
typedef ATYP_TYPE ATYP;
typedef DOMLEN_TYPE DOMLEN;
typedef RSV_TYPE RSV;
typedef NMETHODS_TYPE NMETHODS;
typedef METHOD_TYPE METHOD;
typedef DOMSTR_TYPE DOMSTR;

typedef enum _PACKET_TYPE{
    META_SERVER_START,
    META_SERVER_STOP,
    META_WINDOW,
    SOCKS_CLIENT_IP_CMD,
    SOCKS_CLIENT_IP6_CMD,
    SOCKS_CLIENT_DOMAIN_CMD,
    SOCKS_SERVER_RPL,
    SOCKS_DATA,
    REVERSE_BIND_CMD,
    REVERSE_BIND_RPL,
    REVERSE_CONNECT_CMD,
    REVERSE_CONNECT_RPL,
    REVERSE_CLOSE_REQUEST,
    REVERSE_CLOSE_ACK,
    CLOSE_REQUEST,
    CLOSE_ACK
} PACKET_TYPE_ENUM;


typedef struct pollfd  POLLFD;

typedef struct _VDSOCKS_C2H{
    VD_C2H Header; //the first fiel d must be a VD_C2H struct, then we put whatever we want
}VDSOCKS_C2H, * PVDSOCKS_C2H;

//#pragma pack()
#endif /* __COMMON_H__ */
