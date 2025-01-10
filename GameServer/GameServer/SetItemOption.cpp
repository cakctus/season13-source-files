#include "stdafx.h"
#include "SetItemOption.h"
#include "ItemManager.h"
#include "MemScript.h"
#include "SetItemType.h"
#include "Util.h"

CSetItemOption gSetItemOption;

CSetItemOption::CSetItemOption() // OK
{
	this->Init();
}

CSetItemOption::~CSetItemOption() // OK
{

}

void CSetItemOption::Init() // OK
{
	for(int n=0;n < MAX_SET_ITEM_OPTION;n++)
	{
		this->m_SetItemOptionInfo[n].Index = -1;
	}
}

void CSetItemOption::Load(char* path) // OK
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

	this->Init();

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

			SET_ITEM_OPTION_INFO info;

			memset(&info,0,sizeof(info));

			info.Index = lpMemScript->GetNumber();

			strcpy_s(info.FullName,lpMemScript->GetAsString());
			strcpy_s(info.TypeName, lpMemScript->GetAsString());

			for(int n=0;n < MAX_SET_ITEM_OPTION_TABLE;n++)
			{
				for (int i = 0; i < MAX_SET_ITEM_OPTION_I; i++)
				{
					info.OptionTable[n][i].Index = lpMemScript->GetAsNumber();
				}
			}

			for (int n = 0; n < MAX_SET_ITEM_OPTION_TABLE; n++)
			{
				for (int i = 0; i < MAX_SET_ITEM_OPTION_I; i++)
				{
					info.OptionTable[n][i].Value = lpMemScript->GetAsNumber();
				}
			}			

			info.Ex[0] = lpMemScript->GetAsNumber();
			info.Ex[1] = lpMemScript->GetAsNumber();
			info.Ex[2] = lpMemScript->GetAsNumber();
			info.Ex[3] = lpMemScript->GetAsNumber();
			info.Flag = lpMemScript->GetAsNumber();	

			for(int o=0;o < MAX_SET_ITEM_FULL_OPTION_TABLE;o++)
			{
				info.FullOptionTable[o].Index = lpMemScript->GetAsNumber();				
			}

			for (int o = 0; o < MAX_SET_ITEM_FULL_OPTION_TABLE; o++)
			{
				info.FullOptionTable[o].Value = lpMemScript->GetAsNumber();
			}

			for (int i = 0; i < MAX_CLASS; i++)
			{
				info.Class[i] = lpMemScript->GetAsNumber();
			}

			this->SetInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CSetItemOption::SetInfo(SET_ITEM_OPTION_INFO info) // OK
{
	if(info.Index < 0 || info.Index >= MAX_SET_ITEM_OPTION)
	{
		return;
	}

	this->m_SetItemOptionInfo[info.Index] = info;
}

SET_ITEM_OPTION_INFO* CSetItemOption::GetInfo(int index) // OK
{
	if(index < 0 || index >= MAX_SET_ITEM_OPTION)
	{
		return 0;
	}

	if(this->m_SetItemOptionInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_SetItemOptionInfo[index];
}

bool CSetItemOption::IsSetItem(CItem* lpItem) // OK
{
	if((lpItem->m_SetOption & 3) == 0)
	{
		return 0;
	}
	
	return 1;
}

int CSetItemOption::GetSetItemMaxOptionCount(int index) // OK
{
	SET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(index);

	if(lpInfo == 0)
	{
		return 0;
	}

	int count = 0;

	for(int n=0;n < MAX_SET_ITEM_OPTION_TABLE;n++)
	{
		for (int i = 0; i < MAX_SET_ITEM_OPTION_I; i++)
		{
			if(lpInfo->OptionTable[n][i].Index != -1)
			{
				count++;
				break;
			}
		}
	}

	return count;
}

int CSetItemOption::GetInventorySetItemOptionCount(LPOBJ lpObj,int index) // OK
{
	int OptionCount = 0;
	int WeaponOptionIndex = 0;
	int RingOptionIndex = 0;

	for(int n=0;n < INVENTORY_WEAR_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() == 0 || lpObj->Inventory[n].IsSetItem() == 0 || lpObj->Inventory[n].m_IsValidItem == 0)
		{
			continue;
		}

		if(gSetItemType.CheckSetItemType(lpObj->Inventory[n].m_Index) == 0)
		{
			continue;
		}

		int OptionIndex = gSetItemType.GetSetItemOptionIndex(lpObj->Inventory[n].m_Index,((lpObj->Inventory[n].m_SetOption & 3)-1));

		if(OptionIndex == 0 || OptionIndex != index)
		{
			continue;
		}

		if(n == 0 || n == 1)
		{
			if(WeaponOptionIndex == 0)
			{
				WeaponOptionIndex = OptionIndex;
			}
			else if(WeaponOptionIndex == OptionIndex && lpObj->Inventory[0].m_Index == lpObj->Inventory[1].m_Index) 
			{
				continue;
			}
		}
		else if(n == 10 || n == 11)
		{
			if(RingOptionIndex == 0)
			{
				RingOptionIndex = OptionIndex;
			}
			else if(RingOptionIndex == OptionIndex && lpObj->Inventory[10].m_Index == lpObj->Inventory[11].m_Index) 
			{
				continue;
			}
		}

		OptionCount++;
	}

	return OptionCount;
}

