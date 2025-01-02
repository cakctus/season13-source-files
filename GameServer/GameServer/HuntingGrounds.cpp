#include "stdafx.h"
#include "MemScript.h"
#include "HuntingGrounds.h"
#include "Util.h"
#include "GameMain.h"

CGHuntingGrounds gCGHuntingGrounds;

CGHuntingGrounds::CGHuntingGrounds(void)
{

}

CGHuntingGrounds::~CGHuntingGrounds(void)
{
}

void CGHuntingGrounds::Initialize_HuntingRecordData()//OK
{

	for (int iClearIndex = 0; iClearIndex < MAX_HUNTING_RECORD_MAP_LIST; ++iClearIndex)
	{
		this->m_HuntingRecordMapList[iClearIndex].Clear();
	}
	for (int iClearIndexa = 0; iClearIndexa < MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST; ++iClearIndexa)
	{
		this->m_HuntingRecordExceptMonsterList[iClearIndexa].Clear();
	}
}

void CGHuntingGrounds::LoadHuntingRecordScript(char* pchFileName)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, pchFileName);
		return;
	}

	if (lpMemScript->SetBuffer(pchFileName) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->Initialize_HuntingRecordData();
	int iValueCount = 0;
	int iValueCount1 = 0;

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
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					this->m_HuntingRecordMapList[iValueCount].iMapIndex = lpMemScript->GetNumber();

					lpMemScript->GetAsString();

					iValueCount++;
				}

				if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					this->m_HuntingRecordExceptMonsterList[iValueCount1].iMonsterIndex = lpMemScript->GetNumber();

					lpMemScript->GetAsString();

					iValueCount1++;
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

void CGHuntingGrounds::CGReqHuntingRecordWndtClose(PMSG_REQ_HUNTING_RECORD_WND_CLOSE* lpMsg, int aIndex)
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	this->HuntingRecordWndClose(aIndex);
}

