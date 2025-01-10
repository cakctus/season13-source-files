#include "stdafx.h"
#include "ChaosBox.h"
#include "CastleSiegeSync.h"
#include "DSProtocol.h"
#include "ItemBagManager.h"
#include "ItemOptionRate.h"
#include "JewelOfHarmonyOption.h"
#include "JewelOfHarmonyType.h"
#include "Log.h"
#include "LuckyItem.h"
#include "Notice.h"
#include "PentagramSystem.h"
#include "RandomManager.h"
#include "ServerInfo.h"
#include "SetItemType.h"
#include "Shop.h"
#include "SocketItemOption.h"
#include "SocketItemType.h"
#include "RheaGold.h"
#include "Util.h"

CChaosBox gChaosBox;

CChaosBox::CChaosBox() // OK
{
	this->m_SeniorMixLimitDay = 0;
	this->m_SeniorMixLimitMonth = 0;
	this->m_SeniorMixLimitYear = 0;
}

CChaosBox::~CChaosBox() // OK
{
}

void CChaosBox::ChaosBoxInit(LPOBJ lpObj) // OK
{
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		lpObj->ChaosBox[n].Clear();
		lpObj->ChaosBoxMap[n] = 0xFF;
	}
}

void CChaosBox::ChaosBoxItemDown(LPOBJ lpObj, int slot) // OK
{
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (n != slot)
		{
			lpObj->ChaosBox[n].Clear();
			lpObj->ChaosBoxMap[n] = 0xFF;
			continue;
		}

		CItem* lpItem = &lpObj->ChaosBox[n];

		lpItem->m_Level = 0;

		float dur = (float)gItemManager.GetItemDurability(lpItem->m_Index, lpItem->m_Level, lpItem->IsExcItem(), lpItem->IsSetItem());

		lpItem->m_Durability = dur * (lpItem->m_Durability / lpItem->m_BaseDurability);

		lpItem->Convert(lpItem->m_Index, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, lpItem->m_NewOption, lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_SocketOptionBonus);
	}
}

void CChaosBox::ChaosBoxItemKeep(LPOBJ lpObj, int slot) // OK
{
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (n != slot)
		{
			lpObj->ChaosBox[n].Clear();
			lpObj->ChaosBoxMap[n] = 0xFF;
			continue;
		}
	}
}

void CChaosBox::ChaosBoxItemSave(LPOBJ lpObj) // OK
{
	if (gObjInventoryCommit(lpObj->Index) != 0)
	{
		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			if (lpObj->ChaosBox[n].IsItem() != 0)
			{
				gItemManager.InventoryInsertItem(lpObj->Index, lpObj->ChaosBox[n]);
			}
		}
	}
}

bool CChaosBox::GetTalismanOfLuckRate(LPOBJ lpObj, int* rate) // OK
{
	int count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 53)) // Talisman of Skill
		{
			count += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	(*rate) += count;

	return ((count > MAX_TALISMAN_OF_LUCK) ? 0 : 1);
}

bool CChaosBox::GetElementalTalismanOfLuckRate(LPOBJ lpObj, int* rate) // OK
{
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 189)) // Elemental Talisman of Skill
		{
			(*rate) += 10;
			return 1;
		}
	}	

	return 0;
}

