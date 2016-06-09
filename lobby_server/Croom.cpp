#include "stdafx.h"
#include "Croom.h"
#include "Act.h"
#include "ClientManager.h"

#define MAX_ROOM 100

extern ClientManager* pClientManager;

Croom::Croom()
{
}


Croom::~Croom()
{
}

/*****************************************/
/* init room 0:failed 1:success          */
/* by lsy 16.05.24                       */
/*****************************************/
int Croom::init_room(int idx)
{
	int ret = 0;
	idx_room = idx;

	return ret;
}

/*****************************************/
/* calculating join room idx             */
/* by lsy 16.05.20                       */
/*****************************************/
int Croom::calc_join_room_idx(void)
{
	int ret_room = 0;
	/*
	if (pos_x > CENTER_POS && pos_y > CENTER_POS) {
		ret_zone = 1;
	}
	else if (pos_x > CENTER_POS && pos_y <= CENTER_POS) {
		ret_zone = 2;
	}
	else if (pos_x <= CENTER_POS && pos_y <= CENTER_POS) {
		ret_zone = 3;
	}
	else if (pos_x <= CENTER_POS && pos_y > CENTER_POS) {
		ret_zone = 4;
	}
	*/

	return ret_room;
}

/*****************************************/
/* user join to room                     */
/* by lsy 16.05.23                       */
/*****************************************/
int Croom::user_join_room(int fd)
{
	int ret = 0;

	ObjectUser* pObjUser = pClientManager->FindUser(fd);

	if (vec_user_room.size() < MAX_ROOM_USER_CNT)
	{
		vec_user_room.push_back(pObjUser);
	}

	/* 방번호를 유저구조체에 저장 */
	pObjUser->sUser_info.room_num = idx_room;

	return ret;
}

/*****************************************/
/* user exit room                        */
/* by lsy 16.05.23                       */
/*****************************************/
int Croom::user_exit_room(int fd)
{
	int ret = 0;

	ObjectUser* pObjUser = pClientManager->FindUser(fd);

	if (vec_user_room.size() > 0)
	{
		for (std::vector<ObjectUser*>::iterator iter = vec_user_room.begin(); iter != vec_user_room.end(); )
		{
			if ((*iter)->sUser_info.id == fd)
			{
				/*다행히 erase 함수는 지울요소의 바로다음 반복자를 리턴하므로
				 이 반복자를 받아서 사용하자.*/
				iter = vec_user_room.erase(iter);
			}
			else 
			{
				iter++;
			}
		}
	}

	return ret;
}

/*****************************************/
/* counting user in room                 */
/* by lsy 16.05.23                       */
/*****************************************/
int Croom::get_user_cnt_in_room()
{
	int ret_cnt = 0;

	ret_cnt = vec_user_room.size();

	return ret_cnt;
}

