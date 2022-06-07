//
// Created by hugoc on 09/06/2021.
//

#include "connection_manager_win.h"


int ConnectionManager_Init(pCONNECTION_MANAGER * ppConnectionManager){
    DEBUG("Entering\n");


    if( NULL == (*ppConnectionManager = (pCONNECTION_MANAGER)malloc(sizeof(CONNECTION_MANAGER)))){
        DEBUG( "Memory allocation error : g_pConnectionManager\n");
        return(ERROR_MEMORY);

    }
    pCONNECTION_MANAGER pConnectionManager = *ppConnectionManager;
    pConnectionManager->max_connection = MAX_CONNECTIONS;
    pConnectionManager->max_id = 0;

    DEBUG( "here")
    if( NULL == (pConnectionManager->connection_list = (CONNECTION *)calloc(pConnectionManager->max_connection, sizeof(CONNECTION)))){
        DEBUG( "Memory allocation error : g_pConnectionManager->connection_list\n");
        return(ERROR_MEMORY);

    }


    if(NULL == (pConnectionManager->id_mapping = (int *)calloc(pConnectionManager->max_id, sizeof(int)))){
        DEBUG( "Memory allocation error : g_pConnectionManager->id_mapping\n");
        return(ERROR_MEMORY);
    }

	if (NULL == (pConnectionManager->fds = (WSAPOLLFD *)calloc(pConnectionManager->max_connection, sizeof(WSAPOLLFD)))) {
		DEBUG("Memory allocation error : pConnectionManager->fds\n")
			return(ERROR_MEMORY);
	}

    for(int i=0; i<pConnectionManager->max_connection; i++){
       
        pConnectionManager->connection_list[i].id=-1;
        pConnectionManager->connection_list[i].socket_fd = -1;
        pConnectionManager->connection_list[i].state = FREE;
        pConnectionManager->connection_list[i].pSocketQueue = NULL;
		pConnectionManager->connection_list[i].window = -1;
		//pConnectionManager->connection_list[i].server = //TODO : a quoi initialise-t-on ?
		pConnectionManager->fds[i].fd = -1;
		pConnectionManager->fds[i].events = 0;
		pConnectionManager->fds[i].revents = 0;
    }

	for (int i = 0; i <= pConnectionManager->max_id; i++) {
		pConnectionManager->id_mapping[i] = -1;
	}

    pConnectionManager->n_connection = 0;
	pConnectionManager->n_closing_connection = 0;

    //DEBUG_INFO()

    DEBUG( "Leaving\n");
    return(SUCCESS);
}

void ConnectionManager_Close(pCONNECTION_MANAGER pConnectionManager) {
	DEBUG("Entering.\n");
	for (int i = 0; i < pConnectionManager->n_connection; i++) {
		ConnectionManager_CloseConnectionByIndex(pConnectionManager, i);
	}

	if (pConnectionManager->connection_list) {
		free(pConnectionManager->connection_list);
		pConnectionManager->connection_list = NULL;
	}

	if (pConnectionManager->id_mapping) {
		free(pConnectionManager->id_mapping);
		pConnectionManager->id_mapping = NULL;
	}

	if (pConnectionManager->fds) {
		free(pConnectionManager->fds);
		pConnectionManager->fds = NULL;
	}

	free(pConnectionManager);
	
	DEBUG("Leaving.\n");
}

