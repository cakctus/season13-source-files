#include "stdafx.h"
#include "User.h"
#include "Util.h"
#include "LabyrinthMission.h"
#include "LabyrinthInfo.h"

#include "Labyrinth.h"
#include "RuudTokenRewardProbability.h"
#include "RheaGold.h"
#include "ItemBagManager.h"

LabyrinthMission g_LabyrinthMission;
LabyrinthMission::LabyrinthMission()
{
	this->m_mapSubMission.clear();
	this->m_vecMainMission.clear();
	NULL_MISSION.Clear();
	this->InitMission();
	//this->vfptr = (LabyrinthMissionVtbl *)&LabyrinthMission::`vftable';
}


LabyrinthMission::~LabyrinthMission()
{

	//this->vfptr = (LabyrinthMissionVtbl *)&LabyrinthMission::`vftable';
	
	this->m_mapSubMission.clear();
	this->m_vecMainMission.clear();
}


// 12CB234: using guessed type void *(__thiscall *LabyrinthMission::`vftable')(LabyrinthMission *this, unsigned int);

//----- (00EC2A70) --------------------------------------------------------
void  LabyrinthMission::InitMission()//ok
{

	//std::_Tree<std::_Tmap_traits<int, _LAB_MISSION, std::less<int>, std::allocator<std::pair<int const, _LAB_MISSION>>, 0>>::clear((std::_Tree<std::_Tmap_traits<int, _LAB_MISSION, std::less<int>, std::allocator<std::pair<int const, _LAB_MISSION> >, 0> > *)&this->m_mapSubMission._Myfirstiter);

	this->m_vecMainMission.clear();
}

//----- (00EC2AB0) --------------------------------------------------------
bool  LabyrinthMission::CheckClearMainMission( )//ok
{

	int nClearCnt; // [sp+50h] [bp-8h]@1
	nClearCnt = 0;
	for (int i = 0; i < this->m_vecMainMission.size(); i++)
	{
		if (this->m_vecMainMission[i].nMissionState==3)
		{
			++nClearCnt;
		}
	}

	return nClearCnt== this->m_vecMainMission.size();
}

//----- (00EC2B40) --------------------------------------------------------
char  LabyrinthMission::CheckClearSubMission( int aIndex, int nZoneNumber, int *nErrorCode)//ok
{
	LPOBJ lpObj; // [sp+6Ch] [bp-24h]@3
	BOOL result;
	*nErrorCode = 0;
	std::map<int, _LAB_MISSION>::iterator It = this->m_mapSubMission.find(nZoneNumber);
	if (It == this->m_mapSubMission.end())
	{
		*nErrorCode = 4;

		result= 0;
	}
	else
	{
		result= It->second.nMissionState == 3;
	}

	return result;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC2D00) --------------------------------------------------------
_LAB_MISSION * LabyrinthMission::GetSubMission( int nZoneNumber, int *nErrorCode)//ok
{
	*nErrorCode = 0;
	std::map<int, _LAB_MISSION>::iterator It = this->m_mapSubMission.find(nZoneNumber);

	if (It != this->m_mapSubMission.end())
	{
		return &It->second;
	}

	*nErrorCode = 4;
	return &this->NULL_MISSION;
		
}

//----- (00EC2E20) --------------------------------------------------------
_LAB_MISSION * LabyrinthMission::GetMainMission(int nMainMissionOrdering, int *nErrorCode)//ok
{

	*nErrorCode = 0;

	if (nMainMissionOrdering >= 0 && this->m_vecMainMission.size() - 1 >= nMainMissionOrdering)
	{
		return &this->m_vecMainMission[nMainMissionOrdering];
	}

	*nErrorCode = 3;
	return &this->NULL_MISSION;
}