void CSetItemOption::CalcSetItemStat(LPOBJ lpObj) // OK
{
	for(int n=0;n < INVENTORY_WEAR_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() == 0 || lpObj->Inventory[n].IsSetItem() == 0)
		{
			continue;
		}

		if(gSetItemType.CheckSetItemType(lpObj->Inventory[n].m_Index) == 0)
		{
			continue;
		}

		int ItemOption = gSetItemType.GetSetItemStatType(lpObj->Inventory[n].m_Index);

		if(ItemOption == -1)
		{
			continue;
		}

		this->InsertOption(lpObj,ItemOption,(((lpObj->Inventory[n].m_SetOption >> 2) & 3)*5),1);
	}
}

void CSetItemOption::CalcSetItemOption(LPOBJ lpObj,bool flag) // OK
{
	lpObj->IsFullSetItem = 0;
	lpObj->AddGeneralDamageRate = 0.0f;
	lpObj->ReduceGeneralDamageRate = 0.0f;

	for(int n=0;n < MAX_SET_ITEM_OPTION;n++)
	{
		SET_ITEM_OPTION_INFO* lpInfo = this->GetInfo(n);

		if(lpInfo == 0)
		{
			continue;
		}

		int ItemOptionCount = this->GetInventorySetItemOptionCount(lpObj,lpInfo->Index);

		if(ItemOptionCount == 0)
		{
			continue;
		}

		for(int i=0;i < (ItemOptionCount-1);i++)
		{
			for (int o = 0; o < MAX_SET_ITEM_OPTION_I; o++)
			{
				if(lpInfo->OptionTable[i][o].Index != -1)
				{
					this->InsertOption(lpObj,lpInfo->OptionTable[i][o].Index,lpInfo->OptionTable[i][o].Value,flag);
				}
			}
		}

		if((ItemOptionCount-1) == this->GetSetItemMaxOptionCount(lpInfo->Index))
		{
			for(int o=0;o < MAX_SET_ITEM_FULL_OPTION_TABLE;o++)
			{
				if(lpInfo->FullOptionTable[o].Index != -1)
				{
					this->InsertOption(lpObj,lpInfo->FullOptionTable[o].Index,lpInfo->FullOptionTable[o].Value,flag);
				}
			}

			lpObj->IsFullSetItem = 1;
		}
	}
}