void CChaosBox::ChaosItemMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			ItemCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if (ChaosCount == 0 || ItemCount == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_ChaosItemMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 20000);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_ChaosItemMixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 10000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemLevel = 0;
		BYTE ItemOption1 = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;

		CRandomManager RandomManager;

		RandomManager.AddElement(GET_ITEM(2, 6), 1);
		RandomManager.AddElement(GET_ITEM(4, 6), 1);
		RandomManager.AddElement(GET_ITEM(5, 7), 1);

		RandomManager.GetRandomElement(&ItemIndex);

		gItemOptionRate.GetItemOption0(3, &ItemLevel);
		gItemOptionRate.GetItemOption1(3, &ItemOption1);
		gItemOptionRate.GetItemOption2(3, &ItemOption2);
		gItemOptionRate.GetItemOption3(3, &ItemOption3);

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, ItemLevel, 0, ItemOption1, ItemOption2, ItemOption3, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[ChaosItemMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[ChaosItemMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::DevilSquareMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int EyeCount = 0;
	int EyeLevel = 0;
	int KeyCount = 0;
	int KeyLevel = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 17))
		{
			EyeCount++;
			EyeLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 18))
		{
			KeyCount++;
			KeyLevel = lpObj->ChaosBox[n].m_Level;
		}
	}

	if (ChaosCount != 1 || EyeCount != 1 || KeyCount != 1 || EyeLevel != KeyLevel)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (EyeLevel > 7 || KeyLevel > 7)
	{
		this->GCChaosMixSend(lpObj->Index, 8, 0);
		return;
	}

	switch (EyeLevel)
	{
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate1[lpObj->AccountLevel];
		lpObj->ChaosMoney = 100000;
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate2[lpObj->AccountLevel];
		lpObj->ChaosMoney = 200000;
		break;
	case 3:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate3[lpObj->AccountLevel];
		lpObj->ChaosMoney = 400000;
		break;
	case 4:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate4[lpObj->AccountLevel];
		lpObj->ChaosMoney = 700000;
		break;
	case 5:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate5[lpObj->AccountLevel];
		lpObj->ChaosMoney = 1100000;
		break;
	case 6:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate6[lpObj->AccountLevel];
		lpObj->ChaosMoney = 1600000;
		break;
	case 7:
		lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate7[lpObj->AccountLevel];
		lpObj->ChaosMoney = 2000000;
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 19), EyeLevel, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[DevilSquareMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[DevilSquareMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::DevilSquareMixMP(LPOBJ lpObj, int count) // OK
{
	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));
	int ChaosCount = 0;
	int EyeCount = 0;
	int EyeLevel[10] = { 0 };
	int KeyCount = 0;
	int KeyLevel[10] = { 0 };
	int TaxMoney = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 17))
		{
			EyeCount++;
			EyeLevel[EyeCount - 1] = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 18))
		{
			KeyCount++;
			KeyLevel[KeyCount - 1] = lpObj->ChaosBox[n].m_Level;
		}
	}

	if (ChaosCount > 8 || EyeCount > 8 || KeyCount > 8 || ChaosCount != EyeCount || ChaosCount != KeyCount || KeyCount != EyeCount)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	for (int i = 0; i < count; i++)
	{
		if (EyeLevel[i] != KeyLevel[i])
		{
			this->GCChaosMixSend(lpObj->Index, 7, 0);
			return;
		}
		else if (EyeLevel[i] > 7 || KeyLevel[i] > 7)
		{
			this->GCChaosMixSend(lpObj->Index, 8, 0);
			return;
		}
	}

	lpObj->ChaosMoney = 0;

	for (int num = 0; num < count; num++)
	{
		switch (EyeLevel[num])
		{
		case 1:
			lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate1[lpObj->AccountLevel];
			lpObj->ChaosMoney += 100000;
			break;
		case 2:
			lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate2[lpObj->AccountLevel];
			lpObj->ChaosMoney += 200000;
			break;
		case 3:
			lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate3[lpObj->AccountLevel];
			lpObj->ChaosMoney += 400000;
			break;
		case 4:
			lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate4[lpObj->AccountLevel];
			lpObj->ChaosMoney += 700000;
			break;
		case 5:
			lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate5[lpObj->AccountLevel];
			lpObj->ChaosMoney += 1100000;
			break;
		case 6:
			lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate6[lpObj->AccountLevel];
			lpObj->ChaosMoney += 1600000;
			break;
		case 7:
			lpObj->ChaosSuccessRate = gServerInfo.m_DevilSquareMixRate7[lpObj->AccountLevel];
			lpObj->ChaosMoney += 2000000;
			break;
		}

		TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

		lpObj->ChaosMoney += TaxMoney;

		if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
		{
			this->GCChaosMixSend(lpObj->Index, 2, 0);
			return;
		}
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;

	for (int num1 = 0; num1 < count; num1++)
	{
		if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
		{
			this->GCChaosMixSend(lpObj->Index, 240, 0);
			return;
		}

		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

		if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (num1 % 8);
			pMsg.ItemInfo[num1 / 8] |= btMixNumber;
			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 19), EyeLevel[num1], 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
			gLog.Output(LOG_CHAOS_MIX, "[DevilSquareMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		else
		{
			this->ChaosBoxInit(lpObj);
			this->GCChaosBoxSend(lpObj, 0);
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			gLog.Output(LOG_CHAOS_MIX, "[DevilSquareMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void CChaosBox::PlusItemLevelMix(LPOBJ lpObj, int type) // OK
{
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int ItemCount = 0;
	int ItemSlot = 0;
	int ChaosAmulet = 0;
	int ElementalChaosAmulet = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 96))
		{
			ChaosAmulet++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 190))
		{
			ElementalChaosAmulet++;
		}
		else if (lpObj->ChaosBox[n].m_Level == (9 + type))
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ChaosCount != 1 || SoulCount < (type + 1) || BlessCount < (type + 1) || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	switch (type)
	{
	case 0:
		lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate1[lpObj->AccountLevel];
		break;
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate2[lpObj->AccountLevel];
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate3[lpObj->AccountLevel];
		break;
	case 3:
		lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate4[lpObj->AccountLevel];
		break;
	case 4:
		lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate5[lpObj->AccountLevel];
		break;
	case 5:
		lpObj->ChaosSuccessRate = gServerInfo.m_PlusItemLevelMixRate6[lpObj->AccountLevel];
		break;
	}

	if (lpObj->ChaosBox[ItemSlot].m_Option2 != 0)
	{
		lpObj->ChaosSuccessRate += gServerInfo.m_AddLuckSuccessRate2[lpObj->AccountLevel];
	}

	if (lpObj->ChaosBox[ItemSlot].IsPentagramItem() == 0 && this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (lpObj->ChaosBox[ItemSlot].IsPentagramItem() != 0)
	{
		//this->GCChaosMixSend(lpObj->Index, 240, 0);
		this->GetElementalTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 2000000 * (type + 1);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->ChaosBox[ItemSlot];
		CItem itemOld = lpObj->ChaosBox[ItemSlot];

		item.m_Level++;

		item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());

		item.m_Durability = (item.m_Durability * lpObj->ChaosBox[ItemSlot].m_Durability) / item.m_BaseDurability;

		this->ChaosBoxInit(lpObj);

		gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);

		this->GCChaosMixSend(lpObj->Index, 1, &item);

		gObjCustomLogPlusChaosMix(lpObj, type, item.m_Index);

		gLog.Output(LOG_CHAOS_MIX, "[PlusItemLevelMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

		GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 0, 0);
	}
	else
	{
		if (ChaosAmulet == 0 && ElementalChaosAmulet == 0)
		{
			gPentagramSystem.DelAllPentagramJewelInfo(lpObj, &lpObj->ChaosBox[ItemSlot], 0);

			this->ChaosBoxInit(lpObj);
			this->GCChaosBoxSend(lpObj, 0);
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			gLog.Output(LOG_CHAOS_MIX, "[PlusItemLevelMix][Failure][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d, ChaosAmulet: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney, (ChaosAmulet + ElementalChaosAmulet));
		}
		else
		{
			this->ChaosBoxItemDown(lpObj, ItemSlot);
			this->GCChaosBoxSend(lpObj, 0);
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			gLog.Output(LOG_CHAOS_MIX, "[PlusItemLevelMix][Failure][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d, ChaosAmulet: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney, (ChaosAmulet + ElementalChaosAmulet));
		}
	}
}

void CChaosBox::DinorantMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int UniriaCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 2) && lpObj->ChaosBox[n].m_Durability == 255)
		{
			UniriaCount++;
		}
	}

	if (ChaosCount != 1 || UniriaCount != 10)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_DinorantMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 500000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = GET_ITEM(13, 3);
		BYTE ItemNewOption = 0;

		gItemOptionRate.GetItemOption4(3, &ItemNewOption);

		gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 255, 0, 0, 0, -1, ItemNewOption, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[DinorantMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[DinorantMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::DinorantMixMP(LPOBJ lpObj, int count) // OK
{
	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));
	int ChaosCount = 0;
	int UniriaCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 2) && lpObj->ChaosBox[n].m_Durability == 255)
		{
			UniriaCount++;
		}
	}

	if (ChaosCount != (1 * count) || UniriaCount != (10 * count))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_DinorantMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 500000 * count;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;
	for (int i = 0; i < count; i++)
	{
		if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
		{
			WORD ItemIndex = GET_ITEM(13, 3);
			BYTE ItemNewOption = 0;

			gItemOptionRate.GetItemOption4(3, &ItemNewOption);

			gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

			BYTE btMixNumber = 1 << (i % 8);

			pMsg.ItemInfo[i / 8] |= btMixNumber;

			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 255, 0, 0, 0, -1, ItemNewOption, 0, 0, 0, 0, 0xFF, 0);

			gLog.Output(LOG_CHAOS_MIX, "[DinorantMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		else
		{
			this->ChaosBoxInit(lpObj);

			this->GCChaosBoxSend(lpObj, 0);

			this->GCChaosMixSend(lpObj->Index, 0, 0);

			gLog.Output(LOG_CHAOS_MIX, "[DinorantMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void CChaosBox::FruitMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_FruitMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 3000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		BYTE ItemLevel = GetLargeRand() % 5;

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 15), ItemLevel, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[FruitMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[FruitMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::FruitMixMP(LPOBJ lpObj, int count) // OK
{
	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));
	int ChaosCount = 0;
	int CreationCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
	}

	if (ChaosCount != (1 * count) || CreationCount != (1 * count))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_FruitMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 3000000 * count;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;

	for (int i = 0; i < count; i++)
	{
		if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);

			pMsg.ItemInfo[i / 8] |= btMixNumber;

			BYTE ItemLevel = GetLargeRand() % 5;

			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 15), ItemLevel, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

			gLog.Output(LOG_CHAOS_MIX, "[FruitMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		else
		{
			this->ChaosBoxInit(lpObj);

			this->GCChaosBoxSend(lpObj, 0);

			this->GCChaosMixSend(lpObj->Index, 0, 0);

			gLog.Output(LOG_CHAOS_MIX, "[FruitMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void CChaosBox::Wing2Mix(LPOBJ lpObj, int type) // OK
{
	int ChaosCount = 0;
	int FeatherCount = 0;
	int SleeveCount = 0;
	int WingItemCount = 0;
	int WingItemMoney = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int TalismanOfWingType = 0;
	int MagicStoneItem = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 14) && lpObj->ChaosBox[n].m_Level == 0)
		{
			FeatherCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 14) && lpObj->ChaosBox[n].m_Level == 1)
		{
			SleeveCount++;
		}
		else if ((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 0) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 2)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 41))
		{
			WingItemCount++;
			WingItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 88) && lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 92))
		{
			TalismanOfWingType = lpObj->ChaosBox[n].m_Index - GET_ITEM(13, 87);
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 405))
		{
			MagicStoneItem += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (ChaosCount != 1 || (type == 0 && FeatherCount != 1) || (type == 1 && SleeveCount != 1) || WingItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_Wing2MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (WingItemMoney / 4000000) + (ItemMoney / 40000);
		lpObj->ChaosSuccessRate += (MagicStoneItem * 20);
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 90) ? 90 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Wing2MixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_Wing2MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 90) ? 90 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 5000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		if (type == 0)
		{
			WORD ItemIndex = 0;
			BYTE ItemOption2 = 0;
			BYTE ItemOption3 = 0;
			BYTE ItemNewOption = 0;

			CRandomManager RandomManager;

			RandomManager.AddElement(GET_ITEM(12, 3), 1);

			RandomManager.AddElement(GET_ITEM(12, 4), 1);

			RandomManager.AddElement(GET_ITEM(12, 5), 1);

			RandomManager.AddElement(GET_ITEM(12, 6), 1);

			RandomManager.AddElement(GET_ITEM(12, 42), 1);

			RandomManager.GetRandomElement(&ItemIndex);

			switch (TalismanOfWingType)
			{
			case 1:
				ItemIndex = GET_ITEM(12, 5);
				break;
			case 2:
				ItemIndex = GET_ITEM(12, 4);
				break;
			case 3:
				ItemIndex = GET_ITEM(12, 3);
				break;
			case 4:
				ItemIndex = GET_ITEM(12, 42);
				break;
			case 5:
				ItemIndex = GET_ITEM(12, 6);
				break;
			}

			gItemOptionRate.GetItemOption2(4, &ItemOption2);

			gItemOptionRate.GetItemOption3(4, &ItemOption3);

			gItemOptionRate.GetItemOption4(4, &ItemNewOption);

			gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + (32 * (GetLargeRand() % 2))), 0, 0, 0, 0, 0xFF, 0);

			gLog.Output(LOG_CHAOS_MIX, "[Wing2Mix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		else
		{
			WORD ItemIndex = 0;
			BYTE ItemOption2 = 0;
			BYTE ItemOption3 = 0;
			BYTE ItemNewOption = 0;

			CRandomManager RandomManager;

			RandomManager.AddElement(GET_ITEM(12, 269), 1);
			RandomManager.AddElement(GET_ITEM(12, 49), 1);
			RandomManager.AddElement(GET_ITEM(13, 30), 1);

			RandomManager.GetRandomElement(&ItemIndex);

			gItemOptionRate.GetItemOption2(4, &ItemOption2);
			gItemOptionRate.GetItemOption3(4, &ItemOption3);
			gItemOptionRate.GetItemOption4(5, &ItemNewOption);

			gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + 32), 0, 0, 0, 0, 0xFF, 0);

			gLog.Output(LOG_CHAOS_MIX, "[Wing2Mix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[Wing2Mix][Failure][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::BloodCastleMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel = 0;
	int BoneCount = 0;
	int BoneLevel = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 16))
		{
			ScrollCount++;
			ScrollLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 17))
		{
			BoneCount++;
			BoneLevel = lpObj->ChaosBox[n].m_Level;
		}
	}

	if (ChaosCount != 1 || ScrollCount != 1 || BoneCount != 1 || ScrollLevel != BoneLevel)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (ScrollLevel > 8 || BoneLevel > 8)
	{
		this->GCChaosMixSend(lpObj->Index, 8, 0);
		return;
	}

	switch (ScrollLevel)
	{
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate1[lpObj->AccountLevel];
		lpObj->ChaosMoney = 50000;
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate2[lpObj->AccountLevel];
		lpObj->ChaosMoney = 80000;
		break;
	case 3:
		lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate3[lpObj->AccountLevel];
		lpObj->ChaosMoney = 150000;
		break;
	case 4:
		lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate4[lpObj->AccountLevel];
		lpObj->ChaosMoney = 250000;
		break;
	case 5:
		lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate5[lpObj->AccountLevel];
		lpObj->ChaosMoney = 400000;
		break;
	case 6:
		lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate6[lpObj->AccountLevel];
		lpObj->ChaosMoney = 600000;
		break;
	case 7:
		lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate7[lpObj->AccountLevel];
		lpObj->ChaosMoney = 850000;
		break;
	case 8:
		lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate8[lpObj->AccountLevel];
		lpObj->ChaosMoney = 1050000;
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) <= lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 18), ScrollLevel, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[BloodCastleMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[BloodCastleMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::BloodCastleMixMP(LPOBJ lpObj, int count) // OK
{
	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel[10] = { 0 };
	int BoneCount = 0;
	int BoneLevel[10] = { 0 };

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 16))
		{
			ScrollCount++;
			ScrollLevel[ScrollCount - 1] = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 17))
		{
			BoneCount++;
			BoneLevel[BoneCount - 1] = lpObj->ChaosBox[n].m_Level;
		}
	}
	for (int level = 0; level < count; level++)
	{
		if (ChaosCount != (1 * count) || ScrollCount != (1 * count) || BoneCount != (1 * count) || ScrollLevel[level] != BoneLevel[level])
		{
			this->GCChaosMixSend(lpObj->Index, 7, 0);
			return;
		}

		if (ScrollLevel[level] > 8 || BoneLevel[level] > 8)
		{
			this->GCChaosMixSend(lpObj->Index, 8, 0);
			return;
		}

		switch (ScrollLevel[level])
		{
		case 1:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate1[lpObj->AccountLevel];
			lpObj->ChaosMoney = 50000;
			break;
		case 2:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate2[lpObj->AccountLevel];
			lpObj->ChaosMoney = 80000;
			break;
		case 3:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate3[lpObj->AccountLevel];
			lpObj->ChaosMoney = 150000;
			break;
		case 4:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate4[lpObj->AccountLevel];
			lpObj->ChaosMoney = 250000;
			break;
		case 5:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate5[lpObj->AccountLevel];
			lpObj->ChaosMoney = 400000;
			break;
		case 6:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate6[lpObj->AccountLevel];
			lpObj->ChaosMoney = 600000;
			break;
		case 7:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate7[lpObj->AccountLevel];
			lpObj->ChaosMoney = 850000;
			break;
		case 8:
			lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate8[lpObj->AccountLevel];
			lpObj->ChaosMoney = 1050000;
			break;
		}
	}
	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;
	lpObj->ChaosMoney *= count;
	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);


	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));
	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;

	for (int i = 0; i < count; i++)
	{
		if ((GetLargeRand() % 100) <= lpObj->ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);

			pMsg.ItemInfo[i / 8] |= btMixNumber;
			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 18), ScrollLevel[i], 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

			gLog.Output(LOG_CHAOS_MIX, "[BloodCastleMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		else
		{
			this->ChaosBoxInit(lpObj);

			this->GCChaosBoxSend(lpObj, 0);

			this->GCChaosMixSend(lpObj->Index, 0, 0);

			gLog.Output(LOG_CHAOS_MIX, "[BloodCastleMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void CChaosBox::Wing1Mix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ChaosItem = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int TalismanOfWingType = 0;
	int MagicStoneItem = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			ItemCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if ((lpObj->ChaosBox[n].m_Index == GET_ITEM(2, 6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 7)) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ChaosItem++;
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 83) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(13, 86))
		{
			TalismanOfWingType = lpObj->ChaosBox[n].m_Index - GET_ITEM(13, 82);
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 404))
		{
			MagicStoneItem += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (ChaosCount == 0 || ChaosItem == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_Wing1MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 20000);
		lpObj->ChaosSuccessRate += (MagicStoneItem * 5);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Wing1MixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 10000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;

		CRandomManager RandomManager;

		RandomManager.AddElement(GET_ITEM(12, 0), 1);
		RandomManager.AddElement(GET_ITEM(12, 1), 1);
		RandomManager.AddElement(GET_ITEM(12, 2), 1);
		RandomManager.AddElement(GET_ITEM(12, 41), 1);

		RandomManager.GetRandomElement(&ItemIndex);

		switch (TalismanOfWingType)
		{
		case 1:
			ItemIndex = GET_ITEM(12, 2);
			break;
		case 2:
			ItemIndex = GET_ITEM(12, 1);
			break;
		case 3:
			ItemIndex = GET_ITEM(12, 0);
			break;
		case 4:
			ItemIndex = GET_ITEM(12, 41);
			break;
		}

		gItemOptionRate.GetItemOption2(5, &ItemOption2);
		gItemOptionRate.GetItemOption3(5, &ItemOption3);

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[Wing1Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[Wing1Mix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::PetMix(LPOBJ lpObj, int type) // OK
{
	int ChaosCount = 0;
	int SoulOfDarkHorseCount = 0;
	int SoulOfDarkSpiritCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int CreationCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulOfDarkHorseCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 31) && lpObj->ChaosBox[n].m_Level == 1)
		{
			SoulOfDarkSpiritCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
	}

	if (ChaosCount != 1 || (type == 0 && SoulOfDarkHorseCount != 1) || (type == 1 && SoulOfDarkSpiritCount != 1) || BlessCount != (5 - (type * 3)) || SoulCount != (5 - (type * 3)) || CreationCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_PetMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 5000000 - (4000000 * type);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFE, 0, 0, (GET_ITEM(13, 4) + type), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[PetMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 1);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[PetMix][Failure][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::SiegePotionMix(LPOBJ lpObj, int type) // OK
{
	int BlessCount = 0;
	int SoulCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
	}

	if ((type == 0 && (BlessCount == 0 || BlessCount > 25)) || (type == 1 && (SoulCount == 0 || SoulCount > 25)))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 100000 - (50000 * type);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 7), type, ((BlessCount + SoulCount) * 10), 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[SiegePotionMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::SiegePotionMixMP(LPOBJ lpObj, int type, int count) // OK
{
	int BlessCount = 0;
	int SoulCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
	}

	if ((type == 0 && (BlessCount == 0 || BlessCount > 25)) || (type == 1 && (SoulCount == 0 || SoulCount > 25)))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = (BlessCount + SoulCount) * 100000 - (50000 * type);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));
	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;
	for (int i = 0; i < count; i++)
	{
		BYTE btMixNumber = 1 << (i % 8);

		pMsg.ItemInfo[i / 8] |= btMixNumber;
	}
	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 7), type, ((BlessCount + SoulCount) * 10), 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

	gLog.Output(LOG_CHAOS_MIX, "[SiegePotionMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::LifeStoneMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int GuardianCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31))
		{
			GuardianCount++;
		}
	}

	if (ChaosCount != 1 || BlessCount != 5 || SoulCount != 5 || GuardianCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 5000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 11), 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[LifeStoneMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::LifeStoneMixMP(LPOBJ lpObj, int count) // OK
{
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int GuardianCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31))
		{
			GuardianCount++;
		}
	}

	if (ChaosCount != 1 || BlessCount != 5 || SoulCount != 5 || GuardianCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 5000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 11), 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[LifeStoneMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::SeniorMix(LPOBJ lpObj) // OK
{
	if (gCastleSiegeSync.CheckCastleOwnerMember(lpObj->Index) == 0 || lpObj->GuildStatus != 0x80)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	SYSTEMTIME SystemTime;

	GetSystemTime(&SystemTime);

	if (this->m_SeniorMixLimitDay == SystemTime.wDay && this->m_SeniorMixLimitMonth == SystemTime.wMonth && this->m_SeniorMixLimitYear == SystemTime.wYear)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	int SoulCount = 0;
	int BlessCount = 0;
	int GuardianCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31))
		{
			GuardianCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (SoulCount != 30 || SoulCount != 30 || GuardianCount != 30)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000000;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	CItem item;

	if (gItemBagManager.GetItemBySpecialValue(ITEM_BAG_SENIOR_MIX, lpObj, &item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	this->m_SeniorMixLimitDay = SystemTime.wDay;
	this->m_SeniorMixLimitMonth = SystemTime.wMonth;
	this->m_SeniorMixLimitYear = SystemTime.wYear;

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, (BYTE)item.m_Level, 0, item.m_Option1, item.m_Option2, item.m_Option3, -1, item.m_NewOption, item.m_SetOption, item.m_JewelOfHarmonyOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, 0);
	
	gLog.Output(LOG_CHAOS_MIX, "[SeniorMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::PieceOfHornMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int SplinterOfArmorCount = 0;
	int BlessOfGuardianCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 32))
		{
			SplinterOfArmorCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 33))
		{
			BlessOfGuardianCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (ChaosCount != 1 || SplinterOfArmorCount != 20 || BlessOfGuardianCount != 20)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_PieceOfHornMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 35), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[PieceOfHornMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[PieceOfHornMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::BrokenHornMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ClawOfBeastCount = 0;
	int PieceOfHornCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 34))
		{
			ClawOfBeastCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 35))
		{
			PieceOfHornCount++;
		}
	}

	if (ChaosCount != 1 || ClawOfBeastCount != 10 || PieceOfHornCount != 5)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_BrokenHornMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 36), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[BrokenHornMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[BrokenHornMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::HornOfFenrirMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int BrokenHornCount = 0;
	int LifeCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 36))
		{
			BrokenHornCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 16))
		{
			LifeCount++;
		}
	}

	if (ChaosCount != 1 || BrokenHornCount != 1 || LifeCount != 3)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_HornOfFenrirMixRate[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 37), 0, 255, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[HornOfFenrirMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[HornOfFenrirMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::HornOfFenrirUpgradeMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int HornOfFenrirCount = 0;
	int LifeCount = 0;
	int WeaponCount = 0;
	int WeaponMoney = 0;
	int ArmorCount = 0;
	int ArmorMoney = 0;
	int MagicStoneItem = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 37))
		{
			HornOfFenrirCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 16))
		{
			LifeCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(0, 0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(6, 0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			WeaponCount++;
			WeaponMoney = lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(6, 0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(12, 0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ArmorCount++;
			ArmorMoney = lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 404) && lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 406)) //Magic Stones
		{
			MagicStoneItem += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (ChaosCount != 1 || HornOfFenrirCount != 1 || LifeCount != 5 || (WeaponCount == 0 && ArmorCount == 0) || (WeaponCount != 0 && ArmorCount != 0))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (WeaponMoney + ArmorMoney) / 10000;
		lpObj->ChaosSuccessRate += (MagicStoneItem * 5);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 79) ? 79 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		BYTE ItemNewOption = ((WeaponCount == 0) ? ((ArmorCount == 0) ? 0 : 2) : 1);
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 37), 0, 255, 1, 0, 0, -1, ItemNewOption, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[HornOfFenrirUpgradeMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[HornOfFenrirUpgradeMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::ShieldPotionMix(LPOBJ lpObj, int type) // OK
{
	int LargeHealingPotionCount = 0;
	int SmallCompoundPotionCount = 0;
	int MediumCompoundPotionCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 3))
		{
			LargeHealingPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 38))
		{
			SmallCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 39))
		{
			MediumCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if ((type == 0 && LargeHealingPotionCount != 3) || (type == 1 && SmallCompoundPotionCount != 3) || (type == 2 && MediumCompoundPotionCount != 3))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	switch (type)
	{
	case 0:
		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate1[lpObj->AccountLevel];
		break;
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate2[lpObj->AccountLevel];
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate3[lpObj->AccountLevel];
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = ((type == 0) ? 100000 : (500000 * type));

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(14, 35) + type), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[ShieldPotionMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[ShieldPotionMix][Failure][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::ShieldPotionMixMP(LPOBJ lpObj, int type, int count) // OK
{
	int LargeHealingPotionCount = 0;
	int SmallCompoundPotionCount = 0;
	int MediumCompoundPotionCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 3))
		{
			LargeHealingPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 38))
		{
			SmallCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 39))
		{
			MediumCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if ((type == 0 && LargeHealingPotionCount != (3 * count)) || (type == 1 && SmallCompoundPotionCount != (3 * count)) || (type == 2 && MediumCompoundPotionCount != (3 * count)))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	switch (type)
	{
	case 0:
		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate1[lpObj->AccountLevel];
		break;
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate2[lpObj->AccountLevel];
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_ShieldPotionMixRate3[lpObj->AccountLevel];
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = ((type == 0) ? 100000 : (500000 * type));

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));

	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;
	for (int i = 0; i < count; i++)
	{
		if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);

			pMsg.ItemInfo[i / 8] |= btMixNumber;
			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(14, 35) + type), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

			gLog.Output(LOG_CHAOS_MIX, "[ShieldPotionMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		else
		{
			this->ChaosBoxInit(lpObj);

			this->GCChaosBoxSend(lpObj, 0);

			this->GCChaosMixSend(lpObj->Index, 0, 0);

			gLog.Output(LOG_CHAOS_MIX, "[ShieldPotionMix][Failure][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void CChaosBox::JewelOfHarmonyItemPurityMix(LPOBJ lpObj, int count) // OK
{
	int GemStoneCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 41))
		{
			GemStoneCount++;
		}
	}

	if (GemStoneCount != count)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_JewelOfHarmonyItemPurityMixRate[lpObj->AccountLevel];
	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));
	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	int SuccessCount = 0;

	while (GemStoneCount > 0)
	{
		if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
		{
			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 42), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
			gLog.Output(LOG_CHAOS_MIX, "[JewelOfHarmonyItemPurityMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
			SuccessCount++;

			BYTE btMixNumber = 1 << (GemStoneCount % 8);
			pMsg.ItemInfo[GemStoneCount / 8] |= btMixNumber;
		}
		else
		{
			gLog.Output(LOG_CHAOS_MIX, "[JewelOfHarmonyItemPurityMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		
		GemStoneCount--;
	}

	if(SuccessCount == 0)
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);		
	}
	else
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
}

