#include "stdafx.h"
#include "DevilSquare.h"
#include "DSProtocol.h"
#include "EffectManager.h"
#include "ItemManager.h"
#include "Map.h"
#include "MapServerManager.h"
#include "MemScript.h"
#include "Message.h"
#include "Monster.h"
#include "MonsterSetBase.h"
#include "Notice.h"
#include "NpcTalk.h"
#include "ObjectManager.h"
#include "ScheduleManager.h"
#include "ServerInfo.h"
#include "SXIDecryp.h"
#include "Util.h"
#include "GremoryCase.h"

CDevilSquare gDevilSquare;

CDevilSquare::CDevilSquare() // OK
{
	for(int n=0;n < MAX_DS_LEVEL;n++)
	{
		DEVIL_SQUARE_LEVEL* lpLevel = &this->m_DevilSquareLevel[n];

		lpLevel->Level = n;
		lpLevel->State = DS_STATE_BLANK;
		lpLevel->Map = ((n<4)?MAP_DEVIL_SQUARE1:MAP_DEVIL_SQUARE2);
		lpLevel->RemainTime = 0;
		lpLevel->TargetTime = 0;
		lpLevel->TickCount = GetTickCount();
		lpLevel->EnterEnabled = 0;
		lpLevel->MinutesLeft = -1;
		lpLevel->TimeCount = 0;
		lpLevel->Stage = 0;

		this->CleanUser(lpLevel);

		this->CleanMonster(lpLevel);
	}

	memset(this->m_DevilSquareRewardExperience,0,sizeof(this->m_DevilSquareRewardExperience));

	memset(this->m_DevilSquareRewardMoney,0,sizeof(this->m_DevilSquareRewardMoney));
}

CDevilSquare::~CDevilSquare() // OK
{

}

void CDevilSquare::Init() // OK
{
	for(int n=0;n < MAX_DS_LEVEL;n++)
	{
		if(gServerInfo.m_DevilSquareEvent == 0)
		{
			this->SetState(&this->m_DevilSquareLevel[n],DS_STATE_BLANK);
		}
		else
		{
			this->SetState(&this->m_DevilSquareLevel[n],DS_STATE_EMPTY);
		}
	}
}

void CDevilSquare::Load(char* path) // OK
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

	this->m_DevilSquareStartTime.clear();

	memset(this->m_DevilSquareRewardExperience,0,sizeof(this->m_DevilSquareRewardExperience));

	memset(this->m_DevilSquareRewardMoney,0,sizeof(this->m_DevilSquareRewardMoney));

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

					this->m_WarningTime = lpMemScript->GetNumber();
					this->m_NotifyTime = lpMemScript->GetAsNumber();
					this->m_EventTime = lpMemScript->GetAsNumber();
					this->m_CloseTime = lpMemScript->GetAsNumber();
				}
				else if(section == 1)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					DEVIL_SQUARE_START_TIME info;

					info.Year = lpMemScript->GetNumber();
					info.Month = lpMemScript->GetAsNumber();
					info.Day = lpMemScript->GetAsNumber();
					info.DayOfWeek = lpMemScript->GetAsNumber();
					info.Hour = lpMemScript->GetAsNumber();
					info.Minute = lpMemScript->GetAsNumber();
					info.Second = lpMemScript->GetAsNumber();

					this->m_DevilSquareStartTime.push_back(info);
				}
				else if(section == 2)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					int level = lpMemScript->GetNumber();

					for(int n=0;n < MAX_DS_RANK;n++)
					{
						this->m_DevilSquareRewardExperience[level][n] = lpMemScript->GetAsNumber();
					}
				}
				else if(section == 3)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					int level = lpMemScript->GetNumber();

					for(int n=0;n < MAX_DS_RANK;n++)
					{
						this->m_DevilSquareRewardMoney[level][n] = lpMemScript->GetAsNumber();
					}
				}
				else if (section == 4)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					int level = lpMemScript->GetNumber();

					for (int n = 0; n < MAX_DS_RANK; n++)
					{
						this->m_DevilSquareRewardItem[level][n] = lpMemScript->GetAsNumber();
						this->m_DevilSquareRewardItemQnt[level][n] = lpMemScript->GetAsNumber();
					}
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
}

