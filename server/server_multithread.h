#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wfapi.h>
#include <mswsock.h>


#include "common.h"
#include "queue_win.h"
#include "errors_win.h"
#include "debug_win.h"
#include "dyn_array.h"

//#define SUCCESS 0
//#define FAILURE 1

#define INITIAL_WINDOW 100000
#define INITIAL_MAPPING_SIZE 300

#define ID_SIZE 4
#define ID_TYPE unsigned int

typedef ID_TYPE ID;

typedef enum E_CONNECTION_STATE {
	NEW,
	FREE,
	//SOCKET_OPEN,
	//METHOD_CHOSEN,
	//CMD_SENT,
	//CMD_RECEIVED,
	LISTENING,
	RESOLVING,
	//CLOSE_REQUEST_SENT,
	RESOLVED,
	UNRESOLVED,
	REVERSE_CONNECT_CMD_SENT,
	CONNECTING,
	ESTABLISHED,
	//CLOSING,
} CONNECTION_STATE;

typedef enum E_CLOSE_STATE{ 
	NONE,
	REQ_RECEIVED_TO_CLOSE, //means CLOSE_REQ was received, closesocket() already done, waiting for IO to complete. 
	REQ_SENT, //means CLOSE_REQ is sent, closesocket() has been called, waiting for IO to complete and ACK to arrive simultaneously
	ACK_RECEIVED_TO_CLOSE, //means that ACK was received but IO was pending at the moment, wait for it to complete
} CLOSING_STATE;

typedef enum E_IO_TYPE{
	NONE_PENDING, //used to signal that no IO is currently pending on the overlapped structure.
	RESOLUTION_SUCCESS,
	RESOLUTION_FAIL,
	CONNECTION,
	RECEPTION,
	SENDING,
	ACCEPTING,
}IO_TYPE;

// typedef struct _MEMORY_SECTION {
// 	unsigned int length;
// 	unsigned char * pSection;
// } MEMORY_SECTION;


// typedef struct _ICA_PACKET_QUEUE {
// 	int size;
// 	int head;
// 	int tail;
// 	MEMORY_SECTION * sections;
// 	unsigned char * data_storage;
// }ICA_PACKET_QUEUE, *pICA_PACKET_QUEUE;

typedef struct S_VC_INFO {
	HANDLE hVC;
	//pICA_PACKET_QUEUE pIcaPacketQueue;
}VC_INFO, *pVC_INFO;

typedef struct S_PACKET{
    PACKET_TYPE type;
    ID id;
    LEN len;
    char * pData;
}PACKET, *pPACKET;



typedef struct S_PER_IO_DATA{
	WSAOVERLAPPED wsaoverlapped;
	WSABUF wsabuf;
	CHAR buffer[2000];
	IO_TYPE ioType;
}PER_IO_DATA, *pPER_IO_DATA;

typedef struct S_PER_ACCEPT_DATA{
	WSAOVERLAPPED wsaoverlapped;
	SOCKET acceptSocket;
	char buffer[1000];
	IO_TYPE ioType;
}PER_ACCEPT_DATA, * pPER_ACCEPT_DATA;


typedef struct S_PER_SOCKET_DATA{
	ID id;
	SOCKET socket;
	CONNECTION_STATE state;
	CLOSING_STATE closingState;
	QUEUE sOutQueue;
	QUEUE sInQueue;
	CHAR pDomainName[300];
	CHAR pPortName[10];
	SOCKADDR_STORAGE server;
	PADDRINFOEXW pAddrInfo;
	pPER_IO_DATA pPerIoData;
	pPER_IO_DATA pPerIoDataSending;
	int window;
	BOOL waitForWindow;
	struct S_PER_SOCKET_DATA * pNext;
	struct S_PER_SOCKET_DATA * pPrev;
}PER_SOCKET_DATA, * pPER_SOCKET_DATA;

typedef struct S_PER_REVERSEBIND_DATA{
	ID id;
	SOCKET listeningSocket;
	CLOSING_STATE closingState;
	pPER_ACCEPT_DATA pPerAcceptData;
	struct S_PER_REVERSEBIND_DATA * pNext;
	struct S_PER_REVERSEBIND_DATA * pPrev;
}PER_REVERSEBIND_DATA, *pPER_REVERSEBIND_DATA;

typedef struct S_REVERSEBIND_WORKER_THREAD_ARGS{
	HANDLE completionPort;
	HANDLE reverseBindCompletionPort;
	pPER_REVERSEBIND_DATA * ppReverseBindList;
	pPER_SOCKET_DATA * ppConnectionList;
	pDYN_ARRAY p_daTmpIdMapping;
	LPFN_ACCEPTEX AccecptEx;
	VC_INFO vcInfo;
}REVERSEBIND_WORKER_THREAD_ARGS, * pREVERSEBIND_WORKER_THREAD_ARGS;

typedef struct S_SOCKET_RECV_WORKER_THREAD_ARGS{
	HANDLE completionPort;
	VC_INFO vcInfo;
	//PCHAR pIcaSendBuf;
	pDYN_ARRAY p_daIdMapping;
	pDYN_ARRAY p_daTmpIdMapping;
	pPER_SOCKET_DATA * ppConnectionList;
	LPFN_CONNECTEX ConnectEx;
} SOCKET_RECV_WORKER_THREAD_ARGS, *pSOCKET_RECV_WORKER_THREAD_ARGS;