void CChaosBox::JewelOfHarmonyItemSmeltMix(LPOBJ lpObj) // OK
{
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (gJewelOfHarmonyType.CheckJewelOfHarmonyItemType(&lpObj->ChaosBox[n]) != 0 && lpObj->ChaosBox[n].IsSetItem() == 0 && lpObj->ChaosBox[n].IsJewelOfHarmonyItem() == 0)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (lpObj->ChaosBox[ItemSlot].IsExcItem() == 0)
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_JewelOfHarmonyItemSmeltMixRate1[lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_JewelOfHarmonyItemSmeltMixRate2[lpObj->AccountLevel];
	}

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = ((lpObj->ChaosBox[ItemSlot].IsExcItem() == 0) ? GET_ITEM(14, 43) : GET_ITEM(14, 44));
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[JewelOfHarmonyItemSmeltMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[JewelOfHarmonyItemSmeltMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::JewelOfHarmonyItemSmeltMixMP(LPOBJ lpObj, int count) // OK
{
	int ItemCount = 0;
	int ItemSlot[10] = { 0 };

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (gJewelOfHarmonyType.CheckJewelOfHarmonyItemType(&lpObj->ChaosBox[n]) != 0 && lpObj->ChaosBox[n].IsSetItem() == 0 && lpObj->ChaosBox[n].IsJewelOfHarmonyItem() == 0)
		{
			ItemCount++;
			ItemSlot[ItemCount - 1] = n;
		}
	}

	if (ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));

	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;

	for (int i = 0; i < count; i++)
	{
		if (lpObj->ChaosBox[ItemSlot[i]].IsExcItem() == 0)
		{
			lpObj->ChaosSuccessRate = gServerInfo.m_JewelOfHarmonyItemSmeltMixRate1[lpObj->AccountLevel];
		}
		else
		{
			lpObj->ChaosSuccessRate = gServerInfo.m_JewelOfHarmonyItemSmeltMixRate2[lpObj->AccountLevel];
		}

		if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;
			WORD ItemIndex = ((lpObj->ChaosBox[ItemSlot[i]].IsExcItem() == 0) ? GET_ITEM(14, 43) : GET_ITEM(14, 44));

			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

			gLog.Output(LOG_CHAOS_MIX, "[JewelOfHarmonyItemSmeltMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		else
		{
			this->ChaosBoxInit(lpObj);
			this->GCChaosBoxSend(lpObj, 0);
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			gLog.Output(LOG_CHAOS_MIX, "[JewelOfHarmonyItemSmeltMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void CChaosBox::JewelOfHarmonyItemRestoreMix(LPOBJ lpObj) // OK
{
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].IsJewelOfHarmonyItem() != 0)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = gJewelOfHarmonyOption.GetJewelOfHarmonyItemRestoreMoney(&lpObj->ChaosBox[ItemSlot]);

	if (lpObj->ChaosMoney == -1)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item = lpObj->ChaosBox[ItemSlot];

	item.m_JewelOfHarmonyOption = JEWEL_OF_HARMONY_OPTION_NONE;

	this->ChaosBoxInit(lpObj);

	gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);

	this->GCChaosMixSend(lpObj->Index, 1, &item);

	gLog.Output(LOG_CHAOS_MIX, "[JewelOfHarmonyItemRestoreMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::Item380Mix(LPOBJ lpObj) // OK
{
	int GuardianCount = 0;
	int HarmonyCount = 0;
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31))
		{
			GuardianCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 42))
		{
			HarmonyCount++;
		}
		else if (lpObj->ChaosBox[n].Is380Item() == 0 && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (GuardianCount != 1 || HarmonyCount != 1 || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (lpObj->ChaosBox[ItemSlot].m_Level >= 7)
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Item380MixRate2[lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Item380MixRate1[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->ChaosBox[ItemSlot];
		CItem itemOld = lpObj->ChaosBox[ItemSlot];

		item.m_ItemOptionEx |= 0x80;

		this->ChaosBoxInit(lpObj);

		gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);

		this->GCChaosMixSend(lpObj->Index, 1, &item);

		gLog.Output(LOG_CHAOS_MIX, "[Item380Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

		GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 0, 2);
	}
	else
	{
		this->ChaosBoxItemKeep(lpObj, ItemSlot);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[Item380Mix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::IllusionTempleMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel = 0;
	int PotionCount = 0;
	int PotionLevel = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 49))
		{
			ScrollCount++;
			ScrollLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 50))
		{
			PotionCount++;
			PotionLevel = lpObj->ChaosBox[n].m_Level;
		}
	}

	if (ChaosCount != 1 || ScrollCount != 1 || PotionCount != 1 || ScrollLevel != PotionLevel)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (ScrollLevel > 6 || PotionLevel > 6)
	{
		this->GCChaosMixSend(lpObj->Index, 8, 0);
		return;
	}

	switch (ScrollLevel)
	{
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate1[lpObj->AccountLevel];
		lpObj->ChaosMoney = 3000000;
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate2[lpObj->AccountLevel];
		lpObj->ChaosMoney = 5000000;
		break;
	case 3:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate3[lpObj->AccountLevel];
		lpObj->ChaosMoney = 7000000;
		break;
	case 4:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate4[lpObj->AccountLevel];
		lpObj->ChaosMoney = 9000000;
		break;
	case 5:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate5[lpObj->AccountLevel];
		lpObj->ChaosMoney = 11000000;
		break;
	case 6:
		lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate6[lpObj->AccountLevel];
		lpObj->ChaosMoney = 13000000;
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 51), ScrollLevel, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[IllusionTempleMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[IllusionTempleMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::IllusionTempleMixMP(LPOBJ lpObj, int count) // OK
{
	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel[10] = { 0 };
	int PotionCount = 0;
	int PotionLevel[10] = { 0 };

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 49))
		{
			ScrollCount++;
			ScrollLevel[ScrollCount - 1] = lpObj->ChaosBox[n].m_Level;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 50))
		{
			PotionCount++;
			PotionLevel[PotionCount - 1] = lpObj->ChaosBox[n].m_Level;
		}
	}
	for (int level = 0; level < count; level++)
	{
		if (ChaosCount != (1 * count) || ScrollCount != (1 * count) || PotionCount != (1 * count) || ScrollLevel[level] != PotionLevel[level])
		{
			this->GCChaosMixSend(lpObj->Index, 7, 0);
			return;
		}

		if (ScrollLevel[level] > 6 || PotionLevel[level] > 6)
		{
			this->GCChaosMixSend(lpObj->Index, 8, 0);
			return;
		}

		switch (ScrollLevel[level])
		{
		case 1:
			lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate1[lpObj->AccountLevel];
			lpObj->ChaosMoney = 3000000;
			break;
		case 2:
			lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate2[lpObj->AccountLevel];
			lpObj->ChaosMoney = 5000000;
			break;
		case 3:
			lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate3[lpObj->AccountLevel];
			lpObj->ChaosMoney = 7000000;
			break;
		case 4:
			lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate4[lpObj->AccountLevel];
			lpObj->ChaosMoney = 9000000;
			break;
		case 5:
			lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate5[lpObj->AccountLevel];
			lpObj->ChaosMoney = 11000000;
			break;
		case 6:
			lpObj->ChaosSuccessRate = gServerInfo.m_IllusionTempleMixRate6[lpObj->AccountLevel];
			lpObj->ChaosMoney = 13000000;
			break;
		}
	}

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));

	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;

	for (int i = 0; i < count; i++)
	{
		if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
		{
			this->GCChaosMixSend(lpObj->Index, 240, 0);
			return;
		}

		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
		if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);

			pMsg.ItemInfo[i / 8] |= btMixNumber;
			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 51), ScrollLevel[i], 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

			gLog.Output(LOG_CHAOS_MIX, "[IllusionTempleMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
		else
		{
			this->ChaosBoxInit(lpObj);
			this->GCChaosBoxSend(lpObj, 0);
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			gLog.Output(LOG_CHAOS_MIX, "[IllusionTempleMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void CChaosBox::FeatherOfCondorMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulCount = 0;
	int WingCount = 0;
	int ItemMoney = 0;
	int MagicStoneItem = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if ((lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 269) || (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 3) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 6)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 42) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 49) || lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 30)) && (lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1))
		{
			WingCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 406))
		{
			MagicStoneItem += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || SoulCount != 10 || WingCount != 1 || MagicStoneItem < 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (MagicStoneItem * 5);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 60) ? 60 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 200000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 53), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[FeatherOfCondorMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[FeatherOfCondorMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::Wing3Mix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulCount = 0;
	int BlessCount = 0;
	int FlameCount = 0;
	int FeatherCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int MagicStoneItem = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 52))
		{
			FlameCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 53))
		{
			FeatherCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 405))
		{
			MagicStoneItem += (int)lpObj->ChaosBox[n].m_Durability;
		}		
	}

	if (ChaosCount != 30 || CreationCount != 30 || SoulCount != 30 || BlessCount != 30 || FlameCount != 1 || FeatherCount != 1 || MagicStoneItem < 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_Wing3MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (MagicStoneItem * 5);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Wing3MixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_Wing3MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 40) ? 40 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 200000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;
		BYTE ItemNewOption = 0;

		CRandomManager RandomManager;

		RandomManager.AddElement(GET_ITEM(12, 36), 1);
		RandomManager.AddElement(GET_ITEM(12, 37), 1);
		RandomManager.AddElement(GET_ITEM(12, 38), 1);
		RandomManager.AddElement(GET_ITEM(12, 39), 1);
		RandomManager.AddElement(GET_ITEM(12, 40), 1);
		RandomManager.AddElement(GET_ITEM(12, 43), 1);
		RandomManager.AddElement(GET_ITEM(12, 50), 1);
		RandomManager.AddElement(GET_ITEM(12, 270), 1);

		RandomManager.GetRandomElement(&ItemIndex);

		gItemOptionRate.GetItemOption2(6, &ItemOption2);
		gItemOptionRate.GetItemOption3(6, &ItemOption3);
		gItemOptionRate.GetItemOption4(6, &ItemNewOption);

		gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + (16 * (GetLargeRand() % 3))), 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[Wing3Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[Wing3Mix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::GarudaFeather(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulCount = 0;
	int BlessCount = 0;
	int WingCount = 0;
	int ItemMoney = 0;
	int MagicStoneItem = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if ( ( (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 36) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 40))
			|| lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 43)
			|| lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 50)
			|| lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 270) ) 
			&& (lpObj->ChaosBox[n].m_Level >= 13 && lpObj->ChaosBox[n].m_Option3 >= 1) )
		{
			WingCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 406))
		{
			MagicStoneItem += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (ChaosCount != 30 || CreationCount != 30 || SoulCount != 30 || BlessCount != 30 || WingCount != 1 || MagicStoneItem < 1)
	{
		//LogAdd(LOG_DEBUG,"%d %d %d %d %d %d", ChaosCount,CreationCount,SoulCount,BlessCount,WingCount,MagicStoneItem);
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = (MagicStoneItem * 5);

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 60) ? 60 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 200000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 467), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[GarudaFeatherMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[GarudaFeatherMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::Wing4Mix(LPOBJ lpObj) // OK
{
	int CreationCount = 0;
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int HigherMagicStone = 0;
	int GarudaFlame = 0;
	int GarudasFeather = 0;
	int GoldenCrest = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (lpObj->ChaosBox[i].IsItem() == TRUE)
		{
			if (lpObj->ChaosBox[i].m_Index == GET_ITEM(14, 13))
			{
				BlessCount += (int)lpObj->ChaosBox[i].m_Durability;
			}
			else if (lpObj->ChaosBox[i].m_Index == GET_ITEM(14, 14))
			{
				SoulCount += (int)lpObj->ChaosBox[i].m_Durability;
			}
			else if (lpObj->ChaosBox[i].m_Index == GET_ITEM(14, 22))
			{
				CreationCount += (int)lpObj->ChaosBox[i].m_Durability;
			}
			else if (lpObj->ChaosBox[i].m_Index == GET_ITEM(12, 15))
			{
				ChaosCount += (int)lpObj->ChaosBox[i].m_Durability;
			}
			else if (lpObj->ChaosBox[i].m_Index == GET_ITEM(12, 406))
			{
				HigherMagicStone += (int)lpObj->ChaosBox[i].m_Durability;
			}			
			else if (lpObj->ChaosBox[i].m_Index == GET_ITEM(13, 466))
			{
				GarudaFlame++;
			}
			else if (lpObj->ChaosBox[i].m_Index == GET_ITEM(13, 467))
			{
				GarudasFeather++;
			}
			else if (lpObj->ChaosBox[i].m_Index == GET_ITEM(13, 468))
			{
				GoldenCrest += (int)lpObj->ChaosBox[i].m_Durability;
			}
		}
	}

	if (CreationCount != 30 || ChaosCount != 30 || BlessCount != 30 || SoulCount != 30 || HigherMagicStone < 1 || GarudaFlame == 0 || GarudasFeather == 0 || GoldenCrest < 50)
	{
		this->GCChaosMixSend(lpObj->Index, 7, NULL);
		return;
	}

	if (gServerInfo.m_Wing4MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (HigherMagicStone * 5);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_Wing4MixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, NULL);
		return;
	}

	if (gServerInfo.m_Wing4MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 40) ? 40 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}	

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 200000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, NULL);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;
		BYTE ItemNewOption = 0;

		CRandomManager RandomManager;

		RandomManager.AddElement(GET_ITEM(12, 414), 1);
		RandomManager.AddElement(GET_ITEM(12, 415), 1);
		RandomManager.AddElement(GET_ITEM(12, 416), 1);
		RandomManager.AddElement(GET_ITEM(12, 417), 1);
		RandomManager.AddElement(GET_ITEM(12, 418), 1);
		RandomManager.AddElement(GET_ITEM(12, 419), 1);
		RandomManager.AddElement(GET_ITEM(12, 420), 1);
		RandomManager.AddElement(GET_ITEM(12, 421), 1);

		RandomManager.GetRandomElement(&ItemIndex);

		gItemOptionRate.GetItemOption2(6, &ItemOption2);
		gItemOptionRate.GetItemOption3(6, &ItemOption3);
		gItemOptionRate.GetItemOption4(6, &ItemNewOption);
		gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

		CRandomManager RandomOptions;

		RandomOptions.AddElement(0, 100);
		RandomOptions.AddElement(16, 100);
		RandomOptions.AddElement(32, 100);
		RandomOptions.AddElement(48, 100);
		RandomOptions.AddElement(64, 100);
		RandomOptions.AddElement(80, 100);
		RandomOptions.AddElement(96, 100);
		RandomOptions.AddElement(112, 100);
		RandomOptions.AddElement(128, 100);
		RandomOptions.AddElement(144, 100);
		RandomOptions.AddElement(160, 100);
		RandomOptions.AddElement(176, 100);
		RandomOptions.AddElement(192, 100);
		RandomOptions.AddElement(208, 100);
		RandomOptions.AddElement(224, 100);

		BYTE SocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

		int optionsQuant = (GetLargeRand() % 4);

		for (int i = 0; i < optionsQuant; i++)
		{
			RandomOptions.GetRandomElementUnique(&SocketOption[i]);
			SocketOption[i] += (GetLargeRand() % 4);
		}

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SocketOption, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[WingLv4Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, NULL);
		gLog.Output(LOG_CHAOS_MIX, "[WingLv4Mix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::ChaosCardMix(LPOBJ lpObj, int type) // OK
{
	int ChaosCardCount = 0;
	int ChaosCardGoldCount = 0;
	int ChaosCardRareCount = 0;
	int ChaosCardMiniCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 54))
		{
			type = 1;
			ChaosCardCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 92))
		{
			type = 2;
			ChaosCardGoldCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 93))
		{
			type = 3;
			ChaosCardRareCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 95))
		{
			type = 4;
			ChaosCardMiniCount++;
		}
	}

	if (type == 0 || (type == 1 && ChaosCardCount != 1) || (type == 2 && ChaosCardGoldCount != 1) || (type == 3 && ChaosCardRareCount != 1) || (type == 4 && ChaosCardMiniCount != 1))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	CItem item;

	if (gItemBagManager.GetItemBySpecialValue((ITEM_BAG_CHAOS_CARD_MIX1 + (type - 1)), lpObj, &item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, (BYTE)item.m_Level, 0, item.m_Option1, item.m_Option2, item.m_Option3, -1, item.m_NewOption, item.m_SetOption, item.m_JewelOfHarmonyOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[ChaosCardMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::CherryBlossomMix(LPOBJ lpObj, int type) // OK
{
	int WhiteCherryBlossomCount = 0;
	int RedCherryBlossomCount = 0;
	int GoldenCherryBlossomCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 88))
		{
			type = 1;
			WhiteCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 89))
		{
			type = 2;
			RedCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 90))
		{
			type = 3;
			GoldenCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if (type == 0 || (type == 1 && WhiteCherryBlossomCount != 10) || (type == 2 && RedCherryBlossomCount != 30) || (type == 3 && GoldenCherryBlossomCount != 255))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	CItem item;

	if (gItemBagManager.GetItemBySpecialValue((ITEM_BAG_CHERRY_BLOSSOM_MIX1 + (type - 1)), lpObj, &item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, (BYTE)item.m_Level, 0, item.m_Option1, item.m_Option2, item.m_Option3, -1, item.m_NewOption, item.m_SetOption, item.m_JewelOfHarmonyOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[CherryBlossomMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::SocketItemCreateSeedMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int HarmonyCount = 0;
	int ExcItemCount = 0;
	int SetItemCount = 0;
	int ItemMoney = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 42))
		{
			HarmonyCount++;
		}
		else if (lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4 && ExcItemCount == 0)
		{
			ExcItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].IsSetItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4 && SetItemCount == 0)
		{
			SetItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || HarmonyCount != 1 || ExcItemCount != 1 || SetItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney / 2000000) + 80;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel];
	}

	if (gServerInfo.m_SocketItemCreateSeedMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 90) ? 90 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		int SocketOptionType = 1 + (GetLargeRand() % MAX_SOCKET_ITEM_OPTION_TYPE);

		int SocketOptionIndex = gSocketItemOption.GetSocketItemRandomOption(SocketOptionType);

		int SocketOptionValue = gSocketItemOption.GetSocketItemSeedOption(SocketOptionIndex, SocketOptionType);

		if (SocketOptionIndex == -1 || SocketOptionValue == -1)
		{
			this->GCChaosMixSend(lpObj->Index, 0, 0);
			return;
		}

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(12, 60) + (SocketOptionType - 1)), SocketOptionValue, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[SocketItemCreateSeedMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[SocketItemCreateSeedMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::SocketItemCreateSeedSphereMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int SeedCount = 0;
	int SeedType = 0;
	int SeedLevel = 0;
	int SphereCount = 0;
	int SphereType = 0;
	int ItemMoney = 0;
	int LightLevel;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 112) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 117))
		{
			LightLevel = 1;
			SeedCount++;
			SeedType = lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 112);
			SeedLevel = lpObj->ChaosBox[n].m_Level;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(14, 245))
		{
			lpObj->ChaosSuccessRate += 60;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(14, 246))
		{
			lpObj->ChaosSuccessRate += 45;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(14, 247))
		{
			lpObj->ChaosSuccessRate += 30;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 60) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 66))
		{
			SeedCount++;
			SeedType = lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 60);
			SeedLevel = lpObj->ChaosBox[n].m_Level;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 70) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 74))
		{
			SphereCount++;
			SphereType = lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 70);
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if (ChaosCount != 10 || CreationCount != 10 || SeedCount != 1 || SphereCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel] == -1)
	{
		if (LightLevel == 1)
		{
			lpObj->ChaosSuccessRate += 20;
		}
		else
		{
			lpObj->ChaosSuccessRate = (ItemMoney / 200000) + 80;
		}
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel];
	}

	if (gServerInfo.m_SocketItemCreateSeedSphereMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 90) ? 90 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(12, 100) + (SphereType * 6) + SeedType), SeedLevel, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[SocketItemCreateSeedSphereMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[SocketItemCreateSeedSphereMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::SocketItemMountSeedSphereMix(LPOBJ lpObj, BYTE info) // OK
{
	if (info >= MAX_SOCKET_ITEM_OPTION_TABLE)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	int ChaosCount = 0;
	int CreationCount = 0;
	int SeedSphereCount = 0;
	int SeedSphereType = 0;
	int SeedSphereLevel = 0;
	int SeedSphereOption = 0;
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 100) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 129))
		{
			SeedSphereCount++;
			SeedSphereType = (lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 100)) % MAX_SOCKET_ITEM_OPTION_TYPE;
			SeedSphereLevel = (lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 100)) / MAX_SOCKET_ITEM_OPTION_TYPE;
			SeedSphereOption = lpObj->ChaosBox[n].m_Level;
		}
		else if (gSocketItemType.CheckSocketItemType(lpObj->ChaosBox[n].m_Index) != 0 && lpObj->ChaosBox[n].m_SocketOption[info] == SOCKET_ITEM_OPTION_EMPTY)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || SeedSphereCount != 1 || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	BYTE SocketOption = gSocketItemOption.GetSocketItemOption(&lpObj->ChaosBox[ItemSlot], (SeedSphereType + 1), SeedSphereOption, SeedSphereLevel);

	if (SocketOption == SOCKET_ITEM_OPTION_NONE)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	CItem item = lpObj->ChaosBox[ItemSlot];

	item.m_SocketOption[info] = SocketOption;

	item.m_SocketOptionBonus = gSocketItemOption.GetSocketItemBonusOption(&item);

	this->ChaosBoxInit(lpObj);

	gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);

	this->GCChaosMixSend(lpObj->Index, 1, &item);

	gLog.Output(LOG_CHAOS_MIX, "[SocketItemMountSeedSphereMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::SocketItemUnMountSeedSphereMix(LPOBJ lpObj, BYTE info) // OK
{
	if (info >= MAX_SOCKET_ITEM_OPTION_TABLE)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_SocketOption[info] != SOCKET_ITEM_OPTION_NONE && lpObj->ChaosBox[n].m_SocketOption[info] != SOCKET_ITEM_OPTION_EMPTY)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item = lpObj->ChaosBox[ItemSlot];

	item.m_SocketOption[info] = SOCKET_ITEM_OPTION_EMPTY;

	item.m_SocketOptionBonus = gSocketItemOption.GetSocketItemBonusOption(&item);

	this->ChaosBoxInit(lpObj);

	gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);

	this->GCChaosMixSend(lpObj->Index, 1, &item);
}

