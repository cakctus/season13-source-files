#include "stdafx.h"
#include "IllusionTemple.h"
#include "DSProtocol.h"
#include "EffectManager.h"
#include "Gate.h"
#include "Guild.h"
#include "ItemBagManager.h"
#include "Map.h"
#include "MapServerManager.h"
#include "MemScript.h"
#include "Message.h"
#include "Monster.h"
#include "MonsterSetBase.h"
#include "Notice.h"
#include "NpcTalk.h"
#include "ObjectManager.h"
#include "Party.h"
#include "ScheduleManager.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "Util.h"
#include "Viewport.h"
#include "SXIDecryp.h"

CIllusionTemple gIllusionTemple;

CIllusionTemple::CIllusionTemple() // OK
{	
	ILLUSION_TEMPLE_LEVEL* lpLevel = &this->m_IllusionTempleLevel;

	lpLevel->State = IT_STATE_BLANK;
	lpLevel->Map = MAP_ILLUSION_TEMPLE1;
	lpLevel->RemainTime = 0;
	lpLevel->TargetTime = 0;	
	lpLevel->CurrentRound = 1;
	lpLevel->RoundBreakCount = 0;	
	lpLevel->TickCount = GetTickCount();
	lpLevel->EnterEnabled = 0;
	lpLevel->MinutesLeft = -1;
	lpLevel->TimeCount = 0;
	lpLevel->Score[IT_TEAM_Y] = 0;
	lpLevel->Score[IT_TEAM_B] = 0;
	lpLevel->PartyNumber[IT_TEAM_Y] = -1;
	lpLevel->PartyNumber[IT_TEAM_B] = -1;
	lpLevel->WinnerTeam = IT_TEAM_NONE;
	lpLevel->TeamRewardExperience[IT_TEAM_Y] = 0;
	lpLevel->TeamRewardExperience[IT_TEAM_B] = 0;

	this->CleanUser(lpLevel);

	lpLevel->StoneStatueIndex = -1;

	memset(this->m_IllusionTempleExperienceTable,0,sizeof(this->m_IllusionTempleExperienceTable));
}

CIllusionTemple::~CIllusionTemple() // OK
{

}

void CIllusionTemple::Init() // OK
{
	for(int n=0;n < MAX_IT_LEVEL;n++)
	{
		if(gServerInfo.m_IllusionTempleEvent == 0)
		{
			this->SetState(&this->m_IllusionTempleLevel,IT_STATE_BLANK);
		}
		else
		{
			this->SetState(&this->m_IllusionTempleLevel,IT_STATE_EMPTY);
		}
	}
}

void CIllusionTemple::Load(char* path) // OK
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

	this->m_IllusionTempleStartTime.clear();

	memset(this->m_IllusionTempleExperienceTable,0,sizeof(this->m_IllusionTempleExperienceTable));

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

					ILLUSION_TEMPLE_START_TIME info;

					info.Year = lpMemScript->GetNumber();
					info.Month = lpMemScript->GetAsNumber();
					info.Day = lpMemScript->GetAsNumber();
					info.DayOfWeek = lpMemScript->GetAsNumber();
					info.Hour = lpMemScript->GetAsNumber();
					info.Minute = lpMemScript->GetAsNumber();
					info.Second = lpMemScript->GetAsNumber();
					this->m_IllusionTempleStartTime.push_back(info);
				}
				else if(section == 2)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					int level = lpMemScript->GetNumber();

					this->m_IllusionTempleExperienceTable[0] = lpMemScript->GetAsNumber();
					this->m_IllusionTempleExperienceTable[1] = lpMemScript->GetAsNumber();
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

void CIllusionTemple::MainProc() // OK
{	
	ILLUSION_TEMPLE_LEVEL* lpLevel = &this->m_IllusionTempleLevel;

	DWORD elapsed = GetTickCount()-lpLevel->TickCount;

	if(elapsed < 1000)
	{
		return;
	}

	lpLevel->TickCount = GetTickCount();

	lpLevel->RemainTime = (int)difftime(lpLevel->TargetTime,time(0));

	switch(lpLevel->State)
	{
		case IT_STATE_BLANK:
			this->ProcState_NONE(lpLevel);
			break;
		case IT_STATE_EMPTY:
			this->ProcState_EMPTY(lpLevel);
			break;
		case IT_STATE_STAND:
			this->ProcState_STAND(lpLevel);
			break;
		case IT_STATE_START:
			this->ProcState_START(lpLevel);
			break;
		case IT_STATE_CLEAN:
			this->ProcState_CLEAN(lpLevel);
			break;
		case IT_STATE_ROUNDBREAK:
			this->ProcState_ROUNDBREAK();
			break;
	}	
}

void CIllusionTemple::ProcState_NONE(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{

}

void CIllusionTemple::ProcState_EMPTY(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
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
			
			EventIcon(6,1);
			gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(176),(lpLevel->MinutesLeft+1));			
		}

		lpLevel->EnterEnabled = 1;
	}

	if(lpLevel->RemainTime > 0 && lpLevel->RemainTime <= 30 && lpLevel->TimeCount == 0)
	{
		PMSG_TIME_COUNT_SEND pMsg;
		pMsg.header.set(0x92,sizeof(pMsg));
		pMsg.type = 28;
		DataSendAll((BYTE*)&pMsg,pMsg.header.size);
		lpLevel->TimeCount = 1;
	}

	if(lpLevel->RemainTime <= 0)
	{
		EventIcon(6,0);

		gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(177));
		this->NoticeSendToAll(1,gMessage.GetMessage(178),1,this->m_NotifyTime);
		this->SetState(lpLevel,IT_STATE_STAND);
	}
}

void CIllusionTemple::ProcState_STAND(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	this->CheckUser(lpLevel);

	if(this->GetUserCount(lpLevel,IT_TEAM_Y) < (gServerInfo.m_IllusionTempleMinUser/2) || this->GetUserCount(lpLevel,IT_TEAM_B) < (gServerInfo.m_IllusionTempleMinUser/2))
	{
		LogAdd(LOG_BLACK,"[Illusion Temple] Not enough users");
		this->SetState(lpLevel,IT_STATE_EMPTY);
		return;
	}

	if(lpLevel->RemainTime > 0 && lpLevel->RemainTime <= 30 && lpLevel->TimeCount == 0)
	{
		PMSG_TIME_COUNT_SEND pMsg;
		pMsg.header.set(0x92,sizeof(pMsg));
		pMsg.type = 29;
		this->DataSendToAll((BYTE*)&pMsg,pMsg.header.size);
		lpLevel->TimeCount = 1;
	}

	if(lpLevel->RemainTime <= 0)
	{
		this->DeleteTicket();		
		this->NoticeSendToAll(0,gMessage.GetMessage(179));
		this->SetState(lpLevel,IT_STATE_START);
	}
}

void CIllusionTemple::ProcState_START(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	this->CheckUser(lpLevel);

	if(this->GetUserCount(lpLevel,IT_TEAM_Y) == 0 || this->GetUserCount(lpLevel,IT_TEAM_B) == 0)
	{
		LogAdd(LOG_BLACK,"[Illusion Temple] Not enough users");
		this->SetState(lpLevel,IT_STATE_CLEAN);
		return;
	}

	this->SendPlayState(0);
	this->SendPlayState(1);

	if(lpLevel->RemainTime <= 0)
	{
		LogAdd(LOG_BLACK, "[Illusion Temple] RemainTime 0");
		this->NoticeSendToAll(0,gMessage.GetMessage(180));
		this->SetState(lpLevel,IT_STATE_CLEAN);
	}	
}

void CIllusionTemple::ProcState_CLEAN(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	this->CheckUser(lpLevel);

	if(lpLevel->RemainTime <= 0)
	{
		this->SetState(lpLevel,IT_STATE_EMPTY);
	}
}

void CIllusionTemple::ProcState_ROUNDBREAK()
{
	this->CheckUser(&m_IllusionTempleLevel);

	if (m_IllusionTempleLevel.RoundBreakCount <= 5)
	{
		m_IllusionTempleLevel.RoundBreakCount++;

		this->SendPlayState(0);
		this->SendPlayState(1);		

		return;
	}
	else
	{
		this->SetState(&m_IllusionTempleLevel, IT_STATE_START);
	}
}

void CIllusionTemple::SetState(ILLUSION_TEMPLE_LEVEL* lpLevel,int state) // OK
{
	lpLevel->State = state;

	switch(lpLevel->State)
	{
		case IT_STATE_BLANK:
			LogAdd(LOG_BLUE, "[Illusion Temple] Setando BLANK");
			this->SetState_BLANK(lpLevel);
			break;
		case IT_STATE_EMPTY:
			LogAdd(LOG_BLUE, "[Illusion Temple] Setando EMPTY");
			this->SetState_EMPTY(lpLevel);
			break;
		case IT_STATE_STAND:
			LogAdd(LOG_BLUE, "[Illusion Temple] Setando STAND");
			this->SetState_STAND(lpLevel);
			break;
		case IT_STATE_START:
			LogAdd(LOG_BLUE, "[Illusion Temple] Setando START");
			this->SetState_START(lpLevel);
			break;
		case IT_STATE_CLEAN:
			LogAdd(LOG_BLUE, "[Illusion Temple] Setando CLEAN");
			this->SetState_CLEAN(lpLevel);
			break;
		case IT_STATE_ROUNDBREAK:
			LogAdd(LOG_BLUE, "[Illusion Temple] Setando ROUNDBREAK");
			this->SetState_ROUNDBREAK(lpLevel);
			break;
	}
}

void CIllusionTemple::SetState_BLANK(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	LogAdd(LOG_BLACK,"[Illusion Temple] SetState BLANK");

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.User[i].Index != -1 && gObj[m_IllusionTempleLevel.User[i].Index].Connected > OBJECT_PLAYING && gObj[m_IllusionTempleLevel.User[i].Index].Map == MAP_ILLUSION_TEMPLE1)
		{
			LPOBJ lpObj = &gObj[m_IllusionTempleLevel.User[i].Index];

			lpObj->IllusionTemple_Index = -1;
			lpObj->IllusionTemple_RelicsTick = 0;
			lpObj->IllusionTemple_GettingRelics = false;
			lpObj->IllusionTemple_RegisteringRelics = false;
			lpObj->IllusionTemple_NpcType = 0;
			lpObj->IllusionTemple_StoneState = 99;

			this->DeletePartyTeam(lpObj->PartyNumber);

			gObjMoveGate(lpObj->Index, 503);
			lpObj->Change = -1;
			gObjViewportListProtocolCreate(lpObj);			
		}
	}
}

