#include "stdafx.h"
#include "SocketItemOption.h"
#include "ItemManager.h"
#include "MemScript.h"
#include "RandomManager.h"
#include "Util.h"
#include "MasteryItemExtOptionSet.h"
#include "MUFC.h"
#include "Map.h"

CSocketItemOption gSocketItemOption;

CSocketItemOption::CSocketItemOption() // OK
{
	this->Init();
}

CSocketItemOption::~CSocketItemOption() // OK
{

}

void CSocketItemOption::Init() // OK
{
	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION; n++)
	{
		this->m_SocketItemOptionInfo[n].Index = -1;
		this->m_SocketItemBonusOptionInfo[n].Index = -1;
		this->m_SocketItemPackageOptionInfo[n].Index = -1;
	}
}

void CSocketItemOption::Load(char* path) // OK
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

	this->Init();

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

					SOCKET_ITEM_OPTION_INFO info;

					memset(&info, 0, sizeof(info));

					info.Index = lpMemScript->GetNumber();
					info.Type = lpMemScript->GetAsNumber();
					info.SubIndex = lpMemScript->GetAsNumber();

					strcpy_s(info.Name, lpMemScript->GetAsString());

					info.Rate = lpMemScript->GetAsNumber();

					for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_LEVEL; n++)
					{
						info.ValueTable[n] = lpMemScript->GetAsFloatNumber();
					}

					this->SetInfo(info);
				}
				else if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					SOCKET_ITEM_BONUS_OPTION_INFO info;

					memset(&info, 0, sizeof(info));

					info.Index = lpMemScript->GetNumber();

					info.MinItemSection = lpMemScript->GetAsNumber();
					info.MaxItemSection = lpMemScript->GetAsNumber();

					strcpy_s(info.Name, lpMemScript->GetAsString());

					info.Rate = lpMemScript->GetAsNumber();

					info.Value = lpMemScript->GetAsFloatNumber();

					for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_TABLE; n++)
					{
						info.RequireOptionTypeTable[n] = lpMemScript->GetAsNumber();
					}

					this->SetBonusInfo(info);
				}
				else if (section == 2)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					SOCKET_ITEM_PACKAGE_OPTION_INFO info;

					memset(&info, 0, sizeof(info));

					info.Index = lpMemScript->GetNumber();

					strcpy_s(info.Name, lpMemScript->GetAsString());

					info.Value = lpMemScript->GetAsFloatNumber();

					for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_TYPE; n++)
					{
						info.RequireOptionTypeCountTable[n] = lpMemScript->GetAsNumber();
					}

					this->SetPackageInfo(info);
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

void CSocketItemOption::SetInfo(SOCKET_ITEM_OPTION_INFO info) // OK
{
	if (info.Index < 0 || info.Index >= MAX_SOCKET_ITEM_OPTION)
	{
		return;
	}

	this->m_SocketItemOptionInfo[info.Index] = info;
}

void CSocketItemOption::SetBonusInfo(SOCKET_ITEM_BONUS_OPTION_INFO info) // OK
{
	if (info.Index < 0 || info.Index >= MAX_SOCKET_ITEM_OPTION)
	{
		return;
	}

	this->m_SocketItemBonusOptionInfo[info.Index] = info;
}

void CSocketItemOption::SetPackageInfo(SOCKET_ITEM_PACKAGE_OPTION_INFO info) // OK
{
	if (info.Index < 0 || info.Index >= MAX_SOCKET_ITEM_OPTION)
	{
		return;
	}

	this->m_SocketItemPackageOptionInfo[info.Index] = info;
}

