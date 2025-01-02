#include "stdafx.h"
#include "MiningSystem.h"
#include "DSProtocol.h"
#include "ItemManager.h"
#include "JewelMix.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Util.h"
#include "ItemManager.h"
#include "RandomManager.h"

CMiningSystem gMiningSystem;

CMiningSystem::CMiningSystem() // OK
{
	this->m_MiningInfo.clear();
}

CMiningSystem::~CMiningSystem() // OK
{

}

void CMiningSystem::Load(char* path) // OK
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

	this->m_MiningInfo.clear();

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

			MINING_INFO info;

			info.Index = lpMemScript->GetNumber();
			info.Stage = lpMemScript->GetAsNumber();
			info.Value = lpMemScript->GetAsNumber();
			info.ItemRate = lpMemScript->GetAsNumber();
			info.ItemCount = lpMemScript->GetAsNumber();
			info.BonusItemRate = lpMemScript->GetAsNumber();
			info.BonusItemIndex = lpMemScript->GetAsNumber();
			info.BonusItemCount = lpMemScript->GetAsNumber();
			this->m_MiningInfo.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CMiningSystem::GetMiningInfo(int index, int stage, MINING_INFO* lpInfo) // OK
{
	for (std::vector<MINING_INFO>::iterator it = this->m_MiningInfo.begin(); it != this->m_MiningInfo.end(); it++)
	{
		if (it->Index == index)
		{
			if (it->Stage == stage)
			{
				(*lpInfo) = (*it);
				return 1;
			}
		}
	}

	return 0;
}

void CMiningSystem::CreateMiningReward(int aIndex, int ItemCount) // OK
{
	int ItemIndex = GET_ITEM(12, 15);

	CRandomManager RandomManager;
	char JewelName[21][32] = {
		"Chaos",
		"Bless",
		"Soul",
		"Life",
		"Harmony",
		"Guardian",
		"Creation",
		"Sphere Upgrade",
		"Socket",
		"Skill",
		"Ancient",
		"Level",
		"Elemental Slot",
		"Errtel Rank",
		"Overlife",
		"Excellence",
		"Luck",
		"Errtel Level",
		"4th Wing Element",
		"4th Wing Option",
		"Galaxies Cock"
	};

	int JewelIndex[21] = {
		GET_ITEM(12, 15),
		GET_ITEM(14, 13),
		GET_ITEM(14, 14),
		GET_ITEM(14, 16),
		GET_ITEM(14, 42),
		GET_ITEM(14, 31),
		GET_ITEM(14, 22),
		GET_ITEM(14, 498),
		GET_ITEM(14, 499),
		GET_ITEM(14, 500),
		GET_ITEM(14, 501),
		GET_ITEM(14, 502),
		GET_ITEM(14, 503),
		GET_ITEM(14, 504),
		GET_ITEM(14, 505),
		GET_ITEM(14, 506),
		GET_ITEM(14, 507),
		GET_ITEM(14, 508),
		GET_ITEM(14, 509),
		GET_ITEM(14, 510),
		GET_ITEM(14, 511)
	};

	int JewelCount[21] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	RandomManager.AddElement(0, 6);
	RandomManager.AddElement(1, 6);
	RandomManager.AddElement(2, 6);
	RandomManager.AddElement(3, 6);
	RandomManager.AddElement(4, 6);
	RandomManager.AddElement(5, 6);
	RandomManager.AddElement(6, 6);
	RandomManager.AddElement(7, 6);
	RandomManager.AddElement(8, 6);
	RandomManager.AddElement(9, 6);
	RandomManager.AddElement(10, 5);
	RandomManager.AddElement(11, 5);
	RandomManager.AddElement(12, 5);
	RandomManager.AddElement(13, 5);
	RandomManager.AddElement(14, 5);
	RandomManager.AddElement(15, 5);
	RandomManager.AddElement(16, 5);
	RandomManager.AddElement(17, 5);
	RandomManager.AddElement(18, 5);
	RandomManager.AddElement(19, 5);
	RandomManager.AddElement(20, 1);

	for (int n = 0; n < ItemCount; n++)
	{
		RandomManager.GetRandomElement(&ItemIndex);
		JewelCount[ItemIndex]++;
		GDCreateItemSend(aIndex, 0xEB, 0, 0, JewelIndex[ItemIndex], 0, 1, 0, 0, 0, aIndex, 0, 0, 0, 0, 0, 0xFF, 0);
	}

	for (int i = 0; i < 21; i++)
	{
		if (JewelCount[i] > 0)
		{
			gNotice.GCNoticeSend(aIndex,1,0,0,0,0,0,"Jewel of %s: %d", JewelName[i], JewelCount[i]);
		}
	}
}

