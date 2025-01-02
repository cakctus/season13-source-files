#include "stdafx.h"
#include "PentagramSystem.h"
#include "CastleSiegeSync.h"
#include "ChaosBox.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "ItemManager.h"
#include "MemScript.h"
#include "RandomManager.h"
#include "Util.h"
#include "RheaGold.h"
#include "Log.h"
#include "ObjectManager.h"

CPentagramSystem gPentagramSystem;

CPentagramSystem::CPentagramSystem() // OK
{
	this->m_PentagramTypeInfo.clear();
	this->m_PentagramOptionInfo.clear();
	this->m_PentagramJewelOptionInfo.clear();
	this->m_PentagramJewelRemoveInfo.clear();
	this->m_PentagramJewelUpgradeRankInfo.clear();
	this->m_PentagramJewelUpgradeLevelInfo.clear();
}

CPentagramSystem::~CPentagramSystem() // OK
{
}

void CPentagramSystem::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_PentagramTypeInfo.clear();

	this->m_PentagramOptionInfo.clear();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					PENTAGRAM_TYPE_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.OptionIndex1 = lpMemScript->GetAsNumber();
					info.OptionIndex2 = lpMemScript->GetAsNumber();
					info.OptionIndex3 = lpMemScript->GetAsNumber();
					info.OptionIndex4 = lpMemScript->GetAsNumber();
					info.OptionIndex5 = lpMemScript->GetAsNumber();
					info.OptionIndex6 = lpMemScript->GetAsNumber();
					info.OptionIndex7 = lpMemScript->GetAsNumber();

					this->m_PentagramTypeInfo.insert(std::pair<int,PENTAGRAM_TYPE_INFO>(info.Index,info));
				}
				else if(section == 1)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					PENTAGRAM_OPTION_INFO info;

					info.Index = lpMemScript->GetNumber();

					lpMemScript->GetToken();

					info.Value = lpMemScript->GetAsNumber();

					for(int n=0;n < MAX_PENTAGRAM_JEWEL_RANK;n++)
					{
						info.Type[n] = lpMemScript->GetAsNumber();
						info.Rank[n] = lpMemScript->GetAsNumber();
						info.Level[n] = lpMemScript->GetAsNumber();
					}

					this->m_PentagramOptionInfo.insert(std::pair<int,PENTAGRAM_OPTION_INFO>(info.Index,info));
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CPentagramSystem::LoadJewel(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_PentagramJewelOptionInfo.clear();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			PENTAGRAM_JEWEL_OPTION_INFO info;

			info.Index = lpMemScript->GetNumber();
			info.ItemIndex = lpMemScript->GetAsNumber();
			info.RankNumber = lpMemScript->GetAsNumber();
			info.RankOption = lpMemScript->GetAsNumber();
			info.RankOptionRate = lpMemScript->GetAsNumber();

			lpMemScript->GetAsString();

			for(int n=0;n <= MAX_PENTAGRAM_JEWEL_LEVEL;n++)
			{
				info.RankOptionValue[n] = lpMemScript->GetAsNumber();
			}

			this->m_PentagramJewelOptionInfo.insert(std::pair<int,PENTAGRAM_JEWEL_OPTION_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CPentagramSystem::LoadMixRate(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_PentagramJewelRemoveInfo.clear();
	this->m_PentagramJewelUpgradeRankInfo.clear();
	this->m_PentagramJewelUpgradeLevelInfo.clear();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					PENTAGRAM_JEWEL_REMOVE_INFO info;

					info.Index = lpMemScript->GetNumber();

					for(int n=0;n < MAX_PENTAGRAM_JEWEL_RANK;n++)
					{
						info.MixRate[n] = lpMemScript->GetAsNumber();
					}

					this->m_PentagramJewelRemoveInfo.insert(std::pair<int,PENTAGRAM_JEWEL_REMOVE_INFO>(info.Index,info));
				}
				else if(section == 1)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					PENTAGRAM_JEWEL_UPGRADE_RANK_INFO info;

					info.Index = lpMemScript->GetNumber();
					info.MixMoney = lpMemScript->GetAsNumber();
					info.MixRate = lpMemScript->GetAsNumber();

					this->m_PentagramJewelUpgradeRankInfo.insert(std::pair<int,PENTAGRAM_JEWEL_UPGRADE_RANK_INFO>(info.Index,info));
				}
				else if(section == 2)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					PENTAGRAM_JEWEL_UPGRADE_LEVEL_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.MixMoney = lpMemScript->GetAsNumber();

					for(int n=0;n < MAX_PENTAGRAM_JEWEL_RANK;n++)
					{
						info.MixRate[n] = lpMemScript->GetAsNumber();
					}

					this->m_PentagramJewelUpgradeLevelInfo.insert(std::pair<int,PENTAGRAM_JEWEL_UPGRADE_LEVEL_INFO>(info.Index,info));
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CPentagramSystem::IsPentagramItem(int ItemIndex) // OK
{
	if((ItemIndex >= GET_ITEM(12,200) && ItemIndex <= GET_ITEM(12,220)) || (ItemIndex >= GET_ITEM(12, 305) && ItemIndex <= GET_ITEM(12, 308)))
	{
		return 1;
	}

	return 0;
}

bool CPentagramSystem::IsPentagramItem(CItem* lpItem) // OK
{
	if((lpItem->m_Index >= GET_ITEM(12,200) && lpItem->m_Index <= GET_ITEM(12,220))||
		(lpItem->m_Index >= GET_ITEM(12,306) && lpItem->m_Index <= GET_ITEM(12,308)))

	{
		return 1;
	}

	return 0;
}

bool CPentagramSystem::IsPentagramJewel(int ItemIndex) // OK
{
	if(ItemIndex >= GET_ITEM(12,221) && ItemIndex <= GET_ITEM(12,261))
	{
		return 1;
	}

	return 0;
}

bool CPentagramSystem::IsPentagramJewel(CItem* lpItem) // OK
{
	if(lpItem->m_Index >= GET_ITEM(12,221) && lpItem->m_Index <= GET_ITEM(12,261))
	{
		return 1;
	}

	return 0;
}

bool CPentagramSystem::IsPentagramMithril(int ItemIndex) // OK
{
	if(ItemIndex == GET_ITEM(12,144) || ItemIndex == GET_ITEM(12,145) || ItemIndex == GET_ITEM(12,148))
	{
		return 1;
	}

	return 0;
}

bool CPentagramSystem::IsPentagramMithril(CItem* lpItem) // OK
{
	if(lpItem->m_Index == GET_ITEM(12,144) || lpItem->m_Index == GET_ITEM(12,145) || lpItem->m_Index == GET_ITEM(12,148))
	{
		return 1;
	}

	return 0;
}

PENTAGRAM_JEWEL_INFO* CPentagramSystem::GetPentagramJewelInfo(LPOBJ lpObj,int index,int type) // OK
{
	if(CHECK_RANGE(index,MAX_PENTAGRAM_JEWEL_INFO) == 0)
	{
		return 0;
	}

	if(type == PENTAGRAM_JEWEL_TYPE_INVENTORY)
	{
		if(lpObj->PentagramJewelInfo_Inventory[index].Index == index)
		{
			return &lpObj->PentagramJewelInfo_Inventory[index];
		}
	}
	else
	{
		if(lpObj->PentagramJewelInfo_Warehouse[index].Index == index)
		{
			return &lpObj->PentagramJewelInfo_Warehouse[index];
		}
	}

	return 0;
}

PENTAGRAM_JEWEL_INFO* CPentagramSystem::AddPentagramJewelInfo(LPOBJ lpObj,CItem* lpItem,int type) // OK
{
	PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = ((type==PENTAGRAM_JEWEL_TYPE_INVENTORY)?lpObj->PentagramJewelInfo_Inventory:lpObj->PentagramJewelInfo_Warehouse);

	for(int n=0;n < MAX_PENTAGRAM_JEWEL_INFO;n++)
	{
		if(lpPentagramJewelInfo[n].Index == 0xFF)
		{
			lpPentagramJewelInfo[n].Type = type;
			lpPentagramJewelInfo[n].Index = n;
			lpPentagramJewelInfo[n].Attribute = lpItem->m_SocketOptionBonus;
			lpPentagramJewelInfo[n].ItemSection = lpItem->m_Index/MAX_ITEM_TYPE;
			lpPentagramJewelInfo[n].ItemType = lpItem->m_Index%MAX_ITEM_TYPE;
			lpPentagramJewelInfo[n].ItemLevel = (BYTE)lpItem->m_Level;
			lpPentagramJewelInfo[n].OptionIndexRank1 = lpItem->m_SocketOption[0]%16;
			lpPentagramJewelInfo[n].OptionLevelRank1 = lpItem->m_SocketOption[0]/16;
			lpPentagramJewelInfo[n].OptionIndexRank2 = lpItem->m_SocketOption[1]%16;
			lpPentagramJewelInfo[n].OptionLevelRank2 = lpItem->m_SocketOption[1]/16;
			lpPentagramJewelInfo[n].OptionIndexRank3 = lpItem->m_SocketOption[2]%16;
			lpPentagramJewelInfo[n].OptionLevelRank3 = lpItem->m_SocketOption[2]/16;
			lpPentagramJewelInfo[n].OptionIndexRank4 = lpItem->m_SocketOption[3]%16;
			lpPentagramJewelInfo[n].OptionLevelRank4 = lpItem->m_SocketOption[3]/16;
			lpPentagramJewelInfo[n].OptionIndexRank5 = lpItem->m_SocketOption[4]%16;
			lpPentagramJewelInfo[n].OptionLevelRank5 = lpItem->m_SocketOption[4]/16;

			this->GDPentagramJewelInsertSaveSend(lpObj->Index,&lpPentagramJewelInfo[n]);
			return &lpPentagramJewelInfo[n];
		}
	}

	return 0;
}

PENTAGRAM_JEWEL_INFO* CPentagramSystem::AddPentagramJewelInfo(LPOBJ lpObj,PENTAGRAM_JEWEL_INFO* lpInfo,int type) // OK
{
	PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = ((type==PENTAGRAM_JEWEL_TYPE_INVENTORY)?lpObj->PentagramJewelInfo_Inventory:lpObj->PentagramJewelInfo_Warehouse);

	for(int n=0;n < MAX_PENTAGRAM_JEWEL_INFO;n++)
	{
		if(lpPentagramJewelInfo[n].Index == 0xFF)
		{
			lpPentagramJewelInfo[n].Type = type;
			lpPentagramJewelInfo[n].Index = n;
			lpPentagramJewelInfo[n].Attribute = lpInfo->Attribute;
			lpPentagramJewelInfo[n].ItemSection = lpInfo->ItemSection;
			lpPentagramJewelInfo[n].ItemType = lpInfo->ItemType;
			lpPentagramJewelInfo[n].ItemLevel = lpInfo->ItemLevel;
			lpPentagramJewelInfo[n].OptionIndexRank1 = lpInfo->OptionIndexRank1;
			lpPentagramJewelInfo[n].OptionLevelRank1 = lpInfo->OptionLevelRank1;
			lpPentagramJewelInfo[n].OptionIndexRank2 = lpInfo->OptionIndexRank2;
			lpPentagramJewelInfo[n].OptionLevelRank2 = lpInfo->OptionLevelRank2;
			lpPentagramJewelInfo[n].OptionIndexRank3 = lpInfo->OptionIndexRank3;
			lpPentagramJewelInfo[n].OptionLevelRank3 = lpInfo->OptionLevelRank3;
			lpPentagramJewelInfo[n].OptionIndexRank4 = lpInfo->OptionIndexRank4;
			lpPentagramJewelInfo[n].OptionLevelRank4 = lpInfo->OptionLevelRank4;
			lpPentagramJewelInfo[n].OptionIndexRank5 = lpInfo->OptionIndexRank5;
			lpPentagramJewelInfo[n].OptionLevelRank5 = lpInfo->OptionLevelRank5;
			this->GDPentagramJewelInsertSaveSend(lpObj->Index,&lpPentagramJewelInfo[n]);
			return &lpPentagramJewelInfo[n];
		}
	}

	return 0;
}

void CPentagramSystem::DelPentagramJewelInfo(LPOBJ lpObj,int index,int type) // OK
{
	if(CHECK_RANGE(index,MAX_PENTAGRAM_JEWEL_INFO) == 0)
	{
		return;
	}

	if(type == PENTAGRAM_JEWEL_TYPE_INVENTORY)
	{
		if(lpObj->PentagramJewelInfo_Inventory[index].Index == index)
		{
			lpObj->PentagramJewelInfo_Inventory[index].Clear();
			this->GDPentagramJewelDeleteSaveSend(lpObj->Index,type,index);
		}
	}
	else
	{
		if(lpObj->PentagramJewelInfo_Warehouse[index].Index == index)
		{
			lpObj->PentagramJewelInfo_Warehouse[index].Clear();
			this->GDPentagramJewelDeleteSaveSend(lpObj->Index,type,index);
		}
	}
}

void CPentagramSystem::DelAllPentagramJewelInfo(LPOBJ lpObj,CItem* lpItem,int type) // OK
{
	if(lpItem->IsPentagramItem() != 0)
	{
		for(int n=0;n < MAX_SOCKET_OPTION;n++)
		{
			this->DelPentagramJewelInfo(lpObj,lpItem->m_SocketOption[n],type);
		}
	}
}

bool CPentagramSystem::CheckExchangePentagramItem(LPOBJ lpObj) // OK
{
	/*for(int n=0;n < TRADE_SIZE;n++)
	{
		if(lpObj->Trade[n].IsItem() != 0 && lpObj->Trade[n].IsPentagramItem() != 0)
		{
			if(lpObj->Trade[n].m_Durability == 0)
			{
				return 0;
			}
		}
	}*/

	return 1;
}

bool CPentagramSystem::CheckExchangePentagramItem(LPOBJ lpObj,CItem* lpItem) // OK
{
	/*if(lpItem->IsItem() != 0 && lpItem->IsPentagramItem() != 0)
	{
		if(lpItem->m_Durability == 0)
		{
			return 0;
		}
	}*/

	return 1;
}

bool CPentagramSystem::CheckPentagramOption(LPOBJ lpObj,PENTAGRAM_OPTION_INFO* lpInfo) // OK
{
	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		if(lpInfo->Type[n] != -1)
		{
			PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = this->GetPentagramJewelInfo(lpObj,lpObj->Inventory[236].m_SocketOption[n],PENTAGRAM_JEWEL_TYPE_INVENTORY);

			if(lpPentagramJewelInfo == 0)
			{
				return 0;
			}

			if(lpPentagramJewelInfo->ItemLevel < lpInfo->Level[n])
			{
				return 0;
			}

			if((lpPentagramJewelInfo->Attribute/16) < lpInfo->Rank[n])
			{
				return 0;
			}
		}
	}

	return 1;
}

