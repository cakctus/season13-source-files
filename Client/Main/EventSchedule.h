#pragma once

#include "Protocol.h"

class EventSchedule
{

public:
	EventSchedule();

	void DrawScheduleWindow();
	bool ScheduleWindowClose(DWORD Event);
	void CloseScheduleWindow();
	bool EventWindowMain(DWORD Event);
	void ProcessScheduleData(PMSG_SCHEDULE_EVENTS* Data);
	void ProcessScheduleData2(PMSG_SCHEDULE_INVASIONS* Data);
	void MakeDate( int value, int * var );
	void WriteLine(char * EventName, int Hours, int Minutes, int lineOdd, int PositionX, int PositionY);	

private:
	int BCtime[2];
	int DStime[2];
	int CCtime[2];
	int ITtime[2];	
	int KantruTime[2];
	int SelupanTime[2];
	int KundunTime[2];
	int CrywolfTime[2];
	int PhoenixTime[2];
	int GoDTime[2];
	int MoSTime[2];
	int MoWTime[2];
	int NixTime[2];
	int DpGorgonTime[2];
	int FGTime[2];
	int LoFTime[2];
	int GnomeTime[2];

	int Invasions[16][2];
	char InvasionsName[16][32];
};

extern EventSchedule gEventSchedule;