void CChaosBox::ImperialGuardianMix(LPOBJ lpObj) // OK
{
	int DimensionalPart1Count = 0;
	int DimensionalPart2Count = 0;
	int DimensionalPart3Count = 0;
	int DimensionalPart4Count = 0;
	int DimensionalPart5Count = 0;
	int DimensionalPart6Count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 103))
		{
			DimensionalPart1Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 104))
		{
			DimensionalPart2Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 105))
		{
			DimensionalPart3Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 106))
		{
			DimensionalPart4Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 107))
		{
			DimensionalPart5Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 108))
		{
			DimensionalPart6Count++;
		}
	}

	if (DimensionalPart1Count != 1 && DimensionalPart2Count != 1 && DimensionalPart3Count != 1 && DimensionalPart4Count != 1 && DimensionalPart5Count != 1 && DimensionalPart6Count != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 109), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[ImperialGuardianMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::ChestMix(LPOBJ lpObj) // OK
{
	int GoldKeyCount = 0;
	int GoldChestCount = 0;
	int SilverKeyCount = 0;
	int SilverChestCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 112))
		{
			SilverKeyCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 113))
		{
			GoldKeyCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 121))
		{
			GoldChestCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 122))
		{
			SilverChestCount++;
		}
	}

	if ((GoldKeyCount != 1 && SilverKeyCount != 1) || (GoldKeyCount == 1 && GoldChestCount != 1) || (SilverKeyCount == 1 && SilverChestCount != 1))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(14, 123) + SilverKeyCount), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[ChestMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::ChestMixMP(LPOBJ lpObj, int count) // OK
{
	int GoldKeyCount = 0;
	int GoldChestCount = 0;
	int SilverKeyCount = 0;
	int SilverChestCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 112))
		{
			SilverKeyCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 113))
		{
			GoldKeyCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 121))
		{
			GoldChestCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 122))
		{
			SilverChestCount++;
		}
	}

	if ((GoldKeyCount != (1 * count) && SilverKeyCount != (1 * count)) || (GoldKeyCount == (1 * count) && GoldChestCount != (1 * count)) || (SilverKeyCount == (1 * count) && SilverChestCount != (1 * count)))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	PMSG_CHAOS_MIX_SEND pMsg;
	pMsg.header.set(0x86, sizeof(PMSG_CHAOS_MIX_SEND));

	pMsg.result = 16;
	pMsg.Pos = count;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	lpObj->ChaosMassMixCurrItem = 0;

	for (int i = 0; i < count; i++)
	{
		BYTE btMixNumber = 1 << (i % 8);

		pMsg.ItemInfo[i / 8] |= btMixNumber;
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(14, 123) + (SilverKeyCount / count)), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
	}

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	gLog.Output(LOG_CHAOS_MIX, "[ChestMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::SummonScrollMix(LPOBJ lpObj, int type) // OK
{
	int SummonScroll1Count = 0;
	int SummonScroll2Count = 0;
	int SummonScroll3Count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 210))
		{
			if (SummonScroll2Count) { type = 2; }
			else if (SummonScroll3Count) { type = 3; }
			else { type = 1; }

			SummonScroll1Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 211))
		{
			type = 2;
			SummonScroll2Count++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 212))
		{
			type = 3;
			SummonScroll3Count++;
		}
	}

	if (type == 0 || (type == 1 && SummonScroll1Count != 2) || (type == 2 && (SummonScroll1Count != 1 || SummonScroll2Count != 1)) || (type == 3 && (SummonScroll1Count != 1 || SummonScroll3Count != 1)))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	switch (type)
	{
	case 1:
		lpObj->ChaosSuccessRate = gServerInfo.m_SummonScrollMixRate1[lpObj->AccountLevel];
		lpObj->ChaosMoney = 100000;
		break;
	case 2:
		lpObj->ChaosSuccessRate = gServerInfo.m_SummonScrollMixRate2[lpObj->AccountLevel];
		lpObj->ChaosMoney = 200000;
		break;
	case 3:
		lpObj->ChaosSuccessRate = gServerInfo.m_SummonScrollMixRate3[lpObj->AccountLevel];
		lpObj->ChaosMoney = 300000;
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;
	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, (GET_ITEM(14, 210) + type), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[SummonScrollMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[SummonScrollMix][Failure][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::LuckyItemCreateMix(LPOBJ lpObj) // OK
{
	int LuckyTicketCount = 0;
	int LuckyTicketGroup = 0;
	int LuckyTicketSection = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 135))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 8;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 136))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 9;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 137))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 7;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 138))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 10;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 139))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 0;
			LuckyTicketSection = 11;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 140))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 8;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 141))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 9;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 142))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 7;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 143))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 10;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 144))
		{
			LuckyTicketCount++;
			LuckyTicketGroup = 1;
			LuckyTicketSection = 11;
		}
	}

	if (LuckyTicketCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	WORD ItemIndex = 0;
	BYTE ItemLevel = 0;
	BYTE ItemOption1 = 0;
	BYTE ItemOption2 = 0;
	BYTE ItemOption3 = 0;
	BYTE ItemNewOption = 0;
	BYTE ItemSetOption = 0;
	BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

	if (gLuckyItem.GetLuckyItemIndex(lpObj, LuckyTicketSection, LuckyTicketGroup, &ItemIndex) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	gLuckyItem.GetLuckyItemOption0(ItemIndex, &ItemLevel);
	gLuckyItem.GetLuckyItemOption1(ItemIndex, &ItemOption1);
	gLuckyItem.GetLuckyItemOption2(ItemIndex, &ItemOption2);
	gLuckyItem.GetLuckyItemOption3(ItemIndex, &ItemOption3);
	gLuckyItem.GetLuckyItemOption4(ItemIndex, &ItemNewOption);
	gLuckyItem.GetLuckyItemOption5(ItemIndex, &ItemSetOption);
	gLuckyItem.GetLuckyItemOption6(ItemIndex, &ItemSocketOption[0]);
	gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
	gItemOptionRate.MakeSetOption(ItemIndex, ItemSetOption, &ItemSetOption);
	gItemOptionRate.MakeSocketOption(ItemIndex, ItemSocketOption[0], &ItemSocketOption[0]);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, ItemLevel, 255, ItemOption1, ItemOption2, ItemOption3, -1, ItemNewOption, ItemSetOption, 0, 0, ItemSocketOption, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[LuckyItemCreateMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::LuckyItemRefineMix(LPOBJ lpObj) // OK
{
	int LuckyItemCount = 0;
	int LuckyItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].IsLuckyItem() != 0)
		{
			LuckyItemCount++;
			LuckyItemSlot = n;
		}
	}

	if (LuckyItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (lpObj->ChaosBox[LuckyItemSlot].m_Durability == 255)
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_LuckyItemRefineMixRate1[lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_LuckyItemRefineMixRate2[lpObj->AccountLevel];
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 160), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[LuckyItemRefineMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj, 0);

		this->GCChaosMixSend(lpObj->Index, 0, 0);

		gLog.Output(LOG_CHAOS_MIX, "[LuckyItemRefineMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::MonsterWingMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int MaterialType = 0;
	int WingItemCount = 0;
	int WingItemMoney = 0;
	int WingItemType = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 176))
		{
			MaterialType = 1;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 177))
		{
			MaterialType = 2;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 178))
		{
			MaterialType = 3;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 179))
		{
			MaterialType = 4;
		}
		else if ((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 3) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 6)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 42))
		{
			WingItemCount++;
			WingItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			WingItemType = 0;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 269) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 49) || lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 30))
		{
			WingItemCount++;
			WingItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
			WingItemType = 1;
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || MaterialType == 0 || WingItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_MonsterWingMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = WingItemMoney / ((WingItemType == 0) ? 9000000 : 500000);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_MonsterWingMixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_MonsterWingMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 60) ? 60 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate * 100000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;
		BYTE ItemNewOption = 0;

		switch (MaterialType)
		{
		case 1:
			ItemIndex = GET_ITEM(12, 262);
			break;
		case 2:
			ItemIndex = GET_ITEM(12, 263);
			break;
		case 3:
			ItemIndex = GET_ITEM(12, 264);
			break;
		case 4:
			ItemIndex = GET_ITEM(12, 265);
			break;
		}

		gItemOptionRate.GetItemOption2(7, &ItemOption2);
		gItemOptionRate.GetItemOption3(7, &ItemOption3);
		gItemOptionRate.GetItemOption4(7, &ItemNewOption);

		gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + (16 * (GetLargeRand() % 2))), 0, 0, 0, 0, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[MonsterWingMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[MonsterWingMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::SocketWeaponMix(LPOBJ lpObj) // OK
{
	int IceWalkerManeCount = 0;
	int MammothBundleOfFurCount = 0;
	int GiantIcicleCount = 0;
	int CoolutinPoisonCount = 0;
	int IronKnightHeartCount = 0;
	int DarkMammothHornCount = 0;
	int DarkGiantAxeCount = 0;
	int DarkCoolutinBloodCount = 0;
	int DarkIronKnightSwordCount = 0;
	int CombinationNoteType = 0;
	int SocketItemCount = 0;
	int SocketItemMoney = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 180))
		{
			IceWalkerManeCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 181))
		{
			MammothBundleOfFurCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 182))
		{
			GiantIcicleCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 183))
		{
			CoolutinPoisonCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 184))
		{
			IronKnightHeartCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 185))
		{
			DarkMammothHornCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 186))
		{
			DarkGiantAxeCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 187))
		{
			DarkCoolutinBloodCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 188))
		{
			DarkIronKnightSwordCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(14, 191) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(14, 202))
		{
			CombinationNoteType = lpObj->ChaosBox[n].m_Index - GET_ITEM(14, 190);
		}
		else if (gSocketItemType.CheckSocketItemType(lpObj->ChaosBox[n].m_Index) != 0 && lpObj->ChaosBox[n].m_Level >= 7 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			SocketItemCount++;
			SocketItemMoney += lpObj->ChaosBox[n].m_Value;
		}
	}

	if (CombinationNoteType == 0 || SocketItemCount == 0 || (CombinationNoteType == 1 && (IceWalkerManeCount != 2 || DarkIronKnightSwordCount != 1)) || (CombinationNoteType == 2 && (MammothBundleOfFurCount != 2 || DarkMammothHornCount != 1)) || (CombinationNoteType == 3 && (GiantIcicleCount != 2 || DarkIronKnightSwordCount != 1)) || (CombinationNoteType == 4 && (CoolutinPoisonCount != 2 || DarkMammothHornCount != 1)) || (CombinationNoteType == 5 && (GiantIcicleCount != 2 || DarkGiantAxeCount != 1)) || (CombinationNoteType == 6 && (IceWalkerManeCount != 2 || DarkMammothHornCount != 1)) || (CombinationNoteType == 7 && (IronKnightHeartCount != 2 || DarkCoolutinBloodCount != 1)) || (CombinationNoteType == 8 && (GiantIcicleCount != 2 || DarkMammothHornCount != 1)) || (CombinationNoteType == 9 && (IronKnightHeartCount != 2 || DarkGiantAxeCount != 1)) || (CombinationNoteType == 10 && (IronKnightHeartCount != 2 || DarkIronKnightSwordCount != 1)) || (CombinationNoteType == 11 && (MammothBundleOfFurCount != 2 || DarkCoolutinBloodCount != 1)) || (CombinationNoteType == 12 && (IronKnightHeartCount != 2 || DarkMammothHornCount != 1)))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (gServerInfo.m_SocketWeaponMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = SocketItemMoney / 360000;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo.m_SocketWeaponMixRate[lpObj->AccountLevel];
	}

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	if (gServerInfo.m_SocketWeaponMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 40) ? 40 : lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 1000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

		switch (CombinationNoteType)
		{
		case 1:
			ItemIndex = GET_ITEM(0, 29);
			break;
		case 2:
			ItemIndex = GET_ITEM(0, 36);
			break;
		case 3:
			ItemIndex = GET_ITEM(0, 37);
			break;
		case 4:
			ItemIndex = GET_ITEM(3, 12);
			break;
		case 5:
			ItemIndex = GET_ITEM(2, 20);
			break;
		case 6:
			ItemIndex = GET_ITEM(4, 25);
			break;
		case 7:
			ItemIndex = GET_ITEM(4, 26);
			break;
		case 8:
			ItemIndex = GET_ITEM(5, 35);
			break;
		case 9:
			ItemIndex = GET_ITEM(5, 37);
			break;
		case 10:
			ItemIndex = GET_ITEM(0, 30);
			break;
		case 11:
			ItemIndex = GET_ITEM(2, 19);
			break;
		case 12:
			ItemIndex = GET_ITEM(5, 32);
			break;
		}

		gItemOptionRate.GetItemOption2(8, &ItemOption2);

		gItemOptionRate.GetItemOption6(((gItemManager.GetItemTwoHand(ItemIndex) == 0) ? 6 : 7), &ItemSocketOption[0]);

		gItemOptionRate.MakeSocketOption(ItemIndex, ItemSocketOption[0], &ItemSocketOption[0]);

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 1, ItemOption2, 0, -1, 0, 0, 0, 0, ItemSocketOption, 0xFF, 0);

		gLog.Output(LOG_CHAOS_MIX, "[SocketWeaponMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[SocketWeaponMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::PentagramMithrilMixSXI(LPOBJ lpObj, int Mithril, int Bless) // 2017/04/23
{
	int MithrilCount = 0;
	int BlessPack = 0;
	int MithrilType = 0;

	if (lpObj->Inventory[Mithril].m_Index == GET_ITEM(12, 145))
	{
		MithrilCount++;
		MithrilType = lpObj->Inventory[Mithril].m_SocketOptionBonus;
	}

	if (lpObj->Inventory[Bless].m_Index == GET_ITEM(14, 13))
	{
		BlessPack++;
	}

	if (MithrilCount != 1 || BlessPack != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 250, 0);
		return;
	}

	if (CHECK_RANGE((MithrilType - 1), MAX_PENTAGRAM_ELEMENTAL_ATTRIBUTE) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 251, 0);
		return;
	}
	lpObj->ChaosSuccessRate = gServerInfo.m_PentagramJewelMixRate[lpObj->AccountLevel];

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 100000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 249, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE SocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };
		BYTE SocketOptionBonus = MithrilType | 16;

		CRandomManager RandomManager;

		RandomManager.AddElement(GET_ITEM(12, 221), 5000);
		RandomManager.AddElement(GET_ITEM(12, 231), 4000);
		RandomManager.AddElement(GET_ITEM(12, 241), 3000);
		RandomManager.AddElement(GET_ITEM(12, 251), 2000);
		RandomManager.AddElement(GET_ITEM(12, 261), 1000);

		RandomManager.GetRandomElement(&ItemIndex);

		if (gPentagramSystem.GetPentagramRandomJewelOption(ItemIndex, 1, &SocketOption[0]) == 0)
		{
			this->GCChaosMixSend(lpObj->Index, 248, 0);
			return;
		}

		CItem item;

		item.Clear();

		item.m_Index = ItemIndex;

		memcpy(item.m_SocketOption, SocketOption, MAX_SOCKET_OPTION);

		item.m_SocketOptionBonus = SocketOptionBonus;

		gItemManager.InventoryDelItem(lpObj->Index, Mithril);
		gItemManager.GCItemDeleteSend(lpObj->Index, Mithril, 1);

		gItemManager.DecreaseItemDur(lpObj, Bless, 1);
		gItemManager.GCItemModifySend(lpObj->Index, Bless);

		BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

		PMSG_RHEA_ITEM_BUY_SEND pMsg;

		pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

		pMsg.result = Result;

		gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

		gPentagramSystem.GCPentagramMixSend(lpObj->Index, 1);

		lpObj->ChaosLock = 0;

		gLog.Output(LOG_CHAOS_MIX, "[PentagramJewelMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

		GDItemTrackingSaveSend(item, item, lpObj->Account, lpObj->Account, 0, 3);
	}
	else
	{
		gItemManager.InventoryDelItem(lpObj->Index, Mithril);
		gItemManager.GCItemDeleteSend(lpObj->Index, Mithril, 1);

		gItemManager.DecreaseItemDur(lpObj, Bless, 1);
		gItemManager.GCItemModifySend(lpObj->Index, Bless);

		gPentagramSystem.GCPentagramMixSend(lpObj->Index, 0);
		lpObj->ChaosLock = 0;

		gLog.Output(LOG_CHAOS_MIX, "[PentagramJewelMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::PentagramEcstasySlotMix(LPOBJ lpObj, BYTE Debris1, BYTE Debris2, BYTE Debris3, BYTE Debris4) // 2017/04/25
{
	int DebrisCount1 = 0;
	int DebrisCount2 = 0;
	int DebrisCount3 = 0;
	int DebrisCount4 = 0;

	if (lpObj->Inventory[Debris1].m_Index == GET_ITEM(14, 309))
	{
		DebrisCount1++;
	}

	if (lpObj->Inventory[Debris2].m_Index == GET_ITEM(14, 310))
	{
		DebrisCount2++;
	}

	if (lpObj->Inventory[Debris3].m_Index == GET_ITEM(14, 311))
	{
		DebrisCount3++;
	}

	if (lpObj->Inventory[Debris4].m_Index == GET_ITEM(14, 312))
	{
		DebrisCount4++;
	}

	if (DebrisCount1 != 1 || DebrisCount2 != 1 || DebrisCount3 != 1 || DebrisCount4 != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 250, 0);
		return;
	}

	lpObj->ChaosMoney = 100000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 249, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item;

	item.Clear();

	item.m_Index = GET_ITEM(14, 316);

	gItemManager.InventoryDelItem(lpObj->Index, Debris1);
	gItemManager.GCItemDeleteSend(lpObj->Index, Debris1, 1);

	gItemManager.InventoryDelItem(lpObj->Index, Debris2);
	gItemManager.GCItemDeleteSend(lpObj->Index, Debris2, 1);

	gItemManager.InventoryDelItem(lpObj->Index, Debris3);
	gItemManager.GCItemDeleteSend(lpObj->Index, Debris3, 1);

	gItemManager.InventoryDelItem(lpObj->Index, Debris4);
	gItemManager.GCItemDeleteSend(lpObj->Index, Debris4, 1);

	BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

	PMSG_RHEA_ITEM_BUY_SEND pMsg;

	pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

	pMsg.result = Result;

	gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

	gPentagramSystem.GCPentagramMixSend(lpObj->Index, 1);

	lpObj->ChaosLock = 0;

	GDItemTrackingSaveSend(item, item, lpObj->Account, lpObj->Account, 0, 4);
}

void CChaosBox::PentagramElixirMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ElixirFragmentBunchCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 149))
		{
			ElixirFragmentBunchCount++;
		}
	}

	if (ChaosCount != 1 || ElixirFragmentBunchCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 250, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_PentagramElixirMixRate[lpObj->AccountLevel];

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 100000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 249, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = GET_ITEM(12, 147);
		BYTE SocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };
		BYTE SocketOptionBonus = 0xFF;

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SocketOption, SocketOptionBonus, 0);

		gLog.Output(LOG_CHAOS_MIX, "[PentagramElixirMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 226, 0);
		gLog.Output(LOG_CHAOS_MIX, "[PentagramElixirMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::PentagramJewelMix(LPOBJ lpObj) // OK
{
	int BlessCount = 0;
	int MithrilCount = 0;
	int MithrilType = 0;
	int ElixirCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 145))
		{
			MithrilCount++;
			MithrilType = lpObj->ChaosBox[n].m_SocketOptionBonus;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 147))
		{
			ElixirCount++;
		}
	}

	if (BlessCount != 1 || MithrilCount != 1 || ElixirCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 250, 0);
		return;
	}

	if (CHECK_RANGE((MithrilType - 1), MAX_PENTAGRAM_ELEMENTAL_ATTRIBUTE) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 251, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_PentagramJewelMixRate[lpObj->AccountLevel];

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 100000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 249, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE SocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };
		BYTE SocketOptionBonus = MithrilType | 16;

		CRandomManager RandomManager;

		RandomManager.AddElement(GET_ITEM(12, 221), 4000);
		RandomManager.AddElement(GET_ITEM(12, 231), 3000);
		RandomManager.AddElement(GET_ITEM(12, 241), 1500);
		RandomManager.AddElement(GET_ITEM(12, 251), 1000);
		RandomManager.AddElement(GET_ITEM(12, 261), 500);

		RandomManager.GetRandomElement(&ItemIndex);

		if (gPentagramSystem.GetPentagramRandomJewelOption(ItemIndex, 1, &SocketOption[0]) == 0)
		{
			this->GCChaosMixSend(lpObj->Index, 248, 0);
			return;
		}

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SocketOption, SocketOptionBonus, 0);

		gLog.Output(LOG_CHAOS_MIX, "[PentagramJewelMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 226, 0);
		gLog.Output(LOG_CHAOS_MIX, "[PentagramJewelMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::PentagramDecompositeMix(LPOBJ lpObj, int type) // OK
{
	int HarmonyCount = 0;
	int PentagramItemType = 0;
	int PentagramItemSlot = 0;
	int PentagramDecompositeType = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 42))
		{
			HarmonyCount++;
		}
		if (lpObj->ChaosBox[n].IsPentagramItem() != 0)
		{
			PentagramItemType = lpObj->ChaosBox[n].m_SocketOptionBonus % 16;
			PentagramItemSlot = n;
			PentagramDecompositeType = lpObj->ChaosBox[n].m_Index - GET_ITEM(12, 199);
		}
	}

	if (HarmonyCount != 1 || PentagramDecompositeType == 0 || (PentagramDecompositeType != (type + 1)))
	{
		this->GCChaosMixSend(lpObj->Index, 250, 0);
		return;
	}

	if (CHECK_RANGE((PentagramItemType - 1), MAX_PENTAGRAM_ELEMENTAL_ATTRIBUTE) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 251, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_PentagramDecompositeMixRate[lpObj->AccountLevel];

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 100000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 249, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	gPentagramSystem.DelAllPentagramJewelInfo(lpObj, &lpObj->ChaosBox[PentagramItemSlot], 0);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = 0;
		BYTE SocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };
		BYTE SocketOptionBonus = 0xFF;

		CRandomManager RandomManager;

		RandomManager.AddElement(GET_ITEM(12, 144), 5000);
		RandomManager.AddElement(GET_ITEM(12, 145), 1500);
		RandomManager.AddElement(GET_ITEM(12, 146), 2500);
		RandomManager.AddElement(GET_ITEM(12, 147), 500);
		RandomManager.AddElement(GET_ITEM(12, 150), 500);

		RandomManager.GetRandomElement(&ItemIndex);

		SocketOptionBonus = ((ItemIndex == GET_ITEM(12, 144)) ? PentagramItemType : SocketOptionBonus);
		SocketOptionBonus = ((ItemIndex == GET_ITEM(12, 145)) ? PentagramItemType : SocketOptionBonus);

		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SocketOption, SocketOptionBonus, 0);

		gLog.Output(LOG_CHAOS_MIX, "[PentagramDecompositeMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 226, 0);
		gLog.Output(LOG_CHAOS_MIX, "[PentagramDecompositeMix][Failure][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, type, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::PentagramJewelUpgradeLevelMixSXI(LPOBJ lpObj, BYTE info, BYTE Element, BYTE Rune, BYTE Talisman, BYTE Amulet, BYTE btLevelupTargetRank, BYTE btLevelupSpecialMode) // OK
{
	int ElementCount = 0;
	int RuneCount = 0;
	int TalismanRate = 0;
	int AmuletCount = 0;

	BYTE RuneNumber[3][10] = { {1,1,2,2,3,3,4,5,6,7},{3,3,4,4,5,5,6,7,8,9},{5,5,6,6,7,7,8,9,10,11} };

	if (lpObj->Inventory[Element].IsPentagramJewel() != 0)
	{
		ElementCount++;
	}

	if (lpObj->Inventory[Talisman].m_Index == GET_ITEM(14, 189))
	{
		TalismanRate = 10;
	}

	if (lpObj->Inventory[Amulet].m_Index == GET_ITEM(14, 190))
	{
		AmuletCount++;
	}

	if (lpObj->Inventory[Rune].m_Index == GET_ITEM(12, 288) &&
		lpObj->Inventory[Rune].m_Durability >= RuneNumber[btLevelupTargetRank-1][info - 1])
	{
		RuneCount++;
		lpObj->Inventory[Rune].m_Durability -= RuneNumber[btLevelupTargetRank-1][info - 1];
	}

	if (ElementCount != 1 || RuneCount != 1)
	{
		LogAdd(LOG_RED, "Errtel Upgrade TimerRandom %d", __LINE__);
		this->GCChaosMixSend(lpObj->Index, 250, 0);
		return;
	}

	if (CHECK_RANGE((btLevelupTargetRank - 1), MAX_PENTAGRAM_JEWEL_RANK) == 0)
	{
		LogAdd(LOG_RED, "Errtel Upgrade TimerRandom %d", __LINE__);
		this->GCChaosMixSend(lpObj->Index, 252, 0);
		return;
	}
	
	/*if (CHECK_RANGE(lpObj->Inventory[Element].m_Level, (MAX_PENTAGRAM_JEWEL_LEVEL + 1)) == 0 || lpObj->Inventory[Element].m_Level != info)
	{
		LogAdd(LOG_RED, "Errtel Upgrade TimerRandom %d", __LINE__);
		this->GCChaosMixSend(lpObj->Index, 252, 0);
		return;
	}*/

	PENTAGRAM_JEWEL_UPGRADE_LEVEL_INFO PentagramJewelUpgradeLevelInfo;

	if (gPentagramSystem.GetPentagramJewelUpgradeLevelInfo((info - 1), &PentagramJewelUpgradeLevelInfo) == 0)
	{
		LogAdd(LOG_RED, "Errtel Upgrade TimerRandom %d", __LINE__);
		this->GCChaosMixSend(lpObj->Index, 248, 0);
		return;
	}

	lpObj->ChaosSuccessRate = TalismanRate + PentagramJewelUpgradeLevelInfo.MixRate[btLevelupTargetRank-1] / 100;

	this->GetElementalTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = PentagramJewelUpgradeLevelInfo.MixMoney;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		LogAdd(LOG_RED, "Errtel Upgrade TimerRandom %d", __LINE__);
		this->GCChaosMixSend(lpObj->Index, 249, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->Inventory[Element];
		CItem itemOld = lpObj->Inventory[Element];

		if (item.m_Level < 10) { item.m_Level++; }

		item.m_SocketOption[btLevelupTargetRank-1] += 16;

		gItemManager.GCItemDurSend(lpObj->Index, Rune, (BYTE)lpObj->Inventory[Rune].m_Durability, 0);
		
		if (lpObj->Inventory[Rune].m_Durability < 1)
		{
			gItemManager.InventoryDelItem(lpObj->Index, Rune);
			gItemManager.GCItemDeleteSend(lpObj->Index, Rune, 1);
		}

		gItemManager.InventoryDelItem(lpObj->Index, Amulet);
		gItemManager.GCItemDeleteSend(lpObj->Index, Amulet, 1);

		gItemManager.InventoryDelItem(lpObj->Index, Talisman);
		gItemManager.GCItemDeleteSend(lpObj->Index, Talisman, 1);

		gItemManager.InventoryDelItem(lpObj->Index, Element);
		gItemManager.GCItemDeleteSend(lpObj->Index, Element, 1);

		BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

		PMSG_RHEA_ITEM_BUY_SEND pMsg;

		pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

		pMsg.result = Result;

		gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

		gPentagramSystem.GCPentagramMixSend(lpObj->Index, 1);

		lpObj->ChaosLock = 0;

		gLog.Output(LOG_CHAOS_MIX, "[PentagramJewelUpgradeLevelMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

		GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 0, 3);
	}
	else
	{
		LogAdd(LOG_RED, "Errtel Upgrade TimerRandom %d", __LINE__);

		if (AmuletCount == 0)
		{
			//gItemManager.InventoryDelItem(lpObj->Index,Amulet);
			//gItemManager.GCItemDeleteSend(lpObj->Index,Amulet,1);

			gItemManager.GCItemDurSend(lpObj->Index, Rune, (BYTE)lpObj->Inventory[Rune].m_Durability, 0);

			gItemManager.InventoryDelItem(lpObj->Index, Element);
			gItemManager.GCItemDeleteSend(lpObj->Index, Element, 1);

			gItemManager.InventoryDelItem(lpObj->Index, Talisman);
			gItemManager.GCItemDeleteSend(lpObj->Index, Talisman, 1);

			gLog.Output(LOG_CHAOS_MIX, "[PentagramJewelUpgradeLevelMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d, ChaosAmulet: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney, Amulet);
		}
		else
		{
			gItemManager.InventoryDelItem(lpObj->Index, Amulet);
			gItemManager.GCItemDeleteSend(lpObj->Index, Amulet, 1);

			gItemManager.GCItemDurSend(lpObj->Index, Rune, (BYTE)lpObj->Inventory[Rune].m_Durability, 0);

			gLog.Output(LOG_CHAOS_MIX, "[PentagramJewelUpgradeLevelMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d, ChaosAmulet: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney, Amulet);
		}

		gPentagramSystem.GCPentagramMixSend(lpObj->Index, 0);
	}
}

void CChaosBox::PentagramJewelUpgradeRankMixSXI(LPOBJ lpObj, BYTE info, BYTE Element1, BYTE Element2)
{
	int PentagramJewelCount = 0;

	if (lpObj->Inventory[Element1].IsPentagramJewel() != 0 && lpObj->Inventory[Element1].m_Level >= 7
		&& lpObj->Inventory[Element2].IsPentagramJewel() != 0 && lpObj->Inventory[Element2].m_Level >= 7
		&& lpObj->Inventory[Element1].m_Index == lpObj->Inventory[Element2].m_Index)
	{
		PentagramJewelCount++;
	}

	if (PentagramJewelCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 250, 0);
		return;
	}

	if ((CHECK_RANGE(((lpObj->Inventory[Element1].m_SocketOptionBonus / 16) - 1), (MAX_PENTAGRAM_JEWEL_RANK - 1)) == 0 || (lpObj->Inventory[Element1].m_SocketOptionBonus / 16) != (info - 1))
		&& (CHECK_RANGE(((lpObj->Inventory[Element2].m_SocketOptionBonus / 16) - 1), (MAX_PENTAGRAM_JEWEL_RANK - 1)) == 0 || (lpObj->Inventory[Element2].m_SocketOptionBonus / 16) != (info - 1)))
	{
		this->GCChaosMixSend(lpObj->Index, 252, 0);
		return;
	}

	PENTAGRAM_JEWEL_UPGRADE_RANK_INFO PentagramJewelUpgradeRankInfo;

	if (gPentagramSystem.GetPentagramJewelUpgradeRankInfo((info - 1), &PentagramJewelUpgradeRankInfo) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 248, 0);
		return;
	}

	lpObj->ChaosMoney = PentagramJewelUpgradeRankInfo.MixMoney;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 249, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item = lpObj->Inventory[Element1];
	CItem itemOld = lpObj->Inventory[Element1];

	item.m_Level = 0;

	item.m_SocketOptionBonus += 16;

	if (gPentagramSystem.GetPentagramRandomJewelOption(item.m_Index, (item.m_SocketOptionBonus / 16), &item.m_SocketOption[((item.m_SocketOptionBonus / 16) - 1)]) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 248, 0);
		return;
	}

	gItemManager.InventoryDelItem(lpObj->Index, Element2);
	gItemManager.GCItemDeleteSend(lpObj->Index, Element2, 1);

	gItemManager.InventoryDelItem(lpObj->Index, Element1);
	gItemManager.GCItemDeleteSend(lpObj->Index, Element1, 1);

	BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

	PMSG_RHEA_ITEM_BUY_SEND pMsg;

	pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

	pMsg.result = Result;

	gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

	lpObj->ChaosLock = 0;

	gPentagramSystem.GCPentagramMixSend(lpObj->Index, 1);

	gLog.Output(LOG_CHAOS_MIX, "[PentagramJewelUpgradeRankMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 0, 6);
}

void CChaosBox::CGChaosMixRecv(PMSG_CHAOS_MIX_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpObj->ChaosLock != 0)
	{
		return;
	}

	if (lpObj->PShopOpen != 0)
	{
		return;
	}

	lpObj->ChaosLock = 1;
	lpObj->ChaosMoney = 0;
	lpObj->ChaosSuccessRate = 0;
	lpObj->IsChaosMixCompleted = 1;
	lpObj->ChaosMassMixCurrItem = 0;

	//For testing new mixes
	LogAdd(LOG_DEBUG, "lpMsg->type: %d | lpMsg->Count: %d", lpMsg->type, lpMsg->Count);
	//this->GCChaosMixSend(lpObj->Index, 7, 0);
	//return;

	if (lpMsg->type >= CHAOS_MIX_BLUE_EYE_WEAPON_START && lpMsg->type <= CHAOS_MIX_BLUE_EYE_WEAPON_END)
	{
		this->BlueEyeWeaponMix(lpObj);
		return;
	}

	if (lpMsg->type >= CHAOS_MIX_BLUE_EYE_SET_START && lpMsg->type <= CHAOS_MIX_BLUE_EYE_SET_END)
	{
		this->FrostBlueEyeItemMix(lpObj);
		return;
	}

	if ((lpMsg->type >= CHAOS_MIX_SOUL_WEAPON_START && lpMsg->type <= CHAOS_MIX_SOUL_WEAPON_END) ||
		lpMsg->type == CHAOS_MIX_SOUL_BOOK)
	{
		this->SoulWeaponMix(lpObj);
		return;
	}

	if (lpMsg->type >= CHAOS_MIX_AWAKENING_START && lpMsg->type <= CHAOS_MIX_AWAKENING_END)
	{
		this->AwakeningSoulItemMix(lpObj);
		return;
	}

	if ((lpMsg->type >= CHAOS_MIX_HOLYANGEL_WEAPON_START && lpMsg->type <= CHAOS_MIX_HOLYANGEL_WEAPON_END) || 
		lpMsg->type == CHAOS_MIX_HOLYANGEL_SHIELD || lpMsg->type == CHAOS_MIX_HOLYANGEL_BOOK)
	{
		this->HolyangelWeaponMix(lpObj);
		return;
	}

	if ((lpMsg->type >= CHAOS_MIX_HOLYANGEL_START && lpMsg->type <= CHAOS_MIX_HOLYANGEL_END) ||
		(lpMsg->type >= CHAOS_MIX_HOLYANGEL_ELF_START && lpMsg->type <= CHAOS_MIX_HOLYANGEL_ELF_END) ||
		(lpMsg->type >= CHAOS_MIX_HOLYANGEL_MG_START && lpMsg->type <= CHAOS_MIX_HOLYANGEL_MG_END))
	{
		this->HolyangelItemMix(lpObj);
		return;
	}

	if ((lpMsg->type >= CHAOS_MIX_DARKANGEL_WEAPON_START && lpMsg->type <= CHAOS_MIX_DARKANGEL_WEAPON_END) ||
		lpMsg->type == CHAOS_MIX_DARKANGEL_QUIVER || lpMsg->type == CHAOS_MIX_DARKANGEL_SHIELD ||
		lpMsg->type == CHAOS_MIX_DARKANGEL_BOOK)
	{
		this->DarkAngelWeaponMix(lpObj);
		return;
	}

	if ((lpMsg->type >= CHAOS_MIX_DARK_ANGEL_ITEM_MIN && lpMsg->type <= CHAOS_MIX_DARK_ANGEL_ITEM_MAX) ||
		(lpMsg->type >= CHAOS_MIX_DARKANGEL_ELF_START && lpMsg->type <= CHAOS_MIX_DARKANGEL_ELF_END) ||
		(lpMsg->type >= CHAOS_MIX_DARKANGEL_MG_START && lpMsg->type <= CHAOS_MIX_DARKANGEL_MG_END))
	{
		this->DarkAngelMix(lpObj);
		return;
	}

	if ((lpMsg->type >= CHAOS_MIX_BLOODANGEL1_START && lpMsg->type <= CHAOS_MIX_BLOODANGEL1_END) ||
		(lpMsg->type >= CHAOS_MIX_BLOODANGEL2_START && lpMsg->type <= CHAOS_MIX_BLOODANGEL2_END) ||
		(lpMsg->type >= CHAOS_MIX_BLOODANGEL3_START && lpMsg->type <= CHAOS_MIX_BLOODANGEL3_END) ||
		(lpMsg->type >= CHAOS_MIX_BLOODANGEL4_START && lpMsg->type <= CHAOS_MIX_BLOODANGEL4_END))
	{
		this->BloodItemMix(lpObj);
		return;
	}

	switch (lpMsg->type)
	{

	case CHAOS_MIX_CHAOS_ITEM:
		this->ChaosItemMix(lpObj);
		break;

	case CHAOS_MIX_DEVIL_SQUARE:		
		if (lpMsg->Count == 1)
		{
			this->DevilSquareMix(lpObj);
		}
		else
		{
			this->DevilSquareMixMP(lpObj, lpMsg->Count);
		}		
		break;

	case CHAOS_MIX_PLUS_ITEM_LEVEL1:
		this->PlusItemLevelMix(lpObj, 0);		
		break;

	case CHAOS_MIX_PLUS_ITEM_LEVEL2:
		this->PlusItemLevelMix(lpObj, 1);
		break;

	case CHAOS_MIX_DINORANT:
		if (lpMsg->Count == 1)
		{
			this->DinorantMix(lpObj);
		}
		else
		{
			this->DinorantMixMP(lpObj, lpMsg->Count);
		}
		break;

	case CHAOS_MIX_FRUIT:
		if (lpMsg->Count == 1)
		{
			this->FruitMix(lpObj);
		}
		else
		{
			this->FruitMixMP(lpObj, lpMsg->Count);
		}
		break;

	case CHAOS_MIX_WING1:
		this->Wing2Mix(lpObj, 0);
		break;

	case CHAOS_MIX_CAPES:
		this->Wing2Mix(lpObj, 1);
		break;

	case CHAOS_MIX_BLOOD_CASTLE:		
		if (lpMsg->Count == 1)
		{
			this->BloodCastleMix(lpObj);
		}
		else
		{
			this->BloodCastleMixMP(lpObj, lpMsg->Count);
		}		
		break;
	
	case CHAOS_MIX_WING2:
		this->Wing1Mix(lpObj);
		break;

	case CHAOS_MIX_PET1:
		this->PetMix(lpObj, 0);
		break;

	case CHAOS_MIX_PET2:
		this->PetMix(lpObj, 1);
		break;

	case CHAOS_MIX_SIEGE_POTION1:
		if (lpMsg->Count == 1)
		{
			this->SiegePotionMix(lpObj, 0);
		}
		else
		{
			this->SiegePotionMixMP(lpObj, 0, lpMsg->Count);
		}
		break;

	case CHAOS_MIX_SIEGE_POTION2:
		if (lpMsg->Count == 1)
		{
			this->SiegePotionMix(lpObj, 1);
		}
		else
		{
			this->SiegePotionMixMP(lpObj, 1, lpMsg->Count);
		}
		break;

	case CHAOS_MIX_LIFE_STONE:		
			if (lpMsg->Count == 1)
			{
				this->LifeStoneMix(lpObj);
			}
			else
			{
				this->LifeStoneMixMP(lpObj, lpMsg->Count);
			}
		break;

	case CHAOS_MIX_SENIOR:
		this->SeniorMix(lpObj);
		break;

	case CHAOS_MIX_PLUS_ITEM_LEVEL3:
		this->PlusItemLevelMix(lpObj, 2);
		break;
	case CHAOS_MIX_PLUS_ITEM_LEVEL4:
		this->PlusItemLevelMix(lpObj, 3);
		break;
	/*case CHAOS_MIX_WING3:
		this->Wing3Mix(lpObj);
		break;*/
	case CHAOS_MIX_PIECE_OF_HORN:
		this->PieceOfHornMix(lpObj);
		break;
	case CHAOS_MIX_BROKEN_HORN:
		this->BrokenHornMix(lpObj);
		break;
	case CHAOS_MIX_HORN_OF_FENRIR:
		this->HornOfFenrirMix(lpObj);
		break;
	case CHAOS_MIX_HORN_OF_FENRIR_UPGRADE:
		this->HornOfFenrirUpgradeMix(lpObj);
		break;

	case CHAOS_MIX_SHIELD_POTION1:
		if (lpMsg->Count == 1)
		{
			this->ShieldPotionMix(lpObj, 0);
		}
		else
		{
			this->ShieldPotionMixMP(lpObj, 0, lpMsg->Count);
		}
		break;
	case CHAOS_MIX_SHIELD_POTION2:
		if (lpMsg->Count == 1)
		{
			this->ShieldPotionMix(lpObj, 1);
		}
		else
		{
			this->ShieldPotionMixMP(lpObj, 1, lpMsg->Count);
		}
		break;
	case CHAOS_MIX_SHIELD_POTION3:
		if (lpMsg->Count == 1)
		{
			this->ShieldPotionMix(lpObj, 2);
		}
		else
		{
			this->ShieldPotionMixMP(lpObj, 2, lpMsg->Count);
		}
		break;
	case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_PURITY:
		this->JewelOfHarmonyItemPurityMix(lpObj, lpMsg->Count);
		break;
	case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_SMELT:
		this->JewelOfHarmonyItemSmeltMix(lpObj);
		break;
	case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_RESTORE:
		this->JewelOfHarmonyItemRestoreMix(lpObj);
		break;
	case CHAOS_MIX_ITEM_380:
		this->Item380Mix(lpObj);
		break;
	case CHAOS_MIX_ILLUSION_TEMPLE:
		if (lpMsg->Count == 1)
		{
			this->IllusionTempleMix(lpObj);
		}
		else
		{
			this->IllusionTempleMixMP(lpObj, lpMsg->Count);
		}
		break;
	case CHAOS_MIX_FEATHER_OF_CONDOR:
		this->FeatherOfCondorMix(lpObj);
		break;
	case CHAOS_MIX_WING3:
		this->Wing3Mix(lpObj);
		break;	
	case CHAOS_MIX_CHAOS_CARD:
		this->ChaosCardMix(lpObj, 0);
		break;

	case CHAOS_MIX_CHERRY_BLOSSOM:
		this->CherryBlossomMix(lpObj, 0);
		break;

	case CHAOS_MIX_IMPERIAL_GUARDIAN:
		this->ImperialGuardianMix(lpObj);
		break;
	case CHAOS_MIX_CHEST:
		if (lpMsg->Count == 1)
		{
			this->ChestMix(lpObj);
		}
		else
		{
			this->ChestMixMP(lpObj, lpMsg->Count);
		}
		break;
	case CHAOS_MIX_SUMMON_SCROLL:
		this->SummonScrollMix(lpObj, 0);
		break;

	case CHAOS_MIX_PLUS_ITEM_LEVEL5:
		this->PlusItemLevelMix(lpObj, 4);
		break;

	case CHAOS_MIX_PLUS_ITEM_LEVEL6:
		this->PlusItemLevelMix(lpObj, 5);
		break;

	case CHAOS_MIX_LUCKY_ITEM_CREATE:
		this->LuckyItemCreateMix(lpObj);
		break;

	case CHAOS_MIX_LUCKY_ITEM_REFINE:
		this->LuckyItemRefineMix(lpObj);
		break;

	case CHAOS_MIX_MONSTER_WING:
		this->MonsterWingMix(lpObj);
		break;
	case CHAOS_MIX_SOCKET_WEAPON:
		this->SocketWeaponMix(lpObj);
		break;
	case CHAOS_MIX_CONQUEROR_WING:
		this->ConquerorWingMix(lpObj);
		break;
	case CHAOS_MIX_DIABLO:
		this->DiabloRingMix(lpObj);
		break;
	case CHAOS_MIX_ANCIENTSOUL:
		this->AncientSoulMix(lpObj);
		break;
	case CHAOS_ARCHANGEL_OF_HAMMER:
		this->ArchangelsHammerMIX(lpObj);
		break;
	case CHAOS_BLESSED_DIVINE_SWORD_OF_ARCHA:
		this->BlessedDivineSwordOfArchangelMix(lpObj);
		break;
	case CHAOS_MIX_MAGIC_STONE:
		this->MagicStonemMix(lpObj, lpMsg->Count);
		break;

	case CHAOS_MIX_BOUND_WINGS1:
		this->BoundWings2ndMix(lpObj);
		break;

	case CHAOS_MIX_ELF_CHANGE:
		this->FEItemChange(lpObj, 1);
		break;

	case CHAOS_MIX_MG_CHANGE:
		this->MGItemChange(lpObj, 0);
		break;

	case CHAOS_MIX_ELF_CHANGE2:
		this->FEItemChange(lpObj, 0);
		break;

	case CHAOS_MIX_MG_CHANGE2:
		this->MGItemChange(lpObj, 1);
		break;

	case CHAOS_MIX_BOUND_WINGS2:
		this->BoundWings3rdMix(lpObj);
		break;

	case CHAOS_MIX_GARUDA_FEATHER:
		this->GarudaFeather(lpObj);
		break;

	case CHAOS_MIX_WING4:
		this->Wing4Mix(lpObj);
		break;

	default:
		LogAdd(LOG_DEBUG, "Chaos Mix Default: lpMsg->type: %d | lpMsg->Count: %d", lpMsg->type, lpMsg->Count);
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		break;
	}	
}

void CChaosBox::HolyangelItemMix(LPOBJ lpObj)
{
	int SoulCount = 0;
	int BlessCount = 0;
	int ChaosCount = 0;
	int CreationCount = 0;
	int SpiritDivineAngel = 0;
	int DrakAngelItemCount = 0;
	int DrakAngelItemSolt = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 389))
		{
			SpiritDivineAngel++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].IsSetItem() && lpObj->ChaosBox[n].m_Option3 >= 4 && lpObj->ChaosBox[n].m_Level >= 11)
		{
			DrakAngelItemCount++;
			DrakAngelItemSolt = n;
		}
	}

	if (SoulCount != 30 || BlessCount != 30 || SpiritDivineAngel != 1 || CreationCount != 1 || DrakAngelItemCount != 1 || ChaosCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item = lpObj->ChaosBox[DrakAngelItemSolt];

	int ItemIndex = gRheaGold.BloodItemMixList(lpObj->ChaosBox[DrakAngelItemSolt].m_Index);

	if (ItemIndex == item.m_Index)
	{
		item.m_Index += 12;
	}
	else
	{
		item.m_Index = ItemIndex;
	}

	item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, (BYTE)item.m_Durability, item.m_Option1, item.m_Option2, 0, -1, item.m_NewOption, item.m_SetOption, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Holyangel Set Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::AwakeningSoulItemMix(LPOBJ lpObj)
{
	int SoulCount = 0;
	int BlessCount = 0;
	int ChaosCount = 0;
	int CreationCount = 0;
	int AwakeningSoul = 0;
	int HolyAngelItemCount = 0;
	int HolyAngelItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 415))
		{
			AwakeningSoul++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].IsSetItem() && lpObj->ChaosBox[n].m_Option3 >= 4 && lpObj->ChaosBox[n].m_Level >= 11)
		{
			HolyAngelItemCount++;
			HolyAngelItemSlot = n;
		}
	}

	if (SoulCount != 30 || BlessCount != 30 || AwakeningSoul != 1 || CreationCount != 1 || HolyAngelItemCount != 1 || ChaosCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item = lpObj->ChaosBox[HolyAngelItemSlot];

	int ItemIndex = gRheaGold.BloodItemMixList(item.m_Index);

	if (ItemIndex == item.m_Index)
	{
		item.m_Index -= 44;
	}
	else
	{
		item.m_Index = ItemIndex;
	}

	item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, (BYTE)item.m_Durability, item.m_Option1, item.m_Option2, 0, -1, item.m_NewOption, item.m_SetOption, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Awakening Soul Set Mix][Success][%s][%s] - (Item: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, item.m_Index, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::FrostBlueEyeItemMix(LPOBJ lpObj)
{
	int JewelAncientCount = 0;
	int JewelExcellentCount = 0;
	int JewelLuckCount = 0;
	int JewelLevelCount = 0;
	int JewelGalaxiesCount = 0;
	int ColdSpirit = 0;
	int SoulItemCount = 0;
	int SoulItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 501))
		{
			JewelAncientCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 506))
		{
			JewelExcellentCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 507))
		{
			JewelLuckCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 449))
		{
			ColdSpirit++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 502))
		{
			JewelLevelCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 511))
		{
			JewelGalaxiesCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].IsSetItem() && lpObj->ChaosBox[n].m_Option3 >= 7 && lpObj->ChaosBox[n].m_Level >= 15)
		{
			SoulItemCount++;
			SoulItemSlot = n;
		}
	}

	if (JewelLevelCount != 250 || JewelAncientCount != 250 || JewelExcellentCount != 250 || JewelLuckCount != 250 || JewelGalaxiesCount != 1 || ColdSpirit != 1 || SoulItemCount != 1 )
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	CItem item = lpObj->ChaosBox[SoulItemSlot];

	item.m_Index += 58;

	item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, (BYTE)item.m_Durability, item.m_Option1, item.m_Option2, 0, -1, item.m_NewOption, item.m_SetOption, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Frost Blue Eye Set Mix][Success][%s][%s] - (Item: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, item.m_Index, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::DarkAngelWeaponMix(LPOBJ lpObj)
{
	int SoulCount = 0;
	int BlessCount = 0;
	int ChaosCount = 0;
	int CreationCount = 0;
	int AnvliDarkAngel = 0;
	int BloodAngelItemCount = 0;
	int BloodAngelItemSolt = 0;
	int ItemIndex = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 386))
		{
			AnvliDarkAngel++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 42) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 44) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 46) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(2, 22) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(3, 19) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 28) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 34) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 41) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 43) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 57) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(6, 44))
		{
			if (lpObj->ChaosBox[n].IsExcItem() && lpObj->ChaosBox[n].m_Option3 >= 4 && lpObj->ChaosBox[n].m_Level >= 11)
			{
				BloodAngelItemCount++;
				BloodAngelItemSolt = n;
			}

		}
	}

	if (SoulCount != 30 || BlessCount != 30 || AnvliDarkAngel != 1 || CreationCount != 1 || BloodAngelItemCount != 1 || ChaosCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	switch (lpObj->ChaosBox[BloodAngelItemSolt].m_Index)
	{
	case GET_ITEM(0, 42):
		ItemIndex = GET_ITEM(0, 54);
		break;
	case GET_ITEM(0, 44):
		ItemIndex = GET_ITEM(0, 55);
		break;
	case GET_ITEM(0, 46):
		ItemIndex = GET_ITEM(0, 56);
		break;
	case GET_ITEM(2, 22):
		ItemIndex = GET_ITEM(2, 26);
		break;
	case GET_ITEM(3, 19):
		ItemIndex = GET_ITEM(3, 24);
		break;
	case GET_ITEM(4, 28):
		ItemIndex = GET_ITEM(4, 31);
		break;
	case GET_ITEM(4, 34):
		ItemIndex = GET_ITEM(4, 35);
		break;
	case GET_ITEM(5, 41):
		ItemIndex = GET_ITEM(5, 51);
		break;
	case GET_ITEM(5, 43):
		ItemIndex = GET_ITEM(5, 52);
		break;
	case GET_ITEM(5, 57):
		ItemIndex = GET_ITEM(5, 58);
		break;
	case GET_ITEM(6, 44):
		ItemIndex = GET_ITEM(6, 45);
		break;
	}

	CItem item = lpObj->ChaosBox[BloodAngelItemSolt];
	item.m_Index = ItemIndex;
	
	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, 0, item.m_Option1, item.m_Option2, 0, -1, item.m_NewOption, 0, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Darkangel Weapon Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::HolyangelWeaponMix(LPOBJ lpObj)
{
	int SoulCount = 0;
	int BlessCount = 0;
	int ChaosCount = 0;
	int CreationCount = 0;
	int HolyangelAnvil = 0;
	int BloodAngelItemCount = 0;
	int BloodAngelItemSolt = 0;
	int ItemIndex = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 416))
		{
			HolyangelAnvil++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 54) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 55) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 56) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(2, 26) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(3, 24) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 31) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 35) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 51) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 52) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 58) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(6, 45) )
		{
			if (lpObj->ChaosBox[n].IsExcItem() && lpObj->ChaosBox[n].m_Option3 >= 4 && lpObj->ChaosBox[n].m_Level >= 11)
			{
				BloodAngelItemCount++;
				BloodAngelItemSolt = n;
			}
		}
	}

	if (SoulCount != 30 || BlessCount != 30 || HolyangelAnvil != 1 || CreationCount != 1 || BloodAngelItemCount != 1 || ChaosCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	switch (lpObj->ChaosBox[BloodAngelItemSolt].m_Index)
	{
	case GET_ITEM(0, 54):
		ItemIndex = GET_ITEM(0, 57);
		break;
	case GET_ITEM(0, 55):
		ItemIndex = GET_ITEM(0, 58);
		break;
	case GET_ITEM(0, 56):
		ItemIndex = GET_ITEM(0, 59);
		break;
	case GET_ITEM(2, 26):
		ItemIndex = GET_ITEM(2, 27);
		break;
	case GET_ITEM(3, 24):
		ItemIndex = GET_ITEM(3, 25);
		break;
	case GET_ITEM(4, 31):
		ItemIndex = GET_ITEM(4, 36);
		break;
	case GET_ITEM(4, 35):
		ItemIndex = GET_ITEM(4, 37);
		break;
	case GET_ITEM(5, 51):
		ItemIndex = GET_ITEM(5, 53);
		break;
	case GET_ITEM(5, 52):
		ItemIndex = GET_ITEM(5, 54);
		break;
	case GET_ITEM(5, 58):
		ItemIndex = GET_ITEM(5, 59);
		break;
	case GET_ITEM(6, 45):
		ItemIndex = GET_ITEM(6, 46);
		break;
	}

	CItem item = lpObj->ChaosBox[BloodAngelItemSolt];

	item.m_Index = ItemIndex;

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, 0, item.m_Option1, item.m_Option2, 0, -1, item.m_NewOption, 0, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Holyangel Weapon Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::SoulWeaponMix(LPOBJ lpObj)
{
	int SoulCount = 0;
	int BlessCount = 0;
	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulAnvil = 0;
	int BloodAngelItemCount = 0;
	int BloodAngelItemSolt = 0;
	int ItemIndex = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 450))
		{
			SoulAnvil++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 57) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 58) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 59) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(2, 27) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(3, 25) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 36) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 37) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 53) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 54) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 59) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(6, 46) )
		{
			if (lpObj->ChaosBox[n].IsExcItem() && lpObj->ChaosBox[n].m_Option3 >= 4 && lpObj->ChaosBox[n].m_Level >= 11)
			{
				BloodAngelItemCount++;
				BloodAngelItemSolt = n;
			}
		}
	}

	if (SoulCount != 500 || BlessCount != 500 || SoulAnvil != 1 || CreationCount != 500 || BloodAngelItemCount != 1 || ChaosCount != 500)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	switch (lpObj->ChaosBox[BloodAngelItemSolt].m_Index)
	{
	case GET_ITEM(0, 57):
		ItemIndex = GET_ITEM(0, 62);
		break;
	case GET_ITEM(0, 58):
		ItemIndex = GET_ITEM(0, 63);
		break;
	case GET_ITEM(0, 59):
		ItemIndex = GET_ITEM(0, 64);
		break;
	case GET_ITEM(2, 27):
		ItemIndex = GET_ITEM(2, 36);
		break;
	case GET_ITEM(3, 25):
		ItemIndex = GET_ITEM(3, 28);
		break; 
	case GET_ITEM(4, 36):
		ItemIndex = GET_ITEM(4, 38);
		break;
	case GET_ITEM(4, 37):
		ItemIndex = GET_ITEM(4, 39);
		break;
	case GET_ITEM(5, 53):
		ItemIndex = GET_ITEM(5, 55);
		break;
	case GET_ITEM(5, 54):
		ItemIndex = GET_ITEM(5, 56);
		break;
	case GET_ITEM(5, 59):
		ItemIndex = GET_ITEM(5, 60);
		break;
	case GET_ITEM(6, 46):
		ItemIndex = GET_ITEM(6, 47);
		break;	
	}

	CItem item = lpObj->ChaosBox[BloodAngelItemSolt];

	item.m_Index = ItemIndex;

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, 0, item.m_Option1, item.m_Option2, 0, -1, item.m_NewOption, 0, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Soul Weapon Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::BlueEyeWeaponMix(LPOBJ lpObj)
{
	int JewelSkillCount = 0;
	int JewelExcellentCount = 0;
	int JewelLuckCount = 0;
	int JewelLevelCount = 0;
	int JewelGalaxiesCount = 0;
	int SoulItemCount = 0;
	int SoulItemSlot = 0;
	int BlueEyeAnvil = 0;
	int ItemIndex = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 500))
		{
			JewelSkillCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 506))
		{
			JewelExcellentCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 507))
		{
			JewelLuckCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 502))
		{
			JewelLevelCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 511))
		{
			JewelGalaxiesCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 451))
		{
			BlueEyeAnvil++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 62) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 63) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 64) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(2, 36) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(3, 28) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 38) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 39) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 55) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 56) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 60) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(6, 47))
		{
			if (lpObj->ChaosBox[n].IsExcItem() && lpObj->ChaosBox[n].m_Option3 >= 7 && lpObj->ChaosBox[n].m_Level >= 15)
			{
				SoulItemCount++;
				SoulItemSlot = n;
			}
		}
	}

	if (JewelSkillCount != 500 || JewelExcellentCount != 500 || JewelLuckCount != 500 || JewelLevelCount != 500 || JewelGalaxiesCount != 1 || SoulItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	switch (lpObj->ChaosBox[SoulItemSlot].m_Index)
	{
	case GET_ITEM(0, 62):
		ItemIndex = GET_ITEM(0, 78);
		break;
	case GET_ITEM(0, 63):
		ItemIndex = GET_ITEM(0, 79);
		break;
	case GET_ITEM(0, 64):
		ItemIndex = GET_ITEM(0, 80);
		break;
	case GET_ITEM(2, 36):
		ItemIndex = GET_ITEM(2, 40);
		break;
	case GET_ITEM(3, 28):
		ItemIndex = GET_ITEM(3, 29);
		break;
	case GET_ITEM(4, 38):
		ItemIndex = GET_ITEM(4, 40);
		break;
	case GET_ITEM(4, 39):
		ItemIndex = GET_ITEM(4, 41);
		break;
	case GET_ITEM(5, 55):
		ItemIndex = GET_ITEM(5, 61);
		break;
	case GET_ITEM(5, 56):
		ItemIndex = GET_ITEM(5, 62);
		break;
	case GET_ITEM(5, 60):
		ItemIndex = GET_ITEM(5, 63);
		break;
	case GET_ITEM(6, 47):
		ItemIndex = GET_ITEM(6, 48);
		break;
	}

	CItem item = lpObj->ChaosBox[SoulItemSlot];

	item.m_Index = ItemIndex;

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, 0, item.m_Option1, item.m_Option2, 0, -1, item.m_NewOption, 0, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Blue Eye Weapon Mix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::BlessedDivineSwordOfArchangelMix(LPOBJ lpObj)
{
	//disabled for IconeMU
	// return;
	this->GCChaosMixSend(lpObj->Index, 7, 0);
	

	int SoulCount = 0;
	int BlessCount = 0;
	int LifeCount = 0;
	int ArchangelsHammerCount = 0;
	int ArchangelsCount = 0;
	int ChaosCount = 0;
	int Itemsolt = 0;
	int ItemIndex = 0;
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 16))
		{
			LifeCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 343))
		{
			ArchangelsHammerCount++;
		}
		else if ((lpObj->ChaosBox[n].m_Index == GET_ITEM(0, 19) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(2, 13) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(4, 18) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 10) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(5, 36)) && lpObj->ChaosBox[n].m_Option3 >= 4 && lpObj->ChaosBox[n].m_Level == 15)
		{
			ArchangelsCount++;
			Itemsolt = n;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
	}

	if (SoulCount != 30 || BlessCount != 30 || LifeCount != 30 || ArchangelsHammerCount != 1 || ArchangelsCount != 1 || ChaosCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	switch (lpObj->ChaosBox[Itemsolt].m_Index)
	{
	case GET_ITEM(0, 19):
		ItemIndex = GET_ITEM(0, 51);
		break;
	case GET_ITEM(2, 13):
		ItemIndex = GET_ITEM(2, 25);
		break;
	case GET_ITEM(4, 18):
		ItemIndex = GET_ITEM(4, 30);
		break;
	case GET_ITEM(5, 10):
		ItemIndex = GET_ITEM(5, 49);
		break;
	case GET_ITEM(5, 36):
		ItemIndex = GET_ITEM(5, 50);
		break;
	}
	CItem item = lpObj->ChaosBox[Itemsolt];
	item.m_Index = ItemIndex;
	item.m_NewOption = 0;
	item.m_Option2 = 0;
	item.m_Option1 = 1;

	if ((GetLargeRand() % 100) > gServerInfo.m_ChaosAngelEcxMaxRate)
	{
		item.m_NewOption = 63;
	}

	if ((GetLargeRand() % 100) > gServerInfo.m_ChaosAngelLuckRate)
	{
		item.m_Option2 = 1;
	}

	item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());
	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, (BYTE)item.m_Durability, item.m_Option1, item.m_Option2, 0, -1, item.m_NewOption, 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[Blessed Divine Sword Of ArchangelMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::ArchangelsHammerMIX(LPOBJ lpObj)
{
	//disabled for IconeMU
	// return;
	this->GCChaosMixSend(lpObj->Index, 7, 0);
	

	int SteelOfHeavenCount = 0;
	int SoulCount = 0;
	int BlessCount = 0;
	int ChaosCount = 0;
	int CreationCount = 0;
	int GuardianCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 342))
		{
			SteelOfHeavenCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 31))
		{
			GuardianCount++;
		}
	}
	if (SteelOfHeavenCount != 1 || BlessCount != 10 || SoulCount != 10 || ChaosCount != 1 || CreationCount != 1 || GuardianCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(14, 343), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
	gLog.Output(LOG_CHAOS_MIX, "[Archangel's HammerlMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);

}

void CChaosBox::DarkAngelMix(LPOBJ lpObj)
{
	int SoulCount = 0;
	int BlessCount = 0;
	int ChaosCount = 0;
	int CreationCount = 0;
	int SealedStone = 0;
	int ItemCount = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 22))
		{
			CreationCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 341))
		{
			SealedStone++;
		}
		else if (lpObj->ChaosBox[n].m_Level >= 11 && lpObj->ChaosBox[n].m_Option3 >= 4 && ITEM_GET_TYPE(lpObj->ChaosBox[n].m_Index) >= 7 && ITEM_GET_TYPE(lpObj->ChaosBox[n].m_Index) <= 11)
		{
			if ((ITEM_GET_INDEX(lpObj->ChaosBox[n].m_Index) >= 98 && ITEM_GET_INDEX(lpObj->ChaosBox[n].m_Index) <= 105) || ITEM_GET_INDEX(lpObj->ChaosBox[n].m_Index) == 158 || ITEM_GET_INDEX(lpObj->ChaosBox[n].m_Index) == 161)
			{
				ItemCount++;
				ItemSlot = n;
			}			
		}
	}

	if (ChaosCount != 1 || CreationCount != 1 || BlessCount != 10 || SoulCount != 10 || SealedStone != 1 || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);
	BYTE ItemLuck = 0;
	BYTE ItemSet = 5;

	if (lpObj->ChaosBox[ItemSlot].m_Option2 == 1)
	{
		if ((GetLargeRand() % 100) < gServerInfo.m_ChaosDarkAngelLuckRate)
		{
			ItemLuck = 1;
		}
	}

	if ((GetLargeRand() % 100) < gServerInfo.m_ChaosDarkAngelSet)
	{
		ItemSet = 9;
	}

	CItem item = lpObj->ChaosBox[ItemSlot];

	int ItemIndex = gRheaGold.BloodItemMixList(lpObj->ChaosBox[ItemSlot].m_Index);

	if (ItemIndex == item.m_Index)
	{
		item.m_Index += 40;
	}
	else
	{
		item.m_Index = ItemIndex;
	}

	if (!gServerInfo.m_ChaosDrakAngelInheritBloodAngelEcx)
	{
		item.m_NewOption = 0;
	}

	item.m_Option2 = ItemLuck;

	item.m_SetOption = ItemSet;

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, 0, 1, 0, item.m_Option2, 0, -1, item.m_NewOption, item.m_SetOption, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus, 0);

	gLog.Output(LOG_CHAOS_MIX, "[DarkAngelMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::AncientSoulMix(LPOBJ lpObj)
{
	int Item = 0;
	int solt;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_SetOption != 0 && lpObj->ChaosBox[n].m_Level >= 11)
		{
			Item++;
			solt = n;
		}
	}

	if (Item != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	if (lpObj->ChaosBox[solt].m_Level < 11)
	{
		this->GCChaosMixSend(lpObj->Index, 8, 0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo.m_BloodCastleMixRate7[lpObj->AccountLevel];

	if (this->GetTalismanOfLuckRate(lpObj, &lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index, 240, 0);
		return;
	}

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate > 100) ? 100 : lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) <= lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(13, 407), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
		gLog.Output(LOG_CHAOS_MIX, "[AncientSoulMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);
		this->GCChaosBoxSend(lpObj, 0);
		this->GCChaosMixSend(lpObj->Index, 0, 0);
		gLog.Output(LOG_CHAOS_MIX, "[AncientSoulMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
	}
}

void CChaosBox::BloodItemMix(LPOBJ lpObj)
{
	int ItemCount = 0;
	int ItemSlot = 0;
	int ItemSoul = 0;
	int HeroesItemCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 407))
		{
			ItemSoul++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 399))
		{
			HeroesItemCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index >= GET_ITEM(13, 351) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(13, 402))
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if (ItemCount != 1 || (ItemSoul != 1 && HeroesItemCount != 1))
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	CItem item = lpObj->ChaosBox[ItemSlot];

	if ((GetLargeRand() % 100) <= gServerInfo.m_AngelSetOptionRate)
	{
		item.m_SetOption = 5;
	}
	else
	{
		item.m_SetOption = 9;
	}

	item.m_Index = gRheaGold.BloodItemMixList(lpObj->ChaosBox[ItemSlot].m_Index);

	item.m_NewOption = 0;

	if (gServerInfo.m_AngelEcxAddSwitch)
	{
		item.m_NewOption = gRheaGold.GetItemNewOption(gServerInfo.m_AngelEcxMaxOption, 1);
	}

	if (HeroesItemCount == 1)
	{
		item.m_Level = 11;
	}

	item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());

	this->ChaosBoxInit(lpObj);

	gItemManager.ChaosBoxAddItem(lpObj->Index, item, 0);

	this->GCChaosMixSend(lpObj->Index, 1, &item);

	gLog.Output(LOG_CHAOS_MIX, "[BloodItemMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::DiabloRingMix(LPOBJ lpObj)
{
	//Disabled for IconeMU
	// return;

	int ChaosCount = 0;
	int SoulPack10 = 0;
	int BlessPack10 = 0;
	int LifePack10 = 0;
	int Item = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulPack10++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 30) && lpObj->ChaosBox[n].m_Level == 0)
		{
			BlessPack10++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 136) && lpObj->ChaosBox[n].m_Level == 0)
		{
			LifePack10++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 277))
		{
			Item++;
		}
	}
	if (ChaosCount != 1 || SoulPack10 != 1 || BlessPack10 != 1 || LifePack10 != 1 || Item != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	WORD ItemIndex = GET_ITEM(13, 268);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 255, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[DiabloRingMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::ConquerorWingMix(LPOBJ lpObj)
{
	//Disabled for IconeMU
	// return;

	int ChaosCount = 0;
	int SoulPack10 = 0;
	int BlessPack10 = 0;
	int LifePack10 = 0;
	int Item = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulPack10++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 30) && lpObj->ChaosBox[n].m_Level == 0)
		{
			BlessPack10++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 136) && lpObj->ChaosBox[n].m_Level == 0)
		{
			LifePack10++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 276))
		{
			Item++;
		}
	}

	if (ChaosCount != 1 || SoulPack10 != 1 || BlessPack10 != 1 || LifePack10 != 1 || Item != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	WORD ItemIndex = GET_ITEM(12, 268);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 255, 0, 0, 0, -1, 63, 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[ConquerorWingMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::CGChaosMixCloseRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_CHAOS_BOX)
	{
		return;
	}

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;

	this->ChaosBoxInit(lpObj);

	gObjInventoryCommit(aIndex);

	lpObj->ChaosLock = 0;

	lpObj->IsChaosMixCompleted = 0;

	lpObj->IsCastleNPCUpgradeCompleted = 0;

	PBMSG_HEAD pMsg;

	pMsg.set(0x87, sizeof(pMsg));

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.size);
}

