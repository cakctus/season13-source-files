#pragma once

#include "User.h"

#define MAX_SPEED_FILA 500

struct SPEED_FILA_INFO
{
	int  Skill;
	WORD MinSpeed;
	WORD MaxSpeed;
	WORD Timer;
	BYTE LimPen;
	BYTE LimDC;
	WORD PingTol;
	WORD PenaltyTime;
};

class CSpeedFila
{
public:
	CSpeedFila();
	virtual ~CSpeedFila();
	void Load();
	void SetInfo(int Job, SPEED_FILA_INFO info);

	SPEED_FILA_INFO SpeedFila[8][MAX_SPEED_FILA];
	
	int m_count[8];
	bool isTableLoaded;
};

extern CSpeedFila gSpeedFila;