void CGHuntingGrounds::DGAnsHuntingRecordInfo(SDHP_ANS_HUNTING_RECORD_COUNT* lpRecv)
{
	SYSTEMTIME result;
	GetLocalTime(&result);

	if (gObjIsConnected(lpRecv->iUserIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpRecv->iUserIndex];

	lpObj->m_HuntingRecordCacheData.Clear();

	for (int i = 0; i < lpRecv->btListCnt; ++i)
	{
		HUNTING_RECORD_INFO* info = (HUNTING_RECORD_INFO*)(((BYTE*)lpRecv) + sizeof(SDHP_ANS_HUNTING_RECORD_COUNT) + (sizeof(HUNTING_RECORD_INFO) * i));

		if (!lpRecv->btAnotherUser)
		{
			if (lpObj->m_bMacroStart_HuntingRecord)
			{
				if (lpObj->m_btHuntingRecordReqMapIndex == lpRecv->btMapIndex)
				{
					if (result.wYear == info->iYear)
					{
						if (result.wMonth == info->btMonth)
						{
							if (result.wDay == info->btDay)
								continue;
						}
					}
				}
			}
		}

		if (result.wYear == info->iYear)
		{
			if (result.wMonth == info->btMonth)
			{
				if (result.wDay == info->btDay)
				{
					BOOL v37 = 0;
					for (int j = 0; j < MAX_ST_HUNTING_RECORD_DATA; ++j)
					{
						if (lpObj->m_HuntingRecordData[j].iMapIndex == info->btMapIndex	&& lpObj->m_HuntingRecordData[j].iAnotherRecord == lpRecv->btAnotherUser)
						{
							lpObj->m_HuntingRecordData[j].iMapIndex = info->btMapIndex;
							lpObj->m_HuntingRecordData[j].iYear = info->iYear;
							lpObj->m_HuntingRecordData[j].btMonth = info->btMonth;
							lpObj->m_HuntingRecordData[j].btDay = info->btDay;
							lpObj->m_HuntingRecordData[j].iCurrentLevel = info->iCurrentLevel;
							lpObj->m_HuntingRecordData[j].iHuntingAccrueSecond = info->iHuntingAccrueSecond;
							lpObj->m_HuntingRecordData[j].i64NormalAccrueDamage = info->i64NormalAccrueDamage;
							lpObj->m_HuntingRecordData[j].i64PentagramAccrueDamage = info->i64PentagramAccrueDamage;
							lpObj->m_HuntingRecordData[j].iHealAccrueValue = info->iHealAccrueValue;
							lpObj->m_HuntingRecordData[j].iMonsterKillCount = info->iMonsterKillCount;
							lpObj->m_HuntingRecordData[j].i64GainExp = info->i64AccrueExp;
							lpObj->m_HuntingRecordData[j].iClass = info->iClass;
							lpObj->m_HuntingRecordData[j].iMaxNormalDamage = info->iMaxNormalDamage;
							lpObj->m_HuntingRecordData[j].iMinNormalDamage = info->iMinNormalDamage;
							lpObj->m_HuntingRecordData[j].iAnotherRecord = lpRecv->btAnotherUser;
							v37 = 1;
							break;
						}
					}
					if (!v37)
					{
						for (int k = 0; k < MAX_ST_HUNTING_RECORD_DATA; ++k)
						{
							if (lpObj->m_HuntingRecordData[k].iMapIndex == -1)
							{
								lpObj->m_HuntingRecordData[k].iMapIndex = info->btMapIndex;
								lpObj->m_HuntingRecordData[k].iYear = info->iYear;
								lpObj->m_HuntingRecordData[k].btMonth = info->btMonth;
								lpObj->m_HuntingRecordData[k].btDay = info->btDay;
								lpObj->m_HuntingRecordData[k].iCurrentLevel = info->iCurrentLevel;
								lpObj->m_HuntingRecordData[k].iHuntingAccrueSecond = info->iHuntingAccrueSecond;
								lpObj->m_HuntingRecordData[k].i64NormalAccrueDamage = info->i64NormalAccrueDamage;
								lpObj->m_HuntingRecordData[k].i64PentagramAccrueDamage = info->i64PentagramAccrueDamage;
								lpObj->m_HuntingRecordData[k].iHealAccrueValue = info->iHealAccrueValue;
								lpObj->m_HuntingRecordData[k].iMonsterKillCount = info->iMonsterKillCount;
								lpObj->m_HuntingRecordData[k].i64GainExp = info->i64AccrueExp;
								lpObj->m_HuntingRecordData[k].iClass = info->iClass;
								lpObj->m_HuntingRecordData[k].iMaxNormalDamage = info->iMaxNormalDamage;
								lpObj->m_HuntingRecordData[k].iMinNormalDamage = info->iMinNormalDamage;
								lpObj->m_HuntingRecordData[k].iAnotherRecord = lpRecv->btAnotherUser;
								break;
							}
						}
					}
				}
			}
		}

		lpObj->m_AccrueHuntingRecordData[i].iMapIndex = info->btMapIndex;
		lpObj->m_AccrueHuntingRecordData[i].iYear = info->iYear;
		lpObj->m_AccrueHuntingRecordData[i].btMonth = info->btMonth;
		lpObj->m_AccrueHuntingRecordData[i].btDay = info->btDay;
		lpObj->m_AccrueHuntingRecordData[i].iCurrentLevel = info->iCurrentLevel;
		lpObj->m_AccrueHuntingRecordData[i].iHuntingAccrueSecond = info->iHuntingAccrueSecond;
		lpObj->m_AccrueHuntingRecordData[i].i64NormalAccrueDamage = info->i64NormalAccrueDamage;
		lpObj->m_AccrueHuntingRecordData[i].i64PentagramAccrueDamage = info->i64PentagramAccrueDamage;
		lpObj->m_AccrueHuntingRecordData[i].iHealAccrueValue = info->iHealAccrueValue;
		lpObj->m_AccrueHuntingRecordData[i].iMonsterKillCount = info->iMonsterKillCount;
		lpObj->m_AccrueHuntingRecordData[i].i64GainExp = info->i64AccrueExp;
		lpObj->m_AccrueHuntingRecordData[i].iClass = info->iClass;
		lpObj->m_AccrueHuntingRecordData[i].iMaxNormalDamage = info->iMaxNormalDamage;
		lpObj->m_AccrueHuntingRecordData[i].iMinNormalDamage = info->iMinNormalDamage;
		lpObj->m_AccrueHuntingRecordData[i].iMaxPentagramDamage = info->iMaxPentagramDamage;
		lpObj->m_AccrueHuntingRecordData[i].iMinPentagramDamage = info->iMinPentagramDamage;
		lpObj->m_AccrueHuntingRecordData[i].iGetNormalAccrueDamage = info->iGetNormalAccrueDamage;
		lpObj->m_AccrueHuntingRecordData[i].iGetPentagramAccrueDamage = info->iGetPentagramAccrueDamage;
		lpObj->m_AccrueHuntingRecordData[i].iAnotherRecord = lpRecv->btAnotherUser;
	}

	if (lpRecv->btCallType)
	{
		if (lpRecv->btCallType == 1)
		{
			this->MessageHuntingRecordAccrueInfo(lpRecv->iUserIndex);
		}
	}
	else
	{
		this->GCHuntingRecordInfo_Today(lpRecv->iUserIndex,lpRecv->btMapIndex,lpRecv->btAnotherUser);
		this->GCHuntingRecordAccrueInfo(lpRecv->iUserIndex);
	}
}

void CGHuntingGrounds::DGAnsHuntingRecordInfo_Current(SDHP_ANS_HUNTING_RECORD_INFO_CURRENT* lpRecv)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(lpRecv->iUserIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpRecv->iUserIndex];

	BOOL bPacketError = 0;

	if (lpRecv->btCallType)
	{

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			lpObj->m_AccrueHuntingRecordData[i].Clear();
		}

		for (int k = 0; k < MAX_ST_HUNTING_RECORD_DATA; ++k)
		{
			if (lpObj->m_AccrueHuntingRecordData[k].iMapIndex == -1)
			{
				lpObj->m_AccrueHuntingRecordData[k].iMapIndex = lpRecv->btMapIndex;
				lpObj->m_AccrueHuntingRecordData[k].iYear = lpRecv->iYear;
				lpObj->m_AccrueHuntingRecordData[k].btMonth = lpRecv->btMonth;
				lpObj->m_AccrueHuntingRecordData[k].btDay = lpRecv->btDay;
				lpObj->m_AccrueHuntingRecordData[k].iCurrentLevel = lpRecv->iCurrentLevel;
				lpObj->m_AccrueHuntingRecordData[k].iHuntingAccrueSecond = lpRecv->iHuntingAccrueSecond;
				lpObj->m_AccrueHuntingRecordData[k].i64NormalAccrueDamage = lpRecv->i64NormalAccrueDamage;
				lpObj->m_AccrueHuntingRecordData[k].i64PentagramAccrueDamage = lpRecv->i64PentagramAccrueDamage;
				lpObj->m_AccrueHuntingRecordData[k].iHealAccrueValue = lpRecv->iHealAccrueValue;
				lpObj->m_AccrueHuntingRecordData[k].iMonsterKillCount = lpRecv->iMonsterKillCount;
				lpObj->m_AccrueHuntingRecordData[k].i64GainExp = lpRecv->i64AccrueExp;
				lpObj->m_AccrueHuntingRecordData[k].iClass = lpRecv->iClass;
				lpObj->m_AccrueHuntingRecordData[k].iMaxNormalDamage = lpRecv->iMaxNormalDamage;
				lpObj->m_AccrueHuntingRecordData[k].iMinNormalDamage = lpRecv->iMinNormalDamage;
				lpObj->m_AccrueHuntingRecordData[k].iMaxPentagramDamage = lpRecv->iMaxPentagramDamage;
				lpObj->m_AccrueHuntingRecordData[k].iMinPentagramDamage = lpRecv->iMinPentagramDamage;
				lpObj->m_AccrueHuntingRecordData[k].iGetNormalAccrueDamage = lpRecv->iGetNormalAccrueDamage;
				lpObj->m_AccrueHuntingRecordData[k].iGetPentagramAccrueDamage = lpRecv->iGetPentagramAccrueDamage;
				break;
			}
		}

		this->MessageHuntingRecordAccrueInfo(lpRecv->iUserIndex);
	}
	else
	{
		if (lpRecv->btListCnt <= 0)
		{
			for (int l = 0; l < MAX_ST_HUNTING_RECORD_DATA; ++l)
			{
				if (lpObj->m_HuntingRecordData[l].iMapIndex == lpRecv->btMapIndex)
				{
					bPacketError = 1;
					//LogAdd(LOG_BLACK,"[DGAnsHuntingRecordInfo_Current][%s][%s] B: [0xBC][0x14] Packet Error!!",lpObj->Account,lpObj->Name);
					break;
				}

				if (lpObj->m_HuntingRecordData[l].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[l].iMapIndex = lpRecv->btMapIndex;
					lpObj->m_HuntingRecordData[l].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[l].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[l].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[l].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					lpObj->m_btHuntingRecordReqMapIndex = lpObj->Map;
					break;
				}
			}
		}
		else
		{
			for (int j = 0; j < MAX_ST_HUNTING_RECORD_DATA; ++j)
			{
				if (lpObj->m_HuntingRecordData[j].iMapIndex == lpRecv->btMapIndex)
				{
					bPacketError = 1;
					//LogAdd(LOG_RED, "[DGAnsHuntingRecordInfo_Current][%s][%s] A: [0xBC][0x14] Packet Error!!", lpObj->Account, lpObj->Name);
					break;
				}

				if (lpObj->m_HuntingRecordData[j].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[j].iMapIndex = lpRecv->btMapIndex;
					lpObj->m_HuntingRecordData[j].iYear = lpRecv->iYear;
					lpObj->m_HuntingRecordData[j].btMonth = lpRecv->btMonth;
					lpObj->m_HuntingRecordData[j].btDay = lpRecv->btDay;
					lpObj->m_HuntingRecordData[j].iCurrentLevel = lpRecv->iCurrentLevel;
					lpObj->m_HuntingRecordData[j].iHuntingAccrueSecond = lpRecv->iHuntingAccrueSecond;
					lpObj->m_HuntingRecordData[j].i64NormalAccrueDamage = lpRecv->i64NormalAccrueDamage;
					lpObj->m_HuntingRecordData[j].i64PentagramAccrueDamage = lpRecv->i64PentagramAccrueDamage;
					lpObj->m_HuntingRecordData[j].iHealAccrueValue = lpRecv->iHealAccrueValue;
					lpObj->m_HuntingRecordData[j].iMonsterKillCount = lpRecv->iMonsterKillCount;
					lpObj->m_HuntingRecordData[j].i64GainExp = lpRecv->i64AccrueExp;
					lpObj->m_HuntingRecordData[j].iClass = lpRecv->iClass;
					lpObj->m_HuntingRecordData[j].iMaxNormalDamage = lpRecv->iMaxNormalDamage;
					lpObj->m_HuntingRecordData[j].iMinNormalDamage = lpRecv->iMinNormalDamage;
					lpObj->m_HuntingRecordData[j].iMaxPentagramDamage = lpRecv->iMaxPentagramDamage;
					lpObj->m_HuntingRecordData[j].iMinPentagramDamage = lpRecv->iMinPentagramDamage;
					lpObj->m_HuntingRecordData[j].iGetNormalAccrueDamage = lpRecv->iGetNormalAccrueDamage;
					lpObj->m_HuntingRecordData[j].iGetPentagramAccrueDamage = lpRecv->iGetPentagramAccrueDamage;
					lpObj->m_RecordedSecond = lpRecv->iHuntingAccrueSecond;
					lpObj->m_btHuntingRecordReqMapIndex = lpObj->Map;
					
					//LogAdd(LOG_BLACK,"[DGAnsHuntingRecordInfo_Current][%s][%s] DB Hunting Record Info Load - MapIndex:%d, Date:%d.%d.%d, TimerRandom"":%d, Second:%d, NormalDamage:%I64d, PentagramDamage:%I64d, HealAccrueValue:%d, MonsterKillCount:%d, Accr""ueExp:%I64d \t\t\t\t\t\t\t  , Class:%d, MaxNormalDamage:%d, MinNormalDamage:%d, MaxPentagramDamage:%d, M""inPentagramDamage:%d, GetNormalAccrueDamage:%d, GetPentagramAccrueDamage:%d",
					//	lpObj->Account,lpObj->Name,lpRecv->btMapIndex,lpRecv->iYear,lpRecv->btMonth,lpRecv->btDay,lpRecv->iCurrentLevel,lpRecv->iHuntingAccrueSecond,SET_NUMBERLDW(lpRecv->i64NormalAccrueDamage),SET_NUMBERHDW(lpRecv->i64NormalAccrueDamage),SET_NUMBERLDW(lpRecv->i64PentagramAccrueDamage),SET_NUMBERHDW(lpRecv->i64PentagramAccrueDamage),lpRecv->iHealAccrueValue,lpRecv->iMonsterKillCount,SET_NUMBERLDW(lpRecv->i64AccrueExp),SET_NUMBERHDW(lpRecv->i64AccrueExp),lpRecv->iClass,lpRecv->iMaxNormalDamage,lpRecv->iMinNormalDamage,lpRecv->iMaxPentagramDamage,lpRecv->iMinPentagramDamage,lpRecv->iGetNormalAccrueDamage,lpRecv->iGetPentagramAccrueDamage);
					
					break;
				}
			}
		}
		if (!bPacketError)
		{
			lpObj->m_bMacroStart_HuntingRecord = 1;
			lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
		}
	}
}

