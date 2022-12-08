//for compatibility with Ws2_32.lib, do not  include <windows.h>, instead define the WIN32_LEAN_AND_MEAN macro
//#include <windows.h> // Base  headers

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define UNICODE

#include "server_multithread.h"

CRITICAL_SECTION g_criticalSection;

int main(INT argc, CHAR **argv)
{
	DEBUG_ALWAYS(VIRTUAL_CHANNEL_NAME);
    int exitcode = SUCCESS;
	HANDLE hSocketRecvWorkerThread = NULL;
	HANDLE hCompletionPortSocket = NULL;
	HANDLE hReverseBindWorkerThread = NULL;
	HANDLE hCompletionPortReverseBind = NULL;
	//Initializing critical section
	InitializeCriticalSection(&g_criticalSection);
	DEBUG_ALWAYS("Critical section object initialized\n");

	pPER_SOCKET_DATA pConnectionList = NULL; //pointer to the first PER_SOCKET_DATA structure. This is the handle to the linked list .
	DYN_ARRAY daIdMapping; //dynamic array used to map numeric connection ids to the corresponding PER_SOCKET_DATA struct that is somewhere in the linked list (to limit lookup complexity)
	if(0 != da_init(&daIdMapping, (int)INITIAL_MAPPING_SIZE)){
		DEBUG_ALWAYS("Failed to initialize mapping dynamic array\n");
		exitcode = FAILURE;
		goto ExitMain;
	}


	pPER_REVERSEBIND_DATA pReverseBindList = NULL; //pointer to the first PER_REVERSEBIND_DATA structure of the linked list. To store the list of listening sockets used for reverse binds. No Id mapping, should not be required.

	DYN_ARRAY daTmpIdMapping;
	if(SUCCESS != da_init(&daTmpIdMapping, (INT)INITIAL_MAPPING_SIZE)){
		DEBUG_ALWAYS("Failed to initialize temporary id mapping dynamic array\n");
		exitcode = FAILURE;
		goto ExitMain;
	}


    // Initialize Winsock
	WSADATA wsaData;
	if (SUCCESS != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		DEBUG_ALWAYS("WSAStartup failed\n");
		exitcode = FAILURE;
		goto ExitMain;
	}

	//Récupere le pointeur vers ConnectEx()
	LPFN_CONNECTEX ConnectEx = NULL;
	if(NULL == (ConnectEx = GetConnectExPtr())){
		DEBUG_ALWAYS("Getting ConnectEx() failed\n");
		exitcode = FAILURE;
		goto ExitMain;
	}

	//Recupere AcceptEx()
	LPFN_ACCEPTEX AcceptEx = NULL;
	if(NULL == (AcceptEx = GetAcceptExPtr())){
		DEBUG_ALWAYS("Getting AcceptEx() failed\n");
		exitcode = FAILURE;
		goto ExitMain;
	}

    //Open the virtual channel
    VC_INFO vcInfo = { NULL};
    if (OpenVC(&vcInfo) == FAILURE) {
		exitcode = FAILURE;
		goto ExitMain;
	}

    //Allocate buffer to read ICA data into

	//en fait ptet pas besoin de faire un malloc vu que c'est de taille fixe. on peut l'allouer automatiquement
	CHAR pIcaRecvBuf[3000];

    //PCHAR pIcaRecvBuf = NULL;
    //if(NULL == (pIcaRecvBuf = (PCHAR)malloc(5000))){
    //    DEBUG_ALWAYS("Memory allocation error : pIcaRecvBuf\n");
    //    goto ExitMain;
    //}
    ULONG icaBytesRead = 0; //to store the number of bytes read by WFVirtualChannelRead() in the rest of the code
    // ---

	//Allocate buffer to send ICA data 
	//same que pour pIcaRecvBuf
	CHAR pIcaSendBuf[3000];
	// PCHAR pIcaSendBuf = NULL;
	// if(NULL == (pIcaSendBuf = (PCHAR)malloc(5000))){
	// 	DEBUG_ALWAYS("Memory allocation error : pIcaSendBuf\n");
    //     goto ExitMain;
	// }

	//Creation de l'I/O Completion Port
	
	if (NULL == (hCompletionPortSocket = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)))
	{
		DEBUG_ALWAYS_VAR("CreateIoCompletionPort() failed with error %d\n", GetLastError());
		exitcode= FAILURE;
		goto ExitMain;
	}
	DEBUG_ALWAYS("Completion port created\n");
	// ---


	//Lancement du thread qui gere le completion port 
	
	DWORD socketRecvWorkerThreadId;
	SOCKET_RECV_WORKER_THREAD_ARGS socketRecvWorkerThreadArgs;
	socketRecvWorkerThreadArgs.completionPort = hCompletionPortSocket;
	socketRecvWorkerThreadArgs.vcInfo = vcInfo;
	//socketRecvWorkerThreadArgs.pIcaSendBuf = pIcaSendBuf;
	socketRecvWorkerThreadArgs.p_daIdMapping = &daIdMapping;
	socketRecvWorkerThreadArgs.p_daTmpIdMapping = &daTmpIdMapping;
	socketRecvWorkerThreadArgs.ppConnectionList = &pConnectionList;
	socketRecvWorkerThreadArgs.ConnectEx = ConnectEx;

	if (NULL == (hSocketRecvWorkerThread = CreateThread(NULL, 0, SocketRecvWorkerThread, (LPVOID) &socketRecvWorkerThreadArgs,  0, &socketRecvWorkerThreadId)))
	{
		DEBUG_ALWAYS_VAR("CreateThread() failed with error %d\n", GetLastError());
		exitcode= FAILURE;
		goto ExitMain;
	}
	DEBUG_ALWAYS("Socket Recv worker thread created\n");
	// --- 

	//Lancement du completion port qui gere les reverse binds
	if (NULL == (hCompletionPortReverseBind = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)))
	{
		DEBUG_ALWAYS_VAR("CreateIoCompletionPort() failed with error %d\n", GetLastError());
		exitcode= FAILURE;
		goto ExitMain;
	}
	DEBUG_ALWAYS("Completion port (reverse binds) created\n");

	//

	//Lancement du thread qui gere les reverse binds
	DWORD reverseBindWorkerThreadId;
	REVERSEBIND_WORKER_THREAD_ARGS reverseBindWorkerThreadArgs;
	reverseBindWorkerThreadArgs.AccecptEx = AcceptEx;
	reverseBindWorkerThreadArgs.completionPort = hCompletionPortSocket;
	reverseBindWorkerThreadArgs.reverseBindCompletionPort = hCompletionPortReverseBind;
	reverseBindWorkerThreadArgs.ppReverseBindList = &pReverseBindList;
	reverseBindWorkerThreadArgs.vcInfo = vcInfo;
	reverseBindWorkerThreadArgs.p_daTmpIdMapping = &daTmpIdMapping;
	reverseBindWorkerThreadArgs.ppConnectionList = &pConnectionList;


	if (NULL == (hReverseBindWorkerThread = CreateThread(NULL, 0, ReverseBindWorkerThread, (LPVOID) &reverseBindWorkerThreadArgs,  0, &reverseBindWorkerThreadId)))
	{
		DEBUG_ALWAYS_VAR("CreateThread() failed with error %d\n", GetLastError());
		exitcode= FAILURE;
		goto ExitMain;
	}
	DEBUG_ALWAYS("Reverse binds worker thread created\n");
	//

	//On signale au client que le server est up
	if(SUCCESS != Send_Ica_MetaServerStart(vcInfo, pIcaSendBuf)){
					DEBUG_ALWAYS("Error send_ica_metaserverstart, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

    //Boucle infinie : on lit les données venant de la virtual channel et on agit en conséquence
    while(TRUE){
				
		//help for debug

		// pPER_SOCKET_DATA pNext = pConnectionList;
		// DEBUG_ALWAYS("-----\n");
	
	
		// while(pNext){
		// 	DEBUG_ALWAYS_VAR("id: %d, s: %d, win: %d, st: %d, cls: %d, sp:%d, rp:%d\n", pNext->id, pNext->socket,  pNext->window, pNext->state, pNext->closingState, pNext->pPerIoDataSending->ioType, pNext->pPerIoData->ioType);
		// 	pNext = pNext->pPrev;
		// }
		// DEBUG_ALWAYS("-----\n");
		//fin help for debug
		
		

		DEBUG("--------------------- LEAVE main critical section ---- \n\n");
		LeaveCriticalSection(&g_criticalSection);
		
		//on check que le thread est toujours en vie, sinon, ca veut dire que il faut close le programme
		
		switch(WaitForSingleObject(hSocketRecvWorkerThread, 0))
		{
			case WAIT_ABANDONED:
				DEBUG_ALWAYS("wait abandoned\n");
				break;
			case WAIT_OBJECT_0:
				DEBUG_ALWAYS("Worker thread exited, time to close\n");
				exitcode = FAILURE;
				goto ExitMain;
				break;
			case WAIT_TIMEOUT:
				//Tout va bien, le thread est encore en vie 
				break;
			case WAIT_FAILED:
				DEBUG_ALWAYS_VAR("WaitForSingleObject failed, lasterror: %d\n", GetLastError());
				break;
		}



		BOOL vcReadResult;
		//blocks until something is received on the virtual channel
		vcReadResult = WFVirtualChannelRead(vcInfo.hVC, (ULONG)10000, pIcaRecvBuf, (ULONG)sizeof(pIcaRecvBuf), &icaBytesRead);

		
		//Pour tester je pars sur un truc tres simple et tres conservateur, on execute le traitement complet d'un paquet de maniere atomique, et on fait pareil dans le thread pour le traitement d'un paquet de completion
		EnterCriticalSection(&g_criticalSection);
		//DEBUG_ALWAYS("enter main critical\n");
		DEBUG("=== ENTER main critical section ================= \n");

        if (TRUE != vcReadResult){ 

			

            DWORD error = GetLastError();
            switch (error)
            {
            case 995:
                //TODO : log fichier
				DEBUG("995\n");
                break;
            case 996:
                //TODO : log fichier
                DEBUG("996 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n")
                break;
            //TODO : a completer 
			case WAIT_TIMEOUT:
				DEBUG("WFVirtualChannelTimeout\n");
				continue;
            default:
                DEBUG_ALWAYS_VAR("FATAL ERROR WFVirtualChannelRead() : %d. Exiting.\n", error);
                exitcode = FAILURE;
                goto ExitMain;
            }
        }

        if(icaBytesRead == 0){
            DEBUG_ALWAYS("Chelou, WFVirtualChannelRead() returned but no data was read\n");
            goto ExitMain;
        }

        DEBUG_VAR("ICA data received : %d bytes\n", icaBytesRead);
       
        
        PACKET packet;
        Parse_Ica_GenericPacket(pIcaRecvBuf, &packet);
        DEBUG_VAR(" Parsing : type=%d, id=%d, len=%d, data_addr=%p \n", packet.type, packet.id, packet.len, packet.pData);

        if (packet.len > icaBytesRead - PACKET_TYPE_SIZE - ID_SIZE - LEN_SIZE) {
            DEBUG_ALWAYS("FATAL ERROR : parsing gave a data length parameter exceeding the number of bytes read by WFVirtualChannelRead(). Exiting\n");
            exitcode = FAILURE; //TODO : use more precise exitcodes ?
            goto ExitMain;
        }


		SOCKET connection_socket = INVALID_SOCKET;
		pPER_SOCKET_DATA pPerSocketData = NULL;
		pPER_REVERSEBIND_DATA pPerReverseBindData = NULL;

		VER ver = 0;
		CMD cmd = 0;
		PORT port = 0;
		CHAR pPortStr[6] = "00000";
		DOMLEN domLen = 0;
		DOMSTR pDomData = 0;
		CHAR pDomStr[300] = "";
		IPADDR ipaddr = 0;
		IP6ADDR ip6addr =NULL;
		IPADDR rmtAddr = 0;
		PORT rmtPort = 0;
		RPL rpl = 0;
		ID tmpId = 0;
		ID reverseBindId =0 ;
		int window=0;
		struct sockaddr_in addrbnd;
		struct sockaddr_in6 addrbnd6;
		struct sockaddr_in * pSockaddrIn;
		struct sockaddr_in6 * pSockaddrIn6;
		

		int rc;

        switch (packet.type) {
        
        case SOCKS_DATA:
			
		


			//on check que l'id existe 
			if(NULL == (pPerSocketData = da_get(daIdMapping, packet.id))) {
				//TODO : traiter ce cas la 
				DEBUG_ALWAYS("  WARN  id does not exist");
				//envoyer un close ou un packet meta error pour close cote client 
				break;
			}

			if(pPerSocketData->closingState != NONE){
				DEBUG("Connection closing state is not NONE, drop the packet\n");
				break;
			}

			if(pPerSocketData->state != ESTABLISHED){
				DEBUG_ALWAYS("  WARN  Connection state is not ESTABLISHED\n");
				//TODO : send META_ERROR_NOT_ESTABLISHED and let clientside manage closing ?
				break;
			}


			// queue les octects
			if(SUCCESS != Queue_Enqueue(&(pPerSocketData->sOutQueue),packet.pData, packet.len)){
				DEBUG_ALWAYS("error queueing ica data.\n");
				exitcode = FAILURE;
				goto ExitMain;
			}

			//on start un send si io none pending
			if(pPerSocketData->pPerIoDataSending->ioType == NONE_PENDING){
				DWORD flags=0;
				DWORD bytessent;
				
				pPerSocketData->pPerIoDataSending->wsabuf.buf = Queue_GetHead(&(pPerSocketData->sOutQueue));
				pPerSocketData->pPerIoDataSending->wsabuf.len = Queue_GetSizeCeiled(&(pPerSocketData->sOutQueue));
				
				
	
				pPerSocketData->pPerIoDataSending->ioType = SENDING;
				rc = WSASend(pPerSocketData->socket,&(pPerSocketData->pPerIoDataSending->wsabuf), 1,&bytessent, flags , &(pPerSocketData->pPerIoDataSending->wsaoverlapped), NULL);


				if(rc == SUCCESS){
					DEBUG("WSASend returned true\n");
				
					
				}
				else{
					rc = WSAGetLastError();
					if(rc==WSA_IO_PENDING){
						DEBUG("WSASend returned IO PENDING\n");
					}
					else if (rc==WSAEFAULT || rc==WSAENOBUFS){//TODO check other errors that force us to exit
						pPerSocketData->pPerIoDataSending->ioType = NONE_PENDING;
						DEBUG_ALWAYS_VAR("error wsasend. lasterror: %d\n", rc);
						exitcode = FAILURE;
						goto ExitMain;
					}
					//TODO do some errors allow us to try WSASend again ? 

					else{//otherwise we just close the socket
						DEBUG("wsasend error")
						pPerSocketData->pPerIoDataSending->ioType = NONE_PENDING;
						DEBUG_ALWAYS_VAR("StartClosing - Error WSASend: %d\n", rc);
						if(SUCCESS != StartClosing(pPerSocketData, vcInfo, pIcaSendBuf)){
							DEBUG_ALWAYS("error start closing\n");
							exitcode = FAILURE;
							goto ExitMain;
						}	
					}
				}
				
				
			}

            break;

        case SOCKS_CLIENT_IP_CMD:


			if(NULL != da_get(daIdMapping, packet.id)) {
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				DEBUG_WARN("This ID already exists\n");
				break;
			}

			
			//On crée une nouvelle socket
			//SOCKET connection_socket = INVALID_SOCKET;
			if(INVALID_SOCKET == (connection_socket = NewSocket(AF_INET))){ // on renvoie l'erreur au client si jamais on n'arrive pas a creer la socket
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				DEBUG_WARN("Error creating socket, \n");
				break;
			}

			//On bind la nouvelle socket, c'est necesaire pour COnnectEx
			//struct sockaddr_in addrbnd;
			
      	 	addrbnd.sin_family = AF_INET;
      	  	addrbnd.sin_addr.s_addr = INADDR_ANY;
      	 	addrbnd.sin_port = 0;
       		if(SUCCESS != bind(connection_socket, (SOCKADDR*) &addrbnd, sizeof(addrbnd))){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				closesocket(connection_socket);
				DEBUG_WARN("Error bindding socket, \n");
				break;
			}


			//La nouvelle socket est créee et bindée maintenant on alloue une nouvelle structure connection
			//pPER_SOCKET_DATA pPerSocketData = NULL;
			if(NULL ==(pPerSocketData = NewConnection(packet.id, connection_socket))){

				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				closesocket(connection_socket);
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
			}

			
			//La nouvelle structure de connexion est créee, on ajoute le couple socket/structure de connexion au completionport
			if(SUCCESS != UpdateIOCP(hCompletionPortSocket, connection_socket, pPerSocketData)){

				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo,pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
				
			}

			//Si tout a fonctionné, on a une socket et sa structure de connexion, toute deux associées au completion port. On peut ajouter la structure de connexion a la liste chainée globale, regroupant toutes les connexions existantes.
			AddConnection(&pConnectionList, pPerSocketData);

			//On associe la structure de connexion avec l'id dans le tableau dynamique pour faciliter les lookups et ne pas avoir a parcourir la liste chainée toute entiere
			if(SUCCESS != da_insert(&daIdMapping, packet.id, pPerSocketData)){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				RemoveConnection(&pConnectionList, pPerSocketData);
				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
			}

			
			// On parse le packet socks 
			if(SUCCESS!=Parse_Ica_ClientIpCmd(packet.pData,packet.len, &ver, &cmd, &ipaddr, &port)){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				da_insert(&daIdMapping, packet.id, NULL);
				RemoveConnection(&pConnectionList, pPerSocketData);
				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error parsing the packet \n");
				break;
			}
			

			
			pSockaddrIn = (struct sockaddr_in *)(&(pPerSocketData->server));

		
			pSockaddrIn->sin_addr.s_addr=htonl(ipaddr);
			pSockaddrIn->sin_port = htons(port);
			pSockaddrIn->sin_family = AF_INET;

			
			pPerSocketData->pPerIoData->ioType = CONNECTION;
			DEBUG_VAR("socket : %d\n", pPerSocketData->socket);
	
			rc = ConnectEx(pPerSocketData->socket, (const struct sockaddr *)&(pPerSocketData->server),sizeof(struct sockaddr_in), NULL, 0, NULL, &(pPerSocketData->pPerIoData->wsaoverlapped));


			DEBUG_VAR("rc :%d\n", rc);
			if(rc == TRUE){
				DEBUG_ALWAYS("Should not succeed immediately, weird\n ");
			}
			else {
				rc = WSAGetLastError();
				if(rc != 997){
					pPerSocketData->pPerIoDataSending->ioType = NONE_PENDING;
					DEBUG_ALWAYS_VAR("errorcode = %d, which is not IO_PENDING\n", rc);
					if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
						DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
						exitcode = FAILURE;
						goto ExitMain;
					}
				
					// da_insert(&daIdMapping, packet.id, NULL);
					// RemoveConnection(&pConnectionList, pPerSocketData);
					// FreeConnection(pPerSocketData);
					// pPerSocketData = NULL;
					// closesocket(connection_socket);
					// connection_socket = INVALID_SOCKET; //pas besoin de close car le client envoie une CLOSEREQ en recevant le ServerRpl
					DEBUG_WARN("Error connectex\n")
					break;
				}
			}

			//On passe la connexion en CONNECTING;
			pPerSocketData->state = CONNECTING;
			
			DEBUG_VAR("%d\n",rc)

            break;

        case SOCKS_CLIENT_IP6_CMD:

		
   
			
			if(NULL != da_get(daIdMapping, packet.id)) {
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				DEBUG_WARN("This ID already exists\n");
				break;
			}

			
			//On crée une nouvelle socket
			//SOCKET connection_socket = INVALID_SOCKET;
			if(INVALID_SOCKET == (connection_socket = NewSocket(AF_INET6))){ // on renvoie l'erreur au client si jamais on n'arrive pas a creer la socket
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				DEBUG_WARN("Error creating socket, \n");
				break;
			}

			//On bind la nouvelle socket, c'est necesaire pour COnnectEx
			
			
      	 	addrbnd6.sin6_family = AF_INET6;
      	  	addrbnd6.sin6_addr = in6addr_any;
      	 	addrbnd6.sin6_port = 0;
       		if(SUCCESS != bind(connection_socket, (SOCKADDR*) &addrbnd6, sizeof(addrbnd6))){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				closesocket(connection_socket);
				DEBUG_WARN("Error bindding socket, \n");
				break;
			}


			//La nouvelle socket est créee et bindée maintenant on alloue une nouvelle structure connection
			//pPER_SOCKET_DATA pPerSocketData = NULL;
			if(NULL ==(pPerSocketData = NewConnection(packet.id, connection_socket))){

				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				closesocket(connection_socket);
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
			}


			//La nouvelle structure de connexion est créee, on ajoute le couple socket/structure de connexion au completionport
			if(SUCCESS != UpdateIOCP(hCompletionPortSocket, connection_socket, pPerSocketData)){

				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
				
			}

			//Si tout a fonctionné, on a une socket et sa structure de connexion, toute deux associées au completion port. On peut ajouter la structure de connexion a la liste chainée globale, regroupant toutes les connexions existantes.
			AddConnection(&pConnectionList, pPerSocketData);

			//On associe la structure de connexion avec l'id dans le tableau dynamique pour faciliter les lookups et ne pas avoir a parcourir la liste chainée toute entiere
			if(SUCCESS != da_insert(&daIdMapping, packet.id, pPerSocketData)){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				RemoveConnection(&pConnectionList, pPerSocketData);
				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
			}


			// On parse le packet socks 
			if(SUCCESS!=Parse_Ica_ClientIp6Cmd(packet.pData,packet.len, &ver, &cmd, &ip6addr, &port)){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				da_insert(&daIdMapping, packet.id, NULL);
				RemoveConnection(&pConnectionList, pPerSocketData);
				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error parsing the packet \n");
				break;
			}

			pSockaddrIn6 = (struct sockaddr_in6 *)&(pPerSocketData->server);

			pSockaddrIn6->sin6_family =AF_INET6;
			pSockaddrIn6->sin6_port = htons(port); 
			//RtlIpv4StringToAddressW("fe80::faac:65ff:fee3:b174", NULL, &(pSockaddrIn6->sin6_addr));
			
			memcpy(pSockaddrIn6->sin6_addr.s6_addr, ip6addr, 16);

			DEBUG_VAR("test : %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \n", pSockaddrIn6->sin6_addr.s6_addr[0],pSockaddrIn6->sin6_addr.s6_addr[1], pSockaddrIn6->sin6_addr.s6_addr[2], pSockaddrIn6->sin6_addr.s6_addr[3], pSockaddrIn6->sin6_addr.s6_addr[4], pSockaddrIn6->sin6_addr.s6_addr[5], pSockaddrIn6->sin6_addr.s6_addr[6], pSockaddrIn6->sin6_addr.s6_addr[7], pSockaddrIn6->sin6_addr.s6_addr[8], pSockaddrIn6->sin6_addr.s6_addr[9], pSockaddrIn6->sin6_addr.s6_addr[10], pSockaddrIn6->sin6_addr.s6_addr[11], pSockaddrIn6->sin6_addr.s6_addr[12], pSockaddrIn6->sin6_addr.s6_addr[13], pSockaddrIn6->sin6_addr.s6_addr[14], pSockaddrIn6->sin6_addr.s6_addr[15]);
		

	        

			

		
			
			pPerSocketData->pPerIoData->ioType = CONNECTION;
			DEBUG_VAR("socket : %d\n", pPerSocketData->socket);
	
			rc = ConnectEx(pPerSocketData->socket, (const struct sockaddr *)&(pPerSocketData->server),sizeof(struct sockaddr_in6), NULL, 0, NULL, &(pPerSocketData->pPerIoData->wsaoverlapped));

	
		

			DEBUG_VAR("rc :%d\n", rc);
			if(rc == TRUE){
				DEBUG_ALWAYS("Should not succeed immediately, weird\n ");
			}
			else {
				rc = WSAGetLastError();
				if(rc != 997){
					DEBUG_ALWAYS_VAR("errorcode = %d, which is not IO_PENDING\n", rc);
					pPerSocketData->pPerIoDataSending->ioType = NONE_PENDING;
					if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
						DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
						exitcode = FAILURE;
						goto ExitMain;
					}
				
					da_insert(&daIdMapping, packet.id, NULL);
					RemoveConnection(&pConnectionList, pPerSocketData);
					FreeConnection(pPerSocketData);
					pPerSocketData = NULL;
					closesocket(connection_socket);
					connection_socket = INVALID_SOCKET;
					DEBUG_WARN("Error connectex\n")
					break;
				}
			}

			//On passe la connexion en CONNECTING;
			pPerSocketData->state = CONNECTING;
			
			DEBUG_VAR("%d\n",rc)



            break;

        case SOCKS_CLIENT_DOMAIN_CMD:


			
			if(NULL != da_get(daIdMapping, packet.id)) {
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				DEBUG_WARN("This ID already exists\n");
				break;
			}

			
			//On crée une nouvelle socket
			//SOCKET connection_socket = INVALID_SOCKET;
			if(INVALID_SOCKET == (connection_socket = NewSocket(AF_INET))){ // on renvoie l'erreur au client si jamais on n'arrive pas a creer la socket
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				DEBUG_WARN("Error creating socket, \n");
				break;
			}

			//On bind la nouvelle socket, c'est necesaire pour COnnectEx
			//struct sockaddr_in addrbnd;
			
      	 	addrbnd.sin_family = AF_INET;
      	  	addrbnd.sin_addr.s_addr = INADDR_ANY;
      	 	addrbnd.sin_port = 0;
       		if(SUCCESS != bind(connection_socket, (SOCKADDR*) &addrbnd, sizeof(addrbnd))){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				closesocket(connection_socket);
				DEBUG_WARN("Error bindding socket, \n");
				break;
			}

			//La nouvelle socket est créee, maintenant on alloue une nouvelle structure connection
			//pPER_SOCKET_DATA pPerSocketData = NULL;
			if(NULL ==(pPerSocketData = NewConnection(packet.id, connection_socket))){

				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				closesocket(connection_socket);
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
			}


			//La nouvelle structure de connexion est créee, on ajoute le couple socket/structure de connexion au completionport
			if(SUCCESS != UpdateIOCP(hCompletionPortSocket, connection_socket, pPerSocketData)){

				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo,pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
				
			}

			//Si tout a fonctionné, on a une socket et sa structure de connexion, toute deux associées au completion port. On peut ajouter la structure de connexion a la liste chainée globale, regroupant toutes les connexions existantes.
			AddConnection(&pConnectionList, pPerSocketData);

			//On associe la structure de connexion avec l'id dans le tableau dynamique pour faciliter les lookups et ne pas avoir a parcourir la liste chainée toute entiere
			if(SUCCESS != da_insert(&daIdMapping, packet.id, pPerSocketData)){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				RemoveConnection(&pConnectionList, pPerSocketData);
				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error allocating connection struct, \n");
				break;
			}

			
			// On parse le packet socks 

			if(SUCCESS!=Parse_Ica_ClientDomainCmd(packet.pData,packet.len, &ver, &cmd, &port, &domLen, &pDomData, pDomStr)){
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				da_insert(&daIdMapping, packet.id, NULL);
				RemoveConnection(&pConnectionList, pPerSocketData);
				FreeConnection(pPerSocketData);
				pPerSocketData = NULL;
				closesocket(connection_socket);
				connection_socket = INVALID_SOCKET;
				DEBUG_WARN("Error parsing the packet \n");
				break;
			}
			
			itoa(port, pPortStr, 10);
			strcpy_s(pPerSocketData->pDomainName, 299, pDomStr);
			strcpy_s(pPerSocketData->pPortName, 7, pPortStr);
			
			DEBUG_VAR("domain name : %s\n", pPerSocketData->pDomainName);
			//ADDRINFOEXW hints = {0, 0, 0,0,0,NULL, NULL, NULL, 0, NULL, NULL};

			DWORD resolveDNSThreadId = 0;


			pRESOLVE_THREAD_ARGS pResolveThreadArgs = NULL;
			if(NULL == (pResolveThreadArgs = (pRESOLVE_THREAD_ARGS)malloc(sizeof(RESOLVE_THREAD_ARGS)) )){
				DEBUG_ALWAYS("error malloc resolve thread args\n");
				exitcode = FAILURE;
				goto ExitMain;
			}
			pResolveThreadArgs->pPerSocketData = pPerSocketData;
			pResolveThreadArgs->pCriticalSection = &g_criticalSection;
			pResolveThreadArgs->p_daIdMapping = &daIdMapping;
			pResolveThreadArgs->ppConnectionList = &pConnectionList;
			pResolveThreadArgs->vcInfo = vcInfo;
			pResolveThreadArgs->hCompletionPort= hCompletionPortSocket;

			if (NULL == (pResolveThreadArgs->handleSelf = CreateThread(NULL, 0, ResolveDNS, (LPVOID) pResolveThreadArgs,  0, &resolveDNSThreadId)))
			{
				DEBUG_ALWAYS_VAR("CreateThread() failed with error %d\n", GetLastError());
				exitcode= FAILURE;
				goto ExitMain;
			}
			DEBUG("DNS thread created\n");

		
			//faire les verifs que rc == 997 (WSA_IO_PENDING)
			//rc = GetAddrInfoExW((PCWSTR)pDomStr, (PCWSTR)pPortStr,NS_ALL,NULL,&hints,&(pPerSocketData->pAddrInfo), NULL,&(pPerSocketData->pPerIoData->wsaoverlapped),NULL,NULL);
		 

			//DEBUG_VAR("rc : %d\n", rc);

			DEBUG_VAR("hostname : %s\n", pDomStr);
			//DEBUG_VAR("id : %d\n", packet.id);




			pPerSocketData->state= RESOLVING;

            break;

		case REVERSE_BIND_CMD:
			DEBUG_ALWAYS("REVERSE BIND CMD received\n");

			//Peut etre faire des checks sur l'ID de reverse ?? ou alors on considere que vu que la verif est faite cote client c'est tout bon ?
			

			// On parse le packet 
			if(SUCCESS != Parse_Ica_ReverseBindCmd(packet.pData, packet.len, &rmtAddr, &rmtPort)){
				if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
					DEBUG_ALWAYS("Error send_ica_reversebindrpl, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				DEBUG_ALWAYS("Error parsing\n");
				break;
			}

			pPerReverseBindData = pReverseBindList;
			while(pPerReverseBindData != NULL){
				if(pPerReverseBindData->id == packet.id){
					DEBUG_ALWAYS("This reverse bind id already exists\n")
					exitcode = FAILURE;
					goto ExitMain;
				}
				pPerReverseBindData = pPerReverseBindData->pPrev;
			}




			//On ouvre une socket qui ecoute sur rmtA
			SOCKET listeningSocket = INVALID_SOCKET;

			if ((listeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
			{
				DEBUG_ALWAYS_VAR("Could not create socket : %d", WSAGetLastError());
				if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
					DEBUG_ALWAYS("Error send_ica_reversebindrpl, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				break;
				
			}
			DEBUG("Socket created\n");

			ULONG mode = 1;
			if (ioctlsocket(listeningSocket, FIONBIO, &mode) == SOCKET_ERROR) {
				DEBUG_ALWAYS_VAR("Error ioctlsocket() : %d.\n", WSAGetLastError());
				if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
					DEBUG_ALWAYS("Error ioctl, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				closesocket(listeningSocket);
				listeningSocket = INVALID_SOCKET;
				break;
			}
			DEBUG("Socket set to non blocking.\n");

			addrbnd.sin_family = AF_INET;
      	  	addrbnd.sin_addr.s_addr = htonl(rmtAddr);
      	 	addrbnd.sin_port = htons(rmtPort);
       		if(SUCCESS != bind(listeningSocket, (SOCKADDR*) &addrbnd, sizeof(addrbnd))){
				DEBUG_WARN("Error bindding socket, \n");
				if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
					DEBUG_ALWAYS("Error send_ica_reversebindrpl, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				closesocket(listeningSocket);
				listeningSocket = INVALID_SOCKET;
				
				break;
			}

			if(SOCKET_ERROR == listen(listeningSocket, 100)){
				if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
					DEBUG_ALWAYS("Error send_ica_reversebindrpl, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
		
				closesocket(listeningSocket);
				listeningSocket = INVALID_SOCKET;
				DEBUG_WARN("Error updating completion port \n");
				break;
			}

			SOCKET acceptSocket = INVALID_SOCKET;
			if(INVALID_SOCKET == (acceptSocket = socket(AF_INET, SOCK_STREAM, 0))){
				DEBUG_ALWAYS("error creating accept socket\n");
				if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
					DEBUG_ALWAYS("Error send_ica_reversebindrpl, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
		
				closesocket(listeningSocket);
				listeningSocket = INVALID_SOCKET;
				
				break;
			}

			pPerReverseBindData = NULL;
			if(NULL ==(pPerReverseBindData = NewReverseBind(packet.id, listeningSocket))){

				if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
					DEBUG_ALWAYS("Error send_ica_reversebindrpl, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}
				closesocket(listeningSocket);
				listeningSocket = INVALID_SOCKET;
				closesocket(acceptSocket);
				acceptSocket = INVALID_SOCKET;
				DEBUG_WARN("Error creating new connection, \n");
				break;
			}

			

			//La nouvelle structure de connexion est créee, on ajoute le couple socket/structure de connexion au completionport
			if(SUCCESS != UpdateIOCPReverseBind(hCompletionPortReverseBind, listeningSocket, pPerReverseBindData)){
				
				if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
					DEBUG_ALWAYS("Error send_ica_reversebindrpl, exiting")
					exitcode = FAILURE;
					goto ExitMain;
				}

				FreeReverseBind(pPerReverseBindData);
				pPerReverseBindData = NULL;
				closesocket(listeningSocket);
				listeningSocket = INVALID_SOCKET;
				closesocket(acceptSocket);
				acceptSocket = INVALID_SOCKET;
				DEBUG_WARN("Error updating completion port \n");
				break;
				
			}

	
			pPerReverseBindData->pPerAcceptData->acceptSocket = acceptSocket;
			//On lance un acceptex()
			DWORD bytesReceived= 0;
			pPerReverseBindData->pPerAcceptData->ioType = ACCEPTING;
			rc = AcceptEx(listeningSocket, acceptSocket, pPerReverseBindData->pPerAcceptData->buffer, 0,sizeof (struct sockaddr_in) + 16,sizeof (struct sockaddr_in) + 16,&bytesReceived,&(pPerReverseBindData->pPerAcceptData->wsaoverlapped));

	
		

			DEBUG_VAR("rc :%d\n", rc);
			if(rc == TRUE){
				DEBUG_ALWAYS("Should not succeed immediately, weird\n ");
			}
			else {
				rc = WSAGetLastError();
				if(rc != 997){
					DEBUG_ALWAYS_VAR("errorcode = %d, which is not IO_PENDING\n", rc);
					pPerReverseBindData->pPerAcceptData->ioType = NONE_PENDING;
					
					if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x01)){
						DEBUG_ALWAYS("Error send_ica_reversebindrpl, exiting")
						exitcode = FAILURE;
						goto ExitMain;
					}
				
					FreeReverseBind(pPerReverseBindData);
					pPerReverseBindData = NULL;
					closesocket(connection_socket);
					connection_socket = INVALID_SOCKET;
					closesocket(acceptSocket);
					acceptSocket = INVALID_SOCKET;
					DEBUG_WARN("Error connectex\n")
					break;
				}
			}


			//On renvoie un reverbindrplpacket de succes
			if(SUCCESS != Send_Ica_ReverseBindRplPacket(vcInfo, pIcaSendBuf, packet.id, 0x00)){
				DEBUG_ALWAYS("error send reversebindrpl\n");
				exitcode = FAILURE;
				goto ExitMain;
			}


			//On ajoute le reverse bind
			AddReverseBind(&pReverseBindList,pPerReverseBindData);

			break;
		case REVERSE_CLOSE_REQUEST:
			
			DEBUG("REVERSE_CLOSE_REQUEST\n");

			pPerReverseBindData = pReverseBindList;
			while(pPerReverseBindData != NULL){
				if(pPerReverseBindData->id == packet.id){ 
					DEBUG_VAR("Closing reverse bind %d\n ", packet.id);
				

					if(pPerReverseBindData->closingState == NONE){
						
						DEBUG("Closing the listening socket\n");
						closesocket(pPerReverseBindData->listeningSocket);
						pPerReverseBindData->listeningSocket = INVALID_SOCKET;
						
						//Si IO NONE_PENDING, on close tout de suite et on envoie un ACK
						if(pPerReverseBindData->pPerAcceptData->ioType==NONE_PENDING){
							DEBUG("No accept pending, closing everything now\n");
							
							if(SUCCESS != Send_Ica_ReverseCloseAck(vcInfo, pIcaSendBuf, pPerReverseBindData->id)){
								DEBUG_ALWAYS("Could not send ICA reverse close ACK\n");
								exitcode = FAILURE;
								goto ExitMain;
							}

							if(SUCCESS != CloseReverseBind((&pReverseBindList), pPerReverseBindData)){
								DEBUG_ALWAYS("Could not close Reverse bind\n");
								exitcode = FAILURE;
								goto ExitMain;
							}
							pPerReverseBindData = NULL;

					


						}
						//Si IO PENDING, on close(listening) et on passe en REQ_RECV_TO_CLOSE
						else{
							DEBUG("Accept pending, setting REQ_RECEIVED_TO_CLOSE and closing the listening socket\n");
							pPerReverseBindData->closingState = REQ_RECEIVED_TO_CLOSE;
						}
						
					}
					else if(pPerReverseBindData->closingState == REQ_SENT){
						//veut dire qu'on a deja close(listening_socket) et envoyé une CLOSE_REQ. On attend un ACK et la fin de I/O
						//On envoie un ACK pour repondre a la CLOSE_REQ, et le client doit faire pareil

						//Send ACK
						if(SUCCESS != Send_Ica_ReverseCloseAck(vcInfo, pIcaSendBuf, pPerReverseBindData->id)){
							DEBUG_ALWAYS("Could not send ICA reverse close ACK\n");
							exitcode = FAILURE;
							goto ExitMain;
						}

						//On attend l'ACK du client pour finir le close
					}
					else if(pPerReverseBindData->closingState == REQ_RECEIVED_TO_CLOSE || pPerReverseBindData->closingState == ACK_RECEIVED_TO_CLOSE){
						//pas normal on doit pas recevoir une requete si on en a deja recu une ou si on a recu un ACK, on drop

					}
					
					break;
				}
				pPerReverseBindData = pPerReverseBindData->pPrev;
			}
			if(pPerReverseBindData==NULL){
				DEBUG("This ID does not exist\n");
			}

			break;

		case REVERSE_CLOSE_ACK:

			DEBUG("REVERSE_CLOSE_ACK\n");

			pPerReverseBindData = pReverseBindList;
			while(pPerReverseBindData != NULL){
				if(pPerReverseBindData->id == packet.id){
					
					if(pPerReverseBindData->closingState == REQ_SENT){
						//Normal, on avait envoyé une CLOSE_REQ

						//Si IO NONE_PENDING, on finit de close proprement et c'est terminé 
						if(pPerReverseBindData->pPerAcceptData->ioType == NONE_PENDING){
							if(SUCCESS != CloseReverseBind((&pReverseBindList), pPerReverseBindData)){
								DEBUG_ALWAYS("Could not close Reverse bind\n");
								exitcode = FAILURE;
								goto ExitMain;
							}
							pPerReverseBindData = NULL;
							DEBUG_ALWAYS("Reverse bind closed\n");

						}
						//Sinon, on passe en ACK_RECEIVED_TO_CLOSE et on fermera au moment de la reception du completion packet
						else{
							DEBUG("Accept pending, setting ACK_RECEIVED_TO_CLOSE\n");
							pPerReverseBindData->closingState = ACK_RECEIVED_TO_CLOSE;
						}
					}
					else if(pPerReverseBindData->closingState == REQ_RECEIVED_TO_CLOSE || pPerReverseBindData->closingState == ACK_RECEIVED_TO_CLOSE || pPerReverseBindData->closingState == NONE){
						//pas normal de recevoir 2 ACK
						// pas normal si on est en REQ_RECEIVED_TO_CLOSE, pcq ca veut dire qu 'on a pas envoyé de CLOSE_REQ, sinon on serait en REQ_SENT
						//pas normal de recevoir un ACK sans avoir envoyé de CLOSE REQ
						// on drop
					}
					
					break;
				}
				pPerReverseBindData = pPerReverseBindData->pPrev;
			}
			if(pPerReverseBindData==NULL){
				DEBUG("This ID does not exist\n");
			}

			break;
		case REVERSE_CONNECT_RPL:
			DEBUG("REVERSE CONNECT RPL\n");
			if(SUCCESS != Parse_Ica_ReverseConnectRpl(packet.pData, packet.len,&tmpId, &reverseBindId, &rpl)){
				DEBUG_ALWAYS("Could not parse the packet\n");
				exitcode = FAILURE;
				goto ExitMain;
			}
			if(NULL == (pPerSocketData = da_get(daTmpIdMapping, tmpId))){
				DEBUG_ALWAYS("This tmpId does not exist.\n")
				exitcode = FAILURE;
				goto ExitMain;
			}
			
			if(rpl == 0x00){
				
				pPerSocketData->id = packet.id;
				da_insert(&daIdMapping, pPerSocketData->id, pPerSocketData);
				da_insert(&daTmpIdMapping, tmpId, NULL);
				pPerSocketData->state = ESTABLISHED;

				
				
				if(pPerSocketData->window > pPerSocketData->pPerIoData->wsabuf.len){
					//On lance un premier WSARecv sur la connexion
					DWORD flags = 0;
					pPerSocketData->pPerIoData->ioType = RECEPTION;
					rc  = WSARecv(pPerSocketData->socket, &(pPerSocketData->pPerIoData->wsabuf), 1, NULL, &flags, &(pPerSocketData->pPerIoData->wsaoverlapped), NULL);

					if(rc == SUCCESS){
						DEBUG("WSARecv returned true\n");
						
					}
					else{
						rc = WSAGetLastError();
						if(rc==WSA_IO_PENDING){
							DEBUG("WSARecv returned IO PENDING\n");
						}
						else if (rc==WSAEFAULT || rc==WSAENOBUFS){//TODO check other errors that force us to exit
							pPerSocketData->pPerIoData->ioType = NONE_PENDING;
							DEBUG_ALWAYS_VAR("error wsarecv. lasterror: %d\n", rc);
							return(1);
						}
						//TODO do some errors allow us to try WSARecv again ? 

						else{//otherwise we just close the socket
							DEBUG("wsarecv error")
							pPerSocketData->pPerIoData->ioType = NONE_PENDING;
							DEBUG_ALWAYS_VAR("StartClosing - Error WSARecv: %d\n", rc);
							if(SUCCESS != StartClosing(pPerSocketData, vcInfo, pIcaSendBuf)){
								DEBUG_ALWAYS("error start closing\n");
								return(1);
							}	
						}
					}

				}

				
			}
			else if(rpl == 0x01){
				//pas besoin de close request pcq c'est deja fermé de l'autre cote
				RemoveConnection(&pConnectionList, pPerSocketData);
					
				FreeConnection(pPerSocketData);
				da_insert(&daTmpIdMapping, tmpId, NULL);
				pPerSocketData=NULL;

			}

			break;
        case CLOSE_REQUEST: 
			//DEBUG_ALWAYS("CLOSE_REQUEST received\n");

			//On check si l'ID existe	
			if(NULL == (pPerSocketData = da_get(daIdMapping, packet.id))) {
				//TODO : traiter ce cas la 
				DEBUG_ALWAYS("id does not exist, sending id error packet\n");
				//c'est comme si c'etait deja fermé du coup, on envoie un paquet d'erreur et on laisse le driver client voir pq il a merdé
				//envoyer un close ou un packet meta error pour close cote client 
				//TODO: send META_ERROR_ID
				break;
			}
			if(packet.id != pPerSocketData->id){
				//Chelou
				DEBUG_ALWAYS("mismatch between IdMapping and PerSocketData.id\n");
				exitcode = FAILURE;
				goto ExitMain;

			}
			//On check si on a deja envoyé une CLOSE_REQUEST ou pas 

			else if(pPerSocketData->closingState == REQ_SENT){
				//On a deja envoyé la CLOSING_REQUEST donc a priori on gere deja 
				
				//Je pense que du coup on peut envoyer un ACK pour rep a la req, pcq on a deja appelé closesocket, on attend juste que l'IO termine et de recevoir un ACK du client.
				
				DEBUG("Received CLOSE_REQ after sending one, treating it as an ACK\n");
				if(SUCCESS != Send_Ica_CloseAck(vcInfo, pIcaSendBuf, packet.id)){
					DEBUG_ALWAYS("Error send ica close ack\n");
					exitcode = FAILURE;
					goto ExitMain;
				}

				//Pour l'instant, on fait comme si on avait recu un ack du client 
				if(pPerSocketData->pPerIoData->ioType == NONE_PENDING && pPerSocketData->pPerIoDataSending->ioType == NONE_PENDING){
					DEBUG("No I/O pending, closing immediately\n");
					//si aucune opération I/O n'est en cours 
					RemoveConnection(&pConnectionList, pPerSocketData);
					
					FreeConnection(pPerSocketData);
					da_insert(&daIdMapping, packet.id, NULL);
					pPerSocketData=NULL;

			
					DEBUG_VAR("CLOSE_REQ for connection %d completed\n", packet.id);
					break;
				}
				else{
					//at least one I/O operation pending
					DEBUG("At least one I/O operation pending, setting ACK_RECEIVED_TO_CLOSE\n");
					pPerSocketData->closingState = ACK_RECEIVED_TO_CLOSE;
					break;
				}


				break;
			}
			
			else if(pPerSocketData->closingState == REQ_RECEIVED_TO_CLOSE || pPerSocketData->closingState == ACK_RECEIVED_TO_CLOSE){
				//On a deja recu une CLOSING_REQUEST  ou une ACK et on est en cours de fermeture 
				//TODO send META_ERROR_DOUBLE_CLOSE et on laisse le client gerer ? ou alors on drop cette deuxieme request ? 

				break;
			}
			else{
				//Sinon, si closingState == NONE
				closesocket(pPerSocketData->socket);
				pPerSocketData->socket = INVALID_SOCKET;
				if(pPerSocketData->pPerIoData->ioType == NONE_PENDING && pPerSocketData->pPerIoDataSending->ioType == NONE_PENDING && pPerSocketData->state != RESOLVING){
					DEBUG("No I/O pending, closing immediately\n");
					//si aucune opération I/O n'est en cours 
					RemoveConnection(&pConnectionList, pPerSocketData);
					
					FreeConnection(pPerSocketData);
					da_insert(&daIdMapping, packet.id, NULL);
					pPerSocketData=NULL;

					if(SUCCESS != Send_Ica_CloseAck(vcInfo, pIcaSendBuf, packet.id)){
						DEBUG_ALWAYS("Error send ica close ack\n");
						exitcode = FAILURE;
						goto ExitMain;
					}
					DEBUG_VAR("CLOSE_REQ for connection %d completed, ACK sent\n", packet.id);
					break;
				}
				else{
					//at least one I/O operation pending
					DEBUG("At least one I/O operation pending (or connection is RESOLVING and a RESOLUTION packet is to be posted), setting REQ_RECEIVED_TO_CLOSE\n");
					pPerSocketData->closingState = REQ_RECEIVED_TO_CLOSE;
					break;
				}
			}
			



            break;

        case CLOSE_ACK:
  
			DEBUG("CLOSE_ACK received\n");
			
			//On check si l'ID existe	
			if(NULL == (pPerSocketData = da_get(daIdMapping, packet.id))) {
				//TODO : traiter ce cas la 
				DEBUG_ALWAYS("id does not exist, sending id error packet\n");
				//c'est comme si c'etait deja fermé du coup, on envoie un paquet d'erreur et on laisse le driver client voir pq il a merdé
				//envoyer un close ou un packet meta error pour close cote client 
				//TODO: send META_ERROR_ID
				break;
			}
			if(packet.id != pPerSocketData->id){
				//Chelou
				DEBUG_ALWAYS("mismatch between IdMapping and PerSocketData.id\n");
				exitcode = FAILURE;
				goto ExitMain;

			}
			//On check si on a deja envoyé une CLOSE_REQUEST ou pas 

			else if(pPerSocketData->closingState == REQ_RECEIVED_TO_CLOSE){
				//On a deja recu une CLOSING_REQUEST et on est en cours de fermeture 
				//TODO send META_ERROR_ACK_AFTER_CLOSE et on laisse le client gerer ? ou alors on drop cette deuxieme request ? 

				break;
			}
			
			else if(pPerSocketData->closingState == NONE){
			
				//On a pas envoyé de CLOSE_REQUEST donc on doit pas recevoir d'ACK
				//TODO send META_ERROR_ORPHAN_ACK et on laisse gérer le client comme il veut
				//TODO 
				break;
			}
			else{
				//Sinon, si closingState = REQ_SENT
				
				
				if(pPerSocketData->pPerIoData->ioType == NONE_PENDING && pPerSocketData->pPerIoDataSending->ioType == NONE_PENDING){
					DEBUG("No I/O pending, closing immediately\n");
					//si aucune opération I/O n'est en cours 
					RemoveConnection(&pConnectionList, pPerSocketData);
					
					FreeConnection(pPerSocketData);
					da_insert(&daIdMapping, packet.id, NULL);
					pPerSocketData=NULL;

			
					DEBUG_VAR("CLOSE_REQ for connection %d completed\n", packet.id);
					break;
				}
				else{
					//at least one I/O operation pending
					DEBUG("At least one I/O operation pending, setting ACK_RECEIVED_TO_CLOSE\n");
					pPerSocketData->closingState = ACK_RECEIVED_TO_CLOSE;
					break;
				}
			}

            break;
        case META_WINDOW:

			//on parse le packet window
			if (SUCCESS != Parse_Ica_MetaWindow(packet.pData,packet.len,&window)){
				DEBUG_WARN("Error parsing window packet. Closing connection.\n");
				//TODO: close connection

				break;
			}
			
			//on check que l'id existe 
			if(NULL == (pPerSocketData = da_get(daIdMapping, packet.id))) {
				//TODO : traiter ce cas la 
				DEBUG_ALWAYS("  WARN  id does not exist");
				break;
			}


			if(pPerSocketData->closingState != NONE){
				DEBUG("Connection closing state is not NONE, drop the packet\n");
				break;
			}

			//on met a jour notre structure connexion correspondante 
			//TODO : check si besoin d'un lock ou pas
			pPerSocketData->window += window;
			DEBUG_VAR("Window updated for connection %d: %d\n", pPerSocketData->id, pPerSocketData->window);

			//si aucun IO est pending sur la co correspondante, on en relance une:
			if(pPerSocketData->pPerIoData->ioType == NONE_PENDING && pPerSocketData->waitForWindow == TRUE){
				//On check que la window en face soit suffisamment grande pour lancer un recv de la taille du wsabuf
				DEBUG_VAR("window : %d, bufferlen : %d\n",pPerSocketData->window ,pPerSocketData->pPerIoData->wsabuf.len)
				if(pPerSocketData->window > pPerSocketData->pPerIoData->wsabuf.len){

					//Si c'est ok on lance un recv
					DWORD flags =0;
					pPerSocketData->pPerIoData->ioType=RECEPTION;
					
					rc = WSARecv(pPerSocketData->socket, &(pPerSocketData->pPerIoData->wsabuf), 1,NULL, &flags, &(pPerSocketData->pPerIoData->wsaoverlapped), NULL);

					if(rc == SUCCESS){
					DEBUG("WSARecv returned true\n");
					
				
					}
					else{
						rc = WSAGetLastError();
						if(rc==WSA_IO_PENDING){
							DEBUG("WSARecv returned IO PENDING\n");
						}
						else if (rc==WSAEFAULT || rc==WSAENOBUFS){//TODO check other errors that force us to exit
							pPerSocketData->pPerIoData->ioType = NONE_PENDING;
							DEBUG_ALWAYS_VAR("error wsarecv. lasterror: %d\n", rc);
							exitcode = FAILURE;
							goto ExitMain;
						}
						//TODO do some errors allow us to try WSARecv again ? 

						else{//otherwise we just close the socket
							DEBUG("wsarecv error")
							pPerSocketData->pPerIoData->ioType = NONE_PENDING;
							DEBUG_ALWAYS_VAR("StartClosing - Error WSARecv: %d\n", rc);
							if(SUCCESS != StartClosing(pPerSocketData, vcInfo, pIcaSendBuf)){
								DEBUG_ALWAYS("error start closing\n");
								exitcode = FAILURE;
								goto ExitMain;
							}	
						}
					}
			
				}
			}
            break;

        default:
            DEBUG_VAR("FATAL ERROR : Unknown packet type : %d. Exiting\n ", packet.type);
            exitcode = 1; //TODO : add more precise exitcodes?
            goto ExitMain;
        }

        
        

    }

ExitMain:
	DEBUG_ALWAYS("Entering ExitMain: \n");

	//terminate thread if not already terminated
	if(WAIT_OBJECT_0 != WaitForSingleObject(hSocketRecvWorkerThread,0)){
		//le thread n'est pas terminé, on post on packet pour qu'il se termine, et on attend
		PostQueuedCompletionStatus(hCompletionPortSocket,(DWORD)1234,(ULONG_PTR)NULL, NULL);
		DEBUG_ALWAYS("Completion packet posted to close the thread, waiting\n");
		//on attend
		WaitForSingleObject(hSocketRecvWorkerThread,INFINITE);
		
	}
	DEBUG_ALWAYS("Thread closed\n");


	//terminate thread if not already terminated
	if(WAIT_OBJECT_0 != WaitForSingleObject(hReverseBindWorkerThread,0)){
		//le thread n'est pas terminé, on post on packet pour qu'il se termine, et on attend
		PostQueuedCompletionStatus(hCompletionPortReverseBind,(DWORD)1234,(ULONG_PTR)NULL, NULL);
		DEBUG_ALWAYS("Completion packet posted to close the thread, waiting\n");
		//on attend
		WaitForSingleObject(hReverseBindWorkerThread,INFINITE);
		
	}
	DEBUG_ALWAYS("Thread closed\n");

	da_free(&daIdMapping); //pas besoing de free(&daIdMapping) car daIdMapping est alloué sans malloc donc il va disparaitre quand main() termine
	DEBUG_ALWAYS("daIdMapping freed\n");

    //Closing the virtual channel
    if (vcInfo.hVC) {
	    if(TRUE != WFVirtualChannelClose(vcInfo.hVC)){
            DEBUG_ALWAYS("Could not close the Virtual channel\n");
        }
        DEBUG_ALWAYS("Virtual channel closed\n");
	}

	//Close completion port
	// The I/O completion port handle and every file handle associated with that particular I/O completion port are known as references to the I/O completion port. The I/O completion port is released when there are no more references to it. Therefore, all of these handles must be properly closed to release the I/O completion port and its associated system resources. After these conditions are satisfied, close the I/O completion port handle by calling the CloseHandle function.
    //TODO : close all 
	CloseAllConnections(pConnectionList);
	pConnectionList = NULL;
	DEBUG_ALWAYS("CloseAllConnections() done\n");



	CloseHandle(hCompletionPortSocket);
	DEBUG_ALWAYS("CloseHandle(hCOmpletionPort) done\n");

	//Cleaning Winsock data 
	WSACleanup();
	DEBUG_ALWAYS("WSACleanup() done\n");

	//Deleting critical section
	DeleteCriticalSection(&g_criticalSection);
	DEBUG_ALWAYS("Critical section object deleted\n");

    DEBUG_ALWAYS("Returing from main()\n");
	return(exitcode);

} /* main() */

DWORD WINAPI ReverseBindWorkerThread(LPVOID args){
	DEBUG("ReverseBnd Worker Thread\n");
	BOOL status;

	LPFN_ACCEPTEX AcceptEx = ((pREVERSEBIND_WORKER_THREAD_ARGS)args)->AccecptEx;
	HANDLE completionPort  = ((pREVERSEBIND_WORKER_THREAD_ARGS)args)->completionPort;
	pPER_REVERSEBIND_DATA * ppReverseBindList = ((pREVERSEBIND_WORKER_THREAD_ARGS)args)->ppReverseBindList;
	HANDLE completionPortReverseBind = ((pREVERSEBIND_WORKER_THREAD_ARGS)args)->reverseBindCompletionPort;
	VC_INFO vcInfo = ((pREVERSEBIND_WORKER_THREAD_ARGS)args)->vcInfo;
	pPER_SOCKET_DATA * ppConnectionList = ((pREVERSEBIND_WORKER_THREAD_ARGS)args)->ppConnectionList;
	pDYN_ARRAY p_daTmpIdMapping = ((pREVERSEBIND_WORKER_THREAD_ARGS)args)->p_daTmpIdMapping;

	DWORD bytesTransferred = 0;
	CHAR pIcaSendBuf[3000];
	pPER_REVERSEBIND_DATA pPerReverseBindData = NULL;
	pPER_ACCEPT_DATA pPerAcceptData = NULL;
	LPWSAOVERLAPPED lpWsaOverlapped = NULL;
	pPER_SOCKET_DATA pPerSocketData = NULL;

	DWORD error;

	while(TRUE){
		DEBUG("------------- LEAVE thread critical section --- \n\n")
		LeaveCriticalSection(&g_criticalSection);
		//on ecoute en boucle l'arrivée de completion packets pour les traiter
		//timeout de 10s pour quand meme pouvoir faire des trucs meme si rien n'arrive
		status = GetQueuedCompletionStatus(completionPortReverseBind, &bytesTransferred, (PULONG_PTR)&pPerReverseBindData, (LPOVERLAPPED *)&lpWsaOverlapped, INFINITE); //TODO peut etre remettre un timeout

		EnterCriticalSection(&g_criticalSection);
		//DEBUG_ALWAYS("enter critical\n")
		DEBUG("===== ENTER thread critical section ==============  \n");
		DEBUG("Packet dequeued\n");
		
		//avant tout, on check si le reversebind associé est en cours de fermeture
		if(lpWsaOverlapped != NULL && pPerReverseBindData != NULL){
			DEBUG("test1\n");
			
			if(NULL ==(pPerAcceptData = (pPER_ACCEPT_DATA)CONTAINING_RECORD(lpWsaOverlapped, PER_ACCEPT_DATA, wsaoverlapped))){
				DEBUG_ALWAYS("Could not get the parent PER_ACCEPT_DATA from lpWsaOverlapped\n");
				//TODO gerer la terminaison du thread ? 
				return(0);
			}

			
			pPerAcceptData->ioType = NONE_PENDING; // on passe a none_pending

			if(pPerReverseBindData->closingState == REQ_RECEIVED_TO_CLOSE || pPerReverseBindData->closingState == ACK_RECEIVED_TO_CLOSE){
				DEBUG("test2\n");
				//pas besoin de checker les IO PENDING pcq il n'y a toujours qu 'un seul AcceptEx() a la fois
				ID reverseBindId = pPerReverseBindData->id;
				// on close tout et on envoie un ACK si on est en REQ_RECEIVED_TO_CLOSE
				if(pPerReverseBindData->closingState == REQ_RECEIVED_TO_CLOSE){
					if(SUCCESS != Send_Ica_ReverseCloseAck(vcInfo, pIcaSendBuf, reverseBindId)){
						DEBUG_ALWAYS("Could not send ICA reverse close ACK\n");
						return(1);
					}
				}

				
				if(SUCCESS != CloseReverseBind(ppReverseBindList, pPerReverseBindData)){
					DEBUG_ALWAYS("Could not close Reverse bind\n");
					return(1);
				}
				pPerReverseBindData = NULL;


				DEBUG_VAR("Closing for reverse bind %d  completed\n", reverseBindId);
				continue;
			}
			else if(pPerReverseBindData->closingState == REQ_SENT){
				//ACK pas encore recu donc on doit pas encore supprimer la connexion mais on drop le completion paquet et on relance pas d'IO
				
				DEBUG("ACK not received yet, do nothing\n");
				continue;
			}
		
			// closingState==NONE on fait rien

		}
		

		if(status == FALSE && lpWsaOverlapped == NULL){
			DEBUG("Completion status FALSE && overlapped pointer NULL\n");
			error = GetLastError();
			switch(error)
			{
			case ERROR_ABANDONED_WAIT_0: //failed because completion port was closed (see https://docs.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-getqueuedcompletionstatus)
				DEBUG_ALWAYS("LastError: ERROR_ABANDONED_WAIT_O. Completion port might have been closed.\n")
				/* code */ //TODO: gerer la terminaison du thread.
				return(1);
				break;

			case WAIT_TIMEOUT:
				DEBUG("TIMEOUT fired\n");
				//TODO timeout stuff
				

				break;
			
			default: //might be because of the timeout 
				DEBUG_WARN_VAR("LastError : %d. UNKNOWN.\n", GetLastError());
				break;

			}
		}
		else if(status == FALSE && lpWsaOverlapped != NULL){
			DEBUG("Completion status FALSE && overlapped pointer is set. Indicates asynchronous operation failure\n");

			
			DEBUG_VAR("LastError: %d, wsalasterror: %d\n", GetLastError(), WSAGetLastError());

			//pas besoin de switch sur l'io type pcq c'est toujours des AcceptEx()

			DEBUG_WARN("Received a failed AcceptEx()\n");
			//TODO gerer les failed acceptex()

			//TODO: en fonction de l'erreur, soit on relance un acceptex, soit on ferme le reverse bind listener. Pour l'instant, on ferme dans tous les cas 
			closesocket(pPerReverseBindData->listeningSocket);
			pPerReverseBindData->listeningSocket = INVALID_SOCKET;

			if(SUCCESS != Send_Ica_ReverseCloseRequest(vcInfo, pIcaSendBuf, pPerReverseBindData->id)){
					DEBUG_ALWAYS("Could not send reverse close request\n");
					return(0);
			}

			pPerReverseBindData->closingState = REQ_SENT;
			//on retourne au debut de la boucle
			continue;


		}

		else{ //... if status == TRUE

			if(pPerAcceptData==NULL){
				//should not be null if status is TRUE (except if PostCompletion avec NULL dans le overlapped)
				DEBUG_ALWAYS("Received a successful packet with NULL completionkey. Indicates that the thread must exit.\n ");
				

				//petit check en plus pour etre sur que ca vient du bon PostCompletionStatus()
				if((DWORD)1234 == bytesTransferred ){
					//c'est bien le signal qu'il faut fermer
					
					//TODO: check qu'il y a rien d'autre a faire dans le thread avant de return
					return(0);
				}
			}
			if(lpWsaOverlapped==NULL){
				DEBUG_ALWAYS("Error,lpWsaOverlapped is NULL whereas GetQueuedCompletionStatus succeeded\n");
			}


			//pas besoin de switch sur l'io type, juste traiter les AcceptEx()

			//TODO: gerer les successful AcceptEx()

			SOCKET acceptSocket = pPerAcceptData->acceptSocket;
			if(NULL == (pPerSocketData = NewConnection(acceptSocket, acceptSocket))){
				DEBUG_ALWAYS("Could not create a new connection\n");
				closesocket(acceptSocket);
				acceptSocket = INVALID_SOCKET;

			}
			else{
				//updateiocp
				if(SUCCESS != UpdateIOCP(completionPort, acceptSocket, pPerSocketData)){
					DEBUG_ALWAYS("  ERROR  Could not update completion port\n");
					FreeConnection(pPerSocketData);
					pPerSocketData = NULL;
					closesocket(acceptSocket);
					acceptSocket = INVALID_SOCKET;
				}else{
					
					if(NULL != da_get(*p_daTmpIdMapping, pPerSocketData->id)){
						DEBUG_ALWAYS_VAR("tmpid %d is already taken\n", pPerSocketData->id)
						exit(1);

					}
					if(SUCCESS != da_insert(p_daTmpIdMapping, pPerSocketData->id, pPerSocketData)){
						DEBUG_ALWAYS("  ERROR  could not da_insert\n");
						FreeConnection(pPerSocketData);
						pPerSocketData = NULL;
						closesocket(acceptSocket);
						acceptSocket = INVALID_SOCKET;
					}else{
						if(SUCCESS != Send_Ica_ReverseConnectCmdPacket(vcInfo, pIcaSendBuf, pPerSocketData->id, pPerReverseBindData->id)){
							DEBUG_ALWAYS("  ERROR  Could not send the ica reverse connect command\n");
							FreeConnection(pPerSocketData);
							pPerSocketData = NULL;
							closesocket(acceptSocket);
							acceptSocket = INVALID_SOCKET;
						}else{
							pPerSocketData->state = REVERSE_CONNECT_CMD_SENT;
							AddConnection(ppConnectionList, pPerSocketData);
						}
						
					}
				}

			}

			

			// On relance un AcceptEx()


			SOCKET newAcceptSocket = INVALID_SOCKET;
			if(INVALID_SOCKET == (newAcceptSocket = socket(AF_INET, SOCK_STREAM, 0))){
				DEBUG_ALWAYS("error creating accept socket\n");
				//close le reverse bind
				//Pour close le reverse bind, on close la socket, on envoie une close req, on passe le state a req_sent et on relance pas 
						
				closesocket(pPerReverseBindData->listeningSocket);
				pPerReverseBindData->listeningSocket = INVALID_SOCKET;

				if(SUCCESS != Send_Ica_ReverseCloseRequest(vcInfo, pIcaSendBuf, pPerReverseBindData->id)){
					DEBUG_ALWAYS("Could not send reverse close request\n");
					return(0);
				}

				pPerReverseBindData->closingState = REQ_SENT;
		
				//on retourne au debut de la boucle sans relancer d'acceptex
				continue;
			}

			pPerReverseBindData->pPerAcceptData->acceptSocket = newAcceptSocket;
			
			
			//On lance un acceptex()
			DWORD bytesReceived= 0;
			int rc =0;
			pPerReverseBindData->pPerAcceptData->ioType = ACCEPTING;
			rc = AcceptEx(pPerReverseBindData->listeningSocket, pPerReverseBindData->pPerAcceptData->acceptSocket, pPerReverseBindData->pPerAcceptData->buffer, 0,sizeof (struct sockaddr_in) + 16,sizeof (struct sockaddr_in) + 16,&bytesReceived,&(pPerReverseBindData->pPerAcceptData->wsaoverlapped));

	
			DEBUG_VAR("rc :%d\n", rc);
			if(rc == TRUE){
				DEBUG_ALWAYS("Should not succeed immediately, weird\n ");
			}
			else {
				rc = WSAGetLastError();
				if(rc != 997){
					DEBUG_ALWAYS_VAR("errorcode = %d, which is not IO_PENDING\n", rc);

					//Passer en NONE_PENDING !!!!!!
					pPerReverseBindData->pPerAcceptData->ioType = NONE_PENDING;
					
					closesocket(pPerReverseBindData->listeningSocket);
					pPerReverseBindData->listeningSocket = INVALID_SOCKET;

					if(SUCCESS != Send_Ica_ReverseCloseRequest(vcInfo, pIcaSendBuf, pPerReverseBindData->id)){
						DEBUG_ALWAYS("Could not send reverse close request\n");
						return(0);
					}

					pPerReverseBindData->closingState = REQ_SENT;
					
					break;
					
			
				}
			}
		}


	}//while(true)


	return(0);
}


// Thread qui recupere les paquets de completion (domain name resolu, socket connectée, reception terminée) et qui traite tout ca
DWORD WINAPI SocketRecvWorkerThread(LPVOID args) {
	
	HANDLE completionPort = ((pSOCKET_RECV_WORKER_THREAD_ARGS)args)->completionPort;
	VC_INFO vcInfo = ((pSOCKET_RECV_WORKER_THREAD_ARGS)args)->vcInfo;
	pDYN_ARRAY p_daIdMapping = ((pSOCKET_RECV_WORKER_THREAD_ARGS)args)->p_daIdMapping;
	pPER_SOCKET_DATA * ppConnectionList = ((pSOCKET_RECV_WORKER_THREAD_ARGS)args)->ppConnectionList;
	LPFN_CONNECTEX ConnectEx = ((pSOCKET_RECV_WORKER_THREAD_ARGS)args)->ConnectEx;
	CHAR pIcaSendBuf[3000];
	DWORD bytesTransferred = 0;
	pPER_SOCKET_DATA pPerSocketData;
	pPER_IO_DATA pPerIoData = NULL;
	LPWSAOVERLAPPED lpWsaOverlapped = NULL;
	BOOL status;
	DWORD error;
	DWORD lasterror;
	IO_TYPE ioType = NONE_PENDING;
	int rc;

	DEBUG_ALWAYS("Worker thread started successfully !\n");
	
	
	while(TRUE){
		DEBUG("------------- LEAVE thread critical section --- \n\n")
		LeaveCriticalSection(&g_criticalSection);
		//on ecoute en boucle l'arrivée de completion packets pour les traiter
		//timeout de 10s pour quand meme pouvoir faire des trucs meme si rien n'arrive
		status = GetQueuedCompletionStatus(completionPort, &bytesTransferred, (PULONG_PTR)&pPerSocketData, (LPOVERLAPPED *)&lpWsaOverlapped, (DWORD)10000);

		EnterCriticalSection(&g_criticalSection);
		//DEBUG_ALWAYS("enter critical\n")
		DEBUG("===== ENTER thread critical section ==============  \n");

		//Avant toute chose, on regarde si la connexion associée est en cours de fermeture car dans ce cas la, pas besoin de distinguer le type d'IO, on ferme tout simplement
		if(lpWsaOverlapped != NULL && pPerSocketData != NULL){
			//on passe l'io qui vient de terminer a NONE_PENDING car on va pas avoir besoin de savoir quel type d'IO c'etait, on veut juste close, pas besoin de traiter le paquet de completion, que l'IO soit reussi ou aborted

			if(NULL ==(pPerIoData = (pPER_IO_DATA)CONTAINING_RECORD(lpWsaOverlapped,PER_IO_DATA, wsaoverlapped))){
				DEBUG_ALWAYS("Could not get the parent PER_IO_DATA from lpWsaOverlapped\n");
				return(1);
			} //on recupere toute la structure PER_IO_DATA a partir de l'adresse du field wsaoverlapped qui est lui meme retourné par GetQueuedCompletionStatus
			
			ioType = pPerIoData->ioType; // on garde le type d'io pour le switch plus bas
			pPerIoData->ioType = NONE_PENDING; // on passe a none_pending

			if(pPerSocketData->closingState == REQ_RECEIVED_TO_CLOSE ){
				if(pPerSocketData->pPerIoData->ioType == NONE_PENDING && pPerSocketData->pPerIoDataSending->ioType == NONE_PENDING){
					DEBUG("No I/O pending, closing immediately\n");
					//si aucune opération I/O n'est en cours 
					ID id = pPerSocketData->id;
					RemoveConnection(ppConnectionList, pPerSocketData);
					
					FreeConnection(pPerSocketData);
					DEBUG("1");
					da_insert(p_daIdMapping,pPerSocketData->id, NULL);
					pPerSocketData=NULL;
					DEBUG("2");

					if(SUCCESS != Send_Ica_CloseAck(vcInfo, pIcaSendBuf, id)){
						DEBUG_ALWAYS("Error send ica close ack\n");
						return(1);
					}
					DEBUG_VAR("CLOSE_REQ for connection %d completed, ACK sent\n", id);
					continue;
				}
				else{
					//at least one I/O operation pending
					DEBUG("At least one I/O operation pending, waiting\n");
					
					continue;
				}
			}
			else if(pPerSocketData->closingState == REQ_SENT){
				//ACK pas encore recu donc on doit pas encore supprimer la connexion mais on drop le completion paquet et on relance pas d'IO
				
				DEBUG("ACK not received yet, do nothing\n");
				continue;
			}
			else if(pPerSocketData->closingState == ACK_RECEIVED_TO_CLOSE){
				//ACK received, 
				DEBUG("ACK received\n");
				if(pPerSocketData->pPerIoData->ioType == NONE_PENDING && pPerSocketData->pPerIoDataSending->ioType == NONE_PENDING){
					DEBUG("No I/O pending, closing immediately\n");
					//si aucune opération I/O n'est en cours 
					ID id = pPerSocketData->id;
					RemoveConnection(ppConnectionList, pPerSocketData);
					
					FreeConnection(pPerSocketData);
					pPerSocketData=NULL;

					da_insert(p_daIdMapping,id, NULL);
					

					DEBUG_VAR("CLOSE_REQ for connection %d completed, ACK sent\n", id);
					continue;
				}
				else{
					//at least one I/O operation pending
					DEBUG("At least one I/O operation pending, do nothing\n");
					
					continue;
				}
			}
			
			//closingState == NONE, on fait rien
			
		}

		if(status == FALSE && lpWsaOverlapped == NULL){
			DEBUG("Completion status FALSE && overlapped pointer NULL\n");
			error = GetLastError();
			switch (error)
			{
			case ERROR_ABANDONED_WAIT_0: //failed because completion port was closed (see https://docs.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-getqueuedcompletionstatus)
				DEBUG_ALWAYS("LastError: ERROR_ABANDONED_WAIT_O. Completion port might have been closed.\n")
				return(1);
				break;

			case WAIT_TIMEOUT:
				DEBUG("TIMEOUT fired\n");
				//TODO timeout stuff
				

				break;
			
			default: //might be because of the timeout 
				DEBUG_WARN_VAR("LastError : %d. UNKNOWN.\n", GetLastError());
				break;
			}			
		}
		else if (status == FALSE && lpWsaOverlapped != NULL){
			DEBUG("Completion status FALSE && overlapped pointer is set. Indicates asynchronous operation failure\n");

			lasterror = WSAGetLastError();
			DEBUG_VAR("LastError: %d, wsalasterror: %d\n", GetLastError(), WSAGetLastError());
			

			if(NULL ==(pPerIoData = (pPER_IO_DATA)CONTAINING_RECORD(lpWsaOverlapped,PER_IO_DATA, wsaoverlapped))){
				DEBUG_ALWAYS("Could not get the parent PER_IO_DATA from lpWsaOverlapped\n");
				return(1);
			} //on recupere toute la structure PER_IO_DATA a partir de l'adresse du field wsaoverlapped qui est lui meme retourné par GetQueuedCompletionStatus


			//Si la connection est marquée CLOSING, on sait que c'est géré et on ignore les packets

			switch (ioType)
			{

			case NONE_PENDING:
				DEBUG_ALWAYS("Received a failed NONE_PENDING I/O packet\n");
				//pas normal, si none pending on doit pas recevoir de completion packet
				return(1);
				
			case RESOLUTION_FAIL:
			case RESOLUTION_SUCCESS:
				DEBUG_ALWAYS("Received a failed RESOLUTION I/O packet\n");
				// Pas censé etre possible, POstQueued... donne toujours des success ? 
				return(1);
				
			
			case CONNECTION:
				DEBUG("Received a failed CONNECTION I/O packet\n");

	

				DEBUG_VAR("Connection failed on socket : %d\n", pPerSocketData->socket);
				if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pPerSocketData->id, 0x05, 0x01)){
					DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
					
					return(1);
				}
				//Les serverRplPacket avec un status a 0x00 genere un envoi de 10octets sur la socket coté client, donc on update la window
				pPerSocketData->window -= 10;

				//Pas besoin de fermer la connexion, en recevant le rpl packet, le client transmet l'echec a sa socket qui se close, on recevra donc une CLOSE_REQUEST
				break;

			case RECEPTION:
				DEBUG("Received a failed RECEPTION I/O packet\n");
				//Je sais pas vraiment quand est ce que ca arrive :) Peut etre si on essaye de run WSARecv() sur une socket fermée ou un truc comme ca.
				// En fait ca peut arriver quand on utilise CancelIo, ou quand on close une socket

				//Si on arrive jusque la c'est que la connexion est en closingState==NONE, sinon c'est géré juste apres le GetQueuedCompletion..()
				//Parmi les erreurs possibles de WSARecv(), certaines obligent a terminer le programme, d'autres a close la connexion, d'autres seulement a retenter un WSARecv()
				//TODO: est ce qu'on s'embete a trouver des cas ou on peut relancer, ou alors on close la socket a chaque fois ? 
				if(lasterror == WSAEFAULT ){ //TODO : completer les cas qui obligent a exit
					DEBUG_ALWAYS_VAR("Last error : %d\n", lasterror);
					return(1);
				}
				else{
					DEBUG_ALWAYS_VAR("StartClosing - Failed RECEPTION: %d\n", lasterror);
					if(SUCCESS != StartClosing(pPerSocketData, vcInfo, pIcaSendBuf)){
						DEBUG_ALWAYS("error start closing\n");
						return(1);
					}
				}

				break;

			case SENDING:
				DEBUG("Received a failed SENDING I/O packet\n");
				//TODO
				// Si arrive la c'est que closingState==NONE

				//3 cas possibles: exit, close connection, new wsasend
				//TODO: trouver des cas ou on peut juste relancer sans close la co ?
				if(lasterror == WSAEFAULT || lasterror == WSAENOBUFS){ //TODO: completer les cas qui obligent a exit
					DEBUG_ALWAYS_VAR("Last error : %d\n", lasterror);
					return(0);
				}
				else{
					DEBUG_ALWAYS_VAR("StartClosing - Failed SENDING: %d\n", lasterror);
					if(SUCCESS != StartClosing(pPerSocketData, vcInfo, pIcaSendBuf)){
						DEBUG_ALWAYS("error start closing\n");
						return(0);
					}
				}

				break;
			
			default:
				DEBUG_ALWAYS("Unknown ioType\n");
				return(1);
				break;
			}
			

		}

		else{ //...if status == TRUE
			DEBUG("else \n");

			if(pPerSocketData==NULL){
				//should not be null if status is TRUE (except if PostCompletion avec NULL dans le overlapped)
				DEBUG_ALWAYS("Received a successful packet with NULL completionkey. Indicates that the thread must exit.\n ");
				

				//petit check en plus pour etre sur que ca vient du bon PostCompletionStatus()
				if((DWORD)1234 == bytesTransferred ){
					//c'est bien le signal qu'il faut fermer
					
					//TODO: check qu'il y a rien d'autre a faire dans le thread avant de return
					return(0);
				}
				
			}
			if(lpWsaOverlapped==NULL){
				DEBUG_ALWAYS("Error, lpWasOverlapped is NULL whereas GetQueuedCompletionStatus succeede\n");
				return(1); 
			}


			// plus besoin de faire ca la car c'est fait au debut avec les check sur la fermeture :)
			// if(NULL ==(pPerIoData = (pPER_IO_DATA)CONTAINING_RECORD(lpWsaOverlapped,PER_IO_DATA, wsaoverlapped))){
			// DEBUG_ALWAYS("Error getting the parent PER_IO_DATA from lpWsaOverlapped\n");
			// exit(1);
			// } //on recupere toute la structure PER_IO_DATA a partir de l'adresse du field wsaoverlapped qui est lui meme retourné par GetQueuedCompletionStatus
			
	

			//on regarde a quoi correspond le paquet de completion (resolution de nom terminee, socket connectee, reception de donnees terminee)
			switch(ioType){

				case NONE_PENDING:
					DEBUG_ALWAYS("Received a successful NONE_PENDING I/O packet\n");
					//pas normal, si none pending on doit pas recevoir de completion packet
					return(1);
					break;

				case RESOLUTION_FAIL:
					DEBUG("Received a RESOLUTION_FAIL I/O packet\n");
					if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pPerSocketData->id, 0x05, 0x01)){
						DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
						return(1);
					}

					break;
				case RESOLUTION_SUCCESS:
					DEBUG("Received a RESOLUTION_SUCCESS I/O packet\n");

					//on lance la connexion avec les parametres d'addresse trouvés par la resolution

					
					pPerSocketData->pPerIoData->ioType = CONNECTION;
					DEBUG_VAR("socket : %d\n", pPerSocketData->socket);
			
					rc = ConnectEx(pPerSocketData->socket, (const struct sockaddr *)&(pPerSocketData->server),sizeof(struct sockaddr_in), NULL, 0, NULL, &(pPerSocketData->pPerIoData->wsaoverlapped));


					DEBUG_VAR("rc :%d\n", rc);
					if(rc == TRUE){
						DEBUG_ALWAYS("Should not succeed immediately, weird\n ");
					}
					else {
						rc = WSAGetLastError();
						if(rc != 997){
							
							DEBUG_ALWAYS_VAR("errorcode = %d, which is not IO_PENDING\n", rc);
							pPerSocketData->pPerIoData->ioType = NONE_PENDING;
							if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pPerSocketData->id, 0x05, 0x01)){
								DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
								return(1);
							}
						
							//Les serverRplPacket avec un status a 0x00 genere un envoi de 10octets sur la socket coté client, donc on update la window
							pPerSocketData->window -= 10;

							// da_insert(p_daIdMapping,pPerSocketData->id, NULL);
							// RemoveConnection(ppConnectionList, pPerSocketData);
							// FreeConnection(pPerSocketData);
							// pPerSocketData = NULL;
							// closesocket(pPerSocketData->socket);
							// pPerSocketData->socket = INVALID_SOCKET;
							// DEBUG_WARN("Error connectex\n")
							// break;
							
							

							//Pas besoin de fermer la connexion, en recevant le rpl packet, le client transmet l'echec a sa socket qui se close, on recevra donc une CLOSE_REQUEST
						}
					}

					//On passe la connexion en CONNECTING;
					pPerSocketData->state = CONNECTING;
					
					DEBUG_VAR("%d\n",rc)
					
				break;

				case CONNECTION: //means this IO was posted by ConnectEx() connection should have succeeded
					DEBUG("Received a successful CONNECTION I/O packet\n");
					DEBUG_VAR("socket : %d\n", pPerSocketData->socket);
					
					//On set les options de la socket, notamment pour qu'elle apparaisse connectée si elle l'est.
					if (SUCCESS != setsockopt( pPerSocketData->socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0 )){
						DEBUG_ALWAYS("error setsockopt() \n");
						return(1);
					}

					//Check si la socket est effectivement connectée 
		

					struct sockaddr name;
					int namelen = sizeof(name);
					if (SUCCESS != getpeername(pPerSocketData->socket, (struct sockaddr *)&name, &namelen)){ // en fait on est pas connectés, donc meme traitement que pour la reception d'un failed CONNECTION completion packet
						DEBUG_VAR("getlasterror : %d\n", WSAGetLastError());
						if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pPerSocketData->id, 0x05, 0x01)){
							DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
							//TODO : gerer la terminaison
							return(0);
						}
						//Les serverRplPacket avec un status a 0x00 genere un envoi de 10octets sur la socket coté client, donc on update la window
						pPerSocketData->window -= 10;

						//Pas besoin de fermer la connexion, en recevant le rpl packet, le client transmet l'echec a sa socket qui se close, on recevra donc une CLOSE_REQUEST
						break;
					}
					else{
						DEBUG("Connection established\n");
					}

					// Autre methode pour checker que la connexion est bien faite : 
					// int seconds;
					// int bytes = sizeof(seconds);
					// int iResult = 0;
					// iResult = getsockopt(pPerSocketData->socket, SOL_SOCKET, SO_CONNECT_TIME,(char *)&seconds, (PINT)&bytes );
					// if ( iResult != NO_ERROR ) {
					// 	DEBUG_VAR( "getsockopt(SO_CONNECT_TIME) failed with error: %u\n", 
					// 		WSAGetLastError() );
					// }
					// else {
					// 	if (seconds == 0xFFFFFFFF){
					// 		DEBUG("Connection not established yet\n");
					// 	}
					// 	else{
					// 	DEBUG_VAR("Connection has been established %ld seconds\n",
					// 		seconds);
					// 	}
					// }


					if(SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pPerSocketData->id, 0x05, 0x00)){
						DEBUG_ALWAYS("Error send_ica_serverplpacket, exiting")
						//TODO : gerer la terminaison
						return(1);
					}
					//Les serverRplPacket avec un status a 0x00 genere un envoi de 10octets sur la socket coté client, donc on update la window
					pPerSocketData->window -= 10;

					pPerSocketData->state = ESTABLISHED;

					

					//On check que la window en face soit suffisamment grande pour lancer un recv de la taille du wsabuf
					DEBUG_VAR("window : %d, bufferlen : %d\n",pPerSocketData->window ,pPerIoData->wsabuf.len )
					if(pPerSocketData->window > pPerIoData->wsabuf.len){

						//Si c'est ok on lance un recv
						DWORD flags =0;
						pPerIoData->ioType=RECEPTION;
						//TODO : check status for wsarecv
						rc = WSARecv(pPerSocketData->socket, &(pPerIoData->wsabuf), 1,NULL, &flags, &(pPerSocketData->pPerIoData->wsaoverlapped), NULL);

						if(rc == SUCCESS){
							DEBUG("WSARecv returned true\n");
							
							//TODO : on fait quoi la ?
						}
						else{
							rc = WSAGetLastError();
							if(rc==WSA_IO_PENDING){
								DEBUG("WSARecv returned IO PENDING\n");
							}
							else if (rc==WSAEFAULT || rc==WSAENOBUFS){//TODO trouver d'autres cas qui obligent a exit
								pPerIoData->ioType = NONE_PENDING;
								DEBUG_ALWAYS_VAR("error wsarecv. lasterror: %d\n", rc);
								return(1);
							}
							//TODO : voir si dans certains cas on peut juste relancer ??

							else{//sinon on se contente de close la socket
								DEBUG("wsarecv error")
								pPerIoData->ioType = NONE_PENDING;
								DEBUG_ALWAYS_VAR("StartClosing - Error WSARecv: %d\n", rc);
								if(SUCCESS != StartClosing(pPerSocketData, vcInfo, pIcaSendBuf)){
									DEBUG_ALWAYS("error start closing\n");
									return(1);
								}	
							}
						}
					
					}
					else{
						//TODO: comment faire pour signaler qu'on relance pas de recv et qu'il faudra le faire des que la window regrossit ????
						DEBUG("No window, pause connection\n")
						pPerIoData->ioType=NONE_PENDING;
					}


					

					
				break;

				case RECEPTION:
					DEBUG("Received a successful RECEPTION I/O packet\n");
					DEBUG_VAR("Received %d bytes from socket %d\n", bytesTransferred, pPerSocketData->socket);

				

					
					//si bytesTransferred == 0, c'est que la socket veut se fermer
					if(bytesTransferred==0){
						DEBUG("The socket wants to close.\n");
						DEBUG("StartClosing - Socket closed connection\n");
						if(SUCCESS != StartClosing(pPerSocketData, vcInfo, pIcaSendBuf)){
							DEBUG_ALWAYS("error start closing\n");
							return(0);
						}

					}
					//si bytesTransferred != 0, on a effectivement recu des données, donc il faut les transmettre a ICA
					else{
						DEBUG("Transfer the bytes to ICA\n");

						//TODO : check qu'on est pas dans le cas d'un CancelIo qui a quand meme bien terminé.Dans ce cas il faut pas transfer ni relancer de recv, mais plutot fermer la connexion et la socket. Peut etre se baser sur le state de la co pour identifier cette situation.

						//TODO check que bytesTransferres < ce qu'il faut 
						
						if(SUCCESS != Send_Ica_DataPacket(vcInfo, pIcaSendBuf, pPerSocketData->id, bytesTransferred, pPerIoData->wsabuf.buf)){
							DEBUG_ALWAYS("error send ica data packet\n");
							//TODO: close comme il faut 
						}

						//On update la window de la connexion correspondante 
						pPerSocketData->window -= bytesTransferred;

						//On check que la window en face soit suffisamment grande pour lancer un recv de la taille du wsabuf
						DEBUG_VAR("window : %d, bufferlen : %d\n",pPerSocketData->window ,pPerIoData->wsabuf.len )
						if(pPerSocketData->window > pPerIoData->wsabuf.len){

							//Si c'est ok on lance un recv
							DWORD flags =0;
							pPerIoData->ioType=RECEPTION;
					
							WSARecv(pPerSocketData->socket, &(pPerIoData->wsabuf), 1,NULL, &flags, &(pPerSocketData->pPerIoData->wsaoverlapped), NULL);
					
						}
						else{
							//TODO: comment faire pour signaler qu'on relance pas de recv et qu'il faudra le faire des que la window regrossit ????
							DEBUG("No window, pause connection\n")
							pPerSocketData->waitForWindow = TRUE;
						}
					
			
					}
				break;

				case SENDING:
					DEBUG("Received a successful SENDING I/O packet\n");
					DEBUG_VAR("Successfully sent %d bytes\n", bytesTransferred);
					pPerIoData->ioType=NONE_PENDING;
					
					//petit check pour etre sur
					if(bytesTransferred!=pPerIoData->wsabuf.len){
						DEBUG_ALWAYS("Chelou, bytesTransferrered != wsabuf.len\n");
						exit(1);
					}
					//dequeue the bytes
					if(SUCCESS != Queue_Dequeue(&(pPerSocketData->sOutQueue),bytesTransferred )){
						DEBUG_ALWAYS("error dequeuing\n");

						exit(1);
					}
					//send window packet to client
					if(SUCCESS != Send_Ica_MetaWindow(vcInfo, pIcaSendBuf, bytesTransferred, pPerSocketData->id)){
						DEBUG_ALWAYS("error send ica window\n");
						exit(1);
					}
					DEBUG("1");
					//if queue is not empty, start another send operation,
					if(Queue_IsEmpty(&(pPerSocketData->sOutQueue)) == FALSE){
						DEBUG("Starting another WSASend()\n");
						DWORD flags=0;
						DWORD bytessent;
						//debut critical section
						pPerSocketData->pPerIoDataSending->wsabuf.buf = Queue_GetHead(&(pPerSocketData->sOutQueue));
						pPerSocketData->pPerIoDataSending->wsabuf.len = Queue_GetSizeCeiled(&(pPerSocketData->sOutQueue));
						//fin critical section
						pPerSocketData->pPerIoDataSending->ioType = SENDING;
						
						rc = WSASend(pPerSocketData->socket,&(pPerSocketData->pPerIoDataSending->wsabuf), 1,&bytessent, flags , &(pPerSocketData->pPerIoDataSending->wsaoverlapped), NULL);

						if(rc == SUCCESS){
							DEBUG("WSASend returned true\n");
							
							//TODO : on fait quoi la ?
						}
						else{
							rc = WSAGetLastError();
							if(rc==WSA_IO_PENDING){
								DEBUG("WSASend returned IO PENDING\n");
							}
							else if (rc==WSAEFAULT || rc==WSAENOBUFS){//TODO trouver d'autres cas qui obligent a exit
								pPerSocketData->pPerIoDataSending->ioType = NONE_PENDING;
								
								DEBUG_VAR("error wsasend. lasterror: %d\n", rc);
								DEBUG_VAR("%d\n", pPerSocketData->pPerIoDataSending->wsaoverlapped.Internal);
								DEBUG_VAR("lbbuff: %d\n", &(pPerSocketData->pPerIoDataSending->wsabuf));
								DEBUG_VAR("lbbuff buf: %d\n", pPerSocketData->pPerIoDataSending->wsabuf.buf);
								DEBUG_VAR("lbbuff len: %d\n", pPerSocketData->pPerIoDataSending->wsabuf.len);
								DEBUG_VAR("lbbuff buf[0]: %d\n", pPerSocketData->pPerIoDataSending->wsabuf.buf[0]);
								return(1);
							}
							//TODO : voir si dans certains cas on peut juste relancer ??

							else{//sinon on se contente de close la socket
								DEBUG("wsasend error")
								pPerSocketData->pPerIoDataSending->ioType = NONE_PENDING;
								DEBUG_ALWAYS_VAR("StartClosing - Error WSASend: %d\n", rc);
								DEBUG_ALWAYS_VAR("size: %d\n",pPerSocketData->pPerIoDataSending->wsabuf.len );
								if(SUCCESS != StartClosing(pPerSocketData, vcInfo, pIcaSendBuf)){
									DEBUG_ALWAYS("error start closing\n");
									return(1);
								}	
							}
						}

							
					



					}

					//if queue is empty, none pending 
					else{
						DEBUG("Nothing more in the queue, setting overlapped to NONE_PENDING\n");
						pPerIoData->ioType = NONE_PENDING;
					}
				break;

				default:
					DEBUG_ALWAYS("Error: unknown ioType\n");
					exit(1);
				break;
			}
		}
		

		

		
	}
}