void CIllusionTemple::SetState_EMPTY(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	lpLevel->EnterEnabled = 0;
	lpLevel->MinutesLeft = -1;
	lpLevel->TimeCount = 0;
	lpLevel->CurrentRound = 1;
	lpLevel->RoundBreakCount = 0;
	lpLevel->Score[IT_TEAM_Y] = 0;
	lpLevel->Score[IT_TEAM_B] = 0;
	lpLevel->WinnerTeam = IT_TEAM_NONE;
	lpLevel->TeamRewardExperience[IT_TEAM_Y] = 0;
	lpLevel->TeamRewardExperience[IT_TEAM_B] = 0;

	if (OBJECT_RANGE(lpLevel->PartyNumber[IT_TEAM_Y]) != 0)
	{
		lpLevel->PartyNumber[IT_TEAM_Y] = -1;
	}

	if(OBJECT_RANGE(lpLevel->PartyNumber[IT_TEAM_B]) != 0)
	{
		lpLevel->PartyNumber[IT_TEAM_B] = -1;
	}

	this->GCIllusionTempleChangeScenarioSend(0,-1);

	this->ClearUser(lpLevel);

	if(OBJECT_RANGE(lpLevel->StoneStatueIndex) != 0)
	{
		//LogAdd(LOG_DEBUG,"1 Illusion Deleting %d (%s)", lpLevel->StoneStatueIndex, gObj[lpLevel->StoneStatueIndex].Name);
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[lpLevel->StoneStatueIndex].Class);
		gObjDel(lpLevel->StoneStatueIndex);
		lpLevel->StoneStatueIndex = -1;
	}

	this->ClearMonster(lpLevel);
	this->CheckSync(lpLevel);

	LogAdd(LOG_BLACK,"[Illusion Temple] SetState EMPTY");
}

void CIllusionTemple::SetState_STAND(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	lpLevel->EnterEnabled = 0;
	lpLevel->MinutesLeft = -1;
	lpLevel->TimeCount = 0;

	for(int n=0;n < MAX_IT_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) != 0)
		{
			gObjMoveGate(lpLevel->User[n].Index,148+(lpLevel->User[n].Team *6));
			gObj[lpLevel->User[n].Index].Change = 404+ lpLevel->User[n].Team;
			gObjViewportListProtocolCreate(&gObj[lpLevel->User[n].Index]);

			if(OBJECT_RANGE(lpLevel->PartyNumber[lpLevel->User[n].Team]) == 0)
			{
				if(gParty.Create(lpLevel->User[n].Index) != 0)
				{
					lpLevel->PartyNumber[lpLevel->User[n].Team] = gObj[lpLevel->User[n].Index].PartyNumber;
				}
			}
			else
			{
				if(gParty.AddMember(lpLevel->PartyNumber[lpLevel->User[n].Team],lpLevel->User[n].Index) != 0)
				{
					lpLevel->PartyNumber[lpLevel->User[n].Team] = gObj[lpLevel->User[n].Index].PartyNumber;
				}
			}
		}
	}

	this->GCIllusionTempleChangeScenarioSend(IT_STATE_EMPTY,-1);

	lpLevel->RemainTime = this->m_NotifyTime*60;

	lpLevel->TargetTime = (int)(time(0)+lpLevel->RemainTime);

	LogAdd(LOG_BLACK,"[Illusion Temple] SetState STAND");
}

void CIllusionTemple::SetState_START(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	if (lpLevel->CurrentRound == 1)
	{
		lpLevel->EnterEnabled = 0;
		lpLevel->MinutesLeft = -1;
		lpLevel->TimeCount = 0;

		lpLevel->RemainTime = this->m_EventTime * 60;
		lpLevel->TargetTime = (int)(time(0) + lpLevel->RemainTime);

		this->SetStoneStatue(lpLevel);
		this->SetOccupiedStoneStatue(lpLevel);		
	}

	this->GCIllusionTempleChangeScenarioSend(2,-1);

	LogAdd(LOG_BLACK,"[Illusion Temple] SetState START");
}

void CIllusionTemple::SetState_CLEAN(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	lpLevel->EnterEnabled = 0;
	lpLevel->MinutesLeft = -1;
	lpLevel->TimeCount = 0;

	BYTE byAllied = 0;
	BYTE byIllusion = 0;

	this->GetOccupiedStoneCount(byAllied, byIllusion);

	if(byAllied >= 1 && byAllied > byIllusion)
	{
		lpLevel->WinnerTeam = IT_TEAM_Y;		
	}
	else if(byIllusion >= 1 && byIllusion > byAllied)
	{
		lpLevel->WinnerTeam = IT_TEAM_B;
	}
	else
	{
		this->GetKillCount(byAllied, byIllusion);

		if (byAllied >= 1 && byAllied > byIllusion)
		{
			lpLevel->WinnerTeam = IT_TEAM_Y;
		}
		else if (byIllusion >= 1 && byIllusion > byAllied)
		{
			lpLevel->WinnerTeam = IT_TEAM_B;
		}
		else
		{
			lpLevel->WinnerTeam = -1;
		}
	}

	for(int n=0;n < MAX_IT_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) != 0)
		{
			this->CheckUserStoneState(&gObj[lpLevel->User[n].Index]);
			this->ResetUserFlag_DoingRelicsThing(&gObj[lpLevel->User[n].Index]);

			this->SearchUserDeleteEventItem(lpLevel->User[n].Index);
			gObjMoveGate(lpLevel->User[n].Index,148+(lpLevel->User[n].Team*6));
			this->DeletePartyTeam(gObj[lpLevel->User[n].Index].PartyNumber);

			for (int i = 0; i < MAX_PARTY_USER; i++)
				gParty.m_PartyInfo[gObj[lpLevel->User[n].Index].PartyNumber].IllusionTempleAccepted[i] = false;
		}
	}

	if(OBJECT_RANGE(lpLevel->StoneStatueIndex) != 0)
	{
		//LogAdd(LOG_DEBUG, "2 Illusion Deleting %d (%s)", lpLevel->StoneStatueIndex, gObj[lpLevel->StoneStatueIndex].Name);
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[lpLevel->StoneStatueIndex].Class);
		gObjDel(lpLevel->StoneStatueIndex);
		lpLevel->StoneStatueIndex = -1;
	}

	this->ClearMonster(lpLevel);
	this->GCIllusionTempleChangeScenarioSend(3,-1);
	this->GCIllusionTempleRewardScoreSend(lpLevel);

	lpLevel->RemainTime = this->m_CloseTime*60;
	lpLevel->TargetTime = (int)(time(0)+lpLevel->RemainTime);

	LogAdd(LOG_BLACK,"[Illusion Temple] SetState CLEAN");
}

void CIllusionTemple::SetState_ROUNDBREAK(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{	
	for (int n = 0; n < MAX_IT_USER; n++)
	{
		if (OBJECT_RANGE(lpLevel->User[n].Index) != 0)
		{
			LPOBJ lpObj = &gObj[lpLevel->User[n].Index];
			gObjMoveGate(lpLevel->User[n].Index, 148 + (lpLevel->User[n].Team * 6));
			this->SearchUserDeleteEventItem(lpLevel->User[n].Index);
			//this->ResetUserFlag_DoingRelicsThing(lpObj);
		}
	}

	this->RegenOccupiedStoneStatus();
	this->RegenCursedStoneStatus(m_IllusionTempleLevel.CurrentRound);
	this->GCIllusionTempleChangeScenarioSend(4, -1);

	m_IllusionTempleLevel.RoundBreakCount = 0;

	LogAdd(LOG_BLACK, "[IllusionTemple] Round Break");
}


void CIllusionTemple::CheckSync(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	if(this->m_IllusionTempleStartTime.empty() != 0)
	{
		this->SetState(lpLevel,IT_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for(std::vector<ILLUSION_TEMPLE_START_TIME>::iterator it=this->m_IllusionTempleStartTime.begin();it != this->m_IllusionTempleStartTime.end();it++)
	{
		ScheduleManager.AddSchedule(it->Year,it->Month,it->Day,it->Hour,it->Minute,it->Second,it->DayOfWeek);
	}

	if(ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(lpLevel,IT_STATE_BLANK);
		return;
	}

	lpLevel->RemainTime = (int)difftime(ScheduleTime.GetTime(),time(0));

	lpLevel->TargetTime = (int)ScheduleTime.GetTime();

	LogAdd(LOG_BLACK,"[Illusion Temple] Sync Start Time. [%d] min remain",(lpLevel->RemainTime/60));
}

int CIllusionTemple::GetState() // OK
{
	return this->m_IllusionTempleLevel.State;
}

int CIllusionTemple::GetRemainTime(int level) // OK
{
	if(IT_LEVEL_RANGE(level) == 0)
	{
		return 0;
	}

	if(this->m_IllusionTempleStartTime.empty() != 0)
	{
		return 0;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for(std::vector<ILLUSION_TEMPLE_START_TIME>::iterator it=this->m_IllusionTempleStartTime.begin();it != this->m_IllusionTempleStartTime.end();it++)
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

int CIllusionTemple::GetEnterEnabled(int level) // OK
{
	if(IT_LEVEL_RANGE(level) == 0)
	{
		return 0;
	}
	
	return this->m_IllusionTempleLevel.EnterEnabled;
}

int CIllusionTemple::GetEnteredUserCount(int level) // OK
{
	if(IT_LEVEL_RANGE(level) == 0)
	{
		return 0;
	}

	return this->GetUserCount(&this->m_IllusionTempleLevel,IT_TEAM_NONE);
}

bool CIllusionTemple::DelUser(ILLUSION_TEMPLE_LEVEL* lpLevel,int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	ILLUSION_TEMPLE_USER* lpUser = this->GetUser(lpLevel,aIndex);

	if(lpUser == 0)
	{
		return 0;
	}

	if(lpLevel->State == IT_STATE_START)
	{
		this->SearchUserDropEventItem(aIndex);
	}
	else
	{
		this->SearchUserDeleteEventItem(aIndex);
	}

	if (OBJECT_RANGE(lpLevel->PartyNumber[lpUser->Team]) != 0)
	{
		lpLevel->PartyNumber[lpUser->Team] = ((gParty.IsParty(lpLevel->PartyNumber[lpUser->Team])==0)?-1:lpLevel->PartyNumber[lpUser->Team]);
	}

	if(gObj[aIndex].Change >= 0)
	{
		gObj[aIndex].Change = -1;
		gObjViewportListProtocolCreate(&gObj[aIndex]);
	}

	lpUser->Index = -1;
	lpUser->Team = IT_TEAM_NONE;
	lpUser->Point = 0;
	lpUser->Score = 0;
	lpUser->RewardExperience = 0;

	LPOBJ lpObj = &gObj[aIndex];

	lpObj->IllusionTemple_Index = -1;
	lpObj->IllusionTemple_RelicsTick = 0;
	lpObj->IllusionTemple_GettingRelics = false;
	lpObj->IllusionTemple_RegisteringRelics = false;
	lpObj->IllusionTemple_NpcType = 0;
	lpObj->IllusionTemple_StoneState = 99;

	return 1;
}

ILLUSION_TEMPLE_USER* CIllusionTemple::GetUser(ILLUSION_TEMPLE_LEVEL* lpLevel,int aIndex) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_IT_USER;n++)
	{
		if(lpLevel->User[n].Index == aIndex)
		{
			return &lpLevel->User[n];
		}
	}

	return 0;
}

void CIllusionTemple::CleanUser(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_IT_USER;n++)
	{
		lpLevel->User[n].Index = -1;
		lpLevel->User[n].Team = IT_TEAM_NONE;
		lpLevel->User[n].Point = 0;
		lpLevel->User[n].Score = 0;
		lpLevel->User[n].RewardExperience = 0;
	}
}

void CIllusionTemple::ClearUser(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_IT_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		if(gObj[lpLevel->User[n].Index].Change >= 0)
		{
			gObj[lpLevel->User[n].Index].Change = -1;
			gObjViewportListProtocolCreate(&gObj[lpLevel->User[n].Index]);
		}

		gObjMoveGate(lpLevel->User[n].Index,503);

		lpLevel->User[n].Index = -1;
		lpLevel->User[n].Team = IT_TEAM_NONE;
		lpLevel->User[n].Point = 0;
		lpLevel->User[n].Score = 0;
		lpLevel->User[n].RewardExperience = 0;
	}
}

void CIllusionTemple::CheckUser(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	for(int n=0;n < MAX_IT_USER;n++)
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

		LPOBJ lpObj = &gObj[lpLevel->User[n].Index];

		if(gObj[lpLevel->User[n].Index].Map != lpLevel->Map)
		{
			this->DelUser(lpLevel,lpLevel->User[n].Index);
			continue;
		}

		/*if (this->HaveTicket(lpLevel->User[n].Index) == -1)
		{
			this->DelUser(lpLevel, lpLevel->User[n].Index);
			continue;
		}*/
	}
}