void CDevilSquare::MainProc() // OK
{
	for(int n=0;n < MAX_DS_LEVEL;n++)
	{
		DEVIL_SQUARE_LEVEL* lpLevel = &this->m_DevilSquareLevel[n];

		DWORD elapsed = GetTickCount()-lpLevel->TickCount;

		if(elapsed < 1000)
		{
			continue;
		}

		lpLevel->TickCount = GetTickCount();

		lpLevel->RemainTime = (int)difftime(lpLevel->TargetTime,time(0));

		switch(lpLevel->State)
		{
			case DS_STATE_BLANK:
				this->ProcState_BLANK(lpLevel);
				break;
			case DS_STATE_EMPTY:
				this->ProcState_EMPTY(lpLevel);
				break;
			case DS_STATE_STAND:
				this->ProcState_STAND(lpLevel);
				break;
			case DS_STATE_START:
				this->ProcState_START(lpLevel);
				break;
			case DS_STATE_CLEAN:
				this->ProcState_CLEAN(lpLevel);
				break;
		}
	}
}

void CDevilSquare::ProcState_BLANK(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{

}

void CDevilSquare::ProcState_EMPTY(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	if(lpLevel->RemainTime > 0 && lpLevel->RemainTime <= (this->m_WarningTime*60))
	{
		this->CheckUser(lpLevel);

		int minutes = lpLevel->RemainTime/60;

		if((lpLevel->RemainTime%60) == 0)
		{
			minutes--;
		}

		if(lpLevel->MinutesLeft != minutes)
		{
			lpLevel->MinutesLeft = minutes;

			if(lpLevel->Level == 0)
			{
				EventIcon(3,1);
				gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(144),(lpLevel->MinutesLeft+1));
			}
		}

		lpLevel->EnterEnabled = 1;
	}

	if(lpLevel->RemainTime > 0 && lpLevel->RemainTime <= 30 && lpLevel->TimeCount == 0)
	{
		PMSG_TIME_COUNT_SEND pMsg;

		pMsg.header.set(0x92,sizeof(pMsg));

		pMsg.type = 0;

		DataSendAll((BYTE*)&pMsg,pMsg.header.size);

		lpLevel->TimeCount = 1;
	}

	if(lpLevel->RemainTime <= 0)
	{
		EventIcon(3,0);
		if(lpLevel->Level == 0){gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(145));}
		this->NoticeSendToAll(lpLevel,1,gMessage.GetMessage(146),(lpLevel->Level+1),this->m_NotifyTime);
		this->SetState(lpLevel,DS_STATE_STAND);
	}
}

void CDevilSquare::ProcState_STAND(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	this->CheckUser(lpLevel);

	if(this->GetUserCount(lpLevel) == 0)
	{
		LogAdd(LOG_BLACK,"[Devil Square] (%d) Not enough users",(lpLevel->Level+1));
		this->SetState(lpLevel,DS_STATE_EMPTY);
		return;
	}

	if(lpLevel->RemainTime > 0 && lpLevel->RemainTime <= 30 && lpLevel->TimeCount == 0)
	{
		PMSG_TIME_COUNT_SEND pMsg;

		pMsg.header.set(0x92,sizeof(pMsg));

		pMsg.type = 1;

		this->DataSendToAll(lpLevel,(BYTE*)&pMsg,pMsg.header.size);

		lpLevel->TimeCount = 1;
	}

	if(lpLevel->RemainTime <= 0)
	{
		this->NoticeSendToAll(lpLevel,0,gMessage.GetMessage(147));

		this->SetState(lpLevel,DS_STATE_START);
	}
}

void CDevilSquare::ProcState_START(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	this->CheckUser(lpLevel);

	if(this->GetUserCount(lpLevel) == 0)
	{
		LogAdd(LOG_BLACK,"[Devil Square] (%d) Not enough users",(lpLevel->Level+1));
		this->SetState(lpLevel,DS_STATE_EMPTY);
		return;
	}

	switch(lpLevel->Stage)
	{
		case 0:
			this->SetStage0(lpLevel);
			break;
		case 1:
			this->SetStage1(lpLevel);
			break;
		case 2:
			this->SetStage2(lpLevel);
			break;
		case 3:
			this->SetStage3(lpLevel);
			break;
	}

	if(lpLevel->RemainTime > 0 && lpLevel->RemainTime <= 30 && lpLevel->TimeCount == 0)
	{
		PMSG_TIME_COUNT_SEND pMsg;

		pMsg.header.set(0x92,sizeof(pMsg));

		pMsg.type = 2;

		this->DataSendToAll(lpLevel,(BYTE*)&pMsg,pMsg.header.size);

		lpLevel->TimeCount = 1;
	}

	if(lpLevel->RemainTime <= 0)
	{
		this->NoticeSendToAll(lpLevel,0,gMessage.GetMessage(148));
		this->SetState(lpLevel,DS_STATE_CLEAN);
	}
}

void CDevilSquare::ProcState_CLEAN(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	this->CheckUser(lpLevel);

	if(lpLevel->RemainTime <= 0)
	{
		this->SetState(lpLevel,DS_STATE_EMPTY);
	}
}