void CPentagramSystem::MovePentagramToInventoryFromWarehouse(LPOBJ lpObj,CItem* lpItem) // OK
{
	if(lpItem->IsPentagramItem() == 0)
	{
		return;
	}

	int count = 0;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo_Warehouse = this->GetPentagramJewelInfo(lpObj,lpItem->m_SocketOption[n],PENTAGRAM_JEWEL_TYPE_WAREHOUSE);

		if(lpPentagramJewelInfo_Warehouse != 0)
		{
			PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo_Inventory = this->AddPentagramJewelInfo(lpObj,lpPentagramJewelInfo_Warehouse,PENTAGRAM_JEWEL_TYPE_INVENTORY);

			if(lpPentagramJewelInfo_Inventory != 0)
			{
				this->DelPentagramJewelInfo(lpObj,lpPentagramJewelInfo_Warehouse->Index,PENTAGRAM_JEWEL_TYPE_WAREHOUSE);
				lpItem->m_SocketOption[n] = lpPentagramJewelInfo_Inventory->Index;
				count++;
			}
		}
	}

	if(count > 0)
	{
		this->GCPentagramJewelInfoSend(lpObj->Index, PENTAGRAM_JEWEL_TYPE_WAREHOUSE);
		this->GCPentagramJewelInfoSend(lpObj->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);		
	}
}

void CPentagramSystem::MovePentagramToWarehouseFromInventory(LPOBJ lpObj,CItem* lpItem) // OK
{
	if(lpItem->IsPentagramItem() == 0)
	{
		return;
	}

	int count = 0;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo_Inventory = this->GetPentagramJewelInfo(lpObj,lpItem->m_SocketOption[n],PENTAGRAM_JEWEL_TYPE_INVENTORY);

		if(lpPentagramJewelInfo_Inventory != 0)
		{
			PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo_Warehouse = this->AddPentagramJewelInfo(lpObj,lpPentagramJewelInfo_Inventory,PENTAGRAM_JEWEL_TYPE_WAREHOUSE);

			if(lpPentagramJewelInfo_Warehouse != 0)
			{
				this->DelPentagramJewelInfo(lpObj,lpPentagramJewelInfo_Inventory->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);
				lpItem->m_SocketOption[n] = lpPentagramJewelInfo_Warehouse->Index;
				count++;
			}
		}
	}

	if(count > 0)
	{
		this->GCPentagramJewelInfoSend(lpObj->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);
		this->GCPentagramJewelInfoSend(lpObj->Index,PENTAGRAM_JEWEL_TYPE_WAREHOUSE);
	}
}

void CPentagramSystem::ExchangePentagramItem(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	int count = 0;

	for(int n=0;n < TRADE_SIZE;n++)
	{
		if(lpObj->Trade[n].IsItem() != 0 && lpObj->Trade[n].IsPentagramItem() != 0)
		{
			for(int i=0;i < MAX_SOCKET_OPTION;i++)
			{
				PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo_Source = this->GetPentagramJewelInfo(lpObj,lpObj->Trade[n].m_SocketOption[i],PENTAGRAM_JEWEL_TYPE_INVENTORY);

				if(lpPentagramJewelInfo_Source != 0)
				{
					PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo_Target = this->AddPentagramJewelInfo(lpTarget,lpPentagramJewelInfo_Source,PENTAGRAM_JEWEL_TYPE_INVENTORY);

					if(lpPentagramJewelInfo_Target != 0)
					{
						this->DelPentagramJewelInfo(lpObj,lpPentagramJewelInfo_Source->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);
						lpTarget->Inventory[lpObj->Trade[n].m_Slot].m_SocketOption[i] = lpPentagramJewelInfo_Target->Index;
						count++;
					}
				}
			}

			//lpTarget->Inventory[lpObj->Trade[n].m_Slot].m_Durability--;
		}
	}

	if(count > 0)
	{
		this->GCPentagramJewelInfoSend(lpTarget->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);
	}
}

void CPentagramSystem::ExchangePentagramItem(LPOBJ lpObj,LPOBJ lpTarget,CItem* lpItem) // OK
{
	if(lpItem->IsPentagramItem() == 0)
	{
		return;
	}

	int count = 0;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo_Source = this->GetPentagramJewelInfo(lpObj,lpItem->m_SocketOption[n],PENTAGRAM_JEWEL_TYPE_INVENTORY);

		if(lpPentagramJewelInfo_Source != 0)
		{
			PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo_Target = this->AddPentagramJewelInfo(lpTarget,lpPentagramJewelInfo_Source,PENTAGRAM_JEWEL_TYPE_INVENTORY);

			if(lpPentagramJewelInfo_Target != 0)
			{
				this->DelPentagramJewelInfo(lpObj,lpPentagramJewelInfo_Source->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);
				lpItem->m_SocketOption[n] = lpPentagramJewelInfo_Target->Index;
				count++;
			}
		}
	}

	//lpItem->m_Durability--;

	if(count > 0)
	{
		this->GCPentagramJewelInfoSend(lpTarget->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);
	}
}

bool CPentagramSystem::GetPentagramTypeInfo(int index,PENTAGRAM_TYPE_INFO* lpInfo) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	std::map<int,PENTAGRAM_TYPE_INFO>::iterator it = this->m_PentagramTypeInfo.find(index);

	if(it == this->m_PentagramTypeInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}

	#else

	return 0;

	#endif
}