int CIllusionTemple::GetUserCount(ILLUSION_TEMPLE_LEVEL* lpLevel,int team) // OK
{
	int count = 0;

	for(int n=0;n < MAX_IT_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		if(team == IT_TEAM_NONE || lpLevel->User[n].Team == team)
		{
			count++;
		}
	}

	return count;
}

int CIllusionTemple::GetUserJoinTeam(LPOBJ lpObj) // OK
{
	ILLUSION_TEMPLE_USER* lpUser = this->GetUser(&this->m_IllusionTempleLevel,lpObj->Index);

	if(lpUser == 0)
	{
		return -1;
	}

	return lpUser->Team;
}

bool CIllusionTemple::GetUserRespawnLocation(LPOBJ lpObj,int* gate,int* map,int* x,int* y,int* dir,int* level) // OK
{
	ILLUSION_TEMPLE_USER* lpUser = this->GetUser(&this->m_IllusionTempleLevel,lpObj->Index);

	if(lpUser != 0 && lpUser->Team != IT_TEAM_NONE)
	{
		if(gGate.GetGate(((148+(lpUser->Team*6))),gate,map,x,y,dir,level) == 0)
		{
			return 0;
		}
	}
	else
	{
		if(gGate.GetGate(267,gate,map,x,y,dir,level) == 0)
		{
			return 0;
		}
	}

	return 1;
}

void CIllusionTemple::SearchUserDropEventItem(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	ILLUSION_TEMPLE_LEVEL* lpLevel = &this->m_IllusionTempleLevel;

	if(this->GetUser(lpLevel,aIndex) == 0)
	{
		return;
	}

	int slot = gItemManager.GetInventoryItemSlot(lpObj,GET_ITEM(14,223),0);

	if(slot == -1)
	{
		return;
	}

	PMSG_ITEM_DROP_SEND pMsg;

	pMsg.header.set(0x23,sizeof(pMsg));

	pMsg.result = 0;

	pMsg.slot = slot;

	CItem* lpItem = &gObj[aIndex].Inventory[slot];

	if(gMap[lpObj->Map].ItemDrop(lpItem->m_Index,lpItem->m_Level,lpItem->m_Durability,lpObj->X,lpObj->Y,lpItem->m_Option1,lpItem->m_Option2,lpItem->m_Option3,lpItem->m_NewOption,lpItem->m_SetOption,lpItem->m_Serial,aIndex,lpItem->m_PetItemLevel,lpItem->m_PetItemExp,lpItem->m_JewelOfHarmonyOption,lpItem->m_ItemOptionEx,lpItem->m_SocketOption,lpItem->m_SocketOptionBonus,lpItem->m_PeriodicItemTime) != 0)
	{
		pMsg.result = 1;
		gItemManager.InventoryDelItem(aIndex,slot);
		gItemManager.UpdateInventoryViewport(aIndex,slot);
	}

	this->ResetUserFlag_DoingRelicsThing(lpObj);

	this->SendRelicsUserInfo(lpObj, _RELICS_THROW_ITR);

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CIllusionTemple::SearchUserDeleteEventItem(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	ILLUSION_TEMPLE_LEVEL* lpLevel = &this->m_IllusionTempleLevel;

	if(this->GetUser(lpLevel,aIndex) == 0)
	{
		return;
	}

	int slot = gItemManager.GetInventoryItemSlot(lpObj,GET_ITEM(14,223),0);

	if(slot == -1)
	{
		return;
	}

	gItemManager.InventoryDelItem(lpObj->Index,slot);
	gItemManager.GCItemDeleteSend(lpObj->Index,slot,1);

	this->SendRelicsUserInfo(lpObj, _RELICS_THROW_ITR);
}

void CIllusionTemple::ClearMonster(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	if (lpLevel->StoneStatueIndex >= 0)
	{
		//LogAdd(LOG_DEBUG, "3 Illusion Deleting %d (%s)", lpLevel->StoneStatueIndex, gObj[lpLevel->StoneStatueIndex].Name);
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[lpLevel->StoneStatueIndex].Class);
		gObjDel(lpLevel->StoneStatueIndex);
		lpLevel->StoneStatueIndex = -1;
	}		

	for (int i = 0; i < 10; i++)
	{
		if (lpLevel->OccupiedStone[i].Index >= 0)
		{
			//LogAdd(LOG_DEBUG, "4 Illusion Deleting %d (%s)", lpLevel->OccupiedStone[i].Index, gObj[lpLevel->OccupiedStone[i].Index].Name);
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[lpLevel->OccupiedStone[i].Index].Class);
			gObjDel(lpLevel->OccupiedStone[i].Index);
			lpLevel->OccupiedStone[i].Index = -1;
		}			
	}

	m_IllusionTempleLevel.StoneStatue.byStatus = 0;
}

void CIllusionTemple::SetStoneStatue(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	if(OBJECT_RANGE(lpLevel->StoneStatueIndex) != 0)
	{
		return;
	}

	int index = gObjAddMonster(lpLevel->Map);
	
	if(OBJECT_RANGE(index) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[index];

	int px = 169;
	int py = 85;

	lpObj->PosNum = 0;
	lpObj->X = px;
	lpObj->Y = py;
	lpObj->TX = px;
	lpObj->TY = py;
	lpObj->OldX = px;
	lpObj->OldY = py;
	lpObj->StartX = px;
	lpObj->StartY = py;
	lpObj->Dir = 2;
	lpObj->Map = lpLevel->Map;

	if(gObjSetMonster(index,658) == 0)
	{
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[index].Class);
		gObjDel(index);
		LogAdd(LOG_RED, "[Illusion Temple] Set Statue error");
		return;
	}

	lpLevel->StoneStatueIndex = index;
}

void CIllusionTemple::NpcMirageTheMummy(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(gServerInfo.m_IllusionTempleEvent == 0)
	{
		return;
	}

	if (this->HaveTicket(lpObj->Index) == -1)
	{
		GCMessagePopupSend(lpObj, "Voce nao tem o ticket para entrada no Illusion Temple.\nCompre no NPC de Ruud, em Elbeland.");
		return;
	}

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 20;

	pMsg.level[0] = 0;
	pMsg.level[1] = this->GetUserCount(&this->m_IllusionTempleLevel,IT_TEAM_NONE);

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CIllusionTemple::DropEventItem(int map,int number,int aIndex) // OK
{
	gMap[map].m_Item[number].m_Time = GetTickCount()+ 300000;
	gMap[map].m_Item[number].m_LootTime = GetTickCount() + 30000;
}

void CIllusionTemple::UserDieProc(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if(this->GetState() != IT_STATE_START)
	{
		return;
	} 

	if(this->GetUser(&this->m_IllusionTempleLevel,lpObj->Index) == 0)
	{
		return;
	}

	this->CheckUserStoneState(lpObj);
	this->ResetUserFlag_DoingRelicsThing(lpObj);
	this->SearchUserDropEventItem(lpObj->Index);

	gEffectManager.DelEffect(lpObj,EFFECT_ORDER_OF_PROTECTION);
	gEffectManager.DelEffect(lpObj,EFFECT_ORDER_OF_RESTRAINT);

	if(lpTarget->Type != OBJECT_USER)
	{
		return;
	}

	ILLUSION_TEMPLE_USER* lpUser = this->GetUser(&this->m_IllusionTempleLevel,lpTarget->Index);

	if(lpUser == 0)
	{
		return;
	}

	if((lpUser->Point+5) > 90)
	{
		lpUser->Point = 90;
	}
	else
	{
		lpUser->Point += 5;
	}

	this->GCIllusionTempleKillPointSend(lpUser->Index,lpUser->Point);
	gNotice.GCNoticeSend(lpUser->Index,1,0,0,0,0,0, "Kill count: %d", lpUser->Point);
}

bool CIllusionTemple::CheckPlayerTarget(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	int level = GET_IT_LEVEL(lpObj->Map);

	if(IT_LEVEL_RANGE(level) == 0)
	{
		return 0;
	}

	if(this->GetState() != IT_STATE_START)
	{
		return 0;
	}

	if (lpTarget->X >= 138 && lpTarget->X <= 149 && lpTarget->Y >= 37 && lpTarget->Y <= 49)
	{
		return 0;
	}

	if (lpTarget->X >= 189 && lpTarget->X <= 202 && lpTarget->Y >= 122 && lpTarget->Y <= 130)
	{
		return 0;
	}

	ILLUSION_TEMPLE_USER* lpUser1 = this->GetUser(&this->m_IllusionTempleLevel,lpObj->Index);

	if(lpUser1 == 0)
	{
		return 0;
	}

	ILLUSION_TEMPLE_USER* lpUser2 = this->GetUser(&this->m_IllusionTempleLevel,lpTarget->Index);

	if(lpUser2 == 0)
	{
		return 0;
	}

	if(lpUser1->Team == lpUser2->Team)
	{
		return 0;
	}

	return 1;
}

void CIllusionTemple::NoticeSendToAll(int type,char* message,...) // OK
{
	char buff[256];

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	for(int n=0;n < MAX_IT_USER;n++)
	{
		if(OBJECT_RANGE(m_IllusionTempleLevel.User[n].Index) != 0)
		{
			gNotice.GCNoticeSend(m_IllusionTempleLevel.User[n].Index,type,0,0,0,0,0,buff);
		}
	}
}

void CIllusionTemple::DataSendToAll(BYTE* lpMsg,int size) // OK
{
	for(int n=0;n < MAX_IT_USER;n++)
	{
		if(OBJECT_RANGE(m_IllusionTempleLevel.User[n].Index) != 0)
		{
			DataSend(m_IllusionTempleLevel.User[n].Index,lpMsg,size);
		}
	}
}

void CIllusionTemple::CGIllusionTempleSkillRecv(PMSG_ILLUSION_TEMPLE_SKILL_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int level = GET_IT_LEVEL(lpObj->Map);

	if(IT_LEVEL_RANGE(level) == 0)
	{
		return;
	}

	ILLUSION_TEMPLE_USER* lpUser = this->GetUser(&this->m_IllusionTempleLevel,aIndex);

	if(lpUser == 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return;
	}

	int skill = MAKE_NUMBERW(lpMsg->skill[0],lpMsg->skill[1]);

	SKILL_INFO SkillInfo;

	if (gSkillManager.GetInfo(skill, &SkillInfo) == 0)
	{
		this->GCIllusionTempleAddSkillSend(aIndex,bIndex,0,skill);
		return;
	}

	if (lpUser->Point < SkillInfo.RequireKillCount)
	{
		this->GCIllusionTempleAddSkillSend(aIndex,bIndex,0,skill);
		return;
	}

	int result = 0;

	switch(skill)
	{
		case SKILL_ORDER_OF_PROTECTION:
			result = gSkillManager.SkillOrderOfProtection(aIndex,bIndex);
			break;
		case SKILL_ORDER_OF_RESTRAINT:
			result = gSkillManager.SkillOrderOfRestraint(aIndex,bIndex);
			break;
		case SKILL_ORDER_OF_TRACKING:
			result = gSkillManager.SkillOrderOfTracking(aIndex);
			break;
		case SKILL_ORDER_OF_WEAKEN:
			result = gSkillManager.SkillOrderOfWeaken(aIndex,bIndex);
			break;
	}

	if(result == 0)
	{
		this->GCIllusionTempleAddSkillSend(aIndex,bIndex,0,skill);
	}
	else
	{
		lpUser->Point -= SkillInfo.RequireKillCount;
		gNotice.GCNoticeSend(lpUser->Index, 1, 0, 0, 0, 0, 0, "Kill count: %d", lpUser->Point);
		this->GCIllusionTempleKillPointSend(aIndex,lpUser->Point);
		this->GCIllusionTempleAddSkillSend(aIndex,bIndex,1,skill);
	}
}

void CIllusionTemple::CGIllusionTempleButtonClickRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int level = GET_IT_LEVEL(lpObj->Map);

	if(IT_LEVEL_RANGE(level) == 0)
	{
		return;
	}

	if(this->GetState() != IT_STATE_CLEAN)
	{
		return;
	}

	ILLUSION_TEMPLE_USER* lpUser = this->GetUser(&this->m_IllusionTempleLevel,lpObj->Index);

	if(lpUser == 0)
	{
		return;
	}

	if(lpUser->Team != this->m_IllusionTempleLevel.WinnerTeam)
	{
		return;
	}

	lpObj->IllusionTemple_DailyWins++;

	gItemBagManager.DropItemBySpecialValue(ITEM_BAG_ILLUSION_TEMPLE1,lpObj,lpObj->Map,lpObj->X,lpObj->Y);
}