int ConnectionManager_Resize(pCONNECTION_MANAGER pConnectionManager){
    CONNECTION * connection_list_tmp = NULL;
	WSAPOLLFD * fds_tmp = NULL;

    DEBUG( "Entering\n");


    if(pConnectionManager->n_connection >= pConnectionManager->max_connection - 1){
        DEBUG( "Increasing size\n");


        pConnectionManager->max_connection = 2*pConnectionManager->max_connection;

        if(NULL == (connection_list_tmp = (CONNECTION *) realloc(pConnectionManager->connection_list, pConnectionManager->max_connection*sizeof(CONNECTION)))) {
            DEBUG( "Memory reallocation error : g_pConnectionManager->connection_list\n");
            return(ERROR_MEMORY);
        }
        pConnectionManager->connection_list = connection_list_tmp;

		if (NULL == (fds_tmp = (WSAPOLLFD *)realloc(pConnectionManager->fds, pConnectionManager->max_connection * sizeof(WSAPOLLFD)))) {
			DEBUG("Memory reallocation error : pConnectionManager->fds\n");
			return(ERROR_MEMORY);
		}
		pConnectionManager->fds = fds_tmp;


        for(int i= pConnectionManager->n_connection; i < pConnectionManager->max_connection; i++){
            pConnectionManager->connection_list[i].id=-1;
            pConnectionManager->connection_list[i].socket_fd = -1;
            pConnectionManager->connection_list[i].state = FREE;
            pConnectionManager->connection_list[i].pSocketQueue = NULL;
			pConnectionManager->connection_list[i].window = -1;
			//pConnectionManager->connection_list[i].server = //TODO a quoi initialise-t-on ?
			pConnectionManager->fds[i].fd = -1;
			pConnectionManager->fds[i].events = 0;
			pConnectionManager->fds[i].revents = 0;
        }



    }

    if((pConnectionManager->n_connection <= pConnectionManager->max_connection / 4) && (pConnectionManager->max_connection >= 20) ){
        DEBUG( "Decreasing size\n");

        pConnectionManager->max_connection = pConnectionManager->max_connection/2;

        if(NULL == (connection_list_tmp = (CONNECTION *)realloc(pConnectionManager->connection_list, pConnectionManager->max_connection*sizeof(CONNECTION)))) {
            DEBUG( "Memory reallocation error : g_pConnectionManager->connection_list\n");
            return(ERROR_MEMORY);
        }
		pConnectionManager->connection_list = connection_list_tmp;

		if (NULL == (fds_tmp = (WSAPOLLFD *)realloc(pConnectionManager->fds, pConnectionManager->max_connection * sizeof(WSAPOLLFD)))) {
			DEBUG("Memory reallocation error : pConnectionManager->fds\n")
			return(ERROR_MEMORY);
		}
		pConnectionManager->fds = fds_tmp;
    }

    DEBUG( "Leaving\n");
    return(SUCCESS);
}

int ConnectionManager_NewConnection(pCONNECTION_MANAGER pConnectionManager, ID id){
    DEBUG( "Entering\n");
    if(pConnectionManager->n_connection >= pConnectionManager->max_connection){
        DEBUG( "Error, max number of connections  reached.\n");
        return (ERROR_OVERFLOW);
    }

    pCONNECTION available_row = &pConnectionManager->connection_list[pConnectionManager->n_connection];

    if(available_row->state!=FREE){
        DEBUG( "Error, connection is not FREE.\n");
        return(FAILURE);
    }

	if (id <= pConnectionManager->max_id) {
		if (-1 != pConnectionManager->id_mapping[id]) {
			DEBUG_VAR("ID %d is already used and corresponds to index %d connection.\n", id, pConnectionManager->id_mapping[id]);
			return(FAILURE);
		}
	}
    DEBUG( "Creating new con\n");

    available_row->state = CMD_RECEIVED;
    available_row->id = id;
	available_row->window = MAX_SOCKET_QUEUE_SIZE;


    if(NULL == (available_row->pSocketQueue = (pSOCKET_QUEUE )malloc(sizeof(SOCKET_QUEUE)))){
        DEBUG( "Memory allocation error : socket_queue\n");
        return(ERROR_MEMORY);
    }
    if(SUCCESS != SocketQueue_Init(available_row->pSocketQueue)){
        DEBUG( "Error initializing queue structure.\n");
        return(FAILURE);
    }
	



    if(id > pConnectionManager->max_id){
        int tmp = pConnectionManager->max_id;
        pConnectionManager->max_id  = id;
        int * idMap_tmp = NULL;

        if(NULL == (idMap_tmp = (int *)realloc(pConnectionManager->id_mapping, (pConnectionManager->max_id + 1)* sizeof(int)))) {
            DEBUG( "Memory allocation error : pConnectionManager->id_mapping\n");
            return(ERROR_MEMORY);
        }
        pConnectionManager->id_mapping = idMap_tmp;

        for(int i = tmp+1; i<pConnectionManager->max_id+1; i++){
            pConnectionManager->id_mapping[i] = -1;
        }

    }

    pConnectionManager->id_mapping[available_row->id] = pConnectionManager->n_connection;

    pConnectionManager->n_connection += 1;

    ConnectionManager_Resize(pConnectionManager);
    //DEBUG_INFO(pFile);
    DEBUG( "Leaving\n");
    return (0);
}

