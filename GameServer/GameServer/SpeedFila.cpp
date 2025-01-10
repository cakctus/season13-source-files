#include "stdafx.h"
#include "SpeedFila.h"
#include "MemScript.h"

CSpeedFila gSpeedFila;

CSpeedFila::CSpeedFila() // OK
{
	for(int i = 0; i < 8; i++)
		this->m_count[i] = 0;
}

CSpeedFila::~CSpeedFila() // OK
{
}

void CSpeedFila::Load()
{
	this->isTableLoaded = false;

	CMemScript* lpScript = new CMemScript;

	if(lpScript == 0)
	{
		return;
	}

	if(lpScript->SetBuffer("..\\Data\\Hack\\SpeedFila.txt") == 0)
	{
		delete lpScript;
		return;
	}

	for (int i = 0; i < 8; i++)
		this->m_count[i] = 0;

	int Job;

	try
	{
		while(true)
		{
			if(lpScript->GetToken() == 2)
			{
				break;
			}

			SPEED_FILA_INFO info;

			memset(&info,0,sizeof(info));

			Job				 = lpScript->GetNumber();
			info.Skill		 = lpScript->GetAsNumber();
			info.MinSpeed	 = lpScript->GetAsNumber();
			info.MaxSpeed	 = lpScript->GetAsNumber();
			info.Timer		 = lpScript->GetAsNumber();
			info.LimPen		 = lpScript->GetAsNumber();
			info.LimDC		 = lpScript->GetAsNumber();
			info.PingTol	 = lpScript->GetAsNumber();
			info.PenaltyTime = lpScript->GetAsNumber();

			this->SetInfo(Job, info);
		}
	}
	catch(...)
	{
		MessageBox(0,"SpeedFila","Error",0);
	}

	delete lpScript;

	this->isTableLoaded = true;
}

void CSpeedFila::SetInfo(int Job, SPEED_FILA_INFO info) // OK
{
	if(this->m_count[Job] < 0 || this->m_count[Job] >= MAX_SPEED_FILA)
	{
		return;
	}

	this->SpeedFila[Job][this->m_count[Job]] = info;
	this->m_count[Job]++;
}