void CGHuntingGrounds::CGReqHuntingRecordInfoUserOpen(PMSG_REQ_HUNTING_RECORD_INFO_USER_OPEN* lpMsg, int aIndex)
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	this->HuntingRecordInfoUserOpen(aIndex, lpMsg->btOpen);

}
void  CGHuntingGrounds::CGReqHuntingRecordWndtOpen(PMSG_REQ_HUNTING_RECORD_WND_OPEN* lpMsg, int aIndex)
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	this->HuntingRecordWndOpen(aIndex, lpMsg->btMapIndex, lpMsg->aIndex);
}

void CGHuntingGrounds::CGReqHuntingRecordInfoUserOpenGet(PMSG_REQ_HUNTING_RECORD_INFO_USER_OPEN_GET* lpMsg, int aIndex)
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	this->HuntingRecordInfoUserOpenGet(aIndex, lpMsg->iUserIndex);
}

void CGHuntingGrounds::GDReqHuntingRecordInfo(LPOBJ lpObj, LPOBJ lpTargetObj, char btMapIndex, char btCallType)
{
	int i;
	SDHP_REQ_HUNTING_RECORD_INFO pMsg;

	if (lpObj && lpTargetObj)
	{
		pMsg.h.set(0xBC, 0x10, sizeof(pMsg));
		memcpy(pMsg.AccountId, lpTargetObj->Account, sizeof(pMsg.AccountId));
		memcpy(pMsg.szName, lpTargetObj->Name, sizeof(pMsg.szName));
		pMsg.iUserIndex = lpObj->Index;
		pMsg.btMapIndex = btMapIndex;
		pMsg.btCallType = btCallType;

		if (lpObj->Index == lpTargetObj->Index)
		{
			pMsg.btAnotherUser = 0;
		}
		else
		{
			pMsg.btAnotherUser = 1;
			for (i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
			{
				if (lpObj->m_HuntingRecordData[i].iAnotherRecord == 1)
				{
					lpObj->m_HuntingRecordData[i].Clear();
				}
			}
		}
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
	}
}

void CGHuntingGrounds::GDReqHuntingRecordInfoSave(LPOBJ lpObj)
{
	return;

	if(gObjIsConnectedGP(lpObj->Index) == 0)
	{
		return;
	}

	SDHP_REQ_HUNTING_RECORD_INFO_SAVE pMsg;

	HUNTING_RECORD_INFO info;

	BYTE Buff[2048];

	memcpy(pMsg.AccountId, lpObj->Account, sizeof(pMsg.AccountId));

	memcpy(pMsg.szName, lpObj->Name, sizeof(pMsg.szName));
	pMsg.btListCnt = 0;
	int size = sizeof(pMsg);

	memset(&info, 0, sizeof(info));

	for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
	{
		if (lpObj->m_HuntingRecordData[i].iMapIndex != -1)
		{
			BOOL v6 = 0;
			for (int j = 0; j < MAX_HUNTING_RECORD_MAP_LIST; ++j)
			{
				if (this->m_HuntingRecordMapList[j].iMapIndex == lpObj->m_HuntingRecordData[i].iMapIndex)
					v6 = 1;
			}

			if (v6)
			{
				//for (int k = 0; k < MAX_HUNTING_RECORD_MAP_LIST; ++k)

				info.btMapIndex = lpObj->m_HuntingRecordData[i].iMapIndex;
				info.iYear = lpObj->m_HuntingRecordData[i].iYear;
				info.btMonth = lpObj->m_HuntingRecordData[i].btMonth;
				info.btDay = lpObj->m_HuntingRecordData[i].btDay;
				info.iCurrentLevel = lpObj->m_HuntingRecordData[i].iCurrentLevel;
				info.iHuntingAccrueSecond = lpObj->m_HuntingRecordData[i].iHuntingAccrueSecond;
				info.i64NormalAccrueDamage = lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage;
				info.i64PentagramAccrueDamage = lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage;
				info.iHealAccrueValue = lpObj->m_HuntingRecordData[i].iHealAccrueValue;
				info.iMonsterKillCount = lpObj->m_HuntingRecordData[i].iMonsterKillCount;
				info.i64AccrueExp = lpObj->m_HuntingRecordData[i].i64GainExp;
				info.iClass = lpObj->Class;
				info.iMaxNormalDamage = lpObj->m_HuntingRecordData[i].iMaxNormalDamage;
				info.iMinNormalDamage = lpObj->m_HuntingRecordData[i].iMinNormalDamage;
				info.iMaxPentagramDamage = lpObj->m_HuntingRecordData[i].iMaxPentagramDamage;
				info.iMinPentagramDamage = lpObj->m_HuntingRecordData[i].iMinPentagramDamage;
				info.iGetNormalAccrueDamage = lpObj->m_HuntingRecordData[i].iGetNormalAccrueDamage;
				info.iGetPentagramAccrueDamage = lpObj->m_HuntingRecordData[i].iGetPentagramAccrueDamage;

				memcpy(&Buff[size], &info, sizeof(info));

				size += sizeof(info);

				pMsg.btListCnt++;

				//LogAdd(LOG_BLACK,"[GDReqHuntingRecordInfoSave][%s][%s] DB Save Hunting Record Info - MapIndex:%d, Date:%d.%d.%d, TimerRandom:%d, Sec""ond:%d, NormalDamage:%I64d, PentagramDamage:%I64d, HealValue:%d, MonsterKillCount:%d, AccrueExp:%I64d \t\t\t""\t\t  , Class:%d, MaxNormalDamage:%d, MinNormalDamage:%d, MaxPentagramDamage:%d, MinPentagramDamage:%d, GetN""ormalAccrueDamage:%d, GetPentagramAccrueDamage:%d",
				//	lpObj->Account,lpObj->Name,lpObj->m_HuntingRecordData[i].iMapIndex,lpObj->m_HuntingRecordData[i].iYear,lpObj->m_HuntingRecordData[i].btMonth,lpObj->m_HuntingRecordData[i].btDay,lpObj->m_HuntingRecordData[i].iCurrentLevel,lpObj->m_HuntingRecordData[i].iHuntingAccrueSecond,SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage),SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage),SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage),SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage),lpObj->m_HuntingRecordData[i].iHealAccrueValue,lpObj->m_HuntingRecordData[i].iMonsterKillCount,SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp),SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp),lpObj->Class,lpObj->m_HuntingRecordData[i].iMaxNormalDamage,lpObj->m_HuntingRecordData[i].iMinNormalDamage,lpObj->m_HuntingRecordData[i].iMaxPentagramDamage,lpObj->m_HuntingRecordData[i].iMinPentagramDamage,lpObj->m_HuntingRecordData[i].iGetNormalAccrueDamage,lpObj->m_HuntingRecordData[i].iGetPentagramAccrueDamage);
			}
		}
	}

	if (pMsg.btListCnt > 0)
	{
		pMsg.h.set(0xBC, 0x11, size);
		pMsg.h.size[0] = SET_NUMBERHB(size);
		pMsg.h.size[1] = SET_NUMBERLB(size);
		memcpy(Buff, &pMsg, sizeof(pMsg));
		gDataServerConnection.DataSend(Buff, size);
	}
}

void CGHuntingGrounds::GDReqHuntingRecordInfoUserOpen(LPOBJ lpObj, char* szAccountID)
{
	SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN pMsg; // [sp+4Ch] [bp-24h]@2

	pMsg.h.set(0xBC, 0x12, sizeof(pMsg));

	memcpy(pMsg.AccountId, lpObj->Account, sizeof(pMsg.AccountId));

	memcpy(pMsg.szName, lpObj->Name, sizeof(pMsg.szName));

	pMsg.iUserIndex = lpObj->Index;

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
}

void CGHuntingGrounds::DGAnsHuntingRecordInfoUserOpen(SDHP_ANS_HUNTING_RECORD_INFO_USER_OPEN* lpRecv)
{
	LPOBJ lpObj = &gObj[lpRecv->iUserIndex];

	if (gObjIsConnected(lpObj->Index) == 0)
	{
		return;
	}

	lpObj->m_btHuntingRecordInfoUserOpen = lpRecv->btOpen;

	//LogAdd(LOG_BLACK,"[DGAnsHuntingRecordInfoUserOpen][%s][%s] : Get HuntingRecordInfoUserOpen Flag - %d (0:NotOpen, 1:Open)",lpObj->Account,lpObj->Name,lpRecv->btOpen);
}

void CGHuntingGrounds::GDReqHuntingRecordInfoUserOpenSave(LPOBJ lpObj)
{
	SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN_SAVE pMsg;

	if (lpObj)
	{
		if (lpObj->m_btHuntingRecordInfoUserOpen != 255)
		{
			pMsg.h.set(0xBC, 0x13, sizeof(pMsg));

			memcpy(pMsg.AccountId, lpObj->Account, sizeof(pMsg.AccountId));

			memcpy(pMsg.szName, lpObj->Name, sizeof(pMsg.szName));

			pMsg.iUserIndex = lpObj->Index;

			pMsg.btOpen = lpObj->m_btHuntingRecordInfoUserOpen;

			gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
		}
	}
}

void CGHuntingGrounds::GDReqHuntingRecordInfo_Current(LPOBJ lpObj, LPOBJ lpTargetObj, char btMapIndex, int iYear, char btMonth, char btDay, char btCallType)
{
	SDHP_REQ_HUNTING_RECORD_INFO_CURRENT pMsg; // [sp+4Ch] [bp-30h]@3

	if (lpObj)
	{
		if (lpTargetObj)
		{
			pMsg.h.set(0xBC, 0x14, sizeof(pMsg));

			memcpy(pMsg.AccountId, lpObj->Account, sizeof(pMsg.AccountId));

			memcpy(pMsg.szName, lpObj->Name, sizeof(pMsg.szName));

			pMsg.iUserIndex = lpObj->Index;

			pMsg.btMapIndex = btMapIndex;

			pMsg.iYear = iYear;

			pMsg.btMonth = btMonth;

			pMsg.btDay = btDay;

			pMsg.btCallType = btCallType;

			gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
		}
	}
}

void CGHuntingGrounds::GDReqDeleteHuntingRecordInfo(LPOBJ lpObj, char btMapIndex, int iYear, char btMonth, char btDay)
{
	int i;
	SDHP_REQ_DELETE_HUNTING_RECORD_INFO pMsg;

	if (lpObj)
	{
		pMsg.h.set(0xBC, 0x15, sizeof(pMsg));

		memcpy(pMsg.AccountId, lpObj->Account, sizeof(pMsg.AccountId));

		memcpy(pMsg.szName, lpObj->Name, sizeof(pMsg.szName));

		pMsg.iUserIndex = lpObj->Index;

		pMsg.btMapIndex = btMapIndex;

		pMsg.iYear = iYear;

		pMsg.btMonth = btMonth;

		pMsg.btDay = btDay;

		for (i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == (unsigned __int8)btMapIndex
				&& lpObj->m_HuntingRecordData[i].iYear == iYear
				&& lpObj->m_HuntingRecordData[i].btMonth == (unsigned __int8)btMonth
				&& lpObj->m_HuntingRecordData[i].btDay == (unsigned __int8)btDay)
			{
				lpObj->m_HuntingRecordData[i].Clear();
			}
		}
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
	}
}