void CDevilSquare::SetState(DEVIL_SQUARE_LEVEL* lpLevel,int state) // OK
{
	lpLevel->State = state;

	switch(lpLevel->State)
	{
		case DS_STATE_BLANK:
			this->SetState_BLANK(lpLevel);
			break;
		case DS_STATE_EMPTY:
			this->SetState_EMPTY(lpLevel);
			break;
		case DS_STATE_STAND:
			this->SetState_STAND(lpLevel);
			break;
		case DS_STATE_START:
			this->SetState_START(lpLevel);
			break;
		case DS_STATE_CLEAN:
			this->SetState_CLEAN(lpLevel);
			break;
	}
}

void CDevilSquare::SetState_BLANK(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	LogAdd(LOG_BLACK,"[Devil Square] (%d) SetState BLANK",(lpLevel->Level+1));
}

void CDevilSquare::SetState_EMPTY(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	lpLevel->EnterEnabled = 0;
	lpLevel->MinutesLeft = -1;
	lpLevel->TimeCount = 0;
	lpLevel->Stage = 0;

	this->ClearUser(lpLevel);

	this->ClearMonster(lpLevel);

	this->CheckSync(lpLevel);

	LogAdd(LOG_BLACK,"[Devil Square] (%d) SetState EMPTY",(lpLevel->Level+1));
}

void CDevilSquare::SetState_STAND(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	lpLevel->EnterEnabled = 0;
	lpLevel->MinutesLeft = -1;
	lpLevel->TimeCount = 0;

	lpLevel->RemainTime = this->m_NotifyTime*60;

	lpLevel->TargetTime = (int)(time(0)+lpLevel->RemainTime);

	LogAdd(LOG_BLACK,"[Devil Square] (%d) SetState STAND",(lpLevel->Level+1));
}

void CDevilSquare::SetState_START(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	lpLevel->EnterEnabled = 0;
	lpLevel->MinutesLeft = -1;
	lpLevel->TimeCount = 0;

	switch(lpLevel->Level)
	{
		case 0:
			this->SetMonster(lpLevel,15);
			this->SetMonster(lpLevel,17);
			break;
		case 1:
			this->SetMonster(lpLevel,41);
			this->SetMonster(lpLevel,37);
			break;
		case 2:
			this->SetMonster(lpLevel,294);
			this->SetMonster(lpLevel,60);
			break;
		case 3:
			this->SetMonster(lpLevel,290);
			this->SetMonster(lpLevel,57);
			break;
		case 4:
			this->SetMonster(lpLevel,573);
			this->SetMonster(lpLevel,574);
			break;
		case 5:
			this->SetMonster(lpLevel,449);
			this->SetMonster(lpLevel,575);
			break;
		case 6:
			this->SetMonster(lpLevel,573);
			this->SetMonster(lpLevel,574);
			break;
	}

	lpLevel->RemainTime = this->m_EventTime*60;
	lpLevel->TargetTime = (int)(time(0)+lpLevel->RemainTime);

	LogAdd(LOG_BLACK,"[Devil Square] (%d) SetState START",(lpLevel->Level+1));
}

void CDevilSquare::SetState_CLEAN(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	lpLevel->EnterEnabled = 0;
	lpLevel->MinutesLeft = -1;
	lpLevel->TimeCount = 0;

	this->ClearMonster(lpLevel);

	this->CalcUserRank(lpLevel);

	this->GiveUserRewardExperience(lpLevel);
	this->GiveUserRewardMoney(lpLevel);
	this->GiveUserRewardItem(lpLevel);

	this->GCDevilSquareScoreSend(lpLevel);

	lpLevel->RemainTime = this->m_CloseTime*60;

	lpLevel->TargetTime = (int)(time(0)+lpLevel->RemainTime);

	LogAdd(LOG_BLACK,"[Devil Square] (%d) SetState CLEAN",(lpLevel->Level+1));
}

void CDevilSquare::SetStage0(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	if(((lpLevel->RemainTime*100)/(this->m_EventTime*60)) > 75)
	{
		return;
	}

	lpLevel->Stage = 1;

	switch(lpLevel->Level)
	{
		case 0:
			this->SetMonster(lpLevel,5);
			this->SetMonster(lpLevel,13);
			break;
		case 1:
			this->SetMonster(lpLevel,35);
			this->SetMonster(lpLevel,180);
			break;
		case 2:
			this->SetMonster(lpLevel,71);
			this->SetMonster(lpLevel,190);
			break;
		case 3:
			this->SetMonster(lpLevel,70);
			this->SetMonster(lpLevel,293);
			break;
		case 4:
			this->SetMonster(lpLevel,553);
			this->SetMonster(lpLevel,360);
			break;
		case 5:
			this->SetMonster(lpLevel,576);
			this->SetMonster(lpLevel,554);
			break;
		case 6:
			this->SetMonster(lpLevel,360);
			this->SetMonster(lpLevel,553);
			break;
	}
}