void CIllusionTemple::GCIllusionTempleAddSkillSend(int aIndex,int result,int skill,int bIndex) // OK
{
	PMSG_ILLUSION_TEMPLE_ADD_SKILL_SEND pMsg;

	pMsg.header.set(0xBF,0x02,sizeof(pMsg));

	pMsg.result = result;

	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);

	pMsg.index = aIndex;
	pMsg.target = bIndex;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	if(result != 0)
	{
		MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
	}
}

void CIllusionTemple::GCIllusionTempleRewardScoreSend(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	BYTE send[256];

	PMSG_ILLUSION_TEMPLE_REWARD_SCORE_SEND pMsg;
	pMsg.header.set(0xBF,0x04,0);
	int size = sizeof(pMsg);

	BYTE byAllied = 0;
	BYTE byIllusion = 0;
	this->GetOccupiedStoneCount(byAllied, byIllusion);

	pMsg.btAlliedPoint	 = byAllied;
	pMsg.btIllusionPoint = byIllusion;
	pMsg.byWinTeamIndex = lpLevel->WinnerTeam;

	pMsg.btUserCount = 0;

	PMSG_ILLUSION_TEMPLE_REWARD_SCORE info;

	for(int n=0;n < MAX_IT_USER;n++)
	{
		if(OBJECT_RANGE(lpLevel->User[n].Index) == 0)
		{
			continue;
		}

		memcpy(info.name,gObj[lpLevel->User[n].Index].Name,sizeof(info.name));

		info.map = 45;
		info.team = lpLevel->User[n].Team;
		info.Class = (BYTE)gObj[lpLevel->User[n].Index].Class;
		info.btUserKillCount = lpLevel->User[n].Score;

		GDRankingIllusionTempleSaveSend(lpLevel->User[n].Index,lpLevel->User[n].Score);

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.btUserCount++;
	}

	pMsg.header.size = size;
	memcpy(send,&pMsg,sizeof(pMsg));
	this->DataSendToAll(send,size);
}

