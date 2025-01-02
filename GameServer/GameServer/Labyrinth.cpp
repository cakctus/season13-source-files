#include "stdafx.h"

#include "Labyrinth.h"
#include "Util.h"
#include "EffectManager.h"
#include "GameMain.h"
#include "MemScript.h"
#include "MonsterManager.h"
#include "MonsterSetBase.h"
#include "Monster.h"
#include "Viewport.h"
#include "Map.h"
#include "InstanceCounter.h"
#include "InstanceObject.h"
#include "InstanceSystemInfo.h"
#include "InstanceSystemMgr.h"


Labyrinth g_Labyrinth;
Labyrinth::Labyrinth()
{
	this->m_vecMapInfo.clear();
	this->m_vecZoneTrait.clear();
	this->m_vecZoneConfig.clear();

	this->m_mapRebalanceValue.clear();
	NULL_LAB_ZONE.Clear();
}


Labyrinth::~Labyrinth()
{

}

//----- (00EBD0D0) --------------------------------------------------------
void  Labyrinth::LoadScript(  char *szFileName)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, szFileName);
		return;
	}

	if (lpMemScript->SetBuffer(szFileName) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 1)
				{
					while (true)
					{
						if (strcmp("end", lpMemScript->GetAsString()) == 0)
						{
							break;
						}
						_LAB_REBALANCING_CONST_VALUE stRebalancing;

						int nDimensionLevel = lpMemScript->GetNumber();

						stRebalancing.fHP = lpMemScript->GetAsFloatNumber();

						stRebalancing.fDamageMin = lpMemScript->GetAsFloatNumber();

						stRebalancing.fDamageMax = lpMemScript->GetAsFloatNumber();

						stRebalancing.fDefence = lpMemScript->GetAsFloatNumber();

						stRebalancing.fExpLevel = lpMemScript->GetAsFloatNumber();

						stRebalancing.fJewelOfBless = lpMemScript->GetAsFloatNumber();

						this->m_mapRebalanceValue.insert(std::pair<int, _LAB_REBALANCING_CONST_VALUE>(nDimensionLevel, stRebalancing));
					}
					break;
				}
				else if (section == 2)
				{
					while (true)
					{
						if (strcmp("end", lpMemScript->GetAsString()) == 0)
						{
							break;
						}
						_LAB_ZONE_TRAIT stLabZoneTrait;
						stLabZoneTrait.nZoneType = lpMemScript->GetNumber();
						stLabZoneTrait.nZoneIndex = lpMemScript->GetAsNumber();
						for (int i = 0; i < 6; ++i)
						{
							stLabZoneTrait.stMissionTrait[i].nMissionType = lpMemScript->GetAsNumber();

							stLabZoneTrait.stMissionTrait[i].nMissionValue = lpMemScript->GetAsNumber();
						}
						int nMonsterClass = -1;
						int nMonsterCount = -1;
						for (int j = 0; j < 3; ++j)
						{
							nMonsterClass = lpMemScript->GetAsNumber();

							nMonsterCount = lpMemScript->GetAsNumber();
							if (nMonsterClass != -1 && nMonsterCount != -1)
							{
								stLabZoneTrait.mapMonsterGen.insert(std::pair<int, int>(nMonsterClass, nMonsterCount));
							}
						}
						this->m_vecZoneTrait.push_back(stLabZoneTrait);
					}	
					break;
				}
				else if (section == 3)
				{
					BOOL bEndCondition = 0;
					while (true)
					{
						if (strcmp("end", lpMemScript->GetAsString()) == 0)
						{
							break;
						}
						_LAB_ZONE_CONFIGURATION stLabZoneConfig;

						stLabZoneConfig.nLabConfigNum = lpMemScript->GetNumber();

						stLabZoneConfig.nLabDimensionLevel = lpMemScript->GetAsNumber();

						stLabZoneConfig.nRow = lpMemScript->GetAsNumber();

						stLabZoneConfig.nCol = lpMemScript->GetAsNumber();
						bEndCondition = 0;

						for ( int y = 0; y <= stLabZoneConfig.nRow; ++y)
						{
							for ( int x = 0; x <= stLabZoneConfig.nCol; ++x)
							{
								if (strcmp("end", lpMemScript->GetAsString()) == 0)
								{
									bEndCondition = 1;
									break;
								}
								int nRow = lpMemScript->GetNumber();

								int nCol = lpMemScript->GetAsNumber();

								int nZoneNumber = nCol + stLabZoneConfig.nCol * nRow;

								stLabZoneConfig.stZone[nCol + stLabZoneConfig.nCol * nRow].nRow = nRow;

								stLabZoneConfig.stZone[nZoneNumber].nCol = nCol;

								int nLabIndex = lpMemScript->GetAsNumber();

								int nZoneType = lpMemScript->GetAsNumber();

								int nZoneIndex = lpMemScript->GetAsNumber();

								stLabZoneConfig.stZone[nZoneNumber].nLabIndex = nLabIndex;

								stLabZoneConfig.stZone[nZoneNumber].nZoneType = nZoneType;

								stLabZoneConfig.stZone[nZoneNumber].nZoneIndex = nZoneIndex;

								for (int k = 0; k < 4; ++k)
								{
									BYTE btGateState = lpMemScript->GetAsNumber();
									stLabZoneConfig.stZone[nZoneNumber].btGate[k] = btGateState;
								}
							}
							if (bEndCondition == 1)
								break;
						}
						if (bEndCondition == 1)
							this->m_vecZoneConfig.push_back(stLabZoneConfig);
					}					
					break;
				}
				else
				{
					while (true)
					{
						if (strcmp("end", lpMemScript->GetAsString()) == 0)
						{
							break;
						}
						_LAB_MAP_INFO stLabMapInfo;

						stLabMapInfo.nLabIndex = lpMemScript->GetNumber();

						stLabMapInfo.wLabMapNumber = lpMemScript->GetAsNumber();

						stLabMapInfo.sX = lpMemScript->GetAsNumber();

						stLabMapInfo.sY = lpMemScript->GetAsNumber();

						stLabMapInfo.eX = lpMemScript->GetAsNumber();

						stLabMapInfo.eY = lpMemScript->GetAsNumber();

						this->m_vecMapInfo.push_back(stLabMapInfo);
					}
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

}

//----- (00EBDC30) --------------------------------------------------------
_LAB_ZONE * Labyrinth::GetLabyrinthZone(int nLabConfigNum, int nZoneNumber, int *nErrorCode)//OK
{
	_LAB_ZONE *result; // eax@2
	int nCol; // [sp+4Ch] [bp-Ch]@3
	int nRow; // [sp+50h] [bp-8h]@3

	*nErrorCode = 0;
	if (nLabConfigNum < this->m_vecZoneConfig.size())
	{
		nRow = this->m_vecZoneConfig[nLabConfigNum].nRow;

		nCol = this->m_vecZoneConfig[nLabConfigNum].nCol;

		if (nZoneNumber < nCol * nRow)
		{
			result = &this->m_vecZoneConfig[nLabConfigNum].stZone[nZoneNumber];
		}
		else
		{
			LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] zone num is over the MAX.(%d)(MAX:%d)", nZoneNumber, nCol * nRow);
			*nErrorCode = 3;
			result = &this->NULL_LAB_ZONE;
		}
	}
	else
	{
		LogAdd(LOG_BLACK, "[LABYRINTH][ERROR] config num is over the MAX.(%d)", nLabConfigNum);
		*nErrorCode = 3;
		result = &this->NULL_LAB_ZONE;
	}
	return result;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EBDD30) --------------------------------------------------------
