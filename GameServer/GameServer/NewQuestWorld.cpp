#include "StdAfx.h"
#include "DefaultClassInfo.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "NewQuestWorld.h"
#include "MemScript.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "Util.h"
#include "Log.h"
#include "Quest.h"

NewQuestWorld gNewQuestWorld;

NewQuestWorld::NewQuestWorld(void)
{
	this->m_NewQuestWorldInfo.clear();
}

NewQuestWorld::~NewQuestWorld(void)
{
}

void NewQuestWorld::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_NewQuestWorldInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			NEW_QUEST_INFO info;

			memset(&info, 0, sizeof(info));

			info.Index = lpMemScript->GetNumber();

			strcpy_s(info.Name, lpMemScript->GetAsString());


			info.Accept.TemplateType = lpMemScript->GetAsNumber();
			info.Accept.QuestChapter = lpMemScript->GetAsNumber();
			info.Accept.QuestCategory = lpMemScript->GetAsNumber();
			info.Accept.QuestImportance = lpMemScript->GetAsNumber();
			info.Accept.QuestStartType = lpMemScript->GetAsNumber();
			info.Accept.StartSubType = lpMemScript->GetAsNumber();
			info.Accept.LevelMin = lpMemScript->GetAsNumber();
			info.Accept.QuestRepeatCycle = lpMemScript->GetAsNumber();
			info.Accept.PrecedenceQuest = lpMemScript->GetAsNumber();
			info.Accept.QuestStartItemType = lpMemScript->GetAsNumber();
			info.Accept.QuestStartItemIndex = lpMemScript->GetAsNumber();
			info.Accept.QuestCheckGens = lpMemScript->GetAsNumber();

			for (int i = 0; i < MAX_CLASS; i++)
			{
				info.Accept.Class[i] = lpMemScript->GetAsNumber();
			}

			info.Accept.StartText = lpMemScript->GetAsNumber();
			info.Accept.StartErrorText = lpMemScript->GetAsNumber();

			info.TaskProgress.TemplateType = lpMemScript->GetAsNumber();
			info.TaskProgress.QuestType = lpMemScript->GetAsNumber();
			info.TaskProgress.QMainType = lpMemScript->GetAsNumber();
			info.TaskProgress.QSubType = lpMemScript->GetAsNumber();
			info.TaskProgress.TagetNumber = lpMemScript->GetAsNumber();
			info.TaskProgress.TargetMobMaxLv = lpMemScript->GetAsNumber();
			info.TaskProgress.DropRate = lpMemScript->GetAsNumber();
			info.TaskProgress.GateNumber = lpMemScript->GetAsNumber();
			info.TaskProgress.MapNumber = lpMemScript->GetAsNumber();
			info.TaskProgress.CoordinateX = lpMemScript->GetAsNumber();
			info.TaskProgress.CoordinateY = lpMemScript->GetAsNumber();
			info.TaskProgress.RewardExp = lpMemScript->GetAsNumber();
			info.TaskProgress.RewardZen = lpMemScript->GetAsNumber();

			for (int i = 0; i < 3; i++)
			{
				info.TaskProgress.ItemAward[i].RewardItemType = lpMemScript->GetAsNumber();
				info.TaskProgress.ItemAward[i].RewardItemIndex = lpMemScript->GetAsNumber();
				info.TaskProgress.ItemAward[i].RewardItemCount = lpMemScript->GetAsNumber();
			}

			info.TaskProgress.RewardType = lpMemScript->GetAsNumber();
			info.TaskProgress.RewardSubType = lpMemScript->GetAsNumber();
			info.TaskProgress.RewardNumber = lpMemScript->GetAsNumber();
			info.TaskProgress.ProgressText = lpMemScript->GetAsNumber();
			info.TaskProgress.CompletetText = lpMemScript->GetAsNumber();

			this->m_NewQuestWorldInfo.insert(std::pair<int, NEW_QUEST_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool NewQuestWorld::GetInfo(int index, NEW_QUEST_INFO* lpInfo)
{
	std::map<int, NEW_QUEST_INFO>::iterator it = this->m_NewQuestWorldInfo.find(index);

	if (it == this->m_NewQuestWorldInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

void NewQuestWorld::GCNewQuestWorldLoad(LPOBJ lpObj)
{
	//LogAdd(LOG_DEBUG, "%s %d", __FUNCTION__, __LINE__);

	PMSG_NEW_QUEST_WORLD_LOAD pMsg;
	pMsg.header.set(0xF6, 0x20, sizeof(pMsg));
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	
	this->GDNewQuestWorldSend(lpObj->Index);
}

void NewQuestWorld::GDNewQuestWorldSend(int aIndex) // OK
{
	//LogAdd(LOG_DEBUG, "%s %d Index: %d", __FUNCTION__, __LINE__, aIndex);

	if (gObjIsAccountValid(aIndex, gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_NEW_QUEST_WORLD_SEND pMsg;

	pMsg.header.set(0x10, 0x40, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, gObj[aIndex].Account, sizeof(pMsg.account));

	memcpy(pMsg.name, gObj[aIndex].Name, sizeof(pMsg.name));

	//LogAdd(LOG_DEBUG, "%s %d Name: %s", __FUNCTION__, __LINE__, pMsg.name);

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void NewQuestWorld::DGNewQuestWorldRecv(SDHP_NEW_QUEST_WORLD_RECV* lpMsg) // OK
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGNewQuestWorldRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);

		CloseClient(lpMsg->index);
		return;
	}

	//LogAdd(LOG_DEBUG, "%s %d", __FUNCTION__,__LINE__);

	LPOBJ lpObj = &gObj[lpMsg->index];

	//LogAdd(LOG_DEBUG, "%s %d Index: %d", __FUNCTION__, __LINE__, lpMsg->index);

	memcpy(lpObj->NewQuestWorldList, &lpMsg->Quest, sizeof(lpMsg->Quest));

	this->GCNewQuestWorldInterfaceListSend(lpObj);
}

void NewQuestWorld::GCNewQuestWorldInterfaceListSend(LPOBJ lpObj) // OK
{
	//LogAdd(LOG_DEBUG, "%s %d", __FUNCTION__, __LINE__);

	PMSG_NEW_QUEST_WORLD_INDEX pMsg;

	pMsg.header.set(0xF6, 0x50, sizeof(pMsg));

	if (lpObj->NewQuestWorldList->QuestIndex == 0xFFFF)
	{
		NEW_QUEST_INFO lpInfo;
		int index = 11;
		while (true)
		{
			this->GetInfo(index, &lpInfo);

			if (lpInfo.Accept.Class[lpObj->Class] == 1)
			{
				break;
			}
			index++;
		}

		lpObj->NewQuestWorldList->QuestIndex = lpInfo.Index;
		lpObj->NewQuestWorldList->TagetNumber = 0;
		pMsg.Quest.QuestIndex = lpInfo.Index;
		pMsg.Quest.TagetNumber = 0;
		pMsg.Quest.QuestState = 1;

		//LogAdd(LOG_DEBUG, "%s %d", __FUNCTION__, __LINE__);
	}
	else
	{
		pMsg.Quest.QuestIndex = lpObj->NewQuestWorldList->QuestIndex;
		pMsg.Quest.TagetNumber = lpObj->NewQuestWorldList->TagetNumber;
		pMsg.Quest.QuestState = lpObj->NewQuestWorldList->QuestState;

		//LogAdd(LOG_DEBUG, "%s %d", __FUNCTION__, __LINE__);
	}

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void NewQuestWorld::GDNewQuestWorldSaveSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->NewQuestWorldList->QuestIndex == 0xFFFF)
	{
		//reiniciar servidor normal pra testar
		return;
	}

	SDHP_NEW_QUEST_WORLD_SAVE_SEND pMsg;

	pMsg.header.set(0x10, 0x41, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	memcpy(&pMsg.Quest, lpObj->NewQuestWorldList, sizeof(pMsg.Quest));

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

void NewQuestWorld::CGNewQuestWorldGoNow(PMSG_NEW_QUEST_WORLD_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_NEW_QUEST_WORLD_SEND pMsg;

	NEW_QUEST_INFO lpInfo;

	pMsg.header.set(0xF6, 0x56, sizeof(pMsg));

	if (this->GetInfo(lpMsg->Quest.QuestIndex, &lpInfo) == 0)
	{
		LogAdd(LOG_RED, "Error:[%d] Without this task", lpMsg->Quest.QuestIndex);
		return;
	}

	if (lpObj->Map != lpInfo.TaskProgress.MapNumber)
	{
		gObjMoveGate(aIndex, lpInfo.TaskProgress.GateNumber);
	}

	pMsg.Quest.QuestIndex = lpObj->NewQuestWorldList->QuestIndex;
	pMsg.Quest.TagetNumber = lpObj->NewQuestWorldList->TagetNumber;
	pMsg.Quest.QuestState = lpObj->NewQuestWorldList->QuestState;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void NewQuestWorld::CGNewQuestWorldButtonMove(PMSG_NEW_QUEST_WORLD_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_NEW_QUEST_WORLD_SEND pMsg;

	NEW_QUEST_INFO lpInfo;

	pMsg.header.set(0xF6, 0x50, sizeof(pMsg));

	if (this->GetInfo(lpMsg->Quest.QuestIndex, &lpInfo) == 0)
	{
		LogAdd(LOG_RED, "Error:[%d] Without this task", lpMsg->Quest.QuestIndex);
		return;
	}

	switch (lpInfo.TaskProgress.QuestType)
	{
	case COLLECT:
		break;
	case KILL:
		break;
	case LEVEL:
		break;
	case TRANSFER:
		break;
	case SEEK:
		lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_SUCCESS;
		break;
	default:
		break;
	}

	pMsg.Quest.QuestIndex = lpObj->NewQuestWorldList->QuestIndex;
	pMsg.Quest.TagetNumber = lpObj->NewQuestWorldList->TagetNumber;
	pMsg.Quest.QuestState = lpObj->NewQuestWorldList->QuestState;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void NewQuestWorld::NewQuestComplete(LPOBJ lpObj, int state)
{
	PMSG_NEW_QUEST_COMPLETE_SEND pMsg;

	pMsg.header.set(0xF6, 0x51, sizeof(pMsg));

	pMsg.state = state;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void NewQuestWorld::CGNewQuestWorldGiveUpChapter(PMSG_NEW_QUEST_WORLD_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_NEW_QUEST_WORLD_SEND pMsg;
	NEW_QUEST_INFO lpInfo;
	pMsg.header.set(0xF6, 0x50, sizeof(pMsg));

	lpObj->NewQuestWorldList->QuestIndex = lpMsg->Quest.QuestIndex;
	lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_CANCEL;

	this->NewQuestComplete(lpObj, 4);

	pMsg.Quest.QuestIndex = lpObj->NewQuestWorldList->QuestIndex;
	pMsg.Quest.TagetNumber = 0;
	pMsg.Quest.QuestState = lpObj->NewQuestWorldList->QuestState;
	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));

	this->GCUpdateNewQuestWorld(lpObj);
}

void NewQuestWorld::CGNewQuestWorldGiveUp(PMSG_NEW_QUEST_WORLD_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_NEW_QUEST_WORLD_SEND pMsg;
	NEW_QUEST_INFO lpInfo;
	pMsg.header.set(0xF6, 0x50, sizeof(pMsg));

	lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_CANCEL;

	this->NewQuestComplete(lpObj, 4);

	pMsg.Quest.QuestIndex = lpObj->NewQuestWorldList->QuestIndex;
	pMsg.Quest.TagetNumber = 0;
	pMsg.Quest.QuestState = lpObj->NewQuestWorldList->QuestState;
	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));

	this->GCUpdateNewQuestWorld(lpObj);
}

void NewQuestWorld::CGNewQuestWorldAward(PMSG_NEW_QUEST_WORLD_RECV* lpMsg, int aIndex)
{
	//LogAdd(LOG_DEBUG, "%s %d",__FUNCTION__,__LINE__);

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_NEW_QUEST_WORLD_SEND pMsg;

	NEW_QUEST_INFO lpInfo;

	pMsg.header.set(0xF6, 0x50, sizeof(pMsg));

	if (this->GetInfo(lpMsg->Quest.QuestIndex, &lpInfo) == 0)
	{
		LogAdd(LOG_RED, "Error:[%d] Without this task", lpMsg->Quest.QuestIndex);
		return;
	}

	if (lpMsg->Quest.QuestIndex != lpObj->NewQuestWorldList->QuestIndex)
	{
		return;
	}

	if ((lpObj->Level + lpObj->MasterLevel) < lpInfo.Accept.LevelMin)
	{
		return;
	}

	switch (lpInfo.TaskProgress.QuestType)
	{
	case COLLECT:
		if ((lpObj->NewQuestWorldList->TagetNumber = this->GetNewQuestItem(lpObj, GET_ITEM(lpInfo.TaskProgress.QMainType, lpInfo.TaskProgress.QSubType))) >= lpInfo.TaskProgress.TagetNumber)
		{
			lpObj->NewQuestWorldList->TagetNumber = lpInfo.TaskProgress.TagetNumber;
			lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_FINISH;
		}
		break;
	case KILL:
		if (lpObj->NewQuestWorldList->TagetNumber >= lpInfo.TaskProgress.TagetNumber)
		{
			lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_FINISH;
		}
		break;
	case LEVEL:
		if ((lpObj->Level + lpObj->MasterLevel) >= lpInfo.TaskProgress.QMainType)
		{
			lpObj->NewQuestWorldList->TagetNumber = (lpObj->Level + lpObj->MasterLevel);
			lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_FINISH;
		}
		break;
	case TRANSFER:
		lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_FINISH;
		break;
	case SEEK:
		lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_FINISH;
		break;
	default:
		break;
	}

	if (lpInfo.TaskProgress.RewardExp != -1)
	{
		if (gObjectManager.CharacterLevelUp(lpObj, lpInfo.TaskProgress.RewardExp, gServerInfo.m_MaxLevelUp, EXPERIENCE_COMMON) == 0)
		{
			GCRewardExperienceSend(lpObj->Index, lpInfo.TaskProgress.RewardExp);
		}
	}

	if (lpInfo.TaskProgress.RewardZen != -1)
	{
		if (gObjCheckMaxMoney(aIndex, lpInfo.TaskProgress.RewardZen) == 0)
		{
			lpObj->Money = MAX_MONEY;
		}
		else
		{
			lpObj->Money += lpInfo.TaskProgress.RewardZen;
		}

		GCMoneySend(lpObj->Index, lpObj->Money);
	}

	for (int i = 0; i < MAX_ITEM_AWARD; i++)
	{
		if (lpInfo.TaskProgress.ItemAward[i].RewardItemType != -1 && lpInfo.TaskProgress.ItemAward[i].RewardItemIndex != -1)
		{
			PMSG_ITEM_MODIFY_SEND pMsgItem;

			pMsgItem.header.set(0xF3, 0x14, sizeof(pMsgItem));

			pMsgItem.slot = 0xFF;

			GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, 
				GET_ITEM(lpInfo.TaskProgress.ItemAward[i].RewardItemType, lpInfo.TaskProgress.ItemAward[i].RewardItemIndex), 
				0, 1, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, 0, 0, 0);

			this->NewQuestComplete(lpObj, 3);

			DataSend(lpObj->Index, (BYTE*)&pMsgItem, pMsgItem.header.size);
		}
	}

	this->NewQuestComplete(lpObj, 1);

	pMsg.Quest.QuestIndex = lpObj->NewQuestWorldList->QuestIndex;
	pMsg.Quest.TagetNumber = 0;
	pMsg.Quest.QuestState = lpObj->NewQuestWorldList->QuestState;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));

	this->GCUpdateNewQuestWorld(lpObj);
}

