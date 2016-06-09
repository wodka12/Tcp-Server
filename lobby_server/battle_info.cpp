#include "stdafx.h"
#include "battle_info.h"


battle_info::battle_info()
{
}


battle_info::~battle_info()
{
}

void battle_info::init_battle(int room_idx)
{
	//배틀룸번호 저장
	bt_info_room_idx = room_idx;

	//배틀 시작 상태로 변경
	bt_battle_state = BATTLE_STATE_START;

	//턴 스케쥴러 시작
	p_battle_schedules->start(SCHEDULE_TYPE_TURN_LIMIT);
	
	//카드 선택 스케쥴러 시작
	p_battle_schedules->start(SCHEDULE_TYPE_SELECT_CARD_LIMIT);
}

//배틀 상태처리는 여기서 by lsy 16.06.09
void battle_info::battle_info_update()
{
	p_battle_schedules->update();

	//룸유저들에게 웨이브 개시를 알림

	//룸유저들에게 턴 개시를 알림

	//룸유저들에게 패제시를 요청 -> 이후 반환 처리 추가

	//룸유저들에게 턴 종료를 알림

	//룸유저들에게 웨이브 개시를 알림

}


extern Croom_manager* pRoommanager;
int battle_info::battle_broadcast_game_start(ObjectUser* info, CStreamSP* pStreamSP)
{
	int ret = 0;
	//16.06.09 호스트로부터 스타트 신호가오면 실행하도록 배틀패킷쪽으로 이동
	//by lsy 16.05.31 방배정직후 인원수가 꽉차면 룸 멤버에게 브로드캐스팅
	pRoommanager->broadcast_room(info, pStreamSP, 0);
	
	return ret;
}

int battle_info::battle_broadcast_wave_start()
{
	int ret = 0;

	return ret;
}
int battle_info::battle_broadcast_turn_start()
{
	int ret = 0;

	return ret;
}
int battle_info::battle_request_present_card()
{
	int ret = 0;

	return ret;
}

//카드 선택행위를 다른유저에 통보
int battle_info::battle_broadcast_selected_card(ObjectUser* info, CStreamSP* pStreamSP)
{
	int ret = 0;

	pRoommanager->broadcast_room(info, pStreamSP, 2); // 2 카드선택행위

	return ret;
}