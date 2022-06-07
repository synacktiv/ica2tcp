//
// Created by hugoc on 28/04/2021.
//

#include "vdsocks_v2.h"





pVD_CALLBACK_ARGS g_pVdCallbackArgs = NULL; //for debug only

// *********************** Virtual Driver Functions *******************************************************************
// **************************************************************************************
// These functions must be defined in any virtual channel driver, and are documented in
// the Citrix VCSDK documentation. They are called back by the Winstation driver.
// DriverOpen() is called at the beginning of the ICA session to open the virtual channel,
// even before the server side program is executed.
// Then DriverInfo() is called to exchange some virtual channel information with the server
// DriverClose() is called on session termination.
// ICADataArrival is called when a new packet is readable on the virtual channel from the
// server.
// The other ones are not used.

__attribute__((unused)) int DriverOpen(PVD pVd, PVDOPEN pVdOpen, PUINT16 puiSize) {

    char basePath[180];
    miGetPrivateProfileString(VIRTUAL_CHANNEL_NAME, "LogPath", "", basePath, 180); //FIXME : check bufferoverflow
    DebugInit(basePath);

    DEBUG(pSQueueFile, "init\n")

    DEBUG(pFile, "Entering\n")


    VC_INFO vcInfo = {NULL, 0, 0};


    *puiSize = sizeof(VDOPEN); /* must return  the size of VDOPEN in puiSize
                                * because client use this to determine version of vc driver */

    /* Allocation of a vc : fill in a WDQUERYINFORMATION struct and call
     * VdCallWd with it and pVd */
    WDQUERYINFORMATION wdqi_params;

    wdqi_params.WdInformationClass = WdOpenVirtualChannel;

    wdqi_params.WdInformationLength = sizeof(OPENVIRTUALCHANNEL);

    OPENVIRTUALCHANNEL OpenVirtualChannel; //structure (name(filled here) and nb(filled by winstation) of the vc) given to WinStation in VdCallWd to be filled with information
    OpenVirtualChannel.pVCName = VIRTUAL_CHANNEL_NAME;
    wdqi_params.pWdInformation = &OpenVirtualChannel;

    UINT16 uiSize;
    uiSize = sizeof(WDQUERYINFORMATION); //will be set when VdCallWd returns

    int rc;


    rc = VdCallWd(pVd, WDxQUERYINFORMATION, &wdqi_params, &uiSize);


    if(rc != CLIENT_STATUS_SUCCESS){
        return(rc);
    }

    /* if we are here then VdCallWd returned successfully, and filled some
     * information in wdqi.pWdInformation (mapped to OpenVirtualChannel) and
     * pVd */

//    static USHORT gusVirtualChannelNum;
    vcInfo.vcNum = OpenVirtualChannel.Channel;

    DEBUG_VAR(pFile, " OpenVirtualChannel.Channel=%d.\n", vcInfo.vcNum)

    pVdOpen->ChannelMask = (1L << vcInfo.vcNum); /* this must be filled and is used by the client engine */

    DEBUG_VAR(pFile, " VdOpen.ChannelMask=%d.\n", pVdOpen->ChannelMask)


    /* If you want the virtual driver to allocate memory for state data,
     * it can have a pointer to this data returned on each call by placing the
     * pointer in the virtual driver structure, as follows: */
    pVD_CALLBACK_ARGS pVdCallbackArgs = NULL;
    if(NULL == (pVdCallbackArgs = malloc(sizeof(VD_CALLBACK_ARGS)))){
        DEBUG(pFile, "Memory allocation error : pVdCallbackArgs\n");
        return (ERROR_MEMORY);
    }

    pVd->pPrivate = pVdCallbackArgs; //used to pass data to DriverClose and ICADataArrival


    /* Exchange the entry points (ICADataArrival) of the vc driver with winstation by register
     * a write hook. In return, WinStation gives pointers to helper functions outBufXXX
     * For this, must fill a WDSETINFORMATION struct and call VdCallWd */

    WDSETINFORMATION wdsi_params;

    wdsi_params.WdInformationClass = WdVirtualWriteHook;

    wdsi_params.WdInformationLength = sizeof(VDWRITEHOOK);



    VDWRITEHOOK vdwh;
    vdwh.Type = vcInfo.vcNum;
    vdwh.pVdData = pVd;
    vdwh.pProc = ICADataArrival; // or (PVDWRITEPROCEDURE) ICADataArrival
    wdsi_params.pWdInformation = &vdwh;

    uiSize = sizeof(WDSETINFORMATION);

    rc = VdCallWd(pVd, WDxSETINFORMATION, &wdsi_params, &uiSize );
    if(rc != CLIENT_STATUS_SUCCESS){
        return (rc);
    }

    /* If we are here the VdCallWd returned successfully and set some information
     * in wdsi_params.pWdInformation i.e. in vdwh. We have to save them*/

//    static PWD pWd = NULL;
    vcInfo.pWd = vdwh.pWdData;

//    static USHORT gusMaxDataSize;
    vcInfo.maxDataSize = vdwh.MaximumWriteSize -1;

    /*
     * comment from ping example
 *  ALL MEMORY MUST BE ALLOCATED DURING INITIALIZATION.
 *  Allocate a single buffer to respond to the ping request.
 *  This example shows use of the MaximumWriteSize returned via
 *  the previous call.
 *  Subtract one because the first byte is used internally by the
 *  WD for the channel number.  We are given a pointer to the
 *  second byte.
 */

    unsigned char * pSocketRecvBuf = NULL;
    if(NULL == (pSocketRecvBuf = (unsigned char *)malloc(vcInfo.maxDataSize))){
        DEBUG(pErrorFile, "Memory allocation error : pSocketRecvBuf\n")
        return (CLIENT_ERROR_NO_MEMORY);
    }


    pICA_PACKET_QUEUE pIcaPacketQueue = NULL;
    IcaPacketQueue_Init(&pIcaPacketQueue);


    pCONNECTION_MANAGER pConnectionManager = NULL;
    ConnectionManager_Init(&pConnectionManager);




    /* vdwh is also filled with other outputs that are not always used :
     *
     * LPVOID pWdData;                           out: pointer to wd structure
     * POUTBUFRESERVEPROC pOutBufReserveProc;    out: pointer to OutBufReserve
     * POUTBUFAPPENDPROC pOutBufAppendProc;     out: pointer to OutBufAppend
     * POUTBUFWRITEPROC pOutBufWriteProc;        out: pointer to OutBufWrite
     * PAPPENDVDHEADERPROC pAppendVdHeaderProc;  out: pointer to AppendVdHeader
     * USHORT MaximumWriteSize;                  out: maximum ica write size
     * PQUEUEVIRTUALWRITEPROC pQueueVirtualWriteProc;   out: pointer to QueueVirtualWrite
     *
     * According to sdk docs, OutBufXXX functions are deprecated, and QueueVirtualWrite can
     * be used directly in the vc driver (it is declared but not defined in wdapi.h */


    pEPOLL_ARGS pEpollArgs = NULL;
    if(NULL == (pEpollArgs = (pEPOLL_ARGS)malloc(sizeof(EPOLL_ARGS)))){
        DEBUG(pErrorFile, "Memory allocation error : pEpollArgs\n")
        return (CLIENT_ERROR_NO_MEMORY);
    }
    if(FAILURE == (pEpollArgs->epollfd = epoll_create(1))){
        DEBUG(pFile, "Error epoll_create. Exting.\n");
        DEBUG(pErrorFile, "Error epoll_create. Exting.\n");
        exit(1);
    }
    pEpollArgs->maxevents = pConnectionManager->max_connection;
    if(NULL == (pEpollArgs->pEvents = (struct epoll_event *)calloc(sizeof(struct epoll_event),pEpollArgs->maxevents))){
        DEBUG(pErrorFile, "Memory allocation error : pEvents\n")
        return (CLIENT_ERROR_NO_MEMORY);
    }


    int listening_socket;
    if(FAILURE ==(listening_socket = OpenListeningSocket(LISTENING_PORT))){
        DEBUG(pFile, "Error OpenListeningSocket. Exting.\n");
        DEBUG(pErrorFile, "Error OpenListeningSocket. Exting.\n");
        exit(1);
    }




    PEVT pEvtListeningSocket = NULL;
    PEVT pEvtClientSocket = NULL;

    pCALLBACK_ARGS_CLIENT_SOCKET  pCallbackArgsClientSocket = NULL;
    if(NULL == (pCallbackArgsClientSocket = (pCALLBACK_ARGS_CLIENT_SOCKET)malloc(sizeof(CALLBACK_ARGS_CLIENT_SOCKET)))){ //TODO : verifier comment free toute la memoire allouee au moment du DriverClose pour les pointeurs non globaux.
        DEBUG(pErrorFile, "Memory allocation error : pCallbackArgsClientSocket\n")
        return (CLIENT_ERROR_NO_MEMORY);
    }
    pCallbackArgsClientSocket->pConnectionManager = pConnectionManager;
    pCallbackArgsClientSocket->icasendArgs.vcInfo = vcInfo;
    pCallbackArgsClientSocket->icasendArgs.pIcaPacketQueue = pIcaPacketQueue;
    pCallbackArgsClientSocket->pSocketRecvBuf = pSocketRecvBuf;
    pCallbackArgsClientSocket->pEpollArgs = pEpollArgs;


    if(EVT_SUCCESS != Evt_create(NULL, (PFNDELIVER) &Callback_ClientSocket, pCallbackArgsClientSocket, &pEvtClientSocket)){
        DEBUG(pErrorFile, "Client Socket  Event create failed\n")
        exit(1);
    }
    DEBUG(pFile, "Client Socket Event created\n")




    pCALLBACK_ARGS_LISTENING_SOCKET pCallbackArgsListeningSocket = NULL;
    if(NULL == (pCallbackArgsListeningSocket = (pCALLBACK_ARGS_LISTENING_SOCKET)malloc(sizeof(CALLBACK_ARGS_LISTENING_SOCKET)))){ //TODO : verifier comment free toute la memoire allouee au moment du DriverClose pour les pointeurs non globaux.
        DEBUG(pErrorFile, "Memory allocation error : pCallbackArgsListeningSocket\n")
        return (CLIENT_ERROR_NO_MEMORY);
    }
    pCallbackArgsListeningSocket->pConnectionManager = pConnectionManager;
    pCallbackArgsListeningSocket->listening_socket = listening_socket;
    pCallbackArgsListeningSocket->pEvt = pEvtClientSocket;
    pCallbackArgsListeningSocket->icasendArgs.pIcaPacketQueue = pIcaPacketQueue;
    pCallbackArgsListeningSocket->icasendArgs.vcInfo = vcInfo;
    pCallbackArgsListeningSocket->pEpollArgs = pEpollArgs;


    if(EVT_SUCCESS != Evt_create(NULL, (PFNDELIVER) &Callback_ListeningSocket, pCallbackArgsListeningSocket, &pEvtListeningSocket)){
        DEBUG(pErrorFile, "Listening Socket Event create failed\n")
        exit(1);
    }
    DEBUG(pFile, "Listening Socket Event created\n")


    if(EVT_SUCCESS != Evt_trigger_for_input(pEvtListeningSocket, listening_socket)){
        DEBUG(pErrorFile, "Failed to add trigger\n")
        exit(1);
    }
    DEBUG(pFile, "Listening socket trigger added\n")


    PTMR pTmr = NULL;
    pCALLBACK_ARGS_TIMER pCallbackArgsTimer = NULL;
    if(NULL == (pCallbackArgsTimer = (pCALLBACK_ARGS_TIMER)malloc(sizeof(CALLBACK_ARGS_TIMER)))){
        DEBUG(pErrorFile, "Memory allocation error : pCallbackArgsTimer\n")
        return (CLIENT_ERROR_NO_MEMORY);
    }
    pCallbackArgsTimer->icasendArgs.pIcaPacketQueue = pIcaPacketQueue;
    pCallbackArgsTimer->icasendArgs.vcInfo = vcInfo;
    pCallbackArgsTimer->pConnectionManager = pConnectionManager;
    pCallbackArgsTimer->pEpollArgs = pEpollArgs;


    if(TMR_SUCCESS != Tmr_create(NULL, (UINT32)10000, pCallbackArgsTimer ,(PFNDELIVER) &Callback_Timer ,&pTmr)){
        DEBUG(pErrorFile,"Error creating the timer object\n");
        exit(1);
    }

    if(TMR_SUCCESS != Tmr_setEnabled(pTmr, TRUE)){
        DEBUG(pFile, "Error enabling the timer\n");
        exit(1);
    }



    pVdCallbackArgs->driverCloseArgs.pCallbackArgsListeningSocket = pCallbackArgsListeningSocket;
    pVdCallbackArgs->driverCloseArgs.pCallbackArgsClientSocket = pCallbackArgsClientSocket;
    pVdCallbackArgs->driverCloseArgs.pIcaPacketQueue = pIcaPacketQueue;
    pVdCallbackArgs->driverCloseArgs.pConnectionManager = pConnectionManager;
    pVdCallbackArgs->driverCloseArgs.pSocketRecvBuf = pSocketRecvBuf;
    pVdCallbackArgs->driverCloseArgs.pPevtClientSocket = &pEvtClientSocket;
    pVdCallbackArgs->driverCloseArgs.pPevtListeningSocket = &pEvtListeningSocket;
    pVdCallbackArgs->driverCloseArgs.pPtmr = &pTmr;
    pVdCallbackArgs->driverCloseArgs.pCallbackArgsTimer = pCallbackArgsTimer;
    pVdCallbackArgs->driverCloseArgs.listening_socket = listening_socket;
    pVdCallbackArgs->driverCloseArgs.pEpollArgs = pEpollArgs;

    pVdCallbackArgs->icaArrivalArgs.pConnectionManager = pConnectionManager;
    pVdCallbackArgs->icaArrivalArgs.icasendArgs.pIcaPacketQueue=pIcaPacketQueue;
    pVdCallbackArgs->icaArrivalArgs.icasendArgs.vcInfo = vcInfo;
    pVdCallbackArgs->icaArrivalArgs.pEpollArgs = pEpollArgs;


    g_pVdCallbackArgs = pVdCallbackArgs;

    DEBUG(pFile, "Leaving\n\n")

    return (CLIENT_STATUS_SUCCESS);
}