SOCKET_ITEM_OPTION_INFO* CSocketItemOption::GetInfo(int index) // OK
{
	if (index < 0 || index >= MAX_SOCKET_ITEM_OPTION)
	{
		return 0;
	}

	if (this->m_SocketItemOptionInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_SocketItemOptionInfo[index];
}

SOCKET_ITEM_BONUS_OPTION_INFO* CSocketItemOption::GetBonusInfo(int index) // OK
{
	if (index < 0 || index >= MAX_SOCKET_ITEM_OPTION)
	{
		return 0;
	}

	if (this->m_SocketItemBonusOptionInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_SocketItemBonusOptionInfo[index];
}

SOCKET_ITEM_PACKAGE_OPTION_INFO* CSocketItemOption::GetPackageInfo(int index) // OK
{
	if (index < 0 || index >= MAX_SOCKET_ITEM_OPTION)
	{
		return 0;
	}

	if (this->m_SocketItemPackageOptionInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_SocketItemPackageOptionInfo[index];
}

bool CSocketItemOption::IsSocketItem(CItem* lpItem) // OK
{
	if (gMasteryItemExtOptionSet.IsMasteryItem(lpItem->m_Index) == 1)
	{
		return 0;
	}

	if (gMasteryItemExtOptionSet.IsMasterySet(lpItem->m_Index) == 1)
	{
		return 0;
	}

	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_TABLE; n++)
	{
		if (lpItem->m_SocketOption[n] != SOCKET_ITEM_OPTION_NONE)
		{
			return 1;
		}
	}

	return 0;
}

int CSocketItemOption::GetSocketItemOptionCount(CItem* lpItem) // OK
{
	int count = 0;

	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_TABLE; n++)
	{
		if (lpItem->m_SocketOption[n] != SOCKET_ITEM_OPTION_NONE)
		{
			count++;
		}
	}

	return count;
}

int CSocketItemOption::GetSocketItemExtraMoney(CItem* lpItem) // OK
{
	int money = 0;

	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_TABLE; n++)
	{
		if (lpItem->m_SocketOption[n] != SOCKET_ITEM_OPTION_NONE && lpItem->m_SocketOption[n] != SOCKET_ITEM_OPTION_EMPTY)
		{
			int ItemOptionLevel;
			int ItemOptionType;

			if (lpItem->m_NewOption != 0 || lpItem->m_SetOption != 0)
			{
				int socketOption = lpItem->m_SocketOption[n] | 0x100;

				ItemOptionLevel = socketOption / MAX_SOCKET_ITEM_OPTION;

				if ((ItemOptionLevel + 1) > 5)
				{
					socketOption -= 2;
				}

				ItemOptionType = socketOption % MAX_SOCKET_ITEM_OPTION;
			}
			else
			{
				ItemOptionType = lpItem->m_SocketOption[n] % MAX_SOCKET_ITEM_OPTION;
				ItemOptionLevel = lpItem->m_SocketOption[n] / MAX_SOCKET_ITEM_OPTION;
			}

			SOCKET_ITEM_OPTION_INFO* lpInfoA = this->GetInfo(ItemOptionType);

			if (lpInfoA == 0)
			{
				continue;
			}

			int ItemIndex;

			if ((ItemOptionLevel + 1) > 5)
			{
				ItemIndex = (GET_ITEM(12, 311) + ((ItemOptionLevel - 5) * 6) + (lpInfoA->Type - 1));
			}
			else
			{
				ItemIndex = (GET_ITEM(12, 100) + (ItemOptionLevel * 6) + (lpInfoA->Type - 1));
			}

			ITEM_INFO ItemInfo;

			if (gItemManager.GetInfo(ItemIndex, &ItemInfo) != 0)
			{
				money += ItemInfo.BuyMoney;
			}
		}
	}

	return money;
}

int CSocketItemOption::GetSocketItemSeedOption(int index, int type) // OK
{
	SOCKET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(index);

	if (lpInfo == 0)
	{
		return -1;
	}

	if (lpInfo->Type != type)
	{
		return -1;
	}

	return lpInfo->SubIndex;
}

int CSocketItemOption::GetSocketItemRandomOption(int type) // OK
{
	int SocketItemOption = -1;

	CRandomManager RandomManager;

	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION; n++)
	{
		SOCKET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(n);

		if (lpInfo == 0)
		{
			continue;
		}

		if (lpInfo->Type != type)
		{
			continue;
		}

		RandomManager.AddElement(lpInfo->Index, lpInfo->Rate);
	}

	if (RandomManager.GetRandomElement(&SocketItemOption) != 0)
	{
		return SocketItemOption;
	}
	else
	{
		return -1;
	}
}

CItem CSocketItemOption::GetSocketExtractSeedSphere(CItem* lpItem, int type)
{
	CItem item;

	short seedSphereValue = this->Get10BitSocketOptionIndex(type, lpItem->m_SocketOption[type], lpItem->m_SetOption, lpItem->m_NewOption);

	if (seedSphereValue > 255)
	{
		seedSphereValue -= 2;
	}

	BYTE btSeedIndex = seedSphereValue % 50;
	BYTE btSeedSphereLevel = (seedSphereValue - btSeedIndex) / 50;

	SOCKET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(btSeedIndex);

	if (lpInfo == 0)
	{
		LogAdd(LOG_RED, "GetSocketExtractSeedSphere ERROR: %d %d %d %d %d %d", btSeedIndex, seedSphereValue, type, lpItem->m_SocketOption[type], lpItem->m_SetOption, lpItem->m_NewOption);
		return item;
	}

	if (btSeedSphereLevel < 5)
	{
		item.m_Index = GET_ITEM(12, 100) + btSeedSphereLevel * 6 + (lpInfo->Type - 1);
	}
	else
	{
		item.m_Index = GET_ITEM(12, 311) + (btSeedSphereLevel - 5) * 6 + (lpInfo->Type - 1);
	}

	item.m_Level = lpInfo->SubIndex;

	return item;
}

