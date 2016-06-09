#include "stdafx.h"
#include "Act.h"
#include "network.h"
#include "ClientManager.h"
#include "battle_mgr.h"

extern ClientManager* pClientManager;

unsigned int WINAPI Proactor::ThreadProc(LPVOID lpParam)
{
	SOCKETINFO *sInfo;
	int flags;
	int readn;
	Proactor *p_Pa = (Proactor*)lpParam;
	network *p_Net;

	/* initialize networking */
	p_Net = Singleton<network>::getInstancePtr();

	while (1) {
		if ((p_Net->client_fd = WSAAccept(p_Net->listen_fd, NULL, NULL, NULL, 0)) == INVALID_SOCKET) {
			printf("Accept Error\n");
			return 1;
		}
		printf(" Accepted client fd[%d] ", p_Net->client_fd);

		sInfo = pClientManager->GetEmptySocketinfo();
		pClientManager->PushUser(p_Net->client_fd);


		sInfo->fd = p_Net->client_fd;
		sInfo->dataBuf.len = MAX_BUFF;
		sInfo->dataBuf.buf = sInfo->buf;
		sInfo->IOOperation = ClientIoRead;

		/**************************/
		/* CreateIoCompletionPort */
		/**************************/
		HANDLE check = CreateIoCompletionPort((HANDLE)p_Net->client_fd, p_Pa->g_hIOCP, (DWORD)sInfo, 0);

		flags = 0;

		if (WSARecv(sInfo->fd, &sInfo->dataBuf, 1, (DWORD *)&readn, (DWORD *)&flags, &(sInfo->overlapped), NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				printf("[%d] wsarecv error %d\n", __LINE__, WSAGetLastError());
			}
		}

		// 최초 반환패킷은 수신패킷 전용 스레드에서 처리하기로

		
		ObjectUser* pObjUser = pClientManager->FindUser(p_Net->client_fd);
	
		//16.06.01 by lsy ClientContext 유저구조체에 저장
		pObjUser->user_socket_info = sInfo;

#if 1
		
		if (send(sInfo->fd,
			(char*)&pObjUser->sUser_info,
			sizeof(pObjUser->sUser_info),
			0) == SOCKET_ERROR){
			printf("Send Error\n");
		}

		
		//pClientManager->broadcast_success_login(sInfo->fd);
		
#else
		//ObjectUser* pObjUser = pClientManager->FindUser(p_Net->client_fd);
		DWORD writen = 0;
		// return message by send socket
		if (WSASend(sInfo->fd,
			(WSABUF*)&sInfo->dataBuf,
			1,
			(DWORD *)&writen,
			0,
			&sInfo->overlapped,
			NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				printf("WSASend Error.. [%d] \n", WSAGetLastError());
			}
		}

#endif
		
	}
	return 1;
}

