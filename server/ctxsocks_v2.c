

//for compatibility with Ws2_32.lib, do not  include <windows.h>, instead define the WIN32_LEAN_AND_MEAN macro
//#include <windows.h> // Base  headers

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include "ctxsocks_v2.h" 



pSELECT_ARGS g_pSelectArgs;
HANDLE hConnectionMutex;


int __cdecl
main(INT argc, CHAR **argv)
{

	DEBUG("Ent1ering\n");
	pCONNECTION_MANAGER pConnection_manager = NULL;
	VC_INFO vcInfo = { NULL , NULL};
	BOOL running = TRUE;
	BOOL stopIca = FALSE;
	int         exitcode = SUCCESS;

	LPBYTE      pIcaRecvBuf = NULL;

	LPBYTE		pSocketRecvBuf = NULL;
	LPBYTE      pIcaSendBuf = NULL;
	LPBYTE		pIcaSendBufEnd = NULL;
	DWORD       ByteCount = 0;
	LPTSTR      pSessionInfo = NULL;
	DWORD       Result = 0;
	BOOL rc;
	WSADATA WSAData;
	ULONG bytesRead;
	ULONG bytesWritten;

	int rc_poll;
	int rc_select;



	// Initialize Winsock
	WSADATA wsaData;
	if (SUCCESS != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		DEBUG("WSAStartup failed\n");
		exitcode = FAILURE;
		goto ExitMain;
	}


	/*
	* Open the Virtual Channel
	*/


	if (OpenVC(&vcInfo) == FAILURE) {
		exitcode = FAILURE;
		goto ExitMain;
	}


	/*
	* Allocate the buffers
	*
	* pSend will have the actual buffer to be sent
	*/
	if (NULL == (pIcaSendBuf = (LPBYTE)calloc(1000, MAX_ICA_PACKET_DATA_SIZE))) {
		DEBUG("error calloc");
		exitcode = FAILURE;
		goto ExitMain;
	}
	pIcaSendBufEnd = pIcaSendBuf;

	if (NULL == (pIcaRecvBuf = (LPBYTE)malloc(5005))) {
		DEBUG("error malloc");
		exitcode = FAILURE;
		goto ExitMain;
	}

	if (NULL == (pSocketRecvBuf = (LPBYTE)malloc(MAX_ICA_PACKET_DATA_SIZE))) {
		DEBUG("error malloc");
		exitcode = FAILURE;
		goto ExitMain;
	}




	IcaPacketQueue_Init(&vcInfo.pIcaPacketQueue);


	DEBUG("Memory allocated.\n");


	

	DEBUG("connectionssss initialized\n");
	//debug(pFile);


	
	ConnectionManager_Init(&pConnection_manager);

	SELECT_ARGS selectArgs;
	g_pSelectArgs = &selectArgs;

	FD_ZERO(&(selectArgs.efds));
	selectArgs.nfds = 0;



	if (NULL == (hConnectionMutex = CreateMutex(NULL, FALSE, NULL))) {
		DEBUG_VAR("FATAL ERROR CreateMutex() : %d. Exiting.\n", GetLastError());
		exitcode = FAILURE; //TODO
		goto ExitMain;
	}

	if (SUCCESS != Send_Ica_MetaServerStart(vcInfo, pIcaSendBuf)) {
		DEBUG("FATAL ERROR Send_Ica_MetaServerStart(): Could not send or queue the starting packet. Exiting.\n");
		exitcode = FAILURE;
		goto ExitMain;
	}


	while (running == TRUE) {
	
		
			if (stopIca == FALSE) {
				if (FALSE == WFVirtualChannelRead(vcInfo.hVC, 0, pIcaRecvBuf, 2048, &bytesRead)) {
					DWORD error = GetLastError();
					switch (error)
					{
					case 995:
						//TODO : log fichier
						break;
					case 996:
						//TODO : log fichier
						DEBUG("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n")
							break;

					default:
						DEBUG_VAR("FATAL ERROR WFVirtualChannelRead() : %d. Exiting.\n", error);
						exitcode = FAILURE;
						goto ExitMain;
						break;
					}
				}
				else {
					if (bytesRead > 0) {
						DEBUG_VAR("ICA data received : %d bytes\n", bytesRead);

						int rc;
						PACKET_TYPE type;
						ID id;
						LEN len;
						unsigned char * pData;

						Parse_Ica_GenericPacket(pIcaRecvBuf, &type, &id, &len, &pData);
						DEBUG_VAR(" Parsing : type=%d, id=%d, len=%d, data_addr=%p \n", type, id, len, pData);

						if (len > sizeof(pIcaRecvBuf) - PACKET_TYPE_SIZE - ID_SIZE - LEN_SIZE) {
							DEBUG("FATAL ERROR : parsing gave a data length parameter exceeding the size of the receiving buffer. Exiting\n");
							exitcode = FAILURE; //TODO : use more precise exitcodes ?
							goto ExitMain;
						}


						if (WAIT_FAILED == WaitForSingleObject(hConnectionMutex, INFINITE)) {
							DEBUG_VAR("FATAL ERROR WaitForSingleObject(): %d. Exiting.\n", GetLastError()); //TODO : est ce vraiment irrecuperable ?
							exitcode = FAILURE; //TODO
							goto ExitMain;
						}

						switch (type) {
						case SOCKS_DATA:
							if (SUCCESS != Handler_Ica_DataPacket(pConnection_manager, vcInfo, pIcaSendBuf, id, len, pData)) {
								DEBUG("FATAL ERROR Handler_Ica_DataPacket. Exiting.\n");
								exitcode = FAILURE; //TODO
								goto ExitMain;
							}
							DEBUG("SocksData Packet handled. \n");
							break;

						case SOCKS_CLIENT_IP_CMD:
							if (SUCCESS != Handler_Ica_ClientIpCmd(pConnection_manager, vcInfo, pIcaSendBuf, id, len, pData)) {
								DEBUG("FATAL ERROR Handler_Ica_ClientIpCmd. Exiting.\n");
								exitcode = FAILURE; //TODO
								goto ExitMain;
							}
							DEBUG("ClientIpCmd Packet handled.\n");
							break;

						case SOCKS_CLIENT_IP6_CMD:
							if (SUCCESS != Handler_Ica_ClientIp6Cmd(pConnection_manager, vcInfo, pIcaSendBuf, id, len, pData)) {
								DEBUG("FATAL ERROR Handler_Ica_ClientIp6Cmd. Exiting.\n");
								exitcode = FAILURE; //TODO
								goto ExitMain;
							}
							DEBUG("ClientIp6Cmd Packet handled.\n");
							break;

						case SOCKS_CLIENT_DOMAIN_CMD:
							if (SUCCESS != Handler_Ica_ClientDomainCmd(pConnection_manager, vcInfo, pIcaSendBuf, id, len, pData)) {
								DEBUG("FATAL ERROR Handler_Ica_ClientDomainCmd. Exiting.\n");
								exitcode = FAILURE; //TODO
								goto ExitMain;
							}
							DEBUG("ClientDomainCmd Packet handled.\n");

							break;


						case CLOSE_REQUEST: 
							if (SUCCESS != Handler_Ica_CloseRequest(pConnection_manager, vcInfo, pIcaSendBuf, id)) {
								DEBUG("FATAL ERROR Handler_Ica_CloseRequest. Exiting.\n");
								exitcode = FAILURE; //TODO
								goto ExitMain;
							}
							DEBUG("Close Request handled.\n");
							break;

						case CLOSE_ACK:
							if (SUCCESS != Handler_Ica_CloseAck(pConnection_manager, id)) {
								DEBUG("FATAL ERROR Handler_Ica_CloseAck. Exiting.\n");
								exitcode = FAILURE; //TODO
								goto ExitMain;
							}
							DEBUG("Close Ack handled.\n");
							break;
						case META_WINDOW:
							if (SUCCESS != Handler_Ica_MetaWindow(pData,pConnection_manager ,id)) {
								DEBUG("FATAL ERROR Handler_Ica_MetaWindow(). Exiting.\n");
								exitcode = FAILURE;
								goto ExitMain;
							}
							DEBUG("MetaWindow Packet handled.\n");
							break;

						default:
							DEBUG_VAR("FATAL ERROR : Unknown packet type : %d. Exiting\n ", type);
							exitcode = 1; //TODO : add more precise exitcodes?
							goto ExitMain;
						}

						if (!ReleaseMutex(hConnectionMutex)) {
							DEBUG_VAR("FATAL ERROR ReleaseMutex() : %d. Exiting.\n", GetLastError());
							exitcode = FAILURE; //TODO?
							goto ExitMain;
						}
						DEBUG("----------------------------------------------------\n")
					}
				}
			}
			else {
			DEBUG("STOP ICA TRUE\n");
			stopIca = FALSE;

			if (WAIT_FAILED == WaitForSingleObject(hConnectionMutex, INFINITE)) {
				DEBUG_VAR("FATAL ERROR WaitForSingleObject(): %d. Exiting.\n", GetLastError()); //TODO : est ce vraiment irrecuperable ?
				exitcode = FAILURE; //TODO
				goto ExitMain;
			}

			for (int i = 0; i < ConnectionManager_GetConnectionNumber(pConnection_manager); i++) {
				pCONNECTION pConnection;
				if (NULL == ( pConnection = ConnectionManager_GetConnectionByIndex(pConnection_manager, i))) {
					DEBUG("Error when trying to get connection. Exiting.\n");
					exitcode = 1;
					goto ExitMain;
				}

				if (SUCCESS != SocketQueue_SendAll(pConnection->pSocketQueue, pConnection->socket_fd)) {
					DEBUG("Could not send all the queue.\n")
				}

				if (pConnection->pSocketQueue->size > MAX_SOCKET_QUEUE_SIZE - 2048) {
					stopIca = TRUE;
				}
			}


			if (!ReleaseMutex(hConnectionMutex)) {
				DEBUG_VAR("FATAL ERROR ReleaseMutex() : %d. Exiting.\n", GetLastError());
				exitcode = FAILURE; //TODO?
				goto ExitMain;
			}
		}



		//Check for incoming data on the sockets, and act consequently.

		

		if (WAIT_FAILED == WaitForSingleObject(hConnectionMutex, INFINITE)) {
			DEBUG_VAR("FATAL ERROR WaitForSingleObject(): %d. Exiting.\n", GetLastError()); //TODO : est ce vraiment irrecuperable ?
			exitcode = FAILURE; //TODO
			goto ExitMain;
		}

	

		FD_ZERO(&(selectArgs.efds));
		selectArgs.nfds = 0;
		for (int i = 0; i < ConnectionManager_GetConnectionNumber(pConnection_manager); i++) {
			//DEBUG_VAR("fd_set loop : %d/%d\n", i, ConnectionManager_GetConnectionNumber(pConnection_manager));
			if (selectArgs.nfds >= 63) {
				DEBUG("WARNING : nfds >= 63, FD_SET() does not support more than 64 fd by default, change this with FD_SETSIZE\n");
				exitcode = 1;
				goto ExitMain;
			}
			pCONNECTION pConnection = ConnectionManager_GetConnectionByIndex(pConnection_manager, i);
			
			if (pConnection->state == CONNECTING) {
				FD_SET(pConnection->socket_fd, &(selectArgs.efds));
				selectArgs.nfds += 1;
			}

			if (pConnection->state== RESOLVED) {
				DEBUG("Connecting RESOLVED connection\n");
				ConnectSocket(pConnection_manager, i);
			}
			if (pConnection->state == UNRESOLVED) {
				DEBUG("Closing UNRESOLVED connection\n");
				if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
					DEBUG("Error CloseProcedure() : could not close the UN RESOLVED connection.\n");
					exitcode = 1;
					goto ExitMain;
				}
				
			}

			if (pConnection->pSocketQueue->size > MAX_SOCKET_QUEUE_SIZE - 2048) {
				stopIca = TRUE;
			}

			// //notsure
			// if(pConnection->state == ESTABLISHED){
			// 	int blabla;
			// 	if(SUCCESS != Send_SocketQueue(pConnection->socket_fd, pConnection->pSocketQueue, &blabla)){
			// 		exitcode=1;
			// 		goto ExitMain;
			// 	}
			
			// 	if (SUCCESS != Send_Ica_MetaWindow(vcInfo,pIcaSendBuf, blabla, pConnection->id)) {
			// 		DEBUG("FATAL E RROR : could not send the meta window packet\n");
			// 		exitcode = 1;
			// 		goto ExitMain;
			// 	}
			// }

		}



		if (!ReleaseMutex(hConnectionMutex)) {
			DEBUG_VAR("FATAL ERROR ReleaseMutex() : %d. Exiting.\n", GetLastError());
			exitcode = FAILURE; //TODO?
			goto ExitMain;
		}

		if (WAIT_FAILED == WaitForSingleObject(hConnectionMutex, INFINITE)) {
			DEBUG_VAR("FATAL ERROR WaitForSingleObject(): %d. Exiting.\n", GetLastError()); //TODO : est ce vraiment irrecuperable ?
			exitcode = FAILURE; //TODO
			goto ExitMain;
		}


		rc_select = 0;
		TIMEVAL timeout = { 0,0 };
		if (selectArgs.nfds > 0) { //otherwise select() returns WSAEINVAL if nfds is negative or null
			if (SOCKET_ERROR == (rc_select = select(selectArgs.nfds, NULL, NULL, &(selectArgs.efds), &timeout))) {
				int error = WSAGetLastError();
				switch (error)
				{
				case WSAENOTSOCK:
					DEBUG("select() WSAENOTSOCK error : efds contains an entry that is not a socket\n");
					debug(pConnection_manager, vcInfo.pIcaPacketQueue);
					exitcode = FAILURE;
					goto ExitMain;
				case WSAEINVAL:
					DEBUG_VAR("select WSAEINVAL. nfds : %d\n", selectArgs.nfds);
					exitcode = FAILURE;
					goto ExitMain;
					break;
				default:
					DEBUG_VAR("FATAL ERROR select() : %d. Exiting\n", WSAGetLastError());
					exitcode = FAILURE;
					goto ExitMain;
					break;
				}
			}
		}

		

		rc_poll = 0;
		int nCo = ConnectionManager_GetConnectionNumber(pConnection_manager);
		if (nCo > 0) {
			if (nCo > pConnection_manager->n_closing_connection) {
				if (SOCKET_ERROR == (rc_poll = WSAPoll(pConnection_manager->fds, ConnectionManager_GetConnectionNumber(pConnection_manager),10))) {
				int error = WSAGetLastError();
				switch (error)
				{
				case(WSAEINVAL):
					DEBUG("WSAPoll() WSAEINVAL\n");
					exitcode = FAILURE;
					goto ExitMain;
					break;
				default:
					DEBUG_VAR("FATAL ERROR WSAPoll() : %d. Exiting.\n", error); //TODO : est ce fatal ?
					exitcode = FAILURE; //TODO
					goto ExitMain;
					DEBUG("----------------------------------------------------\n")
						break;
				}
			}
			}
		}
		

		if ((rc_poll > 0) || (rc_select>0)) {
			DEBUG_VAR("queue size  : %d\n", vcInfo.pIcaPacketQueue->size);
			if (IcaPacketQueue_GetFreeSpace(vcInfo.pIcaPacketQueue) >= rc_poll + rc_select) {
				DEBUG("rc_poll>0 || rc_select>0")
					debug(pConnection_manager, vcInfo.pIcaPacketQueue);
				//At least one of the file descriptor is readable or writable

				//if (WAIT_FAILED == WaitForSingleObject(hConnectionMutex, INFINITE)) {
				//	DEBUG_VAR("FATAL ERROR WaitForSingleObject(): %d. Exiting.\n", GetLastError()); //TODO : est ce vraiment irrecuperable ?
				//	exitcode = FAILURE; //TODO
				//	goto ExitMain;
				//}

				for (int i = 0; i < ConnectionManager_GetConnectionNumber(pConnection_manager); i++) {
					IcaPacketQueue_Send(vcInfo);
					if (IcaPacketQueue_IsFull(vcInfo.hVC)) {
						DEBUG("Ica queue is still full, skiping to next connection.\n");
						continue;
					}
					DEBUG_VAR("testing connection : %d\n", i);
					pCONNECTION pConnection;
					WSAPOLLFD * pFd;
					if (NULL == (pConnection = ConnectionManager_GetConnectionByIndex(pConnection_manager, i))) {
						DEBUG("FATAL ERROR ConnectionManager_GetConnectionByIndex : i<n_connection but NULL was returned. Exiting.\n");
						exitcode = FAILURE; //TODO
						goto ExitMain;
					}
					if (NULL == (pFd = ConnectionManager_GetPollFdByIndex(pConnection_manager, i))) {
						DEBUG("FATAL ERROR ConnectionManager_GetPollFdByIndex : i<n_connection but NULL was returned. Exiting.\n");
						exitcode = FAILURE; //TODO
						goto ExitMain;
					}

					if (pConnection->state == CLOSING) {
						continue;
					}

					if (pFd->revents & POLLERR) {
						DEBUG_VAR("POLLERR flag set on socket %d.\n", pFd->fd);
						if (pFd->revents & POLLIN) {
							DEBUG("And POLLIN too, so data may be lost. Exiting.\n");
							exitcode = FAILURE; //TODO
							goto ExitMain;
						}

		
						if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
							DEBUG("ERROR CloseProcedure(): exiting\n");
							exitcode = FAILURE;
							goto ExitMain;
						}

						
						continue;

					}
					if (pFd->revents & POLLNVAL) {
						DEBUG_VAR("POLLNVAL flag set on socket %d. The socket is invalid. Closing connection.\n", pFd->fd);

						if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
							DEBUG("ERROR CloseProcedure(): exiting\n");
							exitcode = FAILURE;
							goto ExitMain;
						}

						
						continue;
					}
					if (pFd->revents & POLLOUT) {
						DEBUG_VAR("POLLOUT flag set on socket %d. Checking successful connection with getpeername()\n", pFd->fd);
						struct sockaddr name;
						struct sockaddr_storage storage;
						int namelen = sizeof(name);
						int storagelen = sizeof(storage);

						if (SUCCESS == getpeername(pFd->fd, (struct sockaddr *)&storage, &storagelen)) {
							//connection succeeded !
							DEBUG("Connection succeded !\n");

							int window = 0;
							if ((window=ConnectionManager_GetConnectionWindowByIndex(pConnection_manager, i)) < MAX_ICA_PACKET_DATA_SIZE) {
								DEBUG_VAR("Connection window not large enough : %d . Skipping to next connection\n", window);
								continue;
							}

							if (SUCCESS != ConnectionManager_SetConnectionStateById(pConnection_manager, pConnection->id, ESTABLISHED)) {
								DEBUG("FATAL ERROR ConnectionManager_SetConnectionStateById() : could not set the connection state while connection should exist. Exiting.\n");
								exitcode = FAILURE; //TODO
								goto ExitMain;
							}

							pFd->events = POLLIN; //TODO : maybe change to a ConnectionManager_... function 

							int n = 0;
							if (SUCCESS != Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pConnection->id, (VER)0x05, (RPL)0x00)) {
								DEBUG("FATAL ERROR Send_Ica_ServerRplPacket(): could not send or queue reply packet. Exiting. \n");
								exitcode = FAILURE;
								goto ExitMain;
							}
							ConnectionManager_SubConnectionWindowByIndex(pConnection_manager, i, 10);

						}


					}
					if (pFd->revents & POLLIN) {
						DEBUG_VAR("POLLIN flag set on socket %d. Trying to read from it.\n", pFd->fd);
						int rc;

						int window = 0;
						if ((window = ConnectionManager_GetConnectionWindowByIndex(pConnection_manager, i)) < MAX_ICA_PACKET_DATA_SIZE) {
							DEBUG_VAR("Connection window not large enough : %d . Skipping to next connection\n", window);
							continue;
						}

						if (SOCKET_ERROR == (rc = recv(pFd->fd, pSocketRecvBuf, MAX_ICA_PACKET_DATA_SIZE, 0))) {
							debug(pConnection_manager, vcInfo.pIcaPacketQueue);
							int error = WSAGetLastError();

							switch (error)
							{
							case WSAEINTR:
								DEBUG("WSAEINTR : not fatal, the error is ignored. \n");
								break;
							case WSAEWOULDBLOCK:
								DEBUG("WSAEWOULDBLOCK : not fatal, the error is ignored.\n");
								break;
							case WSAEINPROGRESS:
								DEBUG("WSAEINPROGRESS : not fatal, the error is ignored.\n");
								break;
							case WSAECONNRESET:
								DEBUG("WSAECONNRESET : Connection reset by host. Closing it.\n");

								if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
									DEBUG("ERROR CloseProcedure(): exiting\n");
									exitcode = FAILURE;
									goto ExitMain;
								}
								
								continue;
								break;
							case WSAENOTSOCK:
								DEBUG("WSAENOTSOCK : The fd is not a valid socket. Closing connection.\n");

								if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
									DEBUG("ERROR CloseProcedure(): exiting\n");
									exitcode = FAILURE;
									goto ExitMain;
								}

								
								continue;
								break;
							case WSAECONNABORTED:
								DEBUG("WSAECONNABORTED : Connection aborted by the computer. Closing it.\n");

								if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
									DEBUG("ERROR CloseProcedure(): exiting\n");
									exitcode = FAILURE;
									goto ExitMain;
								}

								
								continue;
								break;
							case WSAESHUTDOWN:
								DEBUG("WSAESHUTDOWN : Socket has been shutdown, closing connection.\n");

								if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
									DEBUG("ERROR CloseProcedure(): exiting\n");
									exitcode = FAILURE;
									goto ExitMain;
								}
								
								continue;
								break;
							case WSAETIMEDOUT:
								DEBUG("WSAETIMEDOUT : Network failure or peer system failed to respond, closing connection.\n");

								if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
									DEBUG("ERROR CloseProcedure(): exiting\n");
									exitcode = FAILURE;
									goto ExitMain;
								}

								
								continue;
								break;
							default:
								DEBUG_VAR("Unattended error in recv() : %d. Exiting.\n", error);
								exitcode = FAILURE; //TODO
								goto ExitMain;
								break;
							}

						}

						bytesRead = (ULONG)rc;
						if (bytesRead > 0) {
							DEBUG_VAR("bytesRead>0  : %d\n", bytesRead)
								//DEBUG("Received %ld bytes from socket %d (connection %d). \n", bytesRead, fds[i].fd, i);

								//debugBuf(pSocketRecvBuf, bytesRead);

								CONNECTION_STATE state = pConnection->state;
							switch (state) {

							case FREE:
								DEBUG("Error, connection state is FREE but receiving data !\n");
								exitcode = FAILURE;
								goto ExitMain;
								break;
							case ESTABLISHED:
								//printf("Transferring bytes of established connection !\n");
								DEBUG("ESTABLISHED\n");
								DEBUG_VAR("bytesRead : %d\n", bytesRead);
								int n = 0;
								if (SUCCESS != Send_Ica_DataPacket(vcInfo, pIcaSendBuf, pConnection->id, bytesRead, pSocketRecvBuf)) {
									DEBUG("FATAL ERROR Send_Ica_DataPacket : could not send nor queue the data packet. Exiting.\n");
									exitcode = FAILURE;
									goto ExitMain;
								}

								ConnectionManager_SubConnectionWindowByIndex(pConnection_manager, i, bytesRead);

								break;
							}
						}
						if (bytesRead == 0) {
							DEBUG_VAR("0 bytes read. Socket %d trying to close.\n", pFd->fd);
							debug(pConnection_manager, vcInfo.pIcaPacketQueue);

							if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
								DEBUG("ERROR CloseProcedure(): exiting\n");
								exitcode = FAILURE;
								goto ExitMain;
							}

							
							continue;
						}
					}

					if ((pFd->revents & POLLHUP) && ((pFd->revents & POLLIN) == 0)) {
						DEBUG_VAR("POLLHUP flag is set on socket %d, and there are no more input data. The socket wants to close. Closing connection.\n", pFd->fd);

						debug(pConnection_manager, vcInfo.pIcaPacketQueue);

						if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
							DEBUG("ERROR CloseProcedure(): exiting\n");
							exitcode = FAILURE;
							goto ExitMain;
						}

						
						continue;
					}


					if (FD_ISSET(pFd->fd, &(selectArgs.efds))) {
						//connection failed !
						DEBUG_VAR("Connection to socket %d for connection %d failded !\n", pFd->fd, pConnection->id);
						DWORD error;
						int size = sizeof(DWORD);
			
						getsockopt(pFd->fd, SOL_SOCKET, SO_ERROR, &error, &size);

						DEBUG_VAR("SO_ERROR : %d\n", error);
			

						int window = 0;
						if ((window = ConnectionManager_GetConnectionWindowByIndex(pConnection_manager, i)) < MAX_ICA_PACKET_DATA_SIZE) {
							DEBUG_VAR("Connection window not large enough : %d . Skipping to next connection\n", window);
							continue;
						}

						switch (error)
						{
						case WSAENETUNREACH:
							DEBUG("Network unreachable\n");
							Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pConnection->id, (VER)0x05, (RPL)0x03);
							break;
						case WSAEHOSTUNREACH:
							DEBUG("Host unreachable\n");
							Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pConnection->id, (VER)0x05, (RPL)0x04);
							break;
						case WSAECONNREFUSED:
							DEBUG("Connection refused\n");
							Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pConnection->id, (VER)0x05, (RPL)0x05);
							break;
						case WSAETIMEDOUT:
							DEBUG("Timed out\n");
							Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pConnection->id, (VER)0x05, (RPL)0x01);
							break;
						case WSAEADDRINUSE:
							DEBUG("Address already in use\n");
							Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pConnection->id, (VER)0x05, (RPL)0x01);
							break;
						case WSAEALREADY:
							DEBUG("WSAEALREADY. Exiting.\n");
							Send_Ica_ServerRplPacket(vcInfo, pIcaSendBuf, pConnection->id, (VER)0x05, (RPL)0x01);
							exitcode = FAILURE;
							goto ExitMain;
							break;
						default:
							DEBUG("unattended error, exiting.\n");
							exitcode = FAILURE;
							goto ExitMain;
							break;
						}

						//ConnectionManager_CloseConnectionById(pConnection_manager, pConnection->id);
						//i--;

						if (SUCCESS != CloseProcedure(pConnection_manager, vcInfo, pIcaSendBuf, pConnection->id)) {
							DEBUG("ERROR CloseProcedure(): exiting\n");
							exitcode = FAILURE;
							goto ExitMain;
						}
					}



				}

				//if (!ReleaseMutex(hConnectionMutex)) {
				//	DEBUG_VAR("FATAL ERROR ReleaseMutex() : %d. Exiting.\n", GetLastError());
				//	exitcode = FAILURE; //TODO?
				//	goto ExitMain;
				//}
				DEBUG("----------------------------------------------------\n")
			}
			else {
				DEBUG("Not enough free space in the Ica queue. Delaying and trying to send\n");
				Sleep(200);
				IcaPacketQueue_Send(vcInfo);
			}
		}


		if (!ReleaseMutex(hConnectionMutex)) {
			DEBUG_VAR("FATAL ERROR ReleaseMutex() : %d. Exiting.\n", GetLastError());
			exitcode = FAILURE; //TODO?
			goto ExitMain;
		}
	



		
	}



	/*
	*  Finally, close the channel and free allocated memory.
	*/