bool CPentagramSystem::GetPentagramOptionInfo(int index,PENTAGRAM_OPTION_INFO* lpInfo) // OK
{
	std::map<int,PENTAGRAM_OPTION_INFO>::iterator it = this->m_PentagramOptionInfo.find(index);

	if(it == this->m_PentagramOptionInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

bool CPentagramSystem::GetPentagramJewelOptionInfo(int ItemIndex,int RankNumber,int RankOption,PENTAGRAM_JEWEL_OPTION_INFO* lpInfo) // OK
{
	for(std::map<int,PENTAGRAM_JEWEL_OPTION_INFO>::iterator it=this->m_PentagramJewelOptionInfo.begin();it != this->m_PentagramJewelOptionInfo.end();it++)
	{
		if(it->second.ItemIndex == ItemIndex)
		{
			if(it->second.RankNumber == RankNumber)
			{
				if(it->second.RankOption == RankOption)
				{
					(*lpInfo) = it->second;
					return 1;
				}
			}
		}
	}

	return 0;
}

bool CPentagramSystem::GetPentagramJewelRemoveInfo(int index,PENTAGRAM_JEWEL_REMOVE_INFO* lpInfo) // OK
{
	std::map<int,PENTAGRAM_JEWEL_REMOVE_INFO>::iterator it = this->m_PentagramJewelRemoveInfo.find(index);

	if(it == this->m_PentagramJewelRemoveInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

bool CPentagramSystem::GetPentagramJewelUpgradeRankInfo(int index,PENTAGRAM_JEWEL_UPGRADE_RANK_INFO* lpInfo) // OK
{
	std::map<int,PENTAGRAM_JEWEL_UPGRADE_RANK_INFO>::iterator it = this->m_PentagramJewelUpgradeRankInfo.find(index);

	if(it == this->m_PentagramJewelUpgradeRankInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

bool CPentagramSystem::GetPentagramJewelUpgradeLevelInfo(int index,PENTAGRAM_JEWEL_UPGRADE_LEVEL_INFO* lpInfo) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	std::map<int,PENTAGRAM_JEWEL_UPGRADE_LEVEL_INFO>::iterator it = this->m_PentagramJewelUpgradeLevelInfo.find(index);

	if(it == this->m_PentagramJewelUpgradeLevelInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}

	return 0;

	#else

	return 0;

	#endif
}

bool CPentagramSystem::GetPentagramRandomJewelOption(int ItemIndex,int RankNumber,BYTE* option) // OK
{
	CRandomManager RandomManager;

	for(std::map<int,PENTAGRAM_JEWEL_OPTION_INFO>::iterator it=this->m_PentagramJewelOptionInfo.begin();it != this->m_PentagramJewelOptionInfo.end();it++)
	{
		if(it->second.ItemIndex == ItemIndex)
		{
			if(it->second.RankNumber == RankNumber)
			{
				RandomManager.AddElement(it->second.RankOption,it->second.RankOptionRate);
			}
		}
	}

	return RandomManager.GetRandomElement(option);
}

void CPentagramSystem::GetPentagramRelationshipDamage(LPOBJ lpObj,LPOBJ lpTarget,int* damage) // OK
{
	int rate = 100;

	if(lpTarget->ElementalAttribute == 0)
	{
		if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
		{
			rate = 143;
		}

		if(lpObj->Type == OBJECT_MONSTER && lpTarget->Type == OBJECT_USER)
		{
			rate = 110;
		}

		if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_MONSTER)
		{
			rate = 100;
		}
	}
	else
	{
		if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
		{
			rate = gPentagramRelationshipDamageTablePvP[lpObj->ElementalAttribute][lpTarget->ElementalAttribute];
		}
		else
		{
			rate = gPentagramRelationshipDamageTablePvM[lpObj->ElementalAttribute][lpTarget->ElementalAttribute];
		}

		//LogAdd(LOG_DEBUG, "rate 1: %d", rate);

		switch(lpTarget->ElementalAttribute)
		{
			case ELEMENTAL_ATTRIBUTE_FIRE:
				rate += lpObj->PentagramJewelOption.MulElementalDamageVsFire;
				break;
			case ELEMENTAL_ATTRIBUTE_WATER:
				rate += lpObj->PentagramJewelOption.MulElementalDamageVsWater;
				break;
			case ELEMENTAL_ATTRIBUTE_EARTH:
				rate += lpObj->PentagramJewelOption.MulElementalDamageVsEarth;
				break;
			case ELEMENTAL_ATTRIBUTE_WIND:
				rate += lpObj->PentagramJewelOption.MulElementalDamageVsWind;
				break;
			case ELEMENTAL_ATTRIBUTE_DARK:
				rate += lpObj->PentagramJewelOption.MulElementalDamageVsDark;
				break;
		}

		//LogAdd(LOG_DEBUG, "rate 2: %d", rate);
	}

	(*damage) = ((*damage)*rate)/100;
}

void CPentagramSystem::GetPentagramRelationshipDefense(LPOBJ lpObj,LPOBJ lpTarget,int* defense) // OK
{
	int rate = 100;

	if(lpObj->ElementalAttribute == 0)
	{
		if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
		{
			rate = 80;
		}

		if(lpObj->Type == OBJECT_MONSTER && lpTarget->Type == OBJECT_USER)
		{
			rate = 80;
		}

		if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_MONSTER)
		{
			rate = 130;
		}
	}
	else
	{
		if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
		{
			rate = gPentagramRelationshipDefenseTablePvP[lpObj->ElementalAttribute][lpTarget->ElementalAttribute];
		}
		else
		{
			rate = gPentagramRelationshipDefenseTablePvM[lpObj->ElementalAttribute][lpTarget->ElementalAttribute];
		}

		switch(lpTarget->ElementalAttribute)
		{
			case ELEMENTAL_ATTRIBUTE_FIRE:
				rate += lpObj->PentagramJewelOption.MulElementalDefenseVsFire;
				break;
			case ELEMENTAL_ATTRIBUTE_WATER:
				rate += lpObj->PentagramJewelOption.MulElementalDefenseVsWater;
				break;
			case ELEMENTAL_ATTRIBUTE_EARTH:
				rate += lpObj->PentagramJewelOption.MulElementalDefenseVsEarth;
				break;
			case ELEMENTAL_ATTRIBUTE_WIND:
				rate += lpObj->PentagramJewelOption.MulElementalDefenseVsWind;
				break;
			case ELEMENTAL_ATTRIBUTE_DARK:
				rate += lpObj->PentagramJewelOption.MulElementalDefenseVsDark;
				break;
		}
	}

	(*defense) = ((*defense)*rate)/100;
}

void CPentagramSystem::GetPentagramRelationshipDamageIncrease(LPOBJ lpObj, LPOBJ lpTarget, int* damage) // OK
{
	int rate = 0;

	rate = gPentagramRelationshipDamageTablePvM[lpObj->ElementalAttribute][lpTarget->ElementalAttribute];

	//LogAdd(LOG_DEBUG, "rate1: %d", rate);

	switch (lpTarget->ElementalAttribute)
	{
	case ELEMENTAL_ATTRIBUTE_FIRE:
		rate += lpObj->PentagramJewelOption.MulElementalDamageVsFire;
		break;
	case ELEMENTAL_ATTRIBUTE_WATER:
		rate += lpObj->PentagramJewelOption.MulElementalDamageVsWater;
		break;
	case ELEMENTAL_ATTRIBUTE_EARTH:
		rate += lpObj->PentagramJewelOption.MulElementalDamageVsEarth;
		break;
	case ELEMENTAL_ATTRIBUTE_WIND:
		rate += lpObj->PentagramJewelOption.MulElementalDamageVsWind;
		break;
	case ELEMENTAL_ATTRIBUTE_DARK:
		rate += lpObj->PentagramJewelOption.MulElementalDamageVsDark;
		break;
	}
	//LogAdd(LOG_DEBUG, "rate2: %d", rate);

	(*damage) += ((*damage) * rate) / 100;
}

void CPentagramSystem::GetPentagramRelationshipDamageDecrease(LPOBJ lpObj, LPOBJ lpTarget, int* damage) // OK
{
	int rate = 0;

	if (lpTarget->ElementalAttribute == 0)
	{
		rate = 0;
	}
	else
	{
		rate = 100 - (gPentagramRelationshipDefenseTablePvM[lpObj->ElementalAttribute][lpTarget->ElementalAttribute]);
		//LogAdd(LOG_DEBUG, "rate1: %d :  targe element: %d", rate, lpObj->ElementalAttribute);
		switch (lpObj->ElementalAttribute)
		{
		case ELEMENTAL_ATTRIBUTE_FIRE:
			rate += lpTarget->PentagramJewelOption.MulElementalDefenseVsFire;
			break;
		case ELEMENTAL_ATTRIBUTE_WATER:
			rate += lpTarget->PentagramJewelOption.MulElementalDefenseVsWater;
			break;
		case ELEMENTAL_ATTRIBUTE_EARTH:
			rate += lpTarget->PentagramJewelOption.MulElementalDefenseVsEarth;
			break;
		case ELEMENTAL_ATTRIBUTE_WIND:
			rate += lpTarget->PentagramJewelOption.MulElementalDefenseVsWind;
			break;
		case ELEMENTAL_ATTRIBUTE_DARK:
			rate += lpTarget->PentagramJewelOption.MulElementalDefenseVsDark;
			break;
		}
		//LogAdd(LOG_DEBUG, "rate2: %d", rate);
	}

	//LogAdd(LOG_DEBUG, "rate3: %d", rate);

	(*damage) -= ((*damage) * rate) / 100;
}

void CPentagramSystem::CalcPentagramOption(LPOBJ lpObj,bool flag) // OK
{
	if(lpObj->Inventory[236].IsItem() == 0 || lpObj->Inventory[236].IsPentagramItem() == 0 || lpObj->Inventory[236].m_IsValidItem == 0)
	{
		return;
	}

	PENTAGRAM_TYPE_INFO PentagramTypeInfo;

	if(this->GetPentagramTypeInfo(lpObj->Inventory[236].m_Index,&PentagramTypeInfo) != 0)
	{
		PENTAGRAM_OPTION_INFO PentagramOptionInfo;

		if(this->GetPentagramOptionInfo(PentagramTypeInfo.OptionIndex1,&PentagramOptionInfo) != 0)
		{
			if(this->CheckPentagramOption(lpObj,&PentagramOptionInfo) != 0)
			{
				this->InsertOption(lpObj,PentagramOptionInfo.Index,PentagramOptionInfo.Value,flag);
			}
		}

		if(this->GetPentagramOptionInfo(PentagramTypeInfo.OptionIndex2,&PentagramOptionInfo) != 0)
		{
			if(this->CheckPentagramOption(lpObj,&PentagramOptionInfo) != 0)
			{
				this->InsertOption(lpObj,PentagramOptionInfo.Index,PentagramOptionInfo.Value,flag);
			}
		}

		if (this->GetPentagramOptionInfo(PentagramTypeInfo.OptionIndex3, &PentagramOptionInfo) != 0)
		{
			if (this->CheckPentagramOption(lpObj, &PentagramOptionInfo) != 0)
			{
				this->InsertOption(lpObj, PentagramOptionInfo.Index, PentagramOptionInfo.Value, flag);
			}
		}

		if (this->GetPentagramOptionInfo(PentagramTypeInfo.OptionIndex4, &PentagramOptionInfo) != 0)
		{
			if (this->CheckPentagramOption(lpObj, &PentagramOptionInfo) != 0)
			{
				this->InsertOption(lpObj, PentagramOptionInfo.Index, PentagramOptionInfo.Value, flag);
			}
		}

		if (this->GetPentagramOptionInfo(PentagramTypeInfo.OptionIndex5, &PentagramOptionInfo) != 0)
		{
			if (this->CheckPentagramOption(lpObj, &PentagramOptionInfo) != 0)
			{
				this->InsertOption(lpObj, PentagramOptionInfo.Index, PentagramOptionInfo.Value, flag);
			}
		}

		if (this->GetPentagramOptionInfo(PentagramTypeInfo.OptionIndex6, &PentagramOptionInfo) != 0)
		{
			if (this->CheckPentagramOption(lpObj, &PentagramOptionInfo) != 0)
			{
				this->InsertOption(lpObj, PentagramOptionInfo.Index, PentagramOptionInfo.Value, flag);
			}
		}

		if (this->GetPentagramOptionInfo(PentagramTypeInfo.OptionIndex7, &PentagramOptionInfo) != 0)
		{
			if (this->CheckPentagramOption(lpObj, &PentagramOptionInfo) != 0)
			{
				this->InsertOption(lpObj, PentagramOptionInfo.Index, PentagramOptionInfo.Value, flag);
			}
		}
	}
}

void CPentagramSystem::CalcPentagramJewelOption(LPOBJ lpObj,bool flag) // OK
{
	if(lpObj->Inventory[236].IsItem() == 0 || lpObj->Inventory[236].IsPentagramItem() == 0 || lpObj->Inventory[236].m_IsValidItem == 0)
	{
		return;
	}

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = this->GetPentagramJewelInfo(lpObj,lpObj->Inventory[236].m_SocketOption[n],PENTAGRAM_JEWEL_TYPE_INVENTORY);

		if(lpPentagramJewelInfo != 0)
		{
			PENTAGRAM_JEWEL_OPTION_INFO PentagramJewelOptionInfo;

			if(this->GetPentagramJewelOptionInfo(GET_ITEM(lpPentagramJewelInfo->ItemSection,lpPentagramJewelInfo->ItemType),1,lpPentagramJewelInfo->OptionIndexRank1,&PentagramJewelOptionInfo) != 0)
			{
				this->InsertJewelOption(lpObj,PentagramJewelOptionInfo.Index,PentagramJewelOptionInfo.RankOptionValue[lpPentagramJewelInfo->OptionLevelRank1],flag);
			}

			if(this->GetPentagramJewelOptionInfo(GET_ITEM(lpPentagramJewelInfo->ItemSection,lpPentagramJewelInfo->ItemType),2,lpPentagramJewelInfo->OptionIndexRank2,&PentagramJewelOptionInfo) != 0)
			{
				this->InsertJewelOption(lpObj,PentagramJewelOptionInfo.Index,PentagramJewelOptionInfo.RankOptionValue[lpPentagramJewelInfo->OptionLevelRank2],flag);
			}

			if(this->GetPentagramJewelOptionInfo(GET_ITEM(lpPentagramJewelInfo->ItemSection,lpPentagramJewelInfo->ItemType),3,lpPentagramJewelInfo->OptionIndexRank3,&PentagramJewelOptionInfo) != 0)
			{
				this->InsertJewelOption(lpObj,PentagramJewelOptionInfo.Index,PentagramJewelOptionInfo.RankOptionValue[lpPentagramJewelInfo->OptionLevelRank3],flag);
			}

			if(this->GetPentagramJewelOptionInfo(GET_ITEM(lpPentagramJewelInfo->ItemSection,lpPentagramJewelInfo->ItemType),4,lpPentagramJewelInfo->OptionIndexRank4,&PentagramJewelOptionInfo) != 0)
			{
				this->InsertJewelOption(lpObj,PentagramJewelOptionInfo.Index,PentagramJewelOptionInfo.RankOptionValue[lpPentagramJewelInfo->OptionLevelRank4],flag);
			}

			if(this->GetPentagramJewelOptionInfo(GET_ITEM(lpPentagramJewelInfo->ItemSection,lpPentagramJewelInfo->ItemType),5,lpPentagramJewelInfo->OptionIndexRank5,&PentagramJewelOptionInfo) != 0)
			{
				this->InsertJewelOption(lpObj,PentagramJewelOptionInfo.Index,PentagramJewelOptionInfo.RankOptionValue[lpPentagramJewelInfo->OptionLevelRank5],flag);
			}
		}
	}
}

void CPentagramSystem::InsertOption(LPOBJ lpObj,int index,int value,bool flag) // OK
{
	if(flag != 0)
	{
		return;
	}

	switch(index)
	{
	case PENTAGRAM_OPTION_MUL_PENTAGRAM_DAMAGE_AND_DEFENSE:
			lpObj->PentagramOption.MulPentagramDamage += value;
			lpObj->PentagramOption.MulPentagramDefense += value;
			break;
	case PENTAGRAM_OPTION_MUL_PENTAGRAM_DAMAGE:
			lpObj->PentagramOption.MulPentagramDamage += value;
			break;
	case PENTAGRAM_OPTION_MUL_PENTAGRAM_DEFENSE:
			lpObj->PentagramOption.MulPentagramDefense += value;
			break;
	case PENTAGRAM_OPTION_ADD_ELEMENTAL_CRITICAL_DAMAGE_RATE:
			lpObj->PentagramOption.AddElementalCriticalDamageRate += value;
			break;
	case PENTAGRAM_OPTION_SHELTER_RANK1:
			lpObj->PentagramOption.AddBlessingRank1 += value;
			break;
	case PENTAGRAM_OPTION_MUL_PENTAGRAM_DAMAGE_1:
			lpObj->PentagramOption.MulPentagramDamage += value;
			break;
	case PENTAGRAM_OPTION_MUL_PENTAGRAM_DEFENSE_1:
			lpObj->PentagramOption.MulPentagramDefense += value;
			break;
	case PENTAGRAM_OPTION_ADD_ELEMENTAL_CRITICAL_DAMAGE_RATE_1:
			lpObj->PentagramOption.AddElementalCriticalDamageRate += value;
			break;
	case PENTAGRAM_OPTION_ADD_ELEMENTAL_CRITICAL_DAMAGE_RATE_2:
			lpObj->PentagramOption.AddElementalCriticalDamageRate += value;
			break;
	case PENTAGRAM_OPTION_ADD_ELEMENTAL_CRITICAL_DAMAGE_RATE_3:
			lpObj->PentagramOption.AddElementalCriticalDamageRate += value;
			break;
	case PENTAGRAM_OPTION_ADD_ELEMENTAL_DEFENSE_TRANSFER_RATE:
			lpObj->PentagramOption.AddElementalDefenseTransferRate += value;
			break;
	case PENTAGRAM_OPTION_ADD_ELEMENTAL_ATTACK_TRANSFER_RATE:
			lpObj->PentagramOption.AddElementalAttackTransferRate += value;
			break;
	case PENTAGRAM_OPTION_DAMAGE_PERFECT_DODGE_RATE:
			lpObj->PentagramOption.MulPentagramDamagePerfectDodgeRate += value;
			break;
	case PENTAGRAM_OPTION_PENTAGRAM_DAMAGE:
			lpObj->PentagramOption.PentagramDamage += value;
			break;
	case PENTAGRAM_OPTION_PENTAGRAM_DEFENSE:
			lpObj->PentagramOption.PentagramDefense += value;
			break;
	case PENTAGRAM_OPTION_INTERGRITY_RANK_1:
			lpObj->PentagramOption.AddIntegrityRank1 += value;
			break;
	case PENTAGRAM_OPTION_DIVINITY_RANK_1:
			lpObj->PentagramOption.AddDivinityRank1 += value;
			break;
	case PENTAGRAM_OPTION_RADIANCE_RANK_1:
			lpObj->PentagramOption.AddRadianceRank1 += value;
			break;
	case PENTAGRAM_OPTION_MUL_PENTAGRAM_DAMAGE_AND_DEFENSE_1:
			lpObj->PentagramOption.MulPentagramDamage += value;
			lpObj->PentagramOption.MulPentagramDefense += value;
			break;
	case PENTAGRAM_OPTION_DAMAGE_PERFECT_DODGE_RATE_1:
			lpObj->PentagramOption.MulPentagramDamagePerfectDodgeRate += value;
			break;
	case PENTAGRAM_OPTION_ADD_ELEMENTAL_DEFENSE_TRANSFER_RATE_1:
			lpObj->PentagramOption.AddElementalDefenseTransferRate += value;
			break;
	case PENTAGRAM_OPTION_ADD_ELEMENTAL_ATTACK_TRANSFER_RATE_1:
			lpObj->PentagramOption.AddElementalAttackTransferRate += value;
			break;
	}
}

void CPentagramSystem::InsertJewelOption(LPOBJ lpObj,int index,int value,bool flag) // OK
{
	if (flag != 0)
	{
		return;
	}

	switch(index)
	{
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_ADD_ELEMENTAL_DAMAGE:
			lpObj->PentagramJewelOption.AddElementalDamage += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_MUL_ELEMENTAL_DAMAGE_VS_FIRE:
			lpObj->PentagramJewelOption.MulElementalDamageVsFire += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_MUL_ELEMENTAL_DAMAGE_VS_WATER:
			lpObj->PentagramJewelOption.MulElementalDamageVsWater += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_MUL_ELEMENTAL_DAMAGE_VS_EARTH:
			lpObj->PentagramJewelOption.MulElementalDamageVsEarth += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_MUL_ELEMENTAL_DAMAGE_VS_WIND:
			lpObj->PentagramJewelOption.MulElementalDamageVsWind += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_MUL_ELEMENTAL_DAMAGE_VS_DARK:
			lpObj->PentagramJewelOption.MulElementalDamageVsDark += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_ADD_ELEMENTAL_DAMAGE_PVP:
			lpObj->PentagramJewelOption.AddElementalDamagePvP += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_ADD_ELEMENTAL_DAMAGE_PVM:
			lpObj->PentagramJewelOption.AddElementalDamagePvM += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_ADD_ELEMENTAL_DAMAGE_PVM_RATIO:
			lpObj->PentagramJewelOption.AddElementalDamageRatioPvM += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_ADD_ELEMENTAL_DAMAGE_PVP_RATIO:
			lpObj->PentagramJewelOption.AddElementalDamageRatioPvP += value;
			break;
		case PENTAGRAM_JEWEL_OF_ANGER_OPTION_ADD_ELEMENTAL_DAMAGE_RATIO:
			lpObj->PentagramJewelOption.AddElementalDamageRatio += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_ADD_ELEMENTAL_DEFENSE:
			lpObj->PentagramJewelOption.AddElementalDefense += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_MUL_ELEMENTAL_DEFENSE_VS_FIRE:
			lpObj->PentagramJewelOption.MulElementalDefenseVsFire += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_MUL_ELEMENTAL_DEFENSE_VS_WATER:
			lpObj->PentagramJewelOption.MulElementalDefenseVsWater += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_MUL_ELEMENTAL_DEFENSE_VS_EARTH:
			lpObj->PentagramJewelOption.MulElementalDefenseVsEarth += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_MUL_ELEMENTAL_DEFENSE_VS_WIND:
			lpObj->PentagramJewelOption.MulElementalDefenseVsWind += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_MUL_ELEMENTAL_DEFENSE_VS_DARK:
			lpObj->PentagramJewelOption.MulElementalDefenseVsDark += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_ADD_ELEMENTAL_DEFENSE_PVP:
			lpObj->PentagramJewelOption.AddElementalDefensePvP += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_ADD_ELEMENTAL_DEFENSE_PVM:
			lpObj->PentagramJewelOption.AddElementalDefensePvM += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_ADD_ELEMENTAL_DAMAGE_PVP_RATIO:
			lpObj->PentagramJewelOption.AddElementalDamageReductionPvP += value;
			break;
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_ADD_ELEMENTAL_DAMAGE_PVM_RATIO:
			lpObj->PentagramJewelOption.AddElementalDamageReductionPvM += value;
			break;		
		case PENTAGRAM_JEWEL_OF_BLESSING_OPTION_ADD_ELEMENTAL_DAMAGE_RATIO:
			lpObj->PentagramJewelOption.AddElementalDefenseRatio += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_ATTACK_SUCCESS_RATE:
			lpObj->PentagramJewelOption.MulElementalAttackSuccessRate += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_DAMAGE_VS_FIRE:
			lpObj->PentagramJewelOption.MulElementalDamageVsFire += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_DAMAGE_VS_WATER:
			lpObj->PentagramJewelOption.MulElementalDamageVsWater += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_DAMAGE_VS_EARTH:
			lpObj->PentagramJewelOption.MulElementalDamageVsEarth += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_DAMAGE_VS_WIND:
			lpObj->PentagramJewelOption.MulElementalDamageVsWind += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_DAMAGE_VS_DARK:
			lpObj->PentagramJewelOption.MulElementalDamageVsDark += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_ADD_ELEMENTAL_DAMAGE_PVP:
			lpObj->PentagramJewelOption.AddElementalDamagePvP += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_ADD_ELEMENTAL_DAMAGE_PVM:
			lpObj->PentagramJewelOption.AddElementalDamagePvM += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_ATTACK_SUCCESS_RATE_PVP:
			lpObj->PentagramJewelOption.MulElementalAttackSuccessRatePVP += value;			
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_ATTACK_SUCCESS_RATE_PVM:
			lpObj->PentagramJewelOption.MulElementalAttackSuccessRatePVM += value;
			break;
		case PENTAGRAM_JEWEL_OF_INTEGRITY_OPTION_MUL_ELEMENTAL_ATTACK_SUCCESS_RATE1:
			lpObj->PentagramJewelOption.MulElementalAttackSuccessRate += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_SUCCESS_RATE:
			lpObj->PentagramJewelOption.MulElementalDefenseSuccessRate += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_VS_FIRE:
			lpObj->PentagramJewelOption.MulElementalDefenseVsFire += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_VS_WATER:
			lpObj->PentagramJewelOption.MulElementalDefenseVsWater += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_VS_EARTH:
			lpObj->PentagramJewelOption.MulElementalDefenseVsEarth += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_VS_WIND:
			lpObj->PentagramJewelOption.MulElementalDefenseVsWind += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_VS_DARK:
			lpObj->PentagramJewelOption.MulElementalDefenseVsDark += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_ADD_ELEMENTAL_DEFENSE_PVP:
			lpObj->PentagramJewelOption.AddElementalDefensePvP += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_ADD_ELEMENTAL_DEFENSE_PVM:
			lpObj->PentagramJewelOption.AddElementalDefensePvM += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_SUCCESS_RATE_PVP:
			lpObj->PentagramJewelOption.MulElementalDefenseSuccessRatePVP += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_SUCCESS_RATE_PVM:
			lpObj->PentagramJewelOption.MulElementalDefenseSuccessRatePVM += value;
			break;
		case PENTAGRAM_JEWEL_OF_DIVINITY_OPTION_MUL_ELEMENTAL_DEFENSE_SUCCESS_RATE1:
			lpObj->PentagramJewelOption.MulElementalDefenseSuccessRate += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ADD_ELEMENTAL_DAMAGE:
			lpObj->PentagramJewelOption.AddElementalDamage += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ADD_ELEMENTAL_DEFENSE:
			lpObj->PentagramJewelOption.AddElementalDefense += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_MUL_ELEMENTAL_ATTACK_SUCCESS_RATE:
			lpObj->PentagramJewelOption.MulElementalAttackSuccessRate += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_MUL_ELEMENTAL_DEFENSE_SUCCESS_RATE:
			lpObj->PentagramJewelOption.MulElementalDefenseSuccessRate += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ADD_ELEMENTAL_DAMAGE_III:
			lpObj->PentagramJewelOption.AddElementalDamage += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ADD_ELEMENTAL_DEFENSE_III:
			lpObj->PentagramJewelOption.AddElementalDefense += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ABS_DEFENSE:
			lpObj->PentagramJewelOption.DefenseAbs += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ABS_LIFE:
			lpObj->PentagramJewelOption.LifeAbs += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ABS_BASTION:
			lpObj->PentagramJewelOption.Bastion += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ABS_BLEEDING:
			lpObj->PentagramJewelOption.Bleeding += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ABS_PARALYZING:
			lpObj->PentagramJewelOption.Paralyzing += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ABS_BINDING:
			lpObj->PentagramJewelOption.Binding += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ABS_PUNISH:
			lpObj->PentagramJewelOption.Punish += value;
			break;
		case PENTAGRAM_JEWEL_OF_GALE_OPTION_ABS_BLINDING:
			lpObj->PentagramJewelOption.Blinding += value;
			break;
	}
}

void CPentagramSystem::CGPentagramJewelInsertRecv(PMSG_PENTAGRAM_JEWEL_INSERT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_PENTAGRAM_JEWEL_INSERT_SEND pMsg;

	pMsg.header.set(0xEC,0x00,sizeof(pMsg));

	pMsg.result = 0;

	if(INVENTORY_BASE_RANGE(lpMsg->TargetSlot) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(INVENTORY_BASE_RANGE(lpMsg->SourceSlot) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpObj->Inventory[lpMsg->TargetSlot].IsItem() == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpObj->Inventory[lpMsg->SourceSlot].IsItem() == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpObj->Inventory[lpMsg->TargetSlot].IsPentagramItem() == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpObj->Inventory[lpMsg->SourceSlot].IsPentagramJewel() == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	int SocketSlot = ( lpObj->Inventory[lpMsg->SourceSlot].m_Index - GET_ITEM(12,221) ) / 10;

	if(CHECK_RANGE(SocketSlot,MAX_SOCKET_OPTION) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpObj->Inventory[lpMsg->TargetSlot].m_SocketOption[SocketSlot] != 0xFE)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if((lpObj->Inventory[lpMsg->TargetSlot].m_SocketOptionBonus % 16) != (lpObj->Inventory[lpMsg->SourceSlot].m_SocketOptionBonus % 16) )
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = this->AddPentagramJewelInfo(lpObj,&lpObj->Inventory[lpMsg->SourceSlot],PENTAGRAM_JEWEL_TYPE_INVENTORY);

	if(lpPentagramJewelInfo == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	pMsg.result = 1;

	memcpy(&pMsg.Type,lpPentagramJewelInfo,sizeof(PENTAGRAM_JEWEL_INFO));

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	lpObj->Inventory[lpMsg->TargetSlot].m_SocketOption[SocketSlot] = lpPentagramJewelInfo->Index;

	gItemManager.GCItemModifySend(aIndex,(BYTE)lpMsg->TargetSlot);

	gItemManager.InventoryDelItem(aIndex,(BYTE)lpMsg->SourceSlot);

	gItemManager.GCItemDeleteSend(aIndex,(BYTE)lpMsg->SourceSlot,0);

	this->GCPentagramJewelResultSend(aIndex,1);
}

void CPentagramSystem::CGPentagramJewelRemoveRecv(PMSG_PENTAGRAM_JEWEL_REMOVE_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_PENTAGRAM_JEWEL_REMOVE_SEND pMsg;
	pMsg.header.set(0xEC,0x01,sizeof(pMsg));
	pMsg.result = 0;
	pMsg.Type = 0;
	pMsg.Index = 0;

	if(INVENTORY_BASE_RANGE(lpMsg->SourceSlot) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		this->GCPentagramJewelResultSend(aIndex,0);
		return;
	}

	if(lpObj->Inventory[lpMsg->SourceSlot].IsItem() == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		this->GCPentagramJewelResultSend(aIndex,0);
		return;
	}

	if(lpObj->Inventory[lpMsg->SourceSlot].IsPentagramItem() == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		this->GCPentagramJewelResultSend(aIndex,0);
		return;
	}

	if(CHECK_RANGE(lpMsg->SocketSlot,MAX_SOCKET_OPTION) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		this->GCPentagramJewelResultSend(aIndex,0);
		return;
	}

	PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = this->GetPentagramJewelInfo(lpObj,lpObj->Inventory[lpMsg->SourceSlot].m_SocketOption[lpMsg->SocketSlot],PENTAGRAM_JEWEL_TYPE_INVENTORY);

	if(lpPentagramJewelInfo == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		this->GCPentagramJewelResultSend(aIndex,0);
		return;
	}

	if(gItemManager.CheckItemInventorySpace(lpObj,GET_ITEM(lpPentagramJewelInfo->ItemSection,lpPentagramJewelInfo->ItemType)) == 0)
	{
		pMsg.result = 100;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		this->GCPentagramJewelResultSend(aIndex,100);
		return;
	}

	PENTAGRAM_JEWEL_REMOVE_INFO PentagramJewelRamoveInfo;

	if(this->GetPentagramJewelRemoveInfo(lpPentagramJewelInfo->ItemLevel,&PentagramJewelRamoveInfo) == 0)
	{
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		this->GCPentagramJewelResultSend(aIndex,0);
		return;
	}

	int Rate;

	if(lpObj->Inventory[lpMsg->Asylum].m_Index == GET_ITEM(14,320))
	{
		Rate = 10000;
	}
	else
	{
		Rate = PentagramJewelRamoveInfo.MixRate[((lpPentagramJewelInfo->Attribute/16)-1)];
	}

	Rate = 10000;

	if((GetLargeRand()%10000) < Rate)
	{
		BYTE SocketOption[MAX_SOCKET_OPTION] = {0xFF,0xFF,0xFF,0xFF,0xFF};

		SocketOption[0] = lpPentagramJewelInfo->OptionIndexRank1 | (lpPentagramJewelInfo->OptionLevelRank1*16);
		SocketOption[1] = lpPentagramJewelInfo->OptionIndexRank2 | (lpPentagramJewelInfo->OptionLevelRank2*16);
		SocketOption[2] = lpPentagramJewelInfo->OptionIndexRank3 | (lpPentagramJewelInfo->OptionLevelRank3*16);
		SocketOption[3] = lpPentagramJewelInfo->OptionIndexRank4 | (lpPentagramJewelInfo->OptionLevelRank4*16);
		SocketOption[4] = lpPentagramJewelInfo->OptionIndexRank5 | (lpPentagramJewelInfo->OptionLevelRank5*16);

		GDCreateItemSend(aIndex,0xEB,0,0,GET_ITEM(lpPentagramJewelInfo->ItemSection,lpPentagramJewelInfo->ItemType),lpPentagramJewelInfo->ItemLevel,0,0,0,0,aIndex,0,0,0,0,SocketOption,lpPentagramJewelInfo->Attribute,0);

		pMsg.result = 1;

		pMsg.Type = lpPentagramJewelInfo->Type;

		pMsg.Index = lpPentagramJewelInfo->Index;

		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

		lpObj->Inventory[lpMsg->SourceSlot].m_SocketOption[lpMsg->SocketSlot] = 0xFE;

		gItemManager.GCItemModifySend(aIndex,(BYTE)lpMsg->SourceSlot);

		gItemManager.InventoryDelItem(lpObj->Index,lpMsg->Asylum);

		gItemManager.GCItemDeleteSend(lpObj->Index,lpMsg->Asylum,1);

		this->DelPentagramJewelInfo(lpObj,lpPentagramJewelInfo->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);

		this->GCPentagramJewelResultSend(aIndex,1);
	}
	else
	{
		pMsg.result = 2;
		pMsg.Type = lpPentagramJewelInfo->Type;
		pMsg.Index = lpPentagramJewelInfo->Index;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

		lpObj->Inventory[lpMsg->SourceSlot].m_SocketOption[lpMsg->SocketSlot] = 0xFE;
		gItemManager.GCItemModifySend(aIndex,(BYTE)lpMsg->SourceSlot);
		this->DelPentagramJewelInfo(lpObj,lpPentagramJewelInfo->Index,PENTAGRAM_JEWEL_TYPE_INVENTORY);
		this->GCPentagramJewelResultSend(aIndex,2);
	}
}

void CPentagramSystem::CGPentagramJewelRefineRecv(PMSG_PENTAGRAM_JEWEL_REFINE_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->ChaosLock != 0)
	{
		return;
	}

	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	lpObj->ChaosLock = 1;

	lpObj->ChaosMoney = 0;

	lpObj->ChaosSuccessRate = 0;

	lpObj->IsChaosMixCompleted = 1;

	switch(lpMsg->type)
	{
		case CHAOS_MIX_PENTAGRAM_MITHRIL:
			gChaosBox.PentagramMithrilMixSXI(lpObj,lpMsg->solt1,lpMsg->solt2);
			break;
		case CHAOS_MIX_PENTAGRAM_ECSTASY:
			gChaosBox.PentagramEcstasySlotMix(lpObj,lpMsg->solt1,lpMsg->solt2,lpMsg->solt3,lpMsg->solt4);
			break;
		case CHAOS_MIX_PENTAGRAM_JEWEL:
			gChaosBox.PentagramJewelMix(lpObj);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE1:
			gChaosBox.PentagramDecompositeMix(lpObj,0);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE2:
			gChaosBox.PentagramDecompositeMix(lpObj,1);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE3:
			gChaosBox.PentagramDecompositeMix(lpObj,2);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE4:
			gChaosBox.PentagramDecompositeMix(lpObj,3);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE5:
			gChaosBox.PentagramDecompositeMix(lpObj,4);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE6:
			gChaosBox.PentagramDecompositeMix(lpObj,5);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE7:
			gChaosBox.PentagramDecompositeMix(lpObj,6);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE8:
			gChaosBox.PentagramDecompositeMix(lpObj,7);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE9:
			gChaosBox.PentagramDecompositeMix(lpObj,8);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE10:
			gChaosBox.PentagramDecompositeMix(lpObj,9);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE11:
			gChaosBox.PentagramDecompositeMix(lpObj,10);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE12:
			gChaosBox.PentagramDecompositeMix(lpObj,11);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE13:
			gChaosBox.PentagramDecompositeMix(lpObj,12);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE14:
			gChaosBox.PentagramDecompositeMix(lpObj,13);
			break;
		case CHAOS_MIX_PENTAGRAM_DECOMPOSITE15:
			gChaosBox.PentagramDecompositeMix(lpObj,14);
			break;
	}

	#endif
}

void CPentagramSystem::CGPentagramJewelUpgradeRecv(PMSG_PENTAGRAM_JEWEL_UPGRADE_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->ChaosLock != 0)
	{
		return;
	}

	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	lpObj->ChaosLock = 1;
	lpObj->ChaosMoney = 0;
	lpObj->ChaosSuccessRate = 0;
	lpObj->IsChaosMixCompleted = 1;

	switch(lpMsg->type)
	{
		case CHAOS_MIX_PENTAGRAM_JEWEL_UPGRADE_LEVEL:
			gChaosBox.PentagramJewelUpgradeLevelMixSXI(lpObj,lpMsg->info,lpMsg->ItemSlot1,lpMsg->ItemSlot4,lpMsg->ItemSlot2,lpMsg->ItemSlot3,lpMsg->btLevelupTargetRank,lpMsg->btLevelupSpecialMode);
			break;
		case CHAOS_MIX_PENTAGRAM_JEWEL_UPGRADE_RANK:
			gChaosBox.PentagramJewelUpgradeRankMixSXI(lpObj,lpMsg->info,lpMsg->ItemSlot1,lpMsg->ItemSlot4);
			break;
	}
}

void CPentagramSystem::GCPentagramJewelResultSend(int aIndex,int result) // OK
{
	PMSG_PENTAGRAM_JEWEL_RESULT_SEND pMsg;

	pMsg.header.set(0xEC,0x04,sizeof(pMsg));

	pMsg.result = result;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CPentagramSystem::GCPentagramJewelInfoSend(int aIndex,int type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = ((type==PENTAGRAM_JEWEL_TYPE_INVENTORY)?lpObj->PentagramJewelInfo_Inventory:lpObj->PentagramJewelInfo_Warehouse);

	BYTE send[8192];

	PMSG_PENTAGRAM_JEWEL_INFO_SEND pMsg;

	pMsg.header.set(0xEE,0x01,0);

	int size = sizeof(pMsg);

	pMsg.result = 0;

	pMsg.count = 0;

	pMsg.type = type;

	PENTAGRAM_JEWEL_INFO info;

	for(int n=0;n < MAX_PENTAGRAM_JEWEL_INFO;n++)
	{
		if(lpPentagramJewelInfo[n].Index != 0xFF)
		{
			memcpy(&info,&lpPentagramJewelInfo[n],sizeof(info));

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	pMsg.result = ((pMsg.count>0)?1:0);

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(aIndex,send,size);
}

void CPentagramSystem::GCPentagramJewelTradeInfoSend(int aIndex,int bIndex,CItem* lpItem) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	if(lpItem->IsItem() == 0 || lpItem->IsPentagramItem() == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[8192];

	PMSG_PENTAGRAM_JEWEL_INFO_SEND pMsg;

	pMsg.header.set(0xEE,0x01,0);

	int size = sizeof(pMsg);

	pMsg.result = 1;

	pMsg.count = 0;

	pMsg.type = PENTAGRAM_JEWEL_TYPE_TRADE;

	PENTAGRAM_JEWEL_INFO info;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = this->GetPentagramJewelInfo(lpObj,lpItem->m_SocketOption[n],PENTAGRAM_JEWEL_TYPE_INVENTORY);

		if(lpPentagramJewelInfo != 0)
		{
			memcpy(&info,lpPentagramJewelInfo,sizeof(info));

			info.Type = pMsg.type;

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send,&pMsg,sizeof(pMsg));

		DataSend(bIndex,send,size);
	}

	#endif
}

void CPentagramSystem::GCPentagramJewelPShopInfoSend(int aIndex,int bIndex) // OK
{
	LPOBJ lpObj = &gObj[bIndex];

	BYTE send[8192];

	PMSG_PENTAGRAM_JEWEL_INFO_SEND pMsg;

	pMsg.header.set(0xEE,0x01,0);

	int size = sizeof(pMsg);

	pMsg.result = 1;

	pMsg.count = 0;

	pMsg.type = PENTAGRAM_JEWEL_TYPE_PSHOP;

	PENTAGRAM_JEWEL_INFO info;

	for(int n=INVENTORY_EXT4_SIZE;n < INVENTORY_FULL_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() != 0 && lpObj->Inventory[n].IsPentagramItem() != 0)
		{
			for(int i=0;i < MAX_SOCKET_OPTION;i++)
			{
				PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = this->GetPentagramJewelInfo(lpObj,lpObj->Inventory[n].m_SocketOption[i],PENTAGRAM_JEWEL_TYPE_INVENTORY);

				if(lpPentagramJewelInfo != 0)
				{
					memcpy(&info,lpPentagramJewelInfo,sizeof(info));

					info.Type = pMsg.type;

					memcpy(&send[size],&info,sizeof(info));
					size += sizeof(info);

					pMsg.count++;
				}
			}
		}
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send,&pMsg,sizeof(pMsg));

		DataSend(aIndex,send,size);
	}
}

void CPentagramSystem::DGPentagramJewelInfoRecv(SDHP_PENTAGRAM_JEWEL_INFO_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGPentagramJewelInfoRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = ((lpMsg->type==PENTAGRAM_JEWEL_TYPE_INVENTORY)?lpObj->PentagramJewelInfo_Inventory:lpObj->PentagramJewelInfo_Warehouse);

	for(int n=0;n < lpMsg->count;n++)
	{
		SDHP_PENTAGRAM_JEWEL_INFO* lpInfo = (SDHP_PENTAGRAM_JEWEL_INFO*)(((BYTE*)lpMsg)+sizeof(SDHP_PENTAGRAM_JEWEL_INFO_RECV)+(sizeof(SDHP_PENTAGRAM_JEWEL_INFO)*n));

		if(lpInfo->Type != lpMsg->type)
		{
			continue;
		}

		if(lpInfo->Index >= MAX_PENTAGRAM_JEWEL_INFO)
		{
			continue;
		}

		lpPentagramJewelInfo[lpInfo->Index].Type = lpInfo->Type;
		lpPentagramJewelInfo[lpInfo->Index].Index = lpInfo->Index;
		lpPentagramJewelInfo[lpInfo->Index].Attribute = lpInfo->Attribute;
		lpPentagramJewelInfo[lpInfo->Index].ItemSection = lpInfo->ItemSection;
		lpPentagramJewelInfo[lpInfo->Index].ItemType = lpInfo->ItemType;
		lpPentagramJewelInfo[lpInfo->Index].ItemLevel = lpInfo->ItemLevel;
		lpPentagramJewelInfo[lpInfo->Index].OptionIndexRank1 = lpInfo->OptionIndexRank1;
		lpPentagramJewelInfo[lpInfo->Index].OptionLevelRank1 = lpInfo->OptionLevelRank1;
		lpPentagramJewelInfo[lpInfo->Index].OptionIndexRank2 = lpInfo->OptionIndexRank2;
		lpPentagramJewelInfo[lpInfo->Index].OptionLevelRank2 = lpInfo->OptionLevelRank2;
		lpPentagramJewelInfo[lpInfo->Index].OptionIndexRank3 = lpInfo->OptionIndexRank3;
		lpPentagramJewelInfo[lpInfo->Index].OptionLevelRank3 = lpInfo->OptionLevelRank3;
		lpPentagramJewelInfo[lpInfo->Index].OptionIndexRank4 = lpInfo->OptionIndexRank4;
		lpPentagramJewelInfo[lpInfo->Index].OptionLevelRank4 = lpInfo->OptionLevelRank4;
		lpPentagramJewelInfo[lpInfo->Index].OptionIndexRank5 = lpInfo->OptionIndexRank5;
		lpPentagramJewelInfo[lpInfo->Index].OptionLevelRank5 = lpInfo->OptionLevelRank5;
	}

	this->GCPentagramJewelInfoSend(lpObj->Index,lpMsg->type);

	gObjectManager.CharacterCalcAttribute(lpObj->Index);
}

void CPentagramSystem::GDPentagramJewelInfoSend(int aIndex,int type) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_PENTAGRAM_JEWEL_INFO_SEND pMsg;

	pMsg.header.set(0x23,0x00,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	pMsg.type = type;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CPentagramSystem::GDPentagramJewelInfoSaveSend(int aIndex,int type) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	LPOBJ lpObj = &gObj[aIndex];

	PENTAGRAM_JEWEL_INFO* lpPentagramJewelInfo = ((type==PENTAGRAM_JEWEL_TYPE_INVENTORY)?lpObj->PentagramJewelInfo_Inventory:lpObj->PentagramJewelInfo_Warehouse);

	BYTE send[8192];

	SDHP_PENTAGRAM_JEWEL_INFO_SAVE_SEND pMsg;

	pMsg.header.set(0x23,0x30,0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.count = 0;

	SDHP_PENTAGRAM_JEWEL_INFO_SAVE info;

	for(int n=0;n < MAX_PENTAGRAM_JEWEL_INFO;n++)
	{
		if(lpPentagramJewelInfo[n].Index != 0xFF)
		{
			info.Type = lpPentagramJewelInfo[n].Type;
			info.Index = lpPentagramJewelInfo[n].Index;
			info.Attribute = lpPentagramJewelInfo[n].Attribute;
			info.ItemSection = lpPentagramJewelInfo[n].ItemSection;
			info.ItemType = lpPentagramJewelInfo[n].ItemType;
			info.ItemLevel = lpPentagramJewelInfo[n].ItemLevel;
			info.OptionIndexRank1 = lpPentagramJewelInfo[n].OptionIndexRank1;
			info.OptionLevelRank1 = lpPentagramJewelInfo[n].OptionLevelRank1;
			info.OptionIndexRank2 = lpPentagramJewelInfo[n].OptionIndexRank2;
			info.OptionLevelRank2 = lpPentagramJewelInfo[n].OptionLevelRank2;
			info.OptionIndexRank3 = lpPentagramJewelInfo[n].OptionIndexRank3;
			info.OptionLevelRank3 = lpPentagramJewelInfo[n].OptionLevelRank3;
			info.OptionIndexRank4 = lpPentagramJewelInfo[n].OptionIndexRank4;
			info.OptionLevelRank4 = lpPentagramJewelInfo[n].OptionLevelRank4;
			info.OptionIndexRank5 = lpPentagramJewelInfo[n].OptionIndexRank5;
			info.OptionLevelRank5 = lpPentagramJewelInfo[n].OptionLevelRank5;

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send,&pMsg,sizeof(pMsg));

		gDataServerConnection.DataSend(send,size);
	}

	#endif
}

void CPentagramSystem::GDPentagramJewelInsertSaveSend(int aIndex,PENTAGRAM_JEWEL_INFO* lpInfo) // OK
{
	SDHP_PENTAGRAM_JEWEL_INSERT_SAVE_SEND pMsg;
	pMsg.header.set(0x23,0x31,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));
	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.account));

	pMsg.Type = lpInfo->Type;
	pMsg.Index = lpInfo->Index;
	pMsg.Attribute = lpInfo->Attribute;
	pMsg.ItemSection = lpInfo->ItemSection;
	pMsg.ItemType = lpInfo->ItemType;
	pMsg.ItemLevel = lpInfo->ItemLevel;
	pMsg.OptionIndexRank1 = lpInfo->OptionIndexRank1;
	pMsg.OptionLevelRank1 = lpInfo->OptionLevelRank1;
	pMsg.OptionIndexRank2 = lpInfo->OptionIndexRank2;
	pMsg.OptionLevelRank2 = lpInfo->OptionLevelRank2;
	pMsg.OptionIndexRank3 = lpInfo->OptionIndexRank3;
	pMsg.OptionLevelRank3 = lpInfo->OptionLevelRank3;
	pMsg.OptionIndexRank4 = lpInfo->OptionIndexRank4;
	pMsg.OptionLevelRank4 = lpInfo->OptionLevelRank4;
	pMsg.OptionIndexRank5 = lpInfo->OptionIndexRank5;
	pMsg.OptionLevelRank5 = lpInfo->OptionLevelRank5;

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CPentagramSystem::GDPentagramJewelDeleteSaveSend(int aIndex,int type,int index) // OK
{
	SDHP_PENTAGRAM_JEWEL_DELETE_SAVE_SEND pMsg;

	pMsg.header.set(0x23,0x32,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));
	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.account));

	pMsg.Type = type;
	pMsg.Index = index;

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CPentagramSystem::GCPentagramMixSend(int aIndex,BYTE type)
{
	PMSG_PENTAGRAM_MIX_SEND pMsg;
	pMsg.header.set(0xEC,0x02,sizeof(pMsg));
	pMsg.type = type;
	DataSend(aIndex,(BYTE*)&pMsg,sizeof(pMsg));
}