void CDevilSquare::SetStage1(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	if(((lpLevel->RemainTime*100)/(this->m_EventTime*60)) > 50)
	{
		return;
	}

	lpLevel->Stage = 2;

	switch(lpLevel->Level)
	{
		case 0:
			this->SetMonster(lpLevel,8);
			this->SetMonster(lpLevel,36);
			break;
		case 1:
			this->SetMonster(lpLevel,64);
			this->SetMonster(lpLevel,65);
			break;
		case 2:
			this->SetMonster(lpLevel,61);
			this->SetMonster(lpLevel,73);
			break;
		case 3:
			this->SetMonster(lpLevel,74);
			this->SetMonster(lpLevel,292);
			break;
		case 4:
			this->SetMonster(lpLevel,557);
			this->SetMonster(lpLevel,556);
			break;
		case 5:
			this->SetMonster(lpLevel,555);
			this->SetMonster(lpLevel,557);
			break;
		case 6:
			this->SetMonster(lpLevel,556);
			this->SetMonster(lpLevel,557);
			break;
	}
}

void CDevilSquare::SetStage2(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	if(((lpLevel->RemainTime*100)/(this->m_EventTime*60)) > 25)
	{
		return;
	}

	lpLevel->Stage = 3;

	switch(lpLevel->Level)
	{
		case 0:
			//this->SetMonster(lpLevel,18);
			this->SetMonster(lpLevel, 67);
			break;
		case 1:
			this->SetMonster(lpLevel,67);
			break;
		case 2:
			this->SetMonster(lpLevel,291);
			break;
		case 3:
			this->SetMonster(lpLevel,197);
			break;
		case 4:
			this->SetMonster(lpLevel,340);
			break;
		case 5:
			this->SetMonster(lpLevel,558);
			break;
		case 6:
			this->SetMonster(lpLevel,559);
			break;
	}
}

void CDevilSquare::SetStage3(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{

}

void CDevilSquare::CheckSync(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	if(this->m_DevilSquareStartTime.empty() != 0)
	{
		this->SetState(lpLevel,DS_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for(std::vector<DEVIL_SQUARE_START_TIME>::iterator it=this->m_DevilSquareStartTime.begin();it != this->m_DevilSquareStartTime.end();it++)
	{
		ScheduleManager.AddSchedule(it->Year,it->Month,it->Day,it->Hour,it->Minute,it->Second,it->DayOfWeek);
	}

	if(ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(lpLevel,DS_STATE_BLANK);
		return;
	}

	lpLevel->RemainTime = (int)difftime(ScheduleTime.GetTime(),time(0));

	lpLevel->TargetTime = (int)ScheduleTime.GetTime();

	LogAdd(LOG_BLACK,"[Devil Square] (%d) Sync Start Time. [%d] min remain",(lpLevel->Level+1),(lpLevel->RemainTime/60));
}

int CDevilSquare::GetState(int level) // OK
{
	if(DS_LEVEL_RANGE(level) == 0)
	{
		return DS_STATE_BLANK;
	}

	return this->m_DevilSquareLevel[level].State;
}

int CDevilSquare::GetRemainTime(int level) // OK
{
	if(DS_LEVEL_RANGE(level) == 0)
	{
		return 0;
	}

	if(this->m_DevilSquareStartTime.empty() != 0)
	{
		return 0;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for(std::vector<DEVIL_SQUARE_START_TIME>::iterator it=this->m_DevilSquareStartTime.begin();it != this->m_DevilSquareStartTime.end();it++)
	{
		ScheduleManager.AddSchedule(it->Year,it->Month,it->Day,it->Hour,it->Minute,it->Second,it->DayOfWeek);
	}

	if(ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		return 0;
	}

	int RemainTime = (int)difftime(ScheduleTime.GetTime(),time(0));

	return (((RemainTime%60)==0)?(RemainTime/60):((RemainTime/60)+1));
}

int CDevilSquare::GetEnterEnabled(int level) // OK
{
	if(DS_LEVEL_RANGE(level) == 0)
	{
		return 0;
	}
	
	return this->m_DevilSquareLevel[level].EnterEnabled;
}

int CDevilSquare::GetEnteredUserCount(int level) // OK
{
	if(DS_LEVEL_RANGE(level) == 0)
	{
		return 0;
	}

	return this->GetUserCount(&this->m_DevilSquareLevel[level]);
}

bool CDevilSquare::AddUser(DEVIL_SQUARE_LEVEL* lpLevel,int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	if(this->GetUser(lpLevel,aIndex) != 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) != 0)
		{
			continue;
		}

		lpLevel->User[n].Index = aIndex;
		lpLevel->User[n].Score = 0;
		lpLevel->User[n].Rank = 0;
		lpLevel->User[n].RewardExperience = 0;
		lpLevel->User[n].RewardMoney = 0;
		return 1;
	}

	return 0;
}

bool CDevilSquare::DelUser(DEVIL_SQUARE_LEVEL* lpLevel,int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	DEVIL_SQUARE_USER* lpUser = this->GetUser(lpLevel,aIndex);

	if(lpUser == 0)
	{
		return 0;
	}

	lpUser->Index = -1;
	lpUser->Score = 0;
	lpUser->Rank = 0;
	lpUser->RewardExperience = 0;
	lpUser->RewardMoney = 0;
	return 1;
}

DEVIL_SQUARE_USER* CDevilSquare::GetUser(DEVIL_SQUARE_LEVEL* lpLevel,int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(lpLevel->User[n].Index == aIndex)
		{
			return &lpLevel->User[n];
		}
	}

	return 0;
}

void CDevilSquare::CleanUser(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_USER;n++)
	{
		lpLevel->User[n].Index = -1;
		lpLevel->User[n].Score = 0;
		lpLevel->User[n].Rank = 0;
		lpLevel->User[n].RewardExperience = 0;
		lpLevel->User[n].RewardMoney = 0;
	}
}

