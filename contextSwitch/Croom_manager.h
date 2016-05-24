#pragma once

#include "Croom.h"

#define MAX_ROOM_CNT 512

class Croom_manager : Croom
{
public:
	unordered_map<int, Croom*> map_room;

public:
	Croom_manager();
	~Croom_manager();

	/* 2016.05.23*/
	int scan_empty_room(void);
	/* 2016.05.23*/

	bool create_room(int user_fd);
	void delete_room(void);

	int allotate_room(int room_num, int user_fd);
	int allotate_host(void);

	/* 2016.05.24 */
	int exit_room(int room_num, int user_fd);
	int broadcast_room(int room_num, int user_fd);
	/* 2016.05.24 */
};