/*****************************************/
/* counting user in room                 */
/* by lsy 16.05.24                       */
/*****************************************/
int Croom::user_broadcast_room_join(int fd, SOCKETINFO* p_socket_info, CStreamSP* pStreamSP)
{
	int ret = 0;
	char *test_msg = "broadcast";

	/**********************/
	/* packet return      */
	/**********************/
	CStream* p_wStream = *pStreamSP;
	BYTE send_packet[24];
	USHORT p_Head = P_CLIENT_TO_SERVER_MSG;
	USHORT p_s_Head = P_S_CLIENT_ENTER_GAME_ACK;

	/****************/
	/* Open Stream  */
	/****************/
	memset(send_packet, 0, sizeof(send_packet));
	p_wStream->StartWrite(send_packet);
	p_wStream->WriteData(&p_Head);
	p_wStream->WriteData(&p_s_Head);

	int temp_userid[4] = { 12,34,56,78 };

	p_wStream->WriteData(&temp_userid[0]);
	p_wStream->WriteData(&temp_userid[1]);
	p_wStream->WriteData(&temp_userid[2]);
	p_wStream->WriteData(&temp_userid[3]);
	/****************/
	/* Close Stream */
	/****************/
	p_wStream->EndWrite();
	memcpy(p_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

	DWORD writen = 0;
#if 0
	/* return message by send socket */
	if (WSASend(fd,
		(WSABUF*)&p_socket_info->dataBuf,
		1,
		(DWORD *)&writen,
		0,
		&p_socket_info->overlapped,
		NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			printf("WSASend Error.. [%d] \n", WSAGetLastError());
		}
	}
#endif

	ObjectUser* pObjUser = pClientManager->FindUser(fd);


	if (vec_user_room.size() > 0)
	{
		for (std::vector<ObjectUser*>::iterator iter = vec_user_room.begin(); iter != vec_user_room.end(); )
		{
			if ((*iter)->user_socket_info->fd != fd) //현재fd도 asyncsend처리시 브로드캐스트가 끝나고 write-iooperation가 변경되는 문제가있음
			{
				//client context를 wr로 변경
				(*iter)->user_socket_info->IOOperation = ClientIoWrite;

				memcpy((*iter)->user_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

				writen = 0;

				//16.06.01
				if (WSASend(
					(*iter)->user_socket_info->fd,
					(WSABUF*)&(*iter)->user_socket_info->dataBuf,
					1,
					(DWORD *)&writen,
					0,
					&(*iter)->user_socket_info->overlapped,
					NULL) == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING) {
						printf("WSASend Error.. [%d] \n", WSAGetLastError());
					}
				}
				else {
					printf("WSASend to [%d]\n", (*iter)->user_socket_info->fd);
				}
#if 0 //backup
				(*iter)->notify((char*)send_packet);
#endif
			}
			iter++;
		}
	}

	return ret;
}

//2016.06.07 by lsy
/*****************************************/
/* counting user in room                 */
/* by lsy 16.06.07                       */
/*****************************************/
int Croom::user_broadcast_room_exit(int fd, SOCKETINFO* p_socket_info, CStreamSP* pStreamSP)
{
	int ret = 0;
	char *test_msg = "broadcast";

	/**********************/
	/* packet return      */
	/**********************/
	CStream* p_wStream = *pStreamSP;
	BYTE send_packet[24];
	USHORT p_Head = P_CLIENT_TO_SERVER_MSG;
	USHORT p_s_Head = P_S_CLIENT_EXIT_GAME_ACK;

	/****************/
	/* Open Stream  */
	/****************/
	memset(send_packet, 0, sizeof(send_packet));
	p_wStream->StartWrite(send_packet);
	p_wStream->WriteData(&p_Head);
	p_wStream->WriteData(&p_s_Head);

	int temp_userid[4] = { 12,34,56,78 };

	p_wStream->WriteData(&temp_userid[0]);
	p_wStream->WriteData(&temp_userid[1]);
	p_wStream->WriteData(&temp_userid[2]);
	p_wStream->WriteData(&temp_userid[3]);
	/****************/
	/* Close Stream */
	/****************/
	p_wStream->EndWrite();
	memcpy(p_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

	DWORD writen = 0;
#if 0
	/* return message by send socket */
	if (WSASend(fd,
		(WSABUF*)&p_socket_info->dataBuf,
		1,
		(DWORD *)&writen,
		0,
		&p_socket_info->overlapped,
		NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			printf("WSASend Error.. [%d] \n", WSAGetLastError());
		}
	}
#endif

	ObjectUser* pObjUser = pClientManager->FindUser(fd);


	if (vec_user_room.size() > 0)
	{
		for (std::vector<ObjectUser*>::iterator iter = vec_user_room.begin(); iter != vec_user_room.end(); )
		{
			if ((*iter)->user_socket_info->fd != fd) //현재fd도 asyncsend처리시 브로드캐스트가 끝나고 write-iooperation가 변경되는 문제가있음
			{
				//client context를 wr로 변경
				(*iter)->user_socket_info->IOOperation = ClientIoWrite;

				memcpy((*iter)->user_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

				writen = 0;

				//16.06.01
				if (WSASend(
					(*iter)->user_socket_info->fd,
					(WSABUF*)&(*iter)->user_socket_info->dataBuf,
					1,
					(DWORD *)&writen,
					0,
					&(*iter)->user_socket_info->overlapped,
					NULL) == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING) {
						printf("WSASend Error.. [%d] \n", WSAGetLastError());
					}
				}
				else {
					printf("WSASend to [%d]\n", (*iter)->user_socket_info->fd);
				}
#if 0 //backup
				(*iter)->notify((char*)send_packet);
#endif
			}
			iter++;
		}
	}


	return ret;
}

#if 0 //backup
int Croom::user_broadcast_room(int fd)
{
	int ret = 0;
	char *test_msg = "broadcast";

	ObjectUser* pObjUser = pClientManager->FindUser(fd);

	if (vec_user_room.size() > 0)
	{
		for (std::vector<ObjectUser*>::iterator iter = vec_user_room.begin(); iter != vec_user_room.end(); )
		{
			if ((*iter)->sUser_info.id != fd)
			{
				//삭제무한루프 
				iter = vec_user_room.erase(iter);

				(*iter)->notify(test_msg);
			}
			else
			{
				iter++;
			}
		}
	}

	return ret;
}
#endif


//2016.06.09 by lsy
/*************************************************************/
/* broadcasting selected card to other user in sameroom      */
/* by lsy 16.06.09                                           */
/*************************************************************/
int Croom::user_broadcast_selected_card(ObjectUser* info, SOCKETINFO* p_socket_info, CStreamSP* pStreamSP)
{
	int ret = 0;
	char *test_msg = "broadcast";

	/**********************/
	/* packet return      */
	/**********************/
	CStream* p_wStream = *pStreamSP;
	BYTE send_packet[24];
	USHORT p_Head = P_BATTLE_MSG;
	USHORT p_s_Head = P_S_SELECT_CARD_START;

	/****************/
	/* Open Stream  */
	/****************/
	memset(send_packet, 0, sizeof(send_packet));
	p_wStream->StartWrite(send_packet);
	p_wStream->WriteData(&p_Head);
	p_wStream->WriteData(&p_s_Head);
	p_wStream->WriteData(&info->sUser_info.select_card);

	/****************/
	/* Close Stream */
	/****************/
	p_wStream->EndWrite();
	memcpy(p_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

	DWORD writen = 0;

	ObjectUser* pObjUser = pClientManager->FindUser(info->sUser_info.id);


	/****************/
	/* broadcast    */
	/****************/
	if (vec_user_room.size() > 0)
	{
		for (std::vector<ObjectUser*>::iterator iter = vec_user_room.begin(); iter != vec_user_room.end(); )
		{
			if ((*iter)->user_socket_info->fd != info->sUser_info.id) //현재fd도 asyncsend처리시 브로드캐스트가 끝나고 write-iooperation가 변경되는 문제가있음
			{
				//client context를 wr로 변경
				(*iter)->user_socket_info->IOOperation = ClientIoWrite;

				memcpy((*iter)->user_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

				writen = 0;

				if (WSASend(
					(*iter)->user_socket_info->fd,
					(WSABUF*)&(*iter)->user_socket_info->dataBuf,
					1,
					(DWORD *)&writen,
					0,
					&(*iter)->user_socket_info->overlapped,
					NULL) == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING) {
						printf("WSASend Error.. [%d] \n", WSAGetLastError());
					}
				}
				else {
					printf("WSASend to [%d]\n", (*iter)->user_socket_info->fd);
				}
			}
			iter++;
		}
	}


	return ret;
}