void CGHuntingGrounds::GDReqHuntingRecordInfoSave_GM(LPOBJ lpObj, int iMapIndex, int iYear, int btMonth, int btDay, int iSecond, __int64 i64NormalAccrueDamage, __int64 i64PentagramAccrueDamage, __int64 i64GainExp, int iHealAccrueValue, int iMonsterKillCount, int iMaxNormalDamage, int iMinNormalDamage, int iMaxPentagramDamage, int iMinPentagramDamage, int iGetNormalAccrueDamage, int iGetPentagramAccrueDamage)
{
	SDHP_REQ_HUNTING_RECORD_INFO_SAVE pMsg;

	HUNTING_RECORD_INFO info;

	BYTE Buff[2048];

	memcpy(pMsg.AccountId, lpObj->Account, sizeof(pMsg.AccountId));

	memcpy(pMsg.szName, lpObj->Name, sizeof(pMsg.szName));

	int size = sizeof(pMsg);

	if (lpObj)
	{
		if (iMapIndex != -1)
		{
			info.btMapIndex = iMapIndex;
			info.iYear = iYear;
			info.btMonth = btMonth;
			info.btDay = btDay;
			info.iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
			info.iHuntingAccrueSecond = iSecond;
			info.i64NormalAccrueDamage = i64NormalAccrueDamage;
			info.i64PentagramAccrueDamage = i64PentagramAccrueDamage;
			info.iHealAccrueValue = iHealAccrueValue;
			info.iMonsterKillCount = iMonsterKillCount;
			info.i64AccrueExp = i64GainExp;
			info.iClass = lpObj->Class;
			info.iMaxNormalDamage = iMaxNormalDamage;
			info.iMinNormalDamage = iMinNormalDamage;
			info.iMaxPentagramDamage = iMaxPentagramDamage;
			info.iMinPentagramDamage = iMinPentagramDamage;
			info.iGetNormalAccrueDamage = iGetNormalAccrueDamage;
			info.iGetPentagramAccrueDamage = iGetPentagramAccrueDamage;

			memcpy(&Buff[size], &info, sizeof(info));

			size += sizeof(info);

			pMsg.btListCnt++;
		}
		
		if (pMsg.btListCnt > 0)
		{
			pMsg.h.set(0xBC, 0x11, size);

			pMsg.h.size[0] = SET_NUMBERHB(size);

			pMsg.h.size[1] = SET_NUMBERLB(size);

			memcpy(Buff, &pMsg, sizeof(pMsg));

			gDataServerConnection.DataSend(Buff, size);
		}
	}
}

