#include "stdafx.h"
#include "DataServerProtocol.h"
#include "LabyrinthDBSet.h"
#include "Util.h"
#include "QueryManager.h"
LabyrinthDBSet g_LabyrinthDBSet;
LabyrinthDBSet::LabyrinthDBSet()
{
}


LabyrinthDBSet::~LabyrinthDBSet()
{
}


// 58E9F0: using guessed type int (__stdcall *LabyrinthDBSet::`vftable')(int);
//----- (00460FE0) --------------------------------------------------------
void  LabyrinthDBSet::GDReqLabyrinthInfo(SDHP_REQ_LABYRINTH_INFO *lpRecv, int aIndex)
{
	int result; // [sp+D0h] [bp-1B78h]@3
	int nBufferLen; // [sp+DCh] [bp-1B6Ch]@3
	BYTE Buffer[7000]; // [sp+E8h] [bp-1B60h]@3

	if (lpRecv)
	{
		memset(Buffer, 0, sizeof(Buffer));

		nBufferLen = 0;
		result = this->LoadLabyrinthInfo(
			lpRecv->szAccountID,
			lpRecv->szName,
			lpRecv->nUserIndex,
			Buffer,
			&nBufferLen);
		LogAdd(LOG_BLACK,"[GDReqLabyrinthInfo] (Ret: %d)(Send size: %d)", result, nBufferLen);

		gSocketManager.DataSend(aIndex, Buffer, nBufferLen);
	}
	else
	{
		LogAdd(LOG_RED, "Error - [GDReqLabyrinthInfo] lpRecv is NULL");
	}
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00461130) --------------------------------------------------------
void  LabyrinthDBSet::GDReqLabyrinthInfoSave(BYTE *lpRecv)
{

	if (lpRecv)
		this->SaveLabyrinthInfo(lpRecv);
	else
		LogAdd(LOG_RED, "Error - [GDReqLabyrinthInfoSave] lpRecv is NULL");
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00461190) --------------------------------------------------------
void LabyrinthDBSet::GDReqLabyrinthInfoUpdate(SDHP_REQ_LABYRINTH_INFO_UPDATE *lpRecv)
{

	if (lpRecv)
		this->UpdateLabyrinthInfo(lpRecv);
	else
		LogAdd(LOG_RED, "Error - [GDReqLabyrinthInfoUpdate] lpRecv is NULL");
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (004611F0) --------------------------------------------------------
void LabyrinthDBSet::GDReqLabyrinthMissionUpdate(SDHP_REQ_LABYRINTH_MISSION_UPDATE *lpRecv)
{

	if (lpRecv)
		this->UpdateLabyrinthMission(lpRecv);
	else
		LogAdd(LOG_RED, "Error - [GDReqLabyrinthMissionUpdate] lpRecv is NULL");
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00461250) --------------------------------------------------------
void LabyrinthDBSet::GDReqLabyrinthMissionDelete(SDHP_REQ_LABYRINTH_MISSION_DELETE *lpRecv)
{

	if (lpRecv)
		this->DeleteLabyrinthMission(lpRecv->szAccountID, lpRecv->szName);
	else
		LogAdd(LOG_RED, "Error - [GDReqLabyrinthMissionDelete] lpRecv is NULL");
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (004612C0) --------------------------------------------------------
void LabyrinthDBSet::GDReqLabyrinthEndUpdate(SDHP_REQ_LABYRINTH_END_UPDATE *lpRecv)
{

	if (lpRecv)
		this->EndUpdateLabyrinthInfo(
			lpRecv->szAccountID,
			lpRecv->szName,
			lpRecv->nClearCnt,
			lpRecv->btClearState);
	else
		LogAdd(LOG_RED, "Error - [GDReqLabyrinthEndUpdate] lpRecv is NULL");
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00461340) --------------------------------------------------------
void LabyrinthDBSet::GDReqLabyrinthRewardComplete(SDHP_REQ_LABYRINTH_REWARD_COMPLETE *lpRecv)
{

	if (lpRecv)
		this->UpdateLabyrinthRewardState(
			
			lpRecv->szAccountID,
			lpRecv->szName,
			lpRecv->btIsMainMission,
			lpRecv->btRewardCheckState);
	else
		LogAdd(LOG_RED,"Error - [GDReqLabyrinthRewardComplete] lpRecv is NULL");
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (004613C0) --------------------------------------------------------
void LabyrinthDBSet::GDReqLabyrinthSaveClearLog(SDHP_REQ_LABYRINTH_CLEAR_LOG_SET_SAVE *lpRecv)
{

	if (lpRecv)
		this->SaveLabyrinthClearLog(
			lpRecv->szAccountID,
			lpRecv->szName,
			lpRecv->nDimensionLevel);
	else
		LogAdd(LOG_RED,"Error - [GDReqLabyrinthSaveClearLog] lpRecv is NULL");
}
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);
//----- (00440AD0) --------------------------------------------------------
int  LabyrinthDBSet::Connect()
{
	//int result; // eax@2
	//char v2; // [sp+Ch] [bp-CCh]@1
	//LabyrinthDBSet *const thisa; // [sp+D0h] [bp-8h]@1

	//memset(&v2, 0xCCu, 0xCCu);
	//thisa = this;
	//if (CQuery::Connect((CQuery *)this->gap4, 3, szDbConnectDsn, szDbConnectID, szDbConnectPass))
	//{
	//	result = 1;
	//}
	//else
	//{
	//	MsgBox("LabyrinthDBSet ODBC Connect Fail");
	//	result = 0;
	//}
	//return result;
	return 0;
}

//----- (00440B40) --------------------------------------------------------
int  LabyrinthDBSet::LoadLabyrinthInfo(char *szAccountID, char *szName, int nUserIndex, BYTE *Buffer, int *nBufferLen)
{
	SDHP_ANS_LABYRINTH_INFO pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	pMsg.h.set(0x77, 0, sizeof(pMsg));
	pMsg.btResult = 2;
	pMsg.nUserIndex =nUserIndex;
	if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Info_Load '%s', '%s'",szAccountID, szName) == 0)
	{
		gQueryManager.Close();
		memcpy(Buffer, &pMsg, sizeof(pMsg));
		*nBufferLen = sizeof(pMsg);
		return 2;
	}
	else
	{
		int sqlRet = gQueryManager.Fetch();
		if (sqlRet == SQL_NULL_DATA)
		{
			gQueryManager.Close();
			memcpy(Buffer, &pMsg, sizeof(pMsg));
			*nBufferLen = sizeof(pMsg);
			return 2;
		}
		else if (sqlRet == SQL_NO_DATA)
		{
			LogAdd(LOG_BLACK,"[LABYRINTH] LoadLabyrinthInfo, Empty user data. (%s)(%s)", szAccountID, szName);
			gQueryManager.Close();
			pMsg.btResult = 1;
			memcpy(Buffer, &pMsg, sizeof(pMsg));
			*nBufferLen = sizeof(pMsg);
			return 1;
		}
		else
		{
			pMsg.btDimensionLevel = gQueryManager.GetAsInteger("DimensionLevel");

			pMsg.wConfigNum= gQueryManager.GetAsInteger("ConfigNum");

			pMsg.btCurrentZone = gQueryManager.GetAsInteger("CurrentZone");

			pMsg.btVisitedCnt = gQueryManager.GetAsInteger("VisitedCnt");

			pMsg.nEntireExp = gQueryManager.GetAsInteger("EntireExp");

			pMsg.nEntireMonKillCnt = gQueryManager.GetAsInteger("EntireMonKillCnt");

			pMsg.nClearCnt = gQueryManager.GetAsInteger("ClearCnt");

			pMsg.btClearState = gQueryManager.GetAsInteger("ClearState");

			gQueryManager.Close();
			if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Path_Load '%s', '%s'", szAccountID, szName) != 0)
			{
				gQueryManager.Fetch();
				gQueryManager.GetAsBinary("VisitedList", pMsg.btVisitedList, sizeof(pMsg.btVisitedList));
				gQueryManager.Close();
			}

			

			if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Mission_Load '%s', '%s'", szAccountID, szName) == 0)
			{
				gQueryManager.Close();
				memcpy(Buffer, &pMsg, sizeof(pMsg));
				*nBufferLen = sizeof(pMsg);
				return 2;
			}
			else
			{
				int sqlRet = gQueryManager.Fetch();
				if (sqlRet == SQL_NULL_DATA)
				{
					gQueryManager.Close();
					memcpy(Buffer, &pMsg, sizeof(pMsg));
					*nBufferLen = sizeof(pMsg);
					return 2;
				}
				else if (sqlRet == SQL_NO_DATA)
				{
					LogAdd(LOG_BLACK, "[LABYRINTH] LoadLabyrinthInfo, Empty user data. (%s)(%s)", szAccountID, szName);
					gQueryManager.Close();
					pMsg.btResult = (pMsg.btClearState < 1);
					memcpy(Buffer, &pMsg, sizeof(pMsg));
					*nBufferLen = sizeof(pMsg);
					return pMsg.btResult;
				}
				else
				{
					int nsize = sizeof(pMsg);
					int nMissionCnt = 0;
					MISSION_INFO stMissionInfo; // [sp+124h] [bp-17Ch]@16
					while (sqlRet != SQL_NULL_DATA && sqlRet != SQL_NO_DATA)
					{
						stMissionInfo.btZoneNumber = gQueryManager.GetAsInteger("ZoneNumber");
						stMissionInfo.btMissionType = gQueryManager.GetAsInteger("MissionType");
						stMissionInfo.nMissionValue = gQueryManager.GetAsInteger("MissionValue");
						stMissionInfo.nAcquisionValue = gQueryManager.GetAsInteger("AcquisitionValue");
						stMissionInfo.btMissionState = gQueryManager.GetAsInteger("MissionState");
						stMissionInfo.btIsMainMission = gQueryManager.GetAsInteger("IsMainMission");
						stMissionInfo.btMainMissionOrder = gQueryManager.GetAsInteger("MainMissionOrder");
						stMissionInfo.nRewardItemType = gQueryManager.GetAsInteger("RewardItemType");
						stMissionInfo.nRewardItemIndex = gQueryManager.GetAsInteger("RewardItemIndex");
						stMissionInfo.nRewardValue = gQueryManager.GetAsInteger("RewardValue");
						stMissionInfo.btRewardCheckState = gQueryManager.GetAsInteger("RewardCheckState");
						++nMissionCnt;
						memcpy(&Buffer[nsize], &stMissionInfo,sizeof(stMissionInfo));
						nsize += sizeof(stMissionInfo);
						sqlRet = gQueryManager.Fetch();
					}
					gQueryManager.Close();
					pMsg.btMissionCount = nMissionCnt;
					pMsg.h.size[0] = SET_NUMBERHB(nsize);
					pMsg.h.size[1] = SET_NUMBERLB(nsize);
					pMsg.btResult = 0;
					*nBufferLen = nsize;
					memcpy(Buffer, &pMsg, sizeof(pMsg));
					return 0;
				}
			}
		}

	}
}
// 5CDD14: using guessed type int `LabyrinthDBSet::LoadLabyrinthInfo'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);



//----- (004413F0) --------------------------------------------------------
int  LabyrinthDBSet::SaveLabyrinthInfo(BYTE *aRecv)
{

	SDHP_REQ_LABYRINTH_INFO_SAVE *lpRecv = (SDHP_REQ_LABYRINTH_INFO_SAVE *)aRecv;
	gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Info_Save '%s', '%s', %d, %d, %d, %d, ?, %I64d, %I64d, %d, %d",
		lpRecv->szAccountID, 
		lpRecv->szName, 
		lpRecv->btDimensionLevel, 
		lpRecv->wConfigNum, 
		lpRecv->btCurrentZone, 
		lpRecv->btVisitedCnt, 
		SET_NUMBERLDW(lpRecv->nEntireExp),
		SET_NUMBERHDW(lpRecv->nEntireExp),
		SET_NUMBERLDW(lpRecv->nEntireMonKillCnt),
		SET_NUMBERHDW(lpRecv->nEntireMonKillCnt),
		lpRecv->nClearCnt, 
		lpRecv->btClearState);
	gQueryManager.BindParameterAsBinary(1, lpRecv->btVisitedList, sizeof(lpRecv->btVisitedList));
	gQueryManager.Close();
	int nOfs = sizeof(SDHP_REQ_LABYRINTH_INFO_SAVE);
	if (lpRecv->btMissionCount)
	{
		for (int i = 0; i < lpRecv->btMissionCount; i++)
		{
			MISSION_INFO *RecvBuf = (MISSION_INFO *)&aRecv[nOfs];
			if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Mission_Insert '%s', '%s', %d, %d, %d, %d, %d",
				lpRecv->szAccountID,
				lpRecv->szName,
				RecvBuf->btZoneNumber,
				RecvBuf->btMissionType,
				RecvBuf->nMissionValue,
				RecvBuf->btIsMainMission,
				RecvBuf->btMainMissionOrder) == SQL_SUCCESS)
			{
				gQueryManager.Close();
				return 1;
			}
			gQueryManager.Close();
			nOfs += sizeof(MISSION_INFO);
		}

	}
	return 0;
}
// 5CDD18: using guessed type int `LabyrinthDBSet::SaveLabyrinthInfo'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00441690) --------------------------------------------------------
unsigned __int64  LabyrinthDBSet::UpdateLabyrinthInfo(SDHP_REQ_LABYRINTH_INFO_UPDATE *aRecv)
{
	gQueryManager.BindParameterAsBinary(1, aRecv->btVisitedList, sizeof(aRecv->btVisitedList));

	gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Info_Update '%s', '%s', %d, %d, %d, %d, ?, %I64d, %I64d",
		aRecv->szAccountID,
		aRecv->szName,
		aRecv->btDimensionLevel,
		aRecv->wConfigNum,
		aRecv->btCurrentZone,
		aRecv->btVisitedCnt,
		SET_NUMBERLDW(aRecv->nEntireExp),
		SET_NUMBERHDW(aRecv->nEntireExp),
		SET_NUMBERLDW(aRecv->nEntireMonKillCnt),
		SET_NUMBERHDW(aRecv->nEntireMonKillCnt));
	gQueryManager.Close();

	return 0;
}

