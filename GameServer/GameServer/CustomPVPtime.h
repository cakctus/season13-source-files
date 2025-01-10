#pragma once

#define MAX_CUSTOM_PVP_TIME 200

struct CUSTOM_PVP_TIME_START
{
	int Year;
	int Month;
	int Day;
	int DayOfWeek;
	int Hour;
	int Minute;
	int Second;
	int Map;
	int Duration;
};

struct CUSTOM_PVP_TIME_INFO
{
	int Index;
	int State;
	int RemainTime;
	int TargetTime;
	int TickCount;
	int AlarmMinSave;
	int AlarmMinLeft;
	CUSTOM_PVP_TIME_START StartTime;
};

enum eCustomPvPTimeState
{
	CUSTOM_PVP_TIME_STATE_BLANK = 0,
	CUSTOM_PVP_TIME_STATE_EMPTY = 1,
	CUSTOM_PVP_TIME_STATE_START = 2,
};

class CCustomPvPTime
{
public:
	CCustomPvPTime();
	virtual ~CCustomPvPTime();
	void Init();
	void Load(char* path);
	void MainProc();
	void ProcState_BLANK(CUSTOM_PVP_TIME_INFO* lpInfo);
	void ProcState_EMPTY(CUSTOM_PVP_TIME_INFO* lpInfo);
	void ProcState_START(CUSTOM_PVP_TIME_INFO* lpInfo);
	void SetState(CUSTOM_PVP_TIME_INFO* lpInfo,int state);
	void SetState_BLANK(CUSTOM_PVP_TIME_INFO* lpInfo);
	void SetState_EMPTY(CUSTOM_PVP_TIME_INFO* lpInfo);
	void SetState_START(CUSTOM_PVP_TIME_INFO* lpInfo);
	void CheckSync(CUSTOM_PVP_TIME_INFO* lpInfo);
	int PvPtimeMap;

private:
	CUSTOM_PVP_TIME_INFO m_CustomPvPtimeInfo[MAX_CUSTOM_PVP_TIME];
};

extern CCustomPvPTime gCustomPvPTime;
