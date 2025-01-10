#include "stdafx.h"
#include "DataServerProtocol.h"
#include "HuntingRecord.h"
#include "Util.h"
#include "QueryManager.h"
HuntingRecord gHuntingRecord;
HuntingRecord::HuntingRecord()
{
}


HuntingRecord::~HuntingRecord()
{
}
//----- (004609C0) --------------------------------------------------------
void HuntingRecord::GDReqHuntingRecordInfo(SDHP_REQ_HUNTING_RECORD_INFO *lpRecv, int aIndex)
{
	SDHP_ANS_HUNTING_RECORD_COUNT pMsg;
	HUNTING_RECORD_INFO pHuntingRecordInfo;
	BYTE buff[8000] = { 0 };
	int size = sizeof(pMsg);

	pMsg.h.set(0xBC, 0x10, size);

	if (lpRecv)
	{
		pMsg.btMapIndex = lpRecv->btMapIndex;
		pMsg.iUserIndex = lpRecv->iUserIndex;
		pMsg.btCallType = lpRecv->btCallType;
		pMsg.btAnotherUser = lpRecv->btAnotherUser;
		pMsg.btListCnt = 0;

		int bResult = this->LoadHuntingRecordInfo(lpRecv->AccountId, lpRecv->szName, &pHuntingRecordInfo, &pMsg, lpRecv->btMapIndex);

		if (bResult)
		{
			LogAdd(LOG_BLACK,"[HuntingRecordInfo Error - GDReqHuntingRecordInfo] Ret : %d, AccountId : %s, CharName : %s",bResult,lpRecv->AccountId,lpRecv->szName);
		}
		else
		{
			if (pMsg.btListCnt > 0)
			{
				memcpy(&buff[size], &pHuntingRecordInfo, sizeof(pHuntingRecordInfo) * pMsg.btListCnt);
				size += sizeof(pHuntingRecordInfo) * pMsg.btListCnt;
			}

			pMsg.h.set(0xBC, 0x10, size);
			memcpy(buff, &pMsg, sizeof(pMsg));

			gSocketManager.DataSend(aIndex, (BYTE*)buff, size);

			return;
		}
	}
}

// 68372C4: using guessed type void (*LogAddTD)(char *, ...);
// 68372C8: using guessed type void (*LogAddC)(unsigned __int8, char *, ...);

//----- (00460C00) --------------------------------------------------------
void HuntingRecord::GDReqHuntingRecordInfoSave(SDHP_REQ_HUNTING_RECORD_INFO_SAVE *lpRecv)
{

	this->HuntingRecordInfoSave( lpRecv);
}

//----- (00460C40) --------------------------------------------------------
void HuntingRecord::GDReqHuntingRecordInfoUserOpen(SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN *lpRecv, int aIndex)
{

	int bResult; // [sp+D0h] [bp-20h]@3
	SDHP_ANS_HUNTING_RECORD_INFO_USER_OPEN pMsg; // [sp+DCh] [bp-14h]@3


	if (lpRecv)
	{
		pMsg.h.set(0xBC, 0x12, sizeof(pMsg));
		pMsg.iUserIndex = lpRecv->iUserIndex;
		bResult = this->LoadHuntingRecordInfoUserOpen(
			lpRecv->AccountId,
			lpRecv->szName,
			&pMsg);
		if (bResult)
		{
			LogAdd(LOG_BLACK,
				"Error - [GDReqHuntingRecordInfoUserOpen][%s][%s] Error... can't get HuntingRecordInfoUserOpen",
				lpRecv->AccountId,
				lpRecv->szName);
		}
		else
		{
			pMsg.btResult = 1;
			gSocketManager.DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
		}
	}
	else
	{
		LogAdd(LOG_BLACK, "Error - [GDReqHuntingRecordInfoUserOpen] lpRecv is NULL");
	}
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00460D50) --------------------------------------------------------
void HuntingRecord::GDReqHuntingRecordInfoUserOpenSave(SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN_SAVE *lpRecv)
{

	int bResult; // [sp+D0h] [bp-8h]@3

	if (lpRecv)
	{
		bResult = this->HuntingRecordInfoUserOpenSave(lpRecv);
		if (bResult)
			LogAdd(LOG_BLACK,
				"Error - [GDReqHuntingRecordInfoUserOpenSave][%s][%s] Error... HuntingRecordInfoUserOpenSave",
				lpRecv->AccountId,
				lpRecv->szName);
	}
	else
	{
		LogAdd(LOG_BLACK, "Error - [GDReqHuntingRecordInfoUserOpenSave] lpRecv is NULL");
	}
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00460DE0) --------------------------------------------------------
void HuntingRecord::GDReqHuntingRecordInfo_Current(SDHP_REQ_HUNTING_RECORD_INFO_CURRENT *lpRecv, int aIndex)
{
	int iRet;
	SDHP_ANS_HUNTING_RECORD_INFO_CURRENT pMsg;

	if (lpRecv)
	{
		pMsg.btMapIndex = lpRecv->btMapIndex;
		pMsg.iUserIndex = lpRecv->iUserIndex;
		pMsg.btCallType = lpRecv->btCallType;
		iRet = 0;
		iRet = this->LoadHuntingRecordInfo_Current(lpRecv->AccountId,lpRecv->szName,&pMsg,lpRecv->btMapIndex,lpRecv->iYear,lpRecv->btMonth,lpRecv->btDay);
		if (iRet)
		{
			//LogAdd(LOG_BLACK,"[HuntingRecordInfo_Current Error - LoadHuntingRecordInfo_Current] Ret : %d, AccountId : %s, CharName : %s",iRet,lpRecv->AccountId,lpRecv->szName);
			pMsg.btListCnt = 0;
			pMsg.h.set(0xBC, 0x14, sizeof(pMsg));
			gSocketManager.DataSend(aIndex, (BYTE*)&pMsg, sizeof(pMsg));
		}
		else
		{
			pMsg.h.set(0xBC, 0x14, sizeof(pMsg));
			gSocketManager.DataSend(aIndex, (BYTE*)&pMsg,sizeof(pMsg));
		}
	}
}

