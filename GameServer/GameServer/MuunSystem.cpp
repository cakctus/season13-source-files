#include "stdafx.h"
#include "MuunSystem.h"
#include "DSProtocol.h"
#include "Duel.h"
#include "GameMain.h"
#include "ItemManager.h"
#include "ItemMove.h"
#include "Map.h"
#include "MemScript.h"
#include "ObjectManager.h"
#include "RandomManager.h"
#include "ServerInfo.h"
#include "ShopManager.h"
#include "Util.h"
#include "SXIDecryp.h"
#include "mmItemSlot.h"
CMuunSystem gMuunSystem;

CMuunSystem::CMuunSystem() // OK
{
	this->m_MuunSystemInfo.clear();
	this->m_MuunSystemOptionInfo.clear();
	this->m_MuunSystemConvertInfo.clear();
}

CMuunSystem::~CMuunSystem() // OK
{
}

void CMuunSystem::Load(char* path) // OK
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

	this->m_MuunSystemInfo.clear();
	this->m_MuunSystemOptionInfo.clear();
	this->m_MuunSystemConvertInfo.clear();

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

					MUUN_SYSTEM_INFO info;

					info.Index = lpMemScript->GetNumber();
					info.Type = lpMemScript->GetAsNumber();
					info.Rank = lpMemScript->GetAsNumber();
					info.OptionIndex = lpMemScript->GetAsNumber();
					info.EvolutionItemIndex = lpMemScript->GetAsNumber();

					this->m_MuunSystemInfo.insert(std::pair<int, MUUN_SYSTEM_INFO>(info.Index, info));
				}
				else if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					MUUN_SYSTEM_OPTION_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.OptionValue[0] = lpMemScript->GetAsNumber();
					info.OptionValue[1] = lpMemScript->GetAsNumber();
					info.OptionValue[2] = lpMemScript->GetAsNumber();
					info.OptionValue[3] = lpMemScript->GetAsNumber();
					info.OptionValue[4] = lpMemScript->GetAsNumber();
					info.MaxOptionValue = lpMemScript->GetAsNumber();
					info.SpecialOptionIndex = lpMemScript->GetAsNumber();
					info.SpecialOptionValue = lpMemScript->GetAsNumber();
					info.MapZone = lpMemScript->GetAsNumber();
					info.PlayTime = lpMemScript->GetAsNumber();
					info.DayOfWeek = lpMemScript->GetAsNumber();
					info.MinHour = lpMemScript->GetAsNumber();
					info.MaxHour = lpMemScript->GetAsNumber();
					info.MinLevel = lpMemScript->GetAsNumber();
					info.MaxLevel = lpMemScript->GetAsNumber();
					info.MinMasterLevel = lpMemScript->GetAsNumber();
					info.MaxMasterLevel = lpMemScript->GetAsNumber();

					this->m_MuunSystemOptionInfo.insert(std::pair<int, MUUN_SYSTEM_OPTION_INFO>(info.Index, info));
				}
				else if (section == 2)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					MUUN_SYSTEM_CONVERT_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.Convert = lpMemScript->GetAsNumber();

					info.ItemIndex = lpMemScript->GetAsNumber();

					info.Quantity = lpMemScript->GetAsNumber();

					this->m_MuunSystemConvertInfo.insert(std::pair<int, MUUN_SYSTEM_CONVERT_INFO>(info.Index, info));
				}
				else
				{
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

void CMuunSystem::MainProc() // OK
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[n];

		for (int n = 0; n < MUUN_INVENTORY_WEAR_SIZE; n++)
		{
			if (lpObj->MuunInventory[n].IsItem() == 0 || lpObj->MuunInventory[n].IsMuunItem() == 0 || lpObj->MuunInventory[n].m_Durability == 0)
			{
				continue;
			}

			MUUN_SYSTEM_INFO MuunSystemInfo;

			if (this->GetMuunSystemInfo(lpObj->MuunInventory[n].m_Index, &MuunSystemInfo) != 0)
			{
				MUUN_SYSTEM_OPTION_INFO MuunSystemOptionInfo;

				if (this->GetMuunSystemOptionInfo(MuunSystemInfo.OptionIndex, &MuunSystemOptionInfo) != 0)
				{
					if (this->CheckSpecialOption(lpObj, &MuunSystemOptionInfo) == 0)
					{
						if (lpObj->MuunItemStatus[n] != 0)
						{
							gObjectManager.CharacterCalcAttribute(lpObj->Index);
							this->GCMuunItemStatusSend(lpObj->Index, n, 0);
							lpObj->MuunItemStatus[n] = 0;
						}
					}
					else
					{
						if (lpObj->MuunItemStatus[n] == 0)
						{
							gObjectManager.CharacterCalcAttribute(lpObj->Index);
							this->GCMuunItemStatusSend(lpObj->Index, n, 1);
							lpObj->MuunItemStatus[n] = 1;
						}
					}
				}
			}
		}
	}
}