void CIllusionTemple::GCIllusionTempleKillPointSend(int aIndex,int point) // OK
{
	PMSG_ILLUSION_TEMPLE_KILL_POINT_SEND pMsg;
	pMsg.header.set(0xBF,0x06,sizeof(pMsg));
	pMsg.KillPoint = point;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CIllusionTemple::GCIllusionTempleDelSkillSend(int aIndex,int skill) // OK
{
	PMSG_ILLUSION_TEMPLE_DEL_SKILL_SEND pMsg;
	pMsg.header.set(0xBF,0x07,sizeof(pMsg));
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	pMsg.index = aIndex;
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

//----------------------------------------------
//NEW or RENEWED functions
//----------------------------------------------

void CIllusionTemple::CGIllusionTempleEnterRecv(PMSG_ILLUSION_TEMPLE_ENTER_RECV* lpMsg, int aIndex) // OK
{
	if (gServerInfo.m_IllusionTempleEvent == 0)
	{
		return;
	}

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_ILLUSION_TEMPLE_ENTER_SEND pMsg;
	pMsg.header.set(0xBF, 0x70, sizeof(pMsg));
	pMsg.result = 0;

	if (this->HaveTicket(lpObj->Index) == -1)
	{
		GCMessagePopupSend(lpObj, "Voce nao tem o ticket para entrada no Illusion Temple. Compre no NPC de Ruud, em Elbeland.");
		return;
	}

	if (this->GetEnterEnabled(0) == 0)
	{
		pMsg.result = 8;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (lpObj->PartyNumber < 0)
	{
		pMsg.result = 1;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		LogAdd(LOG_RED, "[Illusion Temple] [%s] Não está em party", lpObj->Name);
		return;
	}

	if (!gParty.IsLeader(lpObj->PartyNumber, aIndex))
	{
		pMsg.result = 3;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		LogAdd(LOG_RED, "[Illusion Temple] [%s] Não é lider da party", lpObj->Name);
		return;
	}

	if (gParty.GetMemberCount(lpObj->PartyNumber) < gServerInfo.m_IllusionTempleMinUser)
	{
		pMsg.result = 4;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		LogAdd(LOG_RED, "[Illusion Temple] [%s] Membros da party inferior a 5 [%d]", lpObj->Name, gParty.GetMemberCount(lpObj->PartyNumber));
		return;
	}

	int PartyUsersIndex = 0;
	BYTE bResult = this->EnterCheckUser(aIndex, PartyUsersIndex);

	if (bResult != 0)
	{
		pMsg.result = bResult;
		memcpy(pMsg.Name, gObj[PartyUsersIndex].Name, sizeof(pMsg.Name));
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

		if (aIndex != PartyUsersIndex)
		{
			DataSend(PartyUsersIndex, (BYTE*)&pMsg, pMsg.header.size);
		}

		LogAdd(LOG_RED, "[IllusionTemple] EnterCheckUser [%s] Result: [%d] TroubleUserName: [%s] Index: [%d] ", gObj[aIndex].Name, pMsg.result, gObj[PartyUsersIndex].Name, PartyUsersIndex);
		return;
	}

	memset(pMsg.Name, 0, sizeof(pMsg.Name));
	memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));

	ILLUSION_TEMPLE_LEVEL* lpLevel = &this->m_IllusionTempleLevel;

	BYTE bySlotNum = 0xFF;
	BYTE byMapNumber = this->Find_EmptySlot(gObj[aIndex].PartyNumber, bySlotNum);

	if (byMapNumber == 253)
	{
		pMsg.result = 22;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		LogAdd(LOG_RED, "[IllusionTemple] [%s] Result: [%d], byMapNumber:[%d]", gObj[aIndex].Name, pMsg.result, byMapNumber);
		return;
	}

	if (byMapNumber != 3)
	{
		pMsg.result = 14;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		LogAdd(LOG_RED, "[IllusionTemple] [%s] Result: [%d], byMapNumber:[%d]", gObj[aIndex].Name, pMsg.result, byMapNumber);
		return;
	}

	BYTE bUpdateOk = this->SetPartyTeam(bySlotNum, lpObj->PartyNumber, aIndex);

	if (bUpdateOk != 0)
	{
		LogAdd(LOG_RED, "[IllusionTemple] [%s] ERRORcode: [%d/%d], Party:[%d]", gObj[aIndex].Name, bUpdateOk, pMsg.result, gObj[aIndex].PartyNumber);
		pMsg.result = 255;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	int useridx;
	int dbnumber = 0;

	for (int i = 0; i < MAX_PARTY_USER; i++)
	{
		useridx = gParty.GetMemberIndex(gObj[aIndex].PartyNumber, i);

		if (useridx <= 0)
		{
			continue;
		}

		if (useridx > 0)
		{
			pMsg.result = 0;
			DataSend(useridx, (BYTE*)&pMsg, pMsg.header.size);
			LogAdd(LOG_BLUE, "[IllusionTemple] OK [Leader: %s][Member: %s][Party: %d]", lpObj->Name, gObj[useridx].Name, gObj[aIndex].PartyNumber);
		}
	}
}

BYTE CIllusionTemple::EnterCheckUser(int iIndex, int& TroubleUserIndex)
{
	int useridx = 0;

	for (int i = 0; i < MAX_PARTY_USER; i++)
	{
		useridx = gParty.GetMemberIndex(gObj[iIndex].PartyNumber, i);
		TroubleUserIndex = useridx;

		if (useridx == -1)
		{
			LogAdd(LOG_RED, "[IllusionTemple] EnterCheckUser [Leader: %s][Party: %d(%d)]", gObj[iIndex].Name, gObj[iIndex].PartyNumber, i);
			continue;
		}

		if (useridx > 0)
		{
			if (gObj[useridx].Map != MAP_EVENT_SQUARE)
			{
				if (IT_MAP_RANGE(gObj[useridx].Map) == FALSE)
				{
					return 10;
				}
			}

			if (gServerInfo.m_IllusionTempleMaxDailyEnter > 0 && gObj[useridx].IllusionTemple_DailyEnter >= gServerInfo.m_IllusionTempleMaxDailyEnter)
			{
				LogAdd(LOG_RED, "[IllusionTemple] Daily Enter (%d) [Leader: %s][Party: %d/%d]", gObj[useridx].IllusionTemple_DailyEnter, gObj[iIndex].Name, gObj[iIndex].PartyNumber, gObj[useridx].PartyNumber);
				return 24;
			}

			if (gServerInfo.m_IllusionTempleMaxDailyWins > 0 && gObj[useridx].IllusionTemple_DailyWins >= gServerInfo.m_IllusionTempleMaxDailyWins)
			{
				LogAdd(LOG_RED, "[IllusionTemple] Daily Wins (%d) [Leader: %s][Party: %d/%d]", gObj[useridx].IllusionTemple_DailyWins, gObj[iIndex].Name, gObj[iIndex].PartyNumber, gObj[useridx].PartyNumber);
				return 24;
			}

			if (gObj[useridx].PartyNumber != gObj[iIndex].PartyNumber)
			{
				LogAdd(LOG_RED, "[IllusionTemple] EnterCheckUser [Leader: %s][Party: %d/%d]", gObj[iIndex].Name, gObj[iIndex].PartyNumber, gObj[useridx].PartyNumber);
				return 13;
			}

			if (gObj[useridx].Change != -1)
			{
				return 7;
			}

			if (gObj[useridx].Interface.state != 0 && gObj[useridx].Interface.type != INTERFACE_NONE)
			{
				return 11;
			}

			if (gObj[useridx].Guild != 0 && gObj[useridx].Guild->WarState == 1)
			{
				return 16;
			}

			if (OBJECT_RANGE(gObj[useridx].DuelUser) != 0)
			{
				return 15;
			}

			if (this->HaveTicket(useridx) == -1)
			{
				GCMessagePopupSend(&gObj[iIndex], "O jogador %s nao tem o ticket para entrada no Illusion Temple.", gObj[useridx].Name);
				return 13;
			}

			if (gServerInfo.m_IllusionTempleRestrictPC == 1)
			{
				for (int j = 0; j < MAX_PARTY_USER; j++)
				{
					int hwidIndex = gParty.GetMemberIndex(gObj[useridx].PartyNumber, j);
					if (hwidIndex != useridx && strcmp(gObj[useridx].HardwareId, gObj[hwidIndex].HardwareId) == 0)
					{
						GCMessagePopupSend(&gObj[iIndex], "Mais de um membro da party esta em um mesmo PC, o que e proibido. (%s e %s)", gObj[useridx].Name, gObj[hwidIndex].Name);
						return 13;
					}
				}
			}			
		}
	}

	return 0;
}

void CIllusionTemple::CGIllusionTempleAcceptRecv(PMSG_ILLUSION_TEMPLE_ACCEPT_RECV* lpMsg, int aIndex)
{
	if (gServerInfo.m_IllusionTempleEvent == 0)
	{
		return;
	}

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	BYTE byEnterOK = lpMsg->enterOK;

	PMSG_ILLUSION_TEMPLE_ACCEPT_SEND pMsg;
	pMsg.header.set(0xBF, 0x71, sizeof(pMsg));
	pMsg.result = byEnterOK;

	memset(pMsg.Name, 0, sizeof(pMsg.Name));
	memcpy(pMsg.Name, gObj[aIndex].Name, sizeof(pMsg.Name));

	if (gParty.IsParty(lpObj->PartyNumber) == FALSE)
	{
		LogAdd(LOG_RED, "[Illusion Temple] Aceito chegou por não-membro de party: %s", lpObj->Name);
		this->DeletePartyTeam(lpObj->PartyNumber);
		return;
	}

	if (byEnterOK != FALSE)
	{
		LogAdd(LOG_RED, "[Illusion Temple] [Pedido Recusado] Name: %s,", gObj[aIndex].Name);
		this->DeletePartyTeam(lpObj->PartyNumber);
		pMsg.result = 255;
		memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (gParty.EnterIllusionTemplePartyAuth(lpObj->PartyNumber, lpObj->Index) == false)
	{
		LogAdd(LOG_RED, "[Illusion Temple] [Erro Interno][Line:%d] Name: %s,", __LINE__, gObj[aIndex].Name);
		pMsg.result = 255;
		memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (gParty.AllAgreeEnterIllusionTemple(lpObj->PartyNumber) == true)
	{
		int TroubleUserIndex = -1;
		BYTE bResult = this->EnterCheckUser(aIndex, TroubleUserIndex);

		if (bResult != 0)
		{
			pMsg.result = bResult;
			memcpy(pMsg.Name, gObj[TroubleUserIndex].Name, sizeof(pMsg.Name));
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

			if (aIndex != TroubleUserIndex)
			{
				DataSend(TroubleUserIndex, (BYTE*)&pMsg, pMsg.header.size);
			}

			this->DeletePartyTeam(lpObj->PartyNumber);

			LogAdd(LOG_RED, "[IllusionTemple] EnterCheckUser [%s] Result: [%d] TroubleUserName: [%s] Index: [%d] ", gObj[aIndex].Name, pMsg.result, gObj[TroubleUserIndex].Name, TroubleUserIndex);
			return;
		}

		for (int i = 0; i < MAX_PARTY_USER; i++)
		{
			int UserIndex = gParty.GetMemberIndex(lpObj->PartyNumber, i);

			if (UserIndex == -1 || OBJECT_USER_RANGE(UserIndex) == 0)
			{
				continue;
			}

			BYTE bySlotNum = 0xFF;
			BYTE bReturn = this->FindExistingPartyInfo(lpObj->PartyNumber, bySlotNum);

			if (bReturn != 0)
			{
				LogAdd(LOG_RED, "[IllusionTemple][Find_ExistingPartyInfo] ERROR return:%d Name:%s, mapnumber:%d, bySlotNum:%d ", bReturn, lpObj->Name, lpObj->Map, bySlotNum);
				return;
			}

			if (this->EnterIllusionTemple(UserIndex, bySlotNum) == TRUE)
			{
				pMsg.result = 0;
				memcpy(pMsg.Name, gObj[UserIndex].Name, sizeof(pMsg.Name));
				DataSend(UserIndex, (LPBYTE)&pMsg, pMsg.header.size);

				gObj[UserIndex].Map = MAP_ILLUSION_TEMPLE1;
				gObjMoveGate(UserIndex, 142);

				LogAdd(LOG_RED, "[IllusionTemple] User Entered [%s]", gObj[UserIndex].Name);

				gObj[UserIndex].IllusionTemple_DailyEnter++;
			}
		}
	}
	else
	{
		LogAdd(LOG_RED, "[IllusionTemple] Accept [%s] Waiting for agreement of Party [%d] ", gObj[aIndex].Name, gObj[aIndex].PartyNumber);
	}
}


void CIllusionTemple::GCIllusionTempleChangeScenarioSend(int state,int aIndex) // OK
{
	PMSG_ILLUSION_TEMPLE_CHANGE_SCENARIO_SEND pMsg;

	pMsg.header.set(0xBF,0x60,sizeof(pMsg));

	pMsg.state = state;

	if(OBJECT_RANGE(aIndex) == 0)
	{
		this->DataSendToAll((BYTE*)&pMsg,pMsg.header.size);
	}
	else
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
}

BYTE CIllusionTemple::Find_EmptySlot(int PartyIndex, BYTE& byEmptySlot)
{
	BYTE bReturn = this->FindEmptyPartySlot(PartyIndex, byEmptySlot);

	if (byEmptySlot == 0 || byEmptySlot == 1)
	{
		if (bReturn == 0)
		{
			return 3;
		}
	}

	return bReturn;
}

BYTE CIllusionTemple::FindEmptyPartySlot(int partyindex, BYTE& byEmptySlot)
{
	if (partyindex < 0)
	{
		return 255;
	}

	for (int i = 0; i < 2; i++)
	{
		if (m_IllusionTempleLevel.PartyNumber[i] == partyindex)
		{
			return 253;
		}

		if (m_IllusionTempleLevel.PartyNumber[i] == -1)
		{
			byEmptySlot = i;
			return 0;
		}
	}

	return 254;
}

BYTE CIllusionTemple::SetPartyTeam(BYTE bySlotOrder, int nPartyIdx, int nUserindex)
{
	if (bySlotOrder > 2)
	{
		return -3;
	}

	if (m_IllusionTempleLevel.PartyNumber[bySlotOrder] != -1)
	{
		return -4;
	}

	m_IllusionTempleLevel.PartyNumber[bySlotOrder] = nPartyIdx;

	return 0;
}

BYTE CIllusionTemple::DeletePartyTeam(int nPartyIndex)
{
	for (int i = 0; i < 2; i++)
	{
		if (m_IllusionTempleLevel.PartyNumber[i] == nPartyIndex)
		{
			m_IllusionTempleLevel.PartyNumber[i] = -1;
			return 0;
		}
	}

	return 1;
}

BYTE CIllusionTemple::FindExistingPartyInfo(int partyinfo, BYTE& bySlotNum)
{
	BYTE bReturn1 = -1;
	BYTE bReturn2 = -1;
	
	int nTempNotUseIndex = -1;

	bReturn1 = this->FindSameParty(partyinfo, nTempNotUseIndex);

	if (bReturn1 == 0)
	{
		bReturn2 = this->FindPartySlot(partyinfo, bySlotNum);

		if (bReturn2 == 0)
		{
			return 0;
		}
	}	

	return -1;
}

BYTE CIllusionTemple::FindSameParty(int partyindex, int& nUserindex)
{
	if (partyindex < 0)
	{
		return -1;
	}

	for (int i = 0; i < 2; i++)
	{
		if (m_IllusionTempleLevel.PartyNumber[i] == partyindex)
		{
			return 0;
		}
	}

	return 1;
}

BYTE CIllusionTemple::FindPartySlot(int partyindex, BYTE& bySlotNum)
{
	if (partyindex < 0)
	{
		return -1;
	}

	for (int i = 0; i < 2; i++)
	{
		if (m_IllusionTempleLevel.PartyNumber[i] == partyindex)
		{
			bySlotNum = i;
			return 0;
		}
	}

	return 1;
}

BOOL CIllusionTemple::EnterIllusionTemple(int aIndex, BYTE bySlotNum)
{
	if (gServerInfo.m_IllusionTempleEvent == 0)
	{
		return FALSE;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj == NULL)
	{
		return FALSE;
	}

	if (this->EnterUserIllusionTemple(aIndex, lpObj->PartyNumber) == FALSE)
	{
		LogAdd(LOG_RED,"[Illusion Temple] EnterUser FAIL : (%s)(%s)", lpObj->Account, lpObj->Name);
		return FALSE;
	}

	LogAdd(LOG_BLUE,"[Illusion Temple] EnterUser: (%s)(%s)", lpObj->Account, lpObj->Name);
	this->GCIllusionTempleChangeScenarioSend(0, lpObj->Index);
	return TRUE;
}

BOOL CIllusionTemple::EnterUserIllusionTemple(int aIndex, int nPartyIdx)
{
	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return FALSE;
	}

	if (this->GetState() != IT_STATE_EMPTY)
	{
		LogAdd(LOG_RED, "[Illusion Temple] Entrando fora de estado EMPTY");
		return FALSE;
	}

	int nItemPos = this->SearchRelicsItem(aIndex);

	if (nItemPos >= 0 && gObj[aIndex].Inventory[nItemPos].m_Index == GET_ITEM(14, 223))
	{
		gItemManager.InventoryDelItem(aIndex, nItemPos);
		gItemManager.GCItemDeleteSend(aIndex, nItemPos, 0);
	}

	BOOL bResult = FALSE;
	//EnterCriticalSection(&this->m_critical);

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.User[i].Index == -1)
		{
			m_IllusionTempleLevel.User[i].Index = aIndex;
			m_IllusionTempleLevel.User[i].Party = nPartyIdx;			
			m_IllusionTempleLevel.User[i].EnterOK = true;

			bResult = TRUE;

			if (m_IllusionTempleLevel.PartyNumber[0] == gObj[m_IllusionTempleLevel.User[i].Index].PartyNumber)
			{
				m_IllusionTempleLevel.User[i].Team = 0;
				LogAdd(LOG_BLUE, "[Illusion Temple] [%s] entrou no time ALIADO [0]  ", gObj[m_IllusionTempleLevel.User[i].Index].Name);
			}
			else if (m_IllusionTempleLevel.PartyNumber[1] == gObj[m_IllusionTempleLevel.User[i].Index].PartyNumber)
			{
				m_IllusionTempleLevel.User[i].Team = 1;
				LogAdd(LOG_BLUE, "[Illusion Temple] [%s] entrou no time ILLUSION [1]", gObj[m_IllusionTempleLevel.User[i].Index].Name);
			}
			break;
		}
	}

	//LeaveCriticalSection(&this->m_critical);
	return bResult;
}

int CIllusionTemple::SearchRelicsItem(int iIndex)
{
	for (int x = 0; x < INVENTORY_FULL_SIZE; x++)
	{
		if (gObj[iIndex].Inventory[x].IsItem() == TRUE && gObj[iIndex].Inventory[x].m_Index == GET_ITEM(14, 223))
		{
			return x;
		}
	}

	return -1;
}

void CIllusionTemple::SetOccupiedStoneStatue(ILLUSION_TEMPLE_LEVEL* lpLevel) // OK
{
	int posX[10] = { 136,151,150,165,173,187,187,203,133,206 };
	int posY[10] = { 101,119,88,102,67,81,51,67,121,48 };

	for (int i = 0; i < 10; i++)
	{
		int index = gObjAddMonster(lpLevel->Map);

		if (OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[index];
		int monsterId = 659 + i;

		int px = posX[i];
		int py = posY[i];

		lpObj->PosNum = i+1;
		lpObj->X = px;
		lpObj->Y = py;
		lpObj->TX = px;
		lpObj->TY = py;
		lpObj->OldX = px;
		lpObj->OldY = py;
		lpObj->StartX = px;
		lpObj->StartY = py;
		lpObj->Dir = 2;
		lpObj->Map = lpLevel->Map;

		if (gObjSetMonster(index, monsterId) == 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[index].Class);
			gObjDel(index);
			continue;
		}

		lpLevel->OccupiedStone[i].Index = index;
		lpLevel->OccupiedStone[i].Type = monsterId;
		lpLevel->OccupiedStone[i].PosNum = i;
		lpLevel->OccupiedStone[i].byStatus = IT_OCCUPIED_NEUTRAL_ZONE;
	}	
}

int CIllusionTemple::GetOccupiedStoneStatue(ILLUSION_TEMPLE_LEVEL* lpLevel, int aIndex) // OK
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	for (int n = 0; n < 10; n++)
	{
		if (lpLevel->OccupiedStone[n].Index == aIndex)
		{
			return n;
		}
	}

	return 0;
}

