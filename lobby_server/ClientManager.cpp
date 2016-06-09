#include "stdafx.h"
#include "ClientManager.h"
#include "Croom_manager.h"
#include "Mysql.h" //16.06.07
#include "battle_mgr.h" //16.06.09


/* 2016.05.23*/
Croom_manager* pRoommanager;
/* 2016.05.23*/

void ClientManager::InitClientManager()
{
#if 1
	memset(ClientInfo, 0, sizeof(ClientInfo));
	for (DWORD i = 0; i < MAX_CLIENT; i++) {
		ClientInfo[i].socket_info.fd = INVALID_SOCKET;
	}
#else
	memset(ClientInfo, 0, sizeof(ClientInfo));
	for (DWORD i = 0; i < MAX_CLIENT; i++) {
		ClientInfo[i].socket_info.fd = INVALID_SOCKET;
		vec.push_back(ClientInfo[i]);
	}
#endif
}

void ClientManager::AddClientCnt()
{
	if (client_cnt < MAX_CLIENT) {
		client_cnt++;
	}
}

void ClientManager::SubtractClientCnt()
{
	if (client_cnt > 0) {
		client_cnt--;
	}
}

SOCKETINFO* ClientManager::GetEmptySocketinfo(void)
{
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (ClientInfo[i].socket_info.fd == INVALID_SOCKET) {
			printf("Client Number[%d]\n", i);
			return &ClientInfo[i].socket_info;
		}
	}
	return FALSE;
}

HANDLE        m_hMutex;
#define SHARE_MUTEX "Share_Mutex"


#pragma warning(disable : 4996)
void ClientManager::PushUser(int fd)
{
	int ClientCnt = GetClientCnt();
	UserInfo[ClientCnt] = new ObjectUser;
	ObjectUser* user = UserInfo[ClientCnt];
#ifdef _AI_TEST_
	user->Set_Id(fd);
	srand((unsigned int)timeGetTime());
	user->SetPos_X(rand() % AI_MAX_POS_X);
	user->SetPos_Y(rand() % AI_MAX_POS_Y);
	user->Set_Msg(" !  ");
	user->Set_Zone(user->myWolrd.calc_zone(user->sUser_info.pos_x, user->sUser_info.pos_y));
	/* set state */
	user->sUser_info.state.hp = MAX_HP_USER;
	user->sUser_info.state.mp = MAX_MP_USER;
	user->sUser_info.state.damage = DEF_USER_DMG;
	user->sUser_info.state.shield = DEF_USER_SHD;
	user->sUser_info.state.level = DEF_USER_LEVEL;
	user->sUser_info.state.experience = 0;

	user->is_connected = TRUE;
	vec.push_back(user);

	unordered_map<int, ObjectUser*>::value_type value(fd, user);
	map_user.insert(value);
	map_user[fd] = user;
#if 0
	hash_map<int, User>::iterator it_user;
	map_user.find(fd);
	if (it_user != map_user.end())
	{
		std::cout << std::endl;
		std::cout << it_user->first << " " << it_user->second.zone << " " << std::endl;
	}
#endif
#endif
	AddClientCnt();
}

/* 2016.05.20 channel control */
void ClientManager::room_control(int fd)
{

}


void ClientManager::SubstractUser(int fd)
{
#if 0
	/* Mutex Open */
	m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, (LPCWSTR)SHARE_MUTEX);
	if (m_hMutex == NULL)
	{
		printf("Mutex Handle is NULL.. \n");
		m_hMutex = CreateMutex(NULL, FALSE, (LPCWSTR)SHARE_MUTEX);
	}
	WaitForSingleObject(m_hMutex, INFINITE);
#endif
	/* Mutex Open */
	unordered_map<int, ObjectUser*>::iterator it_user;

	it_user = map_user.find(fd);
	if (it_user != map_user.end()) {
		map_user.erase(it_user);
		delete UserInfo[fd];
		SubtractClientCnt();
	};

#if 0
	/* Mutex Close */
	ReleaseMutex(m_hMutex);
	//CloseHandle(m_hMutex);

	m_hMutex = NULL;
	/* Mutex Close */
#endif
}

