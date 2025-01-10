#pragma once

#define MAX_LOG 20

enum eLogType
{
	LOG_GENERAL = 0,
	LOG_CHAT = 1,
	LOG_COMMAND = 2,
	LOG_TRADE = 3,
	LOG_CONNECT = 4,
	LOG_HACK = 5,
	LOG_CASH_SHOP = 6,
	LOG_CHAOS_MIX = 7,
	LOG_ITEM_DROP = 8,
	ITEM_BAG_LOG = 9,
	ITEM_UPGRADE_LOG = 10,
	STALK_LOG = 11,
	FILA_LOG = 12,
	MONSTER_SPAWNER_LOG = 13,
	CHARACTER_STATS = 14,
	LOG_OFICINA = 15
};

struct LOG_INFO
{
	BOOL Active;
	char Directory[256];
	int Day;
	int Month;
	int Year;
	char Filename[256];
	HANDLE File;
};

class CLog
{
public:
	CLog();
	virtual ~CLog();
	void AddLog(BOOL active,char* directory);
	void Output(eLogType type,char* text,...);
private:
	LOG_INFO m_LogInfo[MAX_LOG];
	int m_count;
};

extern CLog gLog;