bool CMuunSystem::IsMuunItem(int ItemIndex) // OK
{
	std::map<int, MUUN_SYSTEM_INFO>::iterator it = this->m_MuunSystemInfo.find(ItemIndex);

	if (it == this->m_MuunSystemInfo.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool CMuunSystem::IsMuunItem(CItem* lpItem) // OK
{
	std::map<int, MUUN_SYSTEM_INFO>::iterator it = this->m_MuunSystemInfo.find(lpItem->m_Index);

	if (it == this->m_MuunSystemInfo.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool CMuunSystem::IsMuunUtil(int ItemIndex) // OK
{
	if (gGItemSlot.GetItemBySlot(ItemIndex, 2))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CMuunSystem::IsMuunUtil(CItem* lpItem) // OK
{
	if (lpItem->m_Index == GET_ITEM(13, 211) || lpItem->m_Index == GET_ITEM(13, 239) || lpItem->m_Index == GET_ITEM(13, 240) ||
		lpItem->m_Index == GET_ITEM(16, 211) || lpItem->m_Index == GET_ITEM(16, 239) || lpItem->m_Index == GET_ITEM(16, 240))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CMuunSystem::GetMuunSystemInfo(int index, MUUN_SYSTEM_INFO* lpInfo) // OK
{
	std::map<int, MUUN_SYSTEM_INFO>::iterator it = this->m_MuunSystemInfo.find(index);

	if (it == this->m_MuunSystemInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

bool CMuunSystem::GetMuunSystemOptionInfo(int index, MUUN_SYSTEM_OPTION_INFO* lpInfo) // OK
{
	std::map<int, MUUN_SYSTEM_OPTION_INFO>::iterator it = this->m_MuunSystemOptionInfo.find(index);

	if (it == this->m_MuunSystemOptionInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

bool CMuunSystem::GetMuunSystemConvertInfo(int index, MUUN_SYSTEM_CONVERT_INFO* lpInfo) // OK
{
	std::map<int, MUUN_SYSTEM_CONVERT_INFO>::iterator it = this->m_MuunSystemConvertInfo.find(index);

	if (it == this->m_MuunSystemConvertInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}
void CMuunSystem::MuunInventoryItemSet(int aIndex, int slot, BYTE type) // OK
{
	if (MUUN_INVENTORY_BASE_RANGE(slot) == 0)
	{
		return;
	}

	ITEM_INFO ItemInfo;

	if (gItemManager.GetInfo(gObj[aIndex].MuunInventory[slot].m_Index, &ItemInfo) == 0)
	{
		return;
	}

	int x = (slot - MUUN_INVENTORY_WEAR_SIZE) % 4;
	int y = (slot - MUUN_INVENTORY_WEAR_SIZE) / 4;

	if ((x + ItemInfo.Width) > 4 || (y + ItemInfo.Height) > 25)
	{
		return;
	}

	for (int sy = 0; sy < ItemInfo.Height; sy++)
	{
		for (int sx = 0; sx < ItemInfo.Width; sx++)
		{
			gObj[aIndex].MuunInventoryMap[(((sy + y) * 4) + (sx + x))] = type;
		}
	}
}

BYTE CMuunSystem::MuunInventoryRectCheck(int aIndex, int x, int y, int width, int height) // OK
{
	if ((x + width) > 4 || (y + height) > 25)
	{
		return 0xFF;
	}

	for (int sy = 0; sy < height; sy++)
	{
		for (int sx = 0; sx < width; sx++)
		{
			if (gObj[aIndex].MuunInventoryMap[(((sy + y) * 4) + (sx + x))] != 0xFF)
			{
				return 0xFF;
			}
		}
	}

	return (((y * 4) + x) + MUUN_INVENTORY_WEAR_SIZE);
}

BYTE CMuunSystem::MuunInventoryInsertItem(int aIndex, CItem item) // OK
{
	ITEM_INFO ItemInfo;

	if (gItemManager.GetInfo(item.m_Index, &ItemInfo) == 0)
	{
		return 0xFF;
	}

	for (int y = 0; y < 25; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (gObj[aIndex].MuunInventoryMap[((y * 4) + x)] == 0xFF)
			{
				BYTE slot = this->MuunInventoryRectCheck(aIndex, x, y, ItemInfo.Width, ItemInfo.Height);

				if (slot != 0xFF)
				{
					gObj[aIndex].MuunInventory[slot] = item;
					this->MuunInventoryItemSet(aIndex, slot, 1);
					return slot;
				}
			}
		}
	}

	return 0xFF;
}

BYTE CMuunSystem::MuunInventoryAddItem(int aIndex, CItem item, int slot) // OK
{
	if (MUUN_INVENTORY_RANGE(slot) == 0)
	{
		return 0xFF;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->MuunInventory[slot].IsItem() != 0 || item.IsItem() == 0)
	{
		return 0xFF;
	}

	if (MUUN_INVENTORY_WEAR_RANGE(slot) == 0)
	{
		ITEM_INFO ItemInfo;

		if (gItemManager.GetInfo(item.m_Index, &ItemInfo) == 0)
		{
			return 0xFF;
		}

		int x = (slot - MUUN_INVENTORY_WEAR_SIZE) % 4;
		int y = (slot - MUUN_INVENTORY_WEAR_SIZE) / 4;

		if (this->MuunInventoryRectCheck(aIndex, x, y, ItemInfo.Width, ItemInfo.Height) == 0xFF)
		{
			return 0xFF;
		}

		lpObj->MuunInventory[slot] = item;

		this->MuunInventoryItemSet(aIndex, slot, 1);

		return slot;
	}
	else
	{
		lpObj->MuunInventory[slot] = item;
		return slot;
	}
}

void CMuunSystem::MuunInventoryDelItem(int aIndex, int slot) // OK
{
	if (MUUN_INVENTORY_WEAR_RANGE(slot) == 0)
	{
		this->MuunInventoryItemSet(aIndex, slot, 0xFF);
		gObj[aIndex].MuunInventory[slot].Clear();
	}
	else
	{
		gObj[aIndex].MuunInventory[slot].Clear();
	}
}

bool CMuunSystem::CheckSpecialOption(LPOBJ lpObj, MUUN_SYSTEM_OPTION_INFO* lpInfo) // OK
{
	SYSTEMTIME SystemTime;

	GetSystemTime(&SystemTime);

	if (lpInfo->MapZone != -1 && lpInfo->MapZone != lpObj->Map)
	{
		return 0;
	}

	if (lpInfo->PlayTime != -1 && ((DWORD)(lpInfo->PlayTime * 60000)) > (GetTickCount() - lpObj->ServerTickCount))
	{
		return 0;
	}

	if (lpInfo->DayOfWeek != -1 && (lpInfo->DayOfWeek & (1 << SystemTime.wDayOfWeek)) == 0)
	{
		return 0;
	}

	if (lpInfo->MinHour != -1 && lpInfo->MinHour > SystemTime.wHour)
	{
		return 0;
	}

	if (lpInfo->MaxHour != -1 && lpInfo->MaxHour < SystemTime.wHour)
	{
		return 0;
	}

	if (lpInfo->MinLevel != -1 && lpInfo->MinLevel > lpObj->Level)
	{
		return 0;
	}

	if (lpInfo->MaxLevel != -1 && lpInfo->MaxLevel < lpObj->Level)
	{
		return 0;
	}

	if (lpInfo->MinMasterLevel != -1 && lpInfo->MinMasterLevel > lpObj->MasterLevel)
	{
		return 0;
	}

	if (lpInfo->MaxMasterLevel != -1 && lpInfo->MaxMasterLevel < lpObj->MasterLevel)
	{
		return 0;
	}

	return 1;
}

bool CMuunSystem::CheckItemMoveToMuunInventory(LPOBJ lpObj, CItem* lpItem, int slot) // OK
{
	if (MUUN_INVENTORY_WEAR_RANGE(slot) == 0)
	{
		return 1;
	}

	if (lpItem->IsMuunItem() == 0)
	{
		return 0;
	}

	MUUN_SYSTEM_INFO MuunSystemInfoA;

	if (this->GetMuunSystemInfo(lpItem->m_Index, &MuunSystemInfoA) == 0)
	{
		return 0;
	}

	for (int n = 0; n < MUUN_INVENTORY_WEAR_SIZE; n++)
	{
		if (n != slot)
		{
			if (lpObj->MuunInventory[n].IsItem() != 0)
			{
				MUUN_SYSTEM_INFO MuunSystemInfoB;

				if (this->GetMuunSystemInfo(lpObj->MuunInventory[n].m_Index, &MuunSystemInfoB) == 0)
				{
					return 0;
				}

				if (lpObj->MuunInventory[n].m_Index == lpItem->m_Index || lpObj->MuunInventory[n].m_Index == MuunSystemInfoA.EvolutionItemIndex || MuunSystemInfoB.EvolutionItemIndex == lpItem->m_Index)
				{
					return 0;
				}
			}
		}
	}

	return 1;
}

void CMuunSystem::GetOptionValue(LPOBJ lpObj, CItem* lpItem, MUUN_SYSTEM_OPTION_INFO* lpInfo, int* value) // OK
{
	if (lpItem->m_Level == 0)
	{
		(*value) = lpInfo->MaxOptionValue;
	}
	else
	{
		(*value) = lpInfo->OptionValue[(lpItem->m_Level - 1)];
	}

	if (this->CheckSpecialOption(lpObj, lpInfo) != 0)
	{
		switch (lpInfo->SpecialOptionIndex)
		{
		case MUUN_SPECIAL_OPTION_MUL_VALUE:
			(*value) = (*value) * lpInfo->SpecialOptionValue;
			break;
		case MUUN_SPECIAL_OPTION_SUB_COOLDOWN:
			(*value) = lpInfo->SpecialOptionValue;
			break;
		}
	}
}

void CMuunSystem::GetMuunItemSocketOption(int ItemIndex, BYTE* ItemLevel, BYTE* SocketOption, BYTE* SocketOptionBonus) // OK
{
	if (this->IsMuunItem(ItemIndex) != 0)
	{
		MUUN_SYSTEM_INFO MuunSystemInfo;

		if (gMuunSystem.GetMuunSystemInfo(ItemIndex, &MuunSystemInfo) != 0)
		{
			(*SocketOptionBonus) = MuunSystemInfo.Rank;
		}
	}
}

void CMuunSystem::GetMuunUtilSocketOption(int ItemIndex, BYTE* ItemLevel, BYTE* SocketOption, BYTE* SocketOptionBonus) // OK
{
	if (ItemIndex == GET_ITEM(13, 211) || ItemIndex == GET_ITEM(16, 211))
	{
		if (this->IsMuunItem((GET_ITEM(13, 211) + (*ItemLevel))) == 0 || this->IsMuunItem((GET_ITEM(16, 211) + (*ItemLevel))) == 0)
		{
			int MuunItemIndex = -1;

			CRandomManager RandomManager;

			for (std::map<int, MUUN_SYSTEM_INFO>::iterator it = this->m_MuunSystemInfo.begin(); it != this->m_MuunSystemInfo.end(); it++)
			{
				if (it->second.EvolutionItemIndex != -1)
				{
					RandomManager.AddElement(it->first, 1);
				}
			}

			RandomManager.GetRandomElement(&MuunItemIndex);

			(*ItemLevel) = 0;
			SocketOption[0] = HIBYTE(MuunItemIndex);
			SocketOption[1] = MuunItemIndex % MAX_ITEM_TYPE;
		}
		else
		{
			int MuunItemIndex = 0;
			if (ItemIndex == GET_ITEM(13, 211) || ItemIndex == GET_ITEM(16, 211))
			{
				MuunItemIndex = ItemIndex + (*ItemLevel);
			}

			(*ItemLevel) = 0;
			SocketOption[0] = HIBYTE(MuunItemIndex);
			SocketOption[1] = MuunItemIndex % MAX_ITEM_TYPE;
		}
	}
}

bool CMuunSystem::CharacterUseMuunLevelUp(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (MUUN_INVENTORY_BASE_RANGE(SourceSlot) == 0)
	{
		return 0;
	}

	if (MUUN_INVENTORY_BASE_RANGE(TargetSlot) == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[SourceSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[SourceSlot].IsMuunItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].IsMuunItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[SourceSlot].m_Index != lpObj->MuunInventory[TargetSlot].m_Index)
	{
		return 0;
	}

	if (lpObj->MuunInventory[SourceSlot].m_Level != 1 || lpObj->MuunInventory[TargetSlot].m_Level == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].m_Level >= ((lpObj->MuunInventory[TargetSlot].m_SocketOptionBonus & 0x0F) + 1))
	{
		return 0;
	}

	lpObj->MuunInventory[TargetSlot].m_Level++;

	return 1;
}
bool CMuunSystem::CharacterUseConverterDur(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (MUUN_INVENTORY_BASE_RANGE(SourceSlot) == 0)
	{
		return 0;
	}

	if (MUUN_INVENTORY_BASE_RANGE(TargetSlot) == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[SourceSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].m_Index != GET_ITEM(13, 239) || lpObj->MuunInventory[TargetSlot].m_Index != GET_ITEM(16, 239))
	{
		return 0;
	}

	MUUN_SYSTEM_INFO MuunSystemInfo;

	if (lpObj->MuunInventory[SourceSlot].m_Index == GET_ITEM(13, 211) || lpObj->MuunInventory[SourceSlot].m_Index == GET_ITEM(16, 211))
	{
		lpObj->MuunInventory[TargetSlot].m_Durability += 2;
		goto loop;
	}

	if (this->GetMuunSystemInfo(lpObj->MuunInventory[SourceSlot].m_Index, &MuunSystemInfo) == 0)
	{
		return 0;
	}

	else if (MuunSystemInfo.Rank == 1)
	{
		if (lpObj->MuunInventory[SourceSlot].m_Level == 1)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 3;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 2)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 6;
		}
	}
	else if (MuunSystemInfo.Rank == 2)
	{
		if (lpObj->MuunInventory[SourceSlot].m_Level == 1)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 6;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 2)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 12;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 3)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 18;
		}
	}
	else if (MuunSystemInfo.Rank == 3)
	{
		if (lpObj->MuunInventory[SourceSlot].m_Level == 1)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 9;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 2)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 18;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 3)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 27;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 4)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 36;
		}
	}
	else if (MuunSystemInfo.Rank == 4)
	{
		if (lpObj->MuunInventory[SourceSlot].m_Level == 1)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 12;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 2)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 24;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 3)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 36;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 4)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 48;
		}
		else if (lpObj->MuunInventory[SourceSlot].m_Level == 5)
		{
			lpObj->MuunInventory[TargetSlot].m_Durability += 60;
		}
	}