void ClientManager::send_client_closed(int fd)
{
	/* Mutex Open */
	ObjectUser* pUser;
	unordered_map<int, ObjectUser*>::iterator it_user;

	it_user = map_user.find(fd);
	if (it_user != map_user.end())
	{
		pUser = it_user->second;
		pUser->is_connected = FALSE;
		b_client_ping = false;
	}
}

void ClientManager::check_ping_and_cut_user(void)
{
	vector<int> vec;
	vector<int>::iterator vec_it;
	int close_client_id;

	ObjectUser* pUser;
	unordered_map<int, ObjectUser*>::iterator it_user = map_user.begin();
	while (it_user != map_user.end())
	{
		pUser = it_user->second;
		if (pUser->is_connected == FALSE)
		{
			vec.push_back(pUser->sUser_info.id);
		}
		it_user++;
	}

	for (vec_it = vec.begin(); vec_it != vec.end(); vec_it++) {
		close_client_id = *vec_it;
		SubstractUser(close_client_id);
	}
}

/* confirm 2015.05.23 */
ObjectUser* ClientManager::FindUser(int fd)
{
	ObjectUser* pObjUser = NULL;
	unordered_map<int, ObjectUser*>::iterator it_user;

	if (fd > 0) {
		it_user = map_user.find(fd);
		pObjUser = it_user->second;
	}
	return pObjUser;
}

void ClientManager::UpdateUserInfo(int fd, ObjectUser::User* info)
{
	unordered_map<int, ObjectUser*>::iterator it_user;
	ObjectUser* pUser;

	it_user = map_user.find(fd);

	pUser = it_user->second;
	pUser->Set_Id(fd);
	pUser->SetPos_X(info->pos_x);
	pUser->SetPos_Y(info->pos_y);
	pUser->Set_Msg(info->msg);
	pUser->Set_Zone(pUser->myWolrd.calc_zone(info->pos_x, info->pos_y));

	info = &pUser->sUser_info;
}

void ClientManager::NotifyUserInfo()
{
	unordered_map<int, ObjectUser*>::iterator it_user = map_user.begin();
	while (it_user != map_user.end())
	{
		//updateData(*idx);
		//(*idx)->notify();
		it_user++;
	}
}

/* 2016.05.19 success to login broadcast */
void ClientManager::broadcast_success_login(int user_fd)
{
	BYTE packet_broadcasting[10];
	CStream* pStream = *pStreamSP;
	USHORT p_Head = P_BROAD_CAST_MSG;

	try {
		memset(packet_broadcasting, 0, sizeof(packet_broadcasting));
		/****************/
		/* Open Stream  */
		/****************/
		pStream->StartWrite(packet_broadcasting);

		pStream->WriteData(&p_Head);
		pStream->WriteData(&user_fd); /* accepted fd number */
		//pStream->WriteString(info->msg);

		unordered_map<int, ObjectUser*>::iterator it_user = map_user.begin();
		ObjectUser* p_User;

		while (it_user != map_user.end())
		{
			p_User = it_user->second;
			//if ((p_User != NULL) && (p_User->sUser_info.id != user_fd))
			if(p_User != NULL)
			{
				p_User->notify(packet_broadcasting, sizeof(packet_broadcasting));
			}
			it_user++;
		}

		/****************/
		/* Close Stream */
		/****************/
		pStream->EndWrite();
	}
	catch (DWORD dwError)
	{
		printf("Error code : %d", dwError);
	}
}
/* 2016.05.19 end */