DWORD WINAPI ResolveDNS(LPVOID args){
	
	DEBUG("Entering\n");
	int rc = 0;
	//int rcConnect = 0;
	PADDRINFOA  pResult = NULL;
	pRESOLVE_THREAD_ARGS pResolveThreadArgs = (pRESOLVE_THREAD_ARGS)args;
	LPCRITICAL_SECTION pCriticalSection = pResolveThreadArgs->pCriticalSection;
	//CHAR pIcaSendBuf[3000];
	struct addrinfo hints;
	hints.ai_addrlen = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_INET;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;

	DEBUG_VAR("Domain name: %s, port name : %s\n",pResolveThreadArgs->pPerSocketData->pDomainName ,pResolveThreadArgs->pPerSocketData->pPortName);
	DEBUG("1\n");

	rc = getaddrinfo(pResolveThreadArgs->pPerSocketData->pDomainName, pResolveThreadArgs->pPerSocketData->pPortName, &hints, &pResult);

	DEBUG("2\n");
	EnterCriticalSection(pResolveThreadArgs->pCriticalSection);
	DEBUG(" --- Enter ResolveThread critical section --------------\n");

	

	if(SUCCESS == rc){
		pResolveThreadArgs->pPerSocketData->state = RESOLVED;
		DEBUG("getaddrinfo succeeded\n");
		DEBUG_VAR("addrlen : %d, addr fam : %d\n", pResult->ai_addrlen, pResult->ai_addr->sa_family);
		
		struct sockaddr_in * pSockaddrIn;
		pSockaddrIn = (struct sockaddr_in *)(&(pResolveThreadArgs->pPerSocketData->server));

		//*pSockaddrIn = *((struct sockaddr_in *) pResult->ai_addr);
		pSockaddrIn->sin_family = ((struct sockaddr_in *)pResult->ai_addr)->sin_family;
		pSockaddrIn->sin_port = ((struct sockaddr_in *)pResult->ai_addr)->sin_port;
		pSockaddrIn->sin_addr = ((struct sockaddr_in *)pResult->ai_addr)->sin_addr;



		DEBUG("0\n");
		DEBUG_VAR("IP : %s, PORT : %d\n", inet_ntoa(pSockaddrIn->sin_addr), ntohs(pSockaddrIn->sin_port))
		DEBUG_VAR("test: %d\n",pResolveThreadArgs->pPerSocketData->pPerIoData->ioType );
		pResolveThreadArgs->pPerSocketData->pPerIoData->ioType = RESOLUTION_SUCCESS;
		DEBUG("1\n");
		PostQueuedCompletionStatus(pResolveThreadArgs->hCompletionPort, 123, (ULONG_PTR)(pResolveThreadArgs->pPerSocketData), &(pResolveThreadArgs->pPerSocketData->pPerIoData->wsaoverlapped));
		DEBUG("Posted RESOLUTION SUCCESS completion packet\n");
		
	}
	else{
		pResolveThreadArgs->pPerSocketData->state = UNRESOLVED;

		DEBUG_VAR("getaddrinfo failed, error : %d\n", rc);
		
		pResolveThreadArgs->pPerSocketData->pPerIoData->ioType = RESOLUTION_FAIL;
		PostQueuedCompletionStatus(pResolveThreadArgs->hCompletionPort, 123,  (ULONG_PTR)(pResolveThreadArgs->pPerSocketData), &(pResolveThreadArgs->pPerSocketData->pPerIoData->wsaoverlapped));

	}

	

	
	if(pResult){
		DEBUG("Freeing dns result structures\n");
		freeaddrinfo(pResult);
		pResult = NULL;
	}
	


	DEBUG("Closing thread hande\n");
	CloseHandle(pResolveThreadArgs->handleSelf);

	if(pResolveThreadArgs){
		DEBUG("Freeing thread args structure\n");
		free(pResolveThreadArgs);
		pResolveThreadArgs = NULL;
	}

	
	DEBUG(" --- Leave ResolveThread critical section --------------\n");
	LeaveCriticalSection(pCriticalSection);

	DEBUG("Leaving\n");
	return(0);
}