void CGHuntingGrounds::HuntingRecordWndOpen(int aIndex, char btMapIndex, int iTargetIndex)
{
	LPOBJ lpObj;
	LPOBJ lpTargetObj;

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if (gObjIsConnected(iTargetIndex) == 0)
	{
		return;
	}

	lpObj = &gObj[aIndex];

	if (&gObj[aIndex])
	{
		lpTargetObj = &gObj[iTargetIndex];

		if (&gObj[iTargetIndex])
		{
			if (gObj[aIndex].CloseType == -1)
			{
				if (aIndex == iTargetIndex)
				{
					lpObj->m_bHuntingRecordWndOpen = 1;
					lpObj->m_btHuntingRecordWndOpenMapIndex = (unsigned __int8)btMapIndex;
				}

				if (lpObj->m_btHuntingRecordInfoUserOpen == 255)
					lpObj->m_btHuntingRecordInfoUserOpen = 1;

				this->GDReqHuntingRecordInfo(lpObj, lpTargetObj, btMapIndex, 0);
				//LogAdd(LOG_BLACK, "[HuntingRecord][%s][%s] : Hunting Record Window - MapIndex:%d", gObj[aIndex].Account, gObj[aIndex].Name, (unsigned __int8)btMapIndex);
			}
		}
	}
}

void CGHuntingGrounds::HuntingRecordWndClose(int aIndex)
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	lpObj->m_bHuntingRecordWndOpen = 0;

	lpObj->m_btHuntingRecordWndOpenMapIndex = 0;

	//LogAdd(LOG_BLACK, "[HuntingRecord][%s][%s] : Hunting Record Window Close.", lpObj->Account, lpObj->Name);
}

void CGHuntingGrounds::HuntingRecordInfoUserOpen(int aIndex, char btOpen)
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	lpObj->m_btHuntingRecordInfoUserOpen = btOpen;

	this->GDReqHuntingRecordInfoUserOpenSave(lpObj);

	//LogAdd(LOG_BLACK, "[HuntingRecord][%s][%s] : Hunting Record Info User Open : %d(0:NotOpen, 1:Open)", lpObj->Account, lpObj->Name, (unsigned __int8)btOpen);
}

void CGHuntingGrounds::HuntingRecordInfoUserOpenGet(int aIndex, int iUserIndex)
{
	PMSG_ANS_HUNTING_RECORD_INFO_USER_OPEN_GET pMsg;

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if (gObjIsConnected(iUserIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	LPOBJ lpTargetObj = &gObj[iUserIndex];

	pMsg.h.set(0xEC, 0x54, sizeof(pMsg));

	pMsg.btResult = lpTargetObj->m_btHuntingRecordInfoUserOpen;

	DataSend(lpObj->Index, (BYTE*)&pMsg.h, pMsg.h.size);

	//LogAdd(LOG_BLACK,"[HuntingRecord][%s][%s] : Hunting Record Info User Open Get[%s][%s]: %d(0:NotOpen, 1:Open)",lpObj->Account,lpObj->Name,lpTargetObj->Account,lpTargetObj->Name,lpTargetObj->m_btHuntingRecordInfoUserOpen);
}

void CGHuntingGrounds::HuntingRecordMacroStart(int aIndex)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	BYTE bExist = 0;

	for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
	{
		lpObj->m_HuntingRecordData[i].Clear();
	}

	for (int j = 0; j < MAX_HUNTING_RECORD_MAP_LIST; ++j)
	{
		if (this->m_HuntingRecordMapList[j].iMapIndex == lpObj->Map)
		{
			bExist = 1;
		}
	}

	if (bExist)
	{
		this->GDReqHuntingRecordInfo_Current(lpObj, lpObj, lpObj->Map, tCurrentTime.wYear, tCurrentTime.wMonth, tCurrentTime.wDay, 0);
	}

	if (lpObj->m_btHuntingRecordInfoUserOpen == 255)
	{
		lpObj->m_btHuntingRecordInfoUserOpen = 1;
		this->GDReqHuntingRecordInfoUserOpenSave(lpObj);
	}

	lpObj->m_HuntingRecordCacheData.Clear();
}

void CGHuntingGrounds::HuntingRecordMacroEnd(int aIndex)
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
	{
		lpObj->m_HuntingRecordData[i].Clear();
	}

	lpObj->m_HuntingRecordCacheData.Clear();

	lpObj->m_bMacroStart_HuntingRecord = 0;

	lpObj->m_RecordedSecond = 0;

	lpObj->m_dwMacroTick_HuntingRecord_Start = 0;

	lpObj->m_dwMacroTick_HuntingRecord_Process = 0;
}

void CGHuntingGrounds::AddMacroSecond(int aIndex)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->m_bMacroStart_HuntingRecord)
	{
		BOOL bExist = 0;

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->Map
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
			{
				lpObj->m_HuntingRecordData[i].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;

				lpObj->m_HuntingRecordData[i].iHuntingAccrueSecond = lpObj->m_RecordedSecond + (lpObj->m_dwMacroTick_HuntingRecord_Process - lpObj->m_dwMacroTick_HuntingRecord_Start) / 0x3E8;

				if (lpObj->m_HuntingRecordData[i].iHuntingAccrueSecond >= 86397)
				{
					lpObj->m_HuntingRecordData[i].iHuntingAccrueSecond = 86400;
				}

				bExist = 1;
			}
		}

		if (!bExist)
		{
			for (int j = 0; j < MAX_ST_HUNTING_RECORD_DATA; ++j)
			{
				if (lpObj->m_HuntingRecordData[j].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[j].iMapIndex = lpObj->Map;
					lpObj->m_HuntingRecordData[j].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[j].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[j].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[j].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
					lpObj->m_RecordedSecond = 0;
					lpObj->m_HuntingRecordData[j].iHuntingAccrueSecond = 0;

					break;
				}
			}
		}

		if (!bExist)
		{
			lpObj->m_HuntingRecordCacheData.Clear();
		}

		lpObj->m_HuntingRecordCacheData.iHuntingAccrueSecond = (lpObj->m_dwMacroTick_HuntingRecord_Process - lpObj->m_dwMacroTick_HuntingRecord_Start) / 0x3E8;
		this->GCHuntingCacheRecordInfo(lpObj->Index);

		if (lpObj->m_bHuntingRecordWndOpen == 1 && lpObj->m_btHuntingRecordReqMapIndex == lpObj->Map && lpObj->m_btHuntingRecordWndOpenMapIndex == lpObj->Map)
		{
			this->GCHuntingRecordInfo(aIndex);
		}
	}
}

