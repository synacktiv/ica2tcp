//
// Created by hugoc on 09/06/2021.
//

#ifndef VDSOCKS_V2_CONNECTION_MANAGER_H
#define VDSOCKS_V2_CONNECTION_MANAGER_H

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include "socket_queue.h"
#include "debug_extern.h"
#include "errors.h"
#include "debug.h"
#include "socket_queue.h"

#define NULL ((void *)0)

//defining an ID type
#define ID_SIZE 4
#define ID_TYPE unsigned int
typedef ID_TYPE ID;


typedef enum E_CONNECTION_STATE{
    FREE,             /**< Connection is not used and not accounted for in S_CONNECTION_MANAGER::n_connection. fd, id, window should be
                       * -1, and queue NULL */
    SOCKET_OPEN,      /**< Fd is an opened socket, SOCKS negotiation in progress */
    METHOD_CHOSEN,    /**< Client socket sent a list of supported SOCKS authentication methods, we chose one and sent back
                       * a method selection message */
    CMD_SENT,         /**< Client socket sent its SOCKS request and we transferred it through virtual channel to the citrix server*/
    ESTABLISHED,      /**< Citrix server responded to the request and the SOCKS connection is established, ready to
                       * transfer data */
    CLOSING,          /**< A close request was sent to the citrix server but the connection is not closed until a close
                       * acknowledgment packet is received back from the server.*/
} CONNECTION_STATE;



typedef struct S_CONNECTION{
    ID id;                          /**< Unique identifier of the connection. Used server-side to demultiplex the
                                      * virtual channel flow and transfer to the right socket */
    CONNECTION_STATE state;         /**< Current state of the connection. */
    int socket_fd;                  /**< File descriptor representing the local socket linked with the connection*/
    pSOCKET_QUEUE pSocketQueue;     /**< Pointer to a socket queue structure. Used to queue outgoing packets for the
                                      * "socket_fd" socket.*/
    int window;                     /**< Currently remaining number of bytes that can be sent to the citrix server for
                                      * this connection. When those bytes are successfully transferred to the
                                      * server-side socket, an acknowledgment packet is sent back by the server, and
                                      * window is increased*/

} CONNECTION, * pCONNECTION;

typedef struct S_CONNECTION_MANAGER{
    int online;                    /**< TRUE if the server-side program is running.*/
    int n_connection;               /**< Number of currently used (non FREE) connections. */
    int max_connection;             /**< Current size of the connection_list array. */
    int max_id;                     /**< Current highest value among used connections and thus size of id_mapping array. */
    CONNECTION * connection_list;   /**< Array of CONNECTION's */
    int * id_mapping;               /**< Array used to map connection's id's to connection's indexes in connection_list.
                                      * Used to avoid looping over the entire connection_list when looking for a given
                                      * connection id.*/


}CONNECTION_MANAGER, * pCONNECTION_MANAGER;

int ConnectionManager_Init(pCONNECTION_MANAGER*);
void ConnectionManager_Close(pCONNECTION_MANAGER);
int ConnectionManager_Resize(pCONNECTION_MANAGER);
int ConnectionManager_NewConnection(pCONNECTION_MANAGER, int new_socket);
int ConnectionManager_CloseConnectionById(pCONNECTION_MANAGER pConnectionManager, ID id);
int ConnectionManager_CloseConnectionByIndex(pCONNECTION_MANAGER pConnectionManager, int index);
int ConnectionManager_GetConnectionNumber(pCONNECTION_MANAGER pConnectionManager);
pCONNECTION ConnectionManager_GetConnectionById(pCONNECTION_MANAGER pConnectionManager, ID id);
pCONNECTION ConnectionManager_GetConnectionByIndex(pCONNECTION_MANAGER pConnectionManager, int index);
int ConnectionManager_SetConnectionStateByIndex(pCONNECTION_MANAGER pConnectionManager, int index, CONNECTION_STATE state);
int ConnectionManager_SetConnectionStateById(pCONNECTION_MANAGER pConnectionManager, ID id, CONNECTION_STATE state);
int ConnectionManager_GetConnectionStateByIndex(pCONNECTION_MANAGER pConnectionManager, int index, CONNECTION_STATE * pState);
int ConnectionManager_GetConnectionStateById(pCONNECTION_MANAGER pConnectionManager, ID id, CONNECTION_STATE * pState);
int ConnectionManager_AddConnectionWindowByIndex(pCONNECTION_MANAGER pConnectionManager, int index, int to_add);
int ConnectionManager_AddConnectionWindowById(pCONNECTION_MANAGER pConnectionManager, ID id, int to_add);
int ConnectionManager_SubConnectionWindowById(pCONNECTION_MANAGER pConnectionManager, ID id,int to_sub);
int ConnectionManager_SubConnectionWindowByIndex(pCONNECTION_MANAGER pConnectionManager, int index, int to_sub);

#endif //VDSOCKS_V2_CONNECTION_MANAGER_H