void CPentagramSystem::CGPentagramEcstasySlotRecv(PMSG_PENTAGRAM_ECSTASY_SLOT_RECV *lpMsg,int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Inventory[lpMsg->Element].IsPentagramItem() == 0 ||lpObj->Inventory[lpMsg->EcstasySlot].m_Index != GET_ITEM(14,316))
	{
		return;
	}

	CItem item = lpObj->Inventory[lpMsg->Element];

	item.m_SocketOption[4] = 0xFE;

	gItemManager.InventoryDelItem(lpObj->Index,lpMsg->EcstasySlot);

	gItemManager.GCItemDeleteSend(lpObj->Index,lpMsg->EcstasySlot,1);

	gItemManager.InventoryDelItem(lpObj->Index,lpMsg->Element);

	gItemManager.GCItemDeleteSend(lpObj->Index,lpMsg->Element,1);

	BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index,item);

	PMSG_RHEA_ITEM_BUY_SEND pMsg;

	pMsg.header.setE(0xF3,0x14,sizeof(pMsg));

	pMsg.result = Result;

	gItemManager.ItemByteConvert(pMsg.ItemInfo,item);

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CPentagramSystem::CGWingSocketOptionAdd(PMSG_WING4_OPTION_ADD* lpMsg, int aIndex)
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
	lpObj->IsChaosMixCompleted = 1;

	if (!lpObj->Inventory[lpMsg->WingSlot].IsItem() || lpObj->Inventory[lpMsg->WingSlot].m_Index < GET_ITEM(12, 414) || lpObj->Inventory[lpMsg->WingSlot].m_Index > GET_ITEM(12, 421))
	{
		LogAdd(LOG_RED, "[%s][%s] Trying to add 4th Wing option in a no 4th Wing item [%d]", lpObj->Account, lpObj->Name, lpObj->Inventory[lpMsg->WingSlot].m_Index);
		return;
	}

	if (!lpObj->Inventory[lpMsg->ErrtelSlot].IsItem() || lpObj->Inventory[lpMsg->ErrtelSlot].IsPentagramJewel() == 0 || lpObj->Inventory[lpMsg->ErrtelSlot].m_SocketOption[0] < 64 )
	{
		LogAdd(LOG_RED, "[%s][%s] Trying to add 4th Wing option with a non errtel+4 item [%d][%d]", lpObj->Account, lpObj->Name, lpObj->Inventory[lpMsg->ErrtelSlot].m_Index, lpObj->Inventory[lpMsg->ErrtelSlot].m_SocketOption[0]);
		return;
	}

	int GoldenCrestCount = 0;

	for (int i = 0; i < 9; i++)
	{
		if (lpMsg->GoldenCrestSlot[i] != 0xFF && lpObj->Inventory[lpMsg->GoldenCrestSlot[i]].IsItem() && lpObj->Inventory[lpMsg->GoldenCrestSlot[i]].m_Index == GET_ITEM(13,468))
		{
			GoldenCrestCount += lpObj->Inventory[lpMsg->GoldenCrestSlot[i]].m_Durability;
		}
	}

	if (GoldenCrestCount < 9)
	{
		LogAdd(LOG_RED, "[%s][%s] Trying to add 4th Wing option with less than 9 Golden Crest [%d]", lpObj->Account, lpObj->Name, GoldenCrestCount);
		return;
	}

	int MagicStoneCount = 0;

	for (int i = 0; i < 3; i++)
	{
		if (lpMsg->MagicStoneSlot[i] != 0xFF && lpObj->Inventory[lpMsg->MagicStoneSlot[i]].IsItem() && lpObj->Inventory[lpMsg->MagicStoneSlot[i]].m_Index == GET_ITEM(12, 406))
		{
			MagicStoneCount += lpObj->Inventory[lpMsg->MagicStoneSlot[i]].m_Durability;
		}
	}

	if (MagicStoneCount < 3)
	{
		LogAdd(LOG_RED, "[%s][%s] Trying to add 4th Wing option with less than 3 Higher Magic Stone [%d]", lpObj->Account, lpObj->Name, MagicStoneCount);
		return;
	}

	int GoldenCrestToRemove = 9;

	for (int i = 0; i < 9; i++)
	{
		if (lpMsg->GoldenCrestSlot[i] != 0xFF && lpObj->Inventory[lpMsg->GoldenCrestSlot[i]].IsItem() && lpObj->Inventory[lpMsg->GoldenCrestSlot[i]].m_Index == GET_ITEM(13, 468))
		{
			if (lpObj->Inventory[lpMsg->GoldenCrestSlot[i]].m_Durability < GoldenCrestToRemove)
			{
				gItemManager.DecreaseItemDur(lpObj, lpMsg->GoldenCrestSlot[i], lpObj->Inventory[lpMsg->GoldenCrestSlot[i]].m_Durability);
				GoldenCrestToRemove -= lpObj->Inventory[lpMsg->GoldenCrestSlot[i]].m_Durability;
			}
			else
			{
				gItemManager.DecreaseItemDur(lpObj, lpMsg->GoldenCrestSlot[i], GoldenCrestToRemove);
				GoldenCrestToRemove = 0;
			}
		}

		if (GoldenCrestToRemove <= 0)
		{
			break;
		}
	}

	int MagicStoneToRemove = 3;

	for (int i = 0; i < 3; i++)
	{
		if (lpMsg->MagicStoneSlot[i] != 0xFF && lpObj->Inventory[lpMsg->MagicStoneSlot[i]].IsItem() && lpObj->Inventory[lpMsg->MagicStoneSlot[i]].m_Index == GET_ITEM(12, 406))
		{
			if (lpObj->Inventory[lpMsg->MagicStoneSlot[i]].m_Durability < MagicStoneToRemove)
			{
				gItemManager.DecreaseItemDur(lpObj, lpMsg->MagicStoneSlot[i], lpObj->Inventory[lpMsg->MagicStoneSlot[i]].m_Durability);
				MagicStoneToRemove -= lpObj->Inventory[lpMsg->MagicStoneSlot[i]].m_Durability;
			}
			else
			{
				gItemManager.DecreaseItemDur(lpObj, lpMsg->MagicStoneSlot[i], MagicStoneToRemove);
				MagicStoneToRemove = 0;
			}
		}

		if (MagicStoneToRemove <= 0)
		{
			break;
		}
	}

	int WingSocketOptionCreateRate[11] = { 1, 2, 3, 5, 15, 25, 40, 50, 60, 75, 90 };
	int ErrtelLevel = lpObj->Inventory[lpMsg->ErrtelSlot].m_SocketOption[0] / 16;

	lpObj->ChaosSuccessRate = WingSocketOptionCreateRate[ErrtelLevel];

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->ErrtelSlot);
	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->ErrtelSlot, 1);	

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem itemOld = lpObj->Inventory[lpMsg->WingSlot];

		lpObj->Inventory[lpMsg->WingSlot].m_SocketOptionBonus = 0x00;

		int ElementalOption = 0xFF;

		CRandomManager RandomManager;

		RandomManager.AddElement(0 * 16, 13);
		RandomManager.AddElement(1 * 16, 13);
		RandomManager.AddElement(2 * 16, 13);
		RandomManager.AddElement(3 * 16, 8);
		RandomManager.AddElement(4 * 16, 13);
		RandomManager.AddElement(5 * 16, 8);
		RandomManager.AddElement(6 * 16, 8);
		RandomManager.AddElement(255, 24);

		RandomManager.GetRandomElement(&ElementalOption);

		lpObj->Inventory[lpMsg->WingSlot].m_SocketOption[4] = ElementalOption;

		gItemManager.GCItemModifySend(aIndex, lpMsg->WingSlot);

		gPentagramSystem.GCPentagramMixSend(lpObj->Index, 1);

		gLog.Output(LOG_CHAOS_MIX, "[Wing4thAddElemental][Success][%s][%s] - (ChaosSuccessRate: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate);

		GDItemTrackingSaveSend(itemOld, lpObj->Inventory[lpMsg->WingSlot], lpObj->Account, lpObj->Account, 4, 2);
	}
	else
	{
		gLog.Output(LOG_CHAOS_MIX, "[Wing4thAddElemental][Failure][%s][%s] - (ChaosSuccessRate: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate);
		gPentagramSystem.GCPentagramMixSend(lpObj->Index, 0);
	}

	lpObj->ChaosLock = 0;
}