void CGHuntingGrounds::AddNormalDamage(int aIndex, int aTargetIndex, int iDamage)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if (gObjIsConnected(aTargetIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	LPOBJ lpMonsterObj = &gObj[aTargetIndex];

	if (lpObj->m_bMacroStart_HuntingRecord)
	{
		for (int j = 0; j < MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST; ++j)
		{
			if (this->m_HuntingRecordExceptMonsterList[j].iMonsterIndex == lpMonsterObj->Class)
			{
				return;
			}
		}

		BOOL bExist = 0;

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->Map
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
			{

				lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage = lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage + iDamage;

				if (lpObj->m_HuntingRecordData[i].iMaxNormalDamage < iDamage)
				{
					lpObj->m_HuntingRecordData[i].iMaxNormalDamage = iDamage;
				}

				if (lpObj->m_HuntingRecordData[i].iMinNormalDamage)
				{
					if (lpObj->m_HuntingRecordData[i].iMinNormalDamage > iDamage)
					{
						lpObj->m_HuntingRecordData[i].iMinNormalDamage = iDamage;
					}
				}
				else
				{
					lpObj->m_HuntingRecordData[i].iMinNormalDamage = iDamage;
				}

				bExist = 1;
			}
		}

		if (!bExist)
		{
			for (int k = 0; k < MAX_ST_HUNTING_RECORD_DATA; ++k)
			{
				if (lpObj->m_HuntingRecordData[k].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[k].iMapIndex = lpObj->Map;
					lpObj->m_HuntingRecordData[k].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[k].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[k].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[k].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					lpObj->m_HuntingRecordData[k].i64NormalAccrueDamage = iDamage;
					lpObj->m_HuntingRecordData[k].iMaxNormalDamage = iDamage;
					lpObj->m_HuntingRecordData[k].iMinNormalDamage = iDamage;
					lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
					lpObj->m_RecordedSecond = 0;
					lpObj->m_HuntingRecordData[k].iHuntingAccrueSecond = 0;
					break;
				}
			}
		}

		if (!bExist)
			lpObj->m_HuntingRecordCacheData.Clear();

		lpObj->m_HuntingRecordCacheData.i64NormalAccrueDamage = lpObj->m_HuntingRecordCacheData.i64NormalAccrueDamage + iDamage;
	}
}

void CGHuntingGrounds::AddPentagramDamage(int aIndex, int aTargetIndex, int iDamage)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if (gObjIsConnected(aTargetIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	LPOBJ lpMonsterObj = &gObj[aTargetIndex];

	if (lpObj->m_bMacroStart_HuntingRecord)
	{
		for (int j = 0; j < MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST; ++j)
		{
			if (this->m_HuntingRecordExceptMonsterList[j].iMonsterIndex == lpMonsterObj->Class)
			{
				return;
			}
		}

		BOOL bExist = 0;

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->Map
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
			{

				lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage = lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage + iDamage;

				if (lpObj->m_HuntingRecordData[i].iMaxPentagramDamage < iDamage)
				{
					lpObj->m_HuntingRecordData[i].iMaxPentagramDamage = iDamage;
				}

				if (lpObj->m_HuntingRecordData[i].iMinPentagramDamage)
				{
					if (lpObj->m_HuntingRecordData[i].iMinPentagramDamage > iDamage)
					{
						lpObj->m_HuntingRecordData[i].iMinPentagramDamage = iDamage;
					}
				}
				else
				{
					lpObj->m_HuntingRecordData[i].iMinPentagramDamage = iDamage;
				}

				bExist = 1;
			}
		}

		if (!bExist)
		{
			for (int k = 0; k < MAX_ST_HUNTING_RECORD_DATA; ++k)
			{
				if (lpObj->m_HuntingRecordData[k].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[k].iMapIndex = lpObj->Map;
					lpObj->m_HuntingRecordData[k].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[k].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[k].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[k].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					lpObj->m_HuntingRecordData[k].i64PentagramAccrueDamage = iDamage;
					lpObj->m_HuntingRecordData[k].iMaxPentagramDamage = iDamage;
					lpObj->m_HuntingRecordData[k].iMinPentagramDamage = iDamage;
					lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
					lpObj->m_RecordedSecond = 0;
					lpObj->m_HuntingRecordData[k].iHuntingAccrueSecond = 0;
					break;
				}
			}
		}

		if (!bExist)
		{
			lpObj->m_HuntingRecordCacheData.Clear();
		}

		lpObj->m_HuntingRecordCacheData.i64PentagramAccrueDamage = lpObj->m_HuntingRecordCacheData.i64PentagramAccrueDamage + iDamage;
	}
}

//----- (00F0AE70) --------------------------------------------------------
void CGHuntingGrounds::AddGetNormalDamage(int aIndex, int aTargetIndex, int iDamage)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if (gObjIsConnected(aTargetIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	LPOBJ lpMonsterObj = &gObj[aTargetIndex];

	if (lpObj->m_bMacroStart_HuntingRecord)
	{
		for (int j = 0; j < MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST; ++j)
		{
			if (this->m_HuntingRecordExceptMonsterList[j].iMonsterIndex == lpMonsterObj->Class)
			{
				return;
			}
		}

		BOOL bExist = 0;

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->Map
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
			{
				lpObj->m_HuntingRecordData[i].iGetNormalAccrueDamage += iDamage;
				bExist = 1;
			}
		}

		if (!bExist)
		{
			for (int k = 0; k < MAX_ST_HUNTING_RECORD_DATA; ++k)
			{
				if (lpObj->m_HuntingRecordData[k].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[k].iMapIndex = lpObj->Map;
					lpObj->m_HuntingRecordData[k].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[k].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[k].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[k].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					lpObj->m_HuntingRecordData[k].iGetNormalAccrueDamage += iDamage;
					lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
					lpObj->m_RecordedSecond = 0;
					lpObj->m_HuntingRecordData[k].iHuntingAccrueSecond = 0;
					break;
				}
			}
		}

		if (!bExist)
		{
			lpObj->m_HuntingRecordCacheData.Clear();
		}
	}
}

void CGHuntingGrounds::AddGetPentagramDamage(int aIndex, int aTargetIndex, int iDamage)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if (gObjIsConnected(aTargetIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	LPOBJ lpMonsterObj = &gObj[aTargetIndex];

	if (lpObj->m_bMacroStart_HuntingRecord)
	{
		for (int j = 0; j < MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST; ++j)
		{
			if (this->m_HuntingRecordExceptMonsterList[j].iMonsterIndex == lpMonsterObj->Class)
			{
				return;
			}
		}

		BOOL bExist = 0;

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->Map
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
			{
				lpObj->m_HuntingRecordData[i].iGetPentagramAccrueDamage += iDamage;
				bExist = 1;
			}
		}

		if (!bExist)
		{
			for (int k = 0; k < MAX_ST_HUNTING_RECORD_DATA; ++k)
			{
				if (lpObj->m_HuntingRecordData[k].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[k].iMapIndex = lpObj->Map;
					lpObj->m_HuntingRecordData[k].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[k].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[k].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[k].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					lpObj->m_HuntingRecordData[k].iGetPentagramAccrueDamage += iDamage;
					lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
					lpObj->m_RecordedSecond = 0;
					lpObj->m_HuntingRecordData[k].iHuntingAccrueSecond = 0;
					break;
				}
			}
		}

		if (!bExist)
		{
			lpObj->m_HuntingRecordCacheData.Clear();
		}
	}
}

void CGHuntingGrounds::AddHealValue(int aIndex, int iValue)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->m_bMacroStart_HuntingRecord)
	{
		BOOL bExist = 0;

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->Map
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
			{
				lpObj->m_HuntingRecordData[i].iHealAccrueValue += iValue;
				bExist = 1;
			}
		}

		if (!bExist)
		{
			for (int j = 0; j < MAX_ST_HUNTING_RECORD_DATA; ++j)
			{
				if (lpObj->m_HuntingRecordData[j].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[j].iMapIndex = lpObj->Map;
					lpObj->m_HuntingRecordData[j].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[j].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[j].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[j].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					lpObj->m_HuntingRecordData[j].iHealAccrueValue += iValue;
					lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
					lpObj->m_RecordedSecond = 0;
					lpObj->m_HuntingRecordData[j].iHuntingAccrueSecond = 0;
					break;
				}
			}
		}

		if (!bExist)
		{
			lpObj->m_HuntingRecordCacheData.Clear();
		}

		lpObj->m_HuntingRecordCacheData.iHealAccrueValue += iValue;
	}
}