//----- (00460F20) --------------------------------------------------------
void HuntingRecord::GDReqHuntingRecordInfoDelete(SDHP_REQ_DELETE_HUNTING_RECORD_INFO *lpRecv)
{
	int bResult; // [sp+D0h] [bp-8h]@3
	if (lpRecv)
	{
		bResult = this->DeleteHuntingRecordInfo(
			lpRecv->AccountId,
			lpRecv->szName,
			lpRecv->btMapIndex,
			lpRecv->iYear,
			lpRecv->btMonth,
			lpRecv->btDay);
		if (bResult)
			LogAdd(LOG_BLACK,
				"Error - [GDReqHuntingRecordInfoDelete][%s][%s] Error... ReqHuntingRecordInfoDelete",
				lpRecv->AccountId,
				lpRecv->szName);
	}
	else
	{
		LogAdd(LOG_BLACK, "Error - [GDReqHuntingRecordInfoDelete] lpRecv is NULL");
	}
}

int HuntingRecord::LoadHuntingRecordInfo( char *szAccountID, char *Name, HUNTING_RECORD_INFO *pHuntingRecordInfo, SDHP_ANS_HUNTING_RECORD_COUNT *pMsg, int iMapIndex)
{
	if (gQueryManager.ExecQuery("WZ_HuntingRecordLoad '%s', '%s', %d", szAccountID, Name, iMapIndex) == 0)
	{
		gQueryManager.Close();
		return 1;
	}
	else
	{
		int iCnt = 0;

		while (gQueryManager.Fetch() != SQL_NO_DATA)
		{
			pHuntingRecordInfo[iCnt].btMapIndex = gQueryManager.GetAsInteger("MapIndex");
			pHuntingRecordInfo[iCnt].iYear = gQueryManager.GetAsInteger("mYear");
			pHuntingRecordInfo[iCnt].btMonth = gQueryManager.GetAsInteger("mMonth");
			pHuntingRecordInfo[iCnt].btDay = gQueryManager.GetAsInteger("mDay");
			pHuntingRecordInfo[iCnt].iCurrentLevel = gQueryManager.GetAsInteger("CurrentLevel");
			pHuntingRecordInfo[iCnt].iHuntingAccrueSecond = gQueryManager.GetAsInteger("HuntingAccrueSecond");
			pHuntingRecordInfo[iCnt].i64NormalAccrueDamage = gQueryManager.GetAsInteger("NormalAccrueDamage");
			pHuntingRecordInfo[iCnt].i64PentagramAccrueDamage = gQueryManager.GetAsInteger("PentagramAccrueDamage");
			pHuntingRecordInfo[iCnt].iHealAccrueValue = gQueryManager.GetAsInteger("HealAccrueValue");
			pHuntingRecordInfo[iCnt].iMonsterKillCount = gQueryManager.GetAsInteger("MonsterKillCount");
			pHuntingRecordInfo[iCnt].i64AccrueExp = gQueryManager.GetAsInteger("AccrueExp");
			pHuntingRecordInfo[iCnt].iClass = gQueryManager.GetAsInteger("Class");
			pHuntingRecordInfo[iCnt].iMaxNormalDamage = gQueryManager.GetAsInteger("MaxNormalDamage");
			pHuntingRecordInfo[iCnt].iMinNormalDamage = gQueryManager.GetAsInteger("MinNormalDamage");
			pHuntingRecordInfo[iCnt].iMaxPentagramDamage = gQueryManager.GetAsInteger("MaxPentagramDamage");
			pHuntingRecordInfo[iCnt].iMinPentagramDamage = gQueryManager.GetAsInteger("MinPentagramDamage");
			pHuntingRecordInfo[iCnt].iGetNormalAccrueDamage = gQueryManager.GetAsInteger("GetNormalAccrueDamage");
			pHuntingRecordInfo[iCnt].iGetPentagramAccrueDamage = gQueryManager.GetAsInteger("GetPentagramAccrueDamage");
			iCnt++;
			pMsg->btListCnt = iCnt;
		}

		gQueryManager.Close();
	}

	return 0;
}