void CDevilSquare::ClearUser(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		gObjMoveGate(lpLevel->User[n].Index,27);

		lpLevel->User[n].Index = -1;
		lpLevel->User[n].Score = 0;
		lpLevel->User[n].Rank = 0;
		lpLevel->User[n].RewardExperience = 0;
		lpLevel->User[n].RewardMoney = 0;
	}
}

void CDevilSquare::CheckUser(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		if(gObjIsConnected(lpLevel->User[n].Index) == 0)
		{
			this->DelUser(lpLevel,lpLevel->User[n].Index);
			continue;
		}

		if(gObj[lpLevel->User[n].Index].Map != lpLevel->Map)
		{
			this->DelUser(lpLevel,lpLevel->User[n].Index);
			continue;
		}
	}
}

int CDevilSquare::GetUserCount(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	int count = 0;

	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) != 0)
		{
			count++;
		}
	}

	return count;
}

int CDevilSquare::GetUserAbleLevel(LPOBJ lpObj) // OK
{
	return 6;

	int level = -1;
	int leveldec = lpObj->Level + lpObj->MasterLevel;

	if (leveldec >= 50 && leveldec <= 199)
	{
		level = 0;
	}
	else if (leveldec >= 200 && leveldec <= 299)
	{
		level = 1;
	}
	else if (leveldec >= 300 && leveldec <= 399)
	{
		level = 2;
	}
	else if (leveldec >= 400 && leveldec <= 549)
	{
		level = 3;
	}
	else if (leveldec >= 550 && leveldec <= 649)
	{
		level = 4;
	}
	else if (leveldec >= 650 && leveldec <= 769)
	{
		level = 5;
	}
	else if (leveldec >= 770)
	{
		level = 6;
	}

	return level;
}

void CDevilSquare::CalcUserRank(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		int rank = MAX_DS_USER;

		for(int i=0;i < MAX_DS_USER;i++)
		{
			if(OBJECT_RANGE(lpLevel->User[i].Index) == 0)
			{
				rank--;
				continue;
			}

			if(lpLevel->User[n].Index == lpLevel->User[i].Index)
			{
				rank--;
				continue;
			}

			if(lpLevel->User[n].Score > lpLevel->User[i].Score)
			{
				rank--;
				continue;
			}

			if(lpLevel->User[n].Score == lpLevel->User[i].Score && n < i)
			{
				rank--;
				continue;
			}
		}

		lpLevel->User[n].Rank = rank;
	}
}