void CGHuntingGrounds::AddMonsterKillCount(int aIndex, LPOBJ lpMonsterObj)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->m_bMacroStart_HuntingRecord)
	{
		for (int j = 0; j < MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST; ++j)
		{
			if (this->m_HuntingRecordExceptMonsterList[j].iMonsterIndex == lpMonsterObj->Class)
			{
				return;
			}
		}

		BOOL bExist = 0;

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->Map
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
			{
				++lpObj->m_HuntingRecordData[i].iMonsterKillCount;
				bExist = 1;
			}
		}

		if (!bExist)
		{
			for (int k = 0; k < MAX_ST_HUNTING_RECORD_DATA; ++k)
			{
				if (lpObj->m_HuntingRecordData[k].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[k].iMapIndex = lpObj->Map;
					lpObj->m_HuntingRecordData[k].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[k].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[k].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[k].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					++lpObj->m_HuntingRecordData[k].iMonsterKillCount;
					lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
					lpObj->m_RecordedSecond = 0;
					lpObj->m_HuntingRecordData[k].iHuntingAccrueSecond = 0;
					break;
				}
			}
		}

		if (!bExist)
		{
			lpObj->m_HuntingRecordCacheData.Clear();
		}

		++lpObj->m_HuntingRecordCacheData.iMonsterKillCount;
	}
}

void CGHuntingGrounds::AddGainExp(int aIndex, int iMonsterType, __int64 i64Exp)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->m_bMacroStart_HuntingRecord)
	{
		for (int j = 0; j < MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST; ++j)
		{
			if (this->m_HuntingRecordExceptMonsterList[j].iMonsterIndex == iMonsterType)
			{
				return;
			}
		}

		BOOL bExist = 0;

		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->Map
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
			{
				lpObj->m_HuntingRecordData[i].i64GainExp += i64Exp;
				bExist = 1;
			}
		}

		if (!bExist)
		{
			for (int k = 0; k < MAX_ST_HUNTING_RECORD_DATA; ++k)
			{
				if (lpObj->m_HuntingRecordData[k].iMapIndex == -1)
				{
					lpObj->m_HuntingRecordData[k].iMapIndex = lpObj->Map;
					lpObj->m_HuntingRecordData[k].iYear = tCurrentTime.wYear;
					lpObj->m_HuntingRecordData[k].btMonth = tCurrentTime.wMonth;
					lpObj->m_HuntingRecordData[k].btDay = tCurrentTime.wDay;
					lpObj->m_HuntingRecordData[k].iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
					lpObj->m_HuntingRecordData[k].i64GainExp += i64Exp;
					lpObj->m_dwMacroTick_HuntingRecord_Start = GetTickCount();
					lpObj->m_RecordedSecond = 0;
					lpObj->m_HuntingRecordData[k].iHuntingAccrueSecond = 0;
					break;
				}
			}
		}

		if (!bExist)
		{
			lpObj->m_HuntingRecordCacheData.Clear();
		}

		lpObj->m_HuntingRecordCacheData.i64GainExp = i64Exp + lpObj->m_HuntingRecordCacheData.i64GainExp;
	}
}

void CGHuntingGrounds::GCHuntingRecordInfo(int aIndex)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);
	PMSG_ANS_HUNTING_RECORD_DATA pMsg;

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	pMsg.btResult = -1;
	pMsg.h.set(0xEC, 0x53, sizeof(pMsg));

	for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
	{
		if (lpObj->m_HuntingRecordData[i].iMapIndex == lpObj->m_btHuntingRecordReqMapIndex && lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear && lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth && lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay)
		{
			pMsg.iYear = lpObj->m_HuntingRecordData[i].iYear;
			pMsg.btMonth = lpObj->m_HuntingRecordData[i].btMonth;
			pMsg.btDay = lpObj->m_HuntingRecordData[i].btDay;
			pMsg.iCurrentLevel = lpObj->MasterLevel + lpObj->Level;
			pMsg.iHuntingAccrueSecond = lpObj->m_HuntingRecordData[i].iHuntingAccrueSecond;
			pMsg.i64NormalAccrueDamage = lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage;
			pMsg.i64PentagramAccrueDamage = lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage;
			pMsg.iHealAccrueValue = lpObj->m_HuntingRecordData[i].iHealAccrueValue;
			pMsg.iMonsterKillCount = lpObj->m_HuntingRecordData[i].iMonsterKillCount;

			pMsg.btExp1 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp2 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp3 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp4 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp5 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp6 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp7 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp8 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
		}
	}

	pMsg.btResult = 1;

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CGHuntingGrounds::GCHuntingRecordInfo_Today(int aIndex, int iMapIndex, int iAnotherUser)
{
	SYSTEMTIME tCurrentTime;
	GetLocalTime(&tCurrentTime);
	PMSG_ANS_HUNTING_RECORD_DATA pMsg;

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	pMsg.btResult = 0;
	pMsg.h.set(0xEC, 0x53, sizeof(pMsg));

	for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
	{
		if (lpObj->m_HuntingRecordData[i].iMapIndex == iMapIndex
			&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
			&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
			&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay
			&& lpObj->m_HuntingRecordData[i].iAnotherRecord == iAnotherUser)
		{
			pMsg.iYear = lpObj->m_HuntingRecordData[i].iYear;
			pMsg.btMonth = lpObj->m_HuntingRecordData[i].btMonth;
			pMsg.btDay = lpObj->m_HuntingRecordData[i].btDay;
			pMsg.iCurrentLevel = lpObj->m_HuntingRecordData[i].iCurrentLevel;
			pMsg.iHuntingAccrueSecond = lpObj->m_HuntingRecordData[i].iHuntingAccrueSecond;
			pMsg.i64NormalAccrueDamage = lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage;
			pMsg.i64PentagramAccrueDamage = lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage;
			pMsg.iHealAccrueValue = lpObj->m_HuntingRecordData[i].iHealAccrueValue;
			pMsg.iMonsterKillCount = lpObj->m_HuntingRecordData[i].iMonsterKillCount;

			pMsg.btExp1 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp2 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp3 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp4 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp5 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp6 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp7 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btExp8 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp)));
			pMsg.btResult = 1;
		}
	}

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (iAnotherUser)
	{
		for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
		{
			if (lpObj->m_HuntingRecordData[i].iMapIndex == iMapIndex
				&& lpObj->m_HuntingRecordData[i].iYear == tCurrentTime.wYear
				&& lpObj->m_HuntingRecordData[i].btMonth == tCurrentTime.wMonth
				&& lpObj->m_HuntingRecordData[i].btDay == tCurrentTime.wDay
				&& lpObj->m_HuntingRecordData[i].iAnotherRecord == iAnotherUser)
			{
				lpObj->m_HuntingRecordData[i].Clear();
			}
		}
	}
}