void NewQuestWorld::GCUpdateNewQuestWorld(LPOBJ lpObj)
{
	//LogAdd(LOG_DEBUG, "%s %d", __FUNCTION__, __LINE__);

	NEW_QUEST_INFO lpInfo;

	for (int i = 0; i < MAX_NEW_QUEST_WORLD; i++)
	{
		if (this->GetInfo(lpObj->NewQuestWorldList->QuestIndex + i, &lpInfo))
		{
			if (lpInfo.Index <= lpObj->NewQuestWorldList->QuestIndex)
			{
				continue;
			}

			if (lpInfo.Accept.Class[lpObj->Class] == 0)
			{
				continue;
			}

			lpObj->NewQuestWorldList->QuestIndex = lpInfo.Index;
			lpObj->NewQuestWorldList->QuestState = 1;
			lpObj->NewQuestWorldList->TagetNumber = 0;

			return;
		}
	}
}

int NewQuestWorld::GetNewQuestItem(LPOBJ lpObj, int ItemIndex)
{
	int value = 0;

	for (int i = INVENTORY_WEAR_SIZE; i < INVENTORY_SIZE; i++)
	{
		if (lpObj->Inventory[i].m_Index == ItemIndex)
		{
			value += lpObj->Inventory[i].m_Durability;
		}
	}

	return value;
}

