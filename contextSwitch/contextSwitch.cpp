#include "stdafx.h"
#include "network.h"
#include "ClientManager.h"
#include "ObjManager.h"
#include "TcpSocket.h"
#include "Mysql.h"

u_short g_port;
network* Singleton<network>::pInstance = NULL;

ClientManager* pClientManager = new ClientManager;
ObjManager* pObjManager = new ObjManager;
TcpSocket* pTcpSock = new TcpSocket;

/* 2016.05.24 */
CMysql* p_mysql = new CMysql;

int main(int argc, char **argv)
{
	SYSTEM_INFO systemInfo;

	if (argc != 2) {
		printf("Usage : %s [port number]\n", argv[0]);
		return 1;
	}
	g_port = atoi(argv[1]);

	/* initialize mysql module */
	//p_mysql->initialize_mysql();

	/* get system information */
	GetSystemInfo(&systemInfo);

	/* set thread counts */
	pTcpSock->Proactor_->Init(systemInfo.dwNumberOfProcessors);

	/* Initialize Client Manager */
	pClientManager->InitClientManager();

	/* initialize networking */
	Singleton<network>::getInstancePtr()->initialize_network();

	/* begin threads */
	pTcpSock->Proactor_->begin_threads();

	/* release to network instance */
	Singleton<network>::releaseInstance();


	return 0;
}