__attribute__((unused)) int DriverClose(__attribute__((unused)) PVD pVd, __attribute__((unused)) PDLLCLOSE pVdClose,
                                        __attribute__((unused)) PUINT16 puiSize){

    DEBUG(pFile, "Entering \n\n");

    DRIVER_CLOSE_ARGS args = ((pVD_CALLBACK_ARGS)pVd->pPrivate)->driverCloseArgs;

    while (ConnectionManager_GetConnectionNumber(args.pConnectionManager)>0){
        if(SUCCESS != CloseConnection(args.pConnectionManager, args.pEpollArgs,
                                      ConnectionManager_GetConnectionByIndex(args.pConnectionManager, 0)->id)){
            DEBUG(pFile, "Error ConnectionManager_CloseConnectionByIndex(). Returning FAILURE.\n");
            return (FAILURE);
        }
    }
    if(args.pConnectionManager){
        ConnectionManager_Close(args.pConnectionManager);

    }
    if(args.pSocketRecvBuf){
        free(args.pSocketRecvBuf);
    }

    if(args.pIcaPacketQueue){
        IcaPacketQueue_Close(args.pIcaPacketQueue);
    }


    if(args.pCallbackArgsClientSocket){
        free(args.pCallbackArgsClientSocket);

    }

    if(args.pCallbackArgsListeningSocket){
        free(args.pCallbackArgsListeningSocket);
    }

    if(args.pCallbackArgsTimer){
        free(args.pCallbackArgsTimer);
    }

    if(args.pEpollArgs){
        if(args.pEpollArgs->pEvents){
            free(args.pEpollArgs->pEvents);
        }
        free(args.pEpollArgs);
    }

    if(args.pPevtClientSocket){
        Evt_destroy(args.pPevtClientSocket);
    }

    if(args.pPevtListeningSocket){
        if(args.listening_socket != -1){
            Evt_remove_triggers(args.listening_socket);
        }
        Evt_destroy(args.pPevtListeningSocket);

    }

    if(args.pPtmr){
        Tmr_destroy(args.pPtmr);
    }

    if(args.listening_socket != -1){
        close(args.listening_socket);
    }



    if(pVd->pPrivate){
        free(pVd->pPrivate);
        pVd->pPrivate = NULL;
    }

    DEBUG(pFile, "Closing files\n");
#ifdef DEBUG_ON
    fclose(pFile);
    fclose(pPollFile);
    fclose(pUnsentFile);
#endif




    DEBUG(pFile, "Leaving \n\n");
    return (CLIENT_STATUS_SUCCESS);
}

__attribute__((unused)) int DriverGetLastError(PVD pVd, PVDLASTERROR pLastError){
    DEBUG(pFile, "Entering\n\n");

    /* Provided for compatibility, no real use for virtual drivers */
    pLastError->Error = pVd->LastError;
    pLastError->Message[0] = '\0';

    DEBUG(pFile, "Leaving\n\n");
    return (CLIENT_STATUS_SUCCESS);
}

__attribute__((unused)) int DriverInfo(PVD pVd, PDLLINFO pVdInfo, PUINT16 puiSize){
    DEBUG(pFile, "Entering\n\n");

    *puiSize = sizeof(DLLINFO);

    PVDSOCKS_C2H  pVdData;
    USHORT requiredByteCount;
    requiredByteCount = sizeof(VDSOCKS_C2H);

    if (pVdInfo->ByteCount < requiredByteCount){
        DEBUG(pFile, " Info buffer too small...\n")

        pVdInfo->ByteCount = requiredByteCount;

        DEBUG(pFile, "Leaving\n")
        return (CLIENT_ERROR_BUFFER_TOO_SMALL);
    }

    DEBUG(pFile, " Info buffer large enough...\n")

    pVdInfo->ByteCount = requiredByteCount;
    pVdData = (PVDSOCKS_C2H) pVdInfo->pBuffer;

    PMODULE_C2H pModuleHeader;
    pModuleHeader = &pVdData->Header.Header;
    pModuleHeader->ByteCount = requiredByteCount;
    pModuleHeader->ModuleClass = Module_VirtualDriver;
    pModuleHeader->VersionL = 1;
    pModuleHeader->VersionH = 1;
    strcpy((char*)(pModuleHeader->HostModuleName), "ICA");

    pVdData->Header.ChannelMask = pVd->ChannelMask;
    PVDFLOW pFlow;
    pFlow = &pVdData->Header.Flow;
    pFlow->Flow = VirtualFlow_None;
    pFlow->BandwidthQuota = 0;


    pVdInfo->ByteCount = WIRE_WRITE(VDPING_C2H, pVdData, requiredByteCount);
    DEBUG(pFile, "Leaving \n");

    return( CLIENT_STATUS_SUCCESS );
}

__attribute__((unused)) int DriverPoll(__attribute__((unused)) PVD pVd, __attribute__((unused)) PDLLPOLL pVdPoll,
                                       __attribute__((unused)) PUINT16 puiSize){
    DEBUG(pPollFile, "Entering\n\n")

    DEBUG_INFO(pPollFile);
    DEBUG(pPollFile, "Leaving \n");
    
    return (CLIENT_STATUS_SUCCESS);
}

__attribute__((unused)) int DriverQueryInformation(__attribute__((unused)) PVD pVd,
                                                   __attribute__((unused)) PVDQUERYINFORMATION pVdQueryInformation, PUINT16 puiSize){
    /* for compatibility, no real use for virtual drivers*/
    DEBUG(pFile, "Entering DriverQueryInformation.\n\n");
    
    * puiSize = sizeof(VDQUERYINFORMATION);
    return (CLIENT_STATUS_SUCCESS);
}

__attribute__((unused)) int DriverSetInformation(__attribute__((unused)) PVD pVd,
                                                 __attribute__((unused)) PVDSETINFORMATION pVdSetInformation,
                                                 __attribute__((unused)) PUINT16 puiSize ){
    DEBUG(pFile, "Entering and leaving\n\n");
    
    return (CLIENT_STATUS_SUCCESS);
}

static int WFCAPI ICADataArrival(__attribute__((unused)) PVOID pVd, __attribute__((unused)) USHORT uChan, LPBYTE pBuf,
                                 __attribute__((unused)) USHORT Length){

    DEBUG(pFile, "\n\nEntering\n");
    DEBUG_INFO(pFile);
    DEBUG_VAR(pFile, "Received a %d bytes packet from ICA.\n", Length);


    ICA_ARRIVAL_ARGS args = ((pVD_CALLBACK_ARGS)(((PVD)pVd)->pPrivate))->icaArrivalArgs;

    PACKET_TYPE type;
    ID id;
    LEN len;
    unsigned char * pData;
    Parse_Ica_GenericPacket(pBuf, &type, &id, &len, &pData);
    if(len > Length - PACKET_TYPE_SIZE - ID_SIZE - LEN_SIZE){
        DEBUG(pFile, "Error, the parsed length is too large. Exiting.\n");
        exit(1);
    }

    DEBUG_VAR(pFile, " Parsing : type=%d, id=%d, len=%d, data_addr=%p \n", type, id, len, pData);


    switch (type) {
        case META_SERVER_START:
            if(SUCCESS != Handler_Meta_ServerStart(args.pConnectionManager, args.icasendArgs, args.pEpollArgs)){
                DEBUG(pFile, "FATAL ERROR Handler_Meta_ServerStart. Exiting.\n");
                DEBUG(pErrorFile, "FATAL ERROR Handler_Meta_ServerStart. Exiting.\n");
                exit(1);
            }
            break;
        case META_SERVER_STOP:
            if(SUCCESS != Handler_Meta_ServerStop(args.pConnectionManager, args.icasendArgs, args.pEpollArgs)){
               DEBUG(pFile, "FATAL ERROR Handler_Meta_ServerStop. Exiting.\n");
               DEBUG(pErrorFile, "FATAL ERROR Handler_Meta_ServerStop. Exiting.\n");
               exit(1);
            }
            DEBUG(pFile, "ServerStop. Exiting.\n");
            DEBUG(pErrorFile, "ServerStop. Exiting.\n");
            exit(1);


        case SOCKS_DATA:
            if(SUCCESS != Handler_Ica_DataPacket(args.pConnectionManager, args.icasendArgs, args.pEpollArgs, id, len, pData)){
                DEBUG(pFile, "FATAL ERROR Handler_Ica_DataPacket. Exiting.\n");
                DEBUG(pErrorFile, "FATAL ERROR Handler_Ica_DataPacket. Exiting.\n");
                exit(1);
            }
            break;

        case SOCKS_SERVER_RPL:
            if(SUCCESS != Handler_Ica_ServerRplPacket(args.pConnectionManager, args.icasendArgs,args.pEpollArgs , id, len, pData)){
                DEBUG(pFile, "FATAL ERROR Handler_Ica_ServerRplPacket. Exiting.\n");
                DEBUG(pErrorFile, "FATAL ERROR Handler_Ica_ServerRplPacket. Exiting.\n");
                exit(1);
            }
            break;
        case CLOSE_REQUEST:
            if(SUCCESS != Handler_Ica_CloseRequest(args.pConnectionManager, args.icasendArgs, args.pEpollArgs, id)){
                DEBUG(pFile, "FATAL ERROR Handler_Ica_CloseRequest. Exiting.\n");
                DEBUG(pErrorFile, "FATAL ERROR Handler_Ica_CloseRequest. Exiting.\n");

                exit(1);
            }
            break;
        case CLOSE_ACK:
            if(SUCCESS != Handler_Ica_CloseAck(args.pConnectionManager, args.pEpollArgs, id)){
                DEBUG(pFile, "FATAL ERROR Handler_Ica_CloseRequest. Exiting.\n");
                DEBUG(pErrorFile, "FATAL ERROR Handler_Ica_CloseRequest. Exiting.\n");

                exit(1);
            }
            break;
        case META_WINDOW:
            if(SUCCESS != Handler_MetaWindow(args.pConnectionManager, id, pData)){
                DEBUG(pFile, "FATAL ERROR Handler_MetaWindow. Exiting.\n");
                DEBUG(pErrorFile, "FATAL ERROR Handler_MetaWindow. Exiting.\n");

                exit(1);
            }
            break;
        default:
            DEBUG(pFile, "Unknown packet type. Exiting.\n")
            DEBUG(pErrorFile, "Unknown packet type. Exiting.\n")

            exit(1);


    }

    return (CLIENT_STATUS_SUCCESS);
}


// **************************** CALLBACKS ***************************************************
// ******************************************************************************************
// These functions are callbacks triggered by events on the file descriptors representing :
// - the listening socket (the one accepting the connections and returning client sockets)
// - all the client sockets
// The association of the events to these functions is done in DriverOpen() for the listening
// socket and in Callback_ListeningSocket for the client sockets.
// ******************************************************************************************


/**
 * Callback triggered when the listening socket is readable.
 * It accepts the incoming connection, sets the new client socket to non-blocking,
 * allocates a row in the connection manager, and adds the client socket input as
 * a trigger for Callback_ClientSocket()
 *
 * @param arg1 : same pointer as pSubscriberId passed to Evt_create() in DriverOpen().
 * Used to pass arguments to the callback. Here a pointer to a CALLBACK_ARGS_LISTENING_SOCKET
 * is passed.
 * @param arg2 : not used.
 */