void CDevilSquare::GiveUserRewardExperience(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		if(lpLevel->User[n].Rank >= MAX_DS_RANK)
		{
			continue;
		}

		lpLevel->User[n].RewardExperience = this->m_DevilSquareRewardExperience[lpLevel->Level][lpLevel->User[n].Rank];

		lpLevel->User[n].RewardExperience = lpLevel->User[n].RewardExperience*gServerInfo.m_AddEventExperienceRate[gObj[lpLevel->User[n].Index].AccountLevel];

		if(gEffectManager.CheckEffect(&gObj[lpLevel->User[n].Index],EFFECT_SEAL_OF_SUSTENANCE1) != 0 || gEffectManager.CheckEffect(&gObj[lpLevel->User[n].Index],EFFECT_SEAL_OF_SUSTENANCE2) != 0)
		{
			continue;
		}

		if(gObjectManager.CharacterLevelUp(&gObj[lpLevel->User[n].Index],lpLevel->User[n].RewardExperience,gServerInfo.m_MaxLevelUpEvent,EXPERIENCE_DEVIL_SQUARE) != 0)
		{
			continue;
		}

		GCRewardExperienceSend(lpLevel->User[n].Index,lpLevel->User[n].RewardExperience);
	}
}

void CDevilSquare::GiveUserRewardMoney(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		if(lpLevel->User[n].Rank >= MAX_DS_RANK)
		{
			continue;
		}

		lpLevel->User[n].RewardMoney = this->m_DevilSquareRewardMoney[lpLevel->Level][lpLevel->User[n].Rank];

		if(gObjCheckMaxMoney(lpLevel->User[n].Index,lpLevel->User[n].RewardMoney) == 0)
		{
			gObj[lpLevel->User[n].Index].Money = MAX_MONEY;
		}
		else
		{
			gObj[lpLevel->User[n].Index].Money += lpLevel->User[n].RewardMoney;
		}

		GCMoneySend(lpLevel->User[n].Index,gObj[lpLevel->User[n].Index].Money);
	}
}

void CDevilSquare::GiveUserRewardItem(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for (int n = 0; n < MAX_DS_USER; n++)
	{
		if (OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		if (lpLevel->User[n].Rank >= MAX_DS_RANK)
		{
			continue;
		}

		if (this->m_DevilSquareRewardItem[lpLevel->Level][lpLevel->User[n].Rank] > -1)
		{
			_stGremoryCaseItem stItem;
			stItem.btRewardSource = GC_REWARD_DEVIL_SQUARE;
			stItem.btStorageType = GC_STORAGE_CHARACTER;
			stItem.ItemInfo.Clear();

			stItem.ItemInfo.m_Index = this->m_DevilSquareRewardItem[lpLevel->Level][lpLevel->User[n].Rank];

			for (int j = 0; j < this->m_DevilSquareRewardItemQnt[lpLevel->Level][lpLevel->User[n].Rank]; j++)
			{
				g_GremoryCase.GDReqAddItemToGremoryCase(lpLevel->User[n].Index, stItem, 30);
			}
		}
	}
}

/*void CDevilSquare::GiveUserRewardItem(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for (int n = 0; n < MAX_DS_USER; n++)
	{
		if (OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		if (lpLevel->User[n].Rank >= MAX_DS_RANK)
		{
			continue;
		}
		
		_stGremoryCaseItem stItem;
		stItem.btRewardSource = GC_REWARD_TORMENTED_SQUARE_LEAGUE;
		stItem.btStorageType = GC_STORAGE_CHARACTER;
		stItem.ItemInfo.Clear();

		if (lpLevel->User[n].Rank > 5)
		{

		}		

		switch (lpLevel->TimerRandom)
		{
			case 3:
			{
				
					stItem.ItemInfo.m_Index = GET_ITEM(14, 282);
					g_GremoryCase.GDReqAddItemToGremoryCase(lpLevel->User[n].Index, stItem, 30);
			}
			break;

			case 4:
			case 5:
			case 6:
			{
				stItem.ItemInfo.m_Index = GET_ITEM(14, 260);
				g_GremoryCase.GDReqAddItemToGremoryCase(lpLevel->User[n].Index, stItem, 30);
			}
			break;
		}		
	}
}*/

bool CDevilSquare::AddMonster(DEVIL_SQUARE_LEVEL* lpLevel,int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	if(this->GetMonster(lpLevel,aIndex) != 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_DS_MONSTER;n++)
	{
		if(OBJECT_RANGE(lpLevel->MonsterIndex[n]) != 0)
		{
			continue;
		}

		lpLevel->MonsterIndex[n] = aIndex;
		return 1;
	}

	return 0;
}

bool CDevilSquare::DelMonster(DEVIL_SQUARE_LEVEL* lpLevel,int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	int* index = this->GetMonster(lpLevel,aIndex);

	if(index == 0)
	{
		return 0;
	}

	(*index) = -1;
	return 1;
}

int* CDevilSquare::GetMonster(DEVIL_SQUARE_LEVEL* lpLevel,int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_DS_MONSTER;n++)
	{
		if(lpLevel->MonsterIndex[n] == aIndex)
		{
			return &lpLevel->MonsterIndex[n];
		}
	}

	return 0;
}

