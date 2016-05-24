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
