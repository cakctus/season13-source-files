#include "stdafx.h"
#include "ItemBagEx.h"
#include "RheaGold.h"
#include "DSProtocol.h"
#include "ItemOptionRate.h"
#include "Map.h"
#include "MemScript.h"
#include "Party.h"
#include "RandomManager.h"
#include "Notice.h"
#include "Message.h"
#include "Util.h"
#include "MuunSystem.h"
#include "Log.h"
#include "Monster.h"
#include "CustomMonster.h"
#include "JSProtocol.h"
#include "ItemStack.h"

CItemBagEx::CItemBagEx() // OK
{
	this->m_ItemBagInfo.clear();
	this->m_ItemBagItemInfo.clear();
	this->isLoaded = false;
}

CItemBagEx::~CItemBagEx() // OK
{

}

void CItemBagEx::Load(char* path) // OK
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

	this->isLoaded = false;

	this->m_ItemBagInfo.clear();
	this->m_ItemBagItemInfo.clear();

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
				if (section == 3)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_EX_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.DropRate = lpMemScript->GetAsNumber();

					this->m_ItemBagInfo.insert(std::pair<int, ITEM_BAG_EX_INFO>(info.Index, info));
				}
				else if (section == 4)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_EX_DROP_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.Section = lpMemScript->GetAsNumber();

					info.SectionRate = lpMemScript->GetAsNumber();

					info.MoneyAmount = lpMemScript->GetAsNumber();

					info.OptionValue = lpMemScript->GetAsNumber();

					for (int n = 0; n < MAX_CLASS; n++) { info.RequireClass[n] = lpMemScript->GetAsNumber(); }

					std::map<int, ITEM_BAG_EX_INFO>::iterator it = this->m_ItemBagInfo.find(info.Index);

					if (it != this->m_ItemBagInfo.end())
					{
						it->second.DropInfo.push_back(info);
					}
				}
				else if ((section >= 5 && section <= 9) || (section >= 12 && section <= 19))
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_EX_ITEM_INFO info;

					info.Index = lpMemScript->GetNumber();
					info.Level = lpMemScript->GetAsNumber();
					info.Grade = lpMemScript->GetAsNumber();
					info.Option0 = lpMemScript->GetAsNumber();
					info.Option1 = lpMemScript->GetAsNumber();
					info.Option2 = lpMemScript->GetAsNumber();
					info.Option3 = lpMemScript->GetAsNumber();
					info.Option4 = lpMemScript->GetAsNumber();
					info.Option5 = lpMemScript->GetAsNumber();
					info.Option6 = lpMemScript->GetAsNumber();
					info.Duration = lpMemScript->GetAsNumber();

					std::map<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>::iterator it = this->m_ItemBagItemInfo.find(section);

					if (it == this->m_ItemBagItemInfo.end())
					{
						this->m_ItemBagItemInfo.insert(std::pair<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>(section, std::vector<ITEM_BAG_EX_ITEM_INFO>(1, info)));
					}
					else
					{
						it->second.push_back(info);
					}

				}
				else if (section >= 10 && section <= 11) //ELEMENTAL SHIT
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_ELEMENT_ITEM_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.Level = lpMemScript->GetAsNumber();

					info.Element = lpMemScript->GetAsNumber();

					info.Dur = lpMemScript->GetAsNumber();

					info.Socket[0] = lpMemScript->GetAsNumber();
					info.Socket[1] = lpMemScript->GetAsNumber();
					info.Socket[2] = lpMemScript->GetAsNumber();
					info.Socket[3] = lpMemScript->GetAsNumber();
					info.Socket[4] = lpMemScript->GetAsNumber();

					std::map<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>::iterator it = this->m_ItemElementBagItemInfo.find(section);

					if (it == this->m_ItemElementBagItemInfo.end())
					{
						this->m_ItemElementBagItemInfo.insert(std::pair<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>(section, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>(1, info)));
					}
					else
					{
						it->second.push_back(info);
					}
				}
				else if (section == 20) //BUFF EFFECTS
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_ELEMENT_ITEM_INFO info;

					info.Index = lpMemScript->GetNumber();
					info.Dur = lpMemScript->GetAsNumber();

					std::map<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>::iterator it = this->m_ItemElementBagItemInfo.find(section);

					if (it == this->m_ItemElementBagItemInfo.end())
					{
						this->m_ItemElementBagItemInfo.insert(std::pair<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>(section, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>(1, info)));
					}
					else
					{
						it->second.push_back(info);
					}
				}
				else
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

	this->isLoaded = true;
}

