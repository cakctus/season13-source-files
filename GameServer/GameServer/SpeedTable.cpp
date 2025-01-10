#include "stdafx.h"
#include "SpeedTable.h"
#include "MemScript.h"
#include "Util.h"

CSpeedTable gSpeedTable;

CSpeedTable::CSpeedTable() // OK
{
	this->m_count[0] = 0;
	this->m_count[1] = 0;
	this->m_count[2] = 0;
	this->m_count[3] = 0;
	this->m_count[4] = 0;
	this->m_count[5] = 0;
	this->m_count[6] = 0;
	this->m_count[7] = 0;
}

CSpeedTable::~CSpeedTable() // OK
{
}

void CSpeedTable::Load()
{
	CMemScript* lpScript = new CMemScript;

	if(lpScript == 0)
	{
		return;
	}

	if(lpScript->SetBuffer("..\\Data\\Hack\\SpeedTable.txt") == 0)
	{
		delete lpScript;
		return;
	}

	this->m_count[0] = 0;
	this->m_count[1] = 0;
	this->m_count[2] = 0;
	this->m_count[3] = 0;
	this->m_count[4] = 0;
	this->m_count[5] = 0;
	this->m_count[6] = 0;
	this->m_count[7] = 0;

	int Job;

	try
	{
		while(true)
		{
			if(lpScript->GetToken() == 2)
			{
				break;
			}

			SPEED_TABLE_INFO info;

			memset(&info,0,sizeof(info));

			Job				= lpScript->GetNumber();
			info.Skill		= lpScript->GetAsNumber();
			info.PvP		= lpScript->GetAsNumber();
			info.PvM		= lpScript->GetAsNumber();
			info.MinSpeed	= lpScript->GetAsNumber();
			info.MaxSpeed	= lpScript->GetAsNumber();
			info.Hits		= lpScript->GetAsNumber();
			info.Timer		= lpScript->GetAsNumber();

			info.ThresholdOcurrences	= lpScript->GetAsNumber();
			info.ThresholdTimer			= lpScript->GetAsNumber();
			info.DC						= lpScript->GetAsNumber();
			info.Log					= lpScript->GetAsNumber();
			info.CoolDownHits			= lpScript->GetAsNumber();

			this->SetInfo(Job, info);
		}
	}
	catch(...)
	{
		MessageBox(0,"SpeedTable V2","Error",0);
	}

	delete lpScript;
}

void CSpeedTable::SetInfo(int Job, SPEED_TABLE_INFO info) // OK
{
	if(this->m_count[Job] < 0 || this->m_count[Job] >= MAX_SPEED_TABLE)
	{
		return;
	}

	this->SpeedTable[Job][this->m_count[Job]] = info;
	this->m_count[Job]++;

	//LogAdd(LOG_RED,"Add %d %d %d %d %d %d %d %d %d", Job, info.Skill, info.PvP, info.PvM, info.MinSpeed, info.MaxSpeed, info.Hits, info.Timer, info.DC);
}