int CSocketItemOption::GetSocketItemOptionSXI(CItem* lpItem, int type, int option, int level) // OK
{
	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION; n++)
	{
		SOCKET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(n);

		if (lpInfo == 0)
		{
			continue;
		}

		if (lpInfo->Type != type)
		{
			continue;
		}

		if (lpInfo->SubIndex != option)
		{
			continue;
		}

		if (this->CheckSocketItemOption(lpInfo->Index, lpItem) == 0)
		{
			continue;
		}

		return (lpInfo->Index + (level * MAX_SOCKET_ITEM_OPTION));
	}

	return SOCKET_ITEM_OPTION_NONE;
}

BYTE CSocketItemOption::GetSocketItemOption(CItem* lpItem, int type, int option, int level) // OK
{
	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION; n++)
	{
		SOCKET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(n);

		if (lpInfo == 0)
		{
			continue;
		}

		if (lpInfo->Type != type)
		{
			continue;
		}

		if (lpInfo->SubIndex != option)
		{
			continue;
		}

		if (this->CheckSocketItemOption(lpInfo->Index, lpItem) == 0)
		{
			continue;
		}

		return (lpInfo->Index + (level * MAX_SOCKET_ITEM_OPTION));
	}

	return SOCKET_ITEM_OPTION_NONE;
}

BYTE CSocketItemOption::GetSocketItemBonusOption(CItem* lpItem) // OK
{
	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION; n++)
	{
		SOCKET_ITEM_BONUS_OPTION_INFO* lpBonusInfo = this->GetBonusInfo(n);

		if (lpBonusInfo == 0)
		{
			continue;
		}

		if (this->CheckSocketItemBonusOption(lpBonusInfo->Index, lpItem) == 0)
		{
			continue;
		}

		if ((GetLargeRand() % 100) < lpBonusInfo->Rate)
		{
			return lpBonusInfo->Index;
		}
	}

	return SOCKET_ITEM_BONUS_OPTION_NONE;
}

bool CSocketItemOption::CheckSocketItemOption(int index, CItem* lpItem) // OK
{
	SOCKET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(index);

	if (lpInfo == 0)
	{
		return 0;
	}

	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_TABLE; n++)
	{
		if (lpItem->m_SocketOption[n] != SOCKET_ITEM_OPTION_NONE && lpItem->m_SocketOption[n] != SOCKET_ITEM_OPTION_EMPTY)
		{
			int ItemOptionLevel;
			int ItemOption;
			int socketOption = 0;

			short seedSphereValue = Get10BitSocketOptionIndex(n, lpItem->m_SocketOption[n], lpItem->m_SetOption, lpItem->m_NewOption);

			if (seedSphereValue > 255)
				seedSphereValue -= 2;

			ItemOption = seedSphereValue % 50;
			ItemOptionLevel = (seedSphereValue - ItemOption) / 50;

			if (ItemOption == lpInfo->Index)
			{
				return 0;
			}
		}
	}

	if (lpItem->m_Index >= GET_ITEM(0, 0) && lpItem->m_Index < GET_ITEM(6, 0) && lpItem->m_Index != GET_ITEM(4, 7) && lpItem->m_Index != GET_ITEM(4, 15))
	{
		if (lpInfo->Type == SOCKET_ITEM_OPTION_TYPE_FIRE || lpInfo->Type == SOCKET_ITEM_OPTION_TYPE_ICE || lpInfo->Type == SOCKET_ITEM_OPTION_TYPE_LIGHTNING)
		{
			return 1;
		}
	}

	if (lpItem->m_Index >= GET_ITEM(6, 0) && lpItem->m_Index < GET_ITEM(12, 0))
	{
		if (lpInfo->Type == SOCKET_ITEM_OPTION_TYPE_WATER || lpInfo->Type == SOCKET_ITEM_OPTION_TYPE_WIND || lpInfo->Type == SOCKET_ITEM_OPTION_TYPE_EARTH)
		{
			return 1;
		}
	}

	return 0;
}

