#include "stdafx.h"
#include "MemScript.h"
#include "Util.h"
#include "MasteryItemExtOptionSet.h"
#include "ItemManager.h"
#include "ServerInfo.h"

MasteryItemExtOptionSet gMasteryItemExtOptionSet;

MasteryItemExtOptionSet::MasteryItemExtOptionSet()
{
	
}


MasteryItemExtOptionSet::~MasteryItemExtOptionSet()
{
	
}

void MasteryItemExtOptionSet::Load(char* path)
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

	memset(this->m_MasteryItemExtOptionSet, -1, sizeof(this->m_MasteryItemExtOptionSet));

	int count = 0;

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

			m_MasteryItemExtOptionSet[count].ItemType = lpMemScript->GetNumber();

			m_MasteryItemExtOptionSet[count].ItemIndex = lpMemScript->GetAsNumber();

			for (int i = 0; i < 6; i++)
			{
				m_MasteryItemExtOptionSet[count].ExtOption[i]= lpMemScript->GetAsNumber();
			}

			m_MasteryItemExtOptionSet[count].Rate = lpMemScript->GetAsNumber();

			count++;
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

BOOL  MasteryItemExtOptionSet::RandMasteryItemExtOption(int iType, int iIndex, BYTE *ExtOp)
{
	int iAddRate = 0;

	for (int i = 0; i < 100; i++)
	{
		if (this->m_MasteryItemExtOptionSet[i].ItemType == iType && this->m_MasteryItemExtOptionSet[i].ItemIndex == iIndex)
		{
			if (GetLargeRand() % 1000000 < this->m_MasteryItemExtOptionSet[i].Rate + iAddRate)
			{
				for (int j = 0; j < 6; j++)
				{
					ExtOp[j] = this->m_MasteryItemExtOptionSet[i].ExtOption[j];
				}

				return 1;
			}

			iAddRate += this->m_MasteryItemExtOptionSet[i].Rate;
		}
	}

	return 0;
}

BOOL MasteryItemExtOptionSet::IsMasteryItem(int Index)
{
	for (int i = 0; i < 100; i++)
	{
		if (GET_ITEM(this->m_MasteryItemExtOptionSet[i].ItemType,this->m_MasteryItemExtOptionSet[i].ItemIndex) == Index)
		{
			return 1;
		}
	}

	return 0;
}

BOOL MasteryItemExtOptionSet::IsMasterySet(int Index)
{
	if (Index > GET_ITEM(7,0) && Index < GET_ITEM(12,0))
	{
		if ((ITEM_GET_INDEX(Index) >= 98 && ITEM_GET_INDEX(Index) <= 115) 
			|| (ITEM_GET_INDEX(Index) >= 138 && ITEM_GET_INDEX(Index) <= 145) 
			|| (ITEM_GET_INDEX(Index) >= 150 && ITEM_GET_INDEX(Index) <= 173)
			)
		{
			return 1;
		}
	}

	return 0;
}

BOOL MasteryItemExtOptionSet::IsMasteryWeapon(int Index)
{
	if (Index < GET_ITEM(6, 0))
	{
		if ((Index >= GET_ITEM(0, 42) && Index <= GET_ITEM(0, 46)) ||
			(Index >= GET_ITEM(0, 54) && Index <= GET_ITEM(0, 59)) ||
			(Index >= GET_ITEM(0, 62) && Index <= GET_ITEM(0, 64)) ||
			(Index >= GET_ITEM(0, 78) && Index <= GET_ITEM(0, 80)) ||
			Index == GET_ITEM(2, 22) || Index == GET_ITEM(2, 26) || Index == GET_ITEM(2, 27) || Index == GET_ITEM(2, 36) || Index == GET_ITEM(2, 40) ||
			Index == GET_ITEM(3, 19) || Index == GET_ITEM(3, 24) || Index == GET_ITEM(3, 25) || Index == GET_ITEM(3, 28) || Index == GET_ITEM(3, 29) ||
			Index == GET_ITEM(4, 28) || Index == GET_ITEM(4, 31) ||
			(Index >= GET_ITEM(4, 34) && Index <= GET_ITEM(4, 41)) ||
			(Index >= GET_ITEM(5, 41) && Index <= GET_ITEM(5, 43)) ||
			(Index >= GET_ITEM(5, 51) && Index <= GET_ITEM(5, 63))
		)
		{
			return 1;
		}
	}

	return 0;
}

