#include "stdafx.h"

#include "LabyrinthInfo.h"
#include "Labyrinth.h"
#include "Util.h"
#include "DefaultClassInfo.h"

LabyrinthInfo g_LabyrinthInfo;
LabyrinthInfo::LabyrinthInfo()
{

	//this->vfptr = (LabyrinthInfoVtbl *)&LabyrinthInfo::`vftable';
	this->m_vecZonePath.clear();
		//std::vector<int, std::allocator<int>>::vector<int, std::allocator<int>>(&this->m_vecZonePath);
	LabyrinthMission::LabyrinthMission();
	this->Init();
}


LabyrinthInfo::~LabyrinthInfo()
{

	//this->vfptr = (LabyrinthInfoVtbl *)&LabyrinthInfo::`vftable';
	this->Init();
	g_LabyrinthMission.~LabyrinthMission();
	this->m_vecZonePath.clear();
	//std::vector<int, std::allocator<int>>::~vector<int, std::allocator<int>>(&v1->m_vecZonePath);
}
void  LabyrinthInfo::AddEntireExp(QWORD nExp)//ok
{
	this->m_nEntireExp += nExp;
}
//----- (00CD72E0) --------------------------------------------------------
int  LabyrinthInfo::GetCurrentZone()//ok
{
	return this->m_nCurrentZone;
}

//----- (00CD7300) --------------------------------------------------------
int  LabyrinthInfo::GetDimensionLevel()//ok
{
	return this->m_nDimensionLevel;
}

//----- (00CD7320) --------------------------------------------------------
void  LabyrinthInfo::SetDBInfoLoad( bool bFlag)//ok
{
	this->m_bDBInfoLoad = bFlag;
}


//----- (00EC41C0) --------------------------------------------------------
void  LabyrinthInfo::AddZonePath(int nZoneNumber)//ok
{
	bool bFound; // [sp+77h] [bp-11h]@3

	if (this->m_vecZonePath.empty() == 1)
	{
		this->m_vecZonePath.push_back(nZoneNumber);

	}
	else
	{
		bFound = 0;
		for (std::vector<int>::iterator it = this->m_vecZonePath.begin();it!=this->m_vecZonePath.end();it++)
		{
			if (*it==nZoneNumber)
			{
				bFound = 1;
				break;
			}
		}
		if (!bFound)
		{
			this->m_vecZonePath.push_back(nZoneNumber);
		}
	}
}

//----- (00EC4310) --------------------------------------------------------
unsigned int  LabyrinthInfo::GetZonePathCount()//ok
{
	return   this->m_vecZonePath.size();
}

//----- (00EC4340) --------------------------------------------------------
BYTE  LabyrinthInfo::GetZonePath(BYTE idx, int *nErrorCode)//ok
{

	*nErrorCode = 0;
	if (idx >= 0 && this->m_vecZonePath.size() - 1 >= idx)
	{
		return this->m_vecZonePath[idx];
	}
	else
	{
		*nErrorCode = 8;
		return-1;
	}
	return-1;
	
}