void CIllusionTemple::GetRelics(LPOBJ lpNpc, LPOBJ lpObj, WORD wNpcType)
{
	if (lpNpc == NULL || lpObj == NULL)
	{
		return;
	}

	bool bFail = false;

	if (lpObj->IllusionTemple_NpcType < 0)
	{
		lpObj->IllusionTemple_NpcType = 0;
		LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 1");
		bFail = true;
	}

	if (lpObj->IllusionTemple_GettingRelics == false)
	{
		LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 2");
		bFail = true;
	}

	if (bFail == true)
	{
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (lpNpc->State == OBJECT_EMPTY || lpNpc->PosNum == -1)
	{
		LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 3");
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (abs(lpObj->Y - lpNpc->Y) > 3 || abs(lpObj->X - lpNpc->X) > 3)
	{
		LogAdd(LOG_RED,"[IllusionTemple][GetRelics] ERROR CASE 4");
		CheckUserStoneState(lpObj);
		ResetUserFlag_DoingRelicsThing(lpObj);		
		return;
	}

	int UserTick = lpObj->IllusionTemple_RelicsTick;
	int NowTick = GetTickCount();

	if (UserTick > NowTick)
	{
		LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 5");
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_TICKISNOVALID, 0xFF);
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	bool bCanGetRelics = false;
	WORD wMonsterIndex = -1;
	ILLUSION_TEMPLE_USER* ITUser;
	BYTE byUserTeamIndex = -1;

	ITUser = this->GetUser(&m_IllusionTempleLevel,lpObj->Index);
	byUserTeamIndex = ITUser->Team;

	if (wNpcType == 658 && wNpcType == lpObj->IllusionTemple_NpcType)
	{
		if ((m_IllusionTempleLevel.StoneStatue.byStatus - 10) != byUserTeamIndex || lpObj->IllusionTemple_StoneState != m_IllusionTempleLevel.StoneStatue.byStatus)
		{
			SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_STATUSERROR4, 0xFF);
			ResetUserFlag_DoingRelicsThing(lpObj);
			LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 6");
			return;
		}

		bCanGetRelics = true;
		m_IllusionTempleLevel.StoneStatue.byStatus = 255;
		lpObj->IllusionTemple_GettingRelics = false;

		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_OK, _ITR_RELICS_ACT_GET_RELICS);
		this->GCSendStoneInfo(wNpcType, m_IllusionTempleLevel.StoneStatue.byStatus, m_IllusionTempleLevel.StoneStatueIndex);

		LogAdd(LOG_BLACK, "[IllusionTemple][GetRelics] Getting Relics MAP:%d, IDX:%d,NAME:%s,NpcType:%d, NPCx:%d,NPCy:%d", lpNpc->Map, lpObj->Index, lpObj->Name, wNpcType, lpNpc->X, lpNpc->Y);

		lpNpc->PosNum = -1;
		lpNpc->Life = 0.0;
		gObjectManager.CharacterLifeCheck(lpObj, lpNpc, 0, 1, 0, 0, 0, 0);
	}
	else if ((wNpcType >= 659 && wNpcType <= 668) && wNpcType == lpObj->IllusionTemple_NpcType)
	{
		for (int i = 0; i < 10; i++)
		{
			if (lpNpc->Class == m_IllusionTempleLevel.OccupiedStone[i].Type)
			{
				if (m_IllusionTempleLevel.OccupiedStone[i].byStatus != 10 && m_IllusionTempleLevel.OccupiedStone[i].byStatus != 11)
				{
					LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 8");
					SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_STATUSERROR2, 0xFF);
					ResetUserFlag_DoingRelicsThing(lpObj);
					return;
				}

				if ((m_IllusionTempleLevel.OccupiedStone[i].byStatus - 10) == byUserTeamIndex)
				{
					bCanGetRelics = true;
					m_IllusionTempleLevel.OccupiedStone[i].byStatus = 254;
					lpObj->IllusionTemple_RelicsTick = 0;
					lpObj->IllusionTemple_GettingRelics = false;

					SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_OK, _ITR_RELICS_ACT_GET_RELICS);
					this->GCSendStoneInfo(wNpcType, m_IllusionTempleLevel.OccupiedStone[i].byStatus, m_IllusionTempleLevel.OccupiedStone[i].Index);

					LogAdd(LOG_BLACK,"[IllusionTemple][GetRelics] Occupied Stone MAP:%d,IDX:%d,NAME:%s,NpcType:%d,NPCx:%d,NPCy:%d", lpNpc->Map, lpObj->Index, lpObj->Name, wNpcType, lpNpc->X, lpNpc->Y);
					break;
				}
			}
		}
	}
	else
	{
		LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 9");
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_NPCERROR4, 0xFF);
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (bCanGetRelics == true)
	{
		int iType = GET_ITEM(14, 223);

		GDCreateItemSend(lpObj->Index, lpObj->Map, lpNpc->X, lpNpc->Y, iType, 0, 0, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, 0, 0, 0);
		LogAdd(LOG_BLACK, "[IllusionTemple] (%d) [%s][%s] Click To Status (X:%d, Y:%d)", lpObj->Map, lpObj->Account, lpObj->Name, lpNpc->X, lpNpc->Y);
	}

	ResetUserFlag_DoingRelicsThing(lpObj);
}

void CIllusionTemple::ResetUserFlag_DoingRelicsThing(LPOBJ lpObj)
{
	if (lpObj == NULL)
	{
		return;
	}

	LogAdd(LOG_BLACK,"[IllusionTemple][ResetUserFlag_DoingRelicsThing] NAME:%s, StoneState:%d, NPCTYPE:%d, GettingRelics:%d, RegisteringRelics:%d", lpObj->Name, lpObj->IllusionTemple_StoneState, lpObj->IllusionTemple_NpcType, lpObj->IllusionTemple_GettingRelics, lpObj->IllusionTemple_RegisteringRelics);

	lpObj->IllusionTemple_StoneState = 99;
	lpObj->IllusionTemple_NpcType = 0;
	lpObj->IllusionTemple_GettingRelics = false;
	lpObj->IllusionTemple_RegisteringRelics = false;
	lpObj->IllusionTemple_RelicsTick = 0;
}

void CIllusionTemple::SendRelicsError(LPOBJ lpNpc, int index, BYTE byError, BYTE byAct)
{
	if (lpNpc == NULL)
	{
		LogAdd(LOG_RED,"[IllusionTemple][SendRelicsError] lpNpc Is NULL,index:%d Error:%d,Act:%d", index, byError, byAct);
		return;
	}

	PMSG_ILLUSION_TEMPLE_ERROR pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x6A, sizeof(pMsg));

	pMsg.wNpcType = lpNpc->Class;
	pMsg.byNumberH = HIBYTE(lpNpc->Index);
	pMsg.byNumberL = LOBYTE(lpNpc->Index);
	pMsg.byError = byError;
	pMsg.byAct = byAct;

	DataSend(index, (LPBYTE)&pMsg, pMsg.h.size);
	LogAdd(LOG_BLACK,"[IllusionTemple][SendRelicsError]index:%d, Error:%d,Act:%d,NPC_Class:%d,NpcNumber:%d", index, byError, byAct, lpNpc->Class, lpNpc->Index);
}

void CIllusionTemple::GCSendStoneInfo(WORD wNpcType, BYTE byState, int NpcIndex)
{
	LogAdd(LOG_BLUE, "[IllusionTemple] Sending Stone Info");

	PMSG_ILLUSION_TEMPLE_STONE_INFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x6C, sizeof(pMsg));

	pMsg.byMapTagIndex = (BYTE) (wNpcType - 658);
	pMsg.wOccupiedStoneType = wNpcType;
	pMsg.byOccupiedStoneState = byState;
	pMsg.nNpcIndex = NpcIndex;

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.User[i].Index == -1)
			continue;
		
		DataSend(m_IllusionTempleLevel.User[i].Index, (LPBYTE)&pMsg, pMsg.h.size);
		LogAdd(LOG_BLUE,"[IT][SendStoneInfo] User:%d MapTagIndex:%d NpcType:%d NpcState:%d NpcIndex:%d", m_IllusionTempleLevel.User[i].Index, pMsg.byMapTagIndex, wNpcType, byState, NpcIndex);
	}
}