bool CItemBagEx::GetItem(LPOBJ lpObj, CItem* lpItem) // OK PORRA DO SENIOR É AQUI, PARECE
{
	if (!this->isLoaded)
	{
		return false;
	}

	for (std::map<int, ITEM_BAG_EX_INFO>::iterator it = this->m_ItemBagInfo.begin(); it != this->m_ItemBagInfo.end(); it++)
	{
		if ((GetLargeRand() % 10000) < it->second.DropRate)
		{
			CRandomManager RandomManager;

			for (std::vector<ITEM_BAG_EX_DROP_INFO>::iterator DropInfo = it->second.DropInfo.begin(); DropInfo != it->second.DropInfo.end(); DropInfo++)
			{
				if (this->CheckDropClass(lpObj, DropInfo->RequireClass) != 0)
				{
					RandomManager.AddElement((int)(&(*DropInfo)), DropInfo->SectionRate);
				}
			}

			ITEM_BAG_EX_DROP_INFO* lpItemBagDropInfo = 0;

			if (RandomManager.GetRandomElement((int*)&lpItemBagDropInfo) == 0)
			{
				return 0;
			}

			std::map<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>::iterator ItemInfo = this->m_ItemBagItemInfo.find(lpItemBagDropInfo->Section);

			if (ItemInfo == this->m_ItemBagItemInfo.end())
			{
				return 0;
			}

			if (ItemInfo->second.empty() != 0)
			{
				return 0;
			}

			ITEM_BAG_EX_ITEM_INFO* lpItemBagItemInfo = &ItemInfo->second[GetLargeRand() % ItemInfo->second.size()];

			WORD ItemIndex = lpItemBagItemInfo->Index;
			BYTE ItemLevel = lpItemBagItemInfo->Level;
			BYTE ItemOption1 = 0;
			BYTE ItemOption2 = 0;
			BYTE ItemOption3 = 0;
			BYTE ItemNewOption = 0;
			BYTE ItemSetOption = 0;
			BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

			gItemOptionRate.GetItemOption0(lpItemBagItemInfo->Option0, &ItemLevel);
			gItemOptionRate.GetItemOption1(lpItemBagItemInfo->Option1, &ItemOption1);
			gItemOptionRate.GetItemOption2(lpItemBagItemInfo->Option2, &ItemOption2);
			gItemOptionRate.GetItemOption3(lpItemBagItemInfo->Option3, &ItemOption3);
			gItemOptionRate.GetItemOption4(lpItemBagItemInfo->Option4, &ItemNewOption);
			gItemOptionRate.GetItemOption5(lpItemBagItemInfo->Option5, &ItemSetOption);
			gItemOptionRate.GetItemOption6(lpItemBagItemInfo->Option6, &ItemSocketOption[0]);

			gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
			gItemOptionRate.MakeSetOption(ItemIndex, ItemSetOption, &ItemSetOption);
			gItemOptionRate.MakeSocketOption(ItemIndex, ItemSocketOption[0], &ItemSocketOption[0]);

			lpItem->m_Level = ItemLevel;

			lpItem->Convert(ItemIndex, ItemOption1, ItemOption2, ItemOption3, ((ItemNewOption == 0) ? lpItemBagItemInfo->Grade : ItemNewOption), ItemSetOption, 0, 0, ItemSocketOption, 0xFF);

			return 1;
		}
	}

	return 0;
}