void Callback_ListeningSocket(void * arg1 , __attribute__((unused)) void * arg2) {
    DEBUG(pFile, "\n\nEntering\n")
    DEBUG_INFO(pFile);

    pCALLBACK_ARGS_LISTENING_SOCKET pArgs = ((pCALLBACK_ARGS_LISTENING_SOCKET)arg1); //pArgs is a pointer to the structure of arguments


    int client_socket;
    if(-1 == (client_socket = accept(pArgs->listening_socket,NULL,NULL))){
        DEBUG_VAR(pFile, "Error in accept() : errno %d, %s\n", errno, strerror(errno))
        //TODO : deal with each possible error, some should not imply exiting the program
        exit(1);
    }

    if(pArgs->pConnectionManager->online == 0){
        DEBUG(pErrorFile, "Error, server is offline, closing socket.\n");
        DEBUG(pFile, "Error, server is offline, closing socket.\n")
        close(client_socket); //FIXME : faut il envoyer RST ou fermer correctement pour que le client comprenne qu'on est busy
        return;
    }


    int opt= 1;
    if( ioctl(client_socket, FIONBIO, (char *)&opt) < 0){ //sets the socket to non blocking
        DEBUG(pFile,"Error iotctl().\n");
        exit(1);
    }



    DEBUG_VAR(pFile, "New client socket %d accepted !\n", client_socket);

    if(SUCCESS != ConnectionManager_NewConnection(pArgs->pConnectionManager, client_socket)){
        DEBUG(pFile, "DriverPoll(): Could not create a new connection for the socket, closing it.\n");
        close(client_socket);
        exit(1); //TODO : s'adapter aux differentes erreurs

    }


    DEBUG_VAR(pFile, "pEvt : %p, client_socket : %d\n", pArgs->pEvt, client_socket);
    if(EVT_SUCCESS != Evt_trigger_for_input(pArgs->pEvt, client_socket)){ // adds the client socket's input to the list of triggers for Callback_ClientSocket()
        DEBUG(pFile, "Error : could not add trigger to event\n")

    }
    DEBUG(pFile, "Trigger added\n")

    if(pArgs->pConnectionManager->max_connection > pArgs->pEpollArgs->maxevents){
        pArgs->pEpollArgs->maxevents = pArgs->pConnectionManager->max_connection;
        struct epoll_event * pEvents_tmp = NULL;
        if(NULL == (pEvents_tmp = (struct epoll_event *) reallocarray(pArgs->pEpollArgs->pEvents, pArgs->pEpollArgs->maxevents, sizeof(struct epoll_event)))) {
            DEBUG(pFile, "Memory reallocation  error : pEvents\n");
            exit(1);
        }
        pArgs->pEpollArgs->pEvents = pEvents_tmp;
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLRDHUP;
    event.data.fd = client_socket;
    if(SUCCESS != epoll_ctl(pArgs->pEpollArgs->epollfd, EPOLL_CTL_ADD, client_socket, &event)){
        DEBUG(pFile,"Error epoll_ctl().\n");
        exit(1);
    }


}


/**
 * Callback triggered when one of the previously accepted client sockets is readable.
 * Loops over all the opened connections of the connection manager : if possible (queues are not full),
 * tries to read from each socket, and depending on the connection state and the information read, does
 * what must be done (transfer data to the virtual channel, closes the connection, ...)
 *
 * @param arg1  Same pointer as pSubscriberId passed to Evt_create() in DriverOpen().
 * Used to pass arguments to the callback. Here a pointer to a CALLBACK_ARGS_CLIENT_SOCKET
 * is passed.
 * @param arg2  Not used.
 */
void Callback_ClientSocket(void * arg1, __attribute__((unused)) void * arg2){


    pCALLBACK_ARGS_CLIENT_SOCKET pArgs = ((pCALLBACK_ARGS_CLIENT_SOCKET)arg1); //pArgs is a pointer to the structure of arguments

    ssize_t bytesRead;
    CONNECTION_STATE state;
    int socket_fd;
    ID id;
    VER ver;

    int bytesWritten;


    NMETHODS nMethods;
    unsigned char * pMethods;
    CMD cmd;
    RSV rsv;
    ATYP aTyp;
    IPADDR ipAddr;
    IP6ADDR ip6Addr;
    DOMLEN domLen;
    unsigned char * pDomData;
    PORT port;
    int bytes_available=0;

    DEBUG(pFile, "\n\nEntering\n");
    DEBUG_INFO(pFile);


    int ready_events = 0;
    if(FAILURE == (ready_events = epoll_wait(pArgs->pEpollArgs->epollfd, pArgs->pEpollArgs->pEvents, pArgs->pEpollArgs->maxevents, 0))){
        DEBUG(pFile, "Error epoll_wait()\n");
        exit(FAILURE);
    }
    DEBUG_VAR(pFile, "ready_events  = %d\n", ready_events);

    for(int i=0; i < ready_events; i++) { //on parcourt les fds de la readylist de epoll
        int fd = pArgs->pEpollArgs->pEvents[i].data.fd;
        uint32_t events = pArgs->pEpollArgs->pEvents[i].events;
        DEBUG_VAR(pFile, "---#%d, fd = %d = id, events = %d---\n",i, fd, events);

        if (IcaPacketQueue_GetFreeSpace(pArgs->icasendArgs.pIcaPacketQueue) > 0) { //checks if there is some space for
            // at least one Virtual Channel packet in the queue before trying to read


            pCONNECTION pConnection;
            if (NULL == (pConnection = ConnectionManager_GetConnectionById(pArgs->pConnectionManager, fd))) {
                DEBUG(pFile, "Error getting connection\n");
                DEBUG(pErrorFile, "Error getting connection, exiting the program\n");
                exit(FAILURE);
            }
            socket_fd = pConnection->socket_fd;
            state = pConnection->state;
            id = pConnection->id;
            int window = pConnection->window;

            if(pConnection->state == CLOSING){ //skip the connection if it is in CLOSING state.
                DEBUG(pErrorFile, "Should not happen, if state==CLOSING then the fd is not in epoll or Evt..\n")
                DEBUG(pFile, "Should not happen, if state==CLOSING then the fd is not in epoll or Evt..\n")
                continue;
            }

            if(window > MAX_ICA_PACKET_DATA_SIZE) { //checks that the server side socket queue for this connection is
                // large enough before trying to read


                DEBUG_VAR(pFile, "trying to read socket %d in buffer %p\n", socket_fd, pArgs->pSocketRecvBuf);

                ssize_t rc_recv;
                if (SOCKET_ERROR == (rc_recv = recv(socket_fd, pArgs->pSocketRecvBuf, MAX_ICA_PACKET_DATA_SIZE - 20,
                                                    0))) {
                    switch (errno) {
                        case EAGAIN:
                            //we can ignore this error as well as the EWOULDBLOCK, they just mean that there was nothing to read from this fd
                            continue;
                            //case EWOULDBLOCK:
                            //continue;
                        case EBADF:
                            DEBUG(pFile, "EBADF : closing connection.\n");

                            if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                 pArgs->pEpollArgs)){
                                DEBUG(pFile, "Error InitiateCloseProcedure()\n");
                                exit(1);
                            }


                            continue;

                        case ECONNREFUSED:
                            DEBUG(pFile, "ECONNREFUSED : closing connection.\n");
                            if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                 pArgs->pEpollArgs)){
                                DEBUG(pFile, "Error InitiateCloseProcedure()\n");
                                exit(1);
                            }


                            continue;
                        case ECONNRESET:
                            DEBUG(pFile, "ECONNRESET : closing the connection\n");
                            if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                 pArgs->pEpollArgs)){
                                DEBUG(pFile, "Error InitiateCloseProcedure()\n");
                                exit(1);
                            }


                            continue;
                        case EFAULT:
                            DEBUG(pFile, "EFAULT : exiting !\n");
                            exit(FAILURE);
                        case EINTR:
                            DEBUG(pFile, "EINTR : ignoring\n");
                            continue;
                        case ENOTSOCK:
                            DEBUG(pFile, "ENOTSOCK : closing connection\n");
                            if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                 pArgs->pEpollArgs)){
                                DEBUG(pFile, "Error InitiateCloseProcedure()\n");
                                exit(1);
                            }


                            continue;
                        default:
                            DEBUG_VAR(pFile, "Unattended error in recv() : %d : %s. Exiting.\n", errno,
                                      strerror(errno));
                            exit(FAILURE);

                    }
                }

                bytesRead = (int) rc_recv;
                if (bytesRead == 0) {
                    DEBUG_VAR(pFile, "0 bytes read : Socket %d trying to close. Closing the connection. (events = %d)\n", socket_fd, events);
                    if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id, pArgs->pEpollArgs)){
                        DEBUG(pFile, "Error InitiateCloseProcedure()\n");
                        exit(1);
                    }
                    continue;
                }


                DEBUG_VAR(pFile, "Received %ld bytes from %d socket.\n", bytesRead, socket_fd);




                switch (state) {
                    case SOCKET_OPEN:

                        Parse_Socket_ClientMethodsPacket(pArgs->pSocketRecvBuf, &ver, &nMethods, &pMethods);
                        if(nMethods > sizeof(pArgs->pSocketRecvBuf) - VER_SIZE - NMETHODS_SIZE){
                            DEBUG(pFile, "ERROR Parsing : nMethods larger than the buffer\n");
                            exit(1);
                        }

                        DEBUG_VAR(pFile, "Parsing : VER=%d, NMETHODS=%d, METHODS=", ver, nMethods);
                        DEBUG_BUF(pMethods, nMethods);


                        if (ver != 5) {
                            DEBUG(pFile, "Wrong VER, not SOCKS5 protocol.\n");

                            if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                 pArgs->pEpollArgs)){
                                DEBUG(pFile, "FATAL ERROR : could not close the connection. Exiting \n");
                                exit(FAILURE);
                            }
                            break;
                        }
                        if (nMethods < 1) {
                            DEBUG(pFile, "Not enough methods.\n");

                            if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                 pArgs->pEpollArgs)){
                                DEBUG(pFile, "FATAL ERROR : could not close the connection. Exiting \n");
                                exit(FAILURE);
                            }
                            break;
                        }

                        BOOL compatible = FALSE;
                        METHOD chosen_mth = 0xff;
                        for (int m = 0; m < nMethods; m++) {
                            if (pMethods[m] == 0x00) {
                                DEBUG(pFile, "No Auth supported !\n");
                                compatible = TRUE;
                                chosen_mth = 0x00;
                                break;
                            }
                        }

                        int rc;
                        int bytesSent = 0;
                        if (SUCCESS != (rc = Send_Socket_MethodSelectionPacket(socket_fd, ver, chosen_mth,
                                                                               pConnection->pSocketQueue,
                                                                               &bytesSent))) {
                            switch (rc) {
                                case ERROR_FATAL:
                                    DEBUG(pErrorFile, "FATAL Error\n");
                                    exit(1);
                                    break;
                                case ERROR_CLOSE_CONNECTION:
                                    DEBUG(pFile, "Closing the connection\n");
                                    if(SUCCESS !=
                                            InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                   pArgs->pEpollArgs)){
                                        DEBUG(pFile, "FATAL ERROR : could not close the connection. Exiting \n");
                                        exit(FAILURE);
                                    }
                                    break;
                                case FAILURE:
                                    DEBUG(pFile, "Closing the connection\n");
                                    if(SUCCESS !=
                                            InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                   pArgs->pEpollArgs)){
                                        DEBUG(pFile, "FATAL ERROR : could not close the connection. Exiting \n");
                                        exit(FAILURE);
                                    }
                                    break;
                            }
                        }
                        else {
                            DEBUG_VAR(pFile, "%d bytes sent back\n", bytesSent);
                            ConnectionManager_SetConnectionStateById(pArgs->pConnectionManager, id, METHOD_CHOSEN);


                            if (!compatible) {
                                DEBUG(pFile, "No Auth is not supported by client, closing connection\n");


                                if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                     pArgs->pEpollArgs)){
                                    DEBUG(pFile, "FATAL ERROR : could not close the connection. Exiting \n");
                                    exit(FAILURE);
                                }
                                break;
                            }
                        }
                        break;

                    case METHOD_CHOSEN:
                        DEBUG(pFile, "Entering METHOD_CHOSEN\n");


                        Parse_Socket_ClientCmdPacket(pArgs->pSocketRecvBuf, &ver, &cmd, &rsv, &aTyp, &ipAddr, &ip6Addr,
                                                     &port,
                                                     &domLen, &pDomData);
                        DEBUG(pFile, "Exiting ParseClient\n");

                        if (aTyp == 1) {
                            DEBUG_VAR(pFile,
                                      "Parsing : VER=%d, CMD=%d, RSV=%d, ATYP=%d, IPADDR=%d, PORT=%d\n", ver,
                                      cmd, rsv, aTyp, ipAddr, port);

                            if(SUCCESS != Send_Ica_IpCmdPacket(pArgs->icasendArgs,id,ver, cmd, ipAddr, port)){
                                DEBUG(pFile, "Error Send_Ica_IpCmdPacket()");
                                DEBUG(pErrorFile, "Error Send_Ica_IpCmdPacket()");
                                if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                     pArgs->pEpollArgs)){
                                    DEBUG(pFile, "FATAL ERROR : could not close the connection. Exiting \n");
                                    exit(FAILURE);
                                }
                                break;
                            }

                            ConnectionManager_SetConnectionStateById(pArgs->pConnectionManager, id, CMD_SENT);


                        }
                        if (aTyp == 3) {
                            DEBUG_VAR(pFile,
                                      "Parsing : VER=%d, CMD=%d, RSV=%d, ATYP=%d, DOMLEN=%d, PORT=%d\n", ver,
                                      cmd, rsv, aTyp, domLen, port);

                            if(SUCCESS != Send_Ica_DomainCmdPacket(pArgs->icasendArgs, id, ver, cmd, port, domLen,pDomData)){
                                DEBUG(pFile, "Error Send_Ica_DomainCmdPacket()");
                                DEBUG(pErrorFile, "Error Send_Ica_DomainCmdPacket()");
                                if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                     pArgs->pEpollArgs)){
                                    DEBUG(pFile, "FATAL ERROR : could not close the connection. Exiting \n");
                                    exit(FAILURE);
                                }
                                break;
                            }
                            DEBUG_BUF(pDomData, domLen);
                            ConnectionManager_SetConnectionStateById(pArgs->pConnectionManager, id, CMD_SENT);

                        }
                        if (aTyp == 4){
                            DEBUG(pFile, "ipv6")
                            DEBUG_VAR(pFile,
                                      "Parsing : VER=%d, CMD=%d, RSV=%d, ATYP=%d, PORT=%d, IP6ADDR=%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", ver,
                                      cmd, rsv, aTyp, port, ip6Addr[0],ip6Addr[1],ip6Addr[2],ip6Addr[3],ip6Addr[4],ip6Addr[5],ip6Addr[6],ip6Addr[7],ip6Addr[8],ip6Addr[9],ip6Addr[10],ip6Addr[11],ip6Addr[12],ip6Addr[13],ip6Addr[14],ip6Addr[15]);

                            if(SUCCESS != Send_Ica_Ip6CmdPacket(pArgs->icasendArgs,id,ver, cmd, port, ip6Addr)){
                                DEBUG(pFile, "Error Send_Ica_Ip6CmdPacket()");
                                DEBUG(pErrorFile, "Error Send_Ica_Ip6CmdPacket()");
                                if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, id,
                                                                     pArgs->pEpollArgs)){
                                    DEBUG(pFile, "FATAL ERROR : could not close the connection. Exiting \n");
                                    exit(FAILURE);
                                }
                                break;
                            }

                            ConnectionManager_SetConnectionStateById(pArgs->pConnectionManager, id, CMD_SENT);
                        }

                        break;

                    case FREE:
                        DEBUG(pErrorFile, "Error, connection state is FREE but receiving data !\n");
                        exit(FAILURE);
                        break;
                    case ESTABLISHED:
                        DEBUG(pFile, "Transferring bytes of established connection !\n");


                        if(SUCCESS != Send_Ica_DataPacket(pArgs->icasendArgs, id, bytesRead, pArgs->pSocketRecvBuf)){
                            DEBUG(pFile, "FATAL ERROR : could not send or queue the data packet\n");
                            exit(1);
                        }
                        if(SUCCESS != ConnectionManager_SubConnectionWindowById(pArgs->pConnectionManager, id, (int)bytesRead)){
                            DEBUG(pFile, "FATAL ERROR : could not decrease window after send\n");
                            exit(1);
                        }

                        break;
                    case CMD_SENT:
                        DEBUG(pFile,
                              "Command packet sent to the server, still waiting for an answer to consider this connection established.\n");
                        DEBUG(pErrorFile,
                              "Command packet sent to the server, still waiting for an answer to consider this connection established.\n");


                        break;
                    default:
                        DEBUG(pFile, "Unknown state, should not happen...\n");
                        DEBUG(pErrorFile, "Unknown state, should not happen...\n");
                        break;
                }

            }
            else{ //FIXME (a virer) on y passe beaucoup en upload, jamais en download
                DEBUG(pFile, "Window too small\n")
            }


        }
        else{ //FIXME (a virer) on y passe jamais
            DEBUG(pFile, "No space in ica queue.\n");
        }

    }



}
/**
 * Callback triggered by a timer fired every 10seconds. It tries to send the packets in the Virtual Channel queue as
 * well as in the socket queues (and acknowledges the sent bytes accordingly).
 *
 * @param arg1 Same pointer as pvSubscriber passed to Tmr_create() in DriverOpen(). Used to pass arguments to the
 * callback. Here a pointer to a CALLBACK_ARGS_TIMER is passed.
 * @param arg2 Not used.
 */