bool CSocketItemOption::CheckSocketItemBonusOption(int index, CItem* lpItem) // OK
{
	SOCKET_ITEM_BONUS_OPTION_INFO* lpBonusInfo = this->GetBonusInfo(index);

	if (lpBonusInfo == 0)
	{
		return 0;
	}

	if (lpBonusInfo->MinItemSection > (lpItem->m_Index / MAX_ITEM_TYPE) || lpBonusInfo->MaxItemSection < (lpItem->m_Index / MAX_ITEM_TYPE))
	{
		return 0;
	}

	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_TABLE; n++)
	{
		if (lpBonusInfo->RequireOptionTypeTable[n] == SOCKET_ITEM_OPTION_TYPE_NONE)
		{
			continue;
		}

		if (lpItem->m_SocketOption[n] == SOCKET_ITEM_OPTION_NONE || lpItem->m_SocketOption[n] == SOCKET_ITEM_OPTION_EMPTY)
		{
			return 0;
		}

		int ItemOptionLevel;
		int ItemOption;
		int socketOption;

		short seedSphereValue = Get10BitSocketOptionIndex(n, lpItem->m_SocketOption[n], lpItem->m_SetOption, lpItem->m_NewOption);

		if (seedSphereValue > 255)
			seedSphereValue -= 2;

		ItemOption = seedSphereValue % 50;
		ItemOptionLevel = (seedSphereValue - ItemOption) / 50;

		if (ItemOptionLevel < 2 && lpBonusInfo->Index >= 6)
		{
			return 0;
		}

		if (ItemOptionLevel < 4 && lpBonusInfo->Index >= 12)
		{
			return 0;
		}

		if (ItemOptionLevel < 6 && lpBonusInfo->Index >= 18)
		{
			return 0;
		}

		if (ItemOptionLevel < 9 && lpBonusInfo->Index >= 24)
		{
			return 0;
		}

		SOCKET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(ItemOption);

		if (lpInfo == 0)
		{
			return 0;
		}

		if (lpInfo->Type != lpBonusInfo->RequireOptionTypeTable[n])
		{
			return 0;
		}
	}

	return 1;
}

bool CSocketItemOption::CheckSocketItemPackageOption(int index, int* ItemOptionTypeCount) // OK
{
	SOCKET_ITEM_PACKAGE_OPTION_INFO* lpPackageInfo = this->GetPackageInfo(index);

	if (lpPackageInfo == 0)
	{
		return 0;
	}

	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION_TYPE; n++)
	{
		if (lpPackageInfo->RequireOptionTypeCountTable[n] > ItemOptionTypeCount[n])
		{
			return 0;
		}
	}

	return 1;
}