int OpenVC(pVC_INFO pVcInfo) {
	DEBUG("Entering\n");

	DEBUG_ALWAYS("Opening virtual channel\n");
	if (NULL == (pVcInfo->hVC = WFVirtualChannelOpen(WF_CURRENT_SERVER,WF_CURRENT_SESSION,VIRTUAL_CHANNEL_NAME))) {
        DEBUG_ALWAYS("Failed opening vc...Exiting\n");
        return(FAILURE);
    }
	
	DEBUG_ALWAYS_VAR("Virtual channel opened ! handle : %p  \n", pVcInfo->hVC);
	DEBUG_ALWAYS("Querying the virtual channel data\n");
	PVDSOCKS_C2H pVdQBuf = NULL;
	DWORD dwSize;
    if(TRUE != WFVirtualChannelQuery(pVcInfo->hVC, WFVirtualClientData, (void **)&pVdQBuf, &dwSize)){
        DEBUG_ALWAYS_VAR("Query failed. GetLastError() : %d. Exiting.\n", GetLastError());
		return(FAILURE);
    }

	
	PVD_C2H pVdc2h = NULL;
	PMODULE_C2H pModulec2h = NULL;
	pVdc2h = &pVdQBuf->Header;
	pModulec2h = &pVdc2h->Header;
	DEBUG_ALWAYS_VAR("Query OK. Results : channelMask=%d, name=%s %s \n", pVdc2h->ChannelMask, (CHAR*)pModulec2h->ModuleName, (CHAR*)pModulec2h->HostModuleName);
	DEBUG("Leaving\n");
	return(SUCCESS);
}