double  Labyrinth::GetJewelOfBlessValue(int nDimensionLevel)//OK
{
	std::map<int, _LAB_REBALANCING_CONST_VALUE>::iterator it = this->m_mapRebalanceValue.find(nDimensionLevel);
	if (it==this->m_mapRebalanceValue.end())
	{
		return it->second.fJewelOfBless;
	}
	else
	{
		LogAdd(LOG_BLACK,"[LABYRINTH][ERROR], not found, Rebalance info. (%d)", nDimensionLevel);
		return 0;
	}
	
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EBDE50) --------------------------------------------------------
int  Labyrinth::LeaveLabyrinth(int aIndex, int nLabConfigNum)
{
	int result; // eax@2
	_LAB_MISSION *lpLabMission; // [sp+4Ch] [bp-124h]@11
	_LAB_ZONE *lpLabZone; // [sp+54h] [bp-11Ch]@9
	int nInstanceIndex; // [sp+15Ch] [bp-14h]@3
	LPOBJ lpObj; // [sp+160h] [bp-10h]@3
	int nRet; // [sp+164h] [bp-Ch]@1
	if (gObjIsConnected(aIndex) == 0)
	{
		return 0;
	}

	nRet = this->CheckUser(
		aIndex,
		"Labyrinth::LeaveLabyrinth",
		"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",
		41);
	if (nRet)
	{
		LogAdd(LOG_BLACK, "[LABYRINTH][ERROR] %s, Not User.", "Labyrinth::LeaveLabyrinth");
		result = 6;
	}
	else
	{
		lpObj = &gObj[aIndex];
		LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
		nInstanceIndex = -1;
		nRet = g_InstanceSystemInfo.ResolutionInstanceIndex(gObj[aIndex].Map, &nInstanceIndex);
		if (nRet)
		{
			LogAdd(LOG_BLACK,
				"[LABYRINTH][ERROR] %s, Can't translate instance index. [%s][%s](%d)",
				"Labyrinth::LeaveLabyrinth",
				lpObj->Account,
				lpObj->Name,
				lpObj->Map);
			result = nRet;
		}
		else if (lpLabInfo->IsPlaying())
		{
			nRet = g_InstanceSystemMgr->RequestFreeInstanceResource(nInstanceIndex, aIndex, 1); 
			if (nRet)
			{
				LogAdd(LOG_BLACK,
					"[LABYRINTH][ERROR] %s, %s %d",
					"Labyrinth::LeaveLabyrinth",
					"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.cpp",
					__LINE__);
				result = nRet;
			}
			else
			{

				//memset(szMsg, 0, sizeof(szMsg));

				//_wsprintfA(szMsg, "차원의미궁 정보가 저장되어 재입장 시, 이어서 진행 가능합니다.");

				//GCServerMsgStringSend(szMsg, lpObj->Index, 1);
				this->GDReqLabyrinthInfoUpdate(lpObj->Index);

				lpLabZone = this->GetLabyrinthZone(lpLabInfo->GetConfigNum(), lpLabInfo->GetCurrentZone(), &nRet);
				if (nRet)
				{
					LogAdd(LOG_BLACK,
						"[LABYRINTH][ERROR] %s, Can't find Labyrinth zone info.[%s][%s](%d)(%d)",
						"Labyrinth::LeaveLabyrinth",
						lpObj->Account,
						lpObj->Name,
						lpLabInfo->GetConfigNum(),
						lpLabInfo->GetCurrentZone());
					lpLabInfo->SetPlay( 0);
					this->LoggingLabyrinthMissionInfo(aIndex);
					result = nRet;
				}
				else
				{
					LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s] Leave Labyrinth. (Close, OK!)", lpObj->Account, lpObj->Name);


					lpLabMission = lpLabInfo->m_cLabMission.GetProgressQuestMission(lpLabInfo->GetCurrentZone(), lpLabZone->nZoneType, &nRet);
					if (nRet)
					{

						LogAdd(LOG_BLACK,
							"[LABYRINTH] %s, Can't find Labyrinth quest mission info.(It's not mission zone) [%s][%s](%d)(%d)",
							"Labyrinth::LeaveLabyrinth",
							lpObj->Account,
							lpObj->Name,
							lpLabInfo->GetConfigNum(),
							lpLabInfo->GetCurrentZone());
						lpLabInfo->SetPlay( 0);
						this->LoggingLabyrinthMissionInfo( aIndex);
						result = nRet;
					}
					else
					{
						this->GDReqLabyrinthMissionInfoUpdate( lpObj->Index, lpLabMission);
						lpLabInfo->SetPlay( 0);
						this->LoggingLabyrinthMissionInfo(aIndex);
						result = 0;
					}
				}
			}
		}
		else
		{
			LogAdd(LOG_BLACK,"[LABYRINTH][WARN] Already leaved user.[%s][%s](%d)", lpObj->Account, lpObj->Name, lpObj->Map);
			result = 0;
		}
	}
	return result;
}
// 13AD808: using guessed type int `Labyrinth::LeaveLabyrinth'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EBE270) --------------------------------------------------------
int  Labyrinth::ReBalanceMonster( LPOBJ lpObj, LPOBJ lpMonsterObj)//OK
{
	int nExpLevel; // [sp+7Ch] [bp-58h]@4
	unsigned int nDefence; // [sp+80h] [bp-54h]@4
	unsigned int nPrevDefence; // [sp+84h] [bp-50h]@4
	unsigned int nAttackMaxDamage; // [sp+88h] [bp-4Ch]@4
	unsigned int nAttackMinDamage; // [sp+8Ch] [bp-48h]@4
	unsigned int nPrevAttackMax; // [sp+90h] [bp-44h]@4
	unsigned int nMaxLife; // [sp+94h] [bp-40h]@4

	float fRe_ExpLevel; // [sp+A4h] [bp-30h]@1
	float fRe_Defence; // [sp+A8h] [bp-2Ch]@1
	float fRe_DmgMax; // [sp+ACh] [bp-28h]@1
	float fRe_DmgMin; // [sp+B0h] [bp-24h]@1
	float fRe_HP; // [sp+B4h] [bp-20h]@1
	int nUserLevel; // [sp+B8h] [bp-1Ch]@1
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;

	int idx = lpLabInfo->GetDimensionLevel();

	nUserLevel = lpObj->MasterLevel + lpObj->Level+lpObj->ForMasterLevel;
	fRe_HP = 1.0;
	fRe_DmgMin = 1.0;
	fRe_DmgMax = 1.0;
	fRe_Defence = 1.0;
	fRe_ExpLevel = 1.0;
	std::map<int, _LAB_REBALANCING_CONST_VALUE>::iterator it = this->m_mapRebalanceValue.find(idx);
	if (it== this->m_mapRebalanceValue.end())
	{

		LogAdd(LOG_BLACK,
			"[LABYRINTH][ERROR] [%s][%s](%d) Not found, monster rebalance value.(Dimension Lv:%d)",
			lpObj->Account,
			lpObj->Name,
			lpObj->Map,
			lpLabInfo->GetDimensionLevel());
	}
	else
	{
		fRe_HP = it->second.fHP;
		fRe_DmgMin = it->second.fDamageMin;
		fRe_DmgMax = it->second.fDamageMax;
		fRe_Defence = it->second.fDefence;
		fRe_ExpLevel = it->second.fExpLevel;

	}
	
	nMaxLife = (signed __int64)((double)(unsigned int)(signed __int64)((double)nUserLevel * lpMonsterObj->MaxLife
		+ (double)(lpLabInfo->m_nLabAttackMinValue >> 1)
		* lpMonsterObj->MaxLife)
		* fRe_HP);
	nPrevAttackMax = lpMonsterObj->PhysiDamageMax  * (lpLabInfo->m_nLabDefenceValue / 0x14) + nUserLevel / 2;
	nAttackMinDamage = (signed __int64)((double)(lpMonsterObj->PhysiDamageMin * (lpLabInfo->m_nLabDefenceValue / 0x14)
		+ nUserLevel / 2)
		* fRe_DmgMin);
	nAttackMaxDamage = (signed __int64)((double)nPrevAttackMax * fRe_DmgMax);
	nPrevDefence = lpLabInfo->m_nLabAttackMaxValue / lpMonsterObj->Defense + nUserLevel / 2;
	nDefence = (signed __int64)((double)nPrevDefence * fRe_Defence);
	//꼇횅땍  ExperienceRate
	nExpLevel = (double)((nPrevDefence + nPrevAttackMax) / lpMonsterObj->ExperienceRate + nUserLevel / 3) * fRe_ExpLevel;
	lpMonsterObj->MaxLife = (double)nMaxLife;
	lpMonsterObj->Life = lpMonsterObj->MaxLife;
	lpMonsterObj->PhysiDamageMin = nAttackMinDamage;
	lpMonsterObj->PhysiDamageMax = nAttackMaxDamage;
	lpMonsterObj->Defense = nDefence;
	lpMonsterObj->ExperienceRate = nExpLevel;

	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EBE7E0) --------------------------------------------------------
int  Labyrinth::ChangeMissionTypeMaxLevel( int aIndex, int nZoneType, int nZoneIndex, int *nMissionType, int *nMissionValue)
{
	bool bFound; // [sp+7Fh] [bp-2Dh]@11
	int nChkInfinityLoop; // [sp+8Ch] [bp-20h]@3
	int nRand; // [sp+90h] [bp-1Ch]@4
	int nPrevMissionValue; // [sp+94h] [bp-18h]@3
	int nPrevMissionType; // [sp+98h] [bp-14h]@3

	if (gObj[aIndex].MasterLevel + gObj[aIndex].Level >= 820 && *nMissionType == 4)
	{
		nChkInfinityLoop = 0;
		nPrevMissionType = *nMissionType;
		nPrevMissionValue = *nMissionValue;
		LogAdd(LOG_BLACK,
			"[LABYRINTH] [%s][%s](%d) Max level user(%d).",
			gObj[aIndex].Account,
			gObj[aIndex].Name,
			gObj[aIndex].Map,
			gObj[aIndex].MasterLevel + gObj[aIndex].Level);
		if (nZoneType == 3)
		{
			while (1)
			{

				nRand = rand() % this->m_vecZoneTrait.size();
				if (this->m_vecZoneTrait[nRand].nZoneType == 3
					&& this->m_vecZoneTrait[nRand].stMissionTrait[0].nMissionType!= 4)
				{
					break;
				}
				if (!(++nChkInfinityLoop % 2000))
				{
					LogAdd(LOG_BLACK,
						"[LABYRINTH] [%s][%s](%d) whiling over loop, EXP Mission change.[%d]",
						gObj[aIndex].Account,
						gObj[aIndex].Name,
						gObj[aIndex].Map,
						nZoneType);
					nChkInfinityLoop = 0;
				}
			}
			*nMissionType =this->m_vecZoneTrait[nRand].stMissionTrait[0].nMissionType; 

			*nMissionValue = this->m_vecZoneTrait[nRand].stMissionTrait[0].nMissionValue;
			LogAdd(LOG_BLACK,
				"[LABYRINTH] [%s][%s](%d) EXP Mission change. [%d] Type: %d -> %d, Value: %d -> %d",
				gObj[aIndex].Account,
				gObj[aIndex].Name,
				gObj[aIndex].Map,
				nZoneType,
				nPrevMissionType,
				*nMissionType,
				nPrevMissionValue,
				*nMissionValue);
		}
		else if (nZoneType == 4)
		{
			bFound = 0;
			std::vector<_LAB_ZONE_TRAIT>::iterator it = this->m_vecZoneTrait.begin();
			while (true)
			{
				if ((it != this->m_vecZoneTrait.end()) || (it->nZoneType == 4 && it->nZoneIndex == nZoneIndex))
				{
					break;
				}
				it++;
			}

				if (it == this->m_vecZoneTrait.end())
				{
					LogAdd(LOG_BLACK,
						"[LABYRINTH][ERROR] EXP Mission change - failed find main mission zone. [%s][%s](%d)",
						gObj[aIndex].Account,
						gObj[aIndex].Name,
						gObj[aIndex].Map);

						return 9;
				}
				for (it = this->m_vecZoneTrait.begin(); it != this->m_vecZoneTrait.end(); it++)
				{
					nRand = rand() % 6;
					if (it->stMissionTrait[nRand].nMissionType != 4)
						break;
					if (!(++nChkInfinityLoop % 2000))
					{
						LogAdd(LOG_BLACK,
							"[LABYRINTH] [%s][%s](%d) whiling over loop, EXP Mission change.[%d]",
							gObj[aIndex].Account,
							gObj[aIndex].Name,
							gObj[aIndex].Map,
							4);
						nChkInfinityLoop = 0;
					}

				}

				*nMissionType = it->stMissionTrait[nRand].nMissionType;

				*nMissionValue = it->stMissionTrait[nRand].nMissionValue;
				LogAdd(LOG_BLACK,
					"[LABYRINTH] [%s][%s](%d) EXP Mission change. [%d] Type: %d -> %d, Value: %d -> %d",
					gObj[aIndex].Account,
					gObj[aIndex].Name,
					gObj[aIndex].Map,
					4,
					nPrevMissionType,
					*nMissionType,
					nPrevMissionValue,
					*nMissionValue);
		}
	}
	return 0;
}