BOOL MasteryItemExtOptionSet::IsMasteryShield(int Index)
{
	if (Index >= GET_ITEM(6, 44) && Index <= GET_ITEM(6, 48))
	{
		return 1;
	}

	return 0;
}

BOOL MasteryItemExtOptionSet::IsMasteryRing(int Index)
{
	if (Index >= GET_ITEM(13, 173) && Index <= GET_ITEM(13, 176))
	{
		return 1;
	}

	return 0;
}

BOOL MasteryItemExtOptionSet::IsMasteryPendant(int Index)
{
	if (Index >= GET_ITEM(13, 171) && Index <= GET_ITEM(13, 172))
	{
		return 1;
	}

	return 0;
}

void MasteryItemExtOptionSet::CalcMasteryOption(LPOBJ lpObj, CItem * lpItem) // OK
{
	if (lpItem->m_Index < GET_ITEM(6, 0))
	{
		return;
	}

	if (lpItem->m_Index >= GET_ITEM(7, 0) && !lpItem->IsSetItem())
	{
		return;
	}

	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] != 255)
		{
			switch (lpItem->m_SocketOption[n])
			{
		//Sets
			case MASTERY_OPTION_DAMAGE_AGAINST_DW:
			case MASTERY_OPTION_DAMAGE_AGAINST_DK:
			case MASTERY_OPTION_DAMAGE_AGAINST_FE:
			case MASTERY_OPTION_DAMAGE_AGAINST_MG:
			case MASTERY_OPTION_DAMAGE_AGAINST_DL:
			case MASTERY_OPTION_DAMAGE_AGAINST_SU:
			case MASTERY_OPTION_DAMAGE_AGAINST_RF:
			case MASTERY_OPTION_DAMAGE_AGAINST_GL:
				lpObj->AddGeneralDamageRateByClass[lpItem->m_SocketOption[n]-6] += gServerInfo.MASTERY_OPTION_DAMAGE_AGAINST_[lpItem->m_SocketOption[n]-6];
				break;

			case MASTERY_OPTION_DAMAGE_AGAINST_DW2:
			case MASTERY_OPTION_DAMAGE_AGAINST_DK2:
			case MASTERY_OPTION_DAMAGE_AGAINST_FE2:
			case MASTERY_OPTION_DAMAGE_AGAINST_MG2:
			case MASTERY_OPTION_DAMAGE_AGAINST_DL2:
			case MASTERY_OPTION_DAMAGE_AGAINST_SU2:
			case MASTERY_OPTION_DAMAGE_AGAINST_RF2:
			case MASTERY_OPTION_DAMAGE_AGAINST_GL2:
				lpObj->AddGeneralDamageRateByClass[lpItem->m_SocketOption[n] - 46] += gServerInfo.MASTERY_OPTION_DAMAGE_AGAINST_2[lpItem->m_SocketOption[n] - 46];
				break;

			case MASTERY_OPTION_INCREASE_BOSS_DAMAGE:
				lpObj->AddBossDamage += gServerInfo.MASTERY_OPTION_INCREASE_BOSS_DAMAGE;
				break;
			case MASTERY_OPTION_INCREASE_BOSS_DAMAGE2:
				lpObj->AddBossDamage += gServerInfo.MASTERY_OPTION_INCREASE_BOSS_DAMAGE2;
				break;

			case MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE:
				lpObj->AddElementalDamageRate += gServerInfo.MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE;
				break;
			case MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE2:
				lpObj->AddElementalDamageRate += gServerInfo.MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE2;
				break;

		//Shields
			case MASTERY_OPTION_PVP_DAMAGE_REDUCTION:
				lpObj->ReduceGeneralDamageRate += gServerInfo.MASTERY_OPTION_PVP_DAMAGE_REDUCTION;
				break;
			case MASTERY_OPTION_ELEMENTAL_DAMAGE_REDUCTION:
				lpObj->ReduceGeneralElementalDamageRate += gServerInfo.MASTERY_OPTION_ELEMENTAL_DAMAGE_REDUCTION;
				break;
			case MASTERY_OPTION_DOUBLE_DAMAGE_PROTECTION:
				lpObj->ResistDoubleDamageRate += gServerInfo.MASTERY_OPTION_DOUBLE_DAMAGE_PROTECTION;
				break;
			case MASTERY_OPTION_TRIPLE_DAMAGE_PROTECTION:
				lpObj->ResistTripleDamageRate += gServerInfo.MASTERY_OPTION_TRIPLE_DAMAGE_PROTECTION;
				break;
			case MASTERY_OPTION_TRUE_DAMAGE_PROTECTION:
				lpObj->TrueDamageProtection += gServerInfo.MASTERY_OPTION_TRUE_DAMAGE_PROTECTION;
				break;
			case MASTERY_OPTION_ULTRAHIT_PROTECTION:
				lpObj->UltraHitProtection += gServerInfo.MASTERY_OPTION_ULTRAHIT_PROTECTION;
				break;

			//Shields Blue Eye
			case MASTERY_OPTION_PVP_DAMAGE_REDUCTION_2:
				lpObj->ReduceGeneralDamageRate += gServerInfo.MASTERY_OPTION_PVP_DAMAGE_REDUCTION_2;
				break;
			case MASTERY_OPTION_ELEMENTAL_DAMAGE_REDUCTION_2:
				lpObj->ReduceGeneralElementalDamageRate += gServerInfo.MASTERY_OPTION_ELEMENTAL_DAMAGE_REDUCTION_2;
				break;
			case MASTERY_OPTION_DOUBLE_DAMAGE_PROTECTION_2:
				lpObj->ResistDoubleDamageRate += gServerInfo.MASTERY_OPTION_DOUBLE_DAMAGE_PROTECTION_2;
				break;
			case MASTERY_OPTION_TRIPLE_DAMAGE_PROTECTION_2:
				lpObj->ResistTripleDamageRate += gServerInfo.MASTERY_OPTION_TRIPLE_DAMAGE_PROTECTION_2;
				break;
			case MASTERY_OPTION_TRUE_DAMAGE_PROTECTION_2:
				lpObj->TrueDamageProtection += gServerInfo.MASTERY_OPTION_TRUE_DAMAGE_PROTECTION_2;
				break;
			case MASTERY_OPTION_ULTRAHIT_PROTECTION_2:
				lpObj->UltraHitProtection += gServerInfo.MASTERY_OPTION_ULTRAHIT_PROTECTION_2;
				break;
			}
		}
	}

	if (lpItem->m_SocketOption[4] >= 1 && lpItem->m_SocketOption[4] <= 16)
	{
		switch (lpItem->m_SocketOption[4])
		{
		//Sets
		case MASTERY_BONUS_TRIPLE_DAMAGE_RATE:
			lpObj->m_MPSkillOpt.TripleDamageRate += gServerInfo.MASTERY_BONUS_TRIPLE_DAMAGE_RATE;
			break;
		case MASTERY_BONUS_TRIPLE_DAMAGE_RATE2:
			lpObj->m_MPSkillOpt.TripleDamageRate += gServerInfo.MASTERY_BONUS_TRIPLE_DAMAGE_RATE2;
			break;
		case MASTERY_BONUS_FINAL_DAMAGE:
			lpObj->AddGeneralDamageRate += gServerInfo.MASTERY_BONUS_FINAL_DAMAGE;
			break;
		case MASTERY_BONUS_FINAL_DAMAGE2:
			lpObj->AddGeneralDamageRate += gServerInfo.MASTERY_BONUS_FINAL_DAMAGE2;
			break;
		case MASTERY_BONUS_ULTRAHIT_50:
			lpObj->UltraHit50perChance += gServerInfo.MASTERY_BONUS_ULTRAHIT_50;
			break;
		case MASTERY_BONUS_ULTRAHIT_502:
			lpObj->UltraHit50perChance += gServerInfo.MASTERY_BONUS_ULTRAHIT_502;
			break;
		case MASTERY_BONUS_DOUBLE_DAMAGE_BOSS:
			lpObj->BossDoubleDamageRate += gServerInfo.MASTERY_BONUS_DOUBLE_DAMAGE_BOSS;
			break;
		case MASTERY_BONUS_DOUBLE_DAMAGE_BOSS2:
			lpObj->BossDoubleDamageRate += gServerInfo.MASTERY_BONUS_DOUBLE_DAMAGE_BOSS2;
			break;

		//Shields
		case MASTERY_BONUS_HP_DAMAGE_PROTECTION:
			lpObj->UltraHitProtection += gServerInfo.MASTERY_BONUS_HP_DAMAGE_PROTECTION;
			break;
		case MASTERY_BONUS_REFLECT_INCREASE:
			lpObj->AddPlusDamageReflect += gServerInfo.MASTERY_BONUS_REFLECT_INCREASE;
			break;
		case MASTERY_BONUS_BOSS_POWER_PROTECTION:
			lpObj->BossSkillsResistance += gServerInfo.MASTERY_BONUS_BOSS_POWER_PROTECTION;
			break;
		//Shields Blue Eye
		case MASTERY_BONUS_HP_DAMAGE_PROTECTION_2:
			lpObj->UltraHitProtection += gServerInfo.MASTERY_BONUS_HP_DAMAGE_PROTECTION_2;
			break;
		case MASTERY_BONUS_REFLECT_INCREASE_2:
			lpObj->AddPlusDamageReflect += gServerInfo.MASTERY_BONUS_REFLECT_INCREASE_2;
			break;
		case MASTERY_BONUS_BOSS_POWER_PROTECTION_2:
			lpObj->BossSkillsResistance += gServerInfo.MASTERY_BONUS_BOSS_POWER_PROTECTION_2;
			break;
		}
	}
}