void Parse_Ica_GenericPacket(IN char * pPacketBuf, OUT pPACKET pPacket) {
	void * address = (void *)pPacketBuf;

	pPacket->type = *((PACKET_TYPE *)address);
	address = (PACKET_TYPE *)address + 1;

	pPacket->id = htonl(*((ID *)address));
	address = (ID *)address + 1;

	pPacket->len = htons(*((LEN *)address));
	address = (LEN *)address + 1;

	pPacket->pData = (char *)address;
}


int Send_Ica_MetaWindow(VC_INFO vcInfo, char * sendingBuf, int window, ID id) {
	DEBUG("Entering\n");
	PACKET packet;
	packet.id = id;
	packet.type = META_WINDOW;
	packet.len = sizeof(int);
	packet.pData = (char *) &window;

	if (SUCCESS != Send_Ica_GenericPacket(vcInfo, sendingBuf, packet)) {
		DEBUG_ALWAYS("Error in Send_Ica_Meta()\n");
		return(FAILURE);
	}
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_MetaServerStart(VC_INFO vcInfo, char * sendingBuf) {
	DEBUG("Entering\n");

	int rc;
	PACKET packet;
	packet.id = 0;
	packet.len = 0;
	packet.pData = NULL;
	packet.type = META_SERVER_START;

	if (SUCCESS != (rc = Send_Ica_GenericPacket(vcInfo, sendingBuf, packet))) {
		DEBUG_ALWAYS_VAR("FATAL ERROR %d : Packet neither sent nor queued, exiting\n", rc);
		return(FAILURE);
	}
	DEBUG("Packet sent or queued\n");
	DEBUG("Leaving\n");

	return(SUCCESS);
}

int Send_Ica_MetaServerStop(VC_INFO vcInfo, char * sendingBuf) {
	DEBUG("Entering\n");
	
	int rc;
	PACKET packet;
	packet.id = 0;
	packet.len = 0;
	packet.pData = NULL;
	packet.type = META_SERVER_STOP;

	if (SUCCESS != (rc = Send_Ica_GenericPacket(vcInfo, sendingBuf, packet))) {
		DEBUG_ALWAYS_VAR("FATAL ERROR %d : Packet neither sent nor queued, exiting\n", rc);
		return(FAILURE);
	}
	DEBUG("Packet sent or queued\n");
	DEBUG("Leaving\n");

	return(SUCCESS);
}



int Send_Ica_CloseRequest(VC_INFO vcInfo, char * sendingBuf, ID id) {
	DEBUG("Entering\n");

	PACKET packet;
	packet.type = CLOSE_REQUEST;
	packet.id = id;
	packet.len = 0;
	packet.pData = NULL;

	if (SUCCESS != Send_Ica_GenericPacket(vcInfo, sendingBuf, packet)) {
		DEBUG_ALWAYS("FATAL ERROR : close request nor queued not sent\n");
		return(FAILURE);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}


int Send_Ica_CloseAck(VC_INFO vcInfo, char * sendingBuf, ID id) {
	DEBUG("Entering\n");

	PACKET packet;
	packet.type = CLOSE_ACK;
	packet.id = id;
	packet.len = 0;
	packet.pData = NULL;

	if (SUCCESS != Send_Ica_GenericPacket(vcInfo, sendingBuf, packet)) {
		DEBUG_ALWAYS("FATAL ERROR : close ack nor queued not sent\n");
		return(FAILURE);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_ServerRplPacket(  VC_INFO vcInfo, char * sendingBuf, ID id, VER ver, RPL rpl) {
	DEBUG("Entering\n");

	char pSocketServerRplBuf[VER_SIZE + RPL_SIZE]; //ici ou au debut ??
	void * dest = (void *)pSocketServerRplBuf;
	int rc;

	memcpy(dest, &ver, VER_SIZE);
	dest = (VER*)dest + 1;

	memcpy(dest, &rpl, RPL_SIZE);

	PACKET packet;
	packet.type = SOCKS_SERVER_RPL;
	packet.id = id;
	packet.len = sizeof(pSocketServerRplBuf);
	packet.pData = pSocketServerRplBuf;


	if (SUCCESS != (rc = Send_Ica_GenericPacket(vcInfo,sendingBuf,packet))) {
		DEBUG_ALWAYS_VAR("Packet neither sent nor queued, exiting\n", rc);
		return(rc);
	}
	DEBUG("Packet sent or queued\n");
	DEBUG("Leaving\n");

	return(SUCCESS);
}

int Send_Ica_ReverseBindRplPacket(VC_INFO vcInfo, char * sendingBuf, ID revBindId , RPL rpl) {
	DEBUG("Entering\n");

	char pSocketServerRplBuf[RPL_SIZE]; //ici ou au debut ??
	void * dest = (void *)pSocketServerRplBuf;
	int rc;



	memcpy(dest, &rpl, RPL_SIZE);

	PACKET packet;
	packet.type = REVERSE_BIND_RPL;
	packet.id = revBindId;
	packet.len = sizeof(pSocketServerRplBuf);
	packet.pData = pSocketServerRplBuf;


	if (SUCCESS != (rc = Send_Ica_GenericPacket(vcInfo,sendingBuf,packet))) {
		DEBUG_ALWAYS_VAR("Packet neither sent nor queued, exiting\n", rc);
		return(rc);
	}
	DEBUG("Packet sent or queued\n");
	DEBUG("Leaving\n");

	return(SUCCESS);
}

int Send_Ica_ReverseCloseRequest(VC_INFO vcInfo, char * sendingBuf, ID reverseBindId) {
	DEBUG("Entering\n");

	PACKET packet;
	packet.type = REVERSE_CLOSE_REQUEST;
	packet.id = reverseBindId;
	packet.len = 0;
	packet.pData = NULL;

	if (SUCCESS != Send_Ica_GenericPacket(vcInfo, sendingBuf, packet)) {
		DEBUG_ALWAYS("FATAL ERROR : reverse close request nor queued not sent\n");
		return(FAILURE);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_ReverseCloseAck(VC_INFO vcInfo, char * sendingBuf, ID reverseBindId) {
	DEBUG("Entering\n");

	PACKET packet;
	packet.type = REVERSE_CLOSE_ACK;
	packet.id = reverseBindId;
	packet.len = 0;
	packet.pData = NULL;

	if (SUCCESS != Send_Ica_GenericPacket(vcInfo, sendingBuf, packet)) {
		DEBUG_ALWAYS("FATAL ERROR : reverse close ack nor queued not sent\n");
		return(FAILURE);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_ReverseConnectCmdPacket(VC_INFO vcInfo, char * sendingBuf, ID tmpId, ID revBindId) {
	DEBUG("Entering\n");

	char pSocketServerRplBuf[ID_SIZE]; //ici ou au debut ??
	void * dest = (void *)pSocketServerRplBuf;
	int rc;

	memcpy(dest, &revBindId, ID_SIZE);
	

	PACKET packet;
	packet.type = REVERSE_CONNECT_CMD;
	packet.id = tmpId;
	packet.len = sizeof(pSocketServerRplBuf);
	packet.pData = pSocketServerRplBuf;


	if (SUCCESS != (rc = Send_Ica_GenericPacket(vcInfo,sendingBuf,packet))) {
		DEBUG_ALWAYS_VAR("Packet neither sent nor queued, exiting\n", rc);
		return(rc);
	}
	DEBUG("Packet sent or queued\n");
	DEBUG("Leaving\n");

	return(SUCCESS);
}

int Send_Ica_DataPacket(VC_INFO vcInfo, char * sendingBuf, ID id, LEN len, char * pData) {
	DEBUG("Entering\n");
	int rc;
	PACKET packet;
	packet.id = id;
	packet.len = len;
	packet.type = SOCKS_DATA;
	packet.pData = pData;

	if (SUCCESS != (rc = Send_Ica_GenericPacket(vcInfo,sendingBuf,packet))) {
		DEBUG_ALWAYS_VAR("FATAL ERROR Send_Ica_GenericPacket : %d\n", rc);
		return(FAILURE);
	}

	DEBUG("Leaving\n");

	return(SUCCESS);
}

int Send_Ica_GenericPacket(VC_INFO vcInfo, char * sendingBuf, PACKET packet) {
	DEBUG("Entering\n");
	ID netOrderId = htonl(packet.id);
	LEN netOrderLen = htons(packet.len);
	//TODO: start lock on sendingbuf, or critical section
	void* dest = (void*)sendingBuf;

	memcpy(dest, &packet.type, PACKET_TYPE_SIZE);
	dest = (PACKET_TYPE *)dest + 1;

	memcpy(dest, &netOrderId, ID_SIZE);
	dest = (ID *)dest + 1;

	memcpy(dest, &netOrderLen, LEN_SIZE);
	dest = (LEN *)dest + 1;

	if (packet.len > 0) {
		if (packet.pData) {
			memcpy(dest, packet.pData, packet.len);
		}
		else {
			DEBUG_ALWAYS("ERROR pData NULL and len >0\n");
			return(FAILURE);
		}
	}
	
	if (SUCCESS != Send_Ica_Retry(vcInfo, PACKET_TYPE_SIZE + ID_SIZE + LEN_SIZE + packet.len, sendingBuf)) {
		DEBUG_ALWAYS("Error Send_Ica_Retry : could not send nor queue the packet\n");
		return(FAILURE);
	}
	//TODO stop lock
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_Retry(VC_INFO vcInfo, LEN len, char * pBuf){
	DEBUG("Entering\n");
	
	while (SUCCESS != Send_Ica(vcInfo, len, pBuf)){
		Sleep(500);
		continue;
	}
	DEBUG("Leaving\n");
	return(SUCCESS);
}



int Send_Ica(VC_INFO vcInfo, LEN len, char * pBuf) {
	DEBUG("Entering\n");
	ULONG bytesSent = 0;
	if (TRUE != WFVirtualChannelWrite(vcInfo.hVC, pBuf, len, &bytesSent)) {
		int error = GetLastError();
		DEBUG_VAR("Error WFVirtualChannelWrite() : %d\n",error);
		switch (error)
		{	
			//TODO : se renseigner sur les erreurs possibles de WFVirtualChannelWrite et completer.
		default:
            DEBUG_ALWAYS_VAR("Error in WFVirtualChannelWrite() : %d\n", error);
			return(FAILURE);
			break;
		}
		
	}
	DEBUG("Leaving\n");
	return(SUCCESS);
}

void FreeConnection(pPER_SOCKET_DATA pPerSocketData){
	Queue_Free(&(pPerSocketData->sInQueue));
	Queue_Free(&(pPerSocketData->sOutQueue));
	pPerSocketData->socket = INVALID_SOCKET;
	free(pPerSocketData->pPerIoData);
	pPerSocketData->pPerIoData = NULL;
	DEBUG("pPErIoData freed\n");
	free(pPerSocketData->pPerIoDataSending);
	pPerSocketData->pPerIoDataSending = NULL;
	DEBUG("pPErIoDataSending freed\n");
	//free(pPerSocketData); pas une bonne idee pcq on sait pas d'ou vient pPerSocketData : la memoire a pu etre allouee statiquement et pas avec un malloc()
	DEBUG("Connection freed\n");
}

//N'appeler que sur des pPER_REVERSEBIND_DATA obtenus avec NewReverseBind
void FreeReverseBind(pPER_REVERSEBIND_DATA pPerReverseBindData){
	pPerReverseBindData->listeningSocket = INVALID_SOCKET;

	free(pPerReverseBindData->pPerAcceptData);
	pPerReverseBindData->pPerAcceptData = NULL;
	free(pPerReverseBindData);
	
	DEBUG_ALWAYS("Connection freed\n");
}



pPER_SOCKET_DATA NewConnection(ID id, SOCKET socket){
	DEBUG("entering\n");
	pPER_SOCKET_DATA pPerSocketData = NULL;
	DEBUG("0")
	if(NULL == (pPerSocketData = (pPER_SOCKET_DATA )malloc(sizeof(PER_SOCKET_DATA)))){
		DEBUG_ALWAYS("Error allocating connection memory\n");
		return (NULL);
	}

	pPerSocketData->pPerIoData = NULL;
	if(NULL == (pPerSocketData->pPerIoData = (pPER_IO_DATA)malloc(sizeof(PER_IO_DATA))) ){
		DEBUG_ALWAYS("Error allocating perIo memory for connection");
		free(pPerSocketData);
		pPerSocketData = NULL;
		return(NULL);	
	}
	pPerSocketData->pPerIoData->wsaoverlapped.Internal=0;
	pPerSocketData->pPerIoData->wsaoverlapped.InternalHigh = 0;
	pPerSocketData->pPerIoData->wsaoverlapped.Offset=0;
	pPerSocketData->pPerIoData->wsaoverlapped.OffsetHigh = 0;
	pPerSocketData->pPerIoData->wsaoverlapped.hEvent = NULL;
	pPerSocketData->pPerIoData->wsabuf.buf = pPerSocketData->pPerIoData->buffer;
	pPerSocketData->pPerIoData->wsabuf.len = sizeof(pPerSocketData->pPerIoData->buffer);
	pPerSocketData->pPerIoData->ioType = NONE_PENDING;

	pPerSocketData->pPerIoDataSending = NULL;
	if(NULL == (pPerSocketData->pPerIoDataSending = (pPER_IO_DATA)malloc(sizeof(PER_IO_DATA))) ){
		DEBUG_ALWAYS("Error allocating perIo memory for connection");
		free(pPerSocketData);
		pPerSocketData = NULL;
		return(NULL);	
	}
	pPerSocketData->pPerIoDataSending->wsaoverlapped.Internal=0;
	pPerSocketData->pPerIoDataSending->wsaoverlapped.InternalHigh = 0;
	pPerSocketData->pPerIoDataSending->wsaoverlapped.Offset=0;
	pPerSocketData->pPerIoDataSending->wsaoverlapped.OffsetHigh = 0;
	pPerSocketData->pPerIoDataSending->wsaoverlapped.hEvent = NULL;
	pPerSocketData->pPerIoDataSending->wsabuf.buf = NULL;
	pPerSocketData->pPerIoDataSending->wsabuf.len = 0;
	pPerSocketData->pPerIoDataSending->ioType = NONE_PENDING;

	
	if(SUCCESS != Queue_Init(&(pPerSocketData->sInQueue))){
		DEBUG_ALWAYS("Error initialising pInQueue");
		free(pPerSocketData->pPerIoData);
		pPerSocketData->pPerIoData = NULL;
		free(pPerSocketData->pPerIoDataSending);
		pPerSocketData->pPerIoDataSending = NULL;
		free(pPerSocketData);
		pPerSocketData = NULL;
		return(NULL);
	}
	if(SUCCESS != Queue_Init(&(pPerSocketData->sOutQueue))){
		DEBUG_ALWAYS("Error initialising pOutQueue");
	
		Queue_Free(&(pPerSocketData->sInQueue));
		free(pPerSocketData->pPerIoData);
		pPerSocketData->pPerIoData = NULL;
		free(pPerSocketData->pPerIoDataSending);
		pPerSocketData->pPerIoDataSending = NULL;
		free(pPerSocketData);
		pPerSocketData = NULL;
		return(NULL);
	}


	pPerSocketData->id=id;
	//pPerSocketData->pDomainName = NULL;
	//pPerSocketData->pPortName = NULL;   // pas besoin car ils sont déclarés statiquement comme des arrays de taille fixe;
	pPerSocketData->pAddrInfo = NULL;
	pPerSocketData->waitForWindow = FALSE;
	pPerSocketData->pNext=NULL;
	pPerSocketData->pPrev=NULL;
	pPerSocketData->socket=socket;
	pPerSocketData->state=NEW;
	pPerSocketData->closingState = NONE;
	pPerSocketData->window=INITIAL_WINDOW;

	
	//initialiser pPerSocketData->server ???
	DEBUG("NewConnection\n");
	DEBUG("leaving\n");
	return(pPerSocketData);

}

pPER_REVERSEBIND_DATA NewReverseBind(ID id, SOCKET listeningSocket){
	DEBUG("entering\n");
	pPER_REVERSEBIND_DATA pPerReverseBindData = NULL;
	DEBUG("0")
	if(NULL == (pPerReverseBindData = (pPER_REVERSEBIND_DATA )malloc(sizeof(PER_REVERSEBIND_DATA)))){
		DEBUG_ALWAYS("Error allocating connection memory\n");
		return (NULL);
	}

	pPerReverseBindData->pPerAcceptData = NULL;
	if(NULL == (pPerReverseBindData->pPerAcceptData = (pPER_ACCEPT_DATA)malloc(sizeof(PER_ACCEPT_DATA))) ){
		DEBUG_ALWAYS("Error allocating perIo memory for connection");
		free(pPerReverseBindData);
		pPerReverseBindData = NULL;
		return(NULL);	
	}
	pPerReverseBindData->pPerAcceptData->acceptSocket = INVALID_SOCKET;
	pPerReverseBindData->pPerAcceptData->ioType = NONE_PENDING;
	pPerReverseBindData->pPerAcceptData->wsaoverlapped.hEvent = NULL;
	pPerReverseBindData->pPerAcceptData->wsaoverlapped.Internal = 0;
	pPerReverseBindData->pPerAcceptData->wsaoverlapped.InternalHigh = 0;
	pPerReverseBindData->pPerAcceptData->wsaoverlapped.Offset = 0;
	pPerReverseBindData->pPerAcceptData->wsaoverlapped.OffsetHigh = 0;
	pPerReverseBindData->pPerAcceptData->wsaoverlapped.Pointer = NULL;

	pPerReverseBindData->id = id;
	pPerReverseBindData->listeningSocket = listeningSocket;
	pPerReverseBindData->closingState = NONE;
	pPerReverseBindData->pNext = NULL;
	pPerReverseBindData->pPrev = NULL;
	
	DEBUG_ALWAYS("New ReverseBind\n");
	DEBUG("leaving\n");
	return(pPerReverseBindData);
}

int Send_Socket(SOCKET fd, LEN len, char * pBuf, int * pBytesSent) {
	DEBUG("Entering\n");
	int rc;
	if (SOCKET_ERROR == (rc = send(fd, pBuf, len, 0))) {
		int error = WSAGetLastError();
		switch (error)
		{
		case WSAENOTSOCK:
			DEBUG("WSAENOTSOCK : fd is not a valid socket, returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			
		case WSAENETRESET:
			DEBUG("WSAENETRESET : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			
		case WSAENOTCONN:
			DEBUG("WSAENOTCONN : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			
		case WSAESHUTDOWN:
			DEBUG("WSAESHUTDOWN :  returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			
		case WSAEINVAL:
			DEBUG("WSAEINVAL :  returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			
		case WSAECONNABORTED:
			DEBUG("WSAECONNABORTED : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			
		case WSAECONNRESET:
			DEBUG("WSAECONNRESET : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			
		case WSAETIMEDOUT:
			DEBUG("WSAETIMEDOUT : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			
		case WSAEINTR:
			DEBUG("WSAEINTR : call was cancelled, queueing the packet instead.\n");
			return(FAILURE);
			
		case WSAEINPROGRESS:
			DEBUG("WSAEINPROGRESS : queuing the packet.\n");
			return(FAILURE);
			
		case WSAENETDOWN:
			DEBUG("WSAENETDOWN : queuing the packet.\n");
			return(FAILURE);
			
		case WSAENOBUFS:
			DEBUG("WSAENOBUFS : queuing the packet.\n");
			return(FAILURE);
			
		case WSAEWOULDBLOCK:
			DEBUG("WSAEWOULDBLOCK : queuing the packet.\n");
			return(FAILURE);
			
		case WSAEHOSTUNREACH:
			DEBUG("WSAEHOSTUNREACH : queuing the packet.\n");
			return(FAILURE);
			

		default:
			DEBUG_ALWAYS_VAR("Unattended error in send() : %d. Returning FATAL_ERROR.\n", error);
			return(ERROR_FATAL);
			
		}
	}

	if (pBytesSent) {
		*pBytesSent = rc;
	}
	DEBUG_VAR("Successful send : %d bytes.\n", rc);
	DEBUG("Leaving\n");
	return(SUCCESS);
}

SOCKET NewSocket(int family){
	DEBUG("Entering\n");
	SOCKET newSocket = INVALID_SOCKET;

	if ((newSocket = socket(family, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		DEBUG_ALWAYS_VAR("Could not create socket : %d", WSAGetLastError());
		return INVALID_SOCKET;
	}
	DEBUG("Socket created\n");

	ULONG mode = 1;
	if (ioctlsocket(newSocket, FIONBIO, &mode) == SOCKET_ERROR) {
		DEBUG_ALWAYS_VAR("Error ioctlsocket() : %d.\n", WSAGetLastError());
		return INVALID_SOCKET;
	}
	DEBUG("Socket set to non blocking.\n");
	return newSocket;
}

VOID AddConnection (pPER_SOCKET_DATA * ppConnectionList, pPER_SOCKET_DATA pPerSocketData) {

	pPER_SOCKET_DATA     pTemp;


	if( *ppConnectionList == NULL ) {

		//
		// add the first node to the linked list
		//
		pPerSocketData->pPrev    = NULL;
		pPerSocketData->pNext = NULL;
		*ppConnectionList = pPerSocketData;
	} 
	else {

		//
		// add node to head of list
		//
		pTemp = *ppConnectionList;

		*ppConnectionList = pPerSocketData;
		pPerSocketData->pPrev    = pTemp;
		pPerSocketData->pNext = NULL; 

		pTemp->pNext = pPerSocketData;
	}

	return;

}

VOID AddReverseBind (pPER_REVERSEBIND_DATA * ppReverseBindList,pPER_REVERSEBIND_DATA pPerReverseBindData) {

	pPER_REVERSEBIND_DATA     pTemp;


	if( *ppReverseBindList == NULL ) {

		//
		// add the first node to the linked list
		//
		pPerReverseBindData->pPrev    = NULL;
		pPerReverseBindData->pNext = NULL;
		*ppReverseBindList = pPerReverseBindData;
	} 
	else {

		//
		// add node to head of list
		//
		pTemp = *ppReverseBindList;

		*ppReverseBindList = pPerReverseBindData;
		pPerReverseBindData->pPrev    = pTemp;
		pPerReverseBindData->pNext = NULL; 

		pTemp->pNext = pPerReverseBindData;
	}

	return;
}

VOID CloseAllConnections(pPER_SOCKET_DATA pHeadList){
	pPER_SOCKET_DATA pTemp1, pTemp2;

	pTemp1 = pHeadList;
	while(pTemp1){
		pTemp2 = pTemp1->pPrev;
		//TODO close(pTemp1)
		//close la socket
		DEBUG_ALWAYS_VAR("Closing connection %d\n", pTemp1->id);
		closesocket(pTemp1->socket);
		pTemp1->socket = INVALID_SOCKET;

		//Free les queue
		Queue_Free(&(pTemp1->sOutQueue));
		Queue_Free(&(pTemp1->sInQueue));

		//Free les pPER_IO_DATA  // peut on le faire avant que toutes les IO soient finies?
		//The overlapped structure is safe to free when only the posted i/o has completed. Here we only need to test those posted but not yet received by PQCS in the shutdown process.
		//On peut use HasOverlappedIoCompleted((LPOVERLAPPED)pTempIO) mais il faut bloquer > bof..
		//NULL les pPER_IO_DATA

		while(! HasOverlappedIoCompleted(&(pTemp1->pPerIoData->wsaoverlapped))){
			Sleep(0);
		}
		while(! HasOverlappedIoCompleted(&(pTemp1->pPerIoDataSending->wsaoverlapped))){
			Sleep(0);
		}
		
		free(pTemp1->pPerIoData);
		pTemp1->pPerIoData = NULL;
		free(pTemp1->pPerIoDataSending);
		pTemp1->pPerIoDataSending = NULL;

		free(pTemp1);

		pTemp1 = pTemp2;
	}

	return;
}

int StartClosing(pPER_SOCKET_DATA pPerSocketData, VC_INFO vcInfo, PCHAR pIcaSendBuf){
	DEBUG("Entering\n");

	if(SUCCESS != Send_Ica_CloseRequest(vcInfo, pIcaSendBuf, pPerSocketData->id)){
		DEBUG_ALWAYS("error send ica close request\n");
		return(FAILURE);
	}
	closesocket(pPerSocketData->socket);
	pPerSocketData->socket = INVALID_SOCKET;
	pPerSocketData->closingState = REQ_SENT;

	DEBUG("Leaving\n");
	return(SUCCESS);
}

//peut etre utilisée juste apres NewReverseBind() mais seulement si aucun AcceptEx n'est en cours 
int CloseReverseBind(pPER_REVERSEBIND_DATA * ppReverseBindList, pPER_REVERSEBIND_DATA pPerReverseBindData){
	if(pPerReverseBindData->listeningSocket != INVALID_SOCKET){
		closesocket(pPerReverseBindData->listeningSocket);
		pPerReverseBindData->listeningSocket = INVALID_SOCKET;
	}

	if(pPerReverseBindData->pPerAcceptData->acceptSocket != INVALID_SOCKET){
		closesocket(pPerReverseBindData->pPerAcceptData->acceptSocket);
		pPerReverseBindData->pPerAcceptData->acceptSocket = INVALID_SOCKET;
	
	}

	RemoveReverseBind(ppReverseBindList, pPerReverseBindData);
	FreeReverseBind(pPerReverseBindData);
	
	pPerReverseBindData = NULL;
	return(SUCCESS);
}

VOID RemoveConnection(pPER_SOCKET_DATA * ppConnectionList, pPER_SOCKET_DATA pPerSocketData){
	DEBUG("entering\n");
	
	//gere le cas ou le reverse bind a pas ete Add avant
    if(pPerSocketData != *ppConnectionList && pPerSocketData->pPrev == NULL && pPerSocketData->pNext == NULL){
        return;
    }
    // -- 

	if(pPerSocketData == *ppConnectionList){
		if (pPerSocketData->pPrev == NULL){
			*ppConnectionList = NULL;
		}
		else{
			*ppConnectionList = pPerSocketData->pPrev;
			pPerSocketData->pPrev->pNext = NULL;
		}
	}
	else{
		if(pPerSocketData->pPrev == NULL){
			pPerSocketData->pNext->pPrev = NULL;
		}
		else{
			pPerSocketData->pPrev->pNext = pPerSocketData->pNext;
			pPerSocketData->pNext->pPrev = pPerSocketData->pPrev;
		}
	}
	

	DEBUG("leaving\n");
	return;

	}


VOID RemoveReverseBind(pPER_REVERSEBIND_DATA * ppReverseBindList,pPER_REVERSEBIND_DATA pPerReverseBindData){
	DEBUG("entering\n");

	//gere le cas ou le reverse bind a pas ete Add avant
    if(pPerReverseBindData != *ppReverseBindList && pPerReverseBindData->pPrev == NULL && pPerReverseBindData->pNext == NULL){
        return;
    }
    // -- 

	if(pPerReverseBindData == *ppReverseBindList){
		if (pPerReverseBindData->pPrev == NULL){
			*ppReverseBindList = NULL;
		}
		else{
			*ppReverseBindList = pPerReverseBindData->pPrev;
			pPerReverseBindData->pPrev->pNext = NULL;
		}
	}
	else{
		if(pPerReverseBindData->pPrev == NULL){
			pPerReverseBindData->pNext->pPrev = NULL;
		}
		else{
			pPerReverseBindData->pPrev->pNext = pPerReverseBindData->pNext;
			pPerReverseBindData->pNext->pPrev = pPerReverseBindData->pPrev;
		}
	}
	

	DEBUG("leaving\n");
	return;

	}

int UpdateIOCP(HANDLE completionPort, SOCKET socket, pPER_SOCKET_DATA pPerSocketData){
	if(NULL == CreateIoCompletionPort((HANDLE)socket, completionPort , (ULONG_PTR) pPerSocketData, 0)){

		DEBUG_ALWAYS_VAR("CreateIoCompletionPort() failed: %d\n", GetLastError());

		return(FAILURE);
	}
	return(SUCCESS);
}

int UpdateIOCPReverseBind(HANDLE completionPort, SOCKET socket, pPER_REVERSEBIND_DATA pPerReverseBindData){
	if(NULL == CreateIoCompletionPort((HANDLE)socket, completionPort , (ULONG_PTR) pPerReverseBindData, 0)){

		DEBUG_ALWAYS_VAR("CreateIoCompletionPort() failed: %d\n", GetLastError());

		return(FAILURE);
	}
	return(SUCCESS);
}

int Parse_Ica_ClientDomainCmd(IN char * pPacketBuf, IN LEN bufLen, OUT VER *pVer, OUT CMD *pCmd, OUT PORT *pPort, OUT DOMLEN * pDomLen, OUT DOMSTR * ppDomData, OUT CHAR * pDomStr ) {
	DEBUG("Entering\n");
	
	void * address = (void *)pPacketBuf;

	*pVer = *((VER *)address);
	address = (VER *)address + 1;


	*pCmd = *((CMD *)address);
	address = (CMD *)address + 1;

	*pPort = ntohs(*((PORT *)address));
	address = (PORT *)address + 1;

	*pDomLen = *((DOMLEN *)address);
	address = (DOMLEN *)address + 1;

	*ppDomData = (DOMSTR)address;

	if(*pDomLen >= 300){
		DEBUG_ALWAYS("Domain name length too long\n");
		return(FAILURE);
	}
	strncpy(pDomStr, (CHAR *)address, (size_t)*pDomLen);
	pDomStr[*pDomLen]=0; //on termine la string par un null char;
	
	if(bufLen != VER_SIZE+CMD_SIZE+PORT_SIZE+DOMLEN_SIZE+*pDomLen ){
		DEBUG_ALWAYS("Mismatching length in parsing\n");
		return(FAILURE);
	}

	DEBUG_VAR("Parsing : VER=%d, CMD=%d, PORT=%d, DOMLEN=%d, pDOMDATA=%p \n", *pVer, *pCmd, *pPort, *pDomLen, *ppDomData);
	return(SUCCESS);
}

int Parse_Ica_ClientIpCmd(IN char * pPacketBuf, IN LEN bufLen, OUT VER *pVer, OUT CMD *pCmd, OUT IPADDR *pIpAddr, OUT PORT *pPort) {
	DEBUG("Entering\n");

	if(bufLen != VER_SIZE+CMD_SIZE+IPADDR_SIZE+PORT_SIZE){
		DEBUG_ALWAYS("Mismatching length in parsing\n");
		return(FAILURE);
	}
	void * address = (void *)pPacketBuf;

	*pVer = *((VER *)address);
	address = (VER *)address + 1;

	*pCmd = *((CMD *)address);
	address = (CMD *)address + 1;

	*pIpAddr = ntohl(*((IPADDR *)address));
	address = (IPADDR *)address + 1;

	*pPort = ntohs(*((PORT *)address));

	DEBUG_VAR("Parsing : VER=%d, CMD=%d, IPADDR=%u, PORT=%d \n", *pVer, *pCmd, *pIpAddr, *pPort);
	return(SUCCESS);
}

int Parse_Ica_ClientIp6Cmd(IN char * pPacketBuf, IN LEN bufLen, OUT VER *pVer, OUT CMD *pCmd, OUT IP6ADDR *pIp6Addr, OUT PORT *pPort) {
	DEBUG("Entering\n");

	if(bufLen != VER_SIZE+CMD_SIZE+IP6ADDR_SIZE+PORT_SIZE){
		DEBUG_ALWAYS("Mismatching length in parsing\n");
		return(FAILURE);
	}
	void * address = (void *)pPacketBuf;

	*pVer = *((VER *)address);
	address = (VER *)address + 1;

	*pCmd = *((CMD *)address);
	address = (CMD *)address + 1;

	*pPort = ntohs(*((PORT *)address));
	address = (PORT *)address + 1;

	*pIp6Addr =(IP6ADDR) address;
	

	DEBUG_VAR("Parsing : VER=%d, CMD=%d, PORT=%d \n", *pVer, *pCmd, *pPort);
	DEBUG_VAR("IP6ADDR : %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \n", (*pIp6Addr)[0],(*pIp6Addr)[1], (*pIp6Addr)[2], (*pIp6Addr)[3], (*pIp6Addr)[4], (*pIp6Addr)[5], (*pIp6Addr)[6], (*pIp6Addr)[7], (*pIp6Addr)[8], (*pIp6Addr)[9], (*pIp6Addr)[10], (*pIp6Addr)[11], (*pIp6Addr)[12], (*pIp6Addr)[13], (*pIp6Addr)[14], (*pIp6Addr)[15]);
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Parse_Ica_ReverseBindCmd(IN char * pPacketBuf, IN LEN bufLen, OUT IPADDR *pRmtAddr, OUT PORT *pRmtPort) {
	DEBUG("Entering\n");

	if(bufLen != IPADDR_SIZE+PORT_SIZE){
		DEBUG_ALWAYS("Mismatching length in parsing\n");
		return(FAILURE);
	}
	void * address = (void *)pPacketBuf;


	*pRmtAddr = ntohl(*((IPADDR *)address));
	address = (IPADDR *)address + 1;

	*pRmtPort = ntohs(*((PORT *)address));

	DEBUG_VAR("Parsing : RMTADDR=%u, RMTPORT=%d \n",  *pRmtAddr, *pRmtPort);
	return(SUCCESS);
}

int Parse_Ica_ReverseConnectRpl(IN char * pPacketBuf, IN LEN bufLen, OUT ID * pTmpId, OUT ID * pRevBindId, OUT RPL * pRpl) {
	DEBUG("Entering\n");

	if(bufLen != ID_SIZE+ID_SIZE+RPL_SIZE){
		DEBUG_ALWAYS("Mismatching length in parsing\n");
		return(FAILURE);
	}
	void * address = (void *)pPacketBuf;


	*pTmpId = *((ID *)address);
	address = (ID *)address + 1;

	*pRevBindId = *((ID *)address);
	address = (ID *)address + 1;

	*pRpl = *((RPL *)address);

	DEBUG_VAR("Parsing : TMPID=%u, REVBINDID=%d, RPL=%d \n",  *pTmpId, *pRevBindId, *pRpl);
	return(SUCCESS);
}

int Parse_Ica_MetaWindow(IN char * pPacketBuf, IN LEN bufLen, OUT int * pWindow) {
	DEBUG("Entering\n");
	if(bufLen != sizeof(int)){
		DEBUG_ALWAYS("Mismatching length in parsing\n");
		return(FAILURE);
	}
	*pWindow = *((int*)pPacketBuf);
	DEBUG_VAR("Parsing : window=%d.\n", *pWindow);
	DEBUG("Leaving\n");
	return(SUCCESS);
}


LPFN_CONNECTEX GetConnectExPtr()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET){
		return(NULL);
	}
    LPFN_CONNECTEX lpConnectEx = NULL;
    GUID guid = WSAID_CONNECTEX;
    DWORD dwNumBytes = 0;
    WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &lpConnectEx, sizeof(lpConnectEx), &dwNumBytes, NULL, NULL);
	closesocket(sock);
    return lpConnectEx;
}


LPFN_ACCEPTEX GetAcceptExPtr()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET){
		return(NULL);
	}
    LPFN_ACCEPTEX lpAcceptEx = NULL;
    GUID guid = WSAID_ACCEPTEX;
    DWORD dwNumBytes = 0;
    WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &lpAcceptEx, sizeof(lpAcceptEx), &dwNumBytes, NULL, NULL);
	closesocket(sock);
    return lpAcceptEx;
}