loop:
	if (lpObj->MuunInventory[TargetSlot].m_Durability >= 120)
	{
		GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, GET_ITEM(16, 240), 0, 0, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, 0, 0xFF, 0);
		this->MuunInventoryDelItem(lpObj->Index, TargetSlot);
		this->GCMuunItemDeleteSend(lpObj->Index, TargetSlot, 1);
	}
	else
	{
		this->GCMuunItemDurSend(lpObj->Index, TargetSlot, (BYTE)lpObj->MuunInventory[TargetSlot].m_Durability);
	}

	return 1;
}
bool CMuunSystem::CharacterUseEvolutionStone(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (MUUN_INVENTORY_BASE_RANGE(SourceSlot) == 0)
	{
		return 0;
	}

	if (MUUN_INVENTORY_BASE_RANGE(TargetSlot) == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[SourceSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].IsMuunItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[SourceSlot].m_Index != GET_ITEM(16, 211))
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].m_Level != ((lpObj->MuunInventory[TargetSlot].m_SocketOptionBonus & 0x0F) + 1))
	{
		return 0;
	}

	if (MAKE_NUMBERW(lpObj->MuunInventory[SourceSlot].m_SocketOption[0], lpObj->MuunInventory[SourceSlot].m_SocketOption[1]) != lpObj->MuunInventory[TargetSlot].m_Index)
	{
		return 0;
	}

	MUUN_SYSTEM_INFO MuunSystemInfo;

	if (this->GetMuunSystemInfo(lpObj->MuunInventory[TargetSlot].m_Index, &MuunSystemInfo) == 0)
	{
		return 0;
	}

	if (MuunSystemInfo.EvolutionItemIndex == -1)
	{
		return 0;
	}

	GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, MuunSystemInfo.EvolutionItemIndex, 0, (BYTE)lpObj->MuunInventory[TargetSlot].m_Durability, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, 0, 0xFF, 0);

	this->MuunInventoryDelItem(lpObj->Index, TargetSlot);

	this->GCMuunItemDeleteSend(lpObj->Index, TargetSlot, 1);

	return 1;
}

