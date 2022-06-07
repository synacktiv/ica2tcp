#ifndef _CTXSOCKS_H
#define _CTXSOCKS_H

#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wfapi.h>

#include "common.h"
#include "connection_manager_win.h"
#include "socket_queue_win.h"
#include "errors_win.h"
#include "debug_win.h"

#define SUCCESS 0
#define FAILURE 1


#define LOG_PATH "C:\Users\User1\Desktop\debug.txt"
#define POLL_LOG_PATH "C:\Users\User1\Desktop\debugpoll.txt"

typedef struct _MEMORY_SECTION {
	unsigned int length;
	unsigned char * pSection;
} MEMORY_SECTION;


typedef struct _ICA_PACKET_QUEUE {
	int size;
	int head;
	int tail;
	MEMORY_SECTION * sections;
	unsigned char * data_storage;
}ICA_PACKET_QUEUE, *pICA_PACKET_QUEUE;

typedef struct S_VC_INFO {
	HANDLE hVC;
	pICA_PACKET_QUEUE pIcaPacketQueue;
}VC_INFO, *pVC_INFO;

typedef struct S_DNS_PARAMS {
	char * hostname;
	ID id;
	HANDLE handle;
	pCONNECTION_MANAGER pConnectionManager;
	VC_INFO vcInfo;
	unsigned char * pSendingBuf;
}DNS_PARAMS, *pDNS_PARAMS;



typedef struct S_SELECT_ARGS {
	fd_set efds;
	int nfds;
}SELECT_ARGS, * pSELECT_ARGS;

int OpenVC(pVC_INFO);

int Send_Ica_MetaServerStart(VC_INFO, unsigned char *);
int Send_Ica_MetaServerStop(VC_INFO, unsigned char *);
int Send_Ica_CloseRequest(VC_INFO, unsigned char *, ID);
int Send_Ica_CloseAck(VC_INFO, unsigned char *, ID);
int Send_Ica_ServerRplPacket( VC_INFO, unsigned char *, ID, VER, RPL);
int Send_Ica_DataPacket(VC_INFO,  unsigned char *, ID, LEN, unsigned char *);
int Send_Ica_GenericPacket(VC_INFO, unsigned char *, PACKET_TYPE, ID, LEN, unsigned char *);
int Send_IcaQueue(VC_INFO);
int Send_Ica_QeueingPriority(VC_INFO, LEN, unsigned char *);
int Send_Ica_Queueing(VC_INFO, LEN, unsigned char *);
int Send_Icav2(VC_INFO, LEN, unsigned char *);
int Send_Ica(VC_INFO, int, unsigned char *);
int Send_IcaMeta(VC_INFO, unsigned char *, PACKET_TYPE, ID, LEN , unsigned char *);
int Send_Ica_MetaWindow(VC_INFO, unsigned char *, int, ID);

int Send_Socket_DataPacket(SOCKET, pSOCKET_QUEUE, LEN, unsigned char *, int *);
int Send_SocketQueue(SOCKET, pSOCKET_QUEUE, int *);
int Send_Socket_Queueing(SOCKET, pSOCKET_QUEUE, LEN, unsigned char *, int *);
int Send_Socketv2(SOCKET, LEN, unsigned char *, int *);
int Send_Socket(SOCKET, pSOCKET_QUEUE, unsigned int, char *);

void Parse_Ica_GenericPacket(IN unsigned char *, OUT PACKET_TYPE *, OUT ID *, OUT LEN *, OUT unsigned char **);
void Parse_Ica_ClientIpCmd(IN unsigned char *, OUT VER *, OUT CMD *, OUT IPADDR *, OUT PORT *);
void Parse_Ica_ClientIp6Cmd(IN unsigned char *, OUT VER *, OUT CMD *, OUT IP6ADDR *, OUT PORT *);
void Parse_Ica_ClientDomainCmd(IN unsigned char *, OUT VER *, OUT CMD *, OUT PORT *, OUT DOMLEN * , OUT DOMSTR *);
void Parse_Ica_MetaWindow(IN unsigned char *, OUT int *);

int Handler_Ica_DataPacket(pCONNECTION_MANAGER,VC_INFO, unsigned char *, ID, LEN, unsigned char *);
int Handler_Ica_ClientIpCmd(pCONNECTION_MANAGER, VC_INFO, unsigned char *, ID, LEN, unsigned char *);
int Handler_Ica_ClientIp6Cmd(pCONNECTION_MANAGER, VC_INFO, unsigned char *, ID, LEN, unsigned char *);
int Handler_Ica_ClientDomainCmd(pCONNECTION_MANAGER, VC_INFO, unsigned char *, ID, LEN, unsigned char *);
int Handler_Ica_CloseRequest(pCONNECTION_MANAGER, VC_INFO, unsigned char *, ID);
int Handler_Ica_CloseAck(pCONNECTION_MANAGER, ID);
int Handler_Ica_MetaWindow(unsigned char *, pCONNECTION_MANAGER, ID);

SOCKET OpenSocket(IPADDR, PORT);
SOCKET OpenSocketIp6(IP6ADDR, PORT);
SOCKET OpenSocketDomain(DOMSTR, PORT);

DWORD WINAPI ResolveDNS(LPVOID);

int ConnectSocket(pCONNECTION_MANAGER, int);

int IcaPacketQueue_Init(pICA_PACKET_QUEUE *);
void IcaPacketQueue_Close(pICA_PACKET_QUEUE);
int IcaPacketQueue_Enqueue(pICA_PACKET_QUEUE, MEMORY_SECTION *, int);
int IcaPacketQueue_EnqueueHead(pICA_PACKET_QUEUE, MEMORY_SECTION *, int);
int IcaPacketQueue_Dequeue(pICA_PACKET_QUEUE);
MEMORY_SECTION * IcaPacketQueue_GetHead(pICA_PACKET_QUEUE);
int IcaPacketQueue_IsFull(pICA_PACKET_QUEUE);
int IcaPacketQueue_IsEmpty(pICA_PACKET_QUEUE);
int IcaPacketQueue_GetFreeSpace(pICA_PACKET_QUEUE);
int IcaPacketQueue_Send(VC_INFO);

int CloseProcedure(pCONNECTION_MANAGER, VC_INFO, unsigned char *, ID);

void debugBuf(void *, int);
void debug(pCONNECTION_MANAGER, pICA_PACKET_QUEUE);


#endif /* _CTXSOCKS_H  */