//----- (00EBEF40) --------------------------------------------------------
void  Labyrinth::CGReqCheckLabyrinthEntering(int aIndex)//ok
{
	LPOBJ lpObj; // [sp+50h] [bp-Ch]@2
	int nRet; // [sp+54h] [bp-8h]@23

	if (!this->CheckUser(aIndex,"Labyrinth::CGReqCheckLabyrinthEntering","c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",41))
	{
		lpObj = &gObj[aIndex];
		LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
		if (gObj[aIndex].MasterLevel + gObj[aIndex].Level >= 400 && lpObj->ChangeUp >= 2)
		{
			if (IsLabyrinthPlayLimitTime() == 1)
			{
				LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s] Can't enter the labyrinth. Time to limit.", lpObj->Account, lpObj->Name);
				this->SendRequestEnterResult( lpObj->Index, 0, 2);
			}
			else
			{
				if (lpLabInfo->GetDBInfoLoad())
				{
					if (lpObj->PartyNumber >= 0 || lpObj->UsePartyMatching)
					{
						LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s] Can't enter the labyrinth. Party user.", lpObj->Account, lpObj->Name);
						this->SendRequestEnterResult( lpObj->Index, 0, 4);
					}
					//else if (/*g_bKorPlayTimeLimitOn != 1
					//	||*/gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_MINIMUMVITALITY)
					//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_LOWVITALITY)
					//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_MEDIUMVITALITY)
					//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_HIGHVITALITY)
					//	/*|| lpObj->m_bPCBangUser == 1
					//	|| lpObj->m_btFatiguePercent <= 90*/)
					//else if (gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_MINIMUMVITALITY)
					//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_LOWVITALITY)
					//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_MEDIUMVITALITY)
					//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_HIGHVITALITY))
					//	{
							if (lpObj->PShopOpen  == 1)
							{
								LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s](%d) Can't enter the labyrinth. pShop OpenState.",lpObj->Account,lpObj->Name,lpObj->Map);
							}
							else
							{
								nRet = g_InstanceSystemMgr->RequestAllocInstanceResource(aIndex, 1);

								if (nRet)
								{
									LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s] Can't use instance resource.(nRet:%d)",lpObj->Account,lpObj->Name,nRet);
									this->SendRequestEnterResult( lpObj->Index, 0, 5);
								}
								else if (lpLabInfo->GetDimensionLevel())
								{
									this->SendRequestEnterResult( lpObj->Index, 1, 1);
								}
								else
								{
									this->SendRequestEnterResult( lpObj->Index, 1, 0);
								}
							}
					//	}
					//else
					//{
					//	//LogAdd(LOG_BLACK,
					//	//	"[LABYRINTH] [%s][%s] Can't enter the labyrinth. limit fatigue (%d).",
					//	//	lpObj->Account,
					//	//	lpObj->Name,
					//	//	lpObj->m_btFatiguePercent);
					//	this->SendRequestEnterResult( lpObj->Index, 0, 6);
					//}
				}
				else
				{
					LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] [%s][%s] Labyrinth DB info not loaded.(%d)", lpObj->Account, lpObj->Name, lpLabInfo->GetDBInfoLoad());
					this->SendRequestEnterResult( lpObj->Index, 0, 100);
				}
			}
		}
		else
		{
			LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s] Can't enter the labyrinth. Need more the level.(%d)",lpObj->Account,lpObj->Name,lpObj->MasterLevel + lpObj->Level+lpObj->ForMasterLevel);
			this->SendRequestEnterResult( lpObj->Index, 0, 3);
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// B57815C: using guessed type int g_bKorPlayTimeLimitOn;

//----- (00EBF330) --------------------------------------------------------
void  Labyrinth::CGReqEnterLabyrinth( int aIndex)//ok
{
	int nMainMissionZone; // [sp+4Ch] [bp-24h]@30
	int nErrChkFlag; // [sp+50h] [bp-20h]@21
	int nMoveZoneNumber; // [sp+54h] [bp-1Ch]@20
	int nLabConfigNum; // [sp+58h] [bp-18h]@20

	int nInstanceIndex; // [sp+60h] [bp-10h]@2
	LPOBJ lpObj; // [sp+64h] [bp-Ch]@2
	int nRet; // [sp+68h] [bp-8h]@1

	nRet = this->CheckUser(aIndex,"Labyrinth::CGReqEnterLabyrinth","c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",41);
	if (!nRet)
	{
		lpObj = &gObj[aIndex];
		nRet = g_InstanceSystemMgr->FindRegistUser(gObj[aIndex].Name, &nInstanceIndex, 1);
		if (nRet == 1)
		{
			LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] CGReqEnterLabyrinth, not find inst obj info. retry..[%s][%s]",lpObj->Account,lpObj->Name);
		}
		else if (lpObj->PartyNumber >= 0 || lpObj->UsePartyMatching)
		{
			LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s] Can't enter the labyrinth. Party user (Enter Check).",lpObj->Account,lpObj->Name);
			this->SendRequestEnterResult( lpObj->Index, 0, 4);
		}
		else if (IsLabyrinthPlayLimitTime() == 1)
		{
			LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s] Can't enter the labyrinth. (Enter Check).", lpObj->Account, lpObj->Name);
			this->SendRequestEnterResult( lpObj->Index, 0, 2);
		}
		//else if (/*g_bKorPlayTimeLimitOn != 1
		//	|| */gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_MINIMUMVITALITY)
		//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_LOWVITALITY)
		//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_MEDIUMVITALITY)
		//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_HIGHVITALITY)
		//	/*|| lpObj->m_bPCBangUser == 1
		//	|| lpObj->m_btFatiguePercent <= 90*/)
		//{
			if (lpObj->PShopOpen == 1)
			{
				LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s](%d) Can't enter the labyrinth. pShop OpenState.",lpObj->Account,lpObj->Name,lpObj->Map);
			}
			else
			{
				LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
				nLabConfigNum = -1;
				nMoveZoneNumber = -1;
				if (lpLabInfo->GetDimensionLevel())
				{
					nLabConfigNum = lpLabInfo->GetConfigNum();
				}
				else
				{
					lpLabInfo->Init();
					this->GDReqLabyrinthMissionDelete( aIndex);
					lpLabInfo->SetDimensionLevel(1);
					nErrChkFlag = 0;
					while (1)
					{

						nLabConfigNum = rand()% this->m_vecZoneConfig.size();

						if (this->m_vecZoneConfig[nLabConfigNum].nLabDimensionLevel == lpLabInfo->GetDimensionLevel())
							break;
						if (!(++nErrChkFlag % 2000))
						{
							LogAdd(LOG_BLACK,"[LABYRINTH][WARN] %s, while loop... in selection configure(IN Init)(aIndex: %d, dim lv: %d)","Labyrinth::CGReqEnterLabyrinth",aIndex,lpLabInfo->GetDimensionLevel());
							nErrChkFlag = 0;
						}
					}
					lpLabInfo->SetConfigNum(nLabConfigNum);
					this->AddLabyrinthMissionDataSet( aIndex, nLabConfigNum);
					this->GDReqLabyrinthInfoSave( aIndex);
					lpLabInfo->SetDBInfoLoad(1);
				}
				g_InstanceSystemMgr->SetInstanceState(nInstanceIndex, 4);
				lpLabInfo->SetPlay( 1);
				lpObj->Map = nInstanceIndex + INST_MAP_INDEX_LABYRINTH;
				LogAdd(LOG_BLACK,"[LABYRINTH] [INFO] instance user mapnum [%s][%s] (%d + %d) = %d",lpObj->Account,lpObj->Name,INST_MAP_INDEX_LABYRINTH,nInstanceIndex,lpObj->Map);
				if (this->FindEntranceZone( nLabConfigNum, &nMoveZoneNumber))
				{
					LogAdd(LOG_BLACK,"[LABYRINTH][FATAL] %s, Not found Entrace Zone [%s][%s](%d)(%d)","Labyrinth::CGReqEnterLabyrinth",lpObj->Account,lpObj->Name,lpObj->Map,nLabConfigNum);
					this->SendRequestEnterResult( lpObj->Index, 0, -2);
				}
				else
				{
					lpLabInfo->CheckLabyrinthUserAbility(lpObj->Index);
					LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s](%d) Enter the labyrinth.", lpObj->Account, lpObj->Name, lpObj->Map);
					this->LoggingLabyrinthInfo( lpObj->Index);
					this->MoveLabyrinthZone( aIndex, nLabConfigNum, nMoveZoneNumber);
					lpLabInfo->SendCurrentZoneMissionInfo(aIndex, nMoveZoneNumber);
					nMainMissionZone = -1;
					nRet = this->FindMainMissionZone( nLabConfigNum, &nMainMissionZone);
					if (!nRet)
						lpLabInfo->AddZonePath(nMainMissionZone);
					lpLabInfo->SendZonePath( aIndex);
					this->SendNotifyLabyrinthDimensionEnter( aIndex);

				}
			}
		//}
		//else
		//{
		//	//LogAdd(LOG_BLACK,
		//	//	"[LABYRINTH] [%s][%s](%d) Can't enter the labyrinth. limit fatique(%d) (Enter Check).",
		//	//	lpObj->Account,
		//	//	lpObj->Name,
		//	//	lpObj->Map,
		//	//	lpObj->m_btFatiguePercent);
		//	this->SendRequestEnterResult( lpObj->Index, 0, 6);
		//}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// B57815C: using guessed type int g_bKorPlayTimeLimitOn;

