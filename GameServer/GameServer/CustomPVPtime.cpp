#include "stdafx.h"
#include "CustomPVPtime.h"
#include "DSProtocol.h"
#include "Map.h"
#include "MapManager.h"
#include "MemScript.h"
#include "Notice.h"
#include "ScheduleManager.h"
#include "User.h"
#include "Util.h"
#include "Viewport.h"

CCustomPvPTime gCustomPvPTime;

CCustomPvPTime::CCustomPvPTime() // OK
{
	gCustomPvPTime.PvPtimeMap = -1;

	for (int n = 0; n < MAX_CUSTOM_PVP_TIME; n++)
	{
		CUSTOM_PVP_TIME_INFO* lpInfo = &this->m_CustomPvPtimeInfo[n];

		lpInfo->Index = n;
		lpInfo->State = CUSTOM_PVP_TIME_STATE_BLANK;
		lpInfo->RemainTime = 0;
		lpInfo->TargetTime = 0;
		lpInfo->TickCount = GetTickCount();
		lpInfo->AlarmMinSave = -1;
		lpInfo->AlarmMinLeft = -1;
		lpInfo->StartTime.Duration = -1;
	}
}

CCustomPvPTime::~CCustomPvPTime() // OK
{

}

void CCustomPvPTime::Init() // OK
{
	for (int n = 0; n < MAX_CUSTOM_PVP_TIME; n++)
	{
		this->SetState(&this->m_CustomPvPtimeInfo[n], CUSTOM_PVP_TIME_STATE_EMPTY);
	}
}

void CCustomPvPTime::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	for(int n=0;n < MAX_CUSTOM_PVP_TIME;n++)
	{
		CUSTOM_PVP_TIME_INFO* lpInfo = &this->m_CustomPvPtimeInfo[n];

		lpInfo->Index = n;
		lpInfo->State = CUSTOM_PVP_TIME_STATE_BLANK;
		lpInfo->RemainTime = 0;
		lpInfo->TargetTime = 0;
		lpInfo->TickCount = GetTickCount();
		lpInfo->AlarmMinSave = -1;
		lpInfo->AlarmMinLeft = -1;
		lpInfo->StartTime.Duration = -1;
	}

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					CUSTOM_PVP_TIME_START info;

					int index = lpMemScript->GetNumber();
					info.Year = lpMemScript->GetAsNumber();
					info.Month = lpMemScript->GetAsNumber();
					info.Day = lpMemScript->GetAsNumber();
					info.DayOfWeek = lpMemScript->GetAsNumber();
					info.Hour = lpMemScript->GetAsNumber();
					info.Minute = lpMemScript->GetAsNumber();
					info.Second = lpMemScript->GetAsNumber();
					info.Map = lpMemScript->GetAsNumber();
					info.Duration = lpMemScript->GetAsNumber();

					this->m_CustomPvPtimeInfo[index].StartTime = info;
				}				
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

	this->Init();
}

void CCustomPvPTime::MainProc() // OK
{
	for(int n=0;n < MAX_CUSTOM_PVP_TIME;n++)
	{
		CUSTOM_PVP_TIME_INFO* lpInfo = &this->m_CustomPvPtimeInfo[n];

		if((GetTickCount()-lpInfo->TickCount) >= 1000)
		{
			lpInfo->TickCount = GetTickCount();

			lpInfo->RemainTime = (int)difftime(lpInfo->TargetTime,time(0));

			switch(lpInfo->State)
			{
				case CUSTOM_PVP_TIME_STATE_BLANK:
					this->ProcState_BLANK(lpInfo);
					break;
				case CUSTOM_PVP_TIME_STATE_EMPTY:
					this->ProcState_EMPTY(lpInfo);
					break;
				case CUSTOM_PVP_TIME_STATE_START:
					this->ProcState_START(lpInfo);
					break;
			}
		}
	}
}

void CCustomPvPTime::ProcState_BLANK(CUSTOM_PVP_TIME_INFO* lpInfo) // OK
{
	
}

