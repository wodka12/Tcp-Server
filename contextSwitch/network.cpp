#include "stdafx.h"
#include "network.h"
#include "ClientManager.h"
#include "TcpSocket.h"

extern u_short g_port;
TcpSocket* pTcpSocket = new TcpSocket;

void network::initialize_network()
{
	WSADATA wsaData;
	sockaddr_in addr;
	//HANDLE sem_handle;

#if 1
	pTcpSocket->Init();
#endif
#if 0
	/**************************/
	/* CreateIoCompletionPort */
	/**************************/
	g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == g_hIOCP) {
		printf("CreateIoCompletionPort failed: %d\n", GetLastError());
		return;
	}
#endif

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return;
	}

	if ((listen_fd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		return;
	}
	//sem_handle = CreateSemaphore(NULL, 1, 1000, (LPCWSTR)"Test");
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(g_port);

	if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
		printf("[%d] bind Error\n", __LINE__);
		return;
	}
	if (listen(listen_fd, 5) == SOCKET_ERROR) {
		printf("[%d] listen Error\n", __LINE__);
		return;
	}

	// by lsy 16.05.31 connect to lobby server - test
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("socket() error...");
		exit(1);
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(atoi("3001"));

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("Connect() error...");
		exit(1);
	}

#if 1
	char* buff = "123";
	//test
	/*if (send(sock,
		(char*)&buff,
		sizeof(buff),
		0) == SOCKET_ERROR) {
		printf("Send Error\n");
	}*/
#else

	DWORD writen = 0;
	LPOVERLAPPED overlapped = NULL;


	/* return message by send socket */
	if (WSASend(sock,
		(WSABUF*)&buff,
		1,
		(DWORD *)&writen,
		0,
		overlapped,
		NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			printf("WSASend Error.. [%d] \n", WSAGetLastError());
		}
	}
#endif
}