ExitMain:
	if (SUCCESS != Send_Ica_MetaServerStop(vcInfo,pIcaSendBuf )) {
		DEBUG("Could not send the MetaServerStop packet to the client.\n");
	}
	if (vcInfo.hVC) {
		DEBUG("closing vc.\n");
		WFVirtualChannelClose(vcInfo.hVC);
	}
	if (pIcaSendBuf) {
		free(pIcaSendBuf);
		pIcaSendBuf = NULL;
		pIcaSendBufEnd = NULL;
	}
	if (pIcaRecvBuf) {
		free(pIcaRecvBuf);
		pIcaRecvBuf = NULL;
	}

	if (pSocketRecvBuf) {
		free(pSocketRecvBuf);
		pSocketRecvBuf = NULL;
	}

	//if (pConnection_manager) {
	//	ConnectionManager_Close(pConnection_manager);
	//	pConnection_manager = NULL;
	//}

	if (vcInfo.pIcaPacketQueue) {
		IcaPacketQueue_Close(vcInfo.pIcaPacketQueue);
		vcInfo.pIcaPacketQueue = NULL;
	}

	if (exitcode == FAILURE) {
		DEBUG("exiting main with failure code");
	}

	if (vcInfo.hVC) {
		if (TRUE != WFVirtualChannelClose(vcInfo.hVC)) {
			DEBUG("Error trying to close the virtual channel.\n");
		}
	}

	CloseHandle(hConnectionMutex);
	WSACleanup();
	DEBUG("Leaving\n");
	return(exitcode);

} /* main() */