//----- (00EC43D0) --------------------------------------------------------
void  LabyrinthInfo::SendZonePath(int aIndex)//ok
{
	int v2; // eax@3
	_LAB_ZONE *crLabZone; // [sp+70h] [bp-430h]@6
	bool bChangeGateState; // [sp+77h] [bp-429h]@4
	int nErrorCode; // [sp+78h] [bp-428h]@6
	ST_LABYRINTH_MOVE_ZONE_LIST stMoveZoneList; // [sp+7Ch] [bp-424h]@6
	BYTE DataBuffer[1000]; // [sp+84h] [bp-41Ch]@4
	int nOfs; // [sp+46Ch] [bp-34h]@4
	char btVisitedZone; // [sp+473h] [bp-2Dh]@4

	PMSG_LABYRINTH_MOVE_ZONE_LIST_COUNT pMsg; // [sp+480h] [bp-20h]@4
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnected(lpObj->Index) == 0 && lpObj->Type != OBJECT_USER)
	{
		return;
	}


	if (this->m_vecZonePath.empty()==1)
	{
		v2= this->GetConfigNum();
		LogAdd(LOG_BLACK,
			"[LABYRINTH][ERROR] SendZonePath, visited zone list is empty. [%s][%s](%d), (%d)",
			lpObj->Account,
			lpObj->Name,
			lpObj->Map,
			v2);
	}
	else
	{
		pMsg.h.set(0x77, 0x04, sizeof(pMsg));
		pMsg.btVisitedZoneCount = this->m_vecZonePath.size();
		bChangeGateState = 0;
		btVisitedZone = -1;
		nOfs = sizeof(pMsg);
		memset(DataBuffer, 0, sizeof(DataBuffer));

		for (std::vector<int>::iterator it = this->m_vecZonePath.begin(); it != this->m_vecZonePath.end(); it++)
		{
			stMoveZoneList.Clear();
			btVisitedZone = *it;
			crLabZone = g_Labyrinth.GetLabyrinthZone(this->m_nConfigNum, btVisitedZone, &nErrorCode);
			if (nErrorCode)
			{
				LogAdd(LOG_BLACK,
					"[LABYRINTH][ERROR] %s, Failed get lab zone info. [%s][%s](%d), (%d)",
					"LabyrinthInfo::SendZonePath",
					lpObj->Account,
					lpObj->Name,
					lpObj->Map,
					this->m_nConfigNum);
			}
			else
			{
				stMoveZoneList.btVisitedZone = btVisitedZone;
				stMoveZoneList.btZoneType = crLabZone->nZoneType;
				for (int i = 0; i < 4; ++i)
				{
					if (crLabZone->nZoneType == 4)
						stMoveZoneList.btGate[i] = 0;
					else
						stMoveZoneList.btGate[i] = crLabZone->btGate[i];
				}
				if (crLabZone->nZoneType == 4)
				{
					if (lpObj->m_cLabyrinthInfo->m_cLabMission.CheckClearMainMission()== 1)
						bChangeGateState = 1;
				}
				else if (crLabZone->nZoneType == 3
					&& lpObj->m_cLabyrinthInfo->m_cLabMission.CheckClearSubMission(
						aIndex,
						(unsigned __int8)btVisitedZone,
						&nErrorCode) == 1)
				{
					bChangeGateState = 1;
				}
				if (bChangeGateState == 1)
				{
					for (int j = 0; j < 4; ++j)
					{
						if (stMoveZoneList.btGate[j] == 2)
							stMoveZoneList.btGate[j] = 1;
					}
				}
					
				memcpy(&DataBuffer[nOfs], &stMoveZoneList, sizeof(stMoveZoneList));
				nOfs += sizeof(stMoveZoneList);
			}
		}
		pMsg.h.size[0] = SET_NUMBERHB(nOfs);
		pMsg.h.size[1] = SET_NUMBERLB(nOfs);

		memcpy(DataBuffer, &pMsg, sizeof(pMsg));

		DataSend(lpObj->Index, DataBuffer, nOfs);

	}
	
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC4880) --------------------------------------------------------
LabyrinthMission * LabyrinthInfo::GetLabyrinthMissionInfo()//ok
{
	return &this->m_cLabMission;
}

//----- (00EC48A0) --------------------------------------------------------
void  LabyrinthInfo::Init()//ok
{

	this->m_bPlaying = 0;
	this->m_nConfigNum = -1;
	this->m_nCurrentZone = -1;
	this->m_nDimensionLevel = 0;
	this->m_nLimitClearCnt = 0;
	this->m_btClearState = 0;
	this->m_bDBInfoLoad = 0;
	this->m_vecZonePath.clear();
	this->m_cLabMission.InitMission();
	this->m_nEntireExp = 0;
	this->m_nEntireKillMonCnt = 0;
	this->m_nLabAttackMaxValue = 1;
	this->m_nLabAttackMinValue = 1;
	this->m_nLabDefenceValue = 1;
}

//----- (00EC4980) --------------------------------------------------------
void  LabyrinthInfo::AddLabyrinthMission(int nZoneNumber, int nMissionType, int nMissionValue, int nIsMainMission, int nMainMissionOrdering)//OK
{

	_LAB_MISSION stLabMission; // [sp+4Ch] [bp-2Ch]@1

	stLabMission.Clear();

	stLabMission.nZoneNumber = nZoneNumber;

	stLabMission.nIsMainMission = nIsMainMission;

	stLabMission.nMainMissionOrdering = nMainMissionOrdering;

	stLabMission.nMissionType = nMissionType;

	stLabMission.nMissionValue = nMissionValue;

	if (nIsMainMission)
	{
		//g_LabyrinthMission.AddMainMission(stLabMission);
		this->m_cLabMission.AddMainMission(stLabMission);
	}
	else
	{
		//g_LabyrinthMission.AddSubMission(stLabMission);
		this->m_cLabMission.AddSubMission(stLabMission);
	}
}