void CMiningSystem::CGMiningStartRecv(PMSG_MINING_START_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpMsg->index != aIndex)
	{
		return;
	}

	if (gObjIsConnected(lpMsg->MonsterIndex) == 0)
	{
		return;
	}

	MINING_INFO MiningInfo;

	if (this->GetMiningInfo(lpMsg->MonsterClass, (lpMsg->stage + 1), &MiningInfo) == 0)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 3);
		return;
	}

	if (gObj[lpMsg->MonsterIndex].Live == 0)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 3);
		return;
	}

	if (gObj[lpMsg->MonsterIndex].Map != lpObj->Map || gObj[lpMsg->MonsterIndex].Map != lpMsg->map)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 3);
		return;
	}

	if (OBJECT_RANGE(gObj[lpMsg->MonsterIndex].MiningIndex) != 0 && gObj[lpMsg->MonsterIndex].MiningIndex != aIndex)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 8);
		return;
	}

	if (lpObj->Change >= 0)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 10);
		return;
	}

	if (gItemManager.GetInventoryEmptySlotCount(lpObj) < MAX_MINING_JEWEL_PACK)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 11);
		return;
	}

	if (lpMsg->stage == 0 && (lpObj->Interface.use != 0))
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 12);
		return;
	}

	if (lpMsg->stage != 0 && (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_MINING))
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 12);
		return;
	}

	if (lpObj->PShopOpen != 0)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 13);
		return;
	}

	if (lpObj->MiningStage != lpMsg->stage)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 17);
		return;
	}

	if (OBJECT_RANGE(lpObj->MiningIndex) != 0 && lpObj->MiningIndex != lpMsg->MonsterIndex)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 17);
		return;
	}

	if (lpObj->Inventory[0].IsItem() == 0 || lpObj->Inventory[0].m_Index != GET_ITEM(0, 41) || lpObj->Inventory[0].m_Durability < MiningInfo.Value)
	{
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 18);
		gItemManager.InventoryDelItem(aIndex, 0);
		gItemManager.GCItemDeleteSend(aIndex, 0, 1);
		return;
	}

	lpObj->Inventory[0].m_Durability = (lpObj->Inventory[0].m_Durability - MiningInfo.Value);
	gItemManager.GCItemDurSend(aIndex, 0, (BYTE)lpObj->Inventory[0].m_Durability, 0);

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_MINING;
	lpObj->Interface.state = 0;

	gObj[lpMsg->MonsterIndex].MiningStage = 1;
	gObj[lpMsg->MonsterIndex].MiningIndex = aIndex;

	lpObj->MiningIndex = lpMsg->MonsterIndex;

	if ((GetLargeRand() % 10000) < MiningInfo.ItemRate)
	{
		lpObj->MiningStage++;
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpObj->MiningStage, MiningInfo.ItemCount, ((lpObj->MiningStage == MAX_MINING_STAGE) ? 1 : 0));
	}
	else
	{
		lpObj->MiningStage = lpObj->MiningStage;
		this->GCMiningStartSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0, 2);
	}
}