int ConnectionManager_CloseConnectionById(pCONNECTION_MANAGER pConnectionManager, ID id){
	
    if(id > pConnectionManager->max_id){
        DEBUG( "Error : index > max_id\n")
        return(FAILURE);
    }
	DEBUG("here");
    int index = pConnectionManager->id_mapping[id];
    if(index < 0){
        DEBUG_VAR( "Error : index=%d, connection already closed\n", index);
        return (FAILURE);
    }
	DEBUG("there");
	DEBUG("Leaving\n")
    return(ConnectionManager_CloseConnectionByIndex(pConnectionManager, index));
}

int ConnectionManager_CloseConnectionByIndex(pCONNECTION_MANAGER pConnectionManager, int index){
	DEBUG("Entering\n")
		debug(pConnectionManager);
	DEBUG_VAR("index : %d\n", index)
    if(index > pConnectionManager->max_connection-1){
        DEBUG( "Index > max_connection")
        return (FAILURE);
    }

    pCONNECTION pClosing_connection = &pConnectionManager->connection_list[index];
    pCONNECTION pLast_connection = &pConnectionManager->connection_list[pConnectionManager->n_connection-1];
	WSAPOLLFD * pClosing_fds = &pConnectionManager->fds[index];
	WSAPOLLFD * pLast_fds = &pConnectionManager->fds[pConnectionManager->n_connection - 1];
    ID closingId = pClosing_connection->id;
	DEBUG("1\n")
	
	if (pClosing_connection->socket_fd != INVALID_SOCKET) {
		DEBUG("1.25\n")
		closesocket(pClosing_connection->socket_fd);
		DEBUG_VAR("Socket %d closed !\n", pClosing_connection->socket_fd);
	}
 
	DEBUG("1.5\n")

	if (pClosing_connection->pSocketQueue) {
		SocketQueue_Free(pClosing_connection->pSocketQueue);
		DEBUG("1.75\n")
		free(pClosing_connection->pSocketQueue);
		pClosing_connection->pSocketQueue = NULL;
	}

	if (pClosing_connection->state == CLOSING) {
		pConnectionManager->n_closing_connection -= 1;
	}

	DEBUG("2\n")
    pConnectionManager->id_mapping[closingId] = -1;

    pClosing_connection->state = pLast_connection->state;
    pClosing_connection->socket_fd = pLast_connection->socket_fd;
    pClosing_connection->id = pLast_connection->id;
    pClosing_connection->pSocketQueue = pLast_connection->pSocketQueue;
	pClosing_connection->server = pLast_connection->server;
	pClosing_connection->window = pLast_connection->window;

	pClosing_fds->events = pLast_fds->events;
	pClosing_fds->fd = pLast_fds->fd;
	pClosing_fds->revents = pLast_fds->revents;


    pConnectionManager->id_mapping[pLast_connection->id] = index;
	DEBUG("tewts\n")
    pLast_connection->state = FREE;
    pLast_connection->socket_fd = INVALID_SOCKET;
    pLast_connection->id= -1;
    pLast_connection->pSocketQueue=NULL;
	pLast_connection->window = -1;
	//pLast_connection->server = //TODO a quoi initialise-t-on ?

	pLast_fds->events = 0;
	pLast_fds->revents = 0;
	pLast_fds->fd = INVALID_SOCKET;

    pConnectionManager->id_mapping[closingId] = -1;
    pConnectionManager->n_connection -= 1;

    if(closingId == pConnectionManager->max_id){
        int i = pConnectionManager->max_id;
        while(i>=0 && pConnectionManager->id_mapping[i] < 0){
            i--;
            pConnectionManager->max_id = i;

        }
        if(pConnectionManager->max_id<0){pConnectionManager->max_id=0;}

        int * idMap_tmp = NULL;
        if(NULL == (idMap_tmp = (int *)realloc(pConnectionManager->id_mapping, (pConnectionManager->max_id + 1)* sizeof(int)))) {
            DEBUG( "Error reallocating mapping structure.\n");
            return(ERROR_MEMORY);
        }
        pConnectionManager->id_mapping = idMap_tmp;

    }

    int rc = 0;
    if(SUCCESS != (rc=ConnectionManager_Resize(pConnectionManager))){
        DEBUG( "Error resizing connection list\n")
        return(rc);
    }
    DEBUG( "here");
    //DEBUG_INFO(pFile);
    DEBUG( "Leaving\n");
    return(SUCCESS);
}

