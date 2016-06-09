#include "stdafx.h"
#include "battle_mgr.h"

//battle_mgr* battle_manager = new battle_mgr;
//battle_mgr* CSingletonT<battle_mgr>::s_pObject = NULL;

battle_mgr::battle_mgr()
{
	int temp  = 0;
	temp++;
}

battle_mgr::~battle_mgr()
{
}

void battle_mgr::begin_game(ObjectUser* info, CStreamSP* pStreamSP)
{
	static int temp_room_idx = 0; // 임시
	temp_room_idx++; //임시

	battle_info *p_battle_info = new battle_info;
	battle_schedules *p_battle_schedules = new battle_schedules;
	
	/* 배틀인포 초기화 */
	p_battle_info->init_battle(temp_room_idx);

	/* 배틀 스케쥴 새로 생성 시작 시간과 동시에 */
	p_battle_info->p_battle_schedules = p_battle_schedules;

	/* by lsy 배틀인포 스케쥴 등록 */
	unordered_map<int, battle_info*>::value_type value(temp_room_idx, p_battle_info);
	map_battle_info.insert(value);

	/* 룸유저들에게 배틀시작 통보 */
	p_battle_info->battle_broadcast_game_start(info, pStreamSP);
}

void battle_mgr::update_game()
{
	battle_info *p_battle_info = new battle_info;

	unordered_map<int, battle_info*>::iterator it_battle_info = map_battle_info.begin();

	while (it_battle_info != map_battle_info.end())
	{
		p_battle_info = it_battle_info->second;

		p_battle_info->battle_info_update();

		it_battle_info++;
		/*
		if (p_battle_info->p_battle_schedules->get_schedule_status() == false)
		{
			map_battle_info.erase(it_battle_info);
			break;
		}
		else
		{
			it_battle_info++;
		}
		*/
	}
}

void battle_mgr::end_game()
{
	battle_info *p_battle_info = new battle_info;
	unordered_map<int, battle_info*>::iterator it_battle_info = map_battle_info.begin();

	while (it_battle_info != map_battle_info.end())
	{
		p_battle_info = it_battle_info->second;
	}
}

//유저행위총괄
void battle_mgr::user_active(ObjectUser* info, CStreamSP* pStreamSP, BYTE active, int selected_card_num)
{
	battle_info *p_battle_info = new battle_info;
	unordered_map<int, battle_info*>::iterator it_battle_info = map_battle_info.find(info->sUser_info.room_num);
	p_battle_info = it_battle_info->second;

	switch (active)
	{
	case user_active_select_card:
		/* 선택한 카드위치를 갱신*/
		info->sUser_info.select_card = selected_card_num;

		/* 다른 룸유저들에게 선택한 자신의 카드를 통보 */
		p_battle_info->battle_broadcast_selected_card(info, pStreamSP);
		break;
	case user_active_use_card:
		break;
	default:
		break;
	}
}