void Callback_Timer(void * arg1, __attribute__((unused)) void * arg2){
    DEBUG(pFile, "Entering\n");
    pCALLBACK_ARGS_TIMER pArgs = (pCALLBACK_ARGS_TIMER)arg1;

    if(ERROR_FATAL == Send_IcaQueue(pArgs->icasendArgs)){
        DEBUG(pFile, "Error in Send_IcaQueue(). Exiting\n");
        exit(1);
    }

    for(int i=0; i<ConnectionManager_GetConnectionNumber(pArgs->pConnectionManager); i++){
        int bytesSent;
        pCONNECTION pConnection;
        if(NULL == (pConnection = ConnectionManager_GetConnectionByIndex(pArgs->pConnectionManager, i))){
            DEBUG(pFile, "Could not get connection.\n");
            exit(1);
        }
        int rc;
        if(SUCCESS != (rc = Send_SocketQueue(pConnection->socket_fd, pConnection->pSocketQueue, &bytesSent))){
            switch (rc) {
                case ERROR_FATAL:
                    DEBUG(pErrorFile, "FATAL Error\n");
                    exit(1);
                case ERROR_CLOSE_CONNECTION:
                    if(SUCCESS != InitiateCloseProcedure(pArgs->pConnectionManager, pArgs->icasendArgs, pConnection->id,
                                                         pArgs->pEpollArgs)){
                        DEBUG(pFile, "FATAL ERROR InitiateCloseProcedure()\n");
                        exit(1);
                    }
                case FAILURE:
                    break;
            }
        }

        if(bytesSent>0) {
            if (SUCCESS != Send_Ica_MetaWindow(pArgs->icasendArgs, bytesSent, pConnection->id)) {
                DEBUG(pFile,
                      "Error Send_Ica_MetaWindow() : could not send the meta packet to acknowledge successful socket sending.Exiting.\n");
                exit(1);
            }
        }
    }

    DEBUG(pFile, "Leaving\n");
}

// ************************ HANDLERS *********************************************************
// *******************************************************************************************
// These functions are the handlers for the different packets received on the virtual channel.
// ICADataArrival reads the packet type and then call the corresponding handler.
// *******************************************************************************************


/**
 * Handles the ServerStart Meta packet received from the Citrix server : closes all the current connections
 * from the connection manager, empties the virtual channel queue and sets the online variable of the connection
 * manager to TRUE.
 *
 * @param pConnectionManager A pointer to the connection manager initialized in DriverOpen(). See connection_manager.c
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @return SUCCESS or FAILURE.
 */