//----- (0043DBF0) --------------------------------------------------------
int HuntingRecord::HuntingRecordInfoSave(SDHP_REQ_HUNTING_RECORD_INFO_SAVE *lpRecv)
{
	int result;
	if (lpRecv->btListCnt <= 0)
	{
		result = 1;
	}
	else
	{

		for (int i = 0; i < lpRecv->btListCnt; i++)
		{
			HUNTING_RECORD_INFO *pHuntingRecordInfo = (HUNTING_RECORD_INFO *)(((BYTE*)lpRecv) + sizeof(SDHP_REQ_HUNTING_RECORD_INFO_SAVE) + (sizeof(HUNTING_RECORD_INFO)*i));
			gQueryManager.ExecQuery("WZ_HuntingRecordInfoSave '%s', '%s', %d, %d, %d, %d, %d, %d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %d, %d, %d",
				lpRecv->AccountId,
				lpRecv->szName,
				pHuntingRecordInfo->btMapIndex,
			pHuntingRecordInfo->iYear,
			pHuntingRecordInfo->btMonth,
			pHuntingRecordInfo->btDay,
			pHuntingRecordInfo->iCurrentLevel,
			pHuntingRecordInfo->iHuntingAccrueSecond,
			SET_NUMBERLDW(pHuntingRecordInfo->i64NormalAccrueDamage),
			SET_NUMBERHDW(pHuntingRecordInfo->i64NormalAccrueDamage),
			SET_NUMBERLDW(pHuntingRecordInfo->i64PentagramAccrueDamage),
			SET_NUMBERHDW(pHuntingRecordInfo->i64PentagramAccrueDamage),
			pHuntingRecordInfo->iHealAccrueValue,
			pHuntingRecordInfo->iMonsterKillCount,
			SET_NUMBERLDW(pHuntingRecordInfo->i64AccrueExp),
			SET_NUMBERHDW(pHuntingRecordInfo->i64AccrueExp),
			pHuntingRecordInfo->iClass,
			pHuntingRecordInfo->iMaxNormalDamage,
			pHuntingRecordInfo->iMinNormalDamage,
			pHuntingRecordInfo->iMaxPentagramDamage,
			pHuntingRecordInfo->iMinPentagramDamage,
			pHuntingRecordInfo->iGetNormalAccrueDamage,
			pHuntingRecordInfo->iGetPentagramAccrueDamage);
			
		}
		gQueryManager.Close();
		result = 0;
	}
	return result;
}
// 5CDC14: using guessed type int `CHuntingRecordDBSet::HuntingRecordInfoSave'::`2'::__LINE__Var;
// 68372C8: using guessed type void (*LogAddC)(unsigned __int8, char *, ...);

