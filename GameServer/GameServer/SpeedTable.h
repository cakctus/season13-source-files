#pragma once

#include "User.h"

#define MAX_SPEED_TABLE 200

struct SPEED_TABLE_INFO
{
	int Skill;
	BOOL PvP;
	BOOL PvM;
	WORD MinSpeed;
	WORD MaxSpeed;
	BYTE Hits;
	WORD Timer;
	WORD ThresholdOcurrences;
	WORD ThresholdTimer;
	BOOL DC;
	BOOL Log;
	WORD CoolDownHits;
};

class CSpeedTable
{
public:
	CSpeedTable();
	virtual ~CSpeedTable();
	void Load();
	void SetInfo(int Job, SPEED_TABLE_INFO info);

	SPEED_TABLE_INFO SpeedTable[8][MAX_SPEED_TABLE];
	
	int m_count[8];
};

extern CSpeedTable gSpeedTable;