void CCustomPvPTime::ProcState_EMPTY(CUSTOM_PVP_TIME_INFO* lpInfo) // OK
{
	if(lpInfo->RemainTime > 0 && lpInfo->RemainTime <= (5*60))
	{
		if((lpInfo->AlarmMinSave=(((lpInfo->RemainTime%60)==0)?((lpInfo->RemainTime/60)-1):(lpInfo->RemainTime/60))) != lpInfo->AlarmMinLeft)
		{
			lpInfo->AlarmMinLeft = lpInfo->AlarmMinSave;
			gNotice.SendMapServerGroupMsg(0,"Atencao, garis! Limpeza inicia em %d minuto(s)",(lpInfo->AlarmMinLeft+1));
		}
	}

	if(lpInfo->RemainTime <= 0)
	{
		gNotice.SendMapServerGroupMsg(0,"Limpeza de mapa iniciada. Proteja-se, ou morra. *_*");
		this->SetState(lpInfo, CUSTOM_PVP_TIME_STATE_START);
	}
}

void CCustomPvPTime::ProcState_START(CUSTOM_PVP_TIME_INFO* lpInfo) // OK
{
	if(lpInfo->RemainTime <= 0)
	{
		gNotice.SendMapServerGroupMsg(0,"Limpeza encerrada. Obrigado, colaboradores.");
		this->SetState(lpInfo, CUSTOM_PVP_TIME_STATE_EMPTY);
	}
}

void CCustomPvPTime::SetState(CUSTOM_PVP_TIME_INFO* lpInfo,int state) // OK
{
	switch((lpInfo->State=state))
	{
		case CUSTOM_PVP_TIME_STATE_BLANK:
			this->SetState_BLANK(lpInfo);
			break;
		case CUSTOM_PVP_TIME_STATE_EMPTY:
			this->SetState_EMPTY(lpInfo);
			break;
		case CUSTOM_PVP_TIME_STATE_START:
			this->SetState_START(lpInfo);
			break;
	}
}

void CCustomPvPTime::SetState_BLANK(CUSTOM_PVP_TIME_INFO* lpInfo) // OK
{
	gCustomPvPTime.PvPtimeMap = -1;
}

void CCustomPvPTime::SetState_EMPTY(CUSTOM_PVP_TIME_INFO* lpInfo) // OK
{
	lpInfo->AlarmMinSave = -1;
	lpInfo->AlarmMinLeft = -1;

	gCustomPvPTime.PvPtimeMap = -1;

	this->CheckSync(lpInfo);
}

void CCustomPvPTime::SetState_START(CUSTOM_PVP_TIME_INFO* lpInfo) // OK
{
	lpInfo->AlarmMinSave = -1;
	lpInfo->AlarmMinLeft = -1;

	lpInfo->RemainTime = lpInfo->StartTime.Duration * 60;

	lpInfo->TargetTime = (int)(time(0)+lpInfo->RemainTime);

	gCustomPvPTime.PvPtimeMap = lpInfo->StartTime.Map;
}

void CCustomPvPTime::CheckSync(CUSTOM_PVP_TIME_INFO* lpInfo) // OK
{
	CTime ScheduleTime;

	CScheduleManager ScheduleManager;
	
	if (lpInfo->StartTime.Duration > 0)
	{
		ScheduleManager.AddSchedule(lpInfo->StartTime.Year,
			lpInfo->StartTime.Month,
			lpInfo->StartTime.Day,
			lpInfo->StartTime.Hour,
			lpInfo->StartTime.Minute,
			lpInfo->StartTime.Second,
			lpInfo->StartTime.DayOfWeek);
	}

	if(ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(lpInfo,CUSTOM_PVP_TIME_STATE_BLANK);
		return;
	}

	lpInfo->RemainTime = (int)difftime(ScheduleTime.GetTime(),time(0));

	lpInfo->TargetTime = (int)ScheduleTime.GetTime();
}