void CPentagramSystem::CGWingSocketOptionUpgrade(PMSG_WING4_UPGRADE_RECV* lpMsg, int aIndex)
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
	lpObj->IsChaosMixCompleted = 1;
	bool ElementalTalisman = false;

	if (!lpObj->Inventory[lpMsg->WingSlot].IsItem() || lpObj->Inventory[lpMsg->WingSlot].m_Index < GET_ITEM(12, 414) || lpObj->Inventory[lpMsg->WingSlot].m_Index > GET_ITEM(12, 421))
	{
		LogAdd(LOG_RED, "[%s][%s] Trying to upgrade 4th Wing option in a no 4th Wing item [%d]", lpObj->Account, lpObj->Name, lpObj->Inventory[lpMsg->WingSlot].m_Index);
		return;
	}

	if (lpObj->Inventory[lpMsg->WingSlot].m_SocketOptionBonus == 0xFF)
	{
		LogAdd(LOG_RED, "[%s][%s] Trying to upgrade 4th Wing option in a no elemental 4th Wing [%d]", lpObj->Account, lpObj->Name, lpObj->Inventory[lpMsg->WingSlot].m_SocketOptionBonus);
		return;
	}

	int WingLevel = lpObj->Inventory[lpMsg->WingSlot].m_SocketOptionBonus;

	if (WingLevel > 14)
	{
		LogAdd(LOG_RED, "[%s][%s] Trying to upgrade 4th Wing option already +15 [%d]", lpObj->Account, lpObj->Name);
		return;
	}

	int RuneCount = 0;

	for (int i = 0; i < 9; i++)
	{
		if (lpMsg->RuneSlot[i] != 0xFF && lpObj->Inventory[lpMsg->RuneSlot[i]].IsItem() && lpObj->Inventory[lpMsg->RuneSlot[i]].m_Index == GET_ITEM(12, 288))
		{
			RuneCount += lpObj->Inventory[lpMsg->RuneSlot[i]].m_Durability;
		}
	}

	int RuneNeeded[15] = { 1,1,1,2,3,3,4,5,6,7,8,9,10,11,12 };

	if (RuneCount < RuneNeeded[WingLevel])
	{
		LogAdd(LOG_RED, "[%s][%s] Trying to upgrade 4th Wing option (%d) with less than %d Elemental Runes [%d]", lpObj->Account, lpObj->Name, WingLevel, RuneNeeded[WingLevel], RuneCount);
		return;
	}

	if (lpMsg->TalismanSlot != 0xFF)
	{
		if (!lpObj->Inventory[lpMsg->TalismanSlot].IsItem() || lpObj->Inventory[lpMsg->TalismanSlot].m_Index != GET_ITEM(14, 189))
		{
			LogAdd(LOG_RED, "[%s][%s] Trying to add 4th Wing option with a non talisman item [%d]", lpObj->Account, lpObj->Name, lpObj->Inventory[lpMsg->TalismanSlot].m_Index);
			return;
		}
		else
		{
			ElementalTalisman = true;
		}
	}

	int RuneToRemove = RuneNeeded[WingLevel];

	for (int i = 0; i < 9; i++)
	{
		if (lpMsg->RuneSlot[i] != 0xFF && lpObj->Inventory[lpMsg->RuneSlot[i]].IsItem() && lpObj->Inventory[lpMsg->RuneSlot[i]].m_Index == GET_ITEM(12, 288))
		{
			if (lpObj->Inventory[lpMsg->RuneSlot[i]].m_Durability < RuneToRemove)
			{
				gItemManager.DecreaseItemDur(lpObj, lpMsg->RuneSlot[i], lpObj->Inventory[lpMsg->RuneSlot[i]].m_Durability);
				RuneToRemove -= lpObj->Inventory[lpMsg->RuneSlot[i]].m_Durability;
			}
			else
			{
				gItemManager.DecreaseItemDur(lpObj, lpMsg->RuneSlot[i], RuneToRemove);
				RuneToRemove = 0;
			}
		}

		if (RuneToRemove <= 0)
		{
			break;
		}
	}

	int WingSocketOptionUpgradeRate[14] = { 50, 50, 50, 45, 45, 45, 45, 40, 40, 35, 35, 35, 30, 30 };

	lpObj->ChaosSuccessRate = WingSocketOptionUpgradeRate[WingLevel];
	
	if (ElementalTalisman)
	{
		lpObj->ChaosSuccessRate += 10;
		gItemManager.DecreaseItemDur(lpObj, lpMsg->TalismanSlot, 1);
	}

	CItem itemOld = lpObj->Inventory[lpMsg->WingSlot];

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		lpObj->Inventory[lpMsg->WingSlot].m_SocketOptionBonus++;

		if(lpObj->Inventory[lpMsg->WingSlot].m_SocketOption[4] != 0xFF)
		{
			lpObj->Inventory[lpMsg->WingSlot].m_SocketOption[4]++;
		}		

		gItemManager.GCItemModifySend(aIndex, lpMsg->WingSlot);
		gPentagramSystem.GCPentagramMixSend(lpObj->Index, 1);
		gLog.Output(LOG_CHAOS_MIX, "[Wing4thElementalUpgrade][Success][%s][%s] - (ChaosSuccessRate: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate);
	}
	else
	{
		if (WingLevel > 0)
		{
			lpObj->Inventory[lpMsg->WingSlot].m_SocketOptionBonus--;
			lpObj->Inventory[lpMsg->WingSlot].m_SocketOption[4]--;
			gItemManager.GCItemModifySend(aIndex, lpMsg->WingSlot);
		}

		gLog.Output(LOG_CHAOS_MIX, "[Wing4thElementalUpgrade][Failure][%s][%s] - (ChaosSuccessRate: %d)", lpObj->Account, lpObj->Name, lpObj->ChaosSuccessRate);
		gPentagramSystem.GCPentagramMixSend(lpObj->Index, 0);
	}

	GDItemTrackingSaveSend(itemOld, lpObj->Inventory[lpMsg->WingSlot], lpObj->Account, lpObj->Account, 4, 3);

	lpObj->ChaosLock = 0;
}