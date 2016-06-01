#include "stdafx.h"
#include "Croom_manager.h"
#include "Mysql.h"
#include "IDGenerator.h"

extern CMysql* p_mysql;

/* by lsy 2016.05.31 token generator */
CIDGenerator<DWORD> g_TokenIDGenerator(1, 0xFFFFFFFF);

Croom_manager::Croom_manager()
{
}


Croom_manager::~Croom_manager()
{
}

/*****************************************/
/* seeing the empty room                 */
/* by lsy 16.05.24                       */
/*****************************************/
int Croom_manager::scan_empty_room(void)
{
	int ret_room_idx = 0;

	Croom* p_room;
	unordered_map<int, Croom*>::iterator it_room = map_room.begin();
	while (it_room != map_room.end())
	{
		p_room = it_room->second;
		if (p_room->get_user_cnt_in_room() < 4)
		{
			ret_room_idx = p_room->idx_room;
			break;
		}
		it_room++;

	}

	return ret_room_idx;
}

/*****************************************/
/* room manger create to room            */
/* by lsy 16.05.24                       */
/*****************************************/
bool Croom_manager::create_room(int user_fd)
{
	bool ret = false;
	
	Croom* p_room = new Croom;

	/*temp*/static int temp_room_idx = 0;

	int empty_room_num = scan_empty_room();

	if ( empty_room_num == 0)
	{
		/*temp*/temp_room_idx++;

		p_room->init_room(temp_room_idx);

		p_room->user_join_room(user_fd);

		unordered_map<int, Croom*>::value_type value(temp_room_idx, p_room);
		map_room.insert(value);

		ret = true;

		printf("...create room [room number = %d]\n ", temp_room_idx);
	}

	return ret;
}

/*****************************************/
/* notyet                                */
/*****************************************/
int find_room_idx(void)
{
	int ret_idx = 0;


	return ret_idx;
}

/*****************************************/
/* notyet                                */
/*****************************************/
void Croom_manager::delete_room(void)
{
	int room_idx = find_room_idx();

	unordered_map<int, Croom*>::iterator it_room;

	it_room = map_room.find(room_idx);

	map_room.erase(it_room);
}

/*****************************************/
/* room manger allotate to room          */
/* by lsy 16.05.24                       */
/*****************************************/
int Croom_manager::allotate_room(int room_num, int user_fd)
{
	int ret = 0;

	unordered_map<int, Croom*>::iterator it_room;

	it_room = map_room.find(room_num);
	Croom* p_room = it_room->second;

	p_room->user_join_room(user_fd);

	printf("... [%d] user enter [%d] room \n", user_fd, room_num);
	
	return ret;
}

/*****************************************/
/* notyet                                */
/*****************************************/
int Croom_manager::allotate_host(void)
{
	int ret_host_idx = 0;

	return ret_host_idx;
}

/*****************************************/
/* user exit to room                     */
/* by lsy 16.05.24                       */
/*****************************************/
int Croom_manager::exit_room(int room_num, int user_fd)
{
	int ret = 0;

	unordered_map<int, Croom*>::iterator it_room;

	it_room = map_room.find(room_num);
	Croom* p_room = it_room->second;

	p_room->user_exit_room(user_fd);

	printf("... [%d] user exit [%d] room \n", user_fd, room_num);

	return ret;
}

/*****************************************/
/* user broadcast to other user in room  */
/* by lsy 16.05.24                       */
/*****************************************/
int Croom_manager::broadcast_room(int room_num, int user_fd, SOCKETINFO* p_socket_info, CStreamSP* pStreamSP)
{
	int ret = 0;

	unordered_map<int, Croom*>::iterator it_room;

	it_room = map_room.find(room_num);
	Croom* p_room = it_room->second;

	p_room->user_broadcast_room(user_fd, p_socket_info, pStreamSP);

	printf("...broadcast [%d] room \n",  room_num);

	return ret;
}
/* 2016.05.24 */

/*****************************************/
/* request enter game server             */
/* by lsy 16.05.31                       */
/*****************************************/
int Croom_manager::enter_game_req(int user_fd, SOCKETINFO* p_socket_info, CStreamSP* pStreamSP)
{
	int ret = 0;

	/**********************/
	/* packet return      */
	/**********************/
#if 1
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
	p_wStream->WriteData(&user_fd);
	/****************/
	/* Close Stream */
	/****************/
	p_wStream->EndWrite();
	memcpy(p_socket_info->dataBuf.buf, send_packet, sizeof(send_packet));

	DWORD writen = 0;

	/* return message by send socket */
	if (WSASend(user_fd,
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

	return ret;
}

/*****************************************/
/* get user number in room               */
/* by lsy 16.05.31                       */
/*****************************************/
int Croom_manager::get_user_cnt_in_room(int room_num)
{
	int ret_cnt = 0;

	Croom* p_room;
	unordered_map<int, Croom*>::iterator it_room = map_room.begin();
	while (it_room != map_room.end())
	{
		p_room = it_room->second;
		if (p_room->idx_room == room_num)
		{
			ret_cnt = p_room->get_user_cnt_in_room();
			break;
		}
		it_room++;
	}

	return ret_cnt;
}