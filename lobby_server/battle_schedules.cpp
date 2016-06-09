#include "stdafx.h"
#include "battle_schedules.h"


battle_schedules::battle_schedules()
{
}


battle_schedules::~battle_schedules()
{
}


bool battle_schedules::start(BYTE schedule_type)
{
	bool ret = false;

	b_enable = true;

	bt_schedule_type = schedule_type;

	switch (bt_schedule_type)
	{
		case SCHEDULE_TYPE_SELECT_CARD_LIMIT:
			dw_end_time = TIME_SELECT_CARD_LIMIT;
			break;
		case SCHEDULE_TYPE_TURN_LIMIT:
			dw_end_time = TIME_TURN_LIMIT;
			break;
		case SCHEDULE_TYPE_WAVE_LIMIT:
			dw_end_time = TIME_WAVE_LIMIT;
			break;
	}

	dw_init_time = dw_current_time = GetTickCount();

	return ret;
}

bool battle_schedules::update()
{
	bool ret = false;

	if (b_enable == true)
	{
		dw_current_time = GetTickCount();

		if (dw_current_time > dw_init_time + dw_end_time)
		{
			end();
		}
	}

	return ret;
}

bool battle_schedules::end()
{
	bool ret = false;

	b_enable = false;

	return ret;
}

DWORD battle_schedules::get_current_tick()
{
	return dw_current_time;
}

bool battle_schedules::get_schedule_status()
{
	return b_enable;
}