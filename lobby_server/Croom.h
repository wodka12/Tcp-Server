#pragma once
#include "Object.h"

#define MAX_ROOM_USER_CNT 4

class Croom
{

public:
	vector<ObjectUser*> vec_user_room;

public:
	/* ‚Æ‚è‚ ‚¦‚¸ */
	int idx_room;

	Croom();
	~Croom();

	/* init room manager */
	int init_room(int idx);

	/* calculating join room idx */
	int calc_join_room_idx(void);

	/* join,exit user */
	int user_join_room(int fd);
	int user_exit_room(int fd);

	/* get user count in room*/
	int get_user_cnt_in_room(void);

	/* broadcast room join*/
	int user_broadcast_room_join(int fd, SOCKETINFO* p_socket_info, CStreamSP* pStreamSP);

	/* broadcast room exit */
	int user_broadcast_room_exit(int fd, SOCKETINFO* p_socket_info, CStreamSP* pStreamSP);
};