void CSocketItemOption::CalcSocketItemOption(LPOBJ lpObj, bool flag) // OK
{
	int ItemOptionTypeCount[MAX_SOCKET_ITEM_OPTION_TYPE] = { 0,0,0,0,0,0 };

	for (int n = 0; n < 12; n++)
	{
		if (n == 7 || n == 8 || lpObj->Inventory[n].IsItem() == 0)
		{
			continue;
		}

		if (n >= 0 && n <= 1 && !flag)
		{
			gMUFC.CalcWeaponOption(lpObj, &lpObj->Inventory[n]);
		}

		if (n >= 0 && n <= 1 && !flag && gMasteryItemExtOptionSet.IsMasteryWeapon(lpObj->Inventory[n].m_Index))
		{
			gMasteryItemExtOptionSet.CalcMasteryWeaponOption(lpObj, &lpObj->Inventory[n]);
			continue;
		}

		// 9 = pendant
		// 10 e 11 = rings
		if (n == 9 && !flag && gMasteryItemExtOptionSet.IsMasteryPendant(lpObj->Inventory[n].m_Index))
		{
			gMasteryItemExtOptionSet.CalcMasteryPendantOption(lpObj, &lpObj->Inventory[n]);
			continue;
		}

		if (n >= 10 && n <= 11 && !flag && gMasteryItemExtOptionSet.IsMasteryRing(lpObj->Inventory[n].m_Index))
		{
			gMasteryItemExtOptionSet.CalcMasteryRingOption(lpObj, &lpObj->Inventory[n]);
			continue;
		}

		if (n >= 0 && n <= 1 && !flag && gMasteryItemExtOptionSet.IsMasteryShield(lpObj->Inventory[n].m_Index))
		{
			gMasteryItemExtOptionSet.CalcMasteryOption(lpObj, &lpObj->Inventory[n]);
			continue;
		}

		if (n >= 2 && n <= 6 && !flag && gMasteryItemExtOptionSet.IsMasterySet(lpObj->Inventory[n].m_Index))
		{
			gMasteryItemExtOptionSet.CalcMasteryOption(lpObj, &lpObj->Inventory[n]);
			continue;
		}

		if (lpObj->Inventory[n].IsSocketItem() == 0)
		{
			continue;
		}

		/*	Right = 0
			Left = 1			
			Helm = 2
			Armor = 3
			Pants = 4
			Gloves = 5
			Boots = 6
			Wings = 7
			Helper = 8
			Amulet = 9
			Ring1 = 10
			Ring2 = 11
			Pentagram = 236
		*/

		for (int i = 0; i < MAX_SOCKET_ITEM_OPTION_TABLE; i++)
		{
			if (lpObj->Inventory[n].m_SocketOption[i] != SOCKET_ITEM_OPTION_NONE && lpObj->Inventory[n].m_SocketOption[i] != SOCKET_ITEM_OPTION_EMPTY)
			{
				int ItemOptionLevel;
				int ItemOption;

				short seedSphereValue = Get10BitSocketOptionIndex(i, lpObj->Inventory[n].m_SocketOption[i], lpObj->Inventory[n].m_SetOption, lpObj->Inventory[n].m_NewOption);

				if (seedSphereValue > 255)
					seedSphereValue -= 2;

				ItemOption = seedSphereValue % 50;
				ItemOptionLevel = (seedSphereValue - ItemOption) / 50;

				SOCKET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(ItemOption);

				if (lpInfo == 0)
				{
					continue;
				}

				if (ItemOptionLevel < 0 || ItemOptionLevel >= MAX_SOCKET_ITEM_OPTION_LEVEL)
				{
					continue;
				}

				if ((lpObj->Class == CLASS_RF || lpObj->Class == CLASS_MG) && n == 3) //MG and RF Armor double options
				{
					this->InsertOption(lpObj, lpInfo->Index, lpInfo->ValueTable[ItemOptionLevel], flag);
				}

				if (lpObj->Class == CLASS_RF && lpObj->Inventory[1].IsItem() == false && n == 3) //RF Armor double options, if not 1 claw
				{
					this->InsertOption(lpObj, lpInfo->Index, lpInfo->ValueTable[ItemOptionLevel], flag);
				}

				if (lpObj->Class == CLASS_FE && (lpObj->Inventory[1].m_Index == GET_ITEM(4, 15) || lpObj->Inventory[1].m_Index == GET_ITEM(4, 7)) && n == 3) //Elf Armor double options, if not quiver
				{
					this->InsertOption(lpObj, lpInfo->Index, lpInfo->ValueTable[ItemOptionLevel], flag);
				}

				this->InsertOption(lpObj, lpInfo->Index, lpInfo->ValueTable[ItemOptionLevel], flag);

				ItemOptionTypeCount[(lpInfo->Type - 1)]++;
			}
		}

		if (lpObj->Inventory[n].m_SocketOptionBonus != SOCKET_ITEM_BONUS_OPTION_NONE)
		{
			SOCKET_ITEM_BONUS_OPTION_INFO* lpBonusInfo = this->GetBonusInfo(lpObj->Inventory[n].m_SocketOptionBonus);

			if (lpBonusInfo == 0)
			{
				continue;
			}

			if ((lpObj->Class == CLASS_RF || lpObj->Class == CLASS_MG) && n == 3) //MG and RF Armor double bonus
			{
				this->InsertBonusOption(lpObj, lpBonusInfo->Index, lpBonusInfo->Value, flag);
			}

			if (lpObj->Class == CLASS_RF && lpObj->Inventory[1].IsItem() == false && n == 3) //RF and ELF Armor double options, if not shield
			{
				this->InsertBonusOption(lpObj, lpBonusInfo->Index, lpBonusInfo->Value, flag);
			}

			if (lpObj->Class == CLASS_FE && (lpObj->Inventory[1].m_Index == GET_ITEM(4, 15) || lpObj->Inventory[1].m_Index == GET_ITEM(4, 7)) && n == 3) //Elf Armor double options, if not quiver
			{
				this->InsertBonusOption(lpObj, lpBonusInfo->Index, lpBonusInfo->Value, flag);
			}

			this->InsertBonusOption(lpObj, lpBonusInfo->Index, lpBonusInfo->Value, flag);
		}
	}

	this->CalcSocketItemPackageOption(lpObj, ItemOptionTypeCount, flag);
}

void CSocketItemOption::CalcSocketItemPackageOption(LPOBJ lpObj, int* ItemOptionTypeCount, bool flag) // OK
{
	for (int n = 0; n < MAX_SOCKET_ITEM_OPTION; n++)
	{
		SOCKET_ITEM_PACKAGE_OPTION_INFO* lpPackageInfo = this->GetPackageInfo(n);

		if (lpPackageInfo == 0)
		{
			continue;
		}

		if (this->CheckSocketItemPackageOption(lpPackageInfo->Index, ItemOptionTypeCount) == 0)
		{
			continue;
		}

		this->InsertPackageOption(lpObj, lpPackageInfo->Index, lpPackageInfo->Value, flag);
	}
}

