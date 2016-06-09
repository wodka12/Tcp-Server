#pragma once
#include "Singleton.h"
#include "battle_info.h"

#define BATTLE_MATCH_MAX 1024

enum e_battle_user_active {
	user_active_select_card = 0x00,
	user_active_use_card,
};

class battle_mgr : public CSingletonT<battle_mgr> ,public battle_info
{
public:
	battle_mgr();
	~battle_mgr();

public:
	unordered_map<int, battle_info*> map_battle_info;

public:
	void begin_game(ObjectUser* info, CStreamSP* pStreamSP);
	void update_game();
	void end_game();

	//유저행위총괄
	void user_active(ObjectUser* info, CStreamSP* pStreamSP, BYTE active, int selected_card);
};

#define BATTLE_MGR	battle_mgr::GetSingleton()