void CIllusionTemple::NpcStoneStatue(PMSG_ILLUSION_TEMPLE_RELATE_RELICS* lpMsg, int aIndex) // OK
{
	if (gServerInfo.m_IllusionTempleEvent == 0 || OBJECT_RANGE(aIndex) == 0 || gObjIsConnectedGP(aIndex) == 0 || lpMsg == NULL)
	{
		return;
	}

	int NpcNumber = MAKEWORD(lpMsg->NumberL, lpMsg->NumberH);

	if (NpcNumber > MAX_OBJECT - 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj == NULL)
	{
		return;
	}

	if (lpObj->Map != gObj[NpcNumber].Map)
	{
		return;
	}

	LPOBJ lpNpc = &gObj[NpcNumber];

	LogAdd(LOG_BLACK, "[Illusion Temple][ActRelicsGetOrRegister] NpcIndex:%d,byPrePacket:%d,byAct:%d,wNpcType:%d", NpcNumber, lpMsg->byPrePacket, lpMsg->byAct, lpMsg->NpcType);

	if (lpMsg->byPrePacket == 0x00 && lpMsg->byAct == 0x00)
	{
		this->CheckGetRelics(lpNpc, lpObj, lpMsg->NpcType);
		return;
	}

	if (lpMsg->byPrePacket == 0x01 && lpMsg->byAct == 0x00)
	{
		this->GetRelics(lpNpc, lpObj, lpMsg->NpcType);
		return;
	}

	if (lpMsg->byPrePacket == 0x00 && lpMsg->byAct == 0x01)
	{
		this->CheckRegisterRelics(lpNpc, lpObj);
		return;
	}

	if (lpMsg->byPrePacket == 0x01 && lpMsg->byAct == 0x01)
	{
		this->RegisterRelics(lpNpc, lpObj);
		return;
	}
}

void CIllusionTemple::CheckGetRelics(LPOBJ lpNpc, LPOBJ lpObj, WORD wNpcType)
{
	if (lpNpc == NULL || lpObj == NULL)
	{
		return;
	}

	if (lpNpc->State == OBJECT_EMPTY || lpNpc->PosNum == -1)
	{
		LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 3 (%d)(%d)", lpNpc->State, lpNpc->PosNum);
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (abs(lpObj->Y - lpNpc->Y) > 3 || abs(lpObj->X - lpNpc->X) > 3)
	{
		LogAdd(LOG_RED, "[IllusionTemple][GetRelics] ERROR CASE 4");
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	ILLUSION_TEMPLE_USER* ITUser;
	BYTE byUserTeamIndex = 0xFF;

	ITUser = this->GetUser(&m_IllusionTempleLevel, lpObj->Index);
	byUserTeamIndex = ITUser->Team;

	if (lpObj->IllusionTemple_GettingRelics == true)
	{
		LogAdd(LOG_BLACK, "[IllusionTemple][CheckGetRelics]NAME:%s,m_bITR_GettingRelics: %d", lpObj->Name, lpObj->IllusionTemple_GettingRelics);
		return;
	}

	if (lpObj->IllusionTemple_StoneState != 99)
	{
		LogAdd(LOG_BLACK, "[IllusionTemple][CheckGetRelics]NAME:%s,byStoneState: %d", lpObj->Name, lpObj->IllusionTemple_StoneState);
		return;
	}

	if (lpObj->IllusionTemple_RelicsTick != 0)
	{
		LogAdd(LOG_RED, "[IllusionTemple][CheckGetRelics] IllusionTemple_RelicsTick: %d", lpObj->IllusionTemple_RelicsTick);
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_TICKISNOZERO, 0xFF);
		lpObj->IllusionTemple_RelicsTick = 0;
		return;
	}

	if (wNpcType >= 659 && wNpcType <= 668)
	{
		for (int i = 0; i < 10; i++)
		{
			if (lpNpc->Index == m_IllusionTempleLevel.OccupiedStone[i].Index)
			{
				if (m_IllusionTempleLevel.OccupiedStone[i].byStatus != 0 && m_IllusionTempleLevel.OccupiedStone[i].byStatus != 1)
				{
					LogAdd(LOG_RED, "[IllusionTemple][CheckGetRelics] byStatus: %d", m_IllusionTempleLevel.OccupiedStone[i].byStatus);
					SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_STATUSERROR, 0xFF);
					return;
				}

				if (m_IllusionTempleLevel.OccupiedStone[i].byStatus == byUserTeamIndex)
				{
					LogAdd(LOG_RED, "[IllusionTemple][CheckGetRelics] Same team: %d %d", m_IllusionTempleLevel.OccupiedStone[i].byStatus, byUserTeamIndex);
					SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_TEAMERROR, 0xFF);
					return;
				}

				m_IllusionTempleLevel.OccupiedStone[i].byStatus = byUserTeamIndex + 10;
				lpObj->IllusionTemple_StoneState = m_IllusionTempleLevel.OccupiedStone[i].byStatus;
				lpObj->IllusionTemple_RelicsTick = GetTickCount() + 10000;
				lpObj->IllusionTemple_GettingRelics = true;
				lpObj->IllusionTemple_NpcType = wNpcType;

				this->GCSendStoneInfo(m_IllusionTempleLevel.OccupiedStone[i].Type, m_IllusionTempleLevel.OccupiedStone[i].byStatus, m_IllusionTempleLevel.OccupiedStone[i].Index);
				SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_OK, _ITR_RELICS_ACT_CHECK_GET_RELICS);

				LogAdd(LOG_BLACK, "[IllusionTemple][CheckGetRelics] Occupy OK MAP:%d,IDX:%d,NAME:%s,NpcType:%d,X:%d,Y:%d", lpObj->Map, lpObj->Index, lpObj->Name, wNpcType, lpNpc->X, lpNpc->Y);
				return;
			}
		}
	}
	else if (wNpcType == 658)
	{
		if (m_IllusionTempleLevel.StoneStatue.byStatus != 0)
		{
			SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_NPCERROR2, 0xFF);
			lpObj->IllusionTemple_RelicsTick = 0;
			return;
		}

		lpObj->IllusionTemple_RelicsTick = GetTickCount() + 10000;
		m_IllusionTempleLevel.StoneStatue.byStatus = byUserTeamIndex + 10;
		lpObj->IllusionTemple_StoneState = m_IllusionTempleLevel.StoneStatue.byStatus;
		lpObj->IllusionTemple_GettingRelics = true;
		lpObj->IllusionTemple_NpcType = 658;

		this->GCSendStoneInfo(658, m_IllusionTempleLevel.StoneStatue.byStatus, m_IllusionTempleLevel.StoneStatueIndex);
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_OK, _ITR_RELICS_ACT_CHECK_GET_RELICS);

		LogAdd(LOG_BLACK, "[IllusionTemple][CheckGetRelics] Stone OK IDX:%d,NAME:%s,NpcType:%d,NPCx:%d,NPCy:%d", lpObj->Index, lpObj->Name, wNpcType, lpNpc->X, lpNpc->Y);
	}
	else
	{
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_NPCERROR3, 0xFF);
		lpObj->IllusionTemple_RelicsTick = 0;
	}
}

void CIllusionTemple::CheckRegisterRelics(LPOBJ lpNpc, LPOBJ lpObj)
{
	if (lpNpc->State == OBJECT_NONE)
	{
		return;
	}

	if (lpNpc->PosNum == -1)
	{
		return;
	}

	if (abs(lpObj->Y - lpNpc->Y) > 3)
	{
		return;
	}

	if (abs(lpObj->X - lpNpc->X) > 3)
	{
		return;
	}

	bool bFail = false;

	if (lpObj->IllusionTemple_RegisteringRelics == true)
	{
		bFail = true;
	}

	if (lpObj->IllusionTemple_NpcType != 0)
	{
		bFail = true;
	}

	if (lpObj->IllusionTemple_StoneState != 99)
	{
		bFail = true;
	}

	if (bFail == true)
	{
		return;
	}

	ILLUSION_TEMPLE_USER* ITUser;

	ITUser = this->GetUser(&m_IllusionTempleLevel, lpObj->Index);

	int slot = gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 223), 0);

	if (slot <= 0 || lpObj->Inventory[slot].m_Index != GET_ITEM(14, 223))
	{
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_RELICSITEMERROR, 0xFF);
		return;
	}

	if (lpNpc->Class < 659 || lpNpc->Class > 668)
	{
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_NPCERROR5, 0xFF);
		return;
	}

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.OccupiedStone[i].Type == lpNpc->Class)
		{
			if (m_IllusionTempleLevel.OccupiedStone[i].byStatus != 255)
			{
				SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_STATUSERROR3, 0xFF);
				return;
			}

			if (lpObj->IllusionTemple_RelicsTick != 0)
			{
				SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_TICKISNOVALID1, 0xFF);
				return;
			}

			m_IllusionTempleLevel.OccupiedStone[i].byStatus = ITUser->Team + 100;
			lpObj->IllusionTemple_StoneState = m_IllusionTempleLevel.OccupiedStone[i].byStatus;
			lpObj->IllusionTemple_NpcType = m_IllusionTempleLevel.OccupiedStone[i].Type;
			lpObj->IllusionTemple_RelicsTick = GetTickCount() + 10000;
			lpObj->IllusionTemple_RegisteringRelics = true;

			SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_OK, _ITR_RELICS_ACT_CHECK_REGISTER_RELICS);
			this->GCSendStoneInfo(m_IllusionTempleLevel.OccupiedStone[i].Type, m_IllusionTempleLevel.OccupiedStone[i].byStatus, m_IllusionTempleLevel.OccupiedStone[i].Index);

			LogAdd(LOG_BLACK, "[IllusionTemple][CheckRegisterRelics] IDX:%d,NAME:%s,NpcType:%d", lpObj->Index, lpObj->Name, lpNpc->Class);
			return;
		}
	}
}

void CIllusionTemple::RegisterRelics(LPOBJ lpNpc, LPOBJ lpObj)
{
	if (lpNpc->State == OBJECT_NONE || lpNpc->PosNum == -1)
	{
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (abs(lpObj->Y - lpNpc->Y) > 3 || abs(lpObj->X - lpNpc->X) > 3)
	{
		CheckUserStoneState(lpObj);
		ResetUserFlag_DoingRelicsThing(lpObj);		
		return;
	}

	ILLUSION_TEMPLE_USER* ITUser;

	ITUser = this->GetUser(&m_IllusionTempleLevel, lpObj->Index);

	int slot = gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 223), 0);

	if (slot <= 0 || lpObj->Inventory[slot].m_Index != GET_ITEM(14, 223))
	{
		ResetUserFlag_DoingRelicsThing(lpObj);
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_RELICSITEMERROR1, 0xFF);
		return;
	}

	bool bFail = false;

	if (lpObj->IllusionTemple_RegisteringRelics == false)
	{
		bFail = true;
	}

	if (lpObj->IllusionTemple_NpcType < 0)
	{
		bFail = true;
	}

	if (lpObj->IllusionTemple_StoneState == 99)
	{
		bFail = true;
	}

	if (bFail == true)
	{
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	int UserTick = lpObj->IllusionTemple_RelicsTick;
	int NowTick = GetTickCount();

	if (UserTick > NowTick)
	{
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_TICKISNOVALID2, 0xFF);
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (lpNpc->Class < 659 || lpNpc->Class > 668)
	{
		SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_NPCERROR8, 0xFF);
		ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.OccupiedStone[i].Type == lpNpc->Class && ITUser->Team == m_IllusionTempleLevel.OccupiedStone[i].byStatus - 100)
		{
			LogAdd(LOG_BLUE,"[IllusionTemple] Occupying Stone %d Team: %d", i, ITUser->Team);
			m_IllusionTempleLevel.OccupiedStone[i].byStatus = ITUser->Team;
			ITUser->OccupiedCount++;

			SendRelicsError(lpNpc, lpObj->Index, _ITR_RELICS_ERROR_OK, _ITR_RELICS_ACT_REGISTER_RELICS);
			this->GCSendStoneInfo(m_IllusionTempleLevel.OccupiedStone[i].Type, m_IllusionTempleLevel.OccupiedStone[i].byStatus, m_IllusionTempleLevel.OccupiedStone[i].Index);

			lpObj->IllusionTemple_RegisteringRelics = false;

			int slot = gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 223), 0);

			if (slot > 0 && lpObj->Inventory[slot].m_Index == GET_ITEM(14, 223))
			{
				this->SendRelicsUserInfo(lpObj, _RELICS_THROW_ITR);
				LogAdd(LOG_BLACK, "[IllusionTemple][RegisterRelics] (%s)(%s) to Delete Relics Item [Serial:%u]", lpObj->Account, lpObj->Name, lpObj->Inventory[slot].m_Serial);

				gItemManager.InventoryDelItem(lpObj->Index, slot);
				gItemManager.GCItemDeleteSend(lpObj->Index, slot, 0);
			}

			ITUser->Score++;

			LogAdd(LOG_BLACK, "[IllusionTemple][RegisterRelics] IDX:%d,NAME:%s,NpcType:%d", lpObj->Index, lpObj->Name, lpNpc->Class);

			break;
		}
	}

	m_IllusionTempleLevel.CurrentRound++;

	this->SetState(&m_IllusionTempleLevel, IT_STATE_ROUNDBREAK);

	ResetUserFlag_DoingRelicsThing(lpObj);
}