bool CItemBagEx::DropItem(LPOBJ lpObj, int map, int x, int y, int MonsterIndex) // OK
{
	if (!this->isLoaded)
	{
		return false;
	}

	int ElementTypeForMultiple = -1;
	int counter = 0;
	int sortedCount = 0;

	LPOBJ lpMonster = NULL;

	if (MonsterIndex > -1)
	{
		lpMonster = &gObj[MonsterIndex];
		sortedCount = gObjMonsterGetTopHitDamageUser(lpMonster, MAX_HIT_DAMAGE);
	}

	CRandomManager PlayerRandom;
	int Sortudo = 0;

	if (MonsterIndex > -1)
	{
		for (int i = 0; i < sortedCount; i++)
		{
			if (lpMonster->Map != gObj[lpMonster->HitDamage[i].index].Map || gObjCalcDistance(lpMonster, &gObj[lpMonster->HitDamage[i].index]) >= 12)
			{
				continue;
			}

			if ((gObj[lpMonster->HitDamage[i].index].BossDropIncrease + gObj[lpMonster->HitDamage[i].index].EffectOption.BossDropIncrease) > 0)
			{
				int addDamage = (int)((float)(lpMonster->MaxLife / 10000.0f) * (float)(gObj[lpMonster->HitDamage[i].index].BossDropIncrease + gObj[lpMonster->HitDamage[i].index].EffectOption.BossDropIncrease) / 10000.0f);
				int luckyNumber = (int)((float)lpMonster->HitDamage[i].damage / 10000.0f) + addDamage;

				if (gObj[lpMonster->HitDamage[i].index].AccountLevel < 1)
				{
					luckyNumber = (int)((float)luckyNumber / 2.0f);
				}

				PlayerRandom.AddElement(i, luckyNumber);
			}
			else
			{
				int luckyNumber = (int)((float)lpMonster->HitDamage[i].damage / 10000.0f);

				if (gObj[lpMonster->HitDamage[i].index].AccountLevel < 1)
				{
					luckyNumber = (int)((float)luckyNumber / 2.0f);
				}

				PlayerRandom.AddElement(i, luckyNumber);
			}
		}
	}

	for (std::map<int, ITEM_BAG_EX_INFO>::iterator it = this->m_ItemBagInfo.begin(); it != this->m_ItemBagInfo.end(); it++)
	{
		if ((GetLargeRand() % 10000) < it->second.DropRate)
		{
			LPOBJ lpObjRewarded;
			int chance = 100;

			if (counter == 0 && MonsterIndex > -1 && gObjCalcDistance(lpMonster, &gObj[lpMonster->HitDamage[0].index]) < 10)
			{
				lpObjRewarded = &gObj[lpMonster->HitDamage[0].index];
			}
			else if (counter > 0 && MonsterIndex > -1)
			{
				PlayerRandom.GetRandomElementWithPerc(&Sortudo, &chance);
				lpObjRewarded = &gObj[lpMonster->HitDamage[Sortudo].index];
			}
			else
			{
				lpObjRewarded = &gObj[lpObj->Index];
			}

			counter++;

			CRandomManager RandomManager;

			for (std::vector<ITEM_BAG_EX_DROP_INFO>::iterator DropInfo = it->second.DropInfo.begin(); DropInfo != it->second.DropInfo.end(); DropInfo++)
			{
				if (this->CheckDropClass(lpObjRewarded, DropInfo->RequireClass) != 0)
				{
					RandomManager.AddElement((int)(&(*DropInfo)), DropInfo->SectionRate);
				}
			}

			ITEM_BAG_EX_DROP_INFO* lpItemBagDropInfo = 0;

			if (it->second.DropInfo[0].Section == 20) //Buff Effect
			{
				std::map<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>::iterator ItemInfo = this->m_ItemElementBagItemInfo.find(20);

				if ((it->second.DropInfo[0].OptionValue & 4) != 0)
				{
					for (int buffIndex = 0; buffIndex < ItemInfo->second.size(); buffIndex++)
					{
						ITEM_BAG_ELEMENT_ITEM_INFO* lpItemElementBagItemInfo = &ItemInfo->second[buffIndex];
						WORD EffectIndex = lpItemElementBagItemInfo->Index;
						DWORD EffectDuration = lpItemElementBagItemInfo->Dur;

						char timeString[8];

						if ((EffectDuration / 60) >= 60)
						{
							wsprintf(timeString, "%dh", EffectDuration / 60 / 60);
						}
						else
						{
							wsprintf(timeString, "%dm", EffectDuration / 60);
						}

						EFFECT_INFO* lpEffectInfo = gEffectManager.GetInfo(EffectIndex);

						if (!gEffectManager.CheckEffect(lpObjRewarded, EffectIndex))
						{
							gEffectManager.AddEffect(lpObjRewarded, 1, EffectIndex, (int)(time(0) + EffectDuration), 0, 0, 0, 0);
						}
						else
						{
							CEffect* lpEffect = gEffectManager.GetEffect(lpObjRewarded, EffectIndex);
							gEffectManager.AddEffect(lpObjRewarded, 1, EffectIndex, (int)(lpEffect->m_time + EffectDuration), 0, 0, 0, 0);
						}
						gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [All buffs] [Effect: %s +%s]", lpObjRewarded->Account, lpObjRewarded->Name, lpEffectInfo->Name, timeString);
					}
				}
				else
				{
					if (RandomManager.GetRandomElement((int*)&lpItemBagDropInfo) == 0)
					{
						continue;
					}

					ITEM_BAG_ELEMENT_ITEM_INFO* lpItemElementBagItemInfo = &ItemInfo->second[GetLargeRand() % ItemInfo->second.size()];

					WORD EffectIndex = lpItemElementBagItemInfo->Index;
					DWORD EffectDuration = lpItemElementBagItemInfo->Dur;

					char timeString[8];

					if ((EffectDuration / 60) >= 60)
					{
						wsprintf(timeString, "%dh", EffectDuration / 60 / 60);
					}
					else
					{
						wsprintf(timeString, "%dm", EffectDuration / 60);
					}

					EFFECT_INFO* lpEffectInfo = gEffectManager.GetInfo(EffectIndex);

					if (!gEffectManager.CheckEffect(lpObjRewarded, EffectIndex))
					{
						gEffectManager.AddEffect(lpObjRewarded, 1, EffectIndex, (int)(time(0) + EffectDuration), 0, 0, 0, 0);
					}
					else
					{
						CEffect* lpEffect = gEffectManager.GetEffect(lpObjRewarded, EffectIndex);
						gEffectManager.AddEffect(lpObjRewarded, 1, EffectIndex, (int)(lpEffect->m_time + EffectDuration), 0, 0, 0, 0);
					}

					gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [Section: %d] [Effect: %s +%s]", lpObjRewarded->Account, lpObjRewarded->Name, it->second.DropInfo[0].Section, lpEffectInfo->Name, timeString);
					gNotice.SendMapServerGroupMsg(1, "[%s] drop buff %s de %s", lpObjRewarded->Name, lpEffectInfo->Name, timeString);
				}
			}
			else if (it->second.DropInfo[0].Section >= 10 && it->second.DropInfo[0].Section <= 11)
			{
				if (RandomManager.GetRandomElement((int*)&lpItemBagDropInfo) == 0)
				{
					continue;
				}

				int px = x;
				int py = y;

				if (it != this->m_ItemBagInfo.begin() || (gMap[map].CheckAttr(px, py, 4) != 0 || gMap[map].CheckAttr(px, py, 8) != 0))
				{
					if (this->GetRandomItemDropLocation(map, &px, &py, 2, 2, 10) == 0)
					{
						px = lpObjRewarded->X;
						py = lpObjRewarded->Y;
					}
				}

				std::map<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>::iterator ItemInfo = this->m_ItemElementBagItemInfo.find(lpItemBagDropInfo->Section);

				if (ItemInfo == this->m_ItemElementBagItemInfo.end())
				{
					gMap[map].MoneyItemDrop(lpItemBagDropInfo->MoneyAmount, px, py);
					continue;
				}

				if (ItemInfo->second.empty() != 0)
				{
					gMap[map].MoneyItemDrop(lpItemBagDropInfo->MoneyAmount, px, py);
					continue;
				}

				ITEM_BAG_ELEMENT_ITEM_INFO* lpItemElementBagItemInfo = &ItemInfo->second[GetLargeRand() % ItemInfo->second.size()];

				WORD ItemIndex = lpItemElementBagItemInfo->Index;
				BYTE ItemLevel = lpItemElementBagItemInfo->Level;
				BYTE ItemElement;

				if (ElementTypeForMultiple == -1 && it->second.DropInfo[0].Section == 10)
				{
					ItemElement = lpItemElementBagItemInfo->Element | 16;
				}
				else
				{
					ItemElement = ElementTypeForMultiple;
				}

				if (it->second.DropInfo[0].Section == 10)
				{
					ElementTypeForMultiple = ItemElement;
				}

				BYTE ItemDur = lpItemElementBagItemInfo->Dur;
				BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

				memcpy(ItemSocketOption, lpItemElementBagItemInfo->Socket, sizeof(lpItemElementBagItemInfo->Socket));

				gPentagramSystem.GetPentagramRandomJewelOption(ItemIndex, 1, &ItemSocketOption[0]);

				if (lpObjRewarded->MouseItemUp == 1)
				{
					ITEM_INFO ItemInfo;

					if (gItemManager.GetInfo(ItemIndex, &ItemInfo) == 0)
					{
						return 0;
					}

					CItem item;
					item.Clear();
					item.m_Index = ItemIndex;
					item.m_Level = ItemLevel;
					item.m_SocketOptionBonus = ItemElement;
					memcpy(item.m_SocketOption, ItemSocketOption, sizeof(item.m_SocketOption));
					item.m_Slot = ItemInfo.Slot;
					item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());

					gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [Section: %d] [Item: %s +%d] [Element: %d]", lpObjRewarded->Account, lpObjRewarded->Name, it->second.DropInfo[0].Section, gItemManager.GetName(ItemIndex), ItemLevel, ItemElement);

					gRheaGold.RheaItemSend(lpObjRewarded->Index, item, gItemManager.InventoryInsertItem(lpObjRewarded->Index, item));
				}
				else
				{
					char itemName[64];
					strcpy_s(itemName, gItemManager.GetName(ItemIndex));
					gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [Section: %d] [Item: %s +%d]", lpObjRewarded->Account, lpObjRewarded->Name, it->second.DropInfo[0].Section, itemName, ItemLevel);

					if (MonsterIndex > -1)
					{
						GDCreateItemSend(lpObjRewarded->Index, 0xEB, 0, 0, ItemIndex, ItemLevel, ItemDur, 0, 0, 0, lpObjRewarded->Index, 0, 0, 0, 0, ItemSocketOption, ItemElement, 0);
						gNotice.SendMapServerGroupMsg(1, "[%s] drop %s de %s (Chance: %d%%)", lpObjRewarded->Name, itemName, lpMonster->MonsterName, chance);
					}
					else
					{
						GDCreateItemSend(lpObjRewarded->Index, map, px, py, ItemIndex, ItemLevel, ItemDur, 0, 0, 0, lpObjRewarded->Index, 0, 0, 0, 0, ItemSocketOption, ItemElement, 0);
					}
				}

				if ((lpItemBagDropInfo->OptionValue & 1) != 0)
				{
					GCFireworksSend(lpObjRewarded, lpObjRewarded->X, lpObjRewarded->Y);
				}
			} //end if sections 10 and 11 (elemental shit)
			else
			{
				std::map<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>::iterator ItemInfo = this->m_ItemBagItemInfo.find(it->second.DropInfo[0].Section);

				if ((it->second.DropInfo[0].OptionValue & 4) != 0 && this->CheckDropClass(lpObjRewarded, it->second.DropInfo[0].RequireClass) != 0) //todos os itens da lista
				{
					for (int buffIndex = 0; buffIndex < ItemInfo->second.size(); buffIndex++)
					{
						ITEM_BAG_EX_ITEM_INFO* lpItemBagItemInfo = &ItemInfo->second[buffIndex];

						WORD ItemIndex = lpItemBagItemInfo->Index;
						BYTE ItemLevel = lpItemBagItemInfo->Level;

						if ((it->second.DropInfo[0].OptionValue & 8) != 0) //pegar sockets
						{
							BYTE ItemNewOption = lpItemBagItemInfo->Grade;
							BYTE ItemSetOption = lpItemBagItemInfo->Option0;

							BYTE ItemOption1 = 1;
							BYTE ItemOption2 = 1;
							BYTE ItemOption3 = 7;

							BYTE ItemSocketOption[6];

							ItemSocketOption[0] = lpItemBagItemInfo->Option1;
							ItemSocketOption[1] = lpItemBagItemInfo->Option2;
							ItemSocketOption[2] = lpItemBagItemInfo->Option3;
							ItemSocketOption[3] = lpItemBagItemInfo->Option4;
							ItemSocketOption[4] = lpItemBagItemInfo->Option5;
							ItemSocketOption[5] = lpItemBagItemInfo->Option6;

							int px = x;
							int py = y;

							if (it != this->m_ItemBagInfo.begin() || (gMap[map].CheckAttr(px, py, 4) != 0 || gMap[map].CheckAttr(px, py, 8) != 0))
							{
								if (this->GetRandomItemDropLocation(map, &px, &py, 2, 2, 10) == 0)
								{
									px = lpObj->X;
									py = lpObj->Y;
								}
							}

							GDCreateItemSend(lpObjRewarded->Index, map, px, py, ItemIndex, ItemLevel, 255, ItemOption1, ItemOption2, ItemOption3, lpObjRewarded->Index, ItemNewOption, ItemSetOption, 0, 0, ItemSocketOption, ItemSocketOption[5], ((DWORD)time(0) + lpItemBagItemInfo->Duration));
						}

						gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [CS Package]", lpObjRewarded->Account, lpObjRewarded->Name);
					}
				}
				else
				{
					if (RandomManager.GetRandomElement((int*)&lpItemBagDropInfo) == 0)
					{
						continue;
					}

					int px = x;
					int py = y;

					if (it != this->m_ItemBagInfo.begin() || (gMap[map].CheckAttr(px, py, 4) != 0 || gMap[map].CheckAttr(px, py, 8) != 0))
					{
						if (this->GetRandomItemDropLocation(map, &px, &py, 2, 2, 10) == 0)
						{
							px = lpObjRewarded->X;
							py = lpObjRewarded->Y;
						}
					}

					std::map<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>::iterator ItemInfo = this->m_ItemBagItemInfo.find(lpItemBagDropInfo->Section);

					if (ItemInfo == this->m_ItemBagItemInfo.end())
					{
						gMap[map].MoneyItemDrop(lpItemBagDropInfo->MoneyAmount, px, py);
						continue;
					}

					if (ItemInfo->second.empty() != 0)
					{
						gMap[map].MoneyItemDrop(lpItemBagDropInfo->MoneyAmount, px, py);
						continue;
					}

					ITEM_BAG_EX_ITEM_INFO* lpItemBagItemInfo = &ItemInfo->second[GetLargeRand() % ItemInfo->second.size()];

					WORD ItemIndex = lpItemBagItemInfo->Index;
					BYTE ItemLevel = lpItemBagItemInfo->Level;
					BYTE ItemOption1 = 0;
					BYTE ItemOption2 = 0;
					BYTE ItemOption3 = 0;
					BYTE ItemNewOption = 0;
					BYTE ItemSetOption = 0;
					BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

					gItemOptionRate.GetItemOption0(lpItemBagItemInfo->Option0, &ItemLevel);
					gItemOptionRate.GetItemOption1(lpItemBagItemInfo->Option1, &ItemOption1);
					gItemOptionRate.GetItemOption2(lpItemBagItemInfo->Option2, &ItemOption2);
					gItemOptionRate.GetItemOption3(lpItemBagItemInfo->Option3, &ItemOption3);
					gItemOptionRate.GetItemOption4(lpItemBagItemInfo->Option4, &ItemNewOption);
					gItemOptionRate.GetItemOption5(lpItemBagItemInfo->Option5, &ItemSetOption);
					gItemOptionRate.GetItemOption6(lpItemBagItemInfo->Option6, &ItemSocketOption[0]);
					gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
					gItemOptionRate.MakeSetOption(ItemIndex, ItemSetOption, &ItemSetOption);
					gItemOptionRate.MakeSocketOption(ItemIndex, ItemSocketOption[0], &ItemSocketOption[0]);

					if (lpObjRewarded->MouseItemUp == 1)
					{
						if (ItemIndex == GET_ITEM(14, 285))
						{
							int RuudAmount = 0;

							if (lpItemBagItemInfo->Option1 > lpItemBagItemInfo->Option0)
								RuudAmount = lpItemBagItemInfo->Option0 + (GetLargeRand() % (lpItemBagItemInfo->Option1 - lpItemBagItemInfo->Option0));
							else
								RuudAmount = lpItemBagItemInfo->Option0;

							lpObjRewarded->Ruud += RuudAmount;
							gRheaGold.GC_Notify_RuudUpdate(lpObjRewarded->Index, lpObjRewarded->Ruud, RuudAmount, 1);

							gLog.Output(ITEM_BAG_LOG, "[ItemBag][%s][%s] [Ruud: %d]", lpObjRewarded->Account, lpObjRewarded->Name, RuudAmount);
						}
						else if (ItemIndex == GET_ITEM(13, 124))
						{
							GJAccountLevelSaveSend(lpObjRewarded->Index, lpItemBagItemInfo->Option0, lpItemBagItemInfo->Option1);
							GJAccountLevelSend(lpObjRewarded->Index);
							gLog.Output(ITEM_BAG_LOG, "[ItemBag][%s][%s] [VIP: %d] [Time: %d]", lpObjRewarded->Account, lpObjRewarded->Name, lpItemBagItemInfo->Option0, lpItemBagItemInfo->Option1);
						}
						else
						{
							CItem item;
							item.Clear();
							item.m_Index = ItemIndex;

							char itemName[64];
							strcpy_s(itemName, gItemManager.GetName(ItemIndex));

							if (gMuunSystem.IsMuunItem(&item) == 0)
							{
								item.m_Level = lpItemBagItemInfo->Option0;
								item.m_Option1 = lpItemBagItemInfo->Option1;
								item.m_Option2 = lpItemBagItemInfo->Option2;
								item.m_Option3 = lpItemBagItemInfo->Option3;
								item.m_NewOption = lpItemBagItemInfo->Option4;
								item.m_SetOption = lpItemBagItemInfo->Option5;
								memcpy(item.m_SocketOption, ItemSocketOption, sizeof(item.m_SocketOption));
							}

							if (gItemStack.GetItemMaxStack(ItemIndex) > 0)
							{
								item.m_Durability = (lpItemBagItemInfo->Duration > 0) ? lpItemBagItemInfo->Duration : 1;
							}
							else
							{
								item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());
							}

							gLog.Output(ITEM_BAG_LOG, "[ItemBag][%s][%s] [Section: %d] [ItemCode: %d] [Item: %s +%d +%d]", lpObjRewarded->Account, lpObjRewarded->Name, it->second.DropInfo[0].Section, ItemIndex, gItemManager.GetName(ItemIndex), ItemLevel, ItemOption3);
							GDCreateItemSend(lpObjRewarded->Index, 0xEB, px, py, ItemIndex, ItemLevel, item.m_Durability, ItemOption1, ItemOption2, ItemOption3, lpObjRewarded->Index, ((ItemNewOption == 0) ? lpItemBagItemInfo->Grade : ItemNewOption), ItemSetOption, 0, 0, ItemSocketOption, 0xFF, ((lpItemBagItemInfo->Duration > 0) ? ((DWORD)time(0) + lpItemBagItemInfo->Duration) : 0));

							gNotice.GCNoticeSend(lpObjRewarded->Index, 1, 0, 0, 0, 0, 0, "%s obtido", itemName);
						}
					}
					else
					{
						if (ItemIndex == GET_ITEM(14, 285))
						{
							int RuudAmount = 0;

							if (lpItemBagItemInfo->Option1 > lpItemBagItemInfo->Option0)
								RuudAmount = lpItemBagItemInfo->Option0 + (GetLargeRand() % (lpItemBagItemInfo->Option1 - lpItemBagItemInfo->Option0));
							else
								RuudAmount = lpItemBagItemInfo->Option0;

							lpObjRewarded->Ruud += RuudAmount;
							gRheaGold.GC_Notify_RuudUpdate(lpObjRewarded->Index, lpObjRewarded->Ruud, RuudAmount, 1);

							gLog.Output(ITEM_BAG_LOG, "[ItemBag][%s][%s] [Ruud: %d]", lpObjRewarded->Account, lpObjRewarded->Name, RuudAmount);
						}
						else if (ItemIndex == GET_ITEM(13, 124))
						{
							GJAccountLevelSaveSend(lpObjRewarded->Index, lpItemBagItemInfo->Option0, lpItemBagItemInfo->Option1);
							GJAccountLevelSend(lpObjRewarded->Index);
							gLog.Output(ITEM_BAG_LOG, "[ItemBag][%s][%s] [VIP: %d] [Time: %d]", lpObjRewarded->Account, lpObjRewarded->Name, lpItemBagItemInfo->Option0, lpItemBagItemInfo->Option1);
						}
						else
						{
							char itemName[64];
							strcpy_s(itemName, gItemManager.GetName(ItemIndex));

							BYTE Durability = 1;
							int itemCount = 1;

							if (gItemStack.GetItemMaxStack(ItemIndex) > 0)
							{
								Durability = (lpItemBagItemInfo->Duration > 0) ? lpItemBagItemInfo->Duration : 1;
								itemCount = Durability;
							}
							else
							{
								Durability = (float)gItemManager.GetItemDurability(ItemIndex, ItemLevel, ItemNewOption, ItemSetOption);
							}

							if (MonsterIndex > -1)
							{
								GDCreateItemSend(lpObjRewarded->Index, 0xEB, 0, 0, ItemIndex, ItemLevel, Durability, ItemOption1, ItemOption2, ItemOption3, lpObjRewarded->Index, ((ItemNewOption == 0) ? lpItemBagItemInfo->Grade : ItemNewOption), ItemSetOption, 0, 0, ItemSocketOption, 0xFF, ((lpItemBagItemInfo->Duration > 0 && Durability == 1) ? ((DWORD)time(0) + lpItemBagItemInfo->Duration) : 0));
								gNotice.SendMapServerGroupMsg(1, "[%s] drop %d %s de %s (Chance: %d%%)", lpObjRewarded->Name, itemCount, itemName, lpMonster->MonsterName, chance);
							}
							else
							{
								//MUFC ARMAS
								if (ItemIndex == GET_ITEM(0, 51) || //Sword BK e MG
									ItemIndex == GET_ITEM(0, 60) || //Claw RF
									ItemIndex == GET_ITEM(2, 25) || //Scepter DL
									ItemIndex == GET_ITEM(3, 26) || //Lance GL
									ItemIndex == GET_ITEM(4, 30) || //Bow Elf
									ItemIndex == GET_ITEM(5, 49) || //Staff DW
									ItemIndex == GET_ITEM(5, 50) || //Stick SU
									ItemIndex == GET_ITEM(12, 266)	//Wings
									)
								{
									GDCreateItemSend(lpObjRewarded->Index, map, px, py, ItemIndex, ItemLevel, Durability, ItemOption1, ItemOption2, ItemOption3, lpObjRewarded->Index, ((ItemNewOption == 0) ? lpItemBagItemInfo->Grade : ItemNewOption), ItemSetOption, 0, 0, ItemSocketOption, 0xFF, ((DWORD)time(0) + lpItemBagItemInfo->Duration));
								}
								else
								{
									GDCreateItemSend(lpObjRewarded->Index, map, px, py, ItemIndex, ItemLevel, Durability, ItemOption1, ItemOption2, ItemOption3, lpObjRewarded->Index, ((ItemNewOption == 0) ? lpItemBagItemInfo->Grade : ItemNewOption), ItemSetOption, 0, 0, ItemSocketOption, 0xFF, ((lpItemBagItemInfo->Duration > 0 && Durability == 1) ? ((DWORD)time(0) + lpItemBagItemInfo->Duration) : 0));
								}
							}

							gLog.Output(ITEM_BAG_LOG, "[ItemBag][%s][%s] [Section: %d] [ItemCode: %d] [Item: %d %s +%d +%d]", lpObjRewarded->Account, lpObjRewarded->Name, it->second.DropInfo[0].Section, itemCount, ItemIndex, itemName, ItemLevel, ItemOption3);
						}
					}

					if ((lpItemBagDropInfo->OptionValue & 1) != 0)
					{
						GCFireworksSend(lpObjRewarded, lpObjRewarded->X, lpObjRewarded->Y);
					}
				}
			}
		}
	}

	return 1;
}

bool CItemBagEx::CheckDropClass(LPOBJ lpObj, int RequireClass[MAX_CLASS]) // OK
{
	if (RequireClass[lpObj->Class] != 0)
	{
		return 1;
	}

	return 0;
}

bool::CItemBagEx::GetRandomItemDropLocation(int map, int* ox, int* oy, int tx, int ty, int count) // OK
{
	if (!this->isLoaded)
	{
		return false;
	}

	int x = (*ox);
	int y = (*oy);

	tx = ((tx < 1) ? 1 : tx);
	ty = ((ty < 1) ? 1 : ty);

	for (int n = 0; n < count; n++)
	{
		(*ox) = ((GetLargeRand() % (tx + 1)) * ((GetLargeRand() % 2 == 0) ? -1 : 1)) + x;
		(*oy) = ((GetLargeRand() % (ty + 1)) * ((GetLargeRand() % 2 == 0) ? -1 : 1)) + y;

		if (gMap[map].CheckAttr((*ox), (*oy), 4) == 0 && gMap[map].CheckAttr((*ox), (*oy), 8) == 0)
		{
			return 1;
		}
	}

	return 0;
}
