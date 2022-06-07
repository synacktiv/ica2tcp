#ifndef VDSOCKET_H
#define VDSOCKET_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <errno.h>

#ifdef SDK
#include <citrix/citrix.h>
#include <citrix/ica.h>
#include <citrix/ica-c2h.h>
#include <citrix/clterr.h>
#include <citrix/wdapi.h>
#include <citrix/vdapi.h>
#include <citrix/vd.h>
#include <citrix/cmacro.h>
#include <citrix/ctxnevt.h>
#include <citrix/miapi.h>
#include <citrix/ctxntmr.h>
#else
#include <citrix.h>
#include <ica.h>
#include <ica-c2h.h>
#include <clterr.h>
#include <wdapi.h>
#include <vdapi.h>
#include <vd.h>
#include <cmacro.h>
#include <ctxnevt.h>
#include <miapi.h>
#include <ctxntmr.h>
#endif

#include "connection_manager.h"
#include "common.h"
#include "debug.h"
#include "errors.h"
#include "debug_extern.h"
#include "socket_queue.h"
#include "ica_queue.h"
#include "ica_queue.h"





#define LISTENING_PORT 33556

extern int Load();

struct _wrapload {
    int ((*fn)());
} WRAP_LOAD[] = {
        {Load}
};

//typedef struct S_ICA_PACKET_QUEUE{
//    int size;
//    int head;
//    int tail;
//    MEMORY_SECTION * sections ;
//    unsigned char * data_storage;
//}ICA_PACKET_QUEUE, * pICA_PACKET_QUEUE;


typedef struct S_VC_INFO{
    PWD pWd;
    USHORT vcNum;
    USHORT maxDataSize;
}VC_INFO, * pVC_INFO;

typedef struct S_ICASEND_ARGS{
    VC_INFO vcInfo;
    pICA_PACKET_QUEUE pIcaPacketQueue;
}ICASEND_ARGS, * pICASENDARGS;

typedef struct S_EPOLL_ARGS{
    int epollfd;
    int maxevents;
    struct epoll_event * pEvents;
}EPOLL_ARGS, * pEPOLL_ARGS;

typedef struct S_CALLBACK_ARGS_LISTENING_SOCKET{
    pCONNECTION_MANAGER pConnectionManager;
    ICASEND_ARGS icasendArgs;
    int listening_socket;
    PEVT pEvt;
    pEPOLL_ARGS pEpollArgs;

}CALLBACK_ARGS_LISTENING_SOCKET, * pCALLBACK_ARGS_LISTENING_SOCKET;

typedef struct S_CALLBACK_ARGS_CLIENT_SOCKET{
    pCONNECTION_MANAGER pConnectionManager;
    ICASEND_ARGS icasendArgs;
    unsigned char * pSocketRecvBuf;
    pEPOLL_ARGS pEpollArgs;
}CALLBACK_ARGS_CLIENT_SOCKET, * pCALLBACK_ARGS_CLIENT_SOCKET;

typedef struct S_CALLBACK_ARGS_TIMER{
    ICASEND_ARGS  icasendArgs;
    pCONNECTION_MANAGER pConnectionManager;
    pEPOLL_ARGS pEpollArgs;
}CALLBACK_ARGS_TIMER, * pCALLBACK_ARGS_TIMER;

typedef struct S_ICA_ARRIVAL_ARGS{
    ICASEND_ARGS icasendArgs;
    pCONNECTION_MANAGER pConnectionManager;
    pEPOLL_ARGS pEpollArgs;
}ICA_ARRIVAL_ARGS;

typedef struct S_DRIVER_CLOSE_ARGS{
    pCONNECTION_MANAGER pConnectionManager;
    unsigned char * pSocketRecvBuf;
    pICA_PACKET_QUEUE pIcaPacketQueue;
    pCALLBACK_ARGS_CLIENT_SOCKET pCallbackArgsClientSocket;
    pCALLBACK_ARGS_LISTENING_SOCKET pCallbackArgsListeningSocket;
    pCALLBACK_ARGS_TIMER pCallbackArgsTimer;
    PEVT * pPevtListeningSocket;
    PEVT * pPevtClientSocket;
    PTMR * pPtmr;
    int listening_socket;
    pEPOLL_ARGS pEpollArgs;

}DRIVER_CLOSE_ARGS;