void CDevilSquare::CleanMonster(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_MONSTER;n++)
	{
		lpLevel->MonsterIndex[n] = -1;
	}
}

void CDevilSquare::ClearMonster(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_MONSTER;n++)
	{
		if(OBJECT_RANGE(lpLevel->MonsterIndex[n]) != 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[lpLevel->MonsterIndex[n]].Class);
			gObjDel(lpLevel->MonsterIndex[n]);
			lpLevel->MonsterIndex[n] = -1;
		}
	}
}

int CDevilSquare::GetMonsterCount(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	int count = 0;

	for(int n=0;n < MAX_DS_MONSTER;n++)
	{
		if(OBJECT_RANGE(lpLevel->MonsterIndex[n]) != 0)
		{
			count++;
		}
	}

	return count;
}

void CDevilSquare::SetMonster(DEVIL_SQUARE_LEVEL* lpLevel,int MonsterClass) // OK
{
	for(int n=0; n < gMonsterSetBase.m_count; n++)
	{
		MONSTER_SET_BASE_INFO* lpInfo = &gMonsterSetBase.m_MonsterSetBaseInfo[n];

		if(lpInfo->Type != 4 || lpInfo->MonsterClass != MonsterClass || lpInfo->Map != lpLevel->Map)
		{
			continue;
		}

		int index = gObjAddMonster(lpLevel->Map);

		if(OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[index];

		if(gObjSetPosMonster(index,n) == 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[index].Class);
			gObjDel(index);
			continue;
		}

		if(gObjSetMonster(index,lpInfo->MonsterClass) == 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[index].Class);
			gObjDel(index);
			continue;
		}

		lpObj->MaxRegenTime = 1000;

		if(this->AddMonster(lpLevel,index) == 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[index].Class);
			gObjDel(index);
			continue;
		}
	}
}

void CDevilSquare::NpcCharon(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(gServerInfo.m_DevilSquareEvent == 0)
	{
		return;
	}

	int level = this->GetUserAbleLevel(lpObj);

	if(DS_LEVEL_RANGE(level) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage.GetMessage(149));
		return;
	}

	if(gItemManager.GetInventoryItemCount(lpObj,GET_ITEM(14,19),(level+1)) == 0 && gItemManager.GetInventoryItemCount(lpObj,GET_ITEM(13,46),0) == 0)
	{
		GCServerCommandSend(lpObj->Index,1,2,0);
		return;
	}

	if(this->GetEnterEnabled(level) == 0)
	{
		GCServerCommandSend(lpObj->Index,1,3,0);
		return;
	}

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 4;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CDevilSquare::UserDieProc(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	int level = this->GetUserAbleLevel(lpObj);

	if(DS_LEVEL_RANGE(level) == 0)
	{
		return;
	}

	if(this->GetState(level) != DS_STATE_START)
	{
		return;
	}

	if(this->GetUser(&this->m_DevilSquareLevel[level],lpObj->Index) == 0)
	{
		return;
	}

	this->DelUser(&this->m_DevilSquareLevel[level],lpObj->Index);
}

void CDevilSquare::MonsterDieProc(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	int aIndex = gObjMonsterGetTopHitDamageUser(lpObj);

	if(OBJECT_RANGE(aIndex) != 0)
	{
		lpTarget = &gObj[aIndex];
	}

	int level = this->GetUserAbleLevel(lpTarget);

	if(DS_LEVEL_RANGE(level) == 0)
	{
		return;
	}

	if(this->GetState(level) != DS_STATE_START)
	{
		return;
	}

	if(this->GetMonster(&this->m_DevilSquareLevel[level],lpObj->Index) == 0)
	{
		return;
	}

	if(lpTarget->Type != OBJECT_USER)
	{
		return;
	}

	DEVIL_SQUARE_USER* lpUser = this->GetUser(&this->m_DevilSquareLevel[level],lpTarget->Index);

	if(lpUser == 0)
	{
		return;
	}

	lpUser->Score += lpObj->Level*(level+1);
}

void CDevilSquare::NoticeSendToAll(DEVIL_SQUARE_LEVEL* lpLevel,int type,char* message,...) // OK
{
	char buff[256];

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) != 0)
		{
			gNotice.GCNoticeSend(lpLevel->User[n].Index,type,0,0,0,0,0,buff);
		}
	}
}

void CDevilSquare::DataSendToAll(DEVIL_SQUARE_LEVEL* lpLevel,BYTE* lpMsg,int size) // OK
{
	for(int n=0;n < MAX_DS_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) != 0)
		{
			DataSend(lpLevel->User[n].Index,lpMsg,size);
		}
	}
}