void CSocketItemOption::InsertOption(LPOBJ lpObj, int index, float value, bool flag) // OK
{
	if (lpObj->Map != MAP_CASTLE_SIEGE)
	{
		switch(index)
		{
			case SOCKET_ITEM_OPTION_REDUCAO_DANO_GERAL: //OK
			case SOCKET_ITEM_OPTION_ADICIONAL_DANO_REFLECT: //OK
				return;
		}
	}

	if (flag == 0)
	{
		if (index == SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_FORCA || index == SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_AGILIDADE || index == SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_VITALIDADE || index == SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_ENERGIA)
		{
			return;
		}
	}
	else
	{
		if (index != SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_FORCA && index != SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_AGILIDADE && index != SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_VITALIDADE && index != SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_ENERGIA)
		{
			return;
		}
	}

	switch (index)
	{
	case SOCKET_ITEM_OPTION_DANO_ADICIONAL: //OK
		lpObj->AddGeneralDamageRate += value;
		break;
	case SOCKET_ITEM_OPTION_CHANCE_ELEMENTAL_TRIPLO: //OK
		lpObj->ElementalTripleDamageRate += value;
		break;
	case SOCKET_ITEM_OPTION_CHANCE_ABSORV_HP: //OK
		lpObj->AbsorbLifeFromDamage += value;
		break;
	case SOCKET_ITEM_OPTION_DANO_MIN_MAX_ADICIONAL: //OK
		lpObj->PhysiDamageMinLeft += (int)(lpObj->PhysiDamageMinLeft * value / 100);
		lpObj->PhysiDamageMinRight += (int)(lpObj->PhysiDamageMinRight * value / 100);
		lpObj->MagicDamageMin += (int)(lpObj->MagicDamageMin * value / 100);
		lpObj->CurseDamageMin += (int)(lpObj->CurseDamageMin * value / 100);
		lpObj->PhysiDamageMaxLeft += (int) (lpObj->PhysiDamageMaxLeft * value / 100);
		lpObj->PhysiDamageMaxRight += (int)(lpObj->PhysiDamageMaxRight * value / 100);
		lpObj->MagicDamageMax += (int)(lpObj->MagicDamageMax * value / 100);
		lpObj->CurseDamageMax += (int)(lpObj->CurseDamageMax * value / 100);
		break;
	case SOCKET_ITEM_OPTION_DANO_ADICIONAL_ELITE: //OK
		lpObj->AddGeneralDamageRate += value;
		break;
	case SOCKET_ITEM_OPTION_DANO_ELEMENTAL_ADICIONAL: //OK
		lpObj->AddElementalDamageRate += (int)value;
		break;

	case SOCKET_ITEM_OPTION_RESISTENCIA_RADIANCE: //OK
		lpObj->AddRadianceResistance += (int)value;
		break;
	case SOCKET_ITEM_OPTION_DEFESA_ADICIONAL: //OK
		lpObj->Defense += value;
		break;
	case SOCKET_ITEM_OPTION_REDUCAO_DANO_ESCUDO: //OK
		lpObj->DecDamageByShield += value;
		break;
	case SOCKET_ITEM_OPTION_REDUCAO_DANO_GERAL: //OK
		lpObj->DamageReduction[DAMAGE_REDUCTION_SOCKET_ITEM] += value;
		break;
	case SOCKET_ITEM_OPTION_ADICIONAL_DANO_REFLECT: //OK
		lpObj->DamageReflect += value;
		break;

	case SOCKET_ITEM_OPTION_DANO_ADICIONAL_BOSS: //OK
		lpObj->AddBossDamage += value;
		break;
	case SOCKET_ITEM_OPTION_ACELERACAO_HITS_BOSS: //OK
		lpObj->DecBossHitDelay += value;
		break;
	case SOCKET_ITEM_OPTION_DANO_VERDADEIRO: //OK
		lpObj->TrueDamageRate += value;
		break;
	case SOCKET_ITEM_OPTION_CHANCE_DANO_30PERC_HP: //OK
		lpObj->UltraHit30perChance += value;
		break;
	case SOCKET_ITEM_OPTION_CHANCE_REDUZIR_DURABILIDADE: //OK
		lpObj->DirectDurabilityDamage += value;
		break;

	case SOCKET_ITEM_OPTION_CHANCE_RECUPERAR_20PERC_HP: //OK
		lpObj->UltraHPRecoveryChance += value;
		break;
	case SOCKET_ITEM_OPTION_ADICIONAL_HP: //OK
		lpObj->SocketAddLife += value;
		break;
	case SOCKET_ITEM_OPTION_REDUCAO_DANO_ELEMENTAL: //OK
		lpObj->ReduceGeneralElementalDamageRate += value;
		break;
	case SOCKET_ITEM_OPTION_REDUCAO_DANO_BOSS: //OK
		lpObj->DecBossDamage += value;
		//LogAdd(LOG_DEBUG, "Add red boss %f em %s, total %f", value, lpObj->Name, lpObj->DecBossDamage);
		break;
	case SOCKET_ITEM_OPTION_RESISTENCIA_DANO_TRIPLO: //OK
		lpObj->ResistTripleDamageRate += value;
		break;
	case SOCKET_ITEM_OPTION_RESISTENCIA_DANO_DUPLO: //OK
		lpObj->ResistDoubleDamageRate += value;
		break;

	case SOCKET_ITEM_OPTION_ADICIONAL_DANO_EXCELENTE: //OK
		lpObj->ExcellentDamagePerc += value;
		break;
	case SOCKET_ITEM_OPTION_ADICIONAL_CHANCE_DANO_EXCELENTE: //OK
		lpObj->m_MPSkillOpt.ExcellentDamageRate += value;
		break;
	case SOCKET_ITEM_OPTION_ADICIONAL_DANO_CRITICO: //OK
		lpObj->CriticalDamagePerc += value;
		break;
	case SOCKET_ITEM_OPTION_ADICIONAL_CHANCE_DANO_CRITICO: //OK
		lpObj->m_MPSkillOpt.CriticalDamageRate += value;
		break;

	case SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_FORCA:
		lpObj->AddStrength += value;
		break;
	case SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_AGILIDADE:
		lpObj->AddDexterity += value;
		break;
	case SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_VITALIDADE:
		lpObj->AddVitality += value;
		break;
	case SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_ENERGIA:
		lpObj->AddEnergy += value;
		break;
	}
}

