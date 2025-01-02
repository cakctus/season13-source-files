#pragma once
#include "Protocol.h"

struct SWAMP_OF_DARKNESS_BOSS_AREA //0x76, 0x25
{
	PSBMSG_HEAD header;
	BYTE state;
	/*
	default = Boss
	3 = No Boss
	4 = 5 minutes before boss appear
	*/
};

struct SWAMP_OF_DARKNESS_BOSS_TIMER //0x76, 0x24
{
	PSBMSG_HEAD header;
	int time;
};

struct SWAMP_OF_DARKNESS_BOSS_WATER_MONSTER //0x76, 0x23
{
	PSBMSG_HEAD header;
	int id;
};

struct SWAMP_OF_DARKNESS_BOSS_ATTACK //0x76, 0x22
{
	PSBMSG_HEAD header;
	int monster;
	BYTE id;
	/*
	1 = Swamp Monster Summon
	2 = Swamp Pet Attack
	3 = Water Monster Attack Wave
	4 = Water Monster Attack Water Course
	5 = Water Monster HP
	6 = Flame Skill
	7 = Avoid black smoke and save from death!
	*/
};

struct SWAMP_OF_DARKNESS_BOSS_ENTER_RESULT //0x76, 0x21
{
	PSBMSG_HEAD h;
	BYTE result; /* 1=Success 2=Fail */
};

class GodOfDarkness
{
public:
	GodOfDarkness();
	virtual ~GodOfDarkness();
	void RecvReqChkEnterBossZone(SWAMP_OF_DARKNESS_BOSS_ENTER_RESULT* lpMsg, int aIndex);
	int CountBossZonePlayers();
	void RemoveBossZonePlayers();
	bool IsBossZone(int map, int x, int y);
	bool CheckBossLive();

	int GetBossRegenTime();

	void Process();

	void SendStatus(int status);

private:
	
};
extern GodOfDarkness g_GodOfDarkness;