void CChaosBox::GCChaosBoxSend(LPOBJ lpObj, BYTE type) // OK
{
	if ((type == 0 && (lpObj->Interface.type != INTERFACE_CHAOS_BOX || lpObj->Interface.state == 1)) || (type != 0 && (lpObj->Interface.type != INTERFACE_TRAINER || lpObj->Interface.state == 1)))
	{
		return;
	}

	BYTE send[2048];

	PMSG_SHOP_ITEM_LIST_SEND pMsg;

	pMsg.header.set(0x31, 0);

	int size = sizeof(pMsg);

	pMsg.type = ((type == 0) ? 3 : 5);

	pMsg.count = 0;

	PMSG_SHOP_ITEM_LIST info;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		info.slot = n;

		gItemManager.ItemByteConvert(info.ItemInfo, lpObj->ChaosBox[n]);

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(lpObj->Index, send, size);
}

void CChaosBox::GCChaosMixSend(int aIndex, BYTE result, CItem* lpItem) // OK
{
	PMSG_CHAOS_MIX_SEND pMsg;

	LPOBJ lpObj = &gObj[aIndex];

	pMsg.header.set(0x86, sizeof(pMsg));

	pMsg.result = result;

	if (lpItem == 0)
	{
		memset(pMsg.ItemInfo, 0xFF, sizeof(pMsg.ItemInfo));
	}
	else
	{
		gItemManager.ItemByteConvert(pMsg.ItemInfo, (*lpItem));
		pMsg.Pos = 0;
	}

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

	gObj[aIndex].ChaosLock = 0;
}