/* 2016.05.23 */
void ClientManager::recv_packet_user_join_room(ObjectUser* info, CStreamSP* pStreamSP)
{
	bool b_success = false;
	int room_num = 0;
	int user_cnt_room = 0;

	if (pRoommanager == NULL)
	{
		pRoommanager = new Croom_manager;
	}

	b_success = pRoommanager->create_room(info->sUser_info.id, &room_num);

	if (b_success == false)
	{
		room_num = pRoommanager->scan_empty_room();
		pRoommanager->allotate_room(room_num, info->sUser_info.id);

		/*
		//16.06.09 호스트로부터 스타트 신호가오면 실행하도록 배틀패킷쪽으로 이동
		//by lsy 16.05.31 방배정직후 인원수가 꽉차면 룸 멤버에게 브로드캐스팅
		user_cnt_room = pRoommanager->get_user_cnt_in_room(room_num);
		if (user_cnt_room == MAX_ROOM_USER_CNT)
		{
			pRoommanager->broadcast_room(room_num, user_fd, p_socket_info, pStreamSP, 0);
		}
		*/
	}

	//마지막입장유저에게 유저수를 반환. 방이 꽉찬 상태만 통지. 방장에게 알리는 처리는 추후에
	user_cnt_room = pRoommanager->get_user_cnt_in_room(room_num);

	/**********************/
	/* packet return      */
	/**********************/
#if 1
	CStream* p_wStream = *pStreamSP;
	BYTE send_packet[24];
	USHORT p_Head = P_LOBBY_MSG;
	USHORT p_s_Head = P_S_JOIN_ROOM;

	/****************/
	/* Open Stream  */
	/****************/
	memset(send_packet, 0, sizeof(send_packet));
	p_wStream->StartWrite(send_packet);
	p_wStream->WriteData(&p_Head);
	p_wStream->WriteData(&p_s_Head);
	p_wStream->WriteData(&user_cnt_room);
	/****************/
	/* Close Stream */
	/****************/
	p_wStream->EndWrite();
	memcpy(pc_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

	DWORD writen = 0;
	/* return message by send socket */
	if (WSASend(info->sUser_info.id,
		(WSABUF*)&pc_socket_info->dataBuf,
		1,
		(DWORD *)&writen,
		0,
		&pc_socket_info->overlapped,
		NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			printf("WSASend Error.. [%d] \n", WSAGetLastError());
		}
	}
#endif

}
/* 2016.05.23 end */


extern CMysql* p_mysql;

/* 2016.05.24 */
void ClientManager::recv_packet_user_exit_room(ObjectUser* info, CStreamSP* pStreamSP)
{
	bool b_success = false;

	if (pRoommanager == NULL)
	{
		pRoommanager = new Croom_manager;
	}

#if 1
	//16.06.07 by lsy
	pRoommanager->exit_room(info->sUser_info.room_num, info->sUser_info.id);

	//16.06.07 by lsy RPC send/return test

	p_mysql->rpc_send("30");

	pRoommanager->broadcast_room(info, pStreamSP, 1);

	//16.06.07

#endif
	/**********************/
	/* packet return      */
	/**********************/
	CStream* p_wStream = *pStreamSP;
	BYTE send_packet[24];
	USHORT p_Head = P_LOBBY_MSG;
	USHORT p_s_Head = P_S_QUIT_ROOM;

	/****************/
	/* Open Stream  */
	/****************/
	memset(send_packet, 0, sizeof(send_packet));
	p_wStream->StartWrite(send_packet);
	p_wStream->WriteData(&p_Head);
	p_wStream->WriteData(&p_s_Head);
	/****************/
	/* Close Stream */
	/****************/
	p_wStream->EndWrite();
	memcpy(pc_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

	DWORD writen = 0;
	/* return message by send socket */
	if (WSASend(info->sUser_info.id,
		(WSABUF*)&pc_socket_info->dataBuf,
		1,
		(DWORD *)&writen,
		0,
		&pc_socket_info->overlapped,
		NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			printf("WSASend Error.. [%d] \n", WSAGetLastError());
		}
	}

}


int ClientManager::packet_lobby_proc(ObjectUser* info, SOCKETINFO* p_socket_info, CStreamSP* pStreamSP)
{
	int ret = 0;
	USHORT packet_header_second = Read_Stream_Header_second(*pStreamSP);

	switch (packet_header_second)
	{
	case P_S_JOIN_ROOM:
		recv_packet_user_join_room(info, pStreamSP);
		break;
	case P_S_QUIT_ROOM:
		recv_packet_user_exit_room(info, pStreamSP);
		break;
	case P_S_BROADCAST_ROOM:
		break;

	default:
		break;
	}

	return ret;
}
/* 2016.05.24 end */


//16.05.31
int ClientManager::packet_client_to_server_proc(ObjectUser::User* info, SOCKETINFO* p_socket_info)
{
	int ret = 0;
	USHORT packet_header_second = Read_Stream_Header_second(*pStreamSP);

	switch (packet_header_second)
	{
	case P_S_CONNECT_REQ_GAME_TO_LOBBY:
		pRoommanager->enter_game_req(info->id, p_socket_info, pStreamSP);
		break;
	case P_S_SERVER_ENTER_GAME_REQ:
		break;
	case P_S_SERVER_ENTER_GAME_ACK:
		break;
	case P_S_SERVER_EXIT_GAME_REQ:
		break;
	case P_S_SERVER_EXIT_GAME_ACK:
		break;
	default:
		break;
	}

	return ret;
}

void ClientManager::broadcast_userinfo(int own, ObjectUser::User* info)
{
	BYTE packet_broadcasting[10];
	CStream* pStream = *pStreamSP;
	USHORT p_Head = P_BROAD_CAST_MSG;

	try{
		memset(packet_broadcasting, 0, sizeof(packet_broadcasting));
		/****************/
		/* Open Stream  */
		/****************/
		pStream->StartWrite(packet_broadcasting);

		pStream->WriteData(&p_Head);
		pStream->WriteData(&own);
		pStream->WriteString(info->msg);

		unordered_map<int, ObjectUser*>::iterator it_user = map_user.begin();
		ObjectUser* p_User;

		while (it_user != map_user.end())
		{
			p_User = it_user->second;
			if ((p_User != NULL) && (p_User->sUser_info.id != own))
			{
				p_User->notify(packet_broadcasting, sizeof(packet_broadcasting));
			}
			it_user++;
		}

		/****************/
		/* Close Stream */
		/****************/
		pStream->EndWrite();
	}
	catch (DWORD dwError)
	{
		printf("Error code : %d", dwError);
	}
}

void ClientManager::broadcast_other_player(ObjectUser* p_user_self)
{
	CStream* pStream = *pStreamSP;
	BYTE send_packet[22];
	USHORT p_Head = P_SCAN_OTHER_PLAYER_ON_SAME_ZONE;

	ObjectUser* p_User;
	unordered_map<int, ObjectUser*>::iterator it_user = map_user.begin();
	while (it_user != map_user.end())
	{
		p_User = it_user->second;
		if (p_User != NULL &&
			p_User->sUser_info.id != p_user_self->sUser_info.id) /* not for self */
		{
			if (p_User->sUser_info.zone == p_user_self->sUser_info.zone) {
				/****************/
				/* Open Stream  */
				/****************/
				memset(send_packet, 0, sizeof(send_packet));
				pStream->StartWrite(send_packet);
				pStream->WriteData(&p_Head);
				pStream->WriteData(&p_user_self->sUser_info.id);
				pStream->WriteData(&p_user_self->sUser_info.zone);
				pStream->WriteData(&p_user_self->sUser_info.pos_x);
				pStream->WriteData(&p_user_self->sUser_info.pos_y);
				pStream->WriteData(&p_user_self->sUser_info.state.hp);

				//p_User->notify_from_other_player(send_packet, sizeof(send_packet));
				send(p_User->sUser_info.id, (char*)send_packet, sizeof(send_packet), 0);

				/****************/
				/* Close Stream */
				/****************/
				pStream->EndWrite();
			}
		}
		it_user++;
	}
}

void ClientManager::broadcast_userinfo_zone(int own, ObjectUser::User* info)
{
	unordered_map<int, ObjectUser*>::iterator it_user = map_user.begin();
	ObjectUser* p_User;

	while (it_user != map_user.end())
	{
		p_User = it_user->second;
		if (p_User->sUser_info.id != own &&
			p_User->sUser_info.zone == info->zone) {
			p_User->notify(info->msg);
		}
		it_user++;
	}
}

#define AI_BUFF_MAX 40
//#pragma warning(disable : 4996)
void ClientManager::send_msg(int Client_idx)
{
#if 0
	int ret;
#endif
	char buff[AI_BUFF_MAX] = "";
	int fd;
	ObjectUser::User* pUser;

	fd = ClientInfo[Client_idx].socket_info.fd;

	if (fd > 0)
	{
		pUser = &FindUser(fd)->sUser_info;
		if (pUser != NULL)
		{
			broadcast_userinfo(fd, pUser);
		}
	}
#if 0
	if (fd != INVALID_SOCKET) {
		ret = send(fd, 
			(char*)&FindUser(fd)->sUser_info, 
			sizeof(UserInfo[Client_idx]->sUser_info),
			0);

		if (ret == SOCKET_ERROR)
		{
			printf("WSA Error.. [%d]\n", WSAGetLastError());
		}
		//SendPacket(fd, out, out.);

	}
#endif
}


#ifdef _NET_STREAM_TEST_

USHORT ClientManager::Read_Stream_Header(CStream* clsStream, BYTE* src)
{
	USHORT Packet_Header = 0x0;

	clsStream->WriteBuff_for_recv(src);
	clsStream->ReadData(&Packet_Header);

	return Packet_Header;
}

USHORT ClientManager::Read_Stream_Header_second(CStream* clsStream)
{
	USHORT Packet_Header_second = 0x0;

	clsStream->ReadData(&Packet_Header_second);

	return Packet_Header_second;
}
#endif

int ClientManager::Recv_Client_Packet(SOCKETINFO* socket_info, BYTE* packet)
{
	EnterCriticalSection(&cs);
	ObjectUser *pObjectUser = FindUser(socket_info->fd);

	//유저 구조체에서 소켓정보 빼올수있는지 실험 -> 성공
	//pc_socket_info = socket_info;
	pc_socket_info = pObjectUser->user_socket_info;

	if (pObjectUser != NULL) {

		CStream* pStream = *pStreamSP;
		/****************/
		/* Open Stream  */
		/****************/
		pStream->StartRead();
		USHORT P_Header = Read_Stream_Header(*pStreamSP, packet);

		switch (P_Header)
		{
		case P_SEND_MSG:
			packet_send_msg_proc(&pObjectUser->sUser_info);
			break;
		case P_MOVE_POS:
			packet_move_proc(&pObjectUser->sUser_info);
			break;
		case P_BROAD_CAST_MSG:
			/* broad casting */
			broadcast_userinfo(pObjectUser->sUser_info.id, &pObjectUser->sUser_info);
			//pClientManager->broadcast_userinfo_zone(sInfo->fd, pOvluser);
			break;
		case P_LOBBY_MSG:
			//recv_packet_user_join_room(pObjectUser->sUser_info.id);
			packet_lobby_proc(pObjectUser, pc_socket_info, pStreamSP);
			break;
		//16.05.31
		case P_CLIENT_TO_SERVER_MSG:
			packet_client_to_server_proc(&pObjectUser->sUser_info, pc_socket_info);
			break;
		//16.06.09
		case P_BATTLE_MSG:
			recv_packet_battle_proc(pObjectUser, pStreamSP);
			break;

		case P_MAX:
			break;
		}
		/****************/
		/* Close Stream */
		/****************/
		pStream->EndRead();
	}
	/* need mutext */
	LeaveCriticalSection(&cs);
	return true;
}


//broadcast test

int ClientManager::packet_broadcasttest_proc(ObjectUser::User* p_user)
{
	ObjectUser *pObjectUser = FindUser(p_user->id);

	if (pObjectUser != NULL) {

	}

	return true;
}

int ClientManager::packet_move_proc(ObjectUser::User* p_user)
{
	ObjectUser *pObjectUser = FindUser(p_user->id);

	if (pObjectUser != NULL) {
		USHORT packet_header_second = Read_Stream_Header_second(*pStreamSP);
		/* TODO : get user position from data stream */
		CStream *pStream = *pStreamSP;
		int client_pos_x;
		int client_pos_y;
		int client_zone;

		int pos_x;
		int pos_y;

		pStream->ReadData(&client_pos_x);
		pStream->ReadData(&client_pos_y);
		pStream->ReadData(&client_zone);
		/* TODO : end */

		p_user->id = pObjectUser->sUser_info.id;
		p_user->zone = pObjectUser->sUser_info.zone;

		pos_x = pObjectUser->sUser_info.pos_x;
		pos_y = pObjectUser->sUser_info.pos_y;

		switch (packet_header_second)
		{
		case P_S_MOVE_ZONE:
			break;
		case P_S_MOVE_UP:
			if ((pos_y + 1) <= 255)
			{
				pObjectUser->sUser_info.pos_y++;
			}
			break;
		case P_S_MOVE_DOWN:
			if ((pos_y - 1) >= 0)
			{
				pObjectUser->sUser_info.pos_y--;
			}
			break;
		case P_S_MOVE_LEFT:
			if ((pos_x - 1) >= 0)
			{
				pObjectUser->sUser_info.pos_x--;
			}
			break;
		case P_S_MOVE_RIGHT:
			if ((pos_x + 1) <= 255)
			{
				pObjectUser->sUser_info.pos_x++;
			}
			break;
		case P_S_MOVE_RAND_TELEPORT:
			srand((unsigned int)timeGetTime());
			pObjectUser->sUser_info.pos_x = rand() % AI_MAX_POS_X;
			pObjectUser->sUser_info.pos_y = rand() % AI_MAX_POS_Y;

			break;
		case P_S_MOVE_NONE:
		default:
			break;

		}
#if 1
		DWORD writen = 0;

		BYTE buff[30];

		UpdateUserInfo(p_user->id, &pObjectUser->sUser_info);
		
		pStream->UpdateBuff(&pObjectUser->sUser_info.pos_x, 4, sizeof(pObjectUser->sUser_info.pos_x));
		pStream->UpdateBuff(&pObjectUser->sUser_info.pos_y, 8, sizeof(pObjectUser->sUser_info.pos_y));

		pObjectUser->Set_Zone( pObjectUser->myWolrd.calc_zone(pObjectUser->sUser_info.pos_x, pObjectUser->sUser_info.pos_y) );
		pStream->UpdateBuff(&pObjectUser->sUser_info.zone, 12, sizeof(pObjectUser->sUser_info.zone));

		pStream->CopyData(&buff);

		memcpy(pc_socket_info->dataBuf.buf, buff, sizeof(buff));

		/* return message by send socket */
		if (WSASend(p_user->id,
			(WSABUF*)&pc_socket_info->dataBuf,
			1,
			(DWORD *)&writen,
			0,
			&pc_socket_info->overlapped,
			NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				printf("WSASend Error.. [%d] \n", WSAGetLastError());
			}
		}

		//broadcast_userinfo(p_user->id, &pObjectUser->sUser_info);
		broadcast_other_player(pObjectUser);
#endif
	}

	return true;
}

int ClientManager::packet_send_msg_proc(ObjectUser::User* p_user)
{
	ObjectUser *pObjectUser = FindUser(p_user->id);

	if (pObjectUser != NULL)
	{
		p_user->id = pObjectUser->sUser_info.id;
		pObjectUser->Set_Msg("org\n");

#if 0
		DWORD writen = 0;

		/* return message by send socket */
		if (WSASend(p_user->id,
			(WSABUF*)&pc_socket_info->dataBuf,
			1,
			(DWORD *)&writen,
			0,
			&pc_socket_info->overlapped,
			NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				printf("WSASend Error.. [%d] \n", WSAGetLastError());
			}
		}
#endif
		//broadcast_userinfo(p_user->id, &pObjectUser->sUser_info);
	}
	return true;
}

//배틀 관련 수신 프로토콜
int ClientManager::recv_packet_battle_proc(ObjectUser* info, CStreamSP* pStreamSP)
{
	int ret = 0;
	int selected_card_num = 0;

	CStream *pStream = *pStreamSP;

	USHORT packet_header_second = Read_Stream_Header_second(*pStreamSP);

	switch (packet_header_second)
	{
	case P_S_BATTLE_START:
		BATTLE_MGR.begin_game(info, pStreamSP);
		break;
	case P_S_BATTLE_END:
		break;
	case P_S_GS_TO_C_START_WAVE:
		break;
	case P_S_GS_TO_C_END_WAVE:
		break;
	case P_S_SELECT_CARD_START:
		pStream->ReadData(&selected_card_num);
		BATTLE_MGR.user_active(info, pStreamSP, user_active_select_card, selected_card_num);
		break;
	case P_S_SELECT_CARD_END:
		break;
	case P_S_PLAYER_USE_CARD:
		break;
	case P_S_NOTIFY_TURN_START:
		break;
	case P_S_NOTIFY_TURN_END:
		break;
	default:
		break;
	}

	return ret;
}