#pragma once

#define TIME_SELECT_CARD_LIMIT 1 * 30 * 1000 // 카드 선택 제한시간 30 sec
#define TIME_TURN_LIMIT 1 * 60 * 1000 // 턴 제한시간 1 min
#define TIME_WAVE_LIMIT 5 * 60 * 1000 // 웨이브 제한시간 5 min

enum e_schedule_type {
	SCHEDULE_TYPE_SELECT_CARD_LIMIT = 0x01,
	SCHEDULE_TYPE_TURN_LIMIT,
	SCHEDULE_TYPE_WAVE_LIMIT,
};

class battle_schedules
{
public:
	bool b_enable = false;
	DWORD dw_init_time = 0;
	DWORD dw_current_time = 0;
	DWORD dw_end_time = 0;

	BYTE bt_schedule_type = 0; //스케쥴 타입 - 카드선택제한시간, 턴제한시간, 웨이브제한시간등등.. 뒤에두개는 기획에서 미구현이지만 우선

public:
	battle_schedules();
	~battle_schedules();

	bool start(BYTE type);
	bool update();
	bool end();
	DWORD get_current_tick();
	bool get_schedule_status(); //스케쥴 상태
};