typedef struct S_VD_CALLBACK_ARGS{
    ICA_ARRIVAL_ARGS icaArrivalArgs;
    DRIVER_CLOSE_ARGS driverCloseArgs;
}VD_CALLBACK_ARGS, * pVD_CALLBACK_ARGS;



__attribute__((unused)) int DriverOpen( PVD, PVDOPEN, PUINT16 );

__attribute__((unused)) int DriverClose(__attribute__((unused)) PVD, __attribute__((unused)) PDLLCLOSE,
                                        __attribute__((unused)) PUINT16 );

__attribute__((unused)) int DriverInfo( PVD, PDLLINFO, PUINT16 );

__attribute__((unused)) int DriverPoll(__attribute__((unused)) PVD, __attribute__((unused)) PDLLPOLL,
                                       __attribute__((unused)) PUINT16 );

__attribute__((unused)) int DriverQueryInformation(__attribute__((unused)) PVD,
                                                   __attribute__((unused)) PVDQUERYINFORMATION, PUINT16 );

__attribute__((unused)) int DriverSetInformation(__attribute__((unused)) PVD, __attribute__((unused)) PVDSETINFORMATION,
                                                 __attribute__((unused)) PUINT16 );

__attribute__((unused)) int DriverGetLastError( PVD, PVDLASTERROR );

static int WFCAPI ICADataArrival(__attribute__((unused)) PVOID, __attribute__((unused)) USHORT, LPBYTE,
                                 __attribute__((unused)) USHORT );

void Callback_ListeningSocket(void *pSubId, __attribute__((unused)) void *pEvt);
void Callback_ClientSocket(void *pSubId, __attribute__((unused)) void *pEvt);
void Callback_Timer(void * arg1, __attribute__((unused)) void * arg2);

int Handler_Meta_ServerStart(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs);
int Handler_Meta_ServerStop(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs);
int Handler_MetaWindow(pCONNECTION_MANAGER pConnectionManager, ID id, unsigned char * pBuf);
int
Handler_Ica_DataPacket(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs, ID id,
                       LEN len, unsigned char *pBuf);
int
Handler_Ica_ServerRplPacket(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs,
                            ID id, LEN len, unsigned char *pBuf);
int Handler_Ica_ClosePacket(pCONNECTION_MANAGER pConnectionManager, ID id, __attribute__((unused)) LEN len,
                            __attribute__((unused)) unsigned char *pBuf);

int Handler_Ica_CloseRequest(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs,
                             ID id);
int Handler_Ica_CloseAck(pCONNECTION_MANAGER pConnectionManager, pEPOLL_ARGS pEpollArgs, ID id);


int OpenListeningSocket(unsigned short port);

void debug_info(FILE *fd);

int Send_Socket(int fd, LEN len, unsigned char *pBuf, int *bytesSent);
int Send_Socket_Queueing(int fd, pSOCKET_QUEUE pQueue, LEN len, unsigned char *pBuf, int *bytesSent);
int Send_SocketQueue(int fd, pSOCKET_QUEUE pSocketQueue, int *bytesSent);
int Send_Socket_DataPacket(int fd, pSOCKET_QUEUE pSocketQueue, LEN len, unsigned char *pBuf, int *bytesSent);
int Send_Ica(VC_INFO vcInfo, MEMORY_SECTION *pMS, unsigned char nMS);
int Send_Ica_QueueingPriority(ICASEND_ARGS icasendArgs, MEMORY_SECTION *pMS, unsigned char nMS);
int Send_Ica_Queueing(ICASEND_ARGS icasendArgs, MEMORY_SECTION *pMS, unsigned char nMS);
int Send_Ica_GenericPacket(ICASEND_ARGS icasendArgs, PACKET_TYPE type, ID id, LEN len, unsigned char *pData);
int Send_Ica_Meta(ICASEND_ARGS icasendArgs, PACKET_TYPE type, LEN len, unsigned char *pData, ID id);
int Send_Socket_MethodSelectionPacket(int fd, VER ver, METHOD method, pSOCKET_QUEUE pSocketQueue, int *pBytesSent);
int Send_Ica_ClosePacket(VC_INFO vcInfo, pICA_PACKET_QUEUE pIcaPacketQueue, ID id);
int Send_Ica_CloseRequest(ICASEND_ARGS icasendArgs, ID id);
int Send_Ica_CloseAck(ICASEND_ARGS icasendArgs, ID id);
int Send_Ica_MetaWindow(ICASEND_ARGS icasendArgs, int window, ID id);
int Send_Socket_RplPacket(int fd, VER ver, RPL rpl, IPADDR bndAddr, PORT bndPort, pSOCKET_QUEUE pSocketQueue,
                          int *pBytesSent);