//----- (00EC2EB0) --------------------------------------------------------
_LAB_MISSION * LabyrinthMission::GetProgressQuestMission(int nZoneNumber, int nZoneType, int *nErrorCode)//ok
{
	_LAB_MISSION *result; // eax@5
	_LAB_MISSION *lpLabMission; // [sp+4Ch] [bp-10h]@9
	unsigned int i; // [sp+50h] [bp-Ch]@2
	LabyrinthMission *thisa; // [sp+58h] [bp-4h]@1

	thisa = this;
	*nErrorCode = 0;
	if (nZoneType == 4)
	{
		for (i = 0; i < this->m_vecMainMission.size(); ++i)
		{
			if (this->m_vecMainMission[i].nMissionState==1)
			{
				return &this->m_vecMainMission[i];
			}

		}
		*nErrorCode = 4;
		result = &thisa->NULL_MISSION;
	}
	else if (nZoneType == 3)
	{
		lpLabMission = this->GetSubMission(nZoneNumber, nErrorCode);
		if (*nErrorCode == 4)
		{
			result = &thisa->NULL_MISSION;
		}
		else if (lpLabMission->nMissionState == 1)
		{
			result = lpLabMission;
		}
		else
		{
			*nErrorCode = 4;
			result = &thisa->NULL_MISSION;
		}
	}
	else
	{
		*nErrorCode = 4;
		result = &this->NULL_MISSION;
	}
	return result;
}