//----- (004417E0) --------------------------------------------------------
int  LabyrinthDBSet::UpdateLabyrinthMission(SDHP_REQ_LABYRINTH_MISSION_UPDATE *aRecv)
{

	if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Mission_Update '%s', '%s', %d, %d, %d, %d, %d, %d",
		aRecv->szAccountID,
		aRecv->szName,
		aRecv->btZoneNumber,
		aRecv->btMissionType,
		aRecv->nAcquisionValue,
		aRecv->btMissionState,
		aRecv->btIsMainMission,
		aRecv->btMainMissionOrder))
	{
		gQueryManager.Close();

		if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Reward_Update '%s', '%s', %d, %d, %d, %d, %d, %d, %d",
			aRecv->szAccountID,
			aRecv->szName,
			aRecv->btZoneNumber,
			aRecv->btIsMainMission,
			aRecv->btMainMissionOrder,
			aRecv->nRewardItemType,
			aRecv->nRewardItemIndex,
			aRecv->nRewardValue,
			aRecv->btRewardCheckState))
		{
			gQueryManager.Close();
			return 0;
		}
		else
		{
			gQueryManager.Close();
			LogAdd(LOG_RED,"error-L3 : [LABYRINTH] UpdateLabyrinthMission #2");

			return 1;
		}
	}
	else
	{
		gQueryManager.Close();
		LogAdd(LOG_RED, "error-L3 : [LABYRINTH] UpdateLabyrinthMission #1");
		return 1;
	}

}
// 5CDD1C: using guessed type int `LabyrinthDBSet::UpdateLabyrinthMission'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00441A30) --------------------------------------------------------
int  LabyrinthDBSet::DeleteLabyrinthMission(char *szAccountID, char *szName)
{
	
	if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Mission_Delete '%s', '%s'", szAccountID, szName))
	{
		gQueryManager.Close();
		return 0;
	}
	else
	{
		gQueryManager.Close();
		LogAdd(LOG_RED, "error-L3 : [LABYRINTH] DeleteLabyrinthMission");
		return 1;
	}

}
// 5CDD20: using guessed type int `LabyrinthDBSet::DeleteLabyrinthMission'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00441B80) --------------------------------------------------------
int  LabyrinthDBSet::EndUpdateLabyrinthInfo(char *szAccountID, char *szName, int nClearCnt, BYTE btClearState)
{
	if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_End_Update '%s', '%s', %d, %d", szAccountID, szName, nClearCnt, (unsigned __int8)btClearState))
	{
		gQueryManager.Close();
		return 0;
	}
	else
	{
		gQueryManager.Close();
		LogAdd(LOG_RED, "error-L3 : [LABYRINTH] EndUpdateLabyrinthInfom");
		return 1;
	}
}
// 5CDD24: using guessed type int `LabyrinthDBSet::EndUpdateLabyrinthInfo'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00441CE0) --------------------------------------------------------
int  LabyrinthDBSet::UpdateLabyrinthRewardState(char *szAccountID, char *szName, BYTE btIsMainMission, BYTE btRewardCheckState)
{
	if (gQueryManager.ExecQuery("EXEC WZ_Labyrinth_Reward_Complete_Update '%s', '%s', %d, %d", szAccountID, szName, (unsigned __int8)btIsMainMission, (unsigned __int8)btRewardCheckState))
	{
		gQueryManager.Close();
		return 0;
	}
	else
	{
		gQueryManager.Close();
		LogAdd(LOG_RED, "error-L3 : [LABYRINTH] UpdateLabyrinthRewardState");
		return 1;
	}
}
// 5CDD28: using guessed type int `LabyrinthDBSet::UpdateLabyrinthRewardState'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (00441E40) --------------------------------------------------------
int  LabyrinthDBSet::SaveLabyrinthClearLog(char *szAccountID, char *szName, int nDimensionLevel)
{
	if (gQueryManager.ExecQuery("EXEC WZ_LabyrinthClearLogSetSave '%s', '%s', %d", szAccountID, szName, nDimensionLevel))
	{
		gQueryManager.Close();
		return 0;
	}
	else
	{
		gQueryManager.Close();
		LogAdd(LOG_RED, "error-L3 : [LABYRINTH] SaveLabyrinthClearLog");
		return 1;
	}
}
// 5CDD2C: using guessed type int `LabyrinthDBSet::SaveLabyrinthClearLog'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);