void CChaosBox::MGItemChange(LPOBJ lpObj, int type)
{
	//0 MG  Str->ene;
	//1 MG  Ene->Str;
	int ChaosCount = 0;
	int SoulCount = 0;
	int BlessCount = 0;
	int ItemChange = 0;
	int Item = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 396) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 397))
		{
			ItemChange++;
		}
		else if ((lpObj->ChaosBox[n].m_Index == GET_ITEM(8, 102) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(9, 102) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(10, 102) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(11, 102) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(8, 161) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(9, 161) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(10, 161) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(11, 161)) && lpObj->ChaosBox[n].IsSetItem() != 0)
		{
			Item++;
			ItemSlot = n;
		}
	}

	if (ChaosCount != 1 || SoulCount != 10 || BlessCount != 10 || ItemChange != 1 || Item != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	CItem item = lpObj->ChaosBox[ItemSlot];

	item.m_Index = ((type == 0) ? GET_ITEM(ITEM_GET_TYPE(item.m_Index), (ITEM_GET_INDEX(item.m_Index) + 59)) : GET_ITEM(ITEM_GET_TYPE(item.m_Index), (ITEM_GET_INDEX(item.m_Index) - 59)));

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, (BYTE)item.m_Level, item.m_Durability, item.m_Option1, item.m_Option2, item.m_Option3, -1, item.m_NewOption, item.m_SetOption, item.m_JewelOfHarmonyOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, 0);
}

