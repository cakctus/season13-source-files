#include "StdAfx.h"

#include "user.h"
#include "Protocol.h"

#define MAX_MONSTER 600
#define MAX_CONFIG	200

struct TimeTable
{
	int Minutes;
	char Name[32];
};

struct PMSG_SCHEDULE_INVASIONS
{
	PSBMSG_HEAD	h;
	BYTE Order;
	char Name[32];
	DWORD Time;
};

class cMonsterSpawner
{
private:
	
	int Group[MAX_CONFIG];
	bool Enable[MAX_CONFIG];
	int Type[MAX_CONFIG];
	int Value[MAX_CONFIG];
	int Hour[MAX_CONFIG];
	int Min[MAX_CONFIG];
	int TimeEnd[MAX_CONFIG];

	char Name[MAX_CONFIG][64];
	char StartMessage[MAX_CONFIG][128];
	char DeathMessage[MAX_CONFIG][128];
	char EvadeMessage[MAX_CONFIG][128];

	bool EventStart[MAX_CONFIG];	

	struct
	{
		int Group;
		int Mob;
		int Map;
		int X1;
		int Y1;
		int X2;
		int Y2;
		int type;
		int value;
		bool generated;
		bool dead;
		char StartMessage[128];
		char DeathMessage[128];
	} Monster[MAX_MONSTER];
	
	int TotalConfig;

	int Tick[MAX_CONFIG];

	bool isLoaded;

public:
	cMonsterSpawner();
	~cMonsterSpawner();

	void Load(char* path);
	void StartMonster(int p);
	void EndMonster(int p);
	void AddMonster(int Code);
	void KillMonster(LPOBJ lpObj, LPOBJ lpMonster);

	void StartEvent(int p);
	void EndEvent(int p);

	void TickTime();
	bool EventStatus();
	void GetAndSendNextInvasions(int aIndex);

	void CheckAndSendMapEffect(LPOBJ lpObj);
	int TotalMob;
	int MonsterIndex[MAX_MONSTER];
	int MonsterClass[MAX_MONSTER];

};

extern cMonsterSpawner gMonsterSpawner;