void CSocketItemOption::InsertBonusOption(LPOBJ lpObj, int index, float value, bool flag) // OK
{
	if (flag == 1)
	{
		return;
	}

	//LogAdd(LOG_DEBUG, "Bonus index: %d | flag %d", index,flag);

	switch (index%6)
	{
	case SOCKET_ITEM_WEAPON_BONUS_OPTION_DANO_PVP: //OK
	case SOCKET_ITEM_STAFF_BONUS_OPTION_DANO_PVP: //OK
		lpObj->AddPVPDamageRate += value;
		break;
	case SOCKET_ITEM_WEAPON_BONUS_OPTION_DANO_BOSS: //OK
	case SOCKET_ITEM_STAFF_BONUS_OPTION_DANO_BOSS: //OK
		lpObj->AddBossDamage += value;
		break;
	case SOCKET_ITEM_ARMOR_BONUS_OPTION_REDUZ_DANO_BOSS_PVP: //OK
		lpObj->DecBossDamage += value;
		break;
	case SOCKET_ITEM_ARMOR_BONUS_OPTION_AUMENTA_HP_MAXIMO: //OK
		lpObj->AddLife += (int)(lpObj->MaxLife * value / 100);
		break;
	}
}

void CSocketItemOption::InsertPackageOption(LPOBJ lpObj, int index, float value, bool flag) // OK
{
	if (flag != 0)
	{
		return;
	}

	switch (index)
	{
	case SOCKET_ITEM_PACKAGE_OPTION_CHANCE_DANO_EXCELENTE: //OK
		lpObj->m_MPSkillOpt.ExcellentDamageRate += value;
		break;
	case SOCKET_ITEM_PACKAGE_OPTION_CHANCE_DANO_DUPLO: //OK
		lpObj->m_MPSkillOpt.DoubleDamageRate += value;
		break;
	case SOCKET_ITEM_PACKAGE_OPTION_CHANCE_DANO_TRIPLO: //OK
		lpObj->m_MPSkillOpt.TripleDamageRate += value;
		break;
	}
}