int
Handler_Meta_ServerStart(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs) {

    DEBUG(pFile,"Entering\n");
    while(ConnectionManager_GetConnectionNumber(pConnectionManager)>0){
        if(SUCCESS != CloseConnection(pConnectionManager, pEpollArgs,
                                      ConnectionManager_GetConnectionByIndex(pConnectionManager, 0)->id)){
            DEBUG(pFile, "Error ConnectionManager_CloseConnectionByIndex(). Returning FAILURE.\n");
            return (FAILURE);
        }
    }

    while(!IcaPacketQueue_IsEmpty(icasendArgs.pIcaPacketQueue)){
        IcaPacketQueue_Dequeue(icasendArgs.pIcaPacketQueue);
    }

    pConnectionManager->online = 1;

    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 * Handles the ServerStop Meta packet received from the Citrix server : closes all the connections of the connection manager,
 * empties the virtual channel queue and sets the online variable of the connection manager to FALSE.
 *
 * @param pConnectionManager A pointer to the connection manager initialized in DriverOpen(). See connection_manager.c
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @return SUCCESS or FAILURE.
 */
int Handler_Meta_ServerStop(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs) {
    DEBUG(pFile, "Entering\n");

    while (ConnectionManager_GetConnectionNumber(pConnectionManager)>0){
        if(SUCCESS != CloseConnection(pConnectionManager, pEpollArgs,
                                      ConnectionManager_GetConnectionByIndex(pConnectionManager, 0)->id)){
            DEBUG(pFile, "Error ConnectionManager_CloseConnectionByIndex(). Returning FAILURE.\n");
            return (FAILURE);
        }
    }

    while(!IcaPacketQueue_IsEmpty(icasendArgs.pIcaPacketQueue)){
        IcaPacketQueue_Dequeue(icasendArgs.pIcaPacketQueue);
    }
    pConnectionManager->online = 0;
    DEBUG(pFile, "Leaving\n");
    return (SUCCESS);
}

/**
 * Handles the Window Meta packet received from the Citrix server : parses the packet to get the number of bytes that must
 * be added to the window of connection id and then adds them.
 *
 * @param pConnectionManager A pointer to the connection manager initialized in DriverOpen(). See connection_manager.c
 * @param id The id of the connection whose window must be updated.
 * @param pBuf The buffer containing the received Virtual Channel packet.
 * @return SUCCESS or FAILURE
 */
int Handler_MetaWindow(pCONNECTION_MANAGER pConnectionManager, ID id, unsigned char * pBuf){
    DEBUG(pFile, "Entering\n");
    int window;

    Parse_Ica_MetaWindow(pBuf, &window);

    if(SUCCESS != ConnectionManager_AddConnectionWindowById(pConnectionManager, id, window)){
        DEBUG(pFile, "FATAL ERROR : could not add window\n");
        return (FAILURE);
    }


    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 * Handles the Data packet received from the Citrix server : if the connection is in ESTABLISHED state, it transfers the
 * data received to the corresponding socket and then acknowledges by sending back a Window packet containing the number of
 * bytes sent to the socket so that the server side can update its available window value.
 *
 * @param pConnectionManager A pointer to the connection manager initialized in DriverOpen(). See connection_manager.c
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param id The id of the connection whose window must be updated.
 * @param len The length of pBuf
 * @param pBuf The buffer containing the received Virtual Channel packet.
 * @return SUCCESS or FAILURE
 */
int
Handler_Ica_DataPacket(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs, ID id,
                       LEN len, unsigned char *pBuf) {
    DEBUG(pFile, "Entering \n");
    //DEBUG(pFile, " len : %zu\n", len);
    //DEBUG(pFile, " pBuf : %p\n", pBuf);
 //   


    pCONNECTION  pConnection ;
    if(NULL == (pConnection = ConnectionManager_GetConnectionById(pConnectionManager, id))){
        DEBUG_VAR(pFile, "Connection %d was not found\n", id);
        return(FAILURE);
    }

    int socket_fd = pConnection->socket_fd;
    pSOCKET_QUEUE pSocketQueue = pConnection->pSocketQueue;
    CONNECTION_STATE state = pConnection->state;


    if(state == ESTABLISHED){
        int rc;
        int bytesSent = 0;
        if(SUCCESS != (rc = Send_Socket_DataPacket(socket_fd, pSocketQueue, len, pBuf, &bytesSent))){
            switch (rc) {
                case ERROR_FATAL:
                    DEBUG(pErrorFile, "FATAL Error\n");
                    return(FAILURE);
                    break;
                case FAILURE:
                    DEBUG(pErrorFile, "FAILURE\n");
                    return(FAILURE);
                case ERROR_CLOSE_CONNECTION:
                    if(SUCCESS != InitiateCloseProcedure(pConnectionManager, icasendArgs, id, pEpollArgs)){
                        DEBUG(pFile, "FATAL ERROR InitiateCloseProcedure()\n");
                        return(FAILURE);
                    }
                    return (SUCCESS);

            }
        }

        if(SUCCESS != Send_Ica_MetaWindow(icasendArgs, bytesSent, id)){
            DEBUG(pFile, "Error Send_Ica_MetaWindow() : could not send the meta packet to acknowledge successful socket sending. Returning FAILURE\n");
            return (FAILURE);
        }


    }
    else if (state == CLOSING){
        DEBUG(pFile, "Connection in closing state, dropping the packet\n");

    }
    else{
        DEBUG(pFile, "Connection not in ESTABLISHED or CLOSED.\n");
        return(FAILURE);
    }

    DEBUG(pFile, "Leaving\n")
    return(SUCCESS);
}

/**
 * Handles the Server Reply packet received from the Citrix server : transfers the SOCKS5 reply to the corresponding socket and
 * then acknowledges by sending back a Window packet containing the number of bytes sent to the socket so that the server
 * side can update its available window value. Changes the connection state accordingly.
 *
 * @param pConnectionManager A pointer to the connection manager initialized in DriverOpen(). See connection_manager.c
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param id The id of the connection whose window must be updated.
 * @param len The length of pBuf
 * @param pBuf The buffer containing the received Virtual Channel packet.
 * @return SUCCESS or FAILURE
 */
int
Handler_Ica_ServerRplPacket(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs,
                            ID id, LEN len, unsigned char *pBuf) {
    DEBUG(pFile, "Entering\n");

    pCONNECTION  pConnection;
    if(NULL == (pConnection = ConnectionManager_GetConnectionById(pConnectionManager, id))){
        DEBUG_VAR(pFile, "Connection %d was not found\n", id);
        return(FAILURE);
    }
    int socket_fd = pConnection->socket_fd;
    CONNECTION_STATE state = pConnection->state;
    
    VER ver= -1;
    RPL rpl= -1;


    if(state==CMD_SENT){
        int bytesSent = 0;
        Parse_Ica_ServerRplPacket(pBuf, &ver, &rpl);
        DEBUG_VAR(pFile, "Parsing : VER=%d, RPL=%d\n", ver, rpl);

        int rc;
        if(SUCCESS !=(rc= Send_Socket_RplPacket(socket_fd, ver, rpl, 0, 0, pConnection->pSocketQueue, &bytesSent))){
            switch (rc) {
                case ERROR_FATAL:
                    DEBUG(pErrorFile, "FATAL Error\n");
                    return (FAILURE);

                case ERROR_CLOSE_CONNECTION:
                    if(SUCCESS != InitiateCloseProcedure(pConnectionManager, icasendArgs, id, pEpollArgs)){
                        DEBUG(pFile, "FATAL ERROR InitiateCloseProcedure()\n");
                        return(FAILURE);
                    }
                    return (SUCCESS);
            }
        }

        if(SUCCESS != Send_Ica_MetaWindow(icasendArgs, bytesSent, id)){
            DEBUG(pFile, "Error Send_Ica_MetaWindow() : could not send the meta packet to acknowledge successful socket sending. Returning FAILURE\n");
            return (FAILURE);
        }
        if(rpl==0x00){
            ConnectionManager_SetConnectionStateById(pConnectionManager, id, ESTABLISHED);

            DEBUG(pFile, "Connection established !!!\n");
            
        }

    }
    else if(state == CLOSING){
        DEBUG(pFile, "Connection in closing state, dropping the packet\n")
    }
    else{
        DEBUG(pFile, "Error, receiving a server reply packet while not in CMD_SENT or CLOSING state\n");
        
    }
    DEBUG(pFile, "Leaving\n")
    return(SUCCESS);
}

/**
 * Handles a Close Request packet received from the Citrix server : checks if the connection is already in CLOSING state.
 * If it is, ignores the packet and wait for the closing acknowledgement from the server. If not, closes the connection
 * in the connection manager and send the closing acknowledgment to the server.
 *
 * @param pConnectionManager A pointer to the connection manager initialized in DriverOpen(). See connection_manager.c
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param id The id of the connection whose closing is requested.
 * @return SUCCESS or FAILURE
 */
int Handler_Ica_CloseRequest(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, pEPOLL_ARGS pEpollArgs,
                             ID id) {
    DEBUG(pFile, "Entering\n");
    CONNECTION_STATE state;

    if(SUCCESS != ConnectionManager_GetConnectionStateById(pConnectionManager, id, &state)){
        DEBUG(pFile, "ERROR : could not get the connection state\n");
        return (FAILURE);
    }

    if(state == CLOSING){
        DEBUG(pFile, "Connection already on CLOSING state, dropping the CloseRequest on this side\n");
        return (SUCCESS);
    }


    if(SUCCESS != CloseConnection(pConnectionManager, pEpollArgs, id)){
        //TODO : considerer chaque error code en cas d'echec
        DEBUG(pFile, "ERROR : could not close the connection.\n");
        return (FAILURE);
    }

    if(SUCCESS != Send_Ica_CloseAck(icasendArgs, id)){
        DEBUG(pFile, "ERROR : could not send or queue the CloseAck\n");
        return (FAILURE);
    }

    DEBUG(pFile, "Leaving\n");
    return (SUCCESS);
}

/**
 * Handles the Close Acknowledgment packet received from the Citrix server : checks that the connection #id was indeed in
 * CLOSING state and then closes it in the connection manager.
 *
 * @param pConnectionManager A pointer to the connection manager initialized in DriverOpen(). See connection_manager.c
 * @param id The id of the connection whose closing is acknowledged by the server side
 * @return SUCCESS or FAILURE
 */
int Handler_Ica_CloseAck(pCONNECTION_MANAGER pConnectionManager, pEPOLL_ARGS pEpollArgs, ID id) {
    DEBUG(pFile, "Entering\n");
    CONNECTION_STATE state;

    if(SUCCESS != ConnectionManager_GetConnectionStateById(pConnectionManager, id, &state)){
        DEBUG(pFile, "ERROR : could not get the connection state\n");
        return (FAILURE);
    }

    if(state != CLOSING){
        DEBUG(pFile, "ERROR : close ack received but connection not in closing state\n");
        return (FAILURE);
    }

    if(SUCCESS != ConnectionManager_CloseConnectionById(pConnectionManager, id)){
        //TODO : considerer chaque error code en cas d'echec
        DEBUG(pFile, "ERROR : could not close the connection.\n");
        return (FAILURE);
    }
    if(pConnectionManager->max_connection < pEpollArgs->maxevents){
        pEpollArgs->maxevents = pConnectionManager->max_connection;
        struct epoll_event * pEvents_tmp = NULL;
        if(NULL == (pEvents_tmp = (struct epoll_event *) reallocarray(pEpollArgs->pEvents, pEpollArgs->maxevents, sizeof(struct epoll_event)))) {
            DEBUG(pFile, "Memory reallocation error : pEvents\n");
            exit(1);
        }
        pEpollArgs->pEvents = pEvents_tmp;
    }

    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);

}



// ************************ INITIALISATION FUNCTIONS *****************************************
// *******************************************************************************************

/**
 * Creates the listening socket, sets its options, binds it to port, and puts it in listening state.
 *
 * @param port The port to listen to.
 * @return The listening socket just created or FAILURE
 */
int OpenListeningSocket(unsigned short port) {
    DEBUG(pFile, "Entering\n");
    DEBUG(pFile, "No listening socket, creating it...\n");

    int listening_socket;

    if ( FAILURE == (listening_socket = socket(AF_INET, SOCK_STREAM , 0))){
        DEBUG(pFile, "Error creating listening socket.\n");
        return(FAILURE);
    }
    int opt = 1;

    if (SUCCESS != setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,	sizeof(opt))){
        DEBUG(pFile,"Error setting listening socket options.\n");
        return (FAILURE);
    }

    if( SUCCESS != ioctl(listening_socket, FIONBIO, (char *)&opt) ){
        DEBUG(pFile,"Error iotctl().\n");
        return(FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);


    if( SUCCESS != bind(listening_socket, (struct sockaddr *) &address, sizeof(address))){
        DEBUG(pFile,"Error binding listening socket.\n");
        return (FAILURE);
    }

    if( SUCCESS != listen(listening_socket,10)){
        DEBUG(pFile,"Error putting socket to listening mode.\n");
        return(FAILURE);
    }

    DEBUG(pFile, "Leaving\n");
    return(listening_socket);
}

// ***************** PARSING FUNCTIONS ********************************************************
// ********************************************************************************************
// * These functions are used to parse the different packets received on the sockets or on the
// * virtual channel.
// ********************************************************************************************

// ---------- Socket --------------
/**
 * Parses the SOCKS version identifier/method selection packet received in pPacketBuf from the connecting client
 * and accordingly sets the values of the variables pointed by the different OUT pointers. See RFC 1928.
 *
 * @param pPacketBuf The buffer containing the received packet to parse.
 * @param pver The version of the SOCKS protocol (only version 5 is supported)
 * @param pnmethods The number of authentication methods supported by the connecting client.
 * @param ppmethods The array of authentication methods supported by the connecting client.
 */
void Parse_Socket_ClientMethodsPacket(IN unsigned char * pPacketBuf, OUT VER * pver, OUT NMETHODS * pnmethods, OUT unsigned char ** ppmethods ){
    DEBUG(pFile, "Entering\n");
    void * address = (void *)pPacketBuf;

    *pver = *((VER *)address);
    address += VER_SIZE;

    *pnmethods = *((NMETHODS *)address);
    address += NMETHODS_SIZE;

    *ppmethods = (unsigned char *)address;
    DEBUG(pFile, "Leaving\n");
}

/**
 * Parses the SOCKS request received in pPacketBuf from the connecting client and accordingly sets the values of the
 * variables pointed by the different OUT pointers. See RFC 1928.
 *
 * @param pPacketBuf The buffer containing the received packet to parse.
 * @param pver The version of the SOCKS protocol (only version 5 is supported)
 * @param pcmd The command (only CONNECT (0x01) is supported)
 * @param prsv Reserved
 * @param patyp The address type (0x01 for IPv4, 0x03 for DOMAIN NAME, 0x04 for IPv6 (not supported yet))
 * @param pipaddr The IPv4 address to connect to if atyp is 0x01. Else -1.
 * @param pport The port to connect to.
 * @param pdomlen The length of pdomdata if atyp is 0x03. Else -1.
 * @param ppdomdata The domain name to connect to if atyp is 0x03. Else NULL.
 */
void
Parse_Socket_ClientCmdPacket(unsigned char *pPacketBuf, VER *pver, CMD *pcmd, RSV *prsv, ATYP *patyp, IPADDR *pipaddr,
                             IP6ADDR *pip6addr, PORT *pport, DOMLEN *pdomlen, unsigned char **ppdomdata) {

    DEBUG(pFile, "Entering\n");
    void * address = (void *)pPacketBuf;

    *pver = *((VER *)address);
    address += VER_SIZE;


    *pcmd = *((CMD *)address);
    address += CMD_SIZE;


    *prsv = *((RSV *)address);
    address += RSV_SIZE;


    *patyp = *((ATYP *)address);
    address += ATYP_SIZE;


    if (*patyp == 0x01){

        *pipaddr = ntohl(*((IPADDR *)address));
        address += IPADDR_SIZE;


        *pport = ntohs(*((PORT *)address));
        address += PORT_SIZE;


        *pdomlen = -1;
        *ppdomdata = NULL;

        *pip6addr = NULL;
    }

    if(*patyp == 0x03){
        *pdomlen = *((DOMLEN *)address);
        address += DOMLEN_SIZE;

        *ppdomdata = (unsigned char *)address;
        address += *pdomlen;

        *pport = ntohs(*((PORT *)address));

        *pipaddr = -1;
        *pip6addr = NULL;
    }

    if(*patyp == 0x04){

        *pip6addr = (IP6ADDR)address;
        address += IP6ADDR_SIZE;

        *pport = ntohs(*((PORT *)address));
        address += PORT_SIZE;

        *pdomlen = -1;
        *ppdomdata = NULL;
        *pipaddr = -1;
    }
    DEBUG(pFile, "Leaving\n");

}


// ------------ Virtual channel ----------
/**
 * Parses the Window meta packet received in pPacketBuf from the Virtual Channel and sets the values of the variables
 * pointed by the different OUT pointers.
 *
 * @param pPacketBuf The buffer containing the received packet to parse.
 * @param pWindow The number of bytes to add to the window.
 */
void Parse_Ica_MetaWindow(IN const unsigned char * pPacketBuf, OUT int * pWindow){
    DEBUG(pFile, "Entering\n");
    *pWindow = *((int*)pPacketBuf);
    DEBUG_VAR(pFile, "Parsing : window=%d.\n", *pWindow);
    DEBUG(pFile, "Leaving\n");
}

/**
 * Parses the SOCKS Server Reply packet received in pPacketBuf from the Virtual Channel and sets the values of the variables
 * pointed by the different OUT pointers. See RFC 1928
 *
 * @param pPacketBuf The buffer containing the received packet to parse.
 * @param pver The version of the SOCKS protocol (only version 5 is supported)
 * @param prpl The reply REP (see RFC 1928) to the previously sent request.
 */
void Parse_Ica_ServerRplPacket(IN unsigned char * pPacketBuf, OUT VER * pver, OUT RPL * prpl){
    DEBUG(pFile, "Entering\n");
    void * address = (void *)pPacketBuf;

    *pver = *((VER*)address);
    address += VER_SIZE;

    *prpl = *((RPL*)address);
    DEBUG(pFile, "Leaving\n");
}

/**
 * Parses all the packets received from the Virtual Channel to determine their type, the id of the connection they correspond
 * to, and the length of their data buffer. It accordingly sets the values of the variables pointed by the different OUT
 * pointers. Then the other parsers are used depending on the packet type.
 *
 * @param pPacketBuf The buffer containing the received packet to parse.
 * @param ptype The type of the packet (see PACKET_TYPE_ENUM in common.h)
 * @param pid The id of the connection corresponding to the packet.
 * @param plen The length of the data contained in the packet.
 * @param ppdata A pointer to the beginning of the data in the packet.
 */
void Parse_Ica_GenericPacket(IN unsigned char * pPacketBuf, OUT PACKET_TYPE *ptype, OUT ID *pid, OUT LEN *plen, OUT unsigned char ** ppdata){
    DEBUG(pFile, "Entering\n");
    void * address = (void *)pPacketBuf;

    *ptype = *((PACKET_TYPE *)address);
    address += PACKET_TYPE_SIZE;

    *pid = ntohl(*((ID *)address));
    address += ID_SIZE;

    *plen = ntohs(*((LEN *)address));
    address += LEN_SIZE;

    *ppdata = (unsigned char *)address;
    DEBUG(pFile, "Leaving\n");

}


// **************** SOCKET SENDING FUNCTIONS *************************************
// ****************************************************************************************
/**
 * Queues or sends directly (if queue is empty) a SOCKS data packet to the fd socket.
 *
 * @param fd The socket's file descriptor.
 * @param pSocketQueue A pointer to the socket queue associated with the connection in the connection manager.
 * @param len The length of the pBuf buffer to send.
 * @param pBuf The buffer containing the data to send.
 * @param pBytesSent A pointer to an int variable filled with the number of bytes actually sent.
 * @return SUCCESS if the packet was successfully sent or queued or half sent half queued,ERROR_CLOSE_CONNECTION if an error
 * occurred that just requires the connection to be closed, ERROR_FATAL if an error occurred that requires the program to
 * stop, or FAILURE if the packet was not or only partially sent/queued but no further action is required.
 */
int Send_Socket_DataPacket(int fd, pSOCKET_QUEUE pSocketQueue, LEN len, unsigned char *pBuf, int *pBytesSent) {
    DEBUG(pFile, "Entering\n");
    int bytesSent1 = 0;
    int bytesSent2 = 0;

    Send_SocketQueue(fd, pSocketQueue, &bytesSent1);
    int rc;
    rc = Send_Socket_Queueing(fd, pSocketQueue, len, pBuf, &bytesSent2);

    DEBUG(pFile, "Leaving\n");
    if(pBytesSent){
        *pBytesSent = bytesSent2 + bytesSent1;
    }
    return (rc);
}

/**
 * Queues or sends directly (if queue is empty) a SOCKS Method selection packet to the fd socket.
 *
 * @param fd The socket's file descriptor.
 * @param ver The version of the SOCKS protocol (only version 5 is supported)
 * @param method The SOCKS authentication method chosen by the server
 * @param pSocketQueue A pointer to the socket queue associated with the connection in the connection manager.
 * @param pBytesSent A pointer to an int variable filled with the number of bytes actually sent.
 * @return SUCCESS if the packet was successfully sent or queued or half sent half queued,ERROR_CLOSE_CONNECTION if an error
 * occurred that just requires the connection to be closed, ERROR_FATAL if an error occurred that requires the program to
 * stop, or FAILURE if the packet was not or only partially sent/queued but no further action is required.
 */
int Send_Socket_MethodSelectionPacket(int fd, VER ver, METHOD method, pSOCKET_QUEUE pSocketQueue, int *pBytesSent) {
    DEBUG(pFile, "Entering\n");
    unsigned char pSocketServerMethodBuf[VER_SIZE+METHOD_SIZE] = {}; //ici ou au debut ??
    void * dest = (void *)pSocketServerMethodBuf;
    ssize_t n;
    int bytesSent1 = 0;
    int bytesSent2 = 0;

    memcpy(dest, &ver, VER_SIZE);
    dest += VER_SIZE;

    memcpy(dest, &method, METHOD_SIZE);

    DEBUG_VAR(pFile, "Sending to socket %d : ", fd);
    DEBUG_BUF(pSocketServerMethodBuf, sizeof(pSocketServerMethodBuf));

    Send_SocketQueue(fd, pSocketQueue, &bytesSent1);
    int rc;
    rc = Send_Socket_Queueing(fd, pSocketQueue, sizeof(pSocketServerMethodBuf), pSocketServerMethodBuf, &bytesSent2);

    DEBUG(pFile, "Leaving\n");
    if(pBytesSent){
        *pBytesSent = bytesSent2 + bytesSent1;
    }
    return(rc);
}

/**
 * Queues or sends directly (if queue is empty) a SOCKS Reply packet to the fd socket.
 *
 * @param fd The socket's file descriptor.
 * @param ver The version of the SOCKS protocol (only version 5 is supported)
 * @param rpl The SOCKS reply code.
 * @param bndAddr The SOCKS bound address.
 * @param bndPort The SOCKS bound port.
 * @param pSocketQueue A pointer to the socket queue associated with the connection in the connection manager.
 * @param pBytesSent A pointer to an int variable filled with the number of bytes actually sent.
 * @return SUCCESS if the packet was successfully sent or queued or half sent half queued,ERROR_CLOSE_CONNECTION if an error
 * occurred that just requires the connection to be closed, ERROR_FATAL if an error occurred that requires the program to
 * stop, or FAILURE if the packet was not or only partially sent/queued but no further action is required.
 */
int Send_Socket_RplPacket(int fd, VER ver, RPL rpl, IPADDR bndAddr, PORT bndPort, pSOCKET_QUEUE pSocketQueue,
                          int *pBytesSent) {
    DEBUG(pFile, "Entering\n");
    unsigned char pSocketRplBuf[VER_SIZE + RPL_SIZE + RSV_SIZE + ATYP_SIZE + IPADDR_SIZE + PORT_SIZE];
    void * dest = (void *)pSocketRplBuf;
    ssize_t n;
    RSV rsv = 0x00;
    ATYP aTyp = 0x01;
    int bytesSent1 = 0;
    int bytesSent2 = 0;

    memcpy(dest, &ver , VER_SIZE);
    dest += VER_SIZE;

    memcpy(dest, &rpl ,RPL_SIZE );
    dest += RPL_SIZE;

    memcpy(dest, &rsv ,RSV_SIZE );
    dest += RSV_SIZE;

    memcpy(dest, &aTyp ,ATYP_SIZE );
    dest += ATYP_SIZE;

    memcpy(dest, &bndAddr ,IPADDR_SIZE );
    dest += IPADDR_SIZE;

    memcpy(dest, &bndPort ,PORT_SIZE );
    dest += PORT_SIZE;

    DEBUG_VAR(pFile, "Sending to socket %d : ", fd);
    DEBUG_BUF(pSocketRplBuf, sizeof(pSocketRplBuf));

    Send_SocketQueue(fd, pSocketQueue, &bytesSent1);
    int rc;
    rc = Send_Socket_Queueing(fd, pSocketQueue, sizeof(pSocketRplBuf), pSocketRplBuf, &bytesSent2);

    if(pBytesSent){
        *pBytesSent = bytesSent2 + bytesSent1;
    }
    DEBUG(pFile, "Leaving\n");
    return(rc);

}

/**
 * Tries to send the bytes in the pSocketQueue to the socket fd.
 *
 * @param fd The socket's file descriptor.
 * @param pSocketQueue A pointer to the socket queue to send.
 * @param pBytesSent A pointer to an int variable filled with the number of bytes actually sent.
 * @return SUCCESS if all the queue is successfully sent, FAILURE if only a part of the queue is sent but no further
 * action is required, ERROR_CLOSE_CONNECTION if an error occurred that just requires the connection to be closed,
 * ERROR_FATAL if an error occurred that requires the program to stop.
 */
int Send_SocketQueue(int fd, pSOCKET_QUEUE pSocketQueue, int *pBytesSent) {
    DEBUG(pFile, "Entering\n");
    unsigned char * pBuf = NULL;
    int n = MIN(pSocketQueue->size, 100);
    int rc;
    int len;
    int bytesSent = 0;
    int totalSent = 0;

    while (!SocketQueue_IsEmpty(pSocketQueue)){
        len = SocketQueue_GetN(pSocketQueue, n, &pBuf);
        if( SUCCESS != (rc= Send_Socket(fd, len, pBuf, &bytesSent))){
            if(pBytesSent){
                *pBytesSent = totalSent;
            }
            return (rc);
        }
        totalSent += bytesSent;
        SocketQueue_Dequeue(pSocketQueue, bytesSent);

    }

    DEBUG(pFile, "Leaving\n");
    if(pBytesSent){
        *pBytesSent = totalSent;
    }
    return (SUCCESS);
}

/**
 * If pQueue is empty, directly tries to send len bytes of pBuf to fd. Otherwise, enqueues those bytes in pQueue.
 *
 * @param fd The socket's file descriptor.
 * @param pSocketQueue A pointer to the socket queue associated with the connection in the connection manager.
 * @param len The length of the pBuf buffer to send.
 * @param pBuf The buffer containing the data to send.
 * @param pBytesSent A pointer to an int variable filled with the number of bytes actually sent.
 * @return SUCCESS if the packet was successfully sent or queued or half sent half queued,ERROR_CLOSE_CONNECTION if an error
 * occurred that just requires the connection to be closed, ERROR_FATAL if an error occurred that requires the program to
 * stop, or FAILURE if the packet was not or only partially sent/queued but no further action is required.
 */
int Send_Socket_Queueing(int fd, pSOCKET_QUEUE pQueue, LEN len, unsigned char *pBuf, OUT int *pBytesSent) {
    DEBUG(pFile, "Entering\n")
    int bytesSent=0;
    if(SocketQueue_IsEmpty(pQueue)){
        DEBUG(pFile, "Queue is empty, directly sending the packet.\n");
        int rc;
        rc= Send_Socket(fd, len, pBuf, &bytesSent);
        if(rc==ERROR_CLOSE_CONNECTION){
            return (ERROR_CLOSE_CONNECTION);
        }
        if(rc==ERROR_FATAL){
            return (ERROR_FATAL);
        }
        if(rc == SUCCESS){
            if(bytesSent != len){
                DEBUG(pFile, "Packet partially sent, queueing the rest\n");
                if(SUCCESS == SocketQueue_Enqueue(pQueue, pBuf+bytesSent, len-bytesSent)){
                    DEBUG(pFile, "Rest successfully queued\n");
                    if(pBytesSent){
                        *pBytesSent = bytesSent;
                    }
                    return (SUCCESS);
                }
                DEBUG(pFile, "Error queueing the rest\n");
                if(pBytesSent){
                    *pBytesSent = bytesSent;
                }
                return(FAILURE);
            }
            DEBUG(pFile, "Packet entirely sent\n");
            if(pBytesSent){
                *pBytesSent = bytesSent;
            }
            return (SUCCESS);
        }
    }


    DEBUG(pFile, "Queue is not empty or Send_Socket failed, queueing the packet\n");
    if(pBytesSent){
        *pBytesSent = 0;
    }

    if(SUCCESS == SocketQueue_Enqueue(pQueue, pBuf, len)){
        DEBUG(pFile, "Packet queued");
        return (SUCCESS);
    }


    DEBUG(pFile, "Leaving\n");
    return (FAILURE);
}

/**
 * Tries to send len bytes of pBuf to the socket fd and catch errors.
 *
 * @param fd The socket's file descriptor.
 * @param len The length of the pBuf buffer to send.
 * @param pBuf The buffer containing the data to send.
 * @param pBytesSent A pointer to an int variable filled with the number of bytes actually sent.
 * @return SUCCESS if no error is returned by send(),ERROR_CLOSE_CONNECTION if an error occurred that just requires the
 * connection to be closed, ERROR_FATAL if an error occurred that requires the program to stop, or FAILURE if the packet
 * just could not be sent but no further action is required.
 */
int Send_Socket(int fd, LEN len, unsigned char *pBuf, int *pBytesSent) {
    DEBUG(pFile, "Entering\n");
    ssize_t rc;
    if(SOCKET_ERROR == (rc = send(fd, (void *)pBuf, (ssize_t)len, 0))){
        switch (errno) {
            case EACCES:
                DEBUG(pFile, "EACCES\n");
                return(ERROR_CLOSE_CONNECTION);
            case EAGAIN:
                DEBUG(pFile, "EAGAIN\n");
                return(FAILURE);
       //     case EWOULDBLOCK:
         //       break;
            case EBADF:
                DEBUG(pFile, "EBADF\n");
                return(ERROR_CLOSE_CONNECTION);
            case ECONNRESET:
                DEBUG(pFile, "ECONNRESET\n");
                return(ERROR_CLOSE_CONNECTION);
            case EDESTADDRREQ:
                DEBUG(pFile, "EDESTADDRREQ\n");
                return(ERROR_CLOSE_CONNECTION);
            case EFAULT:
                DEBUG(pFile, "EFAULT\n");
                return (ERROR_FATAL);
            case EINTR:
                DEBUG(pFile, "EINTR\n");
                return(FAILURE);
            case EINVAL:
                DEBUG(pFile, "EINVAL\n");
                return(ERROR_CLOSE_CONNECTION);
            case EISCONN:
                DEBUG(pFile, "EISCONN\n");
                return (ERROR_FATAL);
            case EMSGSIZE:
                DEBUG(pFile, "EMSGSIZE\n");
                return (ERROR_FATAL);
            case ENOBUFS:
                DEBUG(pFile, "ENOBUFS\n");
                return(FAILURE);
            case ENOMEM:
                DEBUG(pFile, "ENOMEM\n");
                return(FAILURE);
            case ENOTCONN:
                DEBUG(pFile, "ENOTCONN\n");
                return(ERROR_CLOSE_CONNECTION);
            case ENOTSOCK:
                DEBUG(pFile, "ENOTSOCK\n");
                return(ERROR_CLOSE_CONNECTION);
            case EOPNOTSUPP:
                DEBUG(pFile, "EOPNOTSUPP\n");
                return (ERROR_FATAL);
            case EPIPE:
                DEBUG(pFile, "EPIPE\n");
                return(ERROR_CLOSE_CONNECTION);

        }
    }

    if(pBytesSent){
        *pBytesSent = (int)rc;
    }
    DEBUG_VAR(pFile, "Successful send : %d bytes\n", (int)rc);
    DEBUG(pFile, "Leaving\n");
    return (SUCCESS);
}


//******************** ICA SENDING FUNCTIONS *******************************

/**
 * Queues or sends directly (if the queue is empty) a data packet to the Virtual Channel. Used to transfer
 * data from sockets to the server side once the SOCKS connection is established.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param id The id of the connection sending the packet.
 * @param len The length of the data in pData.
 * @param pData A buffer with the contents of the data packet.
 * @return SUCCESS if the packet was successfully sent or queued. FAILURE otherwise.
 */
int Send_Ica_DataPacket(ICASEND_ARGS icasendArgs, ID id, LEN len, unsigned char *pData) {
    DEBUG(pFile, "Entering\n");
    int rc;
    rc = Send_Ica_GenericPacket(icasendArgs, (PACKET_TYPE) SOCKS_DATA, id, len, pData);
    DEBUG(pFile, "Leaving\n");
    return(rc);
}

/**
 * Queues or sends directly (if queue is empty) a SOCKS request packet to connect to the given IPv4 and port.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel * @param id The id of the connection sending the packet.
 * @param id The id of the connection sending the packet.
 * @param ver The SOCKS protocol version (only SOCKS5 is supported).
 * @param cmd The SOCKS command (only CONNECT (0x01) is supported).
 * @param ipAddr The IPv4 address we want to connect to.
 * @param port The remote port we want to connect to.
 * @return SUCCESS if the packet is successfully sent or queued. FAILURE otherwise.
 */
int Send_Ica_IpCmdPacket(ICASEND_ARGS icasendArgs, ID id, VER ver, CMD cmd, IPADDR ipAddr, PORT port) {
    DEBUG(pFile, "Entering\n");

    
    unsigned char pClientIpCmdPacketBuf[VER_SIZE+CMD_SIZE+IPADDR_SIZE+PORT_SIZE] = {};
    int rc;
    void * dest = (void *)pClientIpCmdPacketBuf;
    IPADDR netOrderIpAddr = htonl(ipAddr);
    PORT netOrderPort = htons(port);

    memcpy(dest, &ver, VER_SIZE);
    dest += VER_SIZE;

    memcpy(dest, &cmd, CMD_SIZE);
    dest += CMD_SIZE;

    memcpy(dest, &netOrderIpAddr, IPADDR_SIZE);
    dest += IPADDR_SIZE;

    memcpy(dest, &netOrderPort, PORT_SIZE);


    DEBUG_BUF(pClientIpCmdPacketBuf, sizeof(pClientIpCmdPacketBuf))
    DEBUG(pFile, "\n");
    

    rc = Send_Ica_GenericPacket(icasendArgs, (PACKET_TYPE) SOCKS_CLIENT_IP_CMD, id, sizeof(pClientIpCmdPacketBuf),
                                pClientIpCmdPacketBuf);
    DEBUG(pFile, "Leaving\n");
    return(rc);
}

/**
 * Queues or sends directly (if queue is empty) a SOCKS request packet to connect to the given domain name and port.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param id The id of the connection sending the packet.
 * @param ver The SOCKS protocol version (only SOCKS5 is supported).
 * @param cmd The SOCKS command (only CONNECT (0x01) is supported).
 * @param port The remote port we want to connect to.
 * @param domLen The length of the domain name string in pDomData.
 * @param pDomData The buffer containing the domain name.
 * @return SUCCESS if the packet is successfully sent or queued. FAILURE otherwise.
 */
int
Send_Ica_DomainCmdPacket(ICASEND_ARGS icasendArgs, ID id, VER ver, CMD cmd, PORT port, DOMLEN domLen, DOMSTR pDomData) {
    DEBUG(pFile, "Entering\n");

    unsigned char pClientDomainCmdPacketBuf[VER_SIZE+CMD_SIZE+PORT_SIZE+DOMLEN_SIZE+MAX_DOMSTR_SIZE] = {};
    int rc;
    void * dest = (void *)pClientDomainCmdPacketBuf;

    PORT netOrderPort = htons(port);

    memcpy(dest, &ver, VER_SIZE);
    dest += VER_SIZE;

    memcpy(dest, &cmd, CMD_SIZE);
    dest += CMD_SIZE;

    memcpy(dest, &netOrderPort, PORT_SIZE);
    dest += PORT_SIZE;

    memcpy(dest, &domLen, DOMLEN_SIZE);
    dest += DOMLEN_SIZE;

    memcpy(dest, pDomData, domLen);



    DEBUG_BUF(pClientDomainCmdPacketBuf, VER_SIZE+CMD_SIZE+PORT_SIZE+DOMLEN_SIZE+domLen)
    DEBUG(pFile, "\n");


    rc = Send_Ica_GenericPacket(icasendArgs, (PACKET_TYPE) SOCKS_CLIENT_DOMAIN_CMD, id,
                                VER_SIZE + CMD_SIZE + PORT_SIZE + DOMLEN_SIZE + domLen,
                                pClientDomainCmdPacketBuf);
    DEBUG(pFile, "Leaving\n");
    return(rc);
}


int
Send_Ica_Ip6CmdPacket(ICASEND_ARGS icasendArgs, ID id, VER ver, CMD cmd, PORT port, IP6ADDR ip6Addr) {
    DEBUG(pFile, "Entering\n");

    unsigned char pClientIp6CmdPacketBuf[VER_SIZE+CMD_SIZE+PORT_SIZE+IP6ADDR_SIZE] = {};
    int rc;
    void * dest = (void *)pClientIp6CmdPacketBuf;

    PORT netOrderPort = htons(port);

    memcpy(dest, &ver, VER_SIZE);
    dest += VER_SIZE;

    memcpy(dest, &cmd, CMD_SIZE);
    dest += CMD_SIZE;

    memcpy(dest, &netOrderPort, PORT_SIZE);
    dest += PORT_SIZE;

    memcpy(dest, ip6Addr, IP6ADDR_SIZE);



    DEBUG_BUF(pClientIp6CmdPacketBuf, VER_SIZE+CMD_SIZE+PORT_SIZE+IP6ADDR_SIZE)
    DEBUG(pFile, "\n");


    rc = Send_Ica_GenericPacket(icasendArgs, (PACKET_TYPE) SOCKS_CLIENT_IP6_CMD, id,
                                VER_SIZE+CMD_SIZE+PORT_SIZE+IP6ADDR_SIZE,
                                pClientIp6CmdPacketBuf);
    DEBUG(pFile, "Leaving\n");
    return(rc);
}

/**
 * Queues or sends directly (if queue is empty) a connection closing request to the Virtual Channel.
 * The server side will then try to close the connection, send back a close acknowledgement and then the connection will
 * be closed here on the client side.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param id The id of the connection we request to close.
 * @return SUCCESS if the packet was successfully sent or queued. FAILURE otherwise.
 */
int Send_Ica_CloseRequest(ICASEND_ARGS icasendArgs, ID id) {
    DEBUG(pFile, "Entering\n");
    if(SUCCESS != Send_Ica_GenericPacket(icasendArgs, CLOSE_REQUEST, id, 0, NULL)){
        DEBUG(pFile, "ERROR : could not send or queue the close request\n");
        return(FAILURE);
    }
    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 * Queues or sends directly (if queue is empty) a connection closing acknowledgement packet to the Virtual Channel.
 * These packets are sent in response to a Close Request packet and after having successfully closed the given connection.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param id The id of the connection whose closing is acknowledged.
 * @return SUCCESS if the packet was successfully sent or queued. FAILURE otherwise.
 */
int Send_Ica_CloseAck(ICASEND_ARGS icasendArgs, ID id) {
    DEBUG(pFile, "Entering\n");
    if(SUCCESS != Send_Ica_GenericPacket(icasendArgs, CLOSE_ACK, id, 0, NULL)){
        DEBUG(pFile, "ERROR : could not send or queue the close acknowledgement\n");
        return(FAILURE);
    }
    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 * Sends or queues in the head of the queue (if sending fails) a packet to acknowledge the transmission of "window" bytes
 * to the socket linked with connection "id".
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param window The number of bytes whose successful transmission is acknowledged.
 * @param id The id of the connection concerned.
 * @return SUCCESS if the packet was successfully sent or queued. FAILURE otherwise.
 */
int Send_Ica_MetaWindow(ICASEND_ARGS icasendArgs, int window, ID id) {
    DEBUG(pFile, "Entering\n");

    if(SUCCESS != Send_Ica_Meta(icasendArgs, META_WINDOW, sizeof(int), (unsigned char *) &window, id)){
        DEBUG(pFile, "ERROR : could not send or queue the window packet\n");
        return(FAILURE);
    }
    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 * Queues or sends directly (if queue is empty) any type of packet to the Virtual Channel.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param type The type of the packet to send. See the PACKET_TYPE enumeration.
 * @param id The id of the connection sending the packet.
 * @param len The length of the data in the pData buffer.
 * @param pData The buffer with the packet's contents.
 * @return SUCCESS if the packet was successfully sent or queued. FAILURE otherwise.
 */
int Send_Ica_GenericPacket(ICASEND_ARGS icasendArgs, PACKET_TYPE type, ID id, LEN len, unsigned char *pData) {
    DEBUG(pFile, "Entering\n");

    unsigned char pIcaHeaderBuf[PACKET_TYPE_SIZE+ID_SIZE+LEN_SIZE] = {}; // ici ou dans DriverOpen() avec un malloc ??
    MEMORY_SECTION pMS[2];
    int nSections = 1;
    int rc;
    void * dest = (void *)pIcaHeaderBuf;
    ID netOrderId = htonl(id);
    LEN netOrderLen = htons(len);

    memcpy(dest, &type, PACKET_TYPE_SIZE);
    dest += PACKET_TYPE_SIZE;

    memcpy(dest, &netOrderId, ID_SIZE);
    dest += ID_SIZE;

    memcpy(dest, &netOrderLen, LEN_SIZE);

    pMS[0].pSection = pIcaHeaderBuf;
    pMS[0].length = sizeof(pIcaHeaderBuf);

    DEBUG_BUF(pIcaHeaderBuf, sizeof(pIcaHeaderBuf));




    if((len > 0) && (pData != NULL)){
        pMS[1].pSection = pData;
        pMS[1].length = len;

        nSections = 2;

        DEBUG_BUF(pData, sizeof(pData));

    }

    if(SUCCESS != Send_IcaQueue(icasendArgs)){
        return(FAILURE);
    }

    if(SUCCESS != Send_Ica_Queueing(icasendArgs, pMS, nSections)){
        DEBUG(pFile, "FATAL ERROR : could not send or queue the packet \n");
        return (FAILURE);
    }

    DEBUG(pFile, "Leaving\n");
    return (SUCCESS);

}

/**
 * Sends or queues in the head of the queue (if sending fails) any type of packet to the Virtual Channel.
 * Used for metadata packets that do not need to respect the same order of arrival as data packets.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param type The type of the packet to send. See the PACKET_TYPE enumeration.
 * @param id The id of the connection sending the packet.
 * @param len The length of the data in the pData buffer.
 * @param pData The buffer with the packet's contents.
 * @return SUCCESS if the packet was successfully sent or queued. FAILURE otherwise.
 */
int Send_Ica_Meta(ICASEND_ARGS icasendArgs, PACKET_TYPE type, LEN len, unsigned char *pData, ID id) {
    DEBUG(pFile, "Entering\n");

    unsigned char pIcaHeaderBuf[PACKET_TYPE_SIZE+ID_SIZE+LEN_SIZE] = {}; // ici ou dans DriverOpen() avec un malloc ??
    MEMORY_SECTION pMS[2];
    int nSections = 1;
    int rc;
    void * dest = (void *)pIcaHeaderBuf;
    ID netOrderId = htonl(id);
    LEN netOrderLen = htons(len);

    memcpy(dest, &type, PACKET_TYPE_SIZE);
    dest += PACKET_TYPE_SIZE;

    memcpy(dest, &netOrderId, ID_SIZE);
    dest += ID_SIZE;

    memcpy(dest, &netOrderLen, LEN_SIZE);

    pMS[0].pSection = pIcaHeaderBuf;
    pMS[0].length = sizeof(pIcaHeaderBuf);

    DEBUG_BUF(pIcaHeaderBuf, sizeof(pIcaHeaderBuf));




    if((len > 0) && (pData != NULL)){
        pMS[1].pSection = pData;
        pMS[1].length = len;

        nSections = 2;

        DEBUG_BUF(pData, sizeof(pData));

    }




    if(SUCCESS != Send_Ica_QueueingPriority(icasendArgs, pMS, 2)){
        DEBUG(pFile, "Error Send_Ica_QueueingPriority()\n");
        return(FAILURE);
    }
    DEBUG(pFile, "Leaving\n");
    return (SUCCESS);

}

/**
 * Tries to send the packets in the virtual channel queue.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @return SUCCESS except if a packet is sent but not dequeued (ERROR_FATAL in this case).
 */
int Send_IcaQueue(ICASEND_ARGS icasendArgs) {
    DEBUG(pFile,"Entering\n");
    while(!IcaPacketQueue_IsEmpty(icasendArgs.pIcaPacketQueue)){
        MEMORY_SECTION * pMS = IcaPacketQueue_GetHead(icasendArgs.pIcaPacketQueue);

        if(SUCCESS != Send_Ica(icasendArgs.vcInfo,pMS, 1)){
            DEBUG(pFile, "Error when trying to send")
            return(SUCCESS); //the only failure reason is if a packet is sent but not dequeued
        }

        if(SUCCESS != IcaPacketQueue_Dequeue(icasendArgs.pIcaPacketQueue)){
            DEBUG(pFile, "FATAL ERROR : message was sent but not dequeued\n");
            return (ERROR_FATAL);
        }
    }
    DEBUG(pFile, "Leaving\n");
    return(SUCCESS);
}

/**
 **If the virtual channel queue is empty, directly tries to send the nMS MEMORY_SECTION's of pMS. Otherwise, it enqueues
 * them in the head of the queue.
 *
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param pMS An array of MEMORY_SECTION's to send.(see Citrix VDSDK documentation)
 * @param nMS The number of MEMORY_SECTION's in pMS.(see Citrix VDSDK documentation)
 * @return SUCCESS if the packet was successfully sent or queued, FAILURE otherwise.
 */
int Send_Ica_QueueingPriority(ICASEND_ARGS icasendArgs, MEMORY_SECTION *pMS, unsigned char nMS) {
    DEBUG(pFile,"Entering\n");

    if(SUCCESS == Send_Ica(icasendArgs.vcInfo, pMS, nMS)){
        DEBUG(pFile, "Packet sent.\n");
        DEBUG(pFile, "Leaving\n");
        return (SUCCESS);
    }

    DEBUG(pFile, "Send_Ica failed, queueing the packet in the head of the queue\n");
    if(SUCCESS == IcaPacketQueue_EnqueueHead(icasendArgs.pIcaPacketQueue, pMS, nMS)){
        DEBUG(pFile,"Packet queued\n");
        DEBUG(pFile, "Leaving\n");
        return (SUCCESS);
    }
    DEBUG(pFile, "Leaving\n");
    return(FAILURE);
}

/**
 * If the virtual channel queue is empty, directly tries to send the nMS MEMORY_SECTION's of pMS. Otherwise, it enqueues
 * them.
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param pMS An array of MEMORY_SECTION's to send.(see Citrix VDSDK documentation)
 * @param nMS The number of MEMORY_SECTION's in pMS.(see Citrix VDSDK documentation)
 * @return SUCCESS if the packet was successfully sent or queued, FAILURE otherwise.
 */
int Send_Ica_Queueing(ICASEND_ARGS icasendArgs, MEMORY_SECTION *pMS, unsigned char nMS) {
    DEBUG(pFile,"Entering\n");
    if(IcaPacketQueue_IsEmpty(icasendArgs.pIcaPacketQueue)){
        DEBUG(pFile, "Queue is empty, directly sending the packet\n");
        if(SUCCESS == Send_Ica(icasendArgs.vcInfo, pMS, nMS)){
            DEBUG(pFile, "Packet sent.\n");
            DEBUG(pFile, "Leaving\n");
            return (SUCCESS);
        }
    }
    DEBUG(pFile, "Queue is not empty, or Send_Ica failed, queueing the packet\n");
    if(SUCCESS == IcaPacketQueue_Enqueue(icasendArgs.pIcaPacketQueue, pMS, nMS)){ //FIXME (a virer), on y passe jamais
        DEBUG(pFile,"Packet queued\n");
        DEBUG(pFile, "Leaving\n");
        return (SUCCESS);
    }
    DEBUG(pFile, "Leaving\n");
    return(FAILURE);
}

/**
 * Tries to send the nMS MEMORY_SECTION's contained in pMS to the Virtual Channel.
 *
 * @param vcInfo A structure containing information about the virtual channel and required to call QueueVirtualWrite()
 * (see Citrix VDSDK documentation)
 * @param pMS An array of MEMORY_SECTION's to send.(see Citrix VDSDK documentation)
 * @param nMS The number of MEMORY_SECTION's in pMS.(see Citrix VDSDK documentation)
 * @return SUCCESS if the the packet was queued by the citrix client, or FAILURE if not.
 */
int Send_Ica(VC_INFO vcInfo, MEMORY_SECTION *pMS, unsigned char nMS) {
    DEBUG(pFile, "Entering\n");

    if(CLIENT_STATUS_SUCCESS != QueueVirtualWrite(vcInfo.pWd, vcInfo.vcNum, pMS, nMS, FLUSH_IMMEDIATELY )){ //the function returns CLIENT_STATUS_SUCCESS or CLIENT_ERROR_NO_OUT_BUF, and no additional error code is set
        DEBUG(pFile, "Error QueueVirtualWrite() : packet was not queued.\n");
        DEBUG(pFile, "Error QueueVirtualWrite() : packet was not queued.\n");
        return(FAILURE);
    }

    DEBUG(pFile, "Leaving\n");
    return (SUCCESS);
}


// **************************************************************************

/**
 * Used to start a closing procedure, initiated by the citrix client side (here).
 * Directly closes a connection or sends a close request depending on the connection state.
 *
 * @param pConnectionManager A pointer to the connection manager initialized in DriverOpen(). See connection_manager.c
 * @param icasendArgs A ICASEND_ARGS structure containing all the required arguments to send data through the virtual channel
 * @param id The id of the connection.
 * @param state The state of the connection.
 * @return SUCCESS if the connection was successfully closed or if the closing request was successfully sent. FAILURE otherwise.
 */
int InitiateCloseProcedure(pCONNECTION_MANAGER pConnectionManager, ICASEND_ARGS icasendArgs, ID id,
                           pEPOLL_ARGS pEpollArgs) {
    pCONNECTION pConnection = NULL;
    if(NULL == (pConnection= ConnectionManager_GetConnectionById(pConnectionManager, id))){
        DEBUG(pFile, "Error getting connection\n");
        return(FAILURE);
    }
    if(pConnection->state == CLOSING){
        DEBUG(pFile, "Connection already in CLOSING state, this should not happen\n");
        return (FAILURE);
    }

    if(pConnection->state == SOCKET_OPEN || pConnection->state == METHOD_CHOSEN){
        if(EVT_SUCCESS != Evt_remove_triggers(pConnection->socket_fd)){
            DEBUG(pFile, "Error removing triggers\n")
            DEBUG(pErrorFile, "Error removing triggers\n")
            return(FAILURE);
        }
        if(SUCCESS != epoll_ctl(pEpollArgs->epollfd, EPOLL_CTL_DEL, pConnection->socket_fd, NULL)){
            DEBUG(pFile, "Error epoll ctl del\n")
            DEBUG(pErrorFile, "Error epoll ctl del\n")
            return(FAILURE);
        }



        if(SUCCESS != ConnectionManager_CloseConnectionById(pConnectionManager, id)){
            DEBUG(pFile, "ERROR CloseConnectionById()\n");
            return (FAILURE);
        }

        if(pConnectionManager->max_connection < pEpollArgs->maxevents){
            pEpollArgs->maxevents = pConnectionManager->max_connection;
            struct epoll_event * pEvents_tmp = NULL;
            if(NULL == (pEvents_tmp = (struct epoll_event *) reallocarray(pEpollArgs->pEvents, pEpollArgs->maxevents, sizeof(struct epoll_event)))) {
                DEBUG(pFile, "Memory reallocation error : pEvents\n");
                exit(1);
            }
            pEpollArgs->pEvents = pEvents_tmp;
        }

        return (SUCCESS);
    }
    if(pConnection->state == ESTABLISHED || pConnection->state == CMD_SENT){
        DEBUG(pFile, "Starting connection closing\n");
        if (SUCCESS != Send_Ica_CloseRequest(icasendArgs, id)) {
            DEBUG(pFile, "FATAL ERROR Send_Ica_CloseRequest()\n");
            return (FAILURE);
        }

        if(EVT_SUCCESS != Evt_remove_triggers(pConnection->socket_fd)){
            DEBUG(pFile, "Error removing triggers\n")
            DEBUG(pUnsentFile, "Error removing triggers\n")
            return(FAILURE);
        }
        if(SUCCESS != epoll_ctl(pEpollArgs->epollfd, EPOLL_CTL_DEL, pConnection->socket_fd, NULL)){
            DEBUG(pFile, "Error epoll ctl del\n")
            DEBUG(pErrorFile, "Error epoll ctl del\n")
            return(FAILURE);
        }

        if (SUCCESS != ConnectionManager_SetConnectionStateById(pConnectionManager, id, CLOSING)) {
            DEBUG(pFile, "FATAL ERROR ConnectionManager_SetConnectionStateById\n");
            return (FAILURE);
        }
        return (SUCCESS);
    }

}

int CloseConnection(pCONNECTION_MANAGER pConnectionManager, pEPOLL_ARGS pEpollArgs, ID id) {
    pCONNECTION pConnection = NULL;
    if(NULL == (pConnection= ConnectionManager_GetConnectionById(pConnectionManager, id))){
        DEBUG(pFile, "Error getting connection\n");
        return(FAILURE);
    }

    if(pConnection->state != CLOSING){ //if connection is in CLOSING state, Evt_remove and epoll_ctl have already been executed in Initiate_close_procedure

        if(EVT_SUCCESS != Evt_remove_triggers(pConnection->socket_fd)){
            DEBUG(pFile, "Error removing triggers\n")
            DEBUG(pErrorFile, "Error removing triggers\n")
            return(FAILURE);
        }
        if(SUCCESS != epoll_ctl(pEpollArgs->epollfd, EPOLL_CTL_DEL, pConnection->socket_fd, NULL)){
            DEBUG_VAR(pFile, "Error epoll ctl del, errno = %d, %s\n", errno, strerror(errno))
            DEBUG(pErrorFile, "Error epoll ctl del\n")
            return(FAILURE);
        }
    }

    if(SUCCESS != ConnectionManager_CloseConnectionById(pConnectionManager, id)){
        DEBUG(pFile, "ERROR CloseConnectionById()\n");
        return (FAILURE);
    }
    if(pConnectionManager->max_connection < pEpollArgs->maxevents){
        pEpollArgs->maxevents = pConnectionManager->max_connection;
        struct epoll_event * pEvents_tmp = NULL;
        if(NULL == (pEvents_tmp = (struct epoll_event *) reallocarray(pEpollArgs->pEvents, pEpollArgs->maxevents, sizeof(struct epoll_event)))) {
            DEBUG(pFile, "Memory reallocation error : pEvents\n");
            exit(1);
        }
        pEpollArgs->pEvents = pEvents_tmp;
    }
    return (SUCCESS);

}


// **************** DEBUG AND LOGGING FUNCTIONS *****************************

void debugBuf(void * buf, int len){
#ifdef DEBUG_ON
    for(int i=0; i<len; i++){
        fprintf(pFile, "%x ", ((unsigned char *)buf)[i]);
        fflush(pFile);

    }
    fprintf(pFile, "\n");
    fflush(pFile);
#endif
}

void debug_info(FILE * fd){
    fprintf(fd,"---DEBUG--- \n");
    fflush(fd);
    pCONNECTION_MANAGER pConnectionManager = g_pVdCallbackArgs->driverCloseArgs.pConnectionManager;

    if(pConnectionManager == NULL){
        fprintf(fd, "g_pConnectionManager is NULL\n");
        fflush(fd);
        return;
    }

    int max_connections = pConnectionManager->max_connection;
    int max_id = pConnectionManager->max_id;
    int nConnection = pConnectionManager->n_connection;
    int * mapping = pConnectionManager->id_mapping;


    for(int i=max_connections-1; i>=0; i--){
        ID id = pConnectionManager->connection_list[i].id;
        CONNECTION_STATE state = pConnectionManager->connection_list[i].state;
        int socket_fd = pConnectionManager->connection_list[i].socket_fd;
        pSOCKET_QUEUE pSocketQueue = pConnectionManager->connection_list[i].pSocketQueue;
        int window = pConnectionManager->connection_list[i].window;
        unsigned char * pBuf = NULL;
        if(pSocketQueue){
            pBuf = pSocketQueue->buf;
        }

        fprintf(fd, "  %d Co: id=%d st=%d fd=%d q=%p qb=%p w=%d\n", i, id, state, socket_fd, pSocketQueue,pBuf, window);
        
    }


    fprintf(pUnsentFile, "\n");
    for(int i=0; i<max_id+1; i++){
        fprintf(pUnsentFile, "  mapping : id %d, index %d \n", i, mapping[i]);
        
    }
    fflush(pUnsentFile);
    fprintf(fd, "  nConnection : %d\n", nConnection);
    
    fprintf(fd, "  max_connection : %d\n", max_connections);
    
    fprintf(fd, "  max_id : %d\n", max_id);
    
    fprintf(fd, "---END--- \n");
    fflush(fd);

}