int Send_Ica_IpCmdPacket(ICASEND_ARGS icasendArgs, ID id, VER ver, CMD cmd, IPADDR ipAddr, PORT port);
int Send_Ica_Ip6CmdPacket(ICASEND_ARGS icasendArgs, ID id, VER ver, CMD cmd, PORT port, IP6ADDR ip6Addr);
int Send_IcaQueue(ICASEND_ARGS icasendArgs);
int
Send_Ica_DomainCmdPacket(ICASEND_ARGS icasendArgs, ID id, VER ver, CMD cmd, PORT port, DOMLEN domLen, DOMSTR pDomData);
int Send_Ica_DataPacket(ICASEND_ARGS icasendArgs, ID id, LEN len, unsigned char *pData);
void Parse_Ica_ServerRplPacket(IN unsigned char *pPacketBuf, OUT VER *pver, OUT RPL *prpl);
void Parse_Socket_ClientMethodsPacket(IN unsigned char *pPacketBuf, OUT VER *pver, OUT NMETHODS *pnmethods, OUT unsigned char **ppmethods);
void
Parse_Socket_ClientCmdPacket(unsigned char *pPacketBuf, VER *pver, CMD *pcmd, RSV *prsv, ATYP *patyp, IPADDR *pipaddr,
                             IP6ADDR *pip6addr, PORT *pport, DOMLEN *pdomlen, unsigned char **ppdomdata);
void Parse_Ica_GenericPacket(IN unsigned char *pPacketBuf, OUT PACKET_TYPE *ptype, OUT ID *pid, OUT LEN *plen, OUT unsigned char **ppdata);
void Parse_Ica_MetaWindow(IN const unsigned char * pPacketBuf, OUT int * pWindow);


MEMORY_SECTION * IcaPacketQueue_GetHead(pICA_PACKET_QUEUE pQueue);
int IcaPacketQueue_Dequeue(pICA_PACKET_QUEUE pQueue);
int IcaPacketQueue_Enqueue(pICA_PACKET_QUEUE pQueue, MEMORY_SECTION * pMS, int nMS);
int IcaPacketQueue_EnqueueHead(pICA_PACKET_QUEUE pQueue, MEMORY_SECTION * pMS, int nMS);
int IcaPacketQueue_Init(pICA_PACKET_QUEUE * ppQueue);
int IcaPacketQueue_Send(pICA_PACKET_QUEUE pQueue, VC_INFO vcInfo);
int IcaPacketQueue_IsEmpty(pICA_PACKET_QUEUE pQueue);

__attribute__((unused)) int IcaPacketQueue_IsFull(pICA_PACKET_QUEUE pQueue);
int IcaPacketQueue_GetFreeSpace(pICA_PACKET_QUEUE pQueue);
void IcaPacketQueue_Close(pICA_PACKET_QUEUE pQueue);

int
InitiateCloseProcedure(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, ID id, pEPOLL_ARGS pEpollArgs);
int CloseConnection(pCONNECTION_MANAGER pConnectionManager, pEPOLL_ARGS pEpollArgs, ID id);



void debugBuf(void *, int);



#endif /* VDSOCKET_H */