#pragma once

#include "LabyrinthMission.h"

class LabyrinthInfo
{
public:
	LabyrinthInfo();
	~LabyrinthInfo();
	void AddEntireExp(QWORD nExp); // idb
	int GetCurrentZone();
	int GetDimensionLevel();
	void SetDBInfoLoad(bool bFlag); // idb
	void AddZonePath(int nZoneNumber); // idb
	void  AddEntireKillMonCnt(int nMonterKillCnt);
	unsigned int GetZonePathCount();
	BYTE GetZonePath(BYTE idx, int *nErrorCode); // idb
	void SendZonePath(int aIndex); // idb
	LabyrinthMission *GetLabyrinthMissionInfo(); // idb
	void Init(); // idb
	void AddLabyrinthMission(int nZoneNumber, int nMissionType, int nMissionValue, int nIsMainMission, int nMainMissionOrdering); // idb
	void SendCurrentZoneMissionInfo(int aIndex, int nZoneNumber); // idb
	bool CheckMissionClear(int aIndex, int nZoneType, int nZoneNumber); // idb
	void CheckPlayMission(int aIndex, int nObtainValue, int nMissionType, int nMissionCondition); // idb
	int MoveDimensionLevel(int aIndex); // idb
	void CheckLabyrinthUserAbility(int  aIndex); // idb
	void SetPlay(bool bFlag); // idb
	void SetConfigNum(int nConfigNum); // idb
	void SetCurrentZone(int nCurrentZone); // idb
	void SetDimensionLevel(int nDimensionLevel); // idb
	void SetLimitClearCnt(int nLimitClearCnt); // idb
	int GetLimitClearCnt(); // idb
	void SetClearState(BYTE btClearState); // idb
	char GetClearState(); // idb
	bool GetDBInfoLoad(); // idb
	unsigned __int64 GetEntireKillMonCnt(); // idb
	void SetEntireKillMonCnt(unsigned __int64 nMonterKillCnt); // idb
	unsigned __int64 GetEntireExp(); // idb
	void SetEntireExp(unsigned __int64 nExp); // idb
	int GetConfigNum(); // idb
	bool  IsPlaying();
	//private:
public:
	bool m_bPlaying;
	int m_nConfigNum;
	int m_nCurrentZone;
	int m_nDimensionLevel;
	unsigned __int64 m_nEntireKillMonCnt;
	unsigned __int64 m_nEntireExp;
	int m_nLimitClearCnt;
	char m_btClearState;
	bool m_bDBInfoLoad;
	std::vector<int> m_vecZonePath;
	LabyrinthMission m_cLabMission;
	int m_nEndCount;
	unsigned int m_nLabAttackMaxValue;
	unsigned int m_nLabAttackMinValue;
	unsigned int m_nLabDefenceValue;
};
extern LabyrinthInfo g_LabyrinthInfo;