#pragma once
#include "battle_schedules.h"
#include "Croom_manager.h"

enum e_battle_state
{
	BATTLE_STATE_PENDING = 0x00,
	BATTLE_STATE_START,
	BATTLE_STATE_WAVE_START,
	BATTLE_STATE_WAIT_TURN_START,
	BATTLE_STATE_TURN_START,
	BATTLE_STATE_WAVE_END,
	BATTLE_STATE_END,
};

class battle_info
{
public:
	battle_info();
	~battle_info();

public:
	BYTE bt_battle_state = BATTLE_STATE_PENDING;
	int bt_info_room_idx = 0; //배틀중인 룸번호

public:
	battle_schedules* p_battle_schedules = new battle_schedules;

	void init_battle(int room_idx);
	void battle_info_update();
	//배틀 개시 통보
	int battle_broadcast_game_start(ObjectUser* info, CStreamSP* pStreamSP);

	//웨이브 개시 통보?
	int battle_broadcast_wave_start();

	//턴 개시 통보
	int battle_broadcast_turn_start();

	//패 제시 요청?
	int battle_request_present_card();

	//카드 선택 통보
	int battle_broadcast_selected_card(ObjectUser* info, CStreamSP* pStreamSP);

};