void MasteryItemExtOptionSet::CalcMasteryWeaponOption(LPOBJ lpObj, CItem* lpItem) // OK
{
	if (lpItem->m_Index >= GET_ITEM(6, 0))
	{
		return;
	}

	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] != 255)
		{
			switch (lpItem->m_SocketOption[n])
			{
			case MASTERY_OPTION_BOSS_DECREASE_DELAY:
				lpObj->DecBossHitDelay += gServerInfo.MASTERY_OPTION_BOSS_DECREASE_DELAY;
				break;
			case MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE_WEAPON:
				lpObj->AddElementalDamageRate += gServerInfo.MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE_WEAPON;
				break;
			case MASTERY_OPTION_BOSS_DAMAGE_INCREASE:
				lpObj->AddBossDamage += gServerInfo.MASTERY_OPTION_BOSS_DAMAGE_INCREASE;
				break;
			case MASTERY_OPTION_PVP_DAMAGE_INCREASE:
				lpObj->AddPVPDamageRate += gServerInfo.MASTERY_OPTION_PVP_DAMAGE_INCREASE;
				break;
			case MASTERY_OPTION_DOUBLE_DAMAGE_RATE:
				lpObj->m_MPSkillOpt.DoubleDamageRate += gServerInfo.MASTERY_OPTION_DOUBLE_DAMAGE_RATE;
				break;
			case MASTERY_OPTION_TRIPLE_DAMAGE_RATE:
				lpObj->m_MPSkillOpt.TripleDamageRate += gServerInfo.MASTERY_OPTION_TRIPLE_DAMAGE_RATE;
				break;
			case MASTERY_OPTION_TRUE_DAMAGE_RATE:
				lpObj->TrueDamageRate += gServerInfo.MASTERY_OPTION_TRUE_DAMAGE_RATE;
				break;
			case MASTERY_OPTION_30P_HP_ULTRAHIT:
				lpObj->UltraHit30perChance += gServerInfo.MASTERY_OPTION_30P_HP_ULTRAHIT;
				break;

			case MASTERY_OPTION_BOSS_DECREASE_DELAY_2:
				lpObj->DecBossHitDelay += gServerInfo.MASTERY_OPTION_BOSS_DECREASE_DELAY_2;
				break;
			case MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE_WEAPON_2:
				lpObj->AddElementalDamageRate += gServerInfo.MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE_WEAPON_2;
				break;
			case MASTERY_OPTION_BOSS_DAMAGE_INCREASE_2:
				lpObj->AddBossDamage += gServerInfo.MASTERY_OPTION_BOSS_DAMAGE_INCREASE_2;
				break;
			case MASTERY_OPTION_PVP_DAMAGE_INCREASE_2:
				lpObj->AddPVPDamageRate += gServerInfo.MASTERY_OPTION_PVP_DAMAGE_INCREASE_2;
				break;
			case MASTERY_OPTION_DOUBLE_DAMAGE_RATE_2:
				lpObj->m_MPSkillOpt.DoubleDamageRate += gServerInfo.MASTERY_OPTION_DOUBLE_DAMAGE_RATE_2;
				break;
			case MASTERY_OPTION_TRIPLE_DAMAGE_RATE_2:
				lpObj->m_MPSkillOpt.TripleDamageRate += gServerInfo.MASTERY_OPTION_TRIPLE_DAMAGE_RATE_2;
				break;
			case MASTERY_OPTION_TRUE_DAMAGE_RATE_2:
				lpObj->TrueDamageRate += gServerInfo.MASTERY_OPTION_TRUE_DAMAGE_RATE_2;
				break;
			case MASTERY_OPTION_30P_HP_ULTRAHIT_2:
				lpObj->UltraHit30perChance += gServerInfo.MASTERY_OPTION_30P_HP_ULTRAHIT_2;
				break;
			}
		}
	}

	if (lpItem->m_SocketOption[4] != 0xFF)
	{
		switch (lpItem->m_SocketOption[4])
		{
		case MASTERY_BONUS_ULTRAHIT_50_WEAPON:
			lpObj->UltraHit50perChance += gServerInfo.MASTERY_BONUS_ULTRAHIT_50_WEAPON;
			break;
		case MASTERY_BONUS_ULTRAHIT_50_WEAPON_2:
			lpObj->UltraHit50perChance += gServerInfo.MASTERY_BONUS_ULTRAHIT_50_WEAPON_2;
			break;
		case MASTERY_BONUS_100P_EXCELLENT:
			lpObj->ExcellentDamageOnly = true;
			break;
		case MASTERY_BONUS_REFLECT_DECREASE:
			lpObj->SubPlusDamageReflect += gServerInfo.MASTERY_BONUS_REFLECT_DECREASE;
			break;
		case MASTERY_BONUS_REFLECT_DECREASE_2:
			lpObj->SubPlusDamageReflect += gServerInfo.MASTERY_BONUS_REFLECT_DECREASE_2;
			break;
		case MASTERY_BONUS_CURE_HARM:
			lpObj->CureHarmRate += gServerInfo.MASTERY_BONUS_CURE_HARM;
			break;
		case MASTERY_BONUS_CURE_HARM_2:
			lpObj->CureHarmRate += gServerInfo.MASTERY_BONUS_CURE_HARM_2;
			break;
		case MASTERY_BONUS_STERN_RATE:
			lpObj->SternRate += gServerInfo.MASTERY_BONUS_STERN_RATE;
			break;
		case MASTERY_BONUS_STERN_RATE_2:
			lpObj->SternRate += gServerInfo.MASTERY_BONUS_STERN_RATE_2;
			break;
		case MASTERY_BONUS_BLEEDING:
			lpObj->BleedingRate += gServerInfo.MASTERY_BONUS_BLEEDING;
			break;
		case MASTERY_BONUS_BLEEDING_2:
			lpObj->BleedingRate += gServerInfo.MASTERY_BONUS_BLEEDING_2;
			break;
		case MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON:
			lpObj->BossDoubleDamageRate += gServerInfo.MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON;
			break;
		case MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON_2:
			lpObj->BossDoubleDamageRate += gServerInfo.MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON_2;
			break;
		case MASTERY_BONUS_FREEZING_DAMAGE:
			lpObj->FreezingDamageRate += gServerInfo.MASTERY_BONUS_FREEZING_DAMAGE;
			break;
		}
	}
}