void CGHuntingGrounds::GCHuntingRecordAccrueInfo(int aIndex)
{
	SYSTEMTIME result;
	GetLocalTime(&result);
	LPOBJ lpObj = &gObj[aIndex];
	BYTE send[3072];

	PMSG_ANS_HUNTING_RECORD_COUNT pMsg;

	_HUNTING_RECORD_ACCRUE_DATA info;

	int size = sizeof(pMsg);
	pMsg.btRecordCnt = 0;

	for (int i = 0; i < MAX_ST_HUNTING_RECORD_DATA; ++i)
	{
		if (lpObj->m_AccrueHuntingRecordData[i].iMapIndex != -1
			&& (lpObj->m_AccrueHuntingRecordData[i].iYear != result.wYear ||
				lpObj->m_AccrueHuntingRecordData[i].btMonth != result.wMonth ||
				lpObj->m_AccrueHuntingRecordData[i].btDay != result.wDay))
		{
			info.iMapIndex = lpObj->m_AccrueHuntingRecordData[i].iMapIndex;
			info.iYear = lpObj->m_AccrueHuntingRecordData[i].iYear;
			info.btMonth = lpObj->m_AccrueHuntingRecordData[i].btMonth;
			info.btDay = lpObj->m_AccrueHuntingRecordData[i].btDay;
			info.iCurrentLevel = lpObj->m_AccrueHuntingRecordData[i].iCurrentLevel;
			info.iHuntingAccrueSecond = lpObj->m_AccrueHuntingRecordData[i].iHuntingAccrueSecond;
			info.i64NormalAccrueDamage = lpObj->m_AccrueHuntingRecordData[i].i64NormalAccrueDamage;
			info.i64PentagramAccrueDamage = lpObj->m_AccrueHuntingRecordData[i].i64PentagramAccrueDamage;
			info.iHealAccrueValue = lpObj->m_AccrueHuntingRecordData[i].iHealAccrueValue;
			info.iMonsterKillCount = lpObj->m_AccrueHuntingRecordData[i].iMonsterKillCount;
			info.btExp1 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp)));
			info.btExp2 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp)));
			info.btExp3 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp)));
			info.btExp4 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp)));
			info.btExp5 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp)));
			info.btExp6 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp)));
			info.btExp7 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp)));
			info.btExp8 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp)));

			memcpy(&send[size], &info, sizeof(info));

			size += sizeof(info);
			pMsg.btRecordCnt++;
			//LogAdd(LOG_BLACK,"[GCHuntingRecordAccrueInfo][%s][%s] : Send Hunting Record - MapIndex:%d, Date:%d.%d.%d, TimerRandom:%d, Second:%d, N""ormalDamage:%I64d, PentagramDamage:%I64d, HealValue:%d, MonsterKillCount:%d, AccrueExp:%I64d",
			//	lpObj->Account,lpObj->Name,lpObj->m_AccrueHuntingRecordData[i].iMapIndex,lpObj->m_AccrueHuntingRecordData[i].iYear,lpObj->m_AccrueHuntingRecordData[i].btMonth,lpObj->m_AccrueHuntingRecordData[i].btDay,lpObj->m_AccrueHuntingRecordData[i].iCurrentLevel,lpObj->m_AccrueHuntingRecordData[i].iHuntingAccrueSecond,SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage),SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64NormalAccrueDamage),SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage),SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64PentagramAccrueDamage),lpObj->m_HuntingRecordData[i].iHealAccrueValue,lpObj->m_HuntingRecordData[i].iMonsterKillCount,SET_NUMBERLDW(lpObj->m_HuntingRecordData[i].i64GainExp),SET_NUMBERHDW(lpObj->m_HuntingRecordData[i].i64GainExp));

		}
	}

	pMsg.btResult = 1;
	pMsg.btUserOpen = lpObj->m_btHuntingRecordInfoUserOpen;

	PHeadSubSetW((LPBYTE)&pMsg, 0xEC, 0x50, size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(lpObj->Index, send, size);
}

void CGHuntingGrounds::LoadHuntingRecord(LPOBJ lpObj, int iMapIndex)
{
	this->GDReqHuntingRecordInfo(lpObj, lpObj, iMapIndex, 1);
}

void CGHuntingGrounds::MessageHuntingRecordAccrueInfo(int aIndex)
{
	char szTemp1[260] = { 0 };
	char szTemp2[260] = { 0 };
	char szTemp3[260] = { 0 };
	char szTemp4[260] = { 0 };

	LPOBJ lpObj = &gObj[aIndex];

	for (int i = 0; i < 60; ++i)
	{
		if (lpObj->m_AccrueHuntingRecordData[i].iMapIndex != -1)
		{
			//wsprintf(szTemp1,"Date:%d.%d.%d, TimerRandom:%d, Second:%d",lpObj->m_AccrueHuntingRecordData[i].iYear,lpObj->m_AccrueHuntingRecordData[i].btMonth,lpObj->m_AccrueHuntingRecordData[i].btDay,lpObj->m_AccrueHuntingRecordData[i].iCurrentLevel,lpObj->m_AccrueHuntingRecordData[i].iHuntingAccrueSecond);
			//wsprintf(szTemp2,"普通伤害:%I64d, 元素伤害:%I64d, 恢复值 :%d, 击杀怪物:%d, 经验:%I64d",SET_NUMBERLDW(lpObj->m_AccrueHuntingRecordData[i].i64NormalAccrueDamage),SET_NUMBERHDW(lpObj->m_AccrueHuntingRecordData[i].i64NormalAccrueDamage),SET_NUMBERLDW(lpObj->m_AccrueHuntingRecordData[i].i64PentagramAccrueDamage),SET_NUMBERHDW(lpObj->m_AccrueHuntingRecordData[i].i64PentagramAccrueDamage),lpObj->m_AccrueHuntingRecordData[i].iHealAccrueValue,lpObj->m_AccrueHuntingRecordData[i].iMonsterKillCount,SET_NUMBERLDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp),SET_NUMBERHDW(lpObj->m_AccrueHuntingRecordData[i].i64GainExp));
			//wsprintf(szTemp3,"Class:%d, 最大普通伤害:%d, 最小普通伤害:%d",lpObj->Class,lpObj->m_AccrueHuntingRecordData[i].iMaxNormalDamage,lpObj->m_AccrueHuntingRecordData[i].iMinNormalDamage);
			//wsprintf(szTemp4,"最大元素伤害:%d, 最小元素伤害:%d, 获取普通伤害:%d, 获取元素伤害:%d",lpObj->m_AccrueHuntingRecordData[i].iMaxPentagramDamage,lpObj->m_AccrueHuntingRecordData[i].iMinPentagramDamage,lpObj->m_AccrueHuntingRecordData[i].iGetNormalAccrueDamage,lpObj->m_AccrueHuntingRecordData[i].iGetPentagramAccrueDamage);
			//this->GCServerMsgStringSend(szTemp1, lpObj->Index, 1);
			//this->GCServerMsgStringSend(szTemp2, lpObj->Index, 1);
			//this->GCServerMsgStringSend(szTemp3, lpObj->Index, 1);
			//this->GCServerMsgStringSend(szTemp4, lpObj->Index, 1);
		}
	}
}

void CGHuntingGrounds::GCHuntingCacheRecordInfo(int aIndex)
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	PMSG_ANS_HUNTING_RECORD_CACHE_DATA pMsg;

	LPOBJ lpObj = &gObj[aIndex];
	pMsg.h.set(0xEC, 0x55, sizeof(pMsg));

	pMsg.iHuntingAccrueSecond = lpObj->m_HuntingRecordCacheData.iHuntingAccrueSecond;

	pMsg.i64NormalAccrueDamage = lpObj->m_HuntingRecordCacheData.i64NormalAccrueDamage;

	pMsg.i64PentagramAccrueDamage = lpObj->m_HuntingRecordCacheData.i64PentagramAccrueDamage;

	pMsg.iHealAccrueValue = lpObj->m_HuntingRecordCacheData.iHealAccrueValue;

	pMsg.iMonsterKillCount = lpObj->m_HuntingRecordCacheData.iMonsterKillCount;

	pMsg.btExp1 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->m_HuntingRecordCacheData.i64GainExp)));
	pMsg.btExp2 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->m_HuntingRecordCacheData.i64GainExp)));
	pMsg.btExp3 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->m_HuntingRecordCacheData.i64GainExp)));
	pMsg.btExp4 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->m_HuntingRecordCacheData.i64GainExp)));
	pMsg.btExp5 = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->m_HuntingRecordCacheData.i64GainExp)));
	pMsg.btExp6 = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->m_HuntingRecordCacheData.i64GainExp)));
	pMsg.btExp7 = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->m_HuntingRecordCacheData.i64GainExp)));
	pMsg.btExp8 = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->m_HuntingRecordCacheData.i64GainExp)));

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
}