void CChaosBox::FEItemChange(LPOBJ lpObj, int type)
{
	//0 Elf  Ene->Agi;
	//1 Elf  Agi->Ene;
	int ChaosCount = 0;
	int SoulCount = 0;
	int BlessCount = 0;
	int EneChangeAgi = 0;
	int Item = 0;
	int ItemSlot = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 14))
		{
			SoulCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 13))
		{
			BlessCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 394) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14, 395))
		{
			EneChangeAgi++;
		}
		else if ((lpObj->ChaosBox[n].m_Index == GET_ITEM(7, 158) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(8, 158) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(9, 158) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(10, 158) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(11, 158) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(7, 100) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(8, 100) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(9, 100) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(10, 100) ||
			lpObj->ChaosBox[n].m_Index == GET_ITEM(11, 100)) && lpObj->ChaosBox[n].IsSetItem() != 0)
		{
			Item++;
			ItemSlot = n;
		}
	}
	if (ChaosCount != 1 || SoulCount != 10 || BlessCount != 10 || EneChangeAgi != 1 || Item != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	CItem item = lpObj->ChaosBox[ItemSlot];

	item.m_Index = ((type == 0) ? GET_ITEM(ITEM_GET_TYPE(item.m_Index), (ITEM_GET_INDEX(item.m_Index) - 58)) : GET_ITEM(ITEM_GET_TYPE(item.m_Index), (ITEM_GET_INDEX(item.m_Index) + 58)));

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, item.m_Index, (BYTE)item.m_Level, item.m_Durability, item.m_Option1, item.m_Option2, item.m_Option3, -1, item.m_NewOption, item.m_SetOption, item.m_JewelOfHarmonyOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, 0);
}

