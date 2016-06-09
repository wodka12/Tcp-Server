#ifndef _PACKET_DEF_
#define _PACKET_DEF_

/* 
P : header
P_S : sub header
*/

typedef enum e_Packet {
	P_SEND_MSG = 0x0,
	P_MOVE_POS = 0x01,
	P_USER_ACTIVITY = 0x02,
	P_BROAD_CAST_MSG = 0x04,
	P_SCAN_MONSTER_ON_SAME_ZONE = 0x08,
	P_SCAN_OTHER_PLAYER_ON_SAME_ZONE = 0x10,
	P_LOBBY_MSG = 0x11, /* 2016.05.23 */
	P_SERVER_RELAY_MSG = 0x12, /* 2016.05.31 */
	P_CLIENT_TO_SERVER_MSG = 0x14, /* 2016.05.31 */
	P_BATTLE_MSG = 0x18, /* 2016.06.09 */
	P_MAX			= 0xFF,
} e_packet_def;

typedef enum e_Packet_Move_Pos {
	P_S_MOVE_ZONE = 0x00,
	P_S_MOVE_UP = 0x01,
	P_S_MOVE_DOWN = 0x02,
	P_S_MOVE_LEFT = 0x04,
	P_S_MOVE_RIGHT = 0x08,
	P_S_MOVE_RAND_TELEPORT = 0x10,
	P_S_MOVE_NONE = 0xFF,
} e_packet_move_pos_def;

typedef enum e_Packet_User_Activity {
	P_S_ACTIVITY_ATTACK_TO_MONSTER = 0x00,
	P_S_ACTIVITY_SKILL = 0x01,
	P_S_ACTIVITY_ATTACK_TO_PLAYER = 0x02,
	P_S_ACTIVITY_NONE = 0xFF,
} e_packet_User_Activity_def;

typedef enum e_Packet_User_Lobby {
	P_S_JOIN_ROOM = 0x00, 
	P_S_QUIT_ROOM = 0x01,
	P_S_BROADCAST_ROOM = 0x02,
	p_S_ROOM_NONE = 0xFF,
} e_packet_lobby;

typedef enum e_Packet_Server_Relay_Msg {
	P_S_CONNECT_REQ_GAME_TO_LOBBY = 0x00,
	P_S_SERVER_ENTER_GAME_REQ = 0x01,
	P_S_SERVER_ENTER_GAME_ACK = 0x02,
	P_S_SERVER_EXIT_GAME_REQ = 0x04,
	P_S_SERVER_EXIT_GAME_ACK = 0x08,
} e_packet_server_relay_msg;


typedef enum e_Packet_Client_to_Server {
	P_S_CLIENT_ENTER_GAME_REQ = 0x00,
	P_S_CLIENT_ENTER_GAME_ACK = 0x01,
	P_S_CLIENT_EXIT_GAME_REQ = 0x02,
	P_S_CLIENT_EXIT_GAME_ACK = 0x04,
} e_packet_client_to_server;


typedef enum e_Packet_Battle {
	P_S_BATTLE_START		 = 0x00, //배틀 시작
	P_S_BATTLE_END		     = 0x01, //배틀 끝

	P_S_GS_TO_C_START_WAVE	 = 0x02, //웨이브 시작
	P_S_GS_TO_C_END_WAVE	 = 0x04, //웨이브 끝

	P_S_SELECT_CARD_START	 = 0x08, //카드 선택
	P_S_SELECT_CARD_END		 = 0x11, //카드 선택 끝
	P_S_PLAYER_USE_CARD		 = 0x12, //카드 사용

	P_S_NOTIFY_TURN_START	 = 0x14, //턴 시작
	P_S_NOTIFY_TURN_END		 = 0x18, //턴 종료 + 패보충
} e_packet_battle_msg;

#endif