bool CMuunSystem::CharacterUseJewelOfLife(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (INVENTORY_FULL_RANGE(SourceSlot) == 0)
	{
		return 0;
}

	if (MUUN_INVENTORY_BASE_RANGE(TargetSlot) == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[SourceSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].IsMuunItem() == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[SourceSlot].m_Index != GET_ITEM(14, 16))
	{
		return 0;
	}

	if (lpObj->MuunInventory[TargetSlot].m_Durability == 255)
	{
		return 0;
	}

	lpObj->MuunInventory[TargetSlot].m_Durability = 255;

	this->GCMuunItemDurSend(lpObj->Index, TargetSlot, (BYTE)lpObj->MuunInventory[TargetSlot].m_Durability);

	return 1;
}

void CMuunSystem::MuunSprite(LPOBJ lpObj, int damage) // OK
{
	if (lpObj->Type != OBJECT_USER)
	{
		return;
}

	for (int n = 0; n < MUUN_INVENTORY_WEAR_SIZE; n++)
	{
		if (lpObj->MuunInventory[n].IsItem() == 0 || lpObj->MuunInventory[n].IsMuunItem() == 0 || lpObj->MuunInventory[n].m_Durability == 0)
		{
			continue;
		}

		float DurabilityValue = ((((damage * 2.0f) / 400.0f) / 400.0f) * gServerInfo.m_MuunDurabilityRate) / 100.0f;

		if ((lpObj->MuunInventory[n].m_Durability - DurabilityValue) >= 1)
		{
			lpObj->MuunInventory[n].m_Durability = lpObj->MuunInventory[n].m_Durability - DurabilityValue;

			this->GCMuunItemDurSend(lpObj->Index, n, (BYTE)lpObj->MuunInventory[n].m_Durability);
		}
		else
		{
			lpObj->MuunInventory[n].m_Durability = 0;

			this->GCMuunItemDurSend(lpObj->Index, n, (BYTE)lpObj->MuunInventory[n].m_Durability);

			gObjectManager.CharacterCalcAttribute(lpObj->Index);
		}
	}
}

