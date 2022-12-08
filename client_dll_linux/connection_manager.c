//
// Created by hugoc on 09/06/2021.
//

/**
 * Management of a dynamic list of CONNECTION structures. The top level structure is the CONNECTION_MANAGER, storing a
 * variable-length list of CONNECTION's, keeping track of the current number of used connections, the current maximum
 * size of the connection array, the mapping between the connection id's and their index in the array, as well as the
 * online or offline state of the server-side program.
 * Each CONNECTION structure represents the connection between one socket client-side and another one server-side : each
 * connection has a unique id that allows to multiplex several socket-to-socket connections in only one ICA virtual
 * channel. A CONNECTION is composed of an id, a file descriptor representing the local socket, a queue for sending
 * bytes to this socket, a current state and a window (that represents the number of bytes that can be sent to the server
 * on this connection before an acknowledgement packet is received from the server to increase the window of this
 * connection.).
 * There is no absolute maximum number of connections : the array is dynamically adapted by the connection manager at
 * runtime to fit the required number of simultaneous connections.
 */

#include "connection_manager.h"


/**
 *
 * @param ppConnectionManager
 * @return
 */
int ConnectionManager_Init(pCONNECTION_MANAGER * ppConnectionManager){
    DEBUG(pFile, "Entering\n");


    if( NULL == (*ppConnectionManager = (pCONNECTION_MANAGER)malloc(sizeof(CONNECTION_MANAGER)))){
        DEBUG(pErrorFile, "Memory allocation error : g_pConnectionManager\n");
        return(ERROR_MEMORY);

    }
    pCONNECTION_MANAGER pConnectionManager = *ppConnectionManager;
    pConnectionManager->max_connection = 2;
    pConnectionManager->max_id = 0;
    pConnectionManager->online = 0;



    DEBUG(pFile, "here")
    if( NULL == (pConnectionManager->connection_list = (CONNECTION *)calloc(pConnectionManager->max_connection, sizeof(CONNECTION)))){
        DEBUG(pErrorFile, "Memory allocation error : g_pConnectionManager->connection_list\n");
        return(ERROR_MEMORY);

    }


    if(NULL == (pConnectionManager->id_mapping = (int *)calloc(pConnectionManager->max_id, sizeof(int)))){
        DEBUG(pErrorFile, "Memory allocation error : g_pConnectionManager->id_mapping\n");
        return(ERROR_MEMORY);
    }

    for(int i=0; i<pConnectionManager->max_connection; i++){
        pConnectionManager->id_mapping[i] = -1;
        pConnectionManager->connection_list[i].id=-1;
        pConnectionManager->connection_list[i].socket_fd = -1;
        pConnectionManager->connection_list[i].state = FREE;
        pConnectionManager->connection_list[i].pSocketQueue = NULL;
        pConnectionManager->connection_list[i].window = -1;
        pConnectionManager->connection_list[i].tmpId = -1;
    }
    pConnectionManager->n_connection = 0;

    //DEBUG_INFO(pFile);

    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 *
 * @param pConnectionManager
 */
void ConnectionManager_Close(pCONNECTION_MANAGER pConnectionManager){
//    DEBUG(pFile, "Entering\n");
//    for(int i=0; i< pConnectionManager->n_connection; i++){
//        ConnectionManager_CloseConnectionByIndex(pConnectionManager, i);
//    }
    if(pConnectionManager->connection_list){
        free(pConnectionManager->connection_list);
        pConnectionManager->connection_list = NULL;
    }
    if(pConnectionManager->id_mapping){
        free(pConnectionManager->id_mapping);
        pConnectionManager->id_mapping = NULL;
    }

    free(pConnectionManager);
    pConnectionManager = NULL;
    DEBUG(pFile, "Leaving\n");
}

/**
 *
 * @param pConnectionManager
 * @return
 */
int ConnectionManager_Resize(pCONNECTION_MANAGER pConnectionManager){
    CONNECTION * connection_list_tmp = NULL;

    DEBUG(pFile, "Entering\n");


    if(pConnectionManager->n_connection >= pConnectionManager->max_connection - 1){
        DEBUG(pFile, "Increasing size\n");


        pConnectionManager->max_connection = 2*pConnectionManager->max_connection;

        if(NULL == (connection_list_tmp = (CONNECTION *) reallocarray(pConnectionManager->connection_list, pConnectionManager->max_connection, sizeof(CONNECTION)))) {
            DEBUG(pErrorFile, "Memory reallocation error : g_pConnectionManager->connection_list\n");
            return(ERROR_MEMORY);
        }
        pConnectionManager->connection_list = connection_list_tmp;


        for(int i= pConnectionManager->n_connection; i < pConnectionManager->max_connection; i++){
            pConnectionManager->connection_list[i].id=-1;
            pConnectionManager->connection_list[i].socket_fd = -1;
            pConnectionManager->connection_list[i].state = FREE;
            pConnectionManager->connection_list[i].pSocketQueue = NULL;
            pConnectionManager->connection_list[i].window = -1;
            pConnectionManager->connection_list[i].tmpId = -1;
        }



    }

    if((pConnectionManager->n_connection <= pConnectionManager->max_connection / 4) && (pConnectionManager->max_connection >= 20) ){
        DEBUG(pFile, "Decreasing size\n");

        pConnectionManager->max_connection = pConnectionManager->max_connection/2;

        if(NULL == (connection_list_tmp = (CONNECTION *)reallocarray(pConnectionManager->connection_list, pConnectionManager->max_connection, sizeof(CONNECTION)))) {
            DEBUG(pErrorFile, "Memory reallocation error : g_pConnectionManager->connection_list\n");
            return(ERROR_MEMORY);
        }
    }

    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 *
 * @param pConnectionManager
 * @param new_socket
 * @return
 */
int ConnectionManager_NewConnection(pCONNECTION_MANAGER pConnectionManager, int new_socket){
    DEBUG(pFile, "Entering\n");
    if(pConnectionManager->n_connection >= pConnectionManager->max_connection){
        DEBUG(pErrorFile, "Error, max number of connections  reached.\n");
        return (ERROR_OVERFLOW);
    }

    pCONNECTION available_row = &pConnectionManager->connection_list[pConnectionManager->n_connection];

    if(available_row->state!=FREE){
        DEBUG(pFile, "Error, connection is not FREE.\n");
        DEBUG(pErrorFile, "Error, connection is not FREE.\n");
        return(FAILURE);
    }
    if(new_socket <= pConnectionManager->max_id) {
        if (-1 != pConnectionManager->id_mapping[new_socket]) {
            DEBUG_VAR(pFile, "ID %d is already used and corresponds to index %d connection\n", new_socket,
                      pConnectionManager->id_mapping[new_socket])
            return (FAILURE);
        }
    }

    DEBUG(pFile, "Creating new con\n");

    available_row->state = SOCKET_OPEN;
    available_row->socket_fd = new_socket;
    available_row->id = new_socket;
    available_row->window = MAX_SOCKET_QUEUE_SIZE;
    available_row->tmpId = -1;

    if(NULL == (available_row->pSocketQueue = (pSOCKET_QUEUE )malloc(sizeof(SOCKET_QUEUE)))){
        DEBUG(pErrorFile, "Memory allocation error : socket_queue\n");
        return(ERROR_MEMORY);
    }
    if(SUCCESS != SocketQueue_Init(available_row->pSocketQueue)){
        DEBUG(pErrorFile, "Error initializing queue structure.\n");
        return(FAILURE);
    }





    if(new_socket > pConnectionManager->max_id){
        int tmp = pConnectionManager->max_id;
        pConnectionManager->max_id  = new_socket;
        int * idMap_tmp = NULL;

        if(NULL == (idMap_tmp = (int *)reallocarray(pConnectionManager->id_mapping, pConnectionManager->max_id + 1, sizeof(int)))) {
            DEBUG(pErrorFile, "Memory allocation error : g_pConnectionManager->g_pConnectionManager->id_mapping\n");
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
    DEBUG(pFile, "Leaving\n");
    return (SUCCESS);
}

/**
 *
 * @param pConnectionManager
 * @param id
 * @return
 */
int ConnectionManager_CloseConnectionById(pCONNECTION_MANAGER pConnectionManager, ID id){
    if(id < 0 || id > pConnectionManager->max_id){
        DEBUG_VAR(pFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        DEBUG_VAR(pErrorFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        return(ERROR_INDEX_RANGE);
    }
    int index = pConnectionManager->id_mapping[id];
    DEBUG_VAR(pFile, "Id %d corresponds to index %d\n", id, index);
//    free(ConnectionManager_GetConnectionById(pConnectionManager, id)->pSocketQueue->buf);
//    ConnectionManager_GetConnectionById(pConnectionManager, id)->pSocketQueue->buf=NULL;
//    DEBUG(pFile, "ok1\n");

    return(ConnectionManager_CloseConnectionByIndex(pConnectionManager, index));
}

/**
 *
 * @param pConnectionManager
 * @param index
 * @return
 */
int ConnectionManager_CloseConnectionByIndex(pCONNECTION_MANAGER pConnectionManager, int index){
    DEBUG(pFile, "Entering\n");
    if(index > pConnectionManager->max_connection-1){
        DEBUG(pErrorFile, "Index > max_connection")
        return (ERROR_INDEX_RANGE);
    }

    if(index < 0 || index >= pConnectionManager->n_connection){
        DEBUG_VAR(pFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        DEBUG_VAR(pErrorFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        return(ERROR_INDEX_RANGE);
    }

    pCONNECTION pClosing_connection = &pConnectionManager->connection_list[index];
    pCONNECTION pLast_connection = &pConnectionManager->connection_list[pConnectionManager->n_connection-1];
    ID closingId = pClosing_connection->id;


    if (pClosing_connection->socket_fd != -1) {
        close(pClosing_connection->socket_fd);
        DEBUG_VAR(pFile, "Socket %d closed !\n", pClosing_connection->socket_fd);
    }

    if(pClosing_connection->pSocketQueue != NULL){
        SocketQueue_Free(pClosing_connection->pSocketQueue);
        free(pClosing_connection->pSocketQueue);
        pClosing_connection->pSocketQueue = NULL;
    }

    DEBUG(pFile, "1\n");
    pConnectionManager->id_mapping[closingId] = -1;
    DEBUG(pFile, "2\n");
    pClosing_connection->state = pLast_connection->state;
    pClosing_connection->socket_fd = pLast_connection->socket_fd;
    pClosing_connection->id = pLast_connection->id;
    pClosing_connection->pSocketQueue = pLast_connection->pSocketQueue;
    pClosing_connection->window = pLast_connection->window;
    pClosing_connection->tmpId = pLast_connection->tmpId;
    DEBUG(pFile, "3\n");
    pConnectionManager->id_mapping[pLast_connection->id] = index;
    DEBUG(pFile, "4\n");
    pLast_connection->state = FREE;
    pLast_connection->socket_fd = -1;
    pLast_connection->id= -1;
    pLast_connection->pSocketQueue=NULL;
    pLast_connection->window = -1;
    pLast_connection->tmpId = -1;
    DEBUG(pFile, "5\n");

    pConnectionManager->id_mapping[closingId] = -1;
    pConnectionManager->n_connection -= 1;
    DEBUG(pFile, "here");
    if(closingId == pConnectionManager->max_id){
        int i = pConnectionManager->max_id;
        while(i>=0 && pConnectionManager->id_mapping[i] == -1){
            i--;
            pConnectionManager->max_id = i;

        }
        if(pConnectionManager->max_id<0){pConnectionManager->max_id=0;}

        int * idMap_tmp = NULL;
        if(NULL == (idMap_tmp = (int *)reallocarray(pConnectionManager->id_mapping, pConnectionManager->max_id + 1, sizeof(int)))) {
            DEBUG(pErrorFile, "Error reallocating mapping structure.\n");
            return(ERROR_MEMORY);
        }
        pConnectionManager->id_mapping = idMap_tmp;

    }

    int rc = 0;
    if(SUCCESS != (rc=ConnectionManager_Resize(pConnectionManager))){
        DEBUG(pErrorFile, "Error resizing connection list\n")
        return(rc);
    }
    DEBUG(pFile, "here");
    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 *
 * @param pConnectionManager
 * @return
 */
int ConnectionManager_GetConnectionNumber(pCONNECTION_MANAGER pConnectionManager){
    DEBUG_VAR(pFile, "co number : %d\n", pConnectionManager->n_connection);
    return pConnectionManager->n_connection;
}

/**
 *
 * @param pConnectionManager
 * @param index
 * @return
 */
pCONNECTION ConnectionManager_GetConnectionByIndex(pCONNECTION_MANAGER pConnectionManager, int index){
    if(index < 0 || index >= pConnectionManager->n_connection){
        DEBUG_VAR(pFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        DEBUG_VAR(pErrorFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        return(NULL);
    }

    return &pConnectionManager->connection_list[index];
}

/**
 *
 * @param pConnectionManager
 * @param id
 * @return
 */
pCONNECTION ConnectionManager_GetConnectionById(pCONNECTION_MANAGER pConnectionManager, ID id){
    if(id < 0 || id > pConnectionManager->max_id){
        DEBUG_VAR(pFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        DEBUG_VAR(pErrorFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        return(NULL);
    }
    return ConnectionManager_GetConnectionByIndex(pConnectionManager, pConnectionManager->id_mapping[id]);
}

/**
 *
 * @param pConnectionManager
 * @param index
 * @param state
 * @return
 */
int ConnectionManager_SetConnectionStateByIndex(pCONNECTION_MANAGER pConnectionManager, int index, CONNECTION_STATE state){
    if(index < 0 || index >= pConnectionManager->n_connection){
        DEBUG_VAR(pFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        DEBUG_VAR(pErrorFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        return(ERROR_INDEX_RANGE);
    }

    pConnectionManager->connection_list[index].state= state;
    return(SUCCESS);
}

/**
 *
 * @param pConnectionManager
 * @param id
 * @param state
 * @return
 */
int ConnectionManager_SetConnectionStateById(pCONNECTION_MANAGER pConnectionManager, ID id, CONNECTION_STATE state){
    if(id < 0 || id > pConnectionManager->max_id){
        DEBUG_VAR(pFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        DEBUG_VAR(pErrorFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        return(ERROR_INDEX_RANGE);
    }

    return ( ConnectionManager_SetConnectionStateByIndex(pConnectionManager, pConnectionManager->id_mapping[id], state));
}

/**
 *
 * @param pConnectionManager
 * @param index
 * @param pState
 * @return
 */
int ConnectionManager_GetConnectionStateByIndex(pCONNECTION_MANAGER pConnectionManager, int index, CONNECTION_STATE * pState){
    if(index < 0 || index >= pConnectionManager->n_connection){
        DEBUG_VAR(pFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        DEBUG_VAR(pErrorFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        return(ERROR_INDEX_RANGE);
    }

    if(!pState){
        DEBUG(pFile, "ERROR : pState is a null pointer\n");
        return(ERROR_NULL_POINTER);
    }
    *pState = pConnectionManager->connection_list[index].state;
    return(SUCCESS);
}

/**
 *
 * @param pConnectionManager
 * @param id
 * @param pState
 * @return
 */
int ConnectionManager_GetConnectionStateById(pCONNECTION_MANAGER pConnectionManager, ID id, CONNECTION_STATE * pState){
    if(id < 0 || id > pConnectionManager->max_id){
        DEBUG_VAR(pFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        DEBUG_VAR(pErrorFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        return(ERROR_INDEX_RANGE);
    }

    return (ConnectionManager_GetConnectionStateByIndex(pConnectionManager, pConnectionManager->id_mapping[id], pState));

}

/**
 *
 * @param pConnectionManager
 * @param index
 * @param to_add
 * @return
 */
int ConnectionManager_AddConnectionWindowByIndex(pCONNECTION_MANAGER pConnectionManager, int index, int to_add){
    if(index < 0 || index >= pConnectionManager->n_connection){
        DEBUG_VAR(pFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        DEBUG_VAR(pErrorFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        return(ERROR_INDEX_RANGE);
    }

    pConnectionManager->connection_list[index].window += to_add;
    return(SUCCESS);
}

/**
 *
 * @param pConnectionManager
 * @param id
 * @param to_add
 * @return
 */
int ConnectionManager_AddConnectionWindowById(pCONNECTION_MANAGER pConnectionManager, ID id,int to_add){
    if(id < 0 || id > pConnectionManager->max_id){
        DEBUG_VAR(pFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        DEBUG_VAR(pErrorFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        return(ERROR_INDEX_RANGE);
    }


    return (ConnectionManager_AddConnectionWindowByIndex(pConnectionManager, pConnectionManager->id_mapping[id], to_add));
}

/**
 *
 * @param pConnectionManager
 * @param index
 * @param to_sub
 * @return
 */
int ConnectionManager_SubConnectionWindowByIndex(pCONNECTION_MANAGER pConnectionManager, int index, int to_sub){
    if(index < 0 || index >= pConnectionManager->n_connection){
        DEBUG_VAR(pFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        DEBUG_VAR(pErrorFile, "index : %d out of range (n_connection = %d)\n", index, pConnectionManager->n_connection);
        return(ERROR_INDEX_RANGE);
    }

    pConnectionManager->connection_list[index].window -= to_sub;
    return(SUCCESS);
}

/**
 *
 * @param pConnectionManager
 * @param id
 * @param to_sub
 * @return
 */
int ConnectionManager_SubConnectionWindowById(pCONNECTION_MANAGER pConnectionManager, ID id,int to_sub){
    if(id < 0 || id > pConnectionManager->max_id){
        DEBUG_VAR(pFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        DEBUG_VAR(pErrorFile, "id : %d out of range (max_id = %d)\n", id, pConnectionManager->max_id);
        return(ERROR_INDEX_RANGE);
    }

    return (ConnectionManager_SubConnectionWindowByIndex(pConnectionManager, pConnectionManager->id_mapping[id], to_sub));
}