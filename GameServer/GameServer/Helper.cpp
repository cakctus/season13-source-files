#include "stdafx.h"
#include "Helper.h"
#include "CastleSiege.h"
#include "GameMain.h"
#include "Map.h"
#include "MapManager.h"
#include "ServerInfo.h"
#include "Util.h"
#include "HuntingGrounds.h"
#include "Notice.h"
#include "Message.h"
#include "CNixiesLakeBossZone.h"
//#include "LabyrinthMission.h"
//#include "LabyrinthInfo.h"
//#include "Labyrinth.h"

CHelper gHelper;

CHelper::CHelper() // OK
{

}

CHelper::~CHelper() // OK
{

}

void CHelper::MainProc() // OK
{
	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[n];

		if(lpObj->HelperDelayTime != 0)
		{
			int HelperStage = 0;

			if (lpObj->Map >= 0 && lpObj->Map <= 200)
			{
				if (gMapManager.GetMapHelperEnable(lpObj->Map) == 0)
				{
					this->DisableHelper(lpObj);
					continue;
				}
			}

			if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0)
			{
				this->DisableHelper(lpObj);
				continue;
			}

			if((HelperStage=((GetTickCount()-lpObj->HelperTotalTime)/12000000)) >= MAX_HELPER_STAGE)
			{
				this->DisableHelper(lpObj);
				continue;
			}

			/*if (g_Labyrinth.IsLabyrinthMapNumber(lpObj->Map) == 1 && lpObj->Type == OBJECT_USER && ((GetTickCount() - lpObj->LabyrinthHelpTime) >= 60000))
			{
				lpObj->m_cLabyrinthInfo->CheckPlayMission(lpObj->Index, 1, LAB_MISSION_MACRO_TIME, -1);
				lpObj->LabyrinthHelpTime = GetTickCount();
			}*/				

			if((GetTickCount()-lpObj->HelperDelayTime) >= ((DWORD)(gServerInfo.m_HelperActiveDelay*60000)))
			{
				if(lpObj->Money < ((DWORD)((lpObj->Level+lpObj->MasterLevel)*gServerInfo.m_HelperActiveMoney[HelperStage])))
				{
					this->DisableHelper(lpObj);
					continue;
				}

				lpObj->HelperDelayTime = GetTickCount();

				lpObj->Money -= (lpObj->Level+lpObj->MasterLevel)*gServerInfo.m_HelperActiveMoney[HelperStage];

				GCMoneySend(lpObj->Index,lpObj->Money);

				this->GCHelperStartSend(lpObj->Index,((GetTickCount()-lpObj->HelperTotalTime)/60000),((lpObj->Level+lpObj->MasterLevel)*gServerInfo.m_HelperActiveMoney[HelperStage]),0);
			}
		}
	}
}

void CHelper::DisableHelper(LPOBJ lpObj) // OK
{
	if(lpObj->HelperDelayTime == 0)
	{
		return;
	}

	lpObj->LabyrinthHelpTime = 0;

	lpObj->HelperDelayTime = 0;

	lpObj->HelperTotalTime = 0;

	this->GCHelperStartSend(lpObj->Index,0,0,1);
}

void CHelper::CGHelperDataRecv(PMSG_HELPER_DATA_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	this->GDHelperDataSaveSend(aIndex,lpMsg->data);
}