void CDevilSquare::CGDevilSquareEnterRecv(PMSG_DEVIL_SQUARE_ENTER_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo.m_DevilSquareEvent == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_DEVIL_SQUARE_ENTER_SEND pMsg;

	pMsg.header.set(0x90,sizeof(pMsg));

	pMsg.result = 0;

	lpMsg->slot = lpMsg->slot-INVENTORY_WEAR_SIZE;

	lpMsg->level = this->GetUserAbleLevel(lpObj);

	if(DS_LEVEL_RANGE(lpMsg->level) == 0)
	{
		pMsg.result = 1;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	DEVIL_SQUARE_LEVEL* lpLevel = &this->m_DevilSquareLevel[lpMsg->level];

	if(INVENTORY_FULL_RANGE(lpMsg->slot) == 0)
	{
		pMsg.result = 1;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpObj->Inventory[lpMsg->slot].m_Index != GET_ITEM(14,19) && lpObj->Inventory[lpMsg->slot].m_Index != GET_ITEM(13,46))
	{
		pMsg.result = 1;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpObj->Inventory[lpMsg->slot].m_Index == GET_ITEM(14,19) && lpObj->Inventory[lpMsg->slot].m_Level != (lpMsg->level+1))
	{
		pMsg.result = 1;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(this->GetEnterEnabled(lpMsg->level) == 0)
	{
		pMsg.result = 2;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(gMapServerManager.CheckMapServer(lpLevel->Map) == 0)
	{
		pMsg.result = 2;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(this->GetUserAbleLevel(lpObj) > lpMsg->level)
	{
		pMsg.result = 3;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(this->GetUserAbleLevel(lpObj) < lpMsg->level)
	{
		pMsg.result = 4;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(this->GetUserCount(lpLevel) >= gServerInfo.m_DevilSquareMaxUser)
	{
		pMsg.result = 5;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(this->AddUser(lpLevel,aIndex) == 0)
	{
		pMsg.result = 5;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	lpObj->DevilSquare_Enter++;

	gItemManager.DecreaseItemDur(lpObj,lpMsg->slot,1);

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;

	gObjMoveGate(aIndex, 270);

	/*switch(lpMsg->level)
	{
		case 0:
			gObjMoveGate(aIndex,58);
			break;
		case 1:
			gObjMoveGate(aIndex,59);
			break;
		case 2:
			gObjMoveGate(aIndex,60);
			break;
		case 3:
			gObjMoveGate(aIndex,61);
			break;
		case 4:
			gObjMoveGate(aIndex,111);
			break;
		case 5:
			gObjMoveGate(aIndex,112);
			break;
		case 6:
			gObjMoveGate(aIndex,270);
			break;
	}*/
}

void CDevilSquare::GCDevilSquareScoreSend(DEVIL_SQUARE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_DS_USER;n++)
	{
		DEVIL_SQUARE_USER* lpUser = &lpLevel->User[n];

		if(OBJECT_RANGE(lpUser->Index) == 0)
		{
			continue;
		}

		BYTE send[256];

		PMSG_DEVIL_SQUARE_SCORE_SEND pMsg;

		pMsg.header.set(0x93,0);

		int size = sizeof(pMsg);

		pMsg.rank = lpUser->Rank+1;

		pMsg.count = 0;
		
		PMSG_DEVIL_SQUARE_SCORE info;

		memcpy(info.name,gObj[lpUser->Index].Name,sizeof(info.name));

		info.score = lpUser->Score;

		info.RewardExperience = lpUser->RewardExperience;

		info.RewardMoney = lpUser->RewardMoney;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;

		int UserCount = this->GetUserCount(lpLevel)+1;

		while(pMsg.count != UserCount && pMsg.count < MAX_DS_RANK)
		{
			for(int i=0;i < MAX_DS_USER;i++)
			{
				if(OBJECT_RANGE(lpLevel->User[i].Index) == 0)
				{
					continue;
				}

				if((lpLevel->User[i].Rank+1) != pMsg.count)
				{
					continue;
				}

				memcpy(info.name,gObj[lpLevel->User[i].Index].Name,sizeof(info.name));

				info.score = lpLevel->User[i].Score;

				info.RewardExperience = lpLevel->User[i].RewardExperience;

				info.RewardMoney = lpLevel->User[i].RewardMoney;

				memcpy(&send[size],&info,sizeof(info));
				size += sizeof(info);

				pMsg.count++;
				break;
			}
		}

		pMsg.header.size = size;

		memcpy(send,&pMsg,sizeof(pMsg));

		DataSend(lpUser->Index,send,size);

		GDRankingDevilSquareSaveSend(lpUser->Index,lpUser->Score);
	}
}
