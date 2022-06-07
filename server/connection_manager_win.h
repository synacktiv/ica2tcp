//
// Created by hugoc on 09/06/2021.
//

#ifndef CONNECTION_MANAGER_WIN_H
#define CONNECTION_MANAGER_WIN_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <malloc.h>

#include "socket_queue_win.h"
#include "errors_win.h"
#include "debug_win.h"


#define MAX_CONNECTIONS 2
#define ID_SIZE 4
#define ID_TYPE unsigned int

typedef ID_TYPE ID;

typedef enum E_CONNECTION_STATE {
	FREE,
	CMD_RECEIVED,
	RESOLVING,
	TO_CLOSE,
	RESOLVED,
	UNRESOLVED,
	CONNECTING,
	ESTABLISHED,
	CLOSING,
} CONNECTION_STATE;

typedef struct S_CONNECTION {
	ID id;
	CONNECTION_STATE state;
	int socket_fd;
	pSOCKET_QUEUE pSocketQueue;
	struct sockaddr_in server;
	int window;

} CONNECTION, *pCONNECTION;

typedef struct S_CONNECTION_MANAGER {
	int n_connection;
	int n_closing_connection;
	int max_connection;
	int max_id;
	CONNECTION * connection_list;
	int * id_mapping;
	WSAPOLLFD * fds;
}CONNECTION_MANAGER, *pCONNECTION_MANAGER;

int ConnectionManager_Init(pCONNECTION_MANAGER*);
void ConnectionManager_Close(pCONNECTION_MANAGER);
int ConnectionManager_Resize(pCONNECTION_MANAGER);
int ConnectionManager_NewConnection(pCONNECTION_MANAGER, ID);
int ConnectionManager_CloseConnectionById(pCONNECTION_MANAGER, ID);
int ConnectionManager_CloseConnectionByIndex(pCONNECTION_MANAGER, int);
int ConnectionManager_StartClosingByIndex(pCONNECTION_MANAGER, int);
int ConnectionManager_StartClosingById(pCONNECTION_MANAGER, ID);
int ConnectionManager_GetConnectionNumber(pCONNECTION_MANAGER);
pCONNECTION ConnectionManager_GetConnectionByIndex(pCONNECTION_MANAGER, int);
pCONNECTION ConnectionManager_GetConnectionById(pCONNECTION_MANAGER, ID);
WSAPOLLFD * ConnectionManager_GetPollFdByIndex(pCONNECTION_MANAGER, int);
WSAPOLLFD * ConnectionManager_GetPollFdById(pCONNECTION_MANAGER, ID);
int ConnectionManager_SetConnectionStateByIndex(pCONNECTION_MANAGER, int, CONNECTION_STATE);
int ConnectionManager_SetConnectionStateById(pCONNECTION_MANAGER, ID, CONNECTION_STATE);
int ConnectionManager_GetConnectionStateByIndex(pCONNECTION_MANAGER, int, CONNECTION_STATE *);
int ConnectionManager_GetConnectionStateById(pCONNECTION_MANAGER, ID, CONNECTION_STATE *);
int ConnectionManager_AddConnectionWindowByIndex(pCONNECTION_MANAGER, int, int);
int ConnectionManager_AddConnectionWindowById(pCONNECTION_MANAGER, ID, int);
int ConnectionManager_SubConnectionWindowByIndex(pCONNECTION_MANAGER, int, int);
int ConnectionManager_SubConnectionWindowById(pCONNECTION_MANAGER, ID, int);
int ConnectionManager_GetConnectionWindowByIndex(pCONNECTION_MANAGER, int);
int ConnectionManager_GetConnectionWindowById(pCONNECTION_MANAGER, ID);

#endif //CONNECTION_MANAGER_WIN_H