void CIllusionTemple::SendRelicsUserInfo(LPOBJ lpObj, BYTE byGet)
{
	PMSG_ILLUSION_TEMPLE_RELICS_GET_USER pRelicsUser;
	PHeadSubSetB((LPBYTE)&pRelicsUser, 0xBF, 0x61, sizeof(pRelicsUser));

	pRelicsUser.wUserIndex = lpObj->Index;
	pRelicsUser.byGet = byGet;
	memcpy(pRelicsUser.Name, lpObj->Name, 10);

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.User[i].Index != -1 && gObj[m_IllusionTempleLevel.User[i].Index].Connected > OBJECT_PLAYING)
		{
			DataSend(m_IllusionTempleLevel.User[i].Index, (LPBYTE)&pRelicsUser, pRelicsUser.h.size);
		}
	}

	LogAdd(LOG_BLACK, "[IllusionTemple][SendRelicsUserInfo] NAME:%s, ACt:%d", lpObj->Name, byGet);
}

void CIllusionTemple::SendPlayState(BYTE btTeam)
{
	int nOffSet = 0;

	PMSG_ITL_PLAY_STATE pMsg;

	BYTE byAllied = 0;
	BYTE byIllusion = 0;
	this->GetOccupiedStoneCount(byAllied, byIllusion);

	pMsg.wRemainSec = m_IllusionTempleLevel.RemainTime;
	pMsg.btAlliedPoint = byAllied;
	pMsg.btIllusionPoint = byIllusion;
	pMsg.btMyTeam = btTeam;

	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x62, sizeof(pMsg));

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.User[i].Index != -1 && gObj[m_IllusionTempleLevel.User[i].Index].Connected > OBJECT_PLAYING && m_IllusionTempleLevel.User[i].Team == btTeam)
		{
			DataSend(m_IllusionTempleLevel.User[i].Index, (LPBYTE)&pMsg, pMsg.h.size);
		}
	}
}

void CIllusionTemple::GetOccupiedStoneCount(BYTE& byAllied, BYTE& byIllusion)
{
	byAllied = 0;
	byIllusion = 0;

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.OccupiedStone[i].byStatus == 0)
		{
			byAllied++;
		}
		else if (m_IllusionTempleLevel.OccupiedStone[i].byStatus == 1)
		{
			byIllusion++;
		}
	}
}

void CIllusionTemple::GetKillCount(BYTE& byAllied, BYTE& byIllusion)
{
	byAllied = 0;
	byIllusion = 0;

	for (int n = 0; n < MAX_IT_USER; n++)
	{
		if (OBJECT_RANGE(m_IllusionTempleLevel.User[n].Index) == 0)
		{
			continue;
		}

		if (m_IllusionTempleLevel.User[n].Team == IT_TEAM_Y)
		{
			byAllied += m_IllusionTempleLevel.User[n].Point;
		}
		else if (m_IllusionTempleLevel.User[n].Team == IT_TEAM_B)
		{
			byIllusion += m_IllusionTempleLevel.User[n].Point;
		}
	}
}

void CIllusionTemple::RegenCursedStoneStatus(BYTE byRound)
{
	int nRandNum = rand() % 100;
	BYTE byOccupiedStoneCnt = 0;

	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.OccupiedStone[i].byStatus == 0 || m_IllusionTempleLevel.OccupiedStone[i].byStatus == 1)
		{
			byOccupiedStoneCnt++;
		}
	}

	BYTE byCursedStoneRegenRatio = 100 - (10 * byOccupiedStoneCnt);

	if (nRandNum <= byCursedStoneRegenRatio)
	{
		m_IllusionTempleLevel.StoneStatue.byStatus = 0;
		m_IllusionTempleLevel.StoneStatueIndex = -1;
		this->SetStoneStatue(&m_IllusionTempleLevel);
	}
	else
	{
		m_IllusionTempleLevel.StoneStatue.byStatus = 255;
	}

	this->GCSendStoneInfo(658, m_IllusionTempleLevel.StoneStatue.byStatus, m_IllusionTempleLevel.StoneStatueIndex);
	LogAdd(LOG_BLACK, "[IllusionTemple][RegenCursedStoneStatus] Create NPC Status  Round:%d,RegenStatus:%d", byRound, m_IllusionTempleLevel.StoneStatue.byStatus);
}

void CIllusionTemple::RegenOccupiedStoneStatus()
{
	for (int i = 0; i < 10; i++)
	{
		if (m_IllusionTempleLevel.OccupiedStone[i].Index != -1)
		{
			LPOBJ lpObj = &gObj[m_IllusionTempleLevel.OccupiedStone[i].Index];
			lpObj->PosNum = m_IllusionTempleLevel.OccupiedStone[i].PosNum;
			lpObj->Live = TRUE;
			lpObj->DieRegen = FALSE;
			lpObj->State = OBJECT_CREATE;

			if (m_IllusionTempleLevel.OccupiedStone[i].byStatus == 10 ||
				m_IllusionTempleLevel.OccupiedStone[i].byStatus == 11 ||
				m_IllusionTempleLevel.OccupiedStone[i].byStatus == 100 ||
				m_IllusionTempleLevel.OccupiedStone[i].byStatus == 101 ||
				m_IllusionTempleLevel.OccupiedStone[i].byStatus == 254)
			{
				m_IllusionTempleLevel.OccupiedStone[i].byStatus = 255;

				this->GCSendStoneInfo(m_IllusionTempleLevel.OccupiedStone[i].Type, m_IllusionTempleLevel.OccupiedStone[i].byStatus, m_IllusionTempleLevel.OccupiedStone[i].Index);
				LogAdd(LOG_BLACK, "[IllusionTemple][RegenOccupiedStoneStatus] Round [%d] NpcIndex:[%d],Type:[%d] MAkE NEUTRAL", 1, m_IllusionTempleLevel.OccupiedStone[i].Index, m_IllusionTempleLevel.OccupiedStone[i].Type);
			}
		}
	}
}

void CIllusionTemple::CheckUserStoneState(LPOBJ lpObj)
{
	if (lpObj->IllusionTemple_RelicsTick > 0 || lpObj->IllusionTemple_NpcType > 0 || lpObj->IllusionTemple_StoneState != 99 || lpObj->IllusionTemple_RegisteringRelics == true || lpObj->IllusionTemple_GettingRelics == true)
	{
		int nNpcIndex = -1;
		BYTE SendState = 99;

		//LogAdd(LOG_DEBUG, "[IllusionTemple][CancelStoneState]");

		if (lpObj->IllusionTemple_NpcType == 658)
		{
			m_IllusionTempleLevel.StoneStatue.byStatus = 0;
			nNpcIndex = m_IllusionTempleLevel.StoneStatueIndex;
			SendState = m_IllusionTempleLevel.StoneStatue.byStatus;
			LogAdd(LOG_BLACK, "[IllusionTemple][CancelStoneState] State: %d", lpObj->IllusionTemple_StoneState);
		}
		else if (lpObj->IllusionTemple_NpcType >= 659 && lpObj->IllusionTemple_NpcType <= 668)
		{
			BYTE byGetRelicsCase = lpObj->IllusionTemple_StoneState % 10;
			BYTE CASE = 255;

			if (lpObj->IllusionTemple_StoneState == 10 || lpObj->IllusionTemple_StoneState == 11)
			{
				CASE = 0;
			}

			for (int i = 0; i < 10; i++)
			{
				if (m_IllusionTempleLevel.OccupiedStone[i].Type == lpObj->IllusionTemple_NpcType)
				{
					if (CASE != 0)
					{
						m_IllusionTempleLevel.OccupiedStone[i].byStatus = 255;
						SendState = m_IllusionTempleLevel.OccupiedStone[i].byStatus;
						nNpcIndex = m_IllusionTempleLevel.OccupiedStone[i].Index;
						LogAdd(LOG_BLACK, "[IllusionTemple][CancelStoneState] Type: %d State: %d", lpObj->IllusionTemple_NpcType, SendState);
						break;
					}

					if (byGetRelicsCase == 0 || byGetRelicsCase == 1)
					{
						if (byGetRelicsCase == 0)
						{
							byGetRelicsCase = 1;
						}
						else if (byGetRelicsCase == 1)
						{
							byGetRelicsCase = 0;
						}

						m_IllusionTempleLevel.OccupiedStone[i].byStatus = byGetRelicsCase;
						SendState = m_IllusionTempleLevel.OccupiedStone[i].byStatus;
						nNpcIndex = m_IllusionTempleLevel.OccupiedStone[i].Index;
						LogAdd(LOG_BLACK, "[IllusionTemple][CancelStoneState] Type: %d State: %d", lpObj->IllusionTemple_NpcType, SendState);
						break;
					}
				}
			}
		}

		this->GCSendStoneInfo(lpObj->IllusionTemple_NpcType, SendState, nNpcIndex);
		this->ResetUserFlag_DoingRelicsThing(lpObj);
	}
}

int CIllusionTemple::HaveTicket(int aIndex)
{
	int n;
	LPOBJ lpObj;

	lpObj = &gObj[aIndex];

	for (n = INVENTORY_WEAR_SIZE; n < INVENTORY_SIZE; ++n)
	{
		if (lpObj->Inventory[n].IsItem() && lpObj->Inventory[n].m_Index == GET_ITEM(13, 61))
		{
			return n;
		}
	}

	return -1;
}

void CIllusionTemple::DeleteTicket()
{
	for (int n = 0; n < MAX_IT_USER; n++)
	{
		if (OBJECT_RANGE(m_IllusionTempleLevel.User[n].Index) == 0)
		{
			continue;
		}

		int ticketSlot = this->HaveTicket(m_IllusionTempleLevel.User[n].Index);

		if (ticketSlot == -1)
		{
			this->DelUser(&m_IllusionTempleLevel, m_IllusionTempleLevel.User[n].Index);
			continue;
		}

		LPOBJ lpObj = &gObj[m_IllusionTempleLevel.User[n].Index];

		gItemManager.InventoryDelItem(lpObj->Index, ticketSlot);
		gItemManager.GCItemDeleteSend(lpObj->Index, ticketSlot, 1);
	}
}