//----- (00EC2FD0) --------------------------------------------------------
void  LabyrinthMission::SendMainMissionInfo(int aIndex, int nZoneNumber)//ok
{



	int v29; // [sp+B0h] [bp-13A4h]@1
	int v30; // [sp+B4h] [bp-13A0h]@1

	BYTE dst[5000]; // [sp+C4h] [bp-1390h]@18
	LPOBJ lpObj = &gObj[aIndex];
	PMSG_LABYRINTH_MAIN_MISSION_COUNT pMsg;
	ST_LABYRINTH_MAIN_MISSION info;
	int size = sizeof(pMsg);
	pMsg.h.set(0x77, 0x06, sizeof(pMsg));

	v30 = 0;
	v29 = 0;
	for (int _Pos  = 0; _Pos < this->m_vecMainMission.size(); _Pos++)
	{
		if (!this->m_vecMainMission[_Pos].nMissionState)
		{
			v30++;
		}
		if (this->m_vecMainMission[_Pos].nMissionState == 3)
		{
			v29++;
		}
	}
	if (v30==this->m_vecMainMission.size())
	{
		this->m_vecMainMission[0].nMissionState = 1;
	}
	else if (v29!=this->m_vecMainMission.size())
	{
		for (int i = 0; i < this->m_vecMainMission.size(); i++)
		{
			if (this->m_vecMainMission[i].nMissionState != 3)
			{
				this->m_vecMainMission[i].nMissionState = 1;
				break;
			}
		}
	}
	for (int i = 0; ; i++)
	{
		if (i > this->m_vecMainMission.size())
		{
			break;
		}
		info.btMainMissionOrderingNum = this->m_vecMainMission[i].nMainMissionOrdering;

		info.btMissionState = this->m_vecMainMission[i].nMissionState;

		info.btMissionType = this->m_vecMainMission[i].nMissionType;

		info.nScoreValue = this->m_vecMainMission[i].nScoreValue;

		info.nMissionValue = this->m_vecMainMission[i].nMissionValue;

		memcpy(&dst[size], &info, sizeof(info));

		size += sizeof(info);

		pMsg.btMainMissionCount = i;

		LogAdd(LOG_BLACK,
			"[LABYRINTH] [%s][%s](%d) [MISSION INFO][MAIN %d] (ZoneNum = %d) MissionType(%d) State(%d) (%d / %d), RewardState(%d)",
			gObj[aIndex].Account,
			gObj[aIndex].Name,
			gObj[aIndex].Map,
			this->m_vecMainMission[i].nMainMissionOrdering,
			nZoneNumber,
			this->m_vecMainMission[i].nMissionType,
			this->m_vecMainMission[i].nMissionState,
			this->m_vecMainMission[i].nScoreValue,
			this->m_vecMainMission[i].nMissionValue,
			this->m_vecMainMission[i].nRewardState);
	}
	pMsg.h.size = size;
	memcpy(dst, &pMsg, sizeof(pMsg));

	DataSend(aIndex, dst, size);

	if (this->CheckClearMainMission() == 1
		&&this->m_vecMainMission[0].nRewardState!= LAB_REWARD_STATE_COMPLETE_REWARD)
	{
		PMSG_NOTIFY_LABYRINTH_RESULT pMsgNotiResult; 
		pMsgNotiResult.h.set(0x77, 0x14, sizeof(pMsgNotiResult));
		pMsgNotiResult.btLabyrinthLevel = lpObj->m_cLabyrinthInfo->GetDimensionLevel();
		pMsgNotiResult.btFailedState = 0;
		pMsgNotiResult.nEntireExp = lpObj->m_cLabyrinthInfo->GetEntireExp();
		pMsgNotiResult.nEntireMonsterKillCnt = lpObj->m_cLabyrinthInfo->GetEntireKillMonCnt();
		pMsgNotiResult.nTotalRewardRuud = 0;
		for (int i = 0; i < 5; i++)
		{
			pMsgNotiResult.nTotalRewardRuud += this->m_vecMainMission[i].nRewardValue;
		}

		pMsgNotiResult.nTotalJewelOfBless = (g_Labyrinth.GetJewelOfBlessValue(pMsgNotiResult.btLabyrinthLevel) * pMsgNotiResult.nEntireMonsterKillCnt);
		if (pMsgNotiResult.nTotalJewelOfBless > 5)
		{
			pMsgNotiResult.nTotalJewelOfBless = 5;
		}

		DataSend(aIndex, (BYTE *)&pMsgNotiResult, pMsgNotiResult.h.size);
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// EC2FD0: using guessed type char dst[5000];

//----- (00EC36D0) --------------------------------------------------------
void  LabyrinthMission::SelectRewardInfo(int aIndex, int nMainMissionOrdering)//ok
{
	LPOBJ lpObj = &gObj[aIndex];
	int nRewardRuudValue; // [sp+50h] [bp-Ch]@1

	nRewardRuudValue = 0;
	if (lpObj->m_cLabyrinthInfo->GetDimensionLevel() == LAB_DIMENSION_LV_1)
	{
		nRewardRuudValue = g_RuudTokenRewardProbability.GetSelectedRuudPoint(aIndex, 216);
	}
	else if (lpObj->m_cLabyrinthInfo->GetDimensionLevel() == LAB_DIMENSION_LV_2)
	{
		nRewardRuudValue = g_RuudTokenRewardProbability.GetSelectedRuudPoint( aIndex, 217);
	}
	else if (lpObj->m_cLabyrinthInfo->GetDimensionLevel() == LAB_DIMENSION_LV_3)
	{
		nRewardRuudValue = g_RuudTokenRewardProbability.GetSelectedRuudPoint(aIndex, 218);
	}
	else if (lpObj->m_cLabyrinthInfo->GetDimensionLevel() == LAB_DIMENSION_LV_4)
	{
		nRewardRuudValue = g_RuudTokenRewardProbability.GetSelectedRuudPoint(aIndex, 219);
	}
	else if (lpObj->m_cLabyrinthInfo->GetDimensionLevel() == LAB_DIMENSION_LV_5)
	{
		nRewardRuudValue = g_RuudTokenRewardProbability.GetSelectedRuudPoint( aIndex, 220);
	}
	else if (lpObj->m_cLabyrinthInfo->GetDimensionLevel() == LAB_DIMENSION_LV_6)
	{
		nRewardRuudValue = g_RuudTokenRewardProbability.GetSelectedRuudPoint( aIndex, 221);
	}
	else if (lpObj->m_cLabyrinthInfo->GetDimensionLevel() == LAB_DIMENSION_LV_7)
	{
		nRewardRuudValue = g_RuudTokenRewardProbability.GetSelectedRuudPoint(aIndex, 222);
	}
	if (nRewardRuudValue <= 0)
	{
		LogAdd(LOG_BLACK,
			"[LABYRINTH][SelectRewardInfo] nRewardRuudValue is invalid.[%s][%s](%d), (%d)",
			gObj[aIndex].Account,
			gObj[aIndex].Name,
			gObj[aIndex].Map,
			nRewardRuudValue);
		nRewardRuudValue = 0;
	}
	for (int i = 0; i <this->m_vecMainMission.size() ; ++i)
	{
		if (this->m_vecMainMission[i].nMainMissionOrdering == nMainMissionOrdering)
		{
			this->m_vecMainMission[i].nRewardItemType = 7453;
			this->m_vecMainMission[i].nRewardValue = nRewardRuudValue;
			this->m_vecMainMission[i].nRewardState = LAB_REWARD_STATE_SELECTED_REWARD;

			LogAdd(LOG_BLACK,
				"[LABYRINTH] [%s][%s](%d) Selected Reward Ruud value.[MainMission:%d][%d]",
				gObj[aIndex].Account,
				gObj[aIndex].Name,
				gObj[aIndex].Map,
				nMainMissionOrdering,
				nRewardRuudValue);
			if (i != this->m_vecMainMission.size() - 1)
				this->m_vecMainMission[i + 1].nMissionState = LAB_REWARD_STATE_SELECTED_REWARD;
			return;
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC3AC0) --------------------------------------------------------
unsigned int  LabyrinthMission::GetSubMissionCount()//ok
{
	return this->m_mapSubMission.size();

}

//----- (00EC3AF0) --------------------------------------------------------
unsigned int  LabyrinthMission::GetMainMissionCount()//ok
{
	return this->m_vecMainMission.size();
}

//----- (00EC3B20) --------------------------------------------------------
int  LabyrinthMission::GetTotalRewardRuud()//ok
{

	int nTotalRewardRuud; // [sp+50h] [bp-8h]@1

	nTotalRewardRuud = 0;
	for (int i = 0; i < this->m_vecMainMission.size(); ++i)
		nTotalRewardRuud += this->m_vecMainMission[i].nRewardValue;
	return nTotalRewardRuud;
}

//----- (00EC3B90) --------------------------------------------------------
int  LabyrinthMission::InsertRewardItem(int aIndex, int nTotalRuud, int nJewelOfBless)//ok
{

	if (this->m_vecMainMission[0].nRewardState != LAB_REWARD_STATE_COMPLETE_REWARD)
	{
		for (int i = 0; i < nJewelOfBless; ++i)
			switch (gObj[aIndex].m_cLabyrinthInfo->GetDimensionLevel())
			{
			case LAB_DIMENSION_LV_1:
				gItemBagManager.GremoryCaseDropItemBySpecialValue(ITEM_BGA_LABYRINTH1, GC_STORAGE_CHARACTER, GC_REWARD_LABYRITH, &gObj[aIndex], gObj[aIndex].Map, gObj[aIndex].X, gObj[aIndex].Y);
				break;
			case LAB_DIMENSION_LV_2:
				gItemBagManager.GremoryCaseDropItemBySpecialValue(ITEM_BGA_LABYRINTH2, GC_STORAGE_CHARACTER, GC_REWARD_LABYRITH, &gObj[aIndex], gObj[aIndex].Map, gObj[aIndex].X, gObj[aIndex].Y);
				break;
			case LAB_DIMENSION_LV_3:
				gItemBagManager.GremoryCaseDropItemBySpecialValue(ITEM_BGA_LABYRINTH3, GC_STORAGE_CHARACTER, GC_REWARD_LABYRITH, &gObj[aIndex], gObj[aIndex].Map, gObj[aIndex].X, gObj[aIndex].Y);
				break;
			case LAB_DIMENSION_LV_4:
				gItemBagManager.GremoryCaseDropItemBySpecialValue(ITEM_BGA_LABYRINTH4, GC_STORAGE_CHARACTER, GC_REWARD_LABYRITH, &gObj[aIndex], gObj[aIndex].Map, gObj[aIndex].X, gObj[aIndex].Y);
				break;
			case LAB_DIMENSION_LV_5:
				gItemBagManager.GremoryCaseDropItemBySpecialValue(ITEM_BGA_LABYRINTH5, GC_STORAGE_CHARACTER, GC_REWARD_LABYRITH, &gObj[aIndex], gObj[aIndex].Map, gObj[aIndex].X, gObj[aIndex].Y);
				break;
			case LAB_DIMENSION_LV_6:
				gItemBagManager.GremoryCaseDropItemBySpecialValue(ITEM_BGA_LABYRINTH6, GC_STORAGE_CHARACTER, GC_REWARD_LABYRITH, &gObj[aIndex], gObj[aIndex].Map, gObj[aIndex].X, gObj[aIndex].Y);
				break;
			case LAB_DIMENSION_LV_7:
				gItemBagManager.GremoryCaseDropItemBySpecialValue(ITEM_BGA_LABYRINTH7, GC_STORAGE_CHARACTER, GC_REWARD_LABYRITH, &gObj[aIndex], gObj[aIndex].Map, gObj[aIndex].X, gObj[aIndex].Y);
				break;
			}
		
		gObj[aIndex].RheaGold += nTotalRuud;
		gRheaGold.GD_RuudTokenCountUpdate(&gObj[aIndex], gObj[aIndex].RheaGold);
		gRheaGold.GC_Notify_RuudUpdate(aIndex, gObj[aIndex].RheaGold, nTotalRuud, 1);
		//gRheaGold.GD_ReqSetRuudTokenLog(&gObj[aIndex], gObj[aIndex].m_dwRuudCharTokenCount, nTotalRuud, 0);
		for (int _Pos = 0; _Pos < this->m_vecMainMission.size(); ++_Pos)
			this->m_vecMainMission[_Pos].nRewardState = LAB_REWARD_STATE_COMPLETE_REWARD;
		g_Labyrinth.GDReqLabyrinthRewardCompleteUpdate(aIndex, LAB_REWARD_STATE_COMPLETE_REWARD);
	}
	return 0;
}

//----- (00EC3D90) --------------------------------------------------------
void  LabyrinthMission::LoggingLabyrinthMission(int aIndex)//ok
{
	LPOBJ lpObj = &gObj[aIndex];

	for (int i = 0; i < this->m_vecMainMission.size(); ++i)
	{

		LogAdd(LOG_BLACK,
			"[LABYRINTH] [%s][%s](%d) [MISSION-LIST][MAIN %d](%d) MissionType(%d) State(%d) (%d / %d), RewardState(%d) RewardRuud(%d)",
			lpObj->Account,
			lpObj->Name,
			lpObj->Map,
			this->m_vecMainMission[i].nMainMissionOrdering,
			this->m_vecMainMission[i].nZoneNumber,
			this->m_vecMainMission[i].nMissionType,
			this->m_vecMainMission[i].nMissionState,
			this->m_vecMainMission[i].nScoreValue,
			this->m_vecMainMission[i].nMissionValue,
			this->m_vecMainMission[i].nRewardState,
			this->m_vecMainMission[i].nRewardValue);
	}
	for (std::map<int,_LAB_MISSION>::iterator it = this->m_mapSubMission.begin(); it != this->m_mapSubMission.end(); it++)
	{
		LogAdd(LOG_BLACK,
			"[LABYRINTH] [%s][%s](%d) [MISSION-LIST][SUB](%d) MissionType(%d) State(%d) (%d / %d)",
			lpObj->Account,
			lpObj->Name,
			lpObj->Map,
			it->second.nZoneNumber,
			it->second.nMissionType,
			it->second.nMissionState,
			it->second.nScoreValue,
			it->second.nMissionValue);
	}

}
//----- (00ED5B30) --------------------------------------------------------
void  LabyrinthMission::AddSubMission( _LAB_MISSION stLabMission)//ok
{

	this->m_mapSubMission.insert(std::pair<int, _LAB_MISSION>(stLabMission.nZoneNumber, stLabMission));
}

//----- (00ED5BB0) --------------------------------------------------------
void  LabyrinthMission::AddMainMission(_LAB_MISSION stLabMission)//ok
{
	this->m_vecMainMission.push_back(stLabMission);
}