int ConnectionManager_StartClosingByIndex(pCONNECTION_MANAGER pConnectionManager, int index) {
	if (index < 0 || index >= pConnectionManager->n_connection) {
		DEBUG_VAR("index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
		return(ERROR_INDEX_RANGE);
	}

	if (pConnectionManager->connection_list[index].socket_fd != INVALID_SOCKET) {
		closesocket(pConnectionManager->connection_list[index].socket_fd);
		pConnectionManager->connection_list[index].socket_fd = INVALID_SOCKET;
	}

	pConnectionManager->fds[index].events = 0;
	pConnectionManager->fds[index].revents = 0;

	pConnectionManager->connection_list[index].state = CLOSING;
	pConnectionManager->n_closing_connection += 1;
	
	return(SUCCESS);
}

int ConnectionManager_StartClosingById(pCONNECTION_MANAGER pConnectionManager, ID id) {
	if (id<0 || id>pConnectionManager->max_id) {
		DEBUG_VAR("id %d out of range (max_id = %d)\n ", id, pConnectionManager->max_id);
		return(ERROR_INDEX_RANGE);
	}

	return(ConnectionManager_StartClosingByIndex(pConnectionManager, pConnectionManager->id_mapping[id]));
}

int ConnectionManager_GetConnectionNumber(pCONNECTION_MANAGER pConnectionManager){
	//DEBUG("Entering-Leaving\n")
    return pConnectionManager->n_connection;
}

pCONNECTION ConnectionManager_GetConnectionByIndex(pCONNECTION_MANAGER pConnectionManager, int index){
	//DEBUG("Entering-Leaving\n")
	if (index < 0) {
		DEBUG("Index < 0 !\n")
			return(NULL);
	}
	if (index >= pConnectionManager->max_connection) {
		DEBUG("Index >= max_connection\n")
			return(NULL);
	}
    return &pConnectionManager->connection_list[index];
}

pCONNECTION ConnectionManager_GetConnectionById(pCONNECTION_MANAGER pConnectionManager, ID id){
	//DEBUG("Entering-Leaving\n")
	if (id > pConnectionManager->max_id) {
		DEBUG("Id > max_id\n");
		return(NULL);
	}
	int index = pConnectionManager->id_mapping[id];
	return ConnectionManager_GetConnectionByIndex(pConnectionManager, index);
}

WSAPOLLFD * ConnectionManager_GetPollFdByIndex(pCONNECTION_MANAGER pConnectionManager, int index) {
	//DEBUG("Entering-Leaving\n")
	if (index < 0) {
		DEBUG("Index < 0 !\n")
			return(NULL);
	}
	if (index >= pConnectionManager->max_connection) {
		DEBUG("Index >= max_connection\n")
			return(NULL);
	}
	return &pConnectionManager->fds[index];
}

WSAPOLLFD * ConnectionManager_GetPollFdById(pCONNECTION_MANAGER pConnectionManager, ID id) {
	//DEBUG("Entering-Leaving\n")
	if (id > pConnectionManager->max_id) {
		DEBUG("Id > max_id\n");
		return(ERROR_INDEX_RANGE);
	}
	int index = pConnectionManager->id_mapping[id];
	return ConnectionManager_GetPollFdByIndex(pConnectionManager, index);
}

int ConnectionManager_SetConnectionStateByIndex(pCONNECTION_MANAGER pConnectionManager, int index, CONNECTION_STATE state){
	DEBUG("Entering\n")
		if (index < 0) {
			DEBUG("Index < 0 !\n")
				return(ERROR_INDEX_RANGE);
		}
	if (index >= pConnectionManager->max_connection) {
		DEBUG("Index >= max_connection\n")
			return(ERROR_INDEX_RANGE);
	}
    pConnectionManager->connection_list[index].state= state;
	DEBUG("Leaving\n")
    return(SUCCESS);
}

int ConnectionManager_SetConnectionStateById(pCONNECTION_MANAGER pConnectionManager, ID id, CONNECTION_STATE state){
	DEBUG("Entering\n");
	if (id > pConnectionManager->max_id) {
		DEBUG("Id > max_id\n");
		return(ERROR_INDEX_RANGE);
	}
	int index = pConnectionManager->id_mapping[id];

	

	DEBUG("Leaving\n")
	return (ConnectionManager_SetConnectionStateByIndex(pConnectionManager, index, state));
}

int ConnectionManager_GetConnectionStateByIndex(pCONNECTION_MANAGER pConnectionManager, int index, CONNECTION_STATE * pState) {
	if (index < 0 || index >= pConnectionManager->n_connection) {
		DEBUG_VAR("index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
		return(ERROR_INDEX_RANGE);
	}

	if (!pState) {
		DEBUG("pState is a null pointer\n");
		return(ERROR_NULL_POINTER);
	}

	*pState = pConnectionManager->connection_list[index].state;
	return(SUCCESS);
}

int ConnectionManager_GetConnectionStateById(pCONNECTION_MANAGER pConnectionManager, ID id, CONNECTION_STATE * pState) {
	if (id<0 || id>pConnectionManager->max_id) {
		DEBUG_VAR("id %d out of range (max_id = %d)\n ", id, pConnectionManager->max_id);
		return(ERROR_INDEX_RANGE);
	}

	return(ConnectionManager_GetConnectionStateByIndex(pConnectionManager, pConnectionManager->id_mapping[id], pState));
}

int ConnectionManager_AddConnectionWindowByIndex(pCONNECTION_MANAGER pConnectionManager, int index, int to_add) {
	DEBUG("Entering\n");
	if (index < 0) {
		DEBUG("Index < 0 !\n")
		return(ERROR_INDEX_RANGE);
	}
	if (index >= pConnectionManager->max_connection) {
		DEBUG("Index >= max_connection\n");
		return(ERROR_INDEX_RANGE);
	}
	pConnectionManager->connection_list[index].window += to_add;
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int ConnectionManager_AddConnectionWindowById(pCONNECTION_MANAGER pConnectionManager, ID id, int to_add) {
	DEBUG("Entering\n");
	if (id > pConnectionManager->max_id) {
		DEBUG("Id > max_id\n");
		return(ERROR_INDEX_RANGE);
	}
	int index = pConnectionManager->id_mapping[id];



	DEBUG("Leaving\n")
		return (ConnectionManager_AddConnectionWindowByIndex(pConnectionManager, index, to_add));
}

int ConnectionManager_SubConnectionWindowByIndex(pCONNECTION_MANAGER pConnectionManager, int index, int to_sub) {
	DEBUG("Entering\n");
	if (index < 0) {
		DEBUG("Index < 0 !\n")
			return(ERROR_INDEX_RANGE);
	}
	if (index >= pConnectionManager->max_connection) {
		DEBUG("Index >= max_connection\n");
		return(ERROR_INDEX_RANGE);
	}
	pConnectionManager->connection_list[index].window -= to_sub;
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int ConnectionManager_SubConnectionWindowById(pCONNECTION_MANAGER pConnectionManager, ID id, int to_sub) {
	DEBUG("Entering\n");
	if (id > pConnectionManager->max_id) {
		DEBUG("Id > max_id\n");
		return(ERROR_INDEX_RANGE);
	}
	int index = pConnectionManager->id_mapping[id];



	DEBUG("Leaving\n")
		return (ConnectionManager_SubConnectionWindowByIndex(pConnectionManager, index, to_sub));
}

int ConnectionManager_GetConnectionWindowByIndex(pCONNECTION_MANAGER pConnectionManager, int index) {
	DEBUG("Entering\n");
	if (index < 0) {
		DEBUG("Index < 0 !\n")
			return(ERROR_INDEX_RANGE);
	}
	if (index >= pConnectionManager->max_connection) {
		DEBUG("Index >= max_connection\n");
		return(ERROR_INDEX_RANGE);
	}

	DEBUG("Leaving\n");
	return(pConnectionManager->connection_list[index].window);
}

int ConnectionManager_GetConnectionWindowById(pCONNECTION_MANAGER pConnectionManager, ID id) {
	DEBUG("Entering\n");
	if (id > pConnectionManager->max_id) {
		DEBUG("Id > max_id\n");
		return(ERROR_INDEX_RANGE);
	}
	int index = pConnectionManager->id_mapping[id];



	DEBUG("Leaving\n")
		return (ConnectionManager_GetConnectionWindowByIndex(pConnectionManager, index));
}