unsigned int WINAPI Proactor::WorkerThread(LPVOID lpParam)
{
	BOOL bSuccess = TRUE;
	SOCKETINFO* pOverlappedEx;
	DWORD               dwIoSize = 0;
	LPOVERLAPPED	 pOverlap = NULL;
#if 0
	int writen;
#endif
	int readn;
	int flags;
	Proactor *p_Pa = (Proactor*)lpParam;
	network *p_Net;
	SOCKETINFO *sInfo;

//16.05.31
	PPER_SOCKET_CONTEXT lpPerSocketContext = NULL;
	PPER_IO_CONTEXT     lpIOContext = NULL;
//16.05.31

	/* initialize networking */
	p_Net = Singleton<network>::getInstancePtr();


	while (p_Pa->bWorkerRun) {
		/*****************************/
		/* GetQueuedCompletionStatus */
		/*****************************/
		//bSuccess = GetQueuedCompletionStatus(p_Pa->g_hIOCP, &dwIoSize, (LPDWORD)&sInfo, &pOverlap, INFINITE); //win32
		bSuccess = GetQueuedCompletionStatus(p_Pa->g_hIOCP, &dwIoSize, (PULONG_PTR)&sInfo, &pOverlap, INFINITE); //X64

		if (bSuccess == FALSE || dwIoSize == 0) {
			printf("Disconnected..[fd:%d]\n", sInfo->fd);
			shutdown(sInfo->fd, SD_BOTH);
			closesocket(sInfo->fd);

			pClientManager->send_client_closed(sInfo->fd);

			sInfo->fd = INVALID_SOCKET;
			continue;
		}

		if (bSuccess == TRUE && dwIoSize == 0 && pOverlap == NULL) {
			p_Pa->bWorkerRun = FALSE;
			continue;
		}

		pOverlappedEx = (SOCKETINFO*)pOverlap;

		if (sInfo->readn == 0) {
			sInfo->readn = dwIoSize;
			sInfo->writen = 0;
			//printf("Client[%d] size[%d]: %s", sInfo->fd, dwIoSize, pOverlappedEx->dataBuf.buf);
			//printf("Exec Thread Num [%d] \n", p->cur_thread_num);
			printf("Exec fd [%d] \n", sInfo->fd);
		}
		else {
			sInfo->writen += dwIoSize;
		}

		//by lsy 16.05.31
		lpIOContext = (PPER_IO_CONTEXT)pOverlap;
		//by lsy 16.05.31

		//switch(lpIOContext->IOOperation)
		switch (sInfo->IOOperation)
		{
		case ClientIoRead:
			sInfo->IOOperation = ClientIoWrite;
			if (sInfo->readn > sInfo->writen) {
				memset(&(sInfo->overlapped), 0x00, sizeof(WSAOVERLAPPED));
				sInfo->fd = pOverlappedEx->fd;
				sInfo->dataBuf.buf = pOverlappedEx->dataBuf.buf;
				sInfo->dataBuf.len = dwIoSize;

#if 1
				//ObjectUser::User* pOvluser = (ObjectUser::User*)pOverlappedEx->dataBuf.buf;
				//pOvluser->id = sInfo->fd;

				BYTE test_byte[30];
				memcpy(test_byte, pOverlappedEx->dataBuf.buf, dwIoSize);

				pClientManager->Recv_Client_Packet(sInfo, test_byte);
#endif

#if 0
				/* return message by send socket */
				if (WSASend(sInfo->fd, 
					&(sInfo->dataBuf), 
					1, 
					(DWORD *)&writen, 
					0, 
					&(sInfo->overlapped), 
					NULL) == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING) {
						printf("WSASend Error\n");
					}
				}
#endif
			}
			break;
		case ClientIoWrite:
			flags = 0;
			memset(&(sInfo->overlapped), 0x00, sizeof(WSAOVERLAPPED));
			memset(&(sInfo->buf), 0, MAX_BUFF);
			sInfo->readn = 0;
			sInfo->fd = pOverlappedEx->fd;
			sInfo->dataBuf.len = MAX_BUFF;
			sInfo->dataBuf.buf = pOverlappedEx->dataBuf.buf;
			sInfo->IOOperation = ClientIoRead;

#if 1
			if (WSARecv(sInfo->fd, &sInfo->dataBuf, 1, (DWORD *)&readn, (DWORD *)&flags, &(sInfo->overlapped), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != WSA_IO_PENDING) {
					printf("wsarecv error %d\n", WSAGetLastError());
				}
			}
#endif
			break;
		default:
			break;

		}
	}
	return 1;
}


/* 2016.05.24 */
unsigned int WINAPI Proactor::SignalProc(LPVOID lpParam)
{
	static int run_dly = 0;

	while (1) {
		/* by lsy 16.06.09
		배틀룸전체 스케쥴러는 이곳에서 전부 갱신한다.
		방추가삭제가 이뤄지면서 업데이트시 메모리검색식을돌리면
		뻑날 소지가 있으므로 이후에 뮤텍스로 씌우던지 싱글오브젝트방식을 도입하자.
		주의할점은 클라이언트매니져에있는 자원을 여기서 갱신해버리면 무거워질수있따.
		네트워크처리나 게임오브젝트들은 전부 워커에서 관리한다.
		시그널스레드는 잠재적으로 스케쥴러 갱신용도로만 쓴다. */
		BATTLE_MGR.update_game();
		Sleep(50);
	}
	return 1;
}
/* 2016.05.24 */
void Proactor::begin_thread_proc()
{
	hThread[cnt_thread] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (LPVOID)this, 0, (unsigned int*)&dwThreadID[0]);
	ResumeThread(hThread[0]);
}
void Proactor::begin_thread_worker()
{
	DWORD result = 0;
	for (int i = 0; i < max_thread; i++) {
		cur_thread_num = i;
		hWorkerThread[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (LPVOID)this, CREATE_SUSPENDED, &dwWorkerThreadID[i]);

		if (hWorkerThread[i] == NULL)
		{
			printf(" thread create error\n");
			return;
		}

		ResumeThread(hWorkerThread[i]);
	}

	//result = WaitForMultipleObjects(MAX_TH,hThread,TRUE,INFINITE);
	result = WaitForSingleObject(hWorkerThread[0], INFINITE);
	if (result == WAIT_FAILED) {
		puts("thread error");
		return;
	}
}


void Proactor::begin_thread_Signal()
{
	signalThread[0] = (HANDLE)_beginthreadex(NULL, 0, SignalProc, (LPVOID)this, 0, (unsigned int*)&dwThreadID[1]);
	ResumeThread(signalThread[0]);
}