//----- (00EBF900) --------------------------------------------------------
void  Labyrinth::CGReqMoveLabyrinthZone( int aIndex, PMSG_REQ_MOVE_LABYRINTH_ZONE *pMsg)//ok
{
	LabyrinthMission *lpLabMission; // ST78_4@30
	
	_LAB_MISSION *lpMission; // [sp+50h] [bp-40h]@30
	_LAB_ZONE *lpMoveLabZone; // [sp+58h] [bp-38h]@24
	int nMaxCol; // [sp+5Ch] [bp-34h]@17
	int nMaxRow; // [sp+60h] [bp-30h]@17
	int nCurZoneNum; // [sp+64h] [bp-2Ch]@17
	int nLabConfigNum; // [sp+68h] [bp-28h]@17
	int nCol; // [sp+6Ch] [bp-24h]@17
	int nRow; // [sp+70h] [bp-20h]@17
	bool bMovable; // [sp+76h] [bp-1Ah]@7
	BYTE btGatePosition; // [sp+77h] [bp-19h]@7
	int nMoveZoneNumber; // [sp+78h] [bp-18h]@7
	_LAB_ZONE *lpLabZone; // [sp+7Ch] [bp-14h]@5

	LPOBJ lpObj; // [sp+84h] [bp-Ch]@2
	int nRet; // [sp+88h] [bp-8h]@1

	nRet = CheckUser(aIndex,"Labyrinth::CGReqMoveLabyrinthZone","c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",41);
	if (!nRet)
	{
		lpObj = &gObj[aIndex];
		LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
		if (lpLabInfo->IsPlaying())
		{
			if (lpObj->RegenOk <= 0)
			{

				lpLabZone = this->GetLabyrinthZone(lpLabInfo->GetConfigNum(), lpLabInfo->GetCurrentZone(), &nRet);
				if (nRet)
				{
					LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, Failed get labyrinth zone. [%s][%s](%d), (%d)(%d)","Labyrinth::CGReqMoveLabyrinthZone",lpObj->Account,lpObj->Name,lpObj->Map,lpLabInfo->GetConfigNum(),lpLabInfo->GetCurrentZone());
				}
				else
				{
					nMoveZoneNumber = -1;
					btGatePosition = pMsg->btGatePosition;
					bMovable = 0;
					if (btGatePosition >= 0 && btGatePosition <= 3)
					{
						if (lpLabZone->btGate[(unsigned __int8)btGatePosition] == 1)
							bMovable = 1;
						if (lpLabZone->btGate[(unsigned __int8)btGatePosition] == 2)
						{

							bMovable = lpLabInfo->CheckMissionClear(aIndex, lpLabZone->nZoneType, lpLabInfo->GetCurrentZone());
						}
						if (bMovable == 1)
						{
							nLabConfigNum = lpLabInfo->GetConfigNum();
							nCurZoneNum = lpLabInfo->GetCurrentZone();
							nMaxRow = this->m_vecZoneConfig[nLabConfigNum].nRow;

							nMaxCol = this->m_vecZoneConfig[nLabConfigNum].nCol;
							this->ZoneNum2Coordi( nMaxRow, nMaxCol, nCurZoneNum, &nRow, &nCol);
							if (this->CheckZoneBoundary( nMaxRow, nMaxCol, nCurZoneNum, (unsigned __int8)btGatePosition))
							{
								switch (btGatePosition)
								{
								case 0:
									nMoveZoneNumber = this->Coordi2ZoneNum( nMaxRow, nMaxCol, nRow - 1, nCol);
									break;
								case 1:
									nMoveZoneNumber = this->Coordi2ZoneNum( nMaxRow, nMaxCol, nRow + 1, nCol);
									break;
								case 2:
									nMoveZoneNumber = this->Coordi2ZoneNum( nMaxRow, nMaxCol, nRow, nCol - 1);
									break;
								case 3:
									nMoveZoneNumber = this->Coordi2ZoneNum( nMaxRow, nMaxCol, nRow, nCol + 1);
									break;
								default:
									break;
								}

								lpMoveLabZone = this->GetLabyrinthZone(lpLabInfo->GetConfigNum(), nMoveZoneNumber, &nRet);
								if (nRet)
								{
									LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, Failed get move labyrinth zone. [%s][%s](%d), (%d)-(Move:%d:%d:%d)","Labyrinth::CGReqMoveLabyrinthZone",lpObj->Account,lpObj->Name,lpObj->Map,lpLabInfo->GetConfigNum(),nMoveZoneNumber,nRow,nCol);
								}
								else if (lpMoveLabZone->nZoneType)
								{
									if (lpLabZone->nZoneType == 3 || lpLabZone->nZoneType == 4)
									{
										lpLabMission = lpLabInfo->GetLabyrinthMissionInfo();

										lpMission = lpLabInfo->m_cLabMission.GetProgressQuestMission(lpLabInfo->GetCurrentZone(), lpLabZone->nZoneType, &nRet);
										if (!nRet)
											this->GDReqLabyrinthMissionInfoUpdate( aIndex, lpMission);
									}
									this->MoveLabyrinthZone( aIndex, nLabConfigNum, nMoveZoneNumber);
									lpLabInfo->SendCurrentZoneMissionInfo(aIndex, nMoveZoneNumber);
									lpLabInfo->SendZonePath(aIndex);
								}
								else
								{
									LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, It's Unregistered zone.[%s][%s](%d), (%d)-(Move:%d:%d:%d)","Labyrinth::CGReqMoveLabyrinthZone",lpObj->Account,lpObj->Name,lpObj->Map,lpLabInfo->GetConfigNum(),nMoveZoneNumber,nRow,nCol);
								}
							}
							else
							{
								LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, over the Zone boundary pos [%s][%s](%d), (%d):(%d)(%d)(%d)","Labyrinth::CGReqMoveLabyrinthZone",lpObj->Account,lpObj->Name,lpObj->Map,nCurZoneNum,nMaxRow,nMaxCol,(unsigned __int8)btGatePosition);
							}
						}
					}
					else
					{
						LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, invalid gate position. [%s][%s](%d), (%d)","Labyrinth::CGReqMoveLabyrinthZone",lpObj->Account,lpObj->Name,lpObj->Map,pMsg->btGatePosition);
					}
				}
			}
		}
		else
		{
			LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, Not playing user. [%s][%s](%d)","Labyrinth::CGReqMoveLabyrinthZone",lpObj->Account,lpObj->Name,lpObj->Map);
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EBFEC0) --------------------------------------------------------
void  Labyrinth::CGReqLevelUpLabyrinth( int aIndex, PMSG_REQ_LABYRINTH_DIMENSION_LEVEL_UP *pMsg)//ok
{
	_LAB_ZONE *lpLabZone; // [sp+50h] [bp-1Ch]@6
	int nMainMissionZoneNum; // [sp+54h] [bp-18h]@4
	int nLabConfigNum; // [sp+58h] [bp-14h]@4

	LPOBJ lpObj; // [sp+60h] [bp-Ch]@2
	int nRet; // [sp+64h] [bp-8h]@1

	nRet = CheckUser(aIndex,"Labyrinth::CGReqLevelUpLabyrinth","c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",41);
	if (!nRet)
	{
		lpObj = &gObj[aIndex];
		LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
		if (lpLabInfo->GetDimensionLevel() == 7)
		{
			LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, current dimension level is maximum. [%s][%s](%d)","Labyrinth::CGReqLevelUpLabyrinth",lpObj->Account,lpObj->Name,lpObj->Map);
		}
		else
		{
			nLabConfigNum = lpLabInfo->GetConfigNum();
			nMainMissionZoneNum = -1;
			nRet = this->FindMainMissionZone( nLabConfigNum, &nMainMissionZoneNum);
			if (nRet)
			{
				LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, Not found main mission zone. [%s][%s](%d), (%d)","Labyrinth::CGReqLevelUpLabyrinth",lpObj->Account,lpObj->Name,lpObj->Map,nLabConfigNum);
			}
			else
			{
				lpLabZone = this->GetLabyrinthZone(nLabConfigNum, nMainMissionZoneNum, &nRet);
				if (nRet)
				{
					LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, Not found labyrinth zone info. [%s][%s](%d), (%d)(%d)","Labyrinth::CGReqLevelUpLabyrinth",lpObj->Account,lpObj->Name,lpObj->Map,nLabConfigNum,nMainMissionZoneNum);
				}
				else if (lpLabInfo->CheckMissionClear( aIndex, lpLabZone->nZoneType, nMainMissionZoneNum) == 1)
				{
					lpLabInfo->MoveDimensionLevel( aIndex);
					this->SendNotifyLabyrinthDimensionEnter( aIndex);
				}
				else
				{
					LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] %s, Not complete all main mission. [%s][%s](%d)","Labyrinth::CGReqLevelUpLabyrinth",lpObj->Account,lpObj->Name,lpObj->Map);
				}
			}
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC00F0) --------------------------------------------------------
void  Labyrinth::GDReqLabyrinthInfo( int aIndex)
{
	SDHP_REQ_LABYRINTH_INFO pMsg; // [sp+4Ch] [bp-30h]@3

	int nRet; // [sp+70h] [bp-Ch]@1

	nRet = CheckUser(aIndex,"Labyrinth::GDReqLabyrinthInfo","c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",41);
	if (nRet)
	{
		LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] GDReqLabyrinthInfo, Not User.(%d)", aIndex);
	}
	else
	{
		LPOBJ lpObj = &gObj[aIndex];
		pMsg.h.set(0x77, 0x0, sizeof(pMsg));

		memcpy(pMsg.szAccountID, lpObj->Account, sizeof(pMsg.szAccountID));
		memcpy(pMsg.szName, lpObj->Name, sizeof(pMsg.szName));
		pMsg.nUserIndex = lpObj->Index;

		gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(SDHP_REQ_LABYRINTH_INFO));

	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC01F0) --------------------------------------------------------
void  Labyrinth::DGAnsLabyrinthInfo( BYTE *aRecv)
{
	
	_LAB_MISSION stLabMission; // [sp+4Ch] [bp-4Ch]@13
	MISSION_INFO *lpMissionInfo; // [sp+78h] [bp-20h]@13

	int nOfs; // [sp+80h] [bp-18h]@11

	LPOBJ lpObj; // [sp+88h] [bp-10h]@3
	int nRet; // [sp+8Ch] [bp-Ch]@1
	SDHP_ANS_LABYRINTH_INFO *RecvBuf; // [sp+90h] [bp-8h]@1

	RecvBuf = (SDHP_ANS_LABYRINTH_INFO *)aRecv;
	nRet = this->CheckUser(*((DWORD *)aRecv + 2),"Labyrinth::DGAnsLabyrinthInfo","c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",41);
	if (nRet)
	{
		LogAdd(LOG_BLACK,"[LABYRINTH][ERROR] DGAnsLabyrinthInfo, Not User.(%d)", RecvBuf->nUserIndex);
	}
	else
	{
		lpObj = &gObj[RecvBuf->nUserIndex];
		LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
		if (RecvBuf->btResult == 2)
		{
			lpLabInfo->Init();
			lpLabInfo->SetDBInfoLoad(0);
			LogAdd(LOG_BLACK,
				"[LABYRINTH][ERROR] DGAnsLabyrinthInfo, DB data is not abnormal. [%s][%s]",
				lpObj->Account,
				lpObj->Name);
		}
		else if (RecvBuf->btResult == 1)
		{
			lpLabInfo->Init();
			lpLabInfo->SetDBInfoLoad(1);
			LogAdd(LOG_BLACK,"[LABYRINTH] [%s][%s] DGAnsLabyrinthInfo, recv Labyrinth Empty data.", lpObj->Account, lpObj->Name);
		}
		else if (!RecvBuf->btResult)
		{
			lpLabInfo->Init();

			lpLabInfo->SetDimensionLevel(RecvBuf->btDimensionLevel);

			lpLabInfo->SetConfigNum( RecvBuf->wConfigNum);

			lpLabInfo->SetCurrentZone(RecvBuf->btCurrentZone);

			for (int i = 0; i < RecvBuf->btVisitedCnt; ++i)
				lpLabInfo->AddZonePath( RecvBuf->btVisitedList[i]);

			lpLabInfo->SetEntireExp( RecvBuf->nEntireExp);

			lpLabInfo->SetEntireKillMonCnt( RecvBuf->nEntireMonKillCnt);

			lpLabInfo->SetLimitClearCnt( RecvBuf->nClearCnt);

			lpLabInfo->SetClearState(RecvBuf->btClearState);

			nOfs = sizeof(SDHP_ANS_LABYRINTH_INFO);

			for (int j = 0; j < RecvBuf->btMissionCount; ++j)
			{
				lpMissionInfo = (MISSION_INFO *)&aRecv[nOfs];


				_LAB_MISSION stLabMission;

				stLabMission.nZoneNumber = lpMissionInfo->btZoneNumber;

				stLabMission.nMissionType = lpMissionInfo->btMissionType;

				stLabMission.nMissionValue = lpMissionInfo->nMissionValue;

				stLabMission.nScoreValue = lpMissionInfo->nAcquisionValue;

				stLabMission.nMissionState = lpMissionInfo->btMissionState;

				stLabMission.nIsMainMission = lpMissionInfo->btIsMainMission;

				stLabMission.nMainMissionOrdering = lpMissionInfo->btMainMissionOrder;

				stLabMission.nRewardItemType = lpMissionInfo->nRewardItemIndex + (lpMissionInfo->nRewardItemType << 9);

				stLabMission.nRewardValue = lpMissionInfo->nRewardValue;

				stLabMission.nRewardState = lpMissionInfo->btRewardCheckState;

				if (lpMissionInfo->btIsMainMission)
				{

					lpLabInfo->m_cLabMission.AddMainMission(stLabMission);
				}
				else
				{

					lpLabInfo->m_cLabMission.AddSubMission(stLabMission);
				}
				nOfs += sizeof(MISSION_INFO);
			}

			lpLabInfo->SetDBInfoLoad(1);
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC05E0) --------------------------------------------------------
void  Labyrinth::GDReqLabyrinthInfoSave( int aIndex)//ok
{
	
	int nZoneNumber; // [sp+64h] [bp-1CDCh]@12
	_LAB_MISSION v10; // [sp+68h] [bp-1CD8h]@12
	int len; // [sp+B0h] [bp-1C90h]@12
	char buff[7000]; // [sp+B4h] [bp-1C8Ch]@12
	int nErrorCode; // [sp+1D28h] [bp-18h]@1

	SDHP_REQ_LABYRINTH_INFO_SAVE pMsg;
	nErrorCode = CheckUser(
		aIndex,
		"Labyrinth::GDReqLabyrinthInfoSave",
		"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",
		41);
	LPOBJ lpObj = &gObj[aIndex];
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
	if (!nErrorCode)
	{
		//v28 = gObj[aIndex].m_cLabyrinthInfo;
		pMsg.h.set(0x77, 0x01, sizeof(pMsg));
		len = sizeof(pMsg);
		pMsg.nUserIndex = aIndex;

		memcpy(pMsg.szAccountID, gObj[aIndex].Account, sizeof(pMsg.szAccountID));
		memcpy(pMsg.szName, gObj[aIndex].Name, sizeof(pMsg.szName));
		pMsg.btDimensionLevel = lpLabInfo->GetDimensionLevel();
		pMsg.wConfigNum = lpLabInfo->GetConfigNum();
		pMsg.btCurrentZone = lpLabInfo->GetCurrentZone();
		pMsg.nEntireExp = lpLabInfo->GetEntireExp();
		pMsg.nEntireMonKillCnt = lpLabInfo->GetEntireKillMonCnt();
		pMsg.nClearCnt = lpLabInfo->GetLimitClearCnt();
		pMsg.btClearState = lpLabInfo->GetClearState();
		pMsg.btVisitedCnt = lpLabInfo->GetZonePathCount();
		memset(pMsg.btVisitedList, -1, sizeof(pMsg.btVisitedList));


		for (int j = 0; j < pMsg.btVisitedCnt; ++j)
		{
			BYTE btZontPath = lpLabInfo->GetZonePath(j, &nErrorCode);
			if (nErrorCode)
			{
				LogAdd(LOG_BLACK,
					"[LABYRINTH][FATAL] %s, Failed get zone path. [%s][%s](%d)(%d)",
					"Labyrinth::GDReqLabyrinthInfoSave",
					gObj[aIndex].Account,
					gObj[aIndex].Name,
					lpLabInfo->GetConfigNum(),
					j);
				return;
			}
			pMsg.btVisitedList[j] = btZontPath;
		}

		pMsg.btMissionCount = lpLabInfo->m_cLabMission.GetSubMissionCount() + lpLabInfo->m_cLabMission.GetMainMissionCount();

		if (pMsg.btMissionCount)
		{
			memset(buff, 0, sizeof(buff));
			
			v10.Clear();
			nZoneNumber = 0;

			this->FindMainMissionZone(lpLabInfo->GetConfigNum(), &nZoneNumber);
			MISSION_INFO info;
			for (int nMainMissionOrdering = 0;nMainMissionOrdering < lpLabInfo->m_cLabMission.GetMainMissionCount();++nMainMissionOrdering)
			{
				memcpy(&v10, lpLabInfo->m_cLabMission.GetMainMission(nMainMissionOrdering, &nErrorCode), sizeof(v10));
				if (nErrorCode)
				{

					LogAdd(LOG_BLACK,
						"[LABYRINTH][FATAL] %s, Failed get zone path. [%s][%s](%d)(%d)",
						"Labyrinth::GDReqLabyrinthInfoSave",
						gObj[aIndex].Account,
						gObj[aIndex].Name,
						lpLabInfo->GetConfigNum(),
						nMainMissionOrdering);
					return;
				}

				info.btZoneNumber = v10.nZoneNumber;
				info.btMissionType = v10.nMissionType;
				info.nMissionValue = v10.nMissionValue;
				info.nAcquisionValue = 0;
				info.btMissionState = 0;
				info.btIsMainMission = v10.nIsMainMission;
				info.btMainMissionOrder = v10.nMainMissionOrdering;
				info.nRewardItemType = v10.nRewardItemType / 512;
				info.nRewardItemIndex = v10.nRewardItemType % 512;
				info.nRewardValue = v10.nRewardValue;
				info.btRewardCheckState = v10.nRewardState;
				//v14 = v10.nScoreValue;
				memcpy(&buff[len], &info, sizeof(info));
				len += sizeof(info);
			}

			std::map<int, _LAB_MISSION>::iterator it = lpLabInfo->m_cLabMission.m_mapSubMission.begin();

			for (int k = 0; k < lpLabInfo->m_cLabMission.GetSubMissionCount(); ++k)
			{

				info.btZoneNumber = it->second.nZoneNumber;
				info.btMissionType = it->second.nMissionType;
				info.nMissionValue = it->second.nMissionValue;
				info.nAcquisionValue = 0;
				info.btMissionState = 0;
				info.btIsMainMission = it->second.nIsMainMission;
				info.btMainMissionOrder = it->second.nMainMissionOrdering;
				info.nRewardItemType = it->second.nRewardItemType / 512;
				info.nRewardItemIndex = it->second.nRewardItemType % 512;
				info.nRewardValue = it->second.nRewardValue;
				info.btRewardCheckState = it->second.nRewardState;
				//v14 = v10.nScoreValue;
				memcpy(&buff[len], &info, sizeof(info));
				len += sizeof(info);

				it++;
			}
			pMsg.h.size[0] = SET_NUMBERHB(len);
			pMsg.h.size[1] = SET_NUMBERLB(len);

			memcpy(buff, &pMsg, sizeof(pMsg));

			gDataServerConnection.DataSend((BYTE*)buff, len);

		}
		else
		{
			pMsg.h.size[0] = SET_NUMBERHB(len);
			pMsg.h.size[1] = SET_NUMBERLB(len);
			gDataServerConnection.DataSend((BYTE*)&pMsg, len);

		}
	
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// EC05E0: using guessed type char var_FA[202];

//----- (00EC0DF0) --------------------------------------------------------
void  Labyrinth::GDReqLabyrinthInfoUpdate( int aIndex)//ok
{
	int nRet; // [sp+50h] [bp-118h]@4
	BYTE btZonePath; // [sp+57h] [bp-111h]@6
	SDHP_REQ_LABYRINTH_INFO_UPDATE pMsg; // [sp+60h] [bp-108h]@1

	LPOBJ lpObj = &gObj[aIndex];

	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;

	pMsg.h.set(0x77, 0x02, sizeof(pMsg));

	pMsg.nUserIndex = lpObj->Index;
	memcpy(pMsg.szAccountID, lpObj->Account, sizeof(pMsg.szAccountID));
	memcpy(pMsg.szName, lpObj->Name, sizeof(pMsg.szName));
	pMsg.btDimensionLevel = lpLabInfo->GetDimensionLevel();
	pMsg.wConfigNum = lpLabInfo->GetConfigNum();
	pMsg.btCurrentZone = lpLabInfo->GetCurrentZone();
	pMsg.nEntireExp = lpLabInfo->GetEntireExp();
	pMsg.nEntireMonKillCnt = lpLabInfo->GetEntireKillMonCnt();
	pMsg.btVisitedCnt = lpLabInfo->GetZonePathCount();
	memset(pMsg.btVisitedList, -1, sizeof(pMsg.btVisitedList));

	nRet = 0;
	for (int j = 0; j < (unsigned __int8)pMsg.btVisitedCnt; ++j)
	{
		btZonePath = lpLabInfo->GetZonePath(j, &nRet);
		if (nRet)
		{
			LogAdd(LOG_BLACK,
				"[LABYRINTH][FATAL] %s, Failed get zone path. [%s][%s](%d)(%d)",
				"Labyrinth::GDReqLabyrinthInfoUpdate",
				lpObj->Account,
				lpObj->Name,
				lpLabInfo->GetConfigNum(),
				j);
			return;
		}
		pMsg.btVisitedList[j] = btZonePath;
	}

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));

}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC10C0) --------------------------------------------------------
void  Labyrinth::GDReqLabyrinthMissionInfoUpdate( int aIndex, _LAB_MISSION *stMission)//ok
{
	//LabyrinthInfo *lpLabInfo; // ST5C_4@1
	SDHP_REQ_LABYRINTH_MISSION_UPDATE pMsg; // [sp+50h] [bp-48h]@1

	//lpLabInfo = gObj[aIndex].m_cLabyrinthInfo;
	pMsg.h.set(0x77, 0x03, sizeof(pMsg));

	pMsg.nUserIndex = aIndex;
	memcpy(pMsg.szAccountID, gObj[aIndex].Account, sizeof(pMsg.szAccountID));
	memcpy(pMsg.szName, gObj[aIndex].Name, sizeof(pMsg.szName));
	pMsg.btZoneNumber = stMission->nZoneNumber;
	pMsg.btMissionType = stMission->nMissionType;
	pMsg.nMissionValue = stMission->nMissionValue;
	pMsg.nAcquisionValue = stMission->nScoreValue;
	pMsg.btMissionState = stMission->nMissionState;
	pMsg.btIsMainMission = stMission->nIsMainMission;
	pMsg.btMainMissionOrder = stMission->nMainMissionOrdering;
	pMsg.nRewardItemType = stMission->nRewardItemType / 512;
	pMsg.nRewardItemIndex = stMission->nRewardItemType % 512;
	pMsg.nRewardValue = stMission->nRewardValue;
	pMsg.btRewardCheckState = stMission->nRewardState;

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

//----- (00EC1240) --------------------------------------------------------
void  Labyrinth::GDReqGJSetStateUpdate( LPOBJ lpObj)//ok
{

	_LAB_MISSION *lpLabMission; // [sp+4Ch] [bp-18h]@5
	_LAB_ZONE *lpLabZone; // [sp+54h] [bp-10h]@3
	int nRet; // [sp+58h] [bp-Ch]@3


	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
	if (lpObj)
	{

		if (lpLabInfo->IsPlaying() == 1)
		{
			this->GDReqLabyrinthInfoUpdate( lpObj->Index);
			nRet = 0;

			lpLabZone = this->GetLabyrinthZone(lpLabInfo->GetConfigNum(), lpLabInfo->GetCurrentZone(), &nRet);
			if (nRet)
			{

				LogAdd(LOG_BLACK,
					"[LABYRINTH][ERROR] %s, failed lab zone info. [%s][%s](%d)(%d)",
					"Labyrinth::GDReqGJSetStateUpdate",
					lpObj->Account,
					lpObj->Name,
					lpLabInfo->GetConfigNum(),
					lpLabInfo->GetCurrentZone());
			}
			else
			{


				lpLabMission = lpLabInfo->m_cLabMission.GetProgressQuestMission( lpLabInfo->GetCurrentZone(), lpLabZone->nZoneType, &nRet);
				if (!nRet)
					this->GDReqLabyrinthMissionInfoUpdate( lpObj->Index, lpLabMission);
			}
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC1380) --------------------------------------------------------
void  Labyrinth::GDReqLabyrinthMissionDelete( int aIndex)//ok
{
	SDHP_REQ_LABYRINTH_MISSION_DELETE pMsg; // [sp+4Ch] [bp-28h]@1
	pMsg.h.set(0x77, 0x04, sizeof(pMsg));

	pMsg.nUserIndex = aIndex;
	memcpy(pMsg.szAccountID, gObj[aIndex].Account, sizeof(pMsg.szAccountID));
	memcpy(pMsg.szName, gObj[aIndex].Name, sizeof(pMsg.szName));

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

//----- (00EC1440) --------------------------------------------------------
void  Labyrinth::GDReqLabyrinthEndUpdate( int aIndex)//ok
{
	SDHP_REQ_LABYRINTH_END_UPDATE pMsg; // [sp+4Ch] [bp-30h]@1
	pMsg.h.set(0x77, 0x05, sizeof(pMsg));
	LPOBJ lpObj = &gObj[aIndex];
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
	pMsg.nUserIndex = aIndex;
	memcpy(pMsg.szAccountID, gObj[aIndex].Account, sizeof(pMsg.szAccountID));
	memcpy(pMsg.szName, gObj[aIndex].Name, sizeof(pMsg.szName));

	pMsg.nClearCnt = lpLabInfo->GetLimitClearCnt();
	pMsg.btClearState = lpLabInfo->GetClearState();

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

//----- (00EC1550) --------------------------------------------------------
void  Labyrinth::GDReqLabyrinthRewardCompleteUpdate( int aIndex, char btRewardState)//ok
{
	SDHP_REQ_LABYRINTH_REWARD_COMPLETE pMsg; // [sp+4Ch] [bp-2Ch]@1

	pMsg.h.set(0x77, 0x06, sizeof(pMsg));
	pMsg.nUserIndex = aIndex;
	memcpy(pMsg.szAccountID, gObj[aIndex].Account, sizeof(pMsg.szAccountID));
	memcpy(pMsg.szName, gObj[aIndex].Name, sizeof(pMsg.szName));
	pMsg.btIsMainMission = 1;
	pMsg.btRewardCheckState = btRewardState;

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

//----- (00EC1620) --------------------------------------------------------
void  Labyrinth::GDReqLabyrinthClearLogSave( int aIndex, int nDimensionLevel)
{
	SDHP_REQ_LABYRINTH_CLEAR_LOG_SET_SAVE pMsg; // [sp+4Ch] [bp-2Ch]@1

	pMsg.h.set(0x77, 0x06, sizeof(pMsg));
	pMsg.nUserIndex = aIndex;
	memcpy(pMsg.szAccountID, gObj[aIndex].Account, sizeof(pMsg.szAccountID));
	memcpy(pMsg.szName, gObj[aIndex].Name, sizeof(pMsg.szName));
	pMsg.nDimensionLevel = nDimensionLevel;

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

//----- (00EC16F0) --------------------------------------------------------
int  Labyrinth::AddLabyrinthMissionDataSet( int aIndex, int nLabConfigNum)//ok
{

	int nRand; // [sp+88h] [bp-44h]@13

	int v20; // [sp+90h] [bp-3Ch]@7
	_LAB_ZONE *lpLabZone; // [sp+94h] [bp-38h]@5
	int nZoneNum; // [sp+98h] [bp-34h]@5

	int nMissionValue; // [sp+A4h] [bp-28h]@7
	int nMissionType; // [sp+A8h] [bp-24h]@7
	int nRet; // [sp+B8h] [bp-14h]@5
	int nMaxRow = this->m_vecZoneConfig[nLabConfigNum].nRow;

	int nMaxCol = this->m_vecZoneConfig[nLabConfigNum].nCol;

	LPOBJ lpObj = &gObj[aIndex];
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
	for (int y = 0; y < nMaxRow; ++y)
	{
		for (int x = 0; x < nMaxCol; ++x)
		{
			nZoneNum = this->Coordi2ZoneNum( nMaxRow, nMaxCol, y, x);
			lpLabZone = this->GetLabyrinthZone( nLabConfigNum, nZoneNum, &nRet);
			if (nRet)
			{

				LogAdd(LOG_BLACK,
					"[LABYRINTH][FATAL] %s, !!!! Fatal Error. Can't play labyrinth event. [%s][%s](%d)",
					"Labyrinth::AddLabyrinthMissionDataSet",
					gObj[aIndex].Account,
					gObj[aIndex].Name,
					lpLabInfo->GetConfigNum());
				return nRet;
			}
			if (lpLabZone->nZoneType == LAB_ZONE_TYPE_TRAP)
			{
				v20 = this->GetLabMissionTypeValue(
					lpLabZone->nZoneType,
					lpLabZone->nZoneIndex,
					&nMissionType,
					&nMissionValue);
				v20 = this->ChangeMissionTypeMaxLevel(
					aIndex,
					lpLabZone->nZoneType,
					lpLabZone->nZoneIndex,
					&nMissionType,
					&nMissionValue);
				if (!v20)
					lpLabInfo->AddLabyrinthMission(nZoneNum, nMissionType, nMissionValue, 0, 0);
			}
			else if (lpLabZone->nZoneType == LAB_ZONE_TYPE_MAIN_MISSION)
			{
				for (int i = 0; i < 5; ++i)
				{
					nRand = rand() % 6;

					std::vector<_LAB_ZONE_TRAIT> ::iterator it = this->m_vecZoneTrait.begin();

					while (true)
					{
						if (it!= this->m_vecZoneTrait.end()&&it->nZoneType == lpLabZone->nZoneType&&it->nZoneIndex == lpLabZone->nZoneIndex)
						{
							break;
						}
						it++;
					}

					if (it == this->m_vecZoneTrait.end())
					{
						LogAdd(LOG_BLACK,
							"[LABYRINTH][FATAL] %s, Mismatching, Zone type - index. [%s][%s](%d)",
							"Labyrinth::AddLabyrinthMissionDataSet",
							gObj[aIndex].Account,
							gObj[aIndex].Name,
							lpLabInfo->GetConfigNum());
						return 9;
					}

					nMissionType = it->stMissionTrait[nRand].nMissionType;

					nMissionValue = it->stMissionTrait[nRand].nMissionValue;

					nRet = this->ChangeMissionTypeMaxLevel(
						aIndex,
						lpLabZone->nZoneType,
						lpLabZone->nZoneIndex,
						&nMissionType,
						&nMissionValue);
					lpLabInfo->AddLabyrinthMission(nZoneNum, nMissionType, nMissionValue, 1, i);
				}
			}
		}
	}
	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC1BC0) --------------------------------------------------------
int  Labyrinth::GetLabMissionTypeValue( int nZoneType, int nZoneIndex, int *nMissionType, int *nMissionValue)//ok
{
	std::vector<_LAB_ZONE_TRAIT> ::iterator it = this->m_vecZoneTrait.begin();

	while (1)
	{
		if (it== this->m_vecZoneTrait.end())
		{
			break;
		}
		if (it->nZoneType==nZoneType&&it->nZoneIndex==nZoneIndex)
		{
			*nMissionType = it->stMissionTrait[0].nMissionType;
			*nMissionValue = it->stMissionTrait[0].nMissionValue;
			break;
		}
		it++;

	}
	if (it== this->m_vecZoneTrait.end())
	{
		return 9;
	}
	else
	{
		return 0;
	}

	return 0;
}

//----- (00EC1DA0) --------------------------------------------------------
int  Labyrinth::FindEntranceZone( int nLabConfigNum, int *nZoneNumber)//ok
{
	int x; // [sp+4Ch] [bp-18h]@3
	int y; // [sp+50h] [bp-14h]@1
	int nMaxCol; // [sp+54h] [bp-10h]@1
	int nMaxRow; // [sp+58h] [bp-Ch]@1

	nMaxRow =this->m_vecZoneConfig[nLabConfigNum].nRow;
	nMaxCol = this->m_vecZoneConfig[nLabConfigNum].nCol;
	for (y = 0; y < nMaxRow; ++y)
	{
		for (x = 0; x < nMaxCol; ++x)
		{
			*nZoneNumber = this->Coordi2ZoneNum( nMaxRow, nMaxCol, y, x);
			if (this->m_vecZoneConfig[nLabConfigNum].stZone[*nZoneNumber].nZoneType == 1)
				return 0;
		}
	}
	return 5;
}

//----- (00EC1EA0) --------------------------------------------------------
int  Labyrinth::FindMainMissionZone( int nLabConfigNum, int *nZoneNumber)//ok
{
	int x; // [sp+4Ch] [bp-18h]@3
	int y; // [sp+50h] [bp-14h]@1
	int nMaxCol; // [sp+54h] [bp-10h]@1
	int nMaxRow; // [sp+58h] [bp-Ch]@1

	nMaxRow = this->m_vecZoneConfig[nLabConfigNum].nRow;
	nMaxCol = this->m_vecZoneConfig[nLabConfigNum].nCol;
	for (y = 0; y < nMaxRow; ++y)
	{
		for (x = 0; x < nMaxCol; ++x)
		{
			*nZoneNumber = this->Coordi2ZoneNum( nMaxRow, nMaxCol, y, x);
			if (this->m_vecZoneConfig[nLabConfigNum].stZone[*nZoneNumber].nZoneType == 4)
				return 0;
		}
	}
	return 5;
}

//----- (00EC1FA0) --------------------------------------------------------
int  Labyrinth::MoveLabyrinthZone( int aIndex, int nLabConfigNum, int nMoveZoneNumber)//ok
{
	int v4; // eax@2

	int n; // [sp+68h] [bp-68h]@24
	PMSG_ANS_MOVE_LABYRINTH_ZONE SendData; // [sp+6Ch] [bp-64h]@24
	MONSTER_INFO *lpMAttr; // [sp+80h] [bp-50h]@16
	int iIndex; // [sp+84h] [bp-4Ch]@15
	int j; // [sp+88h] [bp-48h]@13
	int nMonsterCount; // [sp+8Ch] [bp-44h]@13
	int nMonsterClass; // [sp+90h] [bp-40h]@13

	int i; // [sp+A0h] [bp-30h]@7
	_LAB_ZONE *lpLabZone; // [sp+A4h] [bp-2Ch]@3
	int nRet; // [sp+A8h] [bp-28h]@1
	int nInstanceIndex; // [sp+ACh] [bp-24h]@1
	int nPrevZoneNum; // [sp+B0h] [bp-20h]@1

	LPOBJ lpObj = &gObj[aIndex];
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;

	nPrevZoneNum = lpLabInfo->GetCurrentZone();
	nRet = g_InstanceSystemInfo.ResolutionInstanceIndex(lpObj->Map, &nInstanceIndex);
	if (nRet)
	{
		LogAdd(LOG_BLACK,
			"[LABYRINTH][FATAL] %s, Failed get instance index number [%s][%s](%d)",
			"Labyrinth::MoveLabyrinthZone",
			lpObj->Account,
			lpObj->Name,
			lpObj->Map);
		v4 = nRet;
	}
	else
	{
		g_InstanceSystemMgr->DeleteAllInstanceMonster( nInstanceIndex);
		g_InstanceSystemMgr->DeleteAllInstanceMapItem(nInstanceIndex);
		lpLabZone = Labyrinth::GetLabyrinthZone( nLabConfigNum, nMoveZoneNumber, &nRet);
		if (nRet)
		{
			LogAdd(LOG_BLACK,
				"[LABYRINTH][FATAL] %s, Failed get labyrinth zone info. [%s][%s](%d)(%d)",
				"Labyrinth::MoveLabyrinthZone",
				lpObj->Account,
				lpObj->Name,
				nLabConfigNum,
				nMoveZoneNumber);
			v4 = nRet;
		}
		else
		{
			lpLabInfo->AddZonePath(nMoveZoneNumber);
			if (lpLabZone->nZoneType == 3 || lpLabZone->nZoneType == 4)
			{
				for (i = 0; i < this->m_vecZoneTrait.size(); ++i)
				{
					if (lpLabZone->nZoneType == this->m_vecZoneTrait[i].nZoneType&&lpLabZone->nZoneIndex == this->m_vecZoneTrait[i].nZoneIndex)
					{
						std::map<int,int>::iterator it = this->m_vecZoneTrait[i].mapMonsterGen.begin();
						while (true)
						{
							if (it== this->m_vecZoneTrait[i].mapMonsterGen.end())
							{
								break;
							}
							nMonsterClass = it->first;
							nMonsterCount = it->second;
							for (j = 0; j < nMonsterCount; ++j)
							{

								iIndex = gObjAddMonster(lpObj->Map);
								if (iIndex >= 0)
								{
									gObjSetMonster(iIndex, nMonsterClass);
									gMonsterSetBase.GetBoxPosition(
										gObj[aIndex].Map,
										14,
										7,
										30,
										19,
										&gObj[iIndex].X,
										&gObj[iIndex].Y);
									gObj[iIndex].PosNum = -1;
									gObj[iIndex].Map = lpObj->Map;
									gObj[iIndex].TX = gObj[iIndex].X;
									gObj[iIndex].TY = gObj[iIndex].Y;
									gObj[iIndex].OldX = gObj[iIndex].X;
									gObj[iIndex].OldY = gObj[iIndex].Y;
									gObj[iIndex].StartX = gObj[iIndex].X;
									gObj[iIndex].StartY = gObj[iIndex].Y;

									lpMAttr = gMonsterManager.GetInfo(nMonsterClass); 
									if (lpMAttr)
									{
										gObj[iIndex].MaxRegenTime = 1000 * lpMAttr->RegenTime;
									}
									else
									{
										LogAdd(LOG_BLACK, "[LABYRINTH][ERROR] Failed, get monster attribute. (%d)", nMonsterClass);
										gObj[iIndex].MaxRegenTime = 10000;
									}

									gObj[iIndex].Dir = rand() % 8;
									gObj[iIndex].MoveRange = 15;
									this->ReBalanceMonster(lpObj, &gObj[iIndex]);
									g_InstanceSystemMgr->AddInstanceMonster(nInstanceIndex, iIndex);
									
								}
							}
							it++;
						}
						break;
					}
				}
			}
			lpObj->X = 22;
			lpObj->Y = 13;
			lpObj->TX = lpObj->X;
			lpObj->TY = lpObj->Y;
			lpObj->Dir = 3;
			lpObj->PathCount = 0;
			lpObj->Teleport = 0;
			SendData.h.setE(0x77, 0x01, sizeof(SendData));

			SendData.btResult = 0;
			SendData.btMapNumber = 115;
			SendData.btMaxColumn = this->m_vecZoneConfig[nLabConfigNum].nCol;
			SendData.btMaxRow = this->m_vecZoneConfig[nLabConfigNum].nRow;
			SendData.btColumn = lpLabZone->nCol;
			SendData.btRow = lpLabZone->nRow;
			SendData.posX = 22;
			SendData.posY = 13;
			SendData.btMapIndex = lpLabZone->nLabIndex;
			SendData.btZoneType = lpLabZone->nZoneType;
			SendData.btZoneIndex = lpLabZone->nZoneIndex;
			for (n = 0; n < 4; ++n)
				SendData.btGate[n] = lpLabZone->btGate[n];
			lpLabInfo->SetCurrentZone(nMoveZoneNumber);
			DataSend(aIndex, (BYTE *)&SendData, sizeof(SendData));
			gObjViewportListProtocolDestroy(lpObj);
			gViewport.gObjViewportClose(lpObj);
			gViewport.gObjClearViewportOfMine(lpObj);
			gObjViewportListProtocolCreate(lpObj);
			lpObj->RegenMapNumber = lpObj->Map;
			lpObj->RegenMapX = 22;
			lpObj->RegenMapY = 13;
			lpObj->RegenOk = 1;

			LogAdd(LOG_BLACK,
				"[LABYRINTH] [%s][%s](%d) Move the labyrinth zone (configure num = %d). (%d) -> (%d)",
				lpObj->Account,
				lpObj->Name,
				lpObj->Map,
				lpLabInfo->GetConfigNum(),
				nPrevZoneNum,
				lpLabInfo->GetCurrentZone());
			v4 = 0;
		}
	}
	return v4;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
//----- (00EC5DD0) --------------------------------------------------------
int  Labyrinth::Coordi2ZoneNum(int nMaxRow, int nMaxCol, int nRow, int nCol)//ok
{
	return nCol + nMaxCol * nRow;
}

//----- (00EC5E00) --------------------------------------------------------
void  Labyrinth::ZoneNum2Coordi(int nMaxRow, int nMaxCol, int nZoneNum, int *nRow, int *nCol)//ok
{
	*nRow = nZoneNum / nMaxCol;
	*nCol = nZoneNum % nMaxCol;
}

//----- (00EC5E40) --------------------------------------------------------
BOOL  Labyrinth::CheckZoneBoundary( int nMaxRow, int nMaxCol, int nZoneNum, int nGatePosition)//ok
{


	int nCol; // [sp+50h] [bp-Ch]@1
	int nRow; // [sp+54h] [bp-8h]@1

	this->ZoneNum2Coordi(nMaxRow, nMaxCol, nZoneNum, &nRow, &nCol);
	switch (nGatePosition)
	{
	case 0:
		if (nRow - 1 >= 0)
		{
			if (nRow - 1 <= nMaxRow - 1)
			{
				return 1;
			}
		}
		break;
	case 1:
		if (nRow + 1 >= 0)
		{
			if (nRow + 1 <= nMaxRow - 1)
			{
				return 1;
			}
		}

		break;
	case 2:
		if (nCol -1 >= 0)
		{
			if (nCol - 1 <= nMaxCol - 1)
			{
				return 1;
			}
		}
		break;
	case 3:
		if (nCol + 1 >= 0)
		{
			if (nCol + 1 <= nMaxCol - 1)
			{
				return 1;
			}
		}
		
		break;
	default:
		break;
	}
	return 0;
}

//----- (00EC5FD0) --------------------------------------------------------
void  Labyrinth::LoggingLabyrinthInfo( int aIndex)//ok
{

	int i; // [sp+4Ch] [bp-20h]@2
	int nMaxCol; // [sp+50h] [bp-1Ch]@2
	int nMaxRow; // [sp+54h] [bp-18h]@2
	int nConfigNum; // [sp+58h] [bp-14h]@2


	if (!CheckUser(
		aIndex,
		"Labyrinth::LoggingLabyrinthInfo",
		"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",
		41))
	{
		LPOBJ lpObj = &gObj[aIndex];
		LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;

		LogAdd(LOG_BLACK,
			"[LABYRINTH] [%s][%s](%d) Labyrinth Info. Configure(%d), Dim Lv(%d) (%I64d/%I64d)(%d)",
			lpObj->Account,
			lpObj->Name,
			lpObj->Map,
			lpLabInfo->GetConfigNum() + 1,
			lpLabInfo->GetDimensionLevel(),
			lpLabInfo->GetEntireExp(),
			lpLabInfo->GetEntireKillMonCnt(),
			lpLabInfo->GetZonePathCount());
		nConfigNum = lpLabInfo->GetConfigNum();
		nMaxRow = this->m_vecZoneConfig[nConfigNum].nRow;
		nMaxCol = this->m_vecZoneConfig[nConfigNum].nCol;
		for (i = 0; i < 100; ++i)
		{
			if (this->m_vecZoneConfig[nConfigNum].stZone[i].nZoneType)
			{
				LogAdd(LOG_BLACK,
					"[LABYRINTH] [%s][%s](%d) ZoneNum [%d][%d](=%d) Type-Index(%d-%d) [%d][%d][%d][%d] (count:%d)",
					lpObj->Account,
					lpObj->Name,
					lpObj->Map,
					this->m_vecZoneConfig[nConfigNum].stZone[i].nRow,
					this->m_vecZoneConfig[nConfigNum].stZone[i].nCol,
					this->Coordi2ZoneNum(nMaxRow, nMaxCol, this->m_vecZoneConfig[nConfigNum].stZone[i].nRow, this->m_vecZoneConfig[nConfigNum].stZone[i].nCol),
					this->m_vecZoneConfig[nConfigNum].stZone[i].nZoneType,
					this->m_vecZoneConfig[nConfigNum].stZone[i].nZoneIndex,
					this->m_vecZoneConfig[nConfigNum].stZone[i].btGate[0],
					this->m_vecZoneConfig[nConfigNum].stZone[i].btGate[1],
					this->m_vecZoneConfig[nConfigNum].stZone[i].btGate[2],
					this->m_vecZoneConfig[nConfigNum].stZone[i].btGate[3],
					i);
			}
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC62F0) --------------------------------------------------------
void  Labyrinth::LoggingLabyrinthMissionInfo(int aIndex)//ok
{

	LPOBJ lpObj = &gObj[aIndex];
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
	if (!this->CheckUser(
		aIndex,
		"Labyrinth::LoggingLabyrinthMissionInfo",
		"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",
		41))
	{

		lpLabInfo->m_cLabMission.LoggingLabyrinthMission(aIndex);
	}
}

//----- (00EC6380) --------------------------------------------------------
void  Labyrinth::GM_LabyrinthMissionClear( LPOBJ lpObj)
{

	int ClearCnt; // ST94_4@16
	PMSG_NOTIFY_LABYRINTH_RESULT pMsgNotiResult; // [sp+70h] [bp-14Ch]@13
	PMSG_MAIN_MISSION_UPDATE pMsg; // [sp+90h] [bp-12Ch]@12
	_LAB_MISSION *lpLabMission; // [sp+A0h] [bp-11Ch]@7
	_LAB_ZONE *lpLabZone; // [sp+A4h] [bp-118h]@5
	BYTE szMsg[256]; // [sp+A8h] [bp-114h]@5
	int nRet; // [sp+1A8h] [bp-14h]@5

	
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
	if (/*IsInstanceSystemServer() == 1
		&&*/ this->IsLabyrinthMapNumber(lpObj->Map) == 1
		&& lpLabInfo->IsPlaying())
	{

		memset(&szMsg, 0, sizeof(szMsg));

		lpLabZone = this->GetLabyrinthZone(lpLabInfo->GetConfigNum(), lpLabInfo->GetCurrentZone(), &nRet);
		if (nRet)
		{
			//_wsprintfA(szMsg, "미궁 지역 정보를 찾을 수 없습니다.");
			//GCServerMsgStringSend(szMsg, lpObj->m_Index, 1);
		}
		else
		{

			lpLabMission = lpLabInfo->m_cLabMission.GetProgressQuestMission(lpLabInfo->GetCurrentZone(), lpLabZone->nZoneType, &nRet);
			if (nRet)
			{
				//_wsprintfA(szMsg, "미궁 현재 지역의 진행 미션 정보 찾기 실패.");
				//GCServerMsgStringSend(szMsg, lpObj->m_Index, 1);
			}
			else
			{
				lpLabMission->nScoreValue = lpLabMission->nMissionValue;
				lpLabMission->nMissionState = 3;

				lpLabInfo->SendCurrentZoneMissionInfo( lpObj->Index, lpLabInfo->GetCurrentZone());
				if (lpLabZone->nZoneType == 4)
				{
					if (lpLabMission->nMissionState == 3)
					{
						lpLabInfo->m_cLabMission.SelectRewardInfo(lpObj->Index, lpLabMission->nMainMissionOrdering);
						this->GDReqLabyrinthMissionInfoUpdate( lpObj->Index, lpLabMission);
					}
					pMsg.h.set(0x77, 0x07, sizeof(pMsg));

					pMsg.btMainMissionOrderingNum = lpLabMission->nMainMissionOrdering;
					pMsg.btMissionState = lpLabMission->nMissionState;
					pMsg.btMissionType = lpLabMission->nMissionType;
					pMsg.nMissionValue = lpLabMission->nMissionValue;
					pMsg.nScoreValue = lpLabMission->nScoreValue;
					DataSend(lpObj->Index, (BYTE *)&pMsg, pMsg.h.size);
					if (lpLabInfo->m_cLabMission.CheckClearMainMission() == 1)
					{
						pMsgNotiResult.h.set(0x77, 0x14, sizeof(pMsgNotiResult));

						pMsgNotiResult.btLabyrinthLevel = lpLabInfo->GetDimensionLevel();
						pMsgNotiResult.btFailedState = 0;
						pMsgNotiResult.nEntireExp = lpLabInfo->GetEntireExp();
						pMsgNotiResult.nEntireMonsterKillCnt = lpLabInfo->GetEntireKillMonCnt();
						pMsgNotiResult.nTotalRewardRuud = lpLabInfo->m_cLabMission.GetTotalRewardRuud();

						pMsgNotiResult.nTotalJewelOfBless = (signed __int64)(this->GetJewelOfBlessValue(lpLabInfo->GetDimensionLevel()) * (double)lpLabInfo->GetEntireKillMonCnt() / 100.0);

						LogAdd(LOG_BLACK,
							"[LABYRINTH][GM_CHEAT] [%s][%s](%d) Clear Dimension Level(%d). [%d] (Exp: %I64d)(Kill: %I64d) Reward Ruud=%d, Jewel=%d",
							lpObj->Account,
							lpObj->Name,
							lpObj->Map,
							lpLabInfo->GetDimensionLevel(),
							lpLabInfo->GetCurrentZone(),
							lpLabInfo->GetEntireExp(),
							lpLabInfo->GetEntireKillMonCnt(),
							pMsgNotiResult.nTotalRewardRuud,
							pMsgNotiResult.nTotalJewelOfBless);
						if (pMsgNotiResult.nTotalJewelOfBless > 5)
							pMsgNotiResult.nTotalJewelOfBless = 5;

						DataSend(lpObj->Index, (BYTE *)&pMsgNotiResult, pMsgNotiResult.h.size);
						lpLabInfo->m_cLabMission.InsertRewardItem(
							lpObj->Index,
							pMsgNotiResult.nTotalRewardRuud,
							pMsgNotiResult.nTotalJewelOfBless);

						this->GDReqLabyrinthClearLogSave(lpObj->Index, lpLabInfo->GetDimensionLevel());
						if (lpLabInfo->GetDimensionLevel() == 7)
						{
							lpLabInfo->SetClearState(1);
							ClearCnt = lpLabInfo->GetLimitClearCnt() + 1;
							lpLabInfo->SetLimitClearCnt( ClearCnt);
							this->GDReqLabyrinthEndUpdate( lpObj->Index);
						}
					}
				}
			}
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EC6940) --------------------------------------------------------
void  Labyrinth::GM_LabyrintSelectConfiguration( OBJECTSTRUCT *lpObj, int nConfigureNumber)
{

	

	char szMsg[256]; // [sp+50h] [bp-108h]@1


	memset(&szMsg, 0, sizeof(szMsg));
	if (nConfigureNumber >= 1
		&& nConfigureNumber <= this->m_vecZoneConfig.size())
	{
		//if (IsInstanceSystemServer() == 1)
		//{
			if (IsLabyrinthMapNumber(lpObj->Map) == 1)
			{
				//_wsprintfA(szMsg, "미궁 게이트 앞에서 시도 해 주십시오.");
				//GCServerMsgStringSend(szMsg, lpObj->m_Index, 1);
			}
			else
			{
				this->GDReqLabyrinthMissionDelete( lpObj->Index);
				LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
				lpLabInfo->Init();
				lpLabInfo->SetDBInfoLoad(1);
				lpLabInfo->SetConfigNum( nConfigureNumber - 1);

				lpLabInfo->SetDimensionLevel( this->m_vecZoneConfig[nConfigureNumber-1].nLabDimensionLevel);
				this->AddLabyrinthMissionDataSet( lpObj->Index, nConfigureNumber - 1);
				this->GDReqLabyrinthInfoSave( lpObj->Index);
				//_wsprintfA(szMsg, "미궁 구성 번호 %d 설정 완료, 입장 시도 가능.", nConfigureNumber);
				//GCServerMsgStringSend(szMsg, lpObj->m_Index, 1);
			}
		//}
	}
	else
	{
		//v3 = std::vector<_LAB_ZONE_CONFIGURATION, std::allocator<_LAB_ZONE_CONFIGURATION>>::size(&thisa->m_vecZoneConfig);
		//_wsprintfA(szMsg, "구성 번호 초과, (1 ~ %d) 가능", v3);
		//GCServerMsgStringSend(szMsg, lpObj->m_Index, 1);
	}
}

//----- (00EC6B70) --------------------------------------------------------
void  Labyrinth::SendRequestEnterResult( int aIndex, bool bEnterFlag, char btResult)//ok
{
	PMSG_ANS_ENTER_STATE_LABYRINTH pMsg; // [sp+4Ch] [bp-10h]@1
	pMsg.h.set(0x77, 0x0, sizeof(pMsg));

	pMsg.bEnterPossible = bEnterFlag;
	pMsg.btResultType = btResult;
	if (btResult == 2)
		pMsg.dwLeftTime = 10;
	DataSend(aIndex, (BYTE *)&pMsg,pMsg.h.size);
}

//----- (00EC6BE0) --------------------------------------------------------
void  Labyrinth::SendNotifyLabyrinthDimensionEnter( int aIndex)//ok
{
	PMSG_NOTIFY_LABYRINTH_DIMENSION_ENTER pMsg; // [sp+4Ch] [bp-10h]@1
	pMsg.h.set(0x77, 0x15, sizeof(pMsg));
	LPOBJ lpObj = &gObj[aIndex];
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
	pMsg.btLabyrinthLevel = lpLabInfo->GetDimensionLevel();
	pMsg.bIsFinal = pMsg.btLabyrinthLevel == 7;
	DataSend(aIndex, (BYTE *)&pMsg.h, pMsg.h.size);
}
int  Labyrinth::CheckUser(int aIndex, char *szCallerName, char *szFile, int nLine)//ok
{
	int result; // eax@2

	if (gObjIsConnected(aIndex) == 1)
	{
		if (gObj[aIndex].Type == OBJECT_USER)
		{
			result = 0;
		}
		else
		{
			LogAdd(LOG_BLACK,
				"[LABYRINTH][ERROR] (%s)(%d) %s, not user.(%d)(type:%d)",
				szFile,
				nLine,
				szCallerName,
				aIndex,
				gObj[aIndex].Type);
			result = 7;
		}
	}
	else
	{
		LogAdd(LOG_BLACK, "[LABYRINTH][ERROR] (%s)(%d) %s, not connected obj.(%d)", szFile, nLine, szCallerName, aIndex);
		result = 7;
	}
	return result;
}
//----- (00EBCB80) --------------------------------------------------------
bool   Labyrinth::IsLabyrinthMapNumber(unsigned __int16 wMapNumber)//ok
{
	return wMapNumber - INST_MAP_INDEX_LABYRINTH >= 0 && (signed int)wMapNumber < INST_MAP_INDEX_LABYRINTH*2;
}
bool Labyrinth::IsLabyrinthPlayFatiqueLimit(int aIndex)
{
	bool result; // al@2


	if (this->CheckUser(
		aIndex,
		"IsLabyrinthPlayFatiqueLimit",
		"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\labyrinth.h",
		41))
	{
		LogAdd(LOG_BLACK, "[LABYRINTH][ERROR] %s, Not User.", "IsLabyrinthPlayFatiqueLimit");
		return 1;
	}
	LPOBJ lpObj = &gObj[aIndex];
	LabyrinthInfo *lpLabInfo = lpObj->m_cLabyrinthInfo;
	//if (g_bKorPlayTimeLimitOn != 1)
	//	goto LABEL_19;
	//if(gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_MINIMUMVITALITY)
	//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_LOWVITALITY)
	//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_MEDIUMVITALITY)
	//	|| gEffectManager.CheckEffect(lpObj, EFFECT_MUBLUE_HIGHVITALITY))
	//{
	//	return 0;
	//}
	//if (lpObj->m_bPCBangUser)
	//	return 0;
	if (/*IsInstanceSystemServer() != 1
		|| */IsLabyrinthMapNumber(lpObj->Map) != 1
		|| lpLabInfo->IsPlaying() != 1
		/*|| lpObj->m_btFatiguePercent < 100*/)
	{

		result = 0;
	}
	else
	{
		result = 1;
	}
	return result;
}