void CMiningSystem::CGMiningSuccessRecv(PMSG_MINING_SUCCESS_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpMsg->index != aIndex)
	{
		return;
	}

	if (gObjIsConnected(lpMsg->MonsterIndex) == 0)
	{
		return;
	}

	if (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_MINING)
	{
		return;
	}

	if (OBJECT_RANGE(lpObj->MiningIndex) == 0 || lpObj->MiningIndex != lpMsg->MonsterIndex)
	{
		return;
	}

	if (OBJECT_RANGE(gObj[lpMsg->MonsterIndex].MiningIndex) == 0 || gObj[lpMsg->MonsterIndex].MiningIndex != aIndex)
	{
		return;
	}

	MINING_INFO MiningInfo;

	if (this->GetMiningInfo(lpMsg->MonsterClass, lpObj->MiningStage, &MiningInfo) == 0)
	{
		this->GCMiningSuccessSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0);
		return;
	}

	if (lpObj->MiningStage == MAX_MINING_STAGE && lpObj->MiningStage != (lpMsg->stage + 1))
	{
		this->GCMiningSuccessSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0);
		return;
	}

	if (lpObj->MiningStage != MAX_MINING_STAGE && lpObj->MiningStage != (lpMsg->stage + 0))
	{
		this->GCMiningSuccessSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 0);
		return;
	}

	for (int n = 0; n < MAX_JEWEL_MIX_BUNDLE; n++)
	{
		if (lpObj->MiningStage == MAX_MINING_STAGE)
		{
			GCFireworksSend(lpObj, lpObj->X, lpObj->Y);
		}
		this->CreateMiningReward(aIndex,MiningInfo.ItemCount);
		break;
	}

	if (MiningInfo.BonusItemRate >= 0 && (GetLargeRand() % 10000) < MiningInfo.BonusItemRate)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Mysterious Stone: %d", MiningInfo.BonusItemCount);
		gNotice.SendMapServerGroupMsg(1, "%s acabou de minerar %d Mysterious Stone(s)", lpObj->Name, MiningInfo.BonusItemCount);
		GDCreateItemSend(aIndex, 0xEB, 0, 0, MiningInfo.BonusItemIndex, 0, MiningInfo.BonusItemCount, 0, 0, 0, aIndex, 0, 0, 0, 0, 0, 0xFF, 0);
	}

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
	lpObj->MiningStage = 0;
	lpObj->MiningIndex = -1;

	gObj[lpMsg->MonsterIndex].Life = 0;
	gObj[lpMsg->MonsterIndex].MiningStage = 0;
	gObj[lpMsg->MonsterIndex].MiningIndex = -1;

	gObjectManager.CharacterLifeCheck(lpObj, &gObj[lpMsg->MonsterIndex], 0, 0, 0, 0, 0, 0);

	this->GCMiningSuccessSend(aIndex, lpMsg->MonsterIndex, lpMsg->MonsterClass, lpMsg->map, lpMsg->stage, 3);
}

void CMiningSystem::CGMiningFailureRecv(PMSG_MINING_FAILURE_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpMsg->index != aIndex)
	{
		return;
	}

	if (gObjIsConnected(lpMsg->MonsterIndex) == 0)
	{
		return;
	}

	if (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_MINING)
	{
		return;
	}

	if (OBJECT_RANGE(lpObj->MiningIndex) == 0 || lpObj->MiningIndex != lpMsg->MonsterIndex)
	{
		return;
	}

	if (OBJECT_RANGE(gObj[lpMsg->MonsterIndex].MiningIndex) == 0 || gObj[lpMsg->MonsterIndex].MiningIndex != aIndex)
	{
		return;
	}

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
	lpObj->MiningStage = 0;
	lpObj->MiningIndex = -1;

	gObj[lpMsg->MonsterIndex].Life = 0;
	gObj[lpMsg->MonsterIndex].MiningStage = 0;
	gObj[lpMsg->MonsterIndex].MiningIndex = -1;

	gObjectManager.CharacterLifeCheck(lpObj, &gObj[lpMsg->MonsterIndex], 0, 0, 0, 0, 0, 0);
}

void CMiningSystem::GCMiningStartSend(int aIndex, int MonsterIndex, int MonsterClass, int map, int stage, int value, int result) // OK
{
	PMSG_MINING_START_SEND pMsg;

	pMsg.header.set(0x4C, 0x00, sizeof(pMsg));

	pMsg.index = aIndex;
	pMsg.MonsterIndex = MonsterIndex;
	pMsg.MonsterClass = MonsterClass;
	pMsg.map = map;
	pMsg.stage = stage;
	pMsg.value = value;
	pMsg.result = result;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CMiningSystem::GCMiningSuccessSend(int aIndex, int MonsterIndex, int MonsterClass, int map, int stage, int result) // OK
{
	PMSG_MINING_SUCCESS_SEND pMsg;

	pMsg.header.set(0x4C, 0x01, sizeof(pMsg));

	pMsg.index = aIndex;
	pMsg.MonsterIndex = MonsterIndex;
	pMsg.MonsterClass = MonsterClass;
	pMsg.map = map;
	pMsg.stage = stage;
	pMsg.result = result;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}