typedef struct S_RESOLVE_THREAD_ARGS{
	pPER_SOCKET_DATA pPerSocketData;
	HANDLE handleSelf;
	CRITICAL_SECTION * pCriticalSection;
	VC_INFO vcInfo;
	pDYN_ARRAY p_daIdMapping;
	pPER_SOCKET_DATA * ppConnectionList;
	HANDLE hCompletionPort;
} RESOLVE_THREAD_ARGS, * pRESOLVE_THREAD_ARGS;

DWORD WINAPI SocketRecvWorkerThread(LPVOID args);
DWORD WINAPI ReverseBindWorkerThread(LPVOID args);
DWORD WINAPI ResolveDNS(LPVOID args);

int OpenVC(pVC_INFO);
void Parse_Ica_GenericPacket(IN char * , OUT pPACKET);

int Handler_Ica_DataPacket(VC_INFO, PACKET);
SOCKET OpenSocket(IPADDR addr, PORT port);
void Parse_Ica_GenericPacket(IN char * pPacketBuf, OUT pPACKET pPacket);
int Send_Ica_MetaWindow(VC_INFO vcInfo, char * sendingBuf, int window, ID id);
int Send_Ica_MetaServerStart(VC_INFO vcInfo, char * sendingBuf);
int Send_Ica_MetaServerStop(VC_INFO vcInfo, char * sendingBuf);
int Send_Ica_CloseRequest(VC_INFO vcInfo, char * sendingBuf, ID id);
int Send_Ica_CloseAck(VC_INFO vcInfo, char * sendingBuf, ID id);
int Send_Ica_ServerRplPacket(  VC_INFO vcInfo, char * sendingBuf, ID id, VER ver, RPL rpl);
int Send_Ica_ReverseBindRplPacket(VC_INFO vcInfo, char * sendingBuf, ID revBindId , RPL rpl);
int Send_Ica_ReverseCloseRequest(VC_INFO vcInfo, char * sendingBuf, ID reverseBindId);
int Send_Ica_ReverseCloseAck(VC_INFO vcInfo, char * sendingBuf, ID reverseBindId);
int Send_Ica_ReverseConnectCmdPacket(VC_INFO vcInfo, char * sendingBuf, ID tmpId, ID revBindId);
int Send_Ica_DataPacket(VC_INFO vcInfo,   char * sendingBuf, ID id, LEN len, char * pData) ;
int Send_Ica_GenericPacket(VC_INFO vcInfo, char * sendingBuf, PACKET packet);
int Send_Ica_Retry(VC_INFO vcInfo, LEN len, char * pBuf);
int Send_Ica(VC_INFO vcInfo, LEN len, char * pBuf);
void FreeConnection(pPER_SOCKET_DATA pPerSocketData);
void FreeReverseBind(pPER_REVERSEBIND_DATA pPerReverseBindData);
pPER_SOCKET_DATA NewConnection(ID id, SOCKET socket);
pPER_REVERSEBIND_DATA NewReverseBind(ID id, SOCKET listeningSocket);
//int Send_Socket(SOCKET fd, LEN len, unsigned char * pBuf, int * pBytesSent);
SOCKET NewSocket(int family);

VOID AddConnection (pPER_SOCKET_DATA * ppConnectionList, pPER_SOCKET_DATA pPerSocketData) ;
VOID RemoveConnection(pPER_SOCKET_DATA * ppConnectionList, pPER_SOCKET_DATA pPerSocketData);
VOID AddReverseBind (pPER_REVERSEBIND_DATA * ppReverseBindList,pPER_REVERSEBIND_DATA pPerReverseBindData);
VOID RemoveReverseBind(pPER_REVERSEBIND_DATA * ppReverseBindList,pPER_REVERSEBIND_DATA pPerReverseBindData);

int UpdateIOCP(HANDLE completionPort, SOCKET socket, pPER_SOCKET_DATA pPerSocketData);
int UpdateIOCPReverseBind(HANDLE completionPort, SOCKET socket, pPER_REVERSEBIND_DATA pPerReverseBindData);
int Parse_Ica_ClientDomainCmd(IN char * pPacketBuf, IN LEN bufLen, OUT VER *pVer, OUT CMD *pCmd, OUT PORT *pPort, OUT DOMLEN * pDomLen, OUT DOMSTR * ppDomData, OUT CHAR * pDomStr );
int Parse_Ica_ClientIpCmd(IN char * pPacketBuf, IN LEN bufLen, OUT VER *pVer, OUT CMD *pCmd, OUT IPADDR *pIpAddr, OUT PORT *pPort);
int Parse_Ica_ClientIp6Cmd(IN char * pPacketBuf, IN LEN bufLen, OUT VER *pVer, OUT CMD *pCmd, OUT IP6ADDR *pIp6Addr, OUT PORT *pPort);
int Parse_Ica_ReverseBindCmd(IN char * pPacketBuf, IN LEN bufLen, OUT IPADDR *pRmtAddr, OUT PORT *pRmtPort);
int Parse_Ica_ReverseConnectRpl(IN char * pPacketBuf, IN LEN bufLen, OUT ID * pTmpId, OUT ID * pRevBindId, OUT RPL * pRpl);
LPFN_CONNECTEX GetConnectExPtr();
LPFN_ACCEPTEX GetAcceptExPtr();
int Parse_Ica_MetaWindow(IN char * pPacketBuf, IN LEN bufLen, OUT int * pWindow);
VOID CloseAllConnections(pPER_SOCKET_DATA pHeadList);
int StartClosing(pPER_SOCKET_DATA pPerSocketData, VC_INFO vcInfo, PCHAR pIcaSendBuf);
int CloseReverseBind(pPER_REVERSEBIND_DATA * ppReverseBindList, pPER_REVERSEBIND_DATA pPerReverseBindData);