void CChaosBox::MagicStonemMix(LPOBJ lpObj, BYTE count)
{
	int MagicStone[3] = { 0 };
	int ItemValue = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (lpObj->ChaosBox[i].IsItem() == TRUE)
		{
			lpObj->ChaosBox[i].Value();
			ItemValue = lpObj->ChaosBox[i].m_SellMoney / 1000;

			//Higher
			if (lpObj->ChaosBox[i].IsSetItem() == 1 && !lpObj->ChaosBox[i].IsExcItem() && lpObj->ChaosBox[i].m_Level >= 7 && lpObj->ChaosBox[i].m_Option3 >= 1)
			{
				if (ItemValue > 300 && ItemValue < 800)
				{
					MagicStone[2] += 1 + (GetLargeRand() % 2);
				}
				else if (ItemValue >= 800 && ItemValue < 1500)
				{
					MagicStone[2] += 1 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 1500 && ItemValue < 2000)
				{
					MagicStone[2] += 2 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 2000)
				{
					MagicStone[2] += 5 + (GetLargeRand() % 2);
				}
			}
			//Medium
			else if (lpObj->ChaosBox[i].IsExcItem() == 1 && lpObj->ChaosBox[i].m_Level >= 7 && lpObj->ChaosBox[i].m_Option3 >= 1)
			{
				if (ItemValue > 0 && ItemValue < 7500)
				{
					MagicStone[1] += 1 + (GetLargeRand() % 2);
				}
				else if (ItemValue >= 7500 && ItemValue < 13000)
				{
					MagicStone[1] += 1 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 13000 && ItemValue < 20000)
				{
					MagicStone[1] += 2 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 20000 && ItemValue < 25000)
				{
					MagicStone[1] += 3 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 25000 && ItemValue < 35000)
				{
					MagicStone[1] += 5 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 35000 && ItemValue < 45000)
				{
					MagicStone[1] += 6 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 45000)
				{
					MagicStone[1] += 8 + (GetLargeRand() % 2);
				}
			}
			//Lower
			else if (lpObj->ChaosBox[i].m_Level >= 6 && lpObj->ChaosBox[i].m_Option3 >= 1)
			{
				if (ItemValue > 0 && ItemValue < 100)
				{
					MagicStone[0] += (GetLargeRand() % 2);
				}
				else if (ItemValue >= 100 && ItemValue < 150)
				{
					MagicStone[0] += 2 + (GetLargeRand() % 2);
				}
				else if (ItemValue >= 150 && ItemValue < 200)
				{
					MagicStone[0] += 3 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 200 && ItemValue < 300)
				{
					MagicStone[0] += 5 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 300 && ItemValue < 400)
				{
					MagicStone[0] += 7 + (GetLargeRand() % 3);
				}
				else if (ItemValue >= 400)
				{
					MagicStone[0] += 8 + (GetLargeRand() % 3);
				}
			}
		}
	}

	if (MagicStone[0] < 1 && MagicStone[1] < 1 && MagicStone[2] < 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = count * 50000;

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->ChaosMassMixCurrItem = 0;

	this->ChaosBoxInit(lpObj);
	this->GCChaosBoxSend(lpObj, 0);

	for (int i = 0; i < 3; i++)
	{
		if (MagicStone[i] > 0)
		{
			GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, GET_ITEM(12, (404 + i)), 0, MagicStone[i], 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
			gLog.Output(LOG_CHAOS_MIX, "[MagicStonemMix(%d)][Success][%s][%s] - (Quantity: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", (i+1), lpObj->Account, lpObj->Name, MagicStone[i], lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
		}
	}
}

void CChaosBox::BoundWings2ndMix(LPOBJ lpObj)
{
	int WingItemCount = 0;
	int ItemCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 408))
		{
			ItemCount++;
		}
		else if ((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 0) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 2)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 41))
		{
			WingItemCount++;
		}
	}

	if (ItemCount != 1 || WingItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	lpObj->ChaosMoney = 100000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);
		
	WORD ItemIndex = GET_ITEM(12, 422) + lpObj->Class;

	BYTE ItemOption2 = 0;
	BYTE ItemOption3 = 0;
	BYTE ItemNewOption = 0;

	gItemOptionRate.GetItemOption2(4, &ItemOption2);
	gItemOptionRate.GetItemOption3(4, &ItemOption3);
	gItemOptionRate.GetItemOption4(4, &ItemNewOption);
	gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + (32 * (GetLargeRand() % 2))), 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[BoundWing2ndMix][Success][%s][%s] - (Class: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->Class, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}

void CChaosBox::BoundWings3rdMix(LPOBJ lpObj)
{
	int WingItemCount = 0;
	int ItemCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 410))
		{
			ItemCount++;
		}
		else if (lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 269) || 
			(lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 3) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 6)) || 
			lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 42) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12, 49) || lpObj->ChaosBox[n].m_Index == GET_ITEM(13, 30) || 
			(lpObj->ChaosBox[n].m_Index >= GET_ITEM(12, 422) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12, 429)))
		{
			WingItemCount++;
		}
	}

	if (ItemCount != 1 || WingItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosSuccessRate = 100;

	lpObj->ChaosMoney = 25000000;

	int TaxMoney = (lpObj->ChaosMoney * gCastleSiegeSync.GetTaxRateChaos(lpObj->Index)) / 100;

	lpObj->ChaosMoney += TaxMoney;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gCastleSiegeSync.AddTributeMoney(TaxMoney);

	WORD ItemIndex = GET_ITEM(12, 430) + lpObj->Class;

	BYTE ItemOption2 = 0;
	BYTE ItemOption3 = 0;
	BYTE ItemNewOption = 0;

	gItemOptionRate.GetItemOption2(4, &ItemOption2);
	gItemOptionRate.GetItemOption3(4, &ItemOption3);
	gItemOptionRate.GetItemOption4(4, &ItemNewOption);
	gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

	GDCreateItemSend(lpObj->Index, 0xFF, 0, 0, ItemIndex, 0, 0, 0, ItemOption2, ItemOption3, -1, (ItemNewOption + (32 * (GetLargeRand() % 2))), 0, 0, 0, 0, 0xFF, 0);

	gLog.Output(LOG_CHAOS_MIX, "[BoundWing3rdMix][Success][%s][%s] - (Class: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", lpObj->Account, lpObj->Name, lpObj->Class, lpObj->ChaosSuccessRate, lpObj->ChaosMoney);
}