//----- (00EC4A20) --------------------------------------------------------
void  LabyrinthInfo::SendCurrentZoneMissionInfo(int aIndex, int nZoneNumber)//OK
{
	PMSG_LABYRINTH_MISSION_INFO pMsg; // [sp+4Ch] [bp-24h]@8
	_LAB_MISSION *lpLabMission; // [sp+5Ch] [bp-14h]@6
	int nZoneType; // [sp+60h] [bp-10h]@3
	_LAB_ZONE *lpLabZone; // [sp+64h] [bp-Ch]@1
	int nRet; // [sp+68h] [bp-8h]@1

	nRet = 0;
	lpLabZone = g_Labyrinth.GetLabyrinthZone( this->m_nConfigNum, nZoneNumber, &nRet);
	if (nRet)
	{
		LogAdd(LOG_BLACK,
			"[LABYRINTH][ERROR] %s, Failed get lab zone info. [%s][%s](%d), (%d)(%d)",
			"LabyrinthInfo::SendCurrentZoneMissionInfo",
			gObj[aIndex].Account,
			gObj[aIndex].Name,
			gObj[aIndex].Map,
			this->m_nConfigNum,
			nZoneNumber);
	}
	else
	{
		nZoneType = lpLabZone->nZoneType;
		if (nZoneType == 4)
		{
			gObj[aIndex].m_cLabyrinthInfo->m_cLabMission.SendMainMissionInfo(aIndex, nZoneNumber);
		}
		else if (nZoneType == 3)
		{
			lpLabMission = gObj[aIndex].m_cLabyrinthInfo->m_cLabMission.GetSubMission( nZoneNumber, &nRet);
			if (nRet)
			{
				LogAdd(LOG_BLACK,
					"[LABYRINTH][ERROR] %s, Failed get lab mission info. [%s][%s](%d), (%d)(%d)",
					"LabyrinthInfo::SendCurrentZoneMissionInfo",
					gObj[aIndex].Account,
					gObj[aIndex].Name,
					gObj[aIndex].Map,
					this->m_nConfigNum,
					nZoneNumber);
			}
			else
			{
				pMsg.h.set(0x77, 0x03, sizeof(pMsg));

				pMsg.btMissionType = lpLabMission->nMissionType;
				if (!lpLabMission->nMissionState)
					lpLabMission->nMissionState = 1;
				pMsg.btMissionState = lpLabMission->nMissionState;
				pMsg.nScoreValue = lpLabMission->nScoreValue;
				pMsg.nMissionValue = lpLabMission->nMissionValue;
				DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
				LogAdd(LOG_BLACK,
					"[LABYRINTH] [%s][%s](%d) [MISSION INFO][SUB] (ZoneNum = %d) MissionType(%d) State(%d) (%d / %d)",
					gObj[aIndex].Account,
					gObj[aIndex].Name,
					gObj[aIndex].Map,
					nZoneNumber,
					lpLabMission->nMissionType,
					lpLabMission->nMissionState,
					lpLabMission->nScoreValue,
					lpLabMission->nMissionValue);
			}
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC4CD0) --------------------------------------------------------
bool  LabyrinthInfo::CheckMissionClear(int aIndex, int nZoneType, int nZoneNumber)//OK
{
	bool result; // al@2
	int nRet; // [sp+4Ch] [bp-8h]@4

	if (nZoneType == 4)
	{
		result = this->m_cLabMission.CheckClearMainMission();
	}
	else if (nZoneType == 3)
	{
		nRet = 0;
		result = this->m_cLabMission.CheckClearSubMission( aIndex, nZoneNumber, &nRet);
	}
	else
	{
		result = 0;
	}
	return result;
}

//----- (00EC4D40) --------------------------------------------------------
void  LabyrinthInfo::CheckPlayMission(int aIndex, int nObtainValue, int nMissionType, int nMissionCondition)//ok
{

	PMSG_NOTIFY_LABYRINTH_SUB_MISSION_FAILED pMsgFailed; // [sp+64h] [bp-70h]@45
	int v7; // [sp+68h] [bp-6Ch]@43
	int nEntranceZone; // [sp+6Ch] [bp-68h]@43

	long double fRewardJewelOfBless; // [sp+80h] [bp-54h]@35
	PMSG_NOTIFY_LABYRINTH_RESULT pMsgNotiResult; // [sp+88h] [bp-4Ch]@35
	PMSG_MAIN_MISSION_UPDATE pMsg; // [sp+A8h] [bp-2Ch]@34
	bool bFailed; // [sp+BBh] [bp-19h]@16
	_LAB_MISSION *lpLabMission; // [sp+BCh] [bp-18h]@5
	_LAB_ZONE *lpLabZone; // [sp+C0h] [bp-14h]@3
	LabyrinthInfo *lpLabInfo; // [sp+C4h] [bp-10h]@3
	OBJECTSTRUCT *lpObj; // [sp+C8h] [bp-Ch]@3
	int nRet; // [sp+CCh] [bp-8h]@1

	nRet = g_Labyrinth.CheckUser(
		aIndex,
		"LabyrinthInfo::CheckPlayMission",
		"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",
		41);
	if (!nRet && this->m_bPlaying == 1)
	{
		lpObj = &gObj[aIndex];
		//lpLabInfo = gObj[aIndex].m_cLabyrinthInfo;
		lpLabZone = g_Labyrinth.GetLabyrinthZone(this->m_nConfigNum, this->m_nCurrentZone, &nRet);
		if (nRet)
		{
			LogAdd(LOG_BLACK,
				"[LABYRINTH][ERROR] %s, [%s][%s](%d), (%d)(%d) %s %d.",
				"LabyrinthInfo::CheckPlayMission",
				lpObj->Account,
				lpObj->Name,
				lpObj->Map,
				this->m_nConfigNum,
				this->m_nCurrentZone,
				"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.cpp",
				__LINE__);
			return;
		}
		lpLabMission = this->m_cLabMission.GetProgressQuestMission(
			this->m_nCurrentZone,
			lpLabZone->nZoneType,
			&nRet);
		if (!nRet)
		{
			if (lpLabMission->nMissionType != nMissionType)
			{
				if (lpLabMission->nMissionType != 7 && lpLabMission->nMissionType != 8 || nMissionType != 1)
					return;
				if (nObtainValue + lpLabMission->nScoreValue < lpLabMission->nMissionValue)
				{
					lpLabMission->nScoreValue += nObtainValue;
				}
				else
				{
					lpLabMission->nMissionState = 3;
					lpLabMission->nScoreValue = lpLabMission->nMissionValue;
					g_Labyrinth.GDReqLabyrinthMissionInfoUpdate(aIndex, lpLabMission);
				}
				LogAdd(LOG_BLACK,
					"[LABYRINTH] [%s][%s](%d) Labyrinth Check Playing Mission [Zone:%d (%d-%d)] [MissionType:%d] (%d / %d)(Complete:3=%d)",
					lpObj->Account,
					lpObj->Name,
					lpObj->Map,
					this->m_nCurrentZone,
					lpLabMission->nIsMainMission,
					lpLabMission->nMainMissionOrdering,
					lpLabMission->nMissionType,
					lpLabMission->nScoreValue,
					lpLabMission->nMissionValue,
					lpLabMission->nMissionState);
			}
			bFailed = 0;
			switch (nMissionType)
			{
			case LAB_MISSION_MONSTER_KILL:
				if (lpLabMission->nMissionType == nMissionType)
				{
					if (nObtainValue + lpLabMission->nScoreValue < lpLabMission->nMissionValue)
					{
						lpLabMission->nScoreValue += nObtainValue;
					}
					else
					{
						lpLabMission->nMissionState = 3;
						lpLabMission->nScoreValue = lpLabMission->nMissionValue;
						g_Labyrinth.GDReqLabyrinthMissionInfoUpdate( aIndex, lpLabMission);
					}
					LogAdd(LOG_BLACK,
						"[LABYRINTH] [%s][%s](%d) Labyrinth Check Playing Mission [Zone:%d (%d-%d)] [MissionType:%d] (%d / %d)(Complete:3=%d)",
						lpObj->Account,
						lpObj->Name,
						lpObj->Map,
						this->m_nCurrentZone,
						lpLabMission->nIsMainMission,
						lpLabMission->nMainMissionOrdering,
						lpLabMission->nMissionType,
						lpLabMission->nScoreValue,
						lpLabMission->nMissionValue,
						lpLabMission->nMissionState);
				}
				break;
			case LAB_MISSION_GATHERING_ZEN:
			case LAB_MISSION_DAMAGE:
			case LAB_MISSION_EXP:
			case LAB_MISSION_ASSAULTED_DAMAGE:
			case LAB_MISSION_MACRO_TIME:
				if (nObtainValue + lpLabMission->nScoreValue < lpLabMission->nMissionValue)
				{
					lpLabMission->nScoreValue += nObtainValue;
				}
				else
				{
					lpLabMission->nMissionState = 3;
					lpLabMission->nScoreValue = lpLabMission->nMissionValue;
					g_Labyrinth.GDReqLabyrinthMissionInfoUpdate(aIndex, lpLabMission);
				}
				LogAdd(LOG_BLACK,
					"[LABYRINTH] [%s][%s](%d) Labyrinth Check Playing Mission [Zone:%d (%d-%d)] [MissionType:%d] (%d / %d)(Complete:3=%d)",
					lpObj->Account,
					lpObj->Name,
					lpObj->Map,
					this->m_nCurrentZone,
					lpLabMission->nIsMainMission,
					lpLabMission->nMainMissionOrdering,
					lpLabMission->nMissionType,
					lpLabMission->nScoreValue,
					lpLabMission->nMissionValue,
					lpLabMission->nMissionState);
				break;
			case LAB_MISSION_NOT_USE_HEALTH_POTION:
			case LAB_MISSION_NOT_USE_MANA_POTION:
				if (lpLabMission->nMissionState != 3)
				{
					lpLabMission->nScoreValue = 0;
					bFailed = 1;
					g_Labyrinth.GDReqLabyrinthMissionInfoUpdate( aIndex, lpLabMission);
					LogAdd(LOG_BLACK,
						"[LABYRINTH] [%s][%s](%d) Mission Fail. (Using healthy / mana potion) [Zone:%d] [MissionType:%d]",
						lpObj->Account,
						lpObj->Name,
						lpObj->Map,
						this->m_nCurrentZone,
						lpLabMission->nMissionType);
				}
				break;
			default:
				LogAdd(LOG_BLACK,
					"[LABYRINTH][ERROR] %s, [%s][%s](%d), (%d)(%d) %s %d. ",
					"LabyrinthInfo::CheckPlayMission",
					lpObj->Account,
					lpObj->Name,
					lpObj->Map,
					this->m_nConfigNum,
					this->m_nCurrentZone,
					"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.cpp",
					__LINE__);
				break;
			}
			if (lpLabZone->nZoneType == 4)
			{
				if (lpLabMission->nMissionState == 3)
				{
					this->m_cLabMission.SelectRewardInfo( aIndex, lpLabMission->nMainMissionOrdering);
					g_Labyrinth.GDReqLabyrinthMissionInfoUpdate(aIndex, lpLabMission);
				}
				pMsg.h.set(0x77, 0x07, sizeof(pMsg));

				pMsg.btMainMissionOrderingNum = lpLabMission->nMainMissionOrdering;
				pMsg.btMissionState = lpLabMission->nMissionState;
				pMsg.btMissionType = lpLabMission->nMissionType;
				pMsg.nMissionValue = lpLabMission->nMissionValue;
				pMsg.nScoreValue = lpLabMission->nScoreValue;
				DataSend(aIndex, (BYTE *)&pMsg,pMsg.h.size);
				if (this->m_cLabMission.CheckClearMainMission() == 1)
				{
					pMsgNotiResult.h.set(0x77, 0x14, sizeof(pMsg));

					pMsgNotiResult.btLabyrinthLevel = this->m_nDimensionLevel;
					pMsgNotiResult.btFailedState = 0;
					pMsgNotiResult.nEntireExp = this->m_nEntireExp;
					pMsgNotiResult.nEntireMonsterKillCnt = this->m_nEntireKillMonCnt;
					pMsgNotiResult.nTotalRewardRuud = this->m_cLabMission.GetTotalRewardRuud();

					fRewardJewelOfBless = g_Labyrinth.GetJewelOfBlessValue(this->m_nDimensionLevel) * this->m_nEntireKillMonCnt;
					pMsgNotiResult.nTotalJewelOfBless = (signed __int64)(fRewardJewelOfBless / 100.0);
					LogAdd(LOG_BLACK,
						"[LABYRINTH] [%s][%s](%d) Clear Labyrinth Dimension Level(%d). [%d] (Exp: %I64d)(Kill: %I64d) Reward Ruud=%d, Jewel=%d",
						lpObj->Account,
						lpObj->Name,
						lpObj->Map,
						this->m_nDimensionLevel,
						this->m_nCurrentZone,
						SET_NUMBERLDW(this->m_nEntireExp),
						SET_NUMBERHDW(this->m_nEntireExp),
						SET_NUMBERLDW(this->m_nEntireKillMonCnt),
						SET_NUMBERHDW(this->m_nEntireKillMonCnt),
						pMsgNotiResult.nTotalRewardRuud,
						pMsgNotiResult.nTotalJewelOfBless);
					if (pMsgNotiResult.nTotalJewelOfBless > 5)
					{
						LogAdd(LOG_BLACK,
							"[LABYRINTH] [%s][%s](%d) Exceed Maximum Reward Jewel of bless. Modifying.. (%d) -> 5",
							lpObj->Account,
							lpObj->Name,
							lpObj->Map,
							pMsgNotiResult.nTotalJewelOfBless);
						pMsgNotiResult.nTotalJewelOfBless = 5;
					}
					DataSend(aIndex, (BYTE *)&pMsgNotiResult, pMsgNotiResult.h.size);
					this->m_cLabMission.InsertRewardItem(
						aIndex,
						pMsgNotiResult.nTotalRewardRuud,
						pMsgNotiResult.nTotalJewelOfBless);
					g_Labyrinth.GDReqLabyrinthClearLogSave(aIndex, this->m_nDimensionLevel);
					if (this->m_nDimensionLevel == 7)
					{
						this->m_btClearState = 1;
						++this->m_nLimitClearCnt;
						g_Labyrinth.GDReqLabyrinthEndUpdate( aIndex);
					}
				}
			}
			else if (lpLabZone->nZoneType == 3)
			{
				PMSG_LABYRINTH_MISSION_INFO pMsgLabyMissIonInfo;
				pMsgLabyMissIonInfo.h.set(0x77, 0x03, sizeof(pMsgLabyMissIonInfo));

				pMsgLabyMissIonInfo.btMissionState = lpLabMission->nMissionState;
				pMsgLabyMissIonInfo.btMissionType = lpLabMission->nMissionType;
				pMsgLabyMissIonInfo.nMissionValue = lpLabMission->nMissionValue;
				pMsgLabyMissIonInfo.nScoreValue = lpLabMission->nScoreValue;

				DataSend(aIndex, (BYTE *)&pMsgLabyMissIonInfo, pMsgLabyMissIonInfo.h.size);
			}
			if (bFailed == 1)
			{
				nEntranceZone = -1;
				v7 = g_Labyrinth.FindEntranceZone(this->m_nConfigNum, &nEntranceZone);
				if (v7)
				{
					LogAdd(LOG_BLACK,
						"[LABYRINTH][ERROR] %s, %d,",
						"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.cpp",
						__LINE__);
				}
				else
				{
					g_Labyrinth.MoveLabyrinthZone(aIndex, this->m_nConfigNum, nEntranceZone);
					this->SendCurrentZoneMissionInfo( aIndex, nEntranceZone);
					this->SendZonePath( aIndex);
					pMsgFailed.h.set(0x77, 0x16, sizeof(pMsgFailed));

					DataSend(aIndex, (BYTE *)&pMsgFailed, pMsgFailed.h.size);
				}
			}
		}
	}
}
// 13AD858: using guessed type int `LabyrinthInfo::CheckPlayMission'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC5720) --------------------------------------------------------
//----- (00BA67E0) --------------------------------------------------------
void  LabyrinthInfo::AddEntireKillMonCnt( int nMonterKillCnt)//ok
{
	this->m_nEntireKillMonCnt += nMonterKillCnt;
}
int  LabyrinthInfo::MoveDimensionLevel(int aIndex)//ok
{
	_LAB_ZONE_CONFIGURATION *LabZoneConfing; // esi@2
	int nMainZoneNumber; // [sp+4Ch] [bp-14h]@3
	int nMoveZoneNumber; // [sp+50h] [bp-10h]@3
	int nErrChkFlag; // [sp+54h] [bp-Ch]@1
	int nLabConfigNum; // [sp+58h] [bp-8h]@2

	this->m_nEntireExp = 0;

	this->m_nEntireKillMonCnt = 0;
	this->m_nEntireKillMonCnt = 0;
	this->m_cLabMission.InitMission();
	g_Labyrinth.GDReqLabyrinthMissionDelete(aIndex);
	nErrChkFlag = 0;
	for (std::vector<int>::iterator it = this->m_vecZonePath.begin(); it != this->m_vecZonePath.end(); it++)
	{
		nLabConfigNum = rand() % g_Labyrinth.m_vecZoneConfig.size();

		LabZoneConfing = &g_Labyrinth.m_vecZoneConfig[nLabConfigNum];
		if (LabZoneConfing->nLabDimensionLevel == this->GetDimensionLevel() + 1)
		{
			break;
		}
		if (!(++nErrChkFlag % 2000))
		{
			LogAdd(LOG_BLACK,
				"[LABYRINTH][WARN] %s, while loop... in selection configure.(aIndex: %d, dim lv: %d)",
				"LabyrinthInfo::MoveDimensionLevel",
				aIndex,
				this->m_nDimensionLevel);
			nErrChkFlag = 0;
		}
	}


	this->SetConfigNum(nLabConfigNum);
	g_Labyrinth.AddLabyrinthMissionDataSet( aIndex, nLabConfigNum);
	++this->m_nDimensionLevel;
	nMoveZoneNumber = -1;
	nMainZoneNumber = -1;
	g_Labyrinth.FindEntranceZone(nLabConfigNum, &nMoveZoneNumber);
	LogAdd(LOG_BLACK,
		"[LABYRINTH] [%s][%s](%d) Move the labyrinth (Dimension Level UP). (%d)(%d)",
		gObj[aIndex].Account,
		gObj[aIndex].Name,
		gObj[aIndex].Map,
		nLabConfigNum,
		this->m_nDimensionLevel);
	g_Labyrinth.LoggingLabyrinthInfo(aIndex);
	g_Labyrinth.MoveLabyrinthZone(aIndex, nLabConfigNum, nMoveZoneNumber);
	this->SendCurrentZoneMissionInfo(aIndex, nMoveZoneNumber);
	g_Labyrinth.FindMainMissionZone(nLabConfigNum, &nMainZoneNumber);
	this->AddZonePath(nMainZoneNumber);
	this->SendZonePath(aIndex);
	g_Labyrinth.GDReqLabyrinthInfoSave(aIndex);
	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC59B0) --------------------------------------------------------
void  LabyrinthInfo::CheckLabyrinthUserAbility(int  aIndex)//ok
{
	int nDefenceValue; // [sp+50h] [bp-20h]@31
	int nAttackMinValue; // [sp+54h] [bp-1Ch]@0
	int nAttackMaxValue; // [sp+58h] [bp-18h]@0
	int bTwoHandWeapon; // [sp+5Ch] [bp-14h]@2
	CItem *Right; // [sp+64h] [bp-Ch]@2

	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj)
	{
		LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
		Right = lpObj->Inventory;
		bTwoHandWeapon = 0;
		if ((lpObj->Class == CLASS_DL || lpObj->Class == CLASS_DK || lpObj->Class == CLASS_MG || lpObj->Class == CLASS_RF)
			&& Right->m_Index >= 0
			&& Right->m_Index < 2048
			&& lpObj->Inventory[1].m_Index >= 0
			&& lpObj->Inventory[1].m_Index < 2048
			&& Right->m_IsValidItem
			&& lpObj->Inventory[1].m_IsValidItem)
		{
			bTwoHandWeapon = 1;
		}
		switch (lpObj->Class)
		{
		case CLASS_DW:
		case CLASS_SU:
			nAttackMaxValue = lpObj->MagicDamageMax;
			nAttackMinValue = lpObj->MagicDamageMin;
			break;
		case CLASS_DK:
		case CLASS_FE:
		case CLASS_DL:
		case CLASS_RF:
		case CLASS_FK:
			if (bTwoHandWeapon == 1)
			{
				nAttackMaxValue = lpObj->PhysiDamageMaxLeft+ lpObj->PhysiDamageMaxRight;
				nAttackMinValue = lpObj->PhysiDamageMinLeft + lpObj->PhysiDamageMinRight;
			}
			else if (lpObj->PhysiDamageMaxRight < lpObj->PhysiDamageMaxLeft)
			{
				nAttackMaxValue = lpObj->PhysiDamageMaxLeft;
				nAttackMinValue = lpObj->PhysiDamageMinLeft;
			}
			else
			{
				nAttackMaxValue = lpObj->PhysiDamageMaxRight;
				nAttackMinValue = lpObj->PhysiDamageMinRight;
			}
			break;
		case CLASS_MG:
			if (lpObj->PhysiDamageMaxLeft > lpObj->MagicDamageMax
				|| lpObj->PhysiDamageMaxRight > lpObj->MagicDamageMax)
			{
				if (bTwoHandWeapon == 1)
				{
					nAttackMaxValue = lpObj->PhysiDamageMaxLeft + lpObj->PhysiDamageMaxRight;
					nAttackMinValue = lpObj->PhysiDamageMinLeft + lpObj->PhysiDamageMinRight;
				}
				else if (lpObj->PhysiDamageMaxRight < lpObj->PhysiDamageMaxLeft)
				{
					nAttackMaxValue = lpObj->PhysiDamageMaxLeft;
					nAttackMinValue = lpObj->PhysiDamageMinLeft;
				}
				else
				{
					nAttackMaxValue = lpObj->PhysiDamageMaxRight;
					nAttackMinValue = lpObj->PhysiDamageMinRight;
				}
			}
			else
			{
				nAttackMaxValue = lpObj->MagicDamageMax;
				nAttackMinValue = lpObj->MagicDamageMin;
			}
			break;
		default:
			LogAdd(LOG_BLACK,
				"[LABYRINTH][ERROR] It's unknown class value.[%s][%s](%d), (%d)",
				lpObj->Account,
				lpObj->Name,
				lpObj->Map,
				lpObj->Class);
			lpLabInfo->m_nLabAttackMaxValue = 1;
			lpLabInfo->m_nLabDefenceValue = 1;
			break;
		}
		nDefenceValue = 2 * lpObj->Defense;
		if ((double)nAttackMaxValue <= 1.0)
			nAttackMaxValue = 1;
		if ((double)nAttackMinValue <= 1.0)
			nAttackMinValue = 1;
		if ((double)nDefenceValue <= 1.0)
			nDefenceValue = 1;
		this->m_nLabAttackMaxValue = nAttackMaxValue;
		this->m_nLabAttackMinValue = nAttackMinValue;
		this->m_nLabDefenceValue = nDefenceValue;
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
//----- (00ED5CD0) --------------------------------------------------------
void  LabyrinthInfo::SetPlay(bool bFlag)//ok
{
	this->m_bPlaying = bFlag;
}

//----- (00ED5D00) --------------------------------------------------------
void  LabyrinthInfo::SetConfigNum(int nConfigNum)//ok
{
	this->m_nConfigNum = nConfigNum;
}

//----- (00ED5D30) --------------------------------------------------------
void  LabyrinthInfo::SetCurrentZone(int nCurrentZone)//ok
{
	this->m_nCurrentZone = nCurrentZone;
}

//----- (00ED5D60) --------------------------------------------------------
void  LabyrinthInfo::SetDimensionLevel(int nDimensionLevel)//ok
{
	this->m_nDimensionLevel = nDimensionLevel;
}

//----- (00ED5D90) --------------------------------------------------------
void  LabyrinthInfo::SetLimitClearCnt(int nLimitClearCnt)//ok
{
	this->m_nLimitClearCnt = nLimitClearCnt;
}

//----- (00ED5DC0) --------------------------------------------------------
int  LabyrinthInfo::GetLimitClearCnt()//ok
{
	return this->m_nLimitClearCnt;
}

//----- (00ED5DE0) --------------------------------------------------------
void  LabyrinthInfo::SetClearState(BYTE btClearState)//ok
{
	this->m_btClearState = btClearState;
}

//----- (00ED5E10) --------------------------------------------------------
char  LabyrinthInfo::GetClearState()//ok
{
	return this->m_btClearState;
}

//----- (00ED5E30) --------------------------------------------------------
bool  LabyrinthInfo::GetDBInfoLoad()//ok
{
	return this->m_bDBInfoLoad;
}

//----- (00ED5E50) --------------------------------------------------------
unsigned __int64  LabyrinthInfo::GetEntireKillMonCnt()//ok
{
	return this->m_nEntireKillMonCnt;
}

//----- (00ED5E80) --------------------------------------------------------
void  LabyrinthInfo::SetEntireKillMonCnt(unsigned __int64 nMonterKillCnt)//ok
{
	this->m_nEntireKillMonCnt = nMonterKillCnt;
}

//----- (00ED5EB0) --------------------------------------------------------
unsigned __int64  LabyrinthInfo::GetEntireExp()//ok
{
	return this->m_nEntireExp;
}

//----- (00ED5EE0) --------------------------------------------------------
void  LabyrinthInfo::SetEntireExp(unsigned __int64 nExp)//ok
{
	this->m_nEntireExp = nExp;
}
int  LabyrinthInfo::GetConfigNum()
{
	return this->m_nConfigNum;
}
//----- (00BD18D0) --------------------------------------------------------
bool  LabyrinthInfo::IsPlaying()//ok
{
	return this->m_bPlaying;
}