//----- (0043E180) --------------------------------------------------------
int HuntingRecord::LoadHuntingRecordInfoUserOpen(char *szAccountID, char *Name, SDHP_ANS_HUNTING_RECORD_INFO_USER_OPEN *pMsg)
{
	if (gQueryManager.ExecQuery("WZ_HuntingRecordInfoUserOpenLoad '%s', '%s'", szAccountID, Name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		return 1;
	}
	else
	{
		pMsg->btOpen = gQueryManager.GetAsInteger("HuntingUserOpen");
		gQueryManager.Close();
	}
	return 0;
}
// 5CDC18: using guessed type int `CHuntingRecordDBSet::LoadHuntingRecordInfoUserOpen'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);
// 68372C8: using guessed type void (*LogAddC)(unsigned __int8, char *, ...);

//----- (0043E440) --------------------------------------------------------
int HuntingRecord::HuntingRecordInfoUserOpenSave( SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN_SAVE *lpRecv)
{
	gQueryManager.ExecQuery("WZ_HuntingRecordInfoUserOpenSave '%s', '%s', %d",
		lpRecv->AccountId, lpRecv->szName, lpRecv->btOpen);
	gQueryManager.Close();
	return 0;
}
// 5CDC1C: using guessed type int `CHuntingRecordDBSet::HuntingRecordInfoUserOpenSave'::`2'::__LINE__Var;
// 68372C8: using guessed type void (*LogAddC)(unsigned __int8, char *, ...);

//----- (0043E690) --------------------------------------------------------
int HuntingRecord::LoadHuntingRecordInfo_Current( char *szAccountID, char *Name, SDHP_ANS_HUNTING_RECORD_INFO_CURRENT *pMsg, int iMapIndex, int iYear, char btMonth, char btDay)
{
	if (gQueryManager.ExecQuery("WZ_HuntingRecordLoad_Current '%s', '%s', %d, %d, %d, %d", szAccountID, Name, iMapIndex,iYear,btMonth,btDay)== 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		return 1;
	}
	else
	{

			pMsg->btMapIndex = gQueryManager.GetAsInteger("MapIndex");
			pMsg->iYear = gQueryManager.GetAsInteger("mYear");
			pMsg->btMonth = gQueryManager.GetAsInteger("mMonth");
			pMsg->btDay = gQueryManager.GetAsInteger("mDay");
			pMsg->iCurrentLevel = gQueryManager.GetAsInteger("CurrentLevel");
			pMsg->iHuntingAccrueSecond = gQueryManager.GetAsInteger("HuntingAccrueSecond");
			pMsg->i64NormalAccrueDamage = gQueryManager.GetAsInteger("NormalAccrueDamage");
			pMsg->i64PentagramAccrueDamage = gQueryManager.GetAsInteger("PentagramAccrueDamage");
			pMsg->iHealAccrueValue = gQueryManager.GetAsInteger("HealAccrueValue");
			pMsg->iMonsterKillCount = gQueryManager.GetAsInteger("MonsterKillCount");
			pMsg->i64AccrueExp = gQueryManager.GetAsInteger("AccrueExp");
			pMsg->iClass = gQueryManager.GetAsInteger("Class");
			pMsg->iMaxNormalDamage = gQueryManager.GetAsInteger("MaxNormalDamage");
			pMsg->iMinNormalDamage = gQueryManager.GetAsInteger("MinNormalDamage");
			pMsg->iMaxPentagramDamage = gQueryManager.GetAsInteger("MaxPentagramDamage");
			pMsg->iMinPentagramDamage = gQueryManager.GetAsInteger("MinPentagramDamage");
			pMsg->iGetNormalAccrueDamage = gQueryManager.GetAsInteger("GetNormalAccrueDamage");
			pMsg->iGetPentagramAccrueDamage = gQueryManager.GetAsInteger("GetPentagramAccrueDamage");
			pMsg->btListCnt = 1;
			gQueryManager.Close();
	}
	return 0;
}
// 5CDC20: using guessed type int `CHuntingRecordDBSet::LoadHuntingRecordInfo_Current'::`2'::__LINE__Var;
// 68372C8: using guessed type void (*LogAddC)(unsigned __int8, char *, ...);

//----- (0043EB30) --------------------------------------------------------
int HuntingRecord::DeleteHuntingRecordInfo(char *szAccountID, char *Name, int iMapIndex, int iYear, char btMonth, char btDay)
{
	gQueryManager.ExecQuery("WZ_HuntingRecordDelete '%s', '%s', %d, %d, %d, %d",
		szAccountID, Name, iMapIndex, iYear, btMonth, btDay);
	gQueryManager.Close();
	return 0;
}
// 5CDC24: using guessed type int `CHuntingRecordDBSet::DeleteHuntingRecordInfo'::`2'::__LINE__Var;
// 68372C8: using guessed type void (*LogAddC)(unsigned __int8, char *, ...);