void CMuunSystem::CalcMuunOption(LPOBJ lpObj, bool flag) // OK
{
	for (int n = 0; n < MUUN_INVENTORY_WEAR_SIZE; n++)
	{
		if (lpObj->MuunInventory[n].IsItem() == 0 || lpObj->MuunInventory[n].IsMuunItem() == 0 || lpObj->MuunInventory[n].m_Durability == 0)
		{
			continue;
		}

		MUUN_SYSTEM_INFO MuunSystemInfo;

		if (this->GetMuunSystemInfo(lpObj->MuunInventory[n].m_Index, &MuunSystemInfo) != 0)
		{
			MUUN_SYSTEM_OPTION_INFO MuunSystemOptionInfo;

			if (this->GetMuunSystemOptionInfo(MuunSystemInfo.OptionIndex, &MuunSystemOptionInfo) != 0)
			{
				int value = 0;

				this->GetOptionValue(lpObj, &lpObj->MuunInventory[n], &MuunSystemOptionInfo, &value);

				this->InsertOption(lpObj, MuunSystemOptionInfo.Index, value, flag);
			}
		}
	}
}

void CMuunSystem::InsertOption(LPOBJ lpObj, int index, int value, bool flag) // OK
{
	if (flag != 0)
	{
		return;
	}

	switch (index)
	{
	case MUUN_OPTION_LUKI:
		lpObj->Defense += value;
		break;
	case MUUN_OPTION_TONY:
		lpObj->ExcellentDamage += value;
		break;
	case MUUN_OPTION_NYMPH:
		lpObj->PhysiDamageMinLeft += value;
		lpObj->PhysiDamageMinRight += value;
		lpObj->PhysiDamageMaxLeft += value;
		lpObj->PhysiDamageMaxRight += value;
		lpObj->MagicDamageMin += value;
		lpObj->MagicDamageMax += value;
		lpObj->CurseDamageMin += value;
		lpObj->CurseDamageMax += value;
		break;
	case MUUN_OPTION_SARI:
		lpObj->CriticalDamage += value;
		break;
	case MUUN_OPTION_WILLIAM:
		lpObj->PhysiDamageMinLeft += value;
		lpObj->PhysiDamageMinRight += value;
		lpObj->PhysiDamageMaxLeft += value;
		lpObj->PhysiDamageMaxRight += value;
		lpObj->MagicDamageMin += value;
		lpObj->MagicDamageMax += value;
		lpObj->CurseDamageMin += value;
		lpObj->CurseDamageMax += value;
		break;
	case MUUN_OPTION_PAUL:
		lpObj->ExcellentDamage += value;
		break;
	case MUUN_OPTION_CHIRON:
		lpObj->ExcellentDamage += value;
		break;
	case MUUN_OPTION_WOOPAROO:
		lpObj->PhysiDamageMinLeft += value;
		lpObj->PhysiDamageMinRight += value;
		lpObj->PhysiDamageMaxLeft += value;
		lpObj->PhysiDamageMaxRight += value;
		lpObj->MagicDamageMin += value;
		lpObj->MagicDamageMax += value;
		lpObj->CurseDamageMin += value;
		lpObj->CurseDamageMax += value;
		break;
	case MUUN_OPTION_TIBETTON:
		break;
	case MUUN_OPTION_WITCH:
		lpObj->PhysiDamageMinLeft += value;
		lpObj->PhysiDamageMinRight += value;
		lpObj->PhysiDamageMaxLeft += value;
		lpObj->PhysiDamageMaxRight += value;
		lpObj->MagicDamageMin += value;
		lpObj->MagicDamageMax += value;
		lpObj->CurseDamageMin += value;
		lpObj->CurseDamageMax += value;
		break;
	case MUUN_OPTION_SKULL:
		lpObj->CriticalDamage += value;
		break;
	case MUUN_OPTION_PUMPY:
		lpObj->CriticalDamage += value;
		break;
	case MUUN_OPTION_SAVATH:
		lpObj->ExcellentDamage += value;
		break;
	case MUUN_OPTION_LYCAN:
		lpObj->PhysiDamageMinLeft += value;
		lpObj->PhysiDamageMinRight += value;
		lpObj->PhysiDamageMaxLeft += value;
		lpObj->PhysiDamageMaxRight += value;
		lpObj->MagicDamageMin += value;
		lpObj->MagicDamageMax += value;
		lpObj->CurseDamageMin += value;
		lpObj->CurseDamageMax += value;
		break;
	case MUUN_OPTION_TORBY:
		lpObj->Defense += value;
		break;
	}
}