void MasteryItemExtOptionSet::CalcMasteryPendantOption(LPOBJ lpObj, CItem* lpItem) // OK
{
	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] != 255)
		{
			switch (lpItem->m_SocketOption[n])
			{
				case MASTERY_OPTION_PENDANT_PVP_DMG:
					lpObj->AddPVPDamageRate += gServerInfo.MASTERY_OPTION_PENDANT_PVP_DMG;
					break;
				case MASTERY_OPTION_PENDANT_TRIPLE_DMG:
					lpObj->m_MPSkillOpt.TripleDamageRate += gServerInfo.MASTERY_OPTION_PENDANT_TRIPLE_DMG;
					break;
				case MASTERY_OPTION_PENDANT_TRUE_DMG:
					lpObj->TrueDamageRate += gServerInfo.MASTERY_OPTION_PENDANT_TRUE_DMG;
					break;
				case MASTERY_OPTION_PENDANT_DOUBLE_DMG:
					lpObj->m_MPSkillOpt.DoubleDamageRate += gServerInfo.MASTERY_OPTION_PENDANT_DOUBLE_DMG;
					break;
				case MASTERY_OPTION_PENDANT_BOSS_DMG:
					lpObj->AddBossDamage += gServerInfo.MASTERY_OPTION_PENDANT_BOSS_DMG;
					break;
				case MASTERY_OPTION_PENDANT_BOSS_DECREASE_DELAY:
					lpObj->DecBossHitDelay += gServerInfo.MASTERY_OPTION_BOSS_DECREASE_DELAY;
					break;
				case MASTERY_OPTION_PENDANT_ELEMENTAL_DAMAGE_INCREASE:
					lpObj->AddElementalDamageRate += gServerInfo.MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE_WEAPON;
					break;
			}
		}
	}

	if (lpItem->m_SocketOption[4] != 0xFF)
	{
		switch (lpItem->m_SocketOption[4])
		{
		case MASTERY_BONUS_PENDANT_CURE_HARM:
			lpObj->CureHarmRate += gServerInfo.MASTERY_BONUS_CURE_HARM;
			break;

		case MASTERY_BONUS_PENDANT_STERN:
			lpObj->SternRate += gServerInfo.MASTERY_BONUS_STERN_RATE;
			break;

		case MASTERY_BONUS_PENDANT_BLEEDING:
			lpObj->BleedingRate += gServerInfo.MASTERY_BONUS_BLEEDING;
			break;

		case MASTERY_BONUS_PENDANT_SHIELD_SKILL_IGNORE:
			lpObj->AvoidShieldSkill = 1;
			break;

		case MASTERY_BONUS_PENDANT_ULTRAHIT_50:
			lpObj->UltraHit50perChance += gServerInfo.MASTERY_BONUS_ULTRAHIT_50_WEAPON;
			break;
		}
	}
}