void CSetItemOption::InsertOption(LPOBJ lpObj,int index,int value,bool flag) // OK
{
	if(flag == 0)
	{
		if(index == SET_ITEM_OPTION_ADD_STRENGTH || index == SET_ITEM_OPTION_ADD_DEXTERITY || index == SET_ITEM_OPTION_ADD_VITALITY || index == SET_ITEM_OPTION_ADD_ENERGY || index == SET_ITEM_OPTION_ADD_LEADERSHIP || index == SET_ITEM_OPTION_INCREASE_ALL_STATS)
		{
			return;
		}
	}
	else
	{
		if(index != SET_ITEM_OPTION_ADD_STRENGTH && index != SET_ITEM_OPTION_ADD_DEXTERITY && index != SET_ITEM_OPTION_ADD_VITALITY && index != SET_ITEM_OPTION_ADD_ENERGY && index != SET_ITEM_OPTION_ADD_LEADERSHIP && index != SET_ITEM_OPTION_INCREASE_ALL_STATS)
		{
			return;
		}
	}

	switch(index)
	{
		case SET_ITEM_OPTION_ADD_STRENGTH: //OK
			lpObj->AddStrength += value;
			break;
		case SET_ITEM_OPTION_ADD_DEXTERITY: //OK
			lpObj->AddDexterity += value;
			break;
		case SET_ITEM_OPTION_ADD_VITALITY: //OK
			lpObj->AddVitality += value;
			break;
		case SET_ITEM_OPTION_ADD_ENERGY: //OK
			lpObj->AddEnergy += value;
			break;
		case SET_ITEM_OPTION_ADD_LEADERSHIP: //OK
			lpObj->AddLeadership += value;
			break;
		case SET_ITEM_OPTION_ADD_MIN_PHYSI_DAMAGE: //OK
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			break;
		case SET_ITEM_OPTION_ADD_MAX_PHYSI_DAMAGE: //OK
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
			break;
		case SET_ITEM_OPTION_MUL_MAGIC_DAMAGE: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_MAGIC_DAMAGE_1: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_MAGIC_DAMAGE_2: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_MAGIC_DAMAGE_3: //OK
			lpObj->MagicDamageMin += (lpObj->MagicDamageMin*value)/100;
			lpObj->MagicDamageMax += (lpObj->MagicDamageMax*value)/100;
			break;
		case SET_ITEM_OPTION_ADD_PHYSI_DAMAGE: //OK
		case SET_ITEM_OPTION_INCREASE_DAMAGE: //OK
		case SET_ITEM_OPTION_INCREASE_MAGIC_DAMAGE: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_DAMAGE_1: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_DAMAGE_2: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_DAMAGE_3: //OK
			lpObj->DamagePvP += value;
			break;
		case SET_ITEM_OPTION_MUL_ATTACK_SUCCESS_RATE: //OK
			lpObj->m_MPSkillOpt.AttackSuccessRate += (lpObj->m_MPSkillOpt.AttackSuccessRate*value)/100;
			break;
		case SET_ITEM_OPTION_ADD_DEFENSE: //OK
			lpObj->Defense += value;
			break;
		case SET_ITEM_OPTION_ADD_MAX_LIFE: //OK
			lpObj->AddLife += value;
			break;
		case SET_ITEM_OPTION_ADD_MAX_MANA: //OK
			lpObj->AddMana += value;
			break;
		case SET_ITEM_OPTION_ADD_MAX_BP: //OK
			lpObj->AddBP += value;
			break;
		case SET_ITEM_OPTION_ADD_BP_RECOVERY: //OK
			lpObj->BPRecovery += value;
			break;
		case SET_ITEM_OPTION_ADD_CRITICAL_DAMAGE_RATE: //OK
			lpObj->m_MPSkillOpt.CriticalDamageRate += value;
			break;
		case SET_ITEM_OPTION_ADD_CRITICAL_DAMAGE: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_CRITICAL_DAMAGE_1: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_CRITICAL_DAMAGE_2: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_CRITICAL_DAMAGE_3: //OK
			lpObj->CriticalDamage += value;
			break;
		case SET_ITEM_OPTION_ADD_EXCELLENT_DAMAGE_RATE: //OK
			lpObj->m_MPSkillOpt.ExcellentDamageRate += value;
			break;
		case SET_ITEM_OPTION_ADD_EXCELLENT_DAMAGE: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_EXCELLENT_DAMAGE_1: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_EXCELLENT_DAMAGE_2: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_EXCELLENT_DAMAGE_3: //OK
			lpObj->ExcellentDamage += value;
			break;
		case SET_ITEM_OPTION_ADD_SKILL_DAMAGE: //OK
			lpObj->SkillDamageBonus += value;
			break;
		case SET_ITEM_OPTION_ADD_DOUBLE_DAMAGE_RATE: //OK
			lpObj->m_MPSkillOpt.DoubleDamageRate += value;
			break;		
		case SET_ITEM_OPTION_ADD_IGNORE_DEFENSE_RATE: //OK
			lpObj->m_MPSkillOpt.IgnoreDefenseRate += value;
			break;
		case SET_ITEM_OPTION_MUL_SHIELD_DEFENSE: //OK
			if(lpObj->Inventory[1].m_Index >= GET_ITEM(6,0) && lpObj->Inventory[1].m_Index < GET_ITEM(7,0))
			{
				lpObj->Defense += (lpObj->Defense*value)/100;
			}
			break;
		case SET_ITEM_OPTION_MUL_TWO_HAND_PHYSI_DAMAGE: //OK
			if((lpObj->Inventory[0].IsItem() != 0 && lpObj->Inventory[0].m_TwoHand != 0) || (lpObj->Inventory[1].IsItem() != 0 && lpObj->Inventory[1].m_TwoHand != 0))
			{
				lpObj->PhysiDamageMinLeft += (lpObj->PhysiDamageMinLeft*value)/100;
				lpObj->PhysiDamageMinRight += (lpObj->PhysiDamageMinRight*value)/100;
				lpObj->PhysiDamageMaxLeft += (lpObj->PhysiDamageMaxLeft*value)/100;
				lpObj->PhysiDamageMaxRight += (lpObj->PhysiDamageMaxRight*value)/100;
			}
			break;
		case SET_ITEM_OPTION_INCREASE_ALL_STATS: //OK
			lpObj->AddStrength += value;
			lpObj->AddDexterity += value;
			lpObj->AddVitality += value;
			lpObj->AddEnergy += value;
			lpObj->AddLeadership += (lpObj->Class == CLASS_DL) ? value : 0;
			break;
		case SET_ITEM_OPTION_INCREASE_BASE_DEFENSE: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_BASE_DEFENSE_1: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_BASE_DEFENSE_2: //OK
		case SET_ITEM_OPTION_INCREASE_MASTERY_BASE_DEFENSE_3: //OK
			lpObj->ImpenetrableDefense += value;
			break;
		case SET_ITEM_OPTION_ELF_GREATER_DAMAGE_REINFORCEMENT: //OK
			lpObj->ElfGreaterDamageReinforcement += value;
			break;
		case SET_ITEM_OPTION_ELF_GREATER_DEFENSE_REINFORCEMENT: //OK
			lpObj->ElfGreaterDefenseReinforcement += value;
			break;
		case SET_ITEM_OPTION_ELF_BLESS_REINFORCEMENT: //OK
			lpObj->ElfBlessReinforcement += value;
			break;
		case SET_ITEM_OPTION_INCREASE_TRIPLE_DAMAGE_RATE: //OK
			lpObj->m_MPSkillOpt.TripleDamageRate += value;
			break;

		case SET_ITEM_OPTION_AUMENTO_DANO_MAESTRAL: //OK
		case SET_ITEM_OPTION_AUMENTO_DANO_MAGICO_MAESTRAL: //OK
			lpObj->AddGeneralDamageRate += value;
			break;
		
		case SET_ITEM_OPTION_REDUCAO_DANO_MAESTRAL: //OK
			lpObj->ReduceGeneralDamageRate += value;
			break;

		case SET_ITEM_OPTION_AUMENTO_DANO_EXCELENTE_MAESTRAL: //OK
			lpObj->ExcellentDamagePerc += value;
			break;

		case SET_ITEM_OPTION_AUMENTO_DANO_CRITICO_MAESTRAL: //OK
			lpObj->CriticalDamagePerc += value;
			break;

		case SET_ITEM_OPTION_INCREASE_DAMAGE_BY_STRENGTH: //OK
			lpObj->PhysiDamageMinLeft += ((value == 0) ? 0 : (lpObj->Strength / value));
			lpObj->PhysiDamageMinRight += ((value == 0) ? 0 : (lpObj->Strength / value));
			lpObj->PhysiDamageMaxLeft += ((value == 0) ? 0 : (lpObj->Strength / value));
			lpObj->PhysiDamageMaxRight += ((value == 0) ? 0 : (lpObj->Strength / value));
			break;
		case SET_ITEM_OPTION_INCREASE_DAMAGE_BY_AGILITY: //OK
			lpObj->PhysiDamageMinLeft += ((value == 0) ? 0 : (lpObj->Dexterity / value));
			lpObj->PhysiDamageMinRight += ((value == 0) ? 0 : (lpObj->Dexterity / value));
			lpObj->PhysiDamageMaxLeft += ((value == 0) ? 0 : (lpObj->Dexterity / value));
			lpObj->PhysiDamageMaxRight += ((value == 0) ? 0 : (lpObj->Dexterity / value));
			break;
		case SET_ITEM_OPTION_INCREASE_DEFENSE_BY_AGILITY: //OK
			lpObj->Defense += ((value == 0) ? 0 : (lpObj->Dexterity / value));
			break;
		case SET_ITEM_OPTION_INCREASE_DEFENSE_BY_VITALITY: //OK
			lpObj->Defense += ((value == 0) ? 0 : (lpObj->Vitality / value));
			break;
		case SET_ITEM_OPTION_INCREASE_DAMAGE_BY_ENERGY: //OK
			lpObj->MagicDamageMin += ((value == 0) ? 0 : (lpObj->Energy / value));
			lpObj->MagicDamageMax += ((value == 0) ? 0 : (lpObj->Energy / value));
			break;
		case SET_ITEM_OPTION_INCREASE_ICE_DAMAGE: //OK
			lpObj->AddResistance[0] += value;
			break;
		case SET_ITEM_OPTION_INCREASE_POISON_DAMAGE: //OK
			lpObj->AddResistance[1] += value;
			break;
		case SET_ITEM_OPTION_INCREASE_LIGHTNING_DAMAGE: //OK
			lpObj->AddResistance[2] += value;
			break;
		case SET_ITEM_OPTION_INCREASE_FIRE_DAMAGE: //OK
			lpObj->AddResistance[3] += value;
			break;
		case SET_ITEM_OPTION_INCREASE_EARTH_DAMAGE: //OK
			lpObj->AddResistance[4] += value;
			break;
		case SET_ITEM_OPTION_INCREASE_WIND_DAMAGE: //OK
			lpObj->AddResistance[5] += value;
			break;
		case SET_ITEM_OPTION_INCREASE_WATER_DAMAGE: //OK
			lpObj->AddResistance[6] += value;
			break;
	}
}