int OpenVC(pVC_INFO pVcnfo) {
	DEBUG("Entering\n");
	int rc;
	
	DEBUG("Trying to open a VC.\n");
	pVcnfo->hVC = WFVirtualChannelOpen(WF_CURRENT_SERVER,
		WF_CURRENT_SESSION,
		VIRTUAL_CHANNEL_NAME);
	if (pVcnfo->hVC) {
		DEBUG_VAR("Vc opened ! handle : %p  \n", pVcnfo->hVC);

	}
	else {
		DEBUG("Failed opening vc...Exiting\n");
		return(FAILURE);
	}

	DEBUG("Query the VC data.\n");
	PVDSOCKS_C2H pVdQBuf = NULL;
	DWORD dwSize;
	rc = WFVirtualChannelQuery(pVcnfo->hVC, WFVirtualClientData, &pVdQBuf, &dwSize);
	if (rc != TRUE) {
		DEBUG("Query failed. Exiting.\n");
		return(FAILURE);

	}
	
	PVD_C2H pVdc2h = NULL;
	PMODULE_C2H pModulec2h = NULL;
	pVdc2h = &pVdQBuf->Header;
	pModulec2h = &pVdc2h->Header;
	DEBUG_VAR("Query OK. Results : channelMask=%d, name=%s %s \n", pVdc2h->ChannelMask, (CHAR*)pModulec2h->ModuleName, (CHAR*)pModulec2h->HostModuleName);
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_MetaServerStart(VC_INFO vcInfo, unsigned char * sendingBuf) {
	DEBUG("Entering\n");

	int rc;

	if (SUCCESS != (rc = Send_IcaMeta(vcInfo, sendingBuf, META_SERVER_START, 0, 0, NULL))) {
		DEBUG_VAR("FATAL ERROR %d : Packet neither sent nor queued, exiting\n", rc);
		return(FAILURE);
	}
	DEBUG("Packet sent or queued\n");
	DEBUG("Leaving\n");

	return(SUCCESS);
}

int Send_Ica_MetaServerStop(VC_INFO vcInfo, unsigned char * sendingBuf) {
	DEBUG("Entering\n");
	
	int rc;

	if (SUCCESS != (rc = Send_IcaMeta(vcInfo, sendingBuf, META_SERVER_STOP, 0, 0, NULL))) {
		DEBUG_VAR("FATAL ERROR %d : Packet neither sent nor queued, exiting\n", rc);
		return(FAILURE);
	}
	DEBUG("Packet sent or queued\n");
	DEBUG("Leaving\n");

	return(SUCCESS);
}



int Send_Ica_CloseRequest(VC_INFO vcInfo, unsigned char * sendingBuf, ID id) {
	DEBUG("Entering\n");

	if (SUCCESS != Send_Ica_GenericPacket(vcInfo, sendingBuf, CLOSE_REQUEST, id, 0, NULL)) {
		DEBUG("FATAL ERROR : close request nor queued not sent\n");
		return(FAILURE);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}


int Send_Ica_CloseAck(VC_INFO vcInfo, unsigned char * sendingBuf, ID id) {
	DEBUG("Entering\n");

	if (SUCCESS != Send_Ica_GenericPacket(vcInfo, sendingBuf, CLOSE_ACK, id, 0, NULL)) {
		DEBUG("FATAL ERROR : close ack nor queued not sent\n");
		return(FAILURE);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_ServerRplPacket(  VC_INFO vcInfo, unsigned char * sendingBuf, ID id, VER ver, RPL rpl) {
	DEBUG("Entering\n");

	unsigned char pSocketServerRplBuf[VER_SIZE + RPL_SIZE]; //ici ou au debut ??
	void * dest = (void *)pSocketServerRplBuf;
	int rc;

	memcpy(dest, &ver, VER_SIZE);
	dest = (VER*)dest + 1;

	memcpy(dest, &rpl, RPL_SIZE);


	if (SUCCESS != (rc = Send_Ica_GenericPacket(vcInfo,sendingBuf, SOCKS_SERVER_RPL, id, sizeof(pSocketServerRplBuf), pSocketServerRplBuf))) {
		DEBUG_VAR("Packet neither sent nor queued, exiting\n", rc);
		return(rc);
	}
	DEBUG("Packet sent or queued\n");
	DEBUG("Leaving\n");

	return(SUCCESS);
}

int Send_Ica_DataPacket(VC_INFO vcInfo,   unsigned char * sendingBuf, ID id, LEN len, unsigned char * pData) {
	DEBUG("Entering\n");
	int rc;

	if (SUCCESS != (rc = Send_Ica_GenericPacket(vcInfo,sendingBuf,SOCKS_DATA,id, len, pData))) {
		DEBUG_VAR("FATAL ERROR Send_Ica_GenericPacket : %d\n", rc);
		return(FAILURE);
	}

	DEBUG("Leaving\n");

	return(SUCCESS);
}

int Send_Ica_GenericPacket(VC_INFO vcInfo, unsigned char * sendingBuf, PACKET_TYPE type, ID id, LEN len, unsigned char * pData) {
	DEBUG("Entering\n");
	ID netOrderId = htonl(id);
	LEN netOrderLen = htons(len);
	void* dest = (void*)sendingBuf;

	memcpy(dest, &type, PACKET_TYPE_SIZE);
	dest = (PACKET_TYPE *)dest + 1;

	memcpy(dest, &netOrderId, ID_SIZE);
	dest = (ID *)dest + 1;

	memcpy(dest, &netOrderLen, LEN_SIZE);
	dest = (LEN *)dest + 1;

	if (len > 0) {
		if (pData) {
			memcpy(dest, pData, len);
		}
		else {
			DEBUG("ERROR pData NULL and len >0\n");
			return(FAILURE);
		}
	}
	

	if (SUCCESS != Send_IcaQueue(vcInfo)) {
		DEBUG("Error Send_IcaQueue()\n");
		return(FAILURE);
	}
	if (SUCCESS != Send_Ica_Queueing(vcInfo, PACKET_TYPE_SIZE + ID_SIZE + LEN_SIZE + len, sendingBuf)) {
		DEBUG("Error Send_Ica_Queueing : could not send nor queue the packet\n");
		return(FAILURE);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_IcaQueue(VC_INFO vcInfo) {
	DEBUG("Entering\n");
	while (!IcaPacketQueue_IsEmpty(vcInfo.pIcaPacketQueue)) {
		MEMORY_SECTION	* pMs = IcaPacketQueue_GetHead(vcInfo.pIcaPacketQueue);

		if (SUCCESS != Send_Icav2(vcInfo, pMs->length, pMs->pSection)) {
			DEBUG("Error when trying to send the packet\n");
			//TODO: s'adpater en fonction du type d' erreur de Send_Ica (pour l'instant il ne renvoie que FAILURE)
			return(SUCCESS);
		}

		if (SUCCESS != IcaPacketQueue_Dequeue(vcInfo.pIcaPacketQueue)) {
			DEBUG("FATAL ERROR : packet sent but not dequeued\n");
			return(ERROR_FATAL);
		}
	}
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_QueueingPriority(VC_INFO vcInfo, LEN len, unsigned char * pBuf) {
	DEBUG("Entering\n");

	if (SUCCESS == Send_Icav2(vcInfo, len, pBuf)) {
		DEBUG("Packet sent\n");
		return(SUCCESS);
	}

	DEBUG("Send_Ica failed, queueing the packet at the head of the queue\n");
	MEMORY_SECTION mS;
	mS.length = len;
	mS.pSection = pBuf;
	if (SUCCESS == IcaPacketQueue_EnqueueHead(vcInfo.pIcaPacketQueue, &mS, 1)) {
		DEBUG("Packet queued at head\n");
		return(SUCCESS);
	}

	DEBUG("Leaving\n");
	return(FAILURE);
}

int Send_Ica_Queueing(VC_INFO vcInfo, LEN len, unsigned char * pBuf) {
	DEBUG("Entering\n");
	if (IcaPacketQueue_IsEmpty(vcInfo.pIcaPacketQueue)) {
		DEBUG("Queue is empty, directly sending the packet\n");

		if (SUCCESS == Send_Icav2(vcInfo, len, pBuf)) {
			DEBUG("Packet sent\n");
			return(SUCCESS);
		}

	}


	DEBUG("Queue is not empty, or Send_Ica() failed, queueing the packet \n");
	MEMORY_SECTION mS;
	mS.length = len;
	mS.pSection = pBuf;

	if (SUCCESS == IcaPacketQueue_Enqueue(vcInfo.pIcaPacketQueue, &mS, 1)) {
		DEBUG("Packet queued\n");
		return(SUCCESS);
	}

	DEBUG("Leaving\n");
	return(FAILURE);
}

int Send_Icav2(VC_INFO vcInfo, LEN len, unsigned char * pBuf) {
	DEBUG("Entering\n");
	ULONG bytesSent = 0;
	if (TRUE != WFVirtualChannelWrite(vcInfo.hVC, pBuf, len, &bytesSent)) {
		int error = GetLastError();
		DEBUG_VAR("Error WFVirtualChannelWrite() : %d\n",error);
		switch (error)
		{	
			//TODO : se renseigner sur les erreurs possibles de WFVirtualChannelWrite et completer.
		default:
			return(FAILURE);
			break;
		}
		
	}
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica( VC_INFO vcInfo,  int size, unsigned char * buffer) {
	DEBUG("Entering\n");

	int rc = 0;
	ULONG bytesWritten = 0;

	MEMORY_SECTION memorySection;
	memorySection.pSection = buffer;
	memorySection.length = size;

	if (IcaPacketQueue_Send(vcInfo) != SUCCESS) {
		DEBUG("Cannot send the packet because queue could not be emptied, queueing the packet instead\n");
		if (IcaPacketQueue_Enqueue(vcInfo.pIcaPacketQueue, &memorySection, 1) != SUCCESS) {
			DEBUG("Error queueing packet, returning FAILURE\n");
			return(FAILURE);
		}
		IcaPacketQueue_Send(vcInfo);
		return(SUCCESS);
	}

	DEBUG_VAR("size : %d, buffer : %p\n", size, buffer);
	rc = FALSE;
	//debugBuf(buffer, size);


	rc = WFVirtualChannelWrite(vcInfo.hVC, buffer, size, &bytesWritten);
	
	

	if (rc == FALSE) {
		DEBUG_VAR("Error WFVirtualChannelWrite() : %d, queueing the packet\n", GetLastError());

		if (IcaPacketQueue_Enqueue(vcInfo.pIcaPacketQueue, &memorySection, 1) != SUCCESS) {
			DEBUG("Fatal error queueing the packet, packet lost, returning FAILURE\n");
			return(FAILURE);
		}
		IcaPacketQueue_Send(vcInfo);
	
	}

	DEBUG("Leaving\n")
	return(SUCCESS);
}

int Send_IcaMeta(VC_INFO vcInfo, unsigned char * sendingBuf, PACKET_TYPE type, ID id, LEN len, unsigned char * pData) {
	DEBUG("Entering\n");

	PACKET_TYPE tpe = type;
	ID netOrderId = htonl(id);
	LEN netOrderLen = htons(len);
	void* dest = (void*)sendingBuf;
	DEBUG(" 0")
	memcpy(dest, &tpe, PACKET_TYPE_SIZE);
	dest = (PACKET_TYPE *)dest + 1;
	DEBUG("1")
	memcpy(dest, &netOrderId, ID_SIZE);
	dest = (ID *)dest + 1;
	DEBUG("2")
	memcpy(dest, &netOrderLen, LEN_SIZE);
	dest = (LEN *)dest + 1;
	DEBUG("3")
	if (len > 0) {
		if (pData) {
			memcpy(dest, pData, len);
		}
		else {
			DEBUG("ERROR pData NULL and len >0\n");
			return(FAILURE);
		}
	}


	if (SUCCESS != Send_Ica_QueueingPriority(vcInfo, PACKET_TYPE_SIZE + ID_SIZE + LEN_SIZE + len, sendingBuf)) {
		DEBUG("Error Send_Ica_QueueingPriority : could not send nor queue the packet\n");
		return(FAILURE);
	}

	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Ica_MetaWindow(VC_INFO vcInfo, unsigned char * sendingBuf, int window, ID id) {
	DEBUG("Entering\n");
	if (SUCCESS != Send_IcaMeta(vcInfo, sendingBuf, META_WINDOW ,id, sizeof(int), (unsigned char *)&window)) {
		DEBUG("Error in Send_Ica_Meta()\n");
		return(FAILURE);
	}
	DEBUG("Leaving\n");
	return(SUCCESS);
}


int Send_Socket_DataPacket(SOCKET fd, pSOCKET_QUEUE pSocketQueue, LEN len, unsigned char * pBuf, int *pBytesSent) {
	DEBUG("Entering\n");
	int bytesSent1 = 0;
	int bytesSent2 = 0;
	int rc;

	rc = Send_SocketQueue(fd, pSocketQueue, &bytesSent1);
	if (rc == ERROR_FATAL || rc == ERROR_CLOSE_CONNECTION) {
		return(rc);
	}

	rc = Send_Socket_Queueing(fd, pSocketQueue, len, pBuf, &bytesSent2);
	if (pBytesSent) { *pBytesSent = bytesSent1 + bytesSent2; }

	return(rc);	
}

int Send_SocketQueue(SOCKET fd, pSOCKET_QUEUE pSocketQueue, int * pBytesSent) {
	DEBUG("Entering\n");
	unsigned char * pBuf = NULL;
	int n = min(pSocketQueue->size, 100);
	int rc;
	int len;
	int bytesSent = 0;
	int totalSent = 0;

	if (pBytesSent) { *pBytesSent = totalSent; }

	while (!SocketQueue_IsEmpty(pSocketQueue)) {
		

		len = SocketQueue_GetN(pSocketQueue, n, &pBuf);
		if (SUCCESS != (rc = Send_Socketv2(fd, len, pBuf, &bytesSent))) {
			return(rc);
		}
		SocketQueue_Dequeue(pSocketQueue, bytesSent);
		totalSent += bytesSent;
		if (pBytesSent) { *pBytesSent = totalSent; }
	}
	
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Socket_Queueing(SOCKET fd, pSOCKET_QUEUE pQueue, LEN len, unsigned char * pBuf, int *pBytesSent) {
	DEBUG("Entering\n");
	int bytesSent = 0;
	if (SocketQueue_IsEmpty(pQueue)) {
		DEBUG("Queue is empty, directly sending the packet\n");
		int rc;
		rc = Send_Socketv2(fd, len, pBuf, &bytesSent);
		if (rc == ERROR_CLOSE_CONNECTION) {
			return(ERROR_CLOSE_CONNECTION);
		}
		else if (rc == ERROR_FATAL) {
			return(ERROR_FATAL);
		}
		else if (rc == SUCCESS) {
			if (pBytesSent) { *pBytesSent = bytesSent; }

			if (bytesSent != len) {

			
				DEBUG("Packet partially sent, queueing the rest\n");
				if (SUCCESS != SocketQueue_Enqueue(pQueue, pBuf + bytesSent, len - bytesSent)) {
					DEBUG("Error queueing the remaining bytes.\n");
					return(FAILURE);
				}
				DEBUG("Remaining bytes successfully queued.\n");
				return(SUCCESS);
			}
			else {
				DEBUG("Packet entirely sent\n");
				return(SUCCESS);
			}
		}
	}
	
	DEBUG("Queue is not empty, or Send_Socket() returned FAILURE\n ");
	if (pBytesSent) { *pBytesSent = 0; }
	if (SUCCESS != SocketQueue_Enqueue(pQueue, pBuf, len)) {
		DEBUG("Error queueing the packet\n");
		return(FAILURE);
	}
	DEBUG("Leaving\n");
	return(SUCCESS);

	
}

int Send_Socketv2(SOCKET fd, LEN len, unsigned char * pBuf, int * pBytesSent) {
	DEBUG("Entering\n");
	int rc;
	if (SOCKET_ERROR == (rc = send(fd, pBuf, len, 0))) {
		int error = WSAGetLastError();
		switch (error)
		{
		case WSAENOTSOCK:
			DEBUG("WSAENOTSOCK : fd is not a valid socket, returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAENETRESET:
			DEBUG("WSAENETRESET : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAENOTCONN:
			DEBUG("WSAENOTCONN : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAESHUTDOWN:
			DEBUG("WSAESHUTDOWN :  returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAEINVAL:
			DEBUG("WSAEINVAL :  returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAECONNABORTED:
			DEBUG("WSAECONNABORTED : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAECONNRESET:
			DEBUG("WSAECONNRESET : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAETIMEDOUT:
			DEBUG("WSAETIMEDOUT : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAEINTR:
			DEBUG("WSAEINTR : call was cancelled, queueing the packet instead.\n");
			return(FAILURE);
			break;
		case WSAEINPROGRESS:
			DEBUG("WSAEINPROGRESS : queuing the packet.\n");
			return(FAILURE);
			break;
		case WSAENETDOWN:
			DEBUG("WSAENETDOWN : queuing the packet.\n");
			return(FAILURE);
			break;
		case WSAENOBUFS:
			DEBUG("WSAENOBUFS : queuing the packet.\n");
			return(FAILURE);
			break;
		case WSAEWOULDBLOCK:
			DEBUG("WSAEWOULDBLOCK : queuing the packet.\n");
			return(FAILURE);
			break;
		case WSAEHOSTUNREACH:
			DEBUG("WSAEHOSTUNREACH : queuing the packet.\n");
			return(FAILURE);
			break;

		default:
			DEBUG_VAR("Unattended error in send() : %d. Returning FATAL_ERROR.\n", error);
			return(ERROR_FATAL);
			break;
		}
	}

	if (pBytesSent) {
		*pBytesSent = rc;
	}
	DEBUG_VAR("Successful send : %d bytes.\n", rc);
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Send_Socket(SOCKET fd, pSOCKET_QUEUE pQueue, unsigned int len, char * pBuf) {
	DEBUG("Entering\n");
	ULONG bytesSent = 0;
	int rc = 0;

	if (SUCCESS != SocketQueue_SendAll(pQueue, fd)) {
		DEBUG("Cannot send because queue is not empty. Queueing instead.\n");
		if (SUCCESS != SocketQueue_Enqueue(pQueue, pBuf, len)) {
			DEBUG("Queueing error : packet is not sent nor queued. Returning FAILURE.\n");
			return(FAILURE);
		}
		DEBUG("Packet not sent but queued. Returning SUCCESS.\n");
		return(SUCCESS);
	}

	DEBUG("Queue is now empty. Trying to send packet.\n");

	if (SOCKET_ERROR == (rc = send(fd, pBuf, len, 0))) {
		int error = WSAGetLastError();
		switch (error)
		{
		case WSAENOTSOCK:
			DEBUG("WSAENOTSOCK : fd is not a valid socket, returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAENETRESET:
			DEBUG("WSAENETRESET : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAENOTCONN:
			DEBUG("WSAENOTCONN : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAESHUTDOWN:
			DEBUG("WSAESHUTDOWN :  returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAEINVAL:
			DEBUG("WSAEINVAL :  returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAECONNABORTED:
			DEBUG("WSAECONNABORTED : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAECONNRESET:
			DEBUG("WSAECONNRESET : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAETIMEDOUT:
			DEBUG("WSAETIMEDOUT : returning ERROR_CLOSE_CONNECTION.\n");
			return(ERROR_CLOSE_CONNECTION);
			break;
		case WSAEINTR:
			DEBUG("WSAEINTR : call was cancelled, queueing the packet instead.\n");
			break;
		case WSAEINPROGRESS:
			DEBUG("WSAEINPROGRESS : queuing the packet.\n");
			break;
		case WSAENETDOWN:
			DEBUG("WSAENETDOWN : queuing the packet.\n");
			break;
		case WSAENOBUFS:
			DEBUG("WSAENOBUFS : queuing the packet.\n");
			break;
		case WSAEWOULDBLOCK:
			DEBUG("WSAEWOULDBLOCK : queuing the packet.\n");
			break;
		case WSAEHOSTUNREACH:
			DEBUG("WSAEHOSTUNREACH : queuing the packet.\n");
			break;
	
		default:
			DEBUG_VAR("Unattended error in send() : %d. Returning FAILURE.\n", error);
			return(FAILURE);
			break;
		}

		if (SUCCESS != SocketQueue_Enqueue(pQueue, pBuf, len)) {
			DEBUG("Queueing error : packet is not sent nor queued. Returning FAILURE.\n");
			return(FAILURE);
		}
		DEBUG("Packet not sent but queued. Returning SUCCESS.\n");
		return(SUCCESS);
	}

	bytesSent = (ULONG)rc;

	if (bytesSent != len) {
		DEBUG_VAR("%d bytes were sent out of %d. Queueing the rest.\n", bytesSent, len);
		if (SUCCESS != SocketQueue_Enqueue(pQueue, (char *)(pBuf+bytesSent), len-bytesSent)) {
			DEBUG("Queueing error : packet is not sent nor queued. Returning FAILURE.\n");
			return(FAILURE);
		}
		DEBUG("Packet was part sent and part queued. Returning SUCCESS.\n");
		return(SUCCESS);
	}
	
	DEBUG("Packet sent. Returning SUCCESS.\n")
	

	DEBUG("Leaving\n");
	return(SUCCESS);
}


void Parse_Ica_GenericPacket(IN unsigned char * pPacketBuf, OUT PACKET_TYPE *ptype, OUT ID *pid, OUT LEN *plen, OUT unsigned char ** ppdata) {
	void * address = (void *)pPacketBuf;

	*ptype = *((PACKET_TYPE *)address);
	address = (PACKET_TYPE *)address + 1;

	*pid = htonl(*((ID *)address));
	address = (ID *)address + 1;

	*plen = htons(*((LEN *)address));
	address = (LEN *)address + 1;

	*ppdata = (unsigned char *)address;
}

void Parse_Ica_ClientIpCmd(IN unsigned char * pPacketBuf, OUT VER *pVer, OUT CMD *pCmd, OUT IPADDR *pIpAddr, OUT PORT *pPort) {
	DEBUG("Entering\n");
	void * address = (void *)pPacketBuf;

	*pVer = *((VER *)address);
	address = (VER *)address + 1;

	*pCmd = *((CMD *)address);
	address = (CMD *)address + 1;

	*pIpAddr = ntohl(*((IPADDR *)address));
	address = (IPADDR *)address + 1;

	*pPort = ntohs(*((PORT *)address));

	DEBUG_VAR("Parsing : VER=%d, CMD=%d, IPADDR=%u, PORT=%d \n", *pVer, *pCmd, *pIpAddr, *pPort);

}
void Parse_Ica_ClientIp6Cmd(IN unsigned char * pPacketBuf, OUT VER *pVer, OUT CMD *pCmd, OUT IP6ADDR *pIp6Addr, OUT PORT *pPort) {
	DEBUG("Entering\n");
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
}
void Parse_Ica_ClientDomainCmd(IN unsigned char * pPacketBuf, OUT VER *pVer, OUT CMD *pCmd, OUT PORT *pPort, OUT DOMLEN * pDomLen, OUT DOMSTR * ppDomData) {
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

	DEBUG_VAR("Parsing : VER=%d, CMD=%d, PORT=%d, DOMLEN=%d, pDOMDATA=%p \n", *pVer, *pCmd, *pPort, *pDomLen, *ppDomData);

}

void Parse_Ica_MetaWindow(IN unsigned char * pPacketBuf, OUT int * pWindow) {
	DEBUG("Entering\n");
	*pWindow = *((int*)pPacketBuf);
	DEBUG_VAR("Parsing : window=%d.\n", *pWindow);
	DEBUG("Leaving\n");
}


int Handler_Ica_DataPacket(pCONNECTION_MANAGER pConnectionManager,VC_INFO vcInfo, unsigned char * sendingBuf, ID id, LEN len, unsigned char *pBuf) {
	DEBUG("Entering\n");
	debug(pConnectionManager, vcInfo.pIcaPacketQueue);

	ULONG bytesSent = 0;
	int rc = 0;
	
	
	pCONNECTION pConnection;
	if (NULL == (pConnection = ConnectionManager_GetConnectionById(pConnectionManager, id))) {
		DEBUG_VAR("Error getting connection %d, sending back a closing packet.\n", id);


		return(FAILURE);
	}

	if (pConnection->state == ESTABLISHED) {
		DEBUG("there2\n");
		DEBUG_VAR("fd : %d\n", pConnection->socket_fd);

		if (SUCCESS != (rc = Send_Socket_DataPacket(pConnection->socket_fd, pConnection->pSocketQueue, len, pBuf, &bytesSent))) {
			switch (rc)
			{
			case ERROR_CLOSE_CONNECTION:
				DEBUG("ERROR_CLOSE_CONNECTION : start closing the connection.\n");
				
				if (SUCCESS != CloseProcedure(pConnectionManager, vcInfo, sendingBuf, id)) {
					DEBUG("Error CloseProcedure()\n");
					return(FAILURE);
				}
		
				return(SUCCESS);
				break;
			case FAILURE: 
				DEBUG("FATAL ERROR Send_Socket : packet not sent nor queued. Returning FAILURE. \n");
				return(FAILURE);
			case ERROR_FATAL:
				DEBUG("FATAL ERROR Send_Socket : packet not sent nor queued. Returning FAILURE. \n");
				return(FAILURE);
			default:
				DEBUG_VAR("Unattended error in Send_Socket() : %d. Returning FAILURE.\n", rc);
				return(FAILURE);
				break;
			}
		}

		if (SUCCESS != Send_Ica_MetaWindow(vcInfo,sendingBuf, bytesSent, id)) {
			DEBUG("FATAL ERROR : could not send the meta window packet\n");
			return(FAILURE);
		}

		DEBUG("Packet successfully sent or queued. Returning SUCCESS.\n");
	}
	else if (pConnection->state == CLOSING) {
		DEBUG("Connection in CLOSING state, dropping the packet\n");

	}
	else {
		DEBUG("Connection not established yet. Weird. Returning FAILURE.\n");
		return(FAILURE);
		
	}
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int Handler_Ica_ClientIpCmd(pCONNECTION_MANAGER pConnectionManager, VC_INFO vcInfo, unsigned char * sendingBuf, ID id, LEN len, unsigned char * pBuf) {
	DEBUG("Entering\n");
	DEBUG_VAR(" len : %zu\n", len);
	DEBUG_VAR(" pBuf : %p\n", pBuf);
	VER ver;
	CMD cmd;
	IPADDR ipAddr;
	PORT port;

	


	if (SUCCESS == ConnectionManager_NewConnection(pConnectionManager, id)) {
		DEBUG(" Connection created");
		//debug();

		Parse_Ica_ClientIpCmd(pBuf, &ver, &cmd, &ipAddr, &port);

		pCONNECTION pConnection = ConnectionManager_GetConnectionById(pConnectionManager, id);
		WSAPOLLFD * pFd = ConnectionManager_GetPollFdById(pConnectionManager, id);

		pConnection->socket_fd = OpenSocket(ipAddr, port);


		if (pConnection->socket_fd == INVALID_SOCKET) {
			Send_Ica_ServerRplPacket(vcInfo,sendingBuf , id, 0x05, 0x01);

			if (SUCCESS != CloseProcedure(pConnectionManager, vcInfo, sendingBuf, id)) {
				DEBUG("Error CloseProcedure()\n");
				return(FAILURE);
			}
	
			
		}

		pConnection->state = CONNECTING;
		pFd->fd = pConnection->socket_fd;
		pFd->events = POLLOUT;



	}
	else {
		DEBUG("Could not allocate a connection.\n");
		Send_Ica_ServerRplPacket(vcInfo, sendingBuf, id, 0x05, 0x01);
	}
	DEBUG("Leaving\n");
	return(0);
}

int Handler_Ica_ClientIp6Cmd(pCONNECTION_MANAGER pConnectionManager, VC_INFO vcInfo, unsigned char * sendingBuf, ID id, LEN len, unsigned char * pBuf) {
	DEBUG("Entering\n");
	DEBUG_VAR(" len : %zu\n", len);
	DEBUG_VAR(" pBuf : %p\n", pBuf);
	DEBUG_BUF(pBuf, len);
	VER ver;
	CMD cmd;
	IP6ADDR ip6Addr;
	PORT port;




	if (SUCCESS == ConnectionManager_NewConnection(pConnectionManager, id)) {
		DEBUG(" Connection created\n");
		//debug();
		
		Parse_Ica_ClientIp6Cmd(pBuf, &ver, &cmd, &ip6Addr, &port);

		pCONNECTION pConnection = ConnectionManager_GetConnectionById(pConnectionManager, id);
		WSAPOLLFD * pFd = ConnectionManager_GetPollFdById(pConnectionManager, id);

		pConnection->socket_fd = OpenSocketIp6(ip6Addr, port);


		if (pConnection->socket_fd == INVALID_SOCKET) {
			Send_Ica_ServerRplPacket(vcInfo, sendingBuf, id, 0x05, 0x01);

			if (SUCCESS != CloseProcedure(pConnectionManager, vcInfo, sendingBuf, id)) {
				DEBUG("Error CloseProcedure()\n");
				return(FAILURE);
			}


		}

		pConnection->state = CONNECTING;
		pFd->fd = pConnection->socket_fd;
		pFd->events = POLLOUT;



	}
	else {
		DEBUG("Could not allocate a connection.\n");
		Send_Ica_ServerRplPacket(vcInfo, sendingBuf, id, 0x05, 0x01);
	}
	DEBUG("Leaving\n");
	return(0);
}

int Handler_Ica_ClientDomainCmd(pCONNECTION_MANAGER pConnectionManager, VC_INFO vcInfo, unsigned char * sendingBuf, ID id, LEN len, unsigned char * pBuf) {
	DEBUG("Entering\n");
	DEBUG_VAR(" len : %zu\n", len);
	DEBUG_VAR(" pBuf : %p\n", pBuf);
	VER ver;
	CMD cmd;
	PORT port;
	DOMLEN domLen;
	DOMSTR pDomData;



	if (ConnectionManager_NewConnection(pConnectionManager, id) == 0) {
		DEBUG(" Connection created");
		//debug();

		pCONNECTION pConnection = ConnectionManager_GetConnectionById(pConnectionManager, id);
		WSAPOLLFD * pFd = ConnectionManager_GetPollFdById(pConnectionManager, id);

		
		HANDLE threadHandle;

		Parse_Ica_ClientDomainCmd(pBuf, &ver, &cmd, &port, &domLen, &pDomData);
		//debugBuf(pDomData, domLen);

		pDomData[domLen] = 0;

		pConnection->socket_fd = OpenSocketDomain(pDomData, port);

		if (pConnection->socket_fd == INVALID_SOCKET) {
			Send_Ica_ServerRplPacket(vcInfo,sendingBuf , id, 0x05, 0x01);
			ConnectionManager_CloseConnectionById(pConnectionManager, id);
			exit(1);
		}
		
		pDNS_PARAMS pDns_params;
		if (NULL == (pDns_params = malloc(sizeof(DNS_PARAMS)))) {
			DEBUG(" Error allocation DNSPARAMS\n");
			return(-1);
		}

		pDns_params->hostname = (char *)pDomData;
		pDns_params->id = id;
		pDns_params->pConnectionManager = pConnectionManager;
		pDns_params->pSendingBuf = sendingBuf;
		pDns_params->vcInfo = vcInfo;


		DEBUG_VAR("hostname : %s\n", pDns_params->hostname);
		DEBUG_VAR("id : %d\n", pDns_params->id);

		
		if (NULL == (pDns_params->handle = CreateThread(NULL, 0, ResolveDNS, (LPVOID)pDns_params, 0, NULL))) {
			DEBUG_VAR("Error creating the thread : %d\n", GetLastError());
			return(-1);
		}
		DEBUG_VAR(" handle : %p\n", pDns_params->handle)


		DEBUG("Resolving\n");

		pConnection->server.sin_port = htons(port);
		DEBUG_VAR("Set the port : %d\n", ntohs(pConnection->server.sin_port));
		debug(pConnectionManager, vcInfo.pIcaPacketQueue);

		pConnection->state = RESOLVING;
		pFd->fd = pConnection->socket_fd;
		pFd->events = POLLOUT;



	}
	else {
		DEBUG("Could not allocate a connection.\n");
		Send_Ica_ServerRplPacket(vcInfo, sendingBuf, id, 0x05, 0x01);
	}
	DEBUG("Leaving\n");
	return(0);
}



int Handler_Ica_CloseRequest(pCONNECTION_MANAGER pConnectionManager, VC_INFO vcInfo, unsigned char * sendingBuf, ID id) {
	DEBUG( "Entering\n");
	
	pCONNECTION  pConnection;
	if (NULL == (pConnection = ConnectionManager_GetConnectionById(pConnectionManager, id))) {
		DEBUG("Error get connection\n");
		return(FAILURE);
	}

	if (pConnection->state == RESOLVING) {
		DEBUG("Close request received while connection is still resolving\n");
		if (SUCCESS != ConnectionManager_SetConnectionStateById(pConnectionManager, id, TO_CLOSE)) {
			DEBUG("Error setting state to TO_CLOSE\n");
			return(FAILURE);
		}
		return(SUCCESS);
	}
	

	if (SUCCESS != ConnectionManager_CloseConnectionById(pConnectionManager, id)) {
		//TODO : considerer chaque error code en cas d'echec
		DEBUG( "ERROR : could not close the connection.\n");
		return (FAILURE);
	}

	if (SUCCESS != Send_Ica_CloseAck(vcInfo,sendingBuf, id)) {
		DEBUG( "ERROR : could not send or queue the CloseAck\n");
		return (FAILURE);
	}

	DEBUG( "Leaving\n");
	return (SUCCESS);
}

int Handler_Ica_CloseAck(pCONNECTION_MANAGER pConnectionManager, ID id) {
	DEBUG( "Entering\n");
	CONNECTION_STATE state;

	if (SUCCESS != ConnectionManager_GetConnectionStateById(pConnectionManager, id, &state)) {
		DEBUG( "ERROR : could not get the connection state\n");
		return (FAILURE);
	}

	if (state != CLOSING) {
		DEBUG( "ERROR : close ack received but connection not in closing state\n");
		return (FAILURE);
	}

	if (SUCCESS != ConnectionManager_CloseConnectionById(pConnectionManager, id)) {
		//TODO : considerer chaque error code en cas d'echec
		DEBUG( "ERROR : could not close the connection.\n");
		return (FAILURE);
	}

	DEBUG( "Leaving\n");
	return(SUCCESS);  
}

int Handler_Ica_MetaWindow(unsigned char * pBuf, pCONNECTION_MANAGER pConnectionManager, ID id) {
	DEBUG("Entering.\n");
	int window;

	Parse_Ica_MetaWindow(pBuf, &window);
	
	ConnectionManager_AddConnectionWindowById(pConnectionManager, id, window);

	return(SUCCESS);
	DEBUG("Leaving.\n");
}

SOCKET OpenSocket(IPADDR addr, PORT port) {
	DEBUG("Entering\n");
	SOCKET connection_socket = INVALID_SOCKET;
	struct sockaddr_in server;

	if ((connection_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		DEBUG_VAR("Could not create socket : %d", WSAGetLastError());
		return INVALID_SOCKET;
	}
	DEBUG("Socket created\n");

	ULONG mode = 1;
	if (ioctlsocket(connection_socket, FIONBIO, &mode) == SOCKET_ERROR) {
		DEBUG_VAR("Error ioctlsocket() : %d.\n", WSAGetLastError());
		return INVALID_SOCKET;
	}
	DEBUG("Socket set to non blocking.\n");




	server.sin_addr.s_addr = htonl(addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (connect(connection_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK) {
			//Normal error due to immediate return of connect for non blocking sockets
			DEBUG("connect returned SOCKET_ERROR and WSAEWOULDBLOCK. \n");
			return(connection_socket);
		}

		//If the error code is different from WSAEWOULDBLOCK, it indicates a real error
		DEBUG_VAR("Error connect() : %d.\n", error);
		return INVALID_SOCKET;
	}

	DEBUG("Connected\n");
	DEBUG("Leaving\n");
	return(connection_socket);

}

SOCKET OpenSocketIp6(IP6ADDR ip6Addr, PORT port) {
	DEBUG("Entering\n");
	SOCKET connection_socket = INVALID_SOCKET;
	struct sockaddr_in6 server;
	memset(&server, 0, sizeof(server));
	

	if ((connection_socket = socket(AF_INET6, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		DEBUG_VAR("Could not create socket : %d", WSAGetLastError());
		return INVALID_SOCKET;
	}
	DEBUG("Socket created\n");

	ULONG mode = 1;
	if (ioctlsocket(connection_socket, FIONBIO, &mode) == SOCKET_ERROR) {
		DEBUG_VAR("Error ioctlsocket() : %d.\n", WSAGetLastError());
		return INVALID_SOCKET;
	}
	DEBUG("Socket set to non blocking.\n");


	

	memcpy(server.sin6_addr.s6_addr, ip6Addr, 16);
	server.sin6_family = AF_INET6;
	server.sin6_port = htons(port);

	if (connect(connection_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK) {
			//Normal error due to immediate return of connect for non blocking sockets
			DEBUG("connect returned SOCKET_ERROR and WSAEWOULDBLOCK. \n");
			return(connection_socket);
		}

		//If the error code is different from WSAEWOULDBLOCK, it indicates a real error
		DEBUG_VAR("Error connect() : %d.\n", error);
		return INVALID_SOCKET;
	}

	DEBUG("Connected\n");
	DEBUG("Leaving\n");
	return(connection_socket);

}

SOCKET OpenSocketDomain(DOMSTR domain, PORT port) {
	DEBUG("Entering\n");
	SOCKET connection_socket = INVALID_SOCKET;


	if ((connection_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		DEBUG_VAR("Could not create socket : %d", WSAGetLastError());
		return INVALID_SOCKET;
	}
	DEBUG("Socket created\n");

	ULONG mode = 1;
	if (ioctlsocket(connection_socket, FIONBIO, &mode) == SOCKET_ERROR) {
		DEBUG_VAR("Error ioctlsocket() : %d.\n", WSAGetLastError());
		return INVALID_SOCKET;
	}
	DEBUG("Socket set to non blocking.\n");

	DEBUG("Leaving\n");
	return(connection_socket);
}

DWORD WINAPI ResolveDNS(LPVOID param) {
	DEBUG("Entering\n");
	
	pDNS_PARAMS pDns_params = (pDNS_PARAMS)param;
	debug(pDns_params->pConnectionManager, NULL);

	DEBUG_VAR("%d: hostname : %s\n", pDns_params->handle, pDns_params->hostname);
	DEBUG_VAR("%d:id : %d\n",pDns_params->handle, pDns_params->id);

	struct hostent *hostinfo = NULL;
	struct sockaddr_in server;

	char * hostname = pDns_params->hostname;
	hostinfo = gethostbyname(hostname); /* on r�cup�re les informations de l'h�te auquel on veut se connecter */
	
	WaitForSingleObject(hConnectionMutex, INFINITE);
	DEBUG_VAR("%d:getting co %d\n", pDns_params->handle, pDns_params->id);
	pCONNECTION pConnection;
	if (NULL == (pConnection = ConnectionManager_GetConnectionById(pDns_params->pConnectionManager, pDns_params->id))) {
		DEBUG_VAR("%d:ERROR GetConnectionById(). Exiting\n", pDns_params->handle);
		exit(1);
	}

	if (pConnection->state == TO_CLOSE) {
		DEBUG_VAR("%d:Resolving is over but connection is in TO_CLOSE state, closing it.\n" ,pDns_params->handle);
		if (SUCCESS != ConnectionManager_CloseConnectionById(pDns_params->pConnectionManager, pDns_params->id)) {
			//TODO : considerer chaque error code en cas d'echec
			DEBUG_VAR("%d:ERROR : could not close the connection.\n", pDns_params->handle);
			exit(1);
		}

		if (SUCCESS != Send_Ica_CloseAck(pDns_params->vcInfo, pDns_params->pSendingBuf, pDns_params->id)) {
			DEBUG_VAR("%d:ERROR : could not send or queue the CloseAck\n", pDns_params->handle);
			exit(1);
		}
	}

	if (hostinfo == NULL) /* l'h�te n'existe pas */
	{
		DEBUG_VAR("%d:Unknown host %s.\n", pDns_params->handle, hostname);
		pConnection->state = UNRESOLVED;
		
	}
	else {
		DEBUG_VAR("%d:DNS Resolution OK\n", pDns_params->handle);
		DEBUG_VAR("%d:hostinfo.h_addr : %d\n", pDns_params->handle, ntohl(hostinfo->h_addr));


		pConnection->server.sin_addr = *(IN_ADDR *)hostinfo->h_addr;
		pConnection->server.sin_family = AF_INET;
		pConnection->state = RESOLVED;
	}

	ReleaseMutex(hConnectionMutex);
	debug(pDns_params->pConnectionManager, NULL);
	DEBUG_VAR("%d:Leaving ResolveDNS\n", pDns_params->handle);

	DEBUG_VAR("handle : %p\n", pDns_params->handle)
	CloseHandle(pDns_params->handle);
	if (pDns_params) {
		free(pDns_params);
		pDns_params = NULL;
	}


	DEBUG("Leaving\n");
	return(SUCCESS);
}

int ConnectSocket(pCONNECTION_MANAGER pConnectionManager, int index) {
	DEBUG("Entering\n");
	DEBUG_VAR(" inde  x  : %d\n", index);
	debug(pConnectionManager, NULL);
	pCONNECTION pConnection = ConnectionManager_GetConnectionByIndex(pConnectionManager, index);
	DEBUG_VAR("sin_addr : %d\n", pConnection->server.sin_addr);
	DEBUG_VAR("sin_port : %d\n", pConnection->server.sin_port);
	if (connect(pConnection->socket_fd, (struct sockaddr *)&pConnection->server, sizeof(pConnection->server)) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK) {
			//Normal error due to immediate return of  connect for non blocking sockets
			DEBUG("connect returned SOCKET_ERROR and WSAEWOULDBLOCK. \n");
			pConnection->state = CONNECTING;
			return(0);
		}

		//If the error code is different from WSAEWOULDBLOCK, it indicates a real error
		DEBUG_VAR("Error connect() : %d.\n", error);
		return -1;
	}

	DEBUG("Connected\n");

	DEBUG("Leaving ConnectSocket\n");
	return(0);
}



int IcaPacketQueue_Init(pICA_PACKET_QUEUE * ppQueue) {
	DEBUG("Entering\n");
	if (NULL == (*ppQueue = (pICA_PACKET_QUEUE) malloc(sizeof(ICA_PACKET_QUEUE)))) {
		DEBUG("Memory allocation error : pQueue\n")
		return(ERROR_MEMORY);
	}

	(*ppQueue)->size = 0;
	(*ppQueue)->head = 0;
	(*ppQueue)->tail = -1;

	if (NULL == ((*ppQueue)->sections = calloc(MAX_QUEUE_SIZE ,sizeof(MEMORY_SECTION) ))) {
		DEBUG("Memory allocation error : pQueue->sections\n");

		return(ERROR_MEMORY);
	}

	if (NULL == ((*ppQueue)->data_storage = calloc(MAX_QUEUE_SIZE, MAX_ICA_PACKET_DATA_SIZE ))) {
		DEBUG("Memory allocation error : pQueue->sections\n");

		return(ERROR_MEMORY);
	}
	DEBUG("Leaving\n");
	return(SUCCESS);
}

void IcaPacketQueue_Close(pICA_PACKET_QUEUE pQueue) {
	if (pQueue->data_storage) {
		free(pQueue->data_storage);
		pQueue->data_storage = NULL;
	}

	if (pQueue->sections) {
		free(pQueue->sections);
		pQueue->sections = NULL;
	}

	free(pQueue);
}

int IcaPacketQueue_Enqueue(pICA_PACKET_QUEUE pQueue, MEMORY_SECTION * pMS, int nMS) {
	DEBUG("Entering\n");
	DEBUG("!!!!!!!!!!!!!!!!!!!!!!!ENQUEUE\n");
	unsigned int length = 0;

	if (pQueue->size == MAX_QUEUE_SIZE) {
		DEBUG("Queue Overflow\n");
		return(ERROR_OVERFLOW);
	}

	for (int i = 0; i<nMS; i++) {
		length += pMS[i].length;
	}
	if (length > MAX_ICA_PACKET_DATA_SIZE) {
		DEBUG("Packet overflow\n");
		return(ERROR_OVERFLOW);
	}

	pQueue->tail = (pQueue->tail + 1) % MAX_QUEUE_SIZE;

	unsigned char * p = pQueue->data_storage + (pQueue->tail*MAX_ICA_PACKET_DATA_SIZE);
	for (int i = 0; i<nMS; i++) {
		memcpy(p, pMS[i].pSection, pMS[i].length);
		p += pMS[i].length;
	}

	pQueue->sections[pQueue->tail].length = length;
	pQueue->sections[pQueue->tail].pSection = pQueue->data_storage + (pQueue->tail*MAX_ICA_PACKET_DATA_SIZE);
	pQueue->size++;

	DEBUG("Memory section enqueued\n");
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int IcaPacketQueue_EnqueueHead(pICA_PACKET_QUEUE pQueue, MEMORY_SECTION * pMS, int nMS) {
	DEBUG("Entering\n");
	DEBUG("!!!!!!!!!!!!!!!!!!!!!!!ENQUEUE HEAD\n");
	unsigned int length = 0;

	if (pQueue->size == MAX_QUEUE_SIZE) {
		DEBUG("Queue Overflow\n");
		return(ERROR_OVERFLOW);
	}

	for (int i = 0; i<nMS; i++) {
		length += pMS[i].length;
	}
	if (length > MAX_ICA_PACKET_DATA_SIZE) {
		DEBUG("Packet overflow\n");
		return(ERROR_OVERFLOW);
	}

	pQueue->head = (pQueue->head - 1);
	if (pQueue->head == -1) {
		pQueue->head = MAX_QUEUE_SIZE - 1;
	}

	unsigned char * p = pQueue->data_storage + (pQueue->head*MAX_ICA_PACKET_DATA_SIZE);
	for (int i = 0; i<nMS; i++) {
		memcpy(p, pMS[i].pSection, pMS[i].length);
		p += pMS[i].length;
	}

	pQueue->sections[pQueue->head].length = length;
	pQueue->sections[pQueue->head].pSection = pQueue->data_storage + (pQueue->head*MAX_ICA_PACKET_DATA_SIZE);
	pQueue->size++;

	DEBUG("Memory section enqueued\n");
	DEBUG("Leaving\n");
	return(SUCCESS);
}

int IcaPacketQueue_Dequeue(pICA_PACKET_QUEUE pQueue) {
	DEBUG("Entering\n");
	if (pQueue->size == 0) {
		DEBUG("Queue underlfow\n");
		return(ERROR_UNDERFLOW);
	}

	pQueue->head = (pQueue->head + 1) % MAX_QUEUE_SIZE;
	pQueue->size--;

	DEBUG("Leaving\n");

	return(SUCCESS);
}

MEMORY_SECTION * IcaPacketQueue_GetHead(pICA_PACKET_QUEUE pQueue) {
	DEBUG("Entering\n");
	if (pQueue->size == 0) {
		DEBUG("Queue is empty\n");
		return NULL;
	}
	DEBUG("Leaving\n");
	return (pQueue->sections + (pQueue->head));
}

int IcaPacketQueue_IsFull(pICA_PACKET_QUEUE pQueue) {
	DEBUG("Entering\n");
	DEBUG("Leaving\n");
	return (pQueue->size == MAX_QUEUE_SIZE);
}

int IcaPacketQueue_IsEmpty(pICA_PACKET_QUEUE pQueue) {
	DEBUG("Entering\n");
	DEBUG("Leaving\n");
	return (pQueue->size == 0);
}

int IcaPacketQueue_GetFreeSpace(pICA_PACKET_QUEUE pQueue) {
	DEBUG("Entering\n");
	DEBUG("Leaving\n");
	return(MAX_QUEUE_SIZE - pQueue->size);
}

int IcaPacketQueue_Send(VC_INFO vcInfo) {
	DEBUG("Entering\n");
	ULONG bytesWritten = 0;
	int rc;
	
	if (IcaPacketQueue_IsEmpty(vcInfo.pIcaPacketQueue)) {
		DEBUG("Queue already empty\n");
		return(SUCCESS);
	}

	while (IcaPacketQueue_IsEmpty(vcInfo.pIcaPacketQueue) != 1) {
		DEBUG_VAR("Queue size : %d\n", vcInfo.pIcaPacketQueue->size);

		MEMORY_SECTION *pMS = IcaPacketQueue_GetHead(vcInfo.pIcaPacketQueue);
		
		
		if (TRUE != WFVirtualChannelWrite(vcInfo.hVC, pMS->pSection, pMS->length, &bytesWritten)) {
			DEBUG_VAR("Error WFVirtualChannelWrite : %d",GetLastError() );

			return (ERROR_VC_WRITE);
		}

	
		if (SUCCESS != (rc = IcaPacketQueue_Dequeue(vcInfo.pIcaPacketQueue))) {
			DEBUG_VAR("FATAL ERROR IcaPacketQueue_Dequeue() : %d. A packet was sent but not dequeued. Exiting.\n", GetLastError());
			exit(1);
		}

		DEBUG("Queue packet successfully sent and dequeued\n");


	}
	DEBUG("Queue is empty\n");
	DEBUG("Leaving\n")
	return(SUCCESS);
}


int CloseProcedure(pCONNECTION_MANAGER pConnectionManager, VC_INFO vcInfo, unsigned char * sendingBuf, ID id) {
	pCONNECTION pConnection = ConnectionManager_GetConnectionById(pConnectionManager, id);
	if (pConnection->state == CLOSING) {
		DEBUG("Already in CLOSING state\n");
		return(FAILURE);
	}
	DEBUG("Start the closing procedure\n");
	if (SUCCESS != Send_Ica_CloseRequest(vcInfo, sendingBuf, id)) {
		DEBUG("FATAL ERROR : could not send the ica close request.\n");
		return(FAILURE);
	}
	if (SUCCESS != ConnectionManager_StartClosingById(pConnectionManager, id)) {
		DEBUG("FATAL ERROR : could not start the connection closing\n");
		return(FAILURE);
	}
	return(SUCCESS);
}

void debugBuf(void * buf, int len) {
#ifdef DEBUG_ON
	for (int i = 0; i<len; i++) {
		printf("%x ", ((unsigned char *)buf)[i]);

	}
	printf("\n");
#endif
}

void debug(pCONNECTION_MANAGER pConnectionManager, pICA_PACKET_QUEUE pIcaPacketQueue) {
#ifdef DEBUG_ON
	printf("---DEBUG--- \n");
	
	for (int i = 0; i<pConnectionManager->max_connection; i++) {
		pCONNECTION pConnection = ConnectionManager_GetConnectionByIndex(pConnectionManager, i);
		WSAPOLLFD * pFd = ConnectionManager_GetPollFdByIndex(pConnectionManager, i);
		printf("    %d Co: id=%d st=%d fd=%d p=%d a=%d w=%d| fd=%d ev=%d rv=%d\n", i, pConnection->id, pConnection->state, pConnection->socket_fd, ntohs(pConnection->server.sin_port), ntohl(pConnection->server.sin_addr.S_un.S_addr), pConnection->window,pFd->fd, pFd->events, pFd->revents);

	}


	for (int i = 0; i<pConnectionManager->max_id + 1; i++) {
		printf("    mapping : id %d, index %d \n", i, pConnectionManager->id_mapping[i]);

	}
	printf("    max_connections : %d \n", pConnectionManager->max_connection);
	printf("    max_id : %d \n", pConnectionManager->max_id);
	printf("    nCo : %d\n", pConnectionManager->n_connection);
	printf("    n_closing_connection : %d\n", pConnectionManager->n_closing_connection);
	printf("    nfds : %d\n", g_pSelectArgs->nfds);
	printf("    efds.fd_count : %d\n", g_pSelectArgs->efds.fd_count);
	for (int i = 0; i <  g_pSelectArgs->efds.fd_count; i++) {
		printf("    %d : fd=%d\n", i, g_pSelectArgs->efds.fd_array[i]);
	}
	if(pIcaPacketQueue){
		printf("    ica queue: %d\n", pIcaPacketQueue->size);
	}
	

	printf("---END--- \n");
#endif
}