short CSocketItemOption::Get10BitSocketOptionIndex(BYTE btSocketSlotIndex, BYTE btSocketOption, BYTE btSetOption, BYTE btExcellentOption)
{
	switch (btSocketSlotIndex)
	{
	case 0:
		if (btSocketOption == 0xFF && (btSetOption & 0xC0) < 1)
			return 0xFF;
		if (btSocketOption == 0xFE && (btSetOption & 0xC0) < 1)
			return 0xFE;
		break;
	case 1:
		if (btSocketOption == 0xFF && (btSetOption & 0x48) < 1)
			return 0xFF;
		if (btSocketOption == 0xFE && (btSetOption & 0x48) < 1)
			return 0xFE;
		break;
	case 2:
		if (btSocketOption == 0xFF && (btSetOption & 0x12) < 1)
			return 0xFF;
		if (btSocketOption == 0xFE && (btSetOption & 0x12) < 1)
			return 0xFE;
		break;
	case 3:
		if (btSocketOption == 0xFF && (btSetOption & 3) < 1)
			return 0xFF;
		if (btSocketOption == 0xFE && (btSetOption & 3) < 1)
			return 0xFE;
		break;
	case 4:
		if (btSocketOption == 0xFF && (btExcellentOption & 0x48) < 1)
			return 0xFF;
		if (btSocketOption == 0xFE && (btExcellentOption & 0x48) < 1)
			return 0xFE;
		break;
	}

	switch (btSocketSlotIndex)
	{
	case 0:
		return ((btSetOption & 0xC0) << 2) | btSocketOption;
	case 1:
		return ((btSetOption & 0x30) << 4) | btSocketOption;
	case 2:
		return ((btSetOption & 0x0C) << 6) | btSocketOption;
	case 3:
		return ((btSetOption & 0x03) << 8) | btSocketOption;
	case 4:
		return ((btExcellentOption & 0x30) << 4) | btSocketOption;
	}
}

void CSocketItemOption::Convert10BitSocketOptionValue(short nSeedSphereIndex, BYTE btSocketSlotIndex, BYTE& btSockeOption, BYTE& btSetOption, BYTE& btExcellentOption)
{
	btSockeOption = 0;

	if (nSeedSphereIndex >= 0xFE)
		nSeedSphereIndex += 2;

	btSockeOption |= nSeedSphereIndex;

	switch (btSocketSlotIndex)
	{
	case 0:
		btSetOption |= (nSeedSphereIndex & 0x300) >> 2;
		break;
	case 1:
		btSetOption |= (nSeedSphereIndex & 0x300) >> 4;
		break;
	case 2:
		btSetOption |= (nSeedSphereIndex & 0x300) >> 6;
		break;
	case 3:
		btSetOption |= (nSeedSphereIndex & 0x300) >> 8;
		break;
	case 4:
		btExcellentOption |= (nSeedSphereIndex & 0x300) >> 4;
		break;
	}
}

int CSocketItemOption::GetSeedIndexFromSphereIndexOrderScript(short nSeedSphearIndex)
{
	if (nSeedSphearIndex > 0xFF)
		nSeedSphearIndex -= 2;

	return nSeedSphearIndex % 50;
}

BYTE CSocketItemOption::GetSeedSphearLevel(short btSeedSphereIndex)
{
	if (btSeedSphereIndex > 0xFF)
		btSeedSphereIndex -= 2;

	BYTE btSeedIndex = btSeedSphereIndex % 50;
	BYTE btSeedSphereLevel = (btSeedSphereIndex - btSeedIndex) / 50;

	return btSeedSphereLevel;
}

bool CSocketItemOption::ClearSocketSlot(CItem* lpTargetItem, BYTE btSocketSlotIndex)
{
	if (lpTargetItem == NULL)
		return false;

	//if (IsEnableSocketItem(lpTargetItem) == false)	return false;

	if (btSocketSlotIndex > 5)
		return false;

	int nSocketOptionIndex = Get10BitSocketOptionIndex(btSocketSlotIndex, lpTargetItem->m_SocketOption[btSocketSlotIndex], lpTargetItem->m_SetOption, lpTargetItem->m_NewOption);

	if (nSocketOptionIndex == 0xFF || nSocketOptionIndex == 0xFE)
	{
		return false;
	}

	lpTargetItem->m_SocketOption[btSocketSlotIndex] = 0xFE;

	switch (btSocketSlotIndex)
	{
	case 0:
		lpTargetItem->m_SetOption &= 0x3F;
		break;
	case 1:
		lpTargetItem->m_SetOption &= 0xCF;
		break;
	case 2:
		lpTargetItem->m_SetOption &= 0xF3;
		break;
	case 3:
		lpTargetItem->m_SetOption &= 0xFC;
		break;
	case 4:
		lpTargetItem->m_NewOption &= 0xCF;
		break;
	}

	lpTargetItem->m_SocketOptionBonus = this->GetSocketItemBonusOption(lpTargetItem);
	return true;
}