void CHelper::CGHelperStartRecv(PMSG_HELPER_START_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo.m_HelperSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Level < gServerInfo.m_HelperActiveLevel)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage.GetMessage(65),gServerInfo.m_HelperActiveLevel);
		this->GCHelperStartSend(aIndex,0,0,1);
		return;
	}	

	if (lpObj->Map >= 0 && lpObj->Map <= 200)
	{
		if (gMapManager.GetMapHelperEnable(lpObj->Map) == 0)
		{
			return;
		}
	}

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0)
	{
		return;
	}

	if (lpObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpObj->X, lpObj->Y))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aqui nao, papai! Aqui nao.");
		this->GCHelperStartSend(aIndex, 0, 0, 1);
		return;
	}

	if(lpMsg->type == 0)
	{
		lpObj->HelperDelayTime = GetTickCount();

		lpObj->HelperTotalTime = GetTickCount();

		this->GCHelperStartSend(aIndex,0,0,0);

		gCGHuntingGrounds.HuntingRecordMacroStart(aIndex);

		lpObj->LabyrinthHelpTime = GetTickCount();
	}
	else
	{
		lpObj->LabyrinthHelpTime = 0;

		lpObj->HelperDelayTime = 0;

		lpObj->HelperTotalTime = 0;

		this->GCHelperStartSend(aIndex,0,0,1);

		gCGHuntingGrounds.GDReqHuntingRecordInfoSave(lpObj);
		gCGHuntingGrounds.HuntingRecordMacroEnd(aIndex);
	}
}

void CHelper::GCHelperStartSend(int aIndex,int time,int money,int result) // OK
{
	PMSG_HELPER_START_SEND pMsg;

	pMsg.header.set(0xBF,0x51,sizeof(pMsg));

	pMsg.time = time;
	pMsg.money = money;
	pMsg.result = result;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CHelper::DGHelperDataRecv(SDHP_HELPER_DATA_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGHelperDataRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	PMSG_HELPER_DATA_SEND pMsg;

	pMsg.header.set(0xAE,sizeof(pMsg));

	pMsg.result = lpMsg->result;

	memcpy(pMsg.data,lpMsg->data,sizeof(pMsg.data));

	HELPER_DATA* helper = (HELPER_DATA*)lpMsg->data;
	gObj[lpMsg->index].MuHelperSkill = helper->skill;
	memcpy(gObj[lpMsg->index].MuHelperItem, helper->item, sizeof(gObj[lpMsg->index].MuHelperItem));
	for (int i = 0; i < 25; i++)
	{
		if (strlen(gObj[lpMsg->index].MuHelperItem[i]) < 1)
		{
			continue;
		}

		for (int j = 0; j < 25; j++)
		{
			gObj[lpMsg->index].MuHelperItem[i][j] = tolower(gObj[lpMsg->index].MuHelperItem[i][j]);
		}
	}

	DataSend(lpMsg->index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CHelper::GDHelperDataSend(int aIndex) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_HELPER_DATA_SEND pMsg;

	pMsg.header.set(0x17,0x00,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CHelper::GDHelperDataSaveSend(int aIndex,BYTE* data) // OK
{
	SDHP_HELPER_DATA_SAVE_SEND pMsg;

	pMsg.header.set(0x17,0x30,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	memcpy(pMsg.data,data,sizeof(pMsg.data));

	HELPER_DATA* helper = (HELPER_DATA*)data;	
	gObj[aIndex].MuHelperSkill = helper->skill;
	memcpy(gObj[aIndex].MuHelperItem, helper->item, sizeof(gObj[aIndex].MuHelperItem));
	for (int i = 0; i < 25; i++)
	{
		if (strlen(gObj[aIndex].MuHelperItem[i]) < 1)
		{
			continue;
		}

		for (int j = 0; j < 25; j++)
		{
			gObj[aIndex].MuHelperItem[i][j] = tolower(gObj[aIndex].MuHelperItem[i][j]);
		}
	}

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CHelper::GetItem(int aIndex, int item, int mapIndex)
{
	for (int i = 0; i < 25; i++)
	{
		if (strlen(gObj[aIndex].MuHelperItem[i]) < 3)
		{
			continue;
		}

		if (strstr(gItemManager.GetName(item,true), gObj[aIndex].MuHelperItem[i]))
		{
			PMSG_ITEM_GET_RECV fakeMsg;
			fakeMsg.index[0] = SET_NUMBERHB(mapIndex);
			fakeMsg.index[1] = SET_NUMBERLB(mapIndex);

			gItemManager.CGItemGetRecv(&fakeMsg, aIndex);

			return;
		}
	}
}