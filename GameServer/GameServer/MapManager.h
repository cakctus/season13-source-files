#pragma once
#include "User.h"

struct MAP_MANAGER_INFO
{
	int Index;
	int NonPK;
	int ViewRange;
	int ExperienceRate;
	int ItemDropRate;
	int PvPDamageRate;
	int SetItemDropRate;
	int SocketItemDrop;
	int HelperEnable;
	int GensBattle;
};

struct PVPZONE_DATA
{
	int	MapNumber;
	BYTE	X1;
	BYTE	X2;
	BYTE	Y1;
	BYTE	Y2;
};

class CMapManager
{
public:
	CMapManager();
	virtual ~CMapManager();
	void Load(char* path);
	void LoadPvPZone(char* path);
	int GetMapNonPK(int index);
	int GetPvPZone(LPOBJ lpObj);
	int GetMapNonOutlaw(int index);
	int GetMapViewRange(int index);
	int GetMapExperienceRate(int index);
	int GetMapMasterExperienceRate(int index);
	int GetMapItemDropRate(int index);
	int GetMapPvPDamageRate(int index);
	int GetMapSetItemDropRate(int index);
	int GetMapSocketItemDrop(int index);
	int GetMapHelperEnable(int index);
	int GetMapGensBattle(int index);
public:
	std::map<int, MAP_MANAGER_INFO> m_MapManagerInfo;
	std::map<int, PVPZONE_DATA> m_PvPZone;
	bool isLoaded;
	bool isPvPZoneLoaded;
};

extern CMapManager gMapManager;