BOOL NewQuestWorld::GetTagetNumber(int Taget, int aIndex, int Type)
{
	LPOBJ lpObj = &gObj[aIndex];

	NEW_QUEST_INFO lpInfo;

	if (!lpObj->NewQuestWorldList)
	{
		return FALSE;
	}

	if (this->GetInfo(lpObj->NewQuestWorldList->QuestIndex, &lpInfo))
	{
		if (Type == KILL)
		{
			if (lpObj->NewQuestWorldList->QuestIndex != -1)
			{
				if (lpInfo.TaskProgress.QMainType == Taget)
				{
					return TRUE;
				}
			}
		}

		if (Type == COLLECT)
		{
			if (lpObj->NewQuestWorldList->QuestIndex != -1)
			{
				if (GET_ITEM(lpInfo.TaskProgress.QMainType, lpInfo.TaskProgress.QSubType) == Taget)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void NewQuestWorld::GCKillMonster(int aIndex, LPOBJ lpMonster)
{
	//LogAdd(LOG_DEBUG, "%s %d", __FUNCTION__, __LINE__);

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_NEW_QUEST_WORLD_SEND pMsg;

	NEW_QUEST_INFO lpInfo;

	pMsg.header.set(0xF6, 0x50, sizeof(pMsg));

	if (!lpObj->NewQuestWorldList)
	{
		return;
	}

	if (this->GetInfo(lpObj->NewQuestWorldList->QuestIndex + 1, &lpInfo) == 0 && lpObj->NewQuestWorldList->QuestState >= NEW_QUEST_WORLD_FINISH)
	{
		return;
	}

	pMsg.Quest.QuestIndex = lpObj->NewQuestWorldList->QuestIndex;

	if (this->GetInfo(lpObj->NewQuestWorldList->QuestIndex, &lpInfo))
	{
		switch (lpInfo.TaskProgress.QuestType)
		{
		case COLLECT:
			//if ((lpObj->NewQuestWorldList->TagetNumber = this->GetNewQuestItem(lpObj, GET_ITEM(lpInfo.TaskProgress.QMainType, lpInfo.TaskProgress.QSubType))) >= lpInfo.TaskProgress.TagetNumber)
			if (lpObj->NewQuestWorldList->TagetNumber >= lpInfo.TaskProgress.TagetNumber)
			{
				lpObj->NewQuestWorldList->TagetNumber = lpInfo.TaskProgress.TagetNumber;
				lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_SUCCESS;
			}
			break;
		case KILL:
			if (gNewQuestWorld.GetTagetNumber(lpMonster->Class, lpObj->Index, KILL))
			{
				lpObj->NewQuestWorldList->TagetNumber += 1;
			}

			if (lpObj->NewQuestWorldList->TagetNumber >= lpInfo.TaskProgress.TagetNumber)
			{
				lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_SUCCESS;
			}
			break;
		case LEVEL:
			if ((lpObj->Level + lpObj->MasterLevel) >= lpInfo.TaskProgress.QMainType)
			{
				lpObj->NewQuestWorldList->TagetNumber = (lpObj->Level + lpObj->MasterLevel);
				lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_SUCCESS;
			}
			break;
		case TRANSFER:
			if (lpInfo.TaskProgress.TemplateType == 1 && lpInfo.TaskProgress.QMainType == 1 && lpObj->ChangeUp >= 1) //119, 1st
			{
				lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_SUCCESS;
			}
			else if (lpInfo.TaskProgress.TemplateType == 1 && lpInfo.TaskProgress.QMainType == 2 && gQuest.CheckQuestListState(lpObj, 2, QUEST_FINISH)) //175, Marlon
			{
				lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_SUCCESS;
			}
			else if (lpInfo.TaskProgress.TemplateType == 1 && lpInfo.TaskProgress.QMainType == 6 && lpObj->ChangeUp >= 2) //217, 3rd
			{
				lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_SUCCESS;
			}
			break;
		case SEEK:
			lpObj->NewQuestWorldList->QuestState = NEW_QUEST_WORLD_SUCCESS;
			break;
		default:
			break;
		}
	}

	pMsg.Quest.TagetNumber = lpObj->NewQuestWorldList->TagetNumber;
	pMsg.Quest.QuestState = lpObj->NewQuestWorldList->QuestState;
	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}