void CMuunSystem::CGMuunItemGetRecv(PMSG_MUUN_ITEM_GET_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_MUUN_ITEM_GET_SEND pMsg;

	pMsg.header.setE(0x4E, 0x00, sizeof(pMsg));

	pMsg.result = 0xFF;

	memset(pMsg.ItemInfo, 0, sizeof(pMsg.ItemInfo));

	if (lpObj->DieRegen != 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (lpObj->Interface.use != 0 && lpObj->Interface.type != INTERFACE_SHOP)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	int index = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);

	if (MAP_ITEM_RANGE(index) == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (MAP_RANGE(lpObj->Map) == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (gMap[lpObj->Map].CheckItemGive(aIndex, index) == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	CMapItem* lpItem = &gMap[lpObj->Map].m_Item[index];

	if (lpItem->IsMuunItem() == 0 && lpItem->IsMuunUtil() == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	CItem item = (*lpItem);

	if ((pMsg.result = this->MuunInventoryInsertItem(aIndex, item)) == 0xFF)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	gMap[lpObj->Map].ItemGive(aIndex, index);

	gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

	GCPartyItemInfoSend(aIndex, &item);
}

void CMuunSystem::CGMuunItemUseRecv(PMSG_MUUN_ITEM_USE_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_MUUN_ITEM_USE_SEND pMsg;

	pMsg.header.set(0x4E, 0x08, sizeof(pMsg));

	pMsg.result = 1;

	pMsg.type = lpMsg->type;

	if (lpObj->DieRegen != 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (lpObj->Interface.use != 0 && lpObj->Interface.type != INTERFACE_SHOP)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	switch (lpMsg->type)
	{
	case 1:
		if ((pMsg.result = this->CharacterUseMuunLevelUp(lpObj, lpMsg->SourceSlot, lpMsg->TargetSlot)) != 0)
		{
			this->MuunInventoryDelItem(aIndex, lpMsg->SourceSlot);
			this->GCMuunItemDeleteSend(aIndex, lpMsg->SourceSlot, 1);
			this->GCMuunItemModifySend(aIndex, lpMsg->TargetSlot);
		}
		break;
	case 2:
		if ((pMsg.result = this->CharacterUseEvolutionStone(lpObj, lpMsg->SourceSlot, lpMsg->TargetSlot)) != 0)
		{
			this->MuunInventoryDelItem(aIndex, lpMsg->SourceSlot);
			this->GCMuunItemDeleteSend(aIndex, lpMsg->SourceSlot, 1);
			this->GCMuunItemListSend(aIndex);
		}
		break;
	case 3:
		if ((pMsg.result = this->CharacterUseJewelOfLife(lpObj, lpMsg->SourceSlot, lpMsg->TargetSlot)) != 0)
		{
			gItemManager.InventoryDelItem(aIndex, lpMsg->SourceSlot);
			gItemManager.GCItemDeleteSend(aIndex, lpMsg->SourceSlot, 1);
		}
		break;
	case 4:
		if ((pMsg.result = this->CharacterUseConverterDur(lpObj, lpMsg->SourceSlot, lpMsg->TargetSlot)) != 0)
		{
			this->MuunInventoryDelItem(aIndex, lpMsg->SourceSlot);
			this->GCMuunItemDeleteSend(aIndex, lpMsg->SourceSlot, 1);
			this->GCMuunItemModifySend(aIndex, lpMsg->TargetSlot);
		}
		break;
	default:
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	pMsg.result = ((pMsg.result == 0) ? 1 : 0);

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CMuunSystem::CGMuunItemSellRecv(PMSG_MUUN_ITEM_SELL_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_MUUN_ITEM_SELL_SEND pMsg;

	pMsg.header.setE(0x4E, 0x09, sizeof(pMsg));

	pMsg.result = 0;

	pMsg.money = 0;

	if (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_SHOP)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (SHOP_RANGE(lpObj->TargetShopNumber) == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (MUUN_INVENTORY_RANGE(lpMsg->slot) == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (lpObj->btRestoreInterface == 1)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	CItem* lpItem = &lpObj->MuunInventory[lpMsg->slot];

	if (lpItem->IsItem() == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (gItemMove.CheckItemMoveAllowSell(lpItem->m_Index) == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (lpItem->m_Index == GET_ITEM(13, 4) || lpItem->m_Index == GET_ITEM(13, 5)) // Dark Horse,Dark Reaven
	{
		lpItem->PetValue();
	}
	else
	{
		lpItem->Value();
	}

	if (gObjCheckMaxMoney(aIndex, lpItem->m_SellMoney) == 0)
	{
		lpObj->Money = MAX_MONEY;
	}
	else
	{
		lpObj->Money += lpItem->m_SellMoney;
	}

	this->MuunInventoryDelItem(aIndex, lpMsg->slot);

	if (lpMsg->slot == 0)
	{
		lpObj->MuunInventory[0].m_Index = -1;
	}

	else if (lpMsg->slot == 1)
	{
		lpObj->m_wMuunSubItem = -1;
	}

	pMsg.result = 1;

	pMsg.money = lpObj->Money;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

	if (MUUN_INVENTORY_WEAR_RANGE(lpMsg->slot) != 0)
	{
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		this->GCMuunItemChangeSend(lpObj->Index, lpMsg->slot);
		this->GCMuunItemStatusSend(lpObj->Index, lpMsg->slot, 0);
		lpObj->MuunItemStatus[lpMsg->slot] = 0;
	}
}

void CMuunSystem::CGReqRideSelect(PMSG_MUUN_RIDE_SELECT* lpMsg, int aIndex)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
		return;

	LPOBJ lpObj = &gObj[aIndex];

	lpObj->m_wMuunRideItem = -1;
	int num;
	for (int i = 0; i < 2; i++)
	{
		int nItemNum = lpObj->MuunInventory[i].m_Index;

		if (nItemNum == lpMsg->wItemNum)
		{
			lpObj->m_wMuunRideItem = nItemNum;
			num = i;
			break;
		}
	}

	_tagMuunRideViewPortInfo MuunViewPortInfo;
	BYTE btMuunInfosendBuf[2048];

	MuunViewPortInfo.NumberH = SET_NUMBERHB(lpObj->Index);
	MuunViewPortInfo.NumberL = SET_NUMBERLB(lpObj->Index);
	MuunViewPortInfo.MuunRideItemH = SET_NUMBERHB(lpObj->m_wMuunRideItem);
	MuunViewPortInfo.MuunRideItemL = SET_NUMBERLB(lpObj->m_wMuunRideItem);

	if (lpObj->m_wMuunRideItem == 0xFFFF)
	{
		MuunViewPortInfo.MuunRideItemH = SET_NUMBERHB(lpObj->m_wInvenPet);
		MuunViewPortInfo.MuunRideItemL = SET_NUMBERLB(lpObj->m_wInvenPet);
	}

	PMSG_SEND_MUUN_RIDE_VIEWPORT_INFO pMsgMuun;

	int lOfs = sizeof(pMsgMuun);

	memcpy(&btMuunInfosendBuf[lOfs], &MuunViewPortInfo, 4);
	lOfs += sizeof(_tagMuunRideViewPortInfo);

	pMsgMuun.Count = 1;
	pMsgMuun.h.set(0x4E, 0x14, lOfs);
	memcpy(btMuunInfosendBuf, &pMsgMuun, sizeof(pMsgMuun));

	DataSend(lpObj->Index, btMuunInfosendBuf, lOfs);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != 0)
		{
			int tObjNum = lpObj->VpPlayer2[n].index;

			if (tObjNum >= 0 && lpObj->VpPlayer2[n].type == OBJECT_USER)
			{
				DataSend(tObjNum, btMuunInfosendBuf, lOfs);
			}
		}
	}
}

void CMuunSystem::CGSetMuunDisplayRecv(PMSG_MUUN_SET_DISPLAY_RECV* lpMsg, int aIndex)
{
	gObj[aIndex].MuunDisplaySlot = lpMsg->slot;
}

void CMuunSystem::CGMuunConvertRecv(PMSG_MUUN_ITEM_CONVERT_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	PMSG_MUUN_ITEM_CONVERT_SEND pMsg;

	pMsg.header.setE(0x4E, 0x13, sizeof(pMsg));
	pMsg.result = 1;
	pMsg.money = 0;
	int i = 0;
	BYTE Record[50] = { 0 };
	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	for (int n = 0; n < 102; n++)
	{
		if (lpObj->MuunInventory[n].m_Index == GET_ITEM(13, 240) || lpObj->MuunInventory[n].m_Index == GET_ITEM(16, 240))
		{
			Record[i] = n;
			i++;
		}
	}

	if (i <= this->m_MuunSystemConvertInfo[lpMsg->slot].Quantity)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (i >= this->m_MuunSystemConvertInfo[lpMsg->slot].Quantity)
	{
		for (int num = 0; num <= this->m_MuunSystemConvertInfo[lpMsg->slot].Quantity; num++)
		{
			this->MuunInventoryDelItem(lpObj->Index, Record[num]);
			this->GCMuunItemDeleteSend(lpObj->Index, Record[num], 1);
		}
	}

	BYTE ItemIndex[5];
	memset(&ItemIndex, 0xFF, sizeof(ItemIndex));
	ItemIndex[0] = (this->m_MuunSystemConvertInfo[lpMsg->slot].ItemIndex >> 8) & 0xFF;
	ItemIndex[1] = this->m_MuunSystemConvertInfo[lpMsg->slot].ItemIndex & 0xFF;

	GDCreateConvertItemSend(lpObj->Index, 0xEB, 0, 0, GET_ITEM(16, 211), 0, 0, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, ItemIndex, 0xFF, 0);

	pMsg.result = 0;

	pMsg.money = 0;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CMuunSystem::GCMuunItemListSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[4096];

	PMSG_MUUN_ITEM_LIST_SEND pMsg;

	pMsg.header.setE(0x4E, 0x02, 0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_MUUN_ITEM_LIST info;

	for (int n = 0; n < MUUN_INVENTORY_SIZE; n++)
	{
		if (lpObj->MuunInventory[n].IsItem() != 0 && lpObj->MuunInventory[n].m_ItemExist != 0)
		{
			info.slot = n;

			gItemManager.ItemByteConvert(info.ItemInfo, lpObj->MuunInventory[n]);

			memcpy(&send[size], &info, sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(aIndex, send, size);
}

void CMuunSystem::GCMuunItemDurSend(int aIndex, BYTE slot, BYTE dur) // OK
{
	PMSG_MUUN_ITEM_DUR_SEND pMsg;

	pMsg.header.set(0x4E, 0x03, sizeof(pMsg));

	pMsg.slot = slot;

	pMsg.dur = dur;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CMuunSystem::GCMuunItemDeleteSend(int aIndex, BYTE slot, BYTE flag) // OK
{
	PMSG_MUUN_ITEM_DELETE_SEND pMsg;

	pMsg.header.set(0x4E, 0x04, sizeof(pMsg));

	pMsg.slot = slot;

	pMsg.flag = flag;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CMuunSystem::GCMuunItemModifySend(int aIndex, BYTE slot) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->MuunInventory[slot].IsItem() == 0)
	{
		return;
	}

	PMSG_MUUN_ITEM_MODIFY_SEND pMsg;

	pMsg.header.set(0x4E, 0x05, sizeof(pMsg));

	pMsg.flag = 0;

	pMsg.slot = slot;

	gItemManager.ItemByteConvert(pMsg.ItemInfo, lpObj->MuunInventory[slot]);

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CMuunSystem::GCMuunItemChangeSend(int aIndex, BYTE slot) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_MUUN_ITEM_CHANGE_SEND pMsg;

	pMsg.header.set(0x4E, 0x06, sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);

	gItemManager.ItemByteConvert(pMsg.ItemInfo, lpObj->MuunInventory[slot]);

	pMsg.ItemInfo[1] = slot * 16;
	pMsg.ItemInfo[1] |= ((lpObj->MuunInventory[slot].m_Level - 1) / 2) & 0x0F;

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}

void CMuunSystem::GCMuunItemStatusSend(int aIndex, BYTE slot, BYTE status) // OK
{
	PMSG_MUUN_ITEM_STATUS_SEND pMsg;

	pMsg.header.set(0x4E, 0x07, sizeof(pMsg));

	pMsg.slot = slot;

	pMsg.status = status;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CMuunSystem::DGMuunInventoryRecv(SDHP_MUUN_INVENTORY_RECV* lpMsg) // OK
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGMuunInventoryRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if (lpObj->LoadMuunInventory != 0)
	{
		return;
	}

	lpObj->LoadMuunInventory = 1;

	memset(lpObj->MuunInventory, 0xFF, MUUN_INVENTORY_SIZE);

	for (int n = 0; n < MUUN_INVENTORY_SIZE; n++)
	{
		CItem item;

		lpObj->MuunInventory[n].Clear();

		if (gItemManager.ConvertItemByte(&item, lpMsg->MuunInventory[n]) != 0)
		{
			if (item.m_Index >= GET_ITEM(16, 0))
			{
				if (item.m_Level == 0)
				{
					item.m_Level = 1;
				}

				this->MuunInventoryAddItem(lpObj->Index, item, n);
			}
		}
	}

	this->GCMuunItemListSend(lpObj->Index);
}

void CMuunSystem::GDMuunInventorySend(int aIndex) // OK
{
	if (gObjIsAccountValid(aIndex, gObj[aIndex].Account) == 0)
	{
		return;
	}

	if (gObj[aIndex].LoadMuunInventory != 0)
	{
		return;
	}

	SDHP_MUUN_INVENTORY_SEND pMsg;

	pMsg.header.set(0x27, 0x00, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, gObj[aIndex].Account, sizeof(pMsg.account));

	memcpy(pMsg.name, gObj[aIndex].Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CMuunSystem::GDMuunInventorySaveSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->LoadMuunInventory == 0)
	{
		return;
	}

	SDHP_MUUN_INVENTORY_SAVE_SEND pMsg;

	pMsg.header.set(0x27, 0x30, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	for (int n = 0; n < MUUN_INVENTORY_SIZE; n++)
	{ 
		gItemManager.DBItemByteConvert(pMsg.MuunInventory[n], &lpObj->MuunInventory[n]); 
	}

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

BYTE CMuunSystem::CheckMuunInventoryEmpty(LPOBJ lpObj)
{
	for (int i = MUUN_INVENTORY_WEAR_SIZE; i < MUUN_INVENTORY_SIZE; ++i)
	{
		if (!lpObj->MuunInventory[i].IsItem())
			return i;
	}
	return -1;
}

bool  CMuunSystem::IsMuunExpireDate(CItem* item)
{
	bool result; // al@2

	if (this->IsMuunItem(item))
	{
		MUUN_SYSTEM_INFO MuunSystemInfo;

		if (this->GetMuunSystemInfo(item->m_Index, &MuunSystemInfo) != 0)
		{
			MUUN_SYSTEM_OPTION_INFO MuunSystemOptionInfo;

			if (this->GetMuunSystemOptionInfo(MuunSystemInfo.OptionIndex, &MuunSystemOptionInfo) != 0)
			{

			}
		}

		result = 1;
	}
	else
	{
		result = 0;
	}
	return result;
}