void MasteryItemExtOptionSet::CalcMasteryRingOption(LPOBJ lpObj, CItem* lpItem) // OK
{
	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] != 255)
		{
			switch (lpItem->m_SocketOption[n])
			{
			case MASTERY_OPTION_RING_BACKSPRING_PROTECTION:
				lpObj->BackSpringProtection += gServerInfo.MASTERY_OPTION_RING_BACKSPRING_PROTECTION;
				break;

			case MASTERY_OPTION_RING_DROP_BOSS_INC:
				lpObj->BossDropIncrease += gServerInfo.MASTERY_OPTION_RING_DROP_BOSS_INC;
				break;

			case MASTERY_OPTION_RING_MAX_HP_INC:
				lpObj->SocketAddLife += gServerInfo.MASTERY_OPTION_RING_MAX_HP_INC;
				break;

			case MASTERY_OPTION_RING_BLOCK_CHANCE_INC:
				lpObj->BlockRate += gServerInfo.MASTERY_OPTION_RING_BLOCK_CHANCE_INC;
				break;

			case MASTERY_OPTION_RING_BOSS_SKILL_PROTECTION:
				lpObj->BossSkillsResistance += gServerInfo.MASTERY_OPTION_RING_BOSS_SKILL_PROTECTION;
				break;

			case MASTERY_OPTION_RING_ELEMENTAL_DMG_REDUCE:
				lpObj->ReduceGeneralElementalDamageRate += gServerInfo.MASTERY_OPTION_RING_ELEMENTAL_DMG_REDUCE;
				break;

			case MASTERY_OPTION_RING_PVP_DAMAGE_REDUCE:
				lpObj->ReduceGeneralDamageRate += gServerInfo.MASTERY_OPTION_RING_PVP_DAMAGE_REDUCE;
				break;

			case MASTERY_OPTION_RING_BOSS_DAMAGE_REDUCE:
				lpObj->DecBossDamage += gServerInfo.MASTERY_OPTION_RING_BOSS_DAMAGE_REDUCE;
				break;

			case MASTERY_OPTION_RING_TRIPLE_DMG_PROTECTION:
				lpObj->ResistTripleDamageRate += gServerInfo.MASTERY_OPTION_RING_TRIPLE_DMG_PROTECTION;
				break;

			case MASTERY_OPTION_RING_TRUE_DMG_PROTECTION:
				lpObj->TrueDamageProtection += gServerInfo.MASTERY_OPTION_RING_TRUE_DMG_PROTECTION;
				break;				
			}
		}
	}

	if (lpItem->m_SocketOption[4] >= 20 && lpItem->m_SocketOption[4] <= 40)
	{
		switch (lpItem->m_SocketOption[4])
		{

		case MASTERY_BONUS_RING_STERN_RESIST:
			lpObj->SternResist += gServerInfo.MASTERY_BONUS_RING_STERN_RESIST;
			break;

		case MASTERY_BONUS_RING_BLEEDING_RESIST:
			lpObj->BleedingResist += gServerInfo.MASTERY_BONUS_RING_BLEEDING_RESIST;
			break;

		case MASTERY_BONUS_RING_CURE_HARM_RESIST:
			lpObj->CureHarmResist += gServerInfo.MASTERY_BONUS_RING_CURE_HARM_RESIST;
			break;

		case MASTERY_BONUS_RING_CURE_IMPROVE:
			lpObj->CureImproveRate += gServerInfo.MASTERY_BONUS_RING_CURE_IMPROVE;
			break;

		case MASTERY_BONUS_RING_FULL_REFLECT_INC:
			lpObj->m_MPSkillOpt.FullDamageReflectRate += gServerInfo.MASTERY_BONUS_RING_FULL_REFLECT_INC;
			break;

		case MASTERY_BONUS_RING_MASTERY_REVERT:
			lpObj->MasteryBonusRevertRate += gServerInfo.MASTERY_BONUS_RING_MASTERY_REVERT;
			break;
		}
	}
}