#include "stdafx.h"
#include "Wings4thOption.h"
#include "MemScript.h"
#include "Util.h"
#include "ItemManager.h"

CWings4thOption gWings4thOption;

CWings4thOption::CWings4thOption() // OK
{

}

CWings4thOption::~CWings4thOption() // OK
{

}

void CWings4thOption::Load()
{
	CMemScript* lpScript = new CMemScript;

	if (lpScript == 0)
	{
		return;
	}

	if (lpScript->SetBuffer("..\\Data\\Item\\Wing4thOption.txt") == 0)
	{
		delete lpScript;
		return;
	}

	try
	{
		while (true)
		{
			if (lpScript->GetToken() == 2)
			{
				break;
			}

			int section = lpScript->GetNumber();

			while (true)
			{
				if (section == 1)
				{
					if (strcmp("end", lpScript->GetAsString()) == 0)
					{
						break;
					}

					lpScript->GetNumber();

					for (int i = 0; i < 16; i++)
					{
						Wing4thElementalBasic[i] = lpScript->GetAsNumber();
					}
				}
				else if (section == 2)
				{
					if (strcmp("end", lpScript->GetAsString()) == 0)
					{
						break;
					}

					WING4THELEMENT info;

					memset(&info, 0, sizeof(info));

					info.index = lpScript->GetNumber();

					for (int i = 0; i < 16; i++)
					{
						info.level[i] = lpScript->GetAsNumber();
					}

					this->SetElementalInfo(info);
				}
				else if (section == 3)
				{
					if (strcmp("end", lpScript->GetAsString()) == 0)
					{
						break;
					}

					WING4THOPTION info;

					memset(&info, 0, sizeof(info));

					info.index = lpScript->GetNumber();

					for (int i = 0; i < 10; i++)
					{
						info.level[i] = lpScript->GetAsNumber();
					}

					this->SetInfo(info);
				}
			}			
		}
	}
	catch (...)
	{
		MessageBox(0, "Wing4thOption", "Error", 0);
	}

	delete lpScript;
}

void CWings4thOption::SetInfo(WING4THOPTION info) // OK
{
	this->Wing4thOption[info.index] = info;
}

void CWings4thOption::SetElementalInfo(WING4THELEMENT info) // OK
{
	this->Wing4thElemental[info.index] = info;
}

void CWings4thOption::Calc4thWingOption(LPOBJ lpObj, bool flag) // OK
{
	if (lpObj->Inventory[7].IsItem() == 0)
	{
		return;
	}

	CItem* lpItem = &lpObj->Inventory[7];

	if (lpItem->m_Index == GET_ITEM(12, 266))
	{
		lpObj->ReduceGeneralDamageRate += (float)this->Wing4thOption[WING4TH_OPTION_REDUCAO_DANO_FINAL].level[9];
		lpObj->m_MPSkillOpt.OffensiveFullHPRestoreRate += this->Wing4thOption[WING4TH_OPTION_HP_RECOVERY].level[9];
		lpObj->DamageReflect += this->Wing4thOption[WING4TH_OPTION_REFLECT].level[9];
		lpObj->m_MPSkillOpt.IgnoreDefenseRate += this->Wing4thOption[WING4TH_OPTION_IGNORE_DEFENSE].level[9];
		lpObj->PhysiSpeed += this->Wing4thOption[WING4TH_OPTION_REDUCAO_DELAY_BOSS].level[9];
		lpObj->MagicSpeed += this->Wing4thOption[WING4TH_OPTION_REDUCAO_DELAY_BOSS].level[9];
		lpObj->DecBossHitDelay += this->Wing4thOption[WING4TH_OPTION_REDUCAO_DELAY_BOSS].level[9];
		lpObj->m_MPSkillOpt.ExcellentDamageRate += this->Wing4thOption[WING4TH_OPTION_EXCELLENT_DAMAGE_RATE].level[9];
		lpObj->m_MPSkillOpt.DoubleDamageRate += this->Wing4thOption[WING4TH_OPTION_DOUBLE_DAMAGE_RATE].level[9];
		lpObj->AddGeneralDamageRate += this->Wing4thOption[WING4TH_OPTION_ADICIONAL_DANO_FINAL].level[9];
		lpObj->AddBossDamage += this->Wing4thOption[WING4TH_OPTION_ADICIONAL_DANO_BOSS].level[9];
		lpObj->BossSkillsResistance += this->Wing4thOption[WING4TH_OPTION_RESISTENCIA_BOSS].level[9];
		lpObj->BossDropIncrease += this->Wing4thOption[WING4TH_OPTION_BOSS_DROP_INCREASE].level[9];

		//lpObj->ReduceGeneralElementalDamageRate += this->Wing4thElementalBasic[15];
		//lpObj->AddElementalDamageRate += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_ELEMENTAL_PVP].level[15];
		lpObj->AddPunishRate += this->Wing4thElemental[WING4TH_ELEMENTAL_CHANCE_PUNISH].level[15];
		lpObj->AddRadianceResistance += this->Wing4thElemental[WING4TH_ELEMENTAL_RESISTENCIA_RADIANCE].level[15];
		lpObj->AddElementalDamageRate += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_ELEMENTAL_PVP_ELITE].level[15];
		lpObj->ReduceGeneralElementalDamageRate += this->Wing4thElemental[WING4TH_ELEMENTAL_DEFESA_ELEMENTAL_PVP].level[15];
		//lpObj->AddBossElementalDamage += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_ELEMENTAL_BOSS].level[15];
		//lpObj->DecBossElementalDamage += this->Wing4thElemental[WING4TH_ELEMENTAL_DEFESA_ELEMENTAL_BOSS].level[15];
		lpObj->PentagramJewelOption.MulElementalDamageVsFire += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_CONTRA_FOGO].level[15];
		lpObj->PentagramJewelOption.MulElementalDamageVsWater += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_CONTRA_AGUA].level[15];
		lpObj->PentagramJewelOption.MulElementalDamageVsEarth += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_CONTRA_TERRA].level[15];
		lpObj->PentagramJewelOption.MulElementalDamageVsWind += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_CONTRA_VENTO].level[15];
		lpObj->PentagramJewelOption.MulElementalDamageVsDark += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_CONTRA_ESCURIDAO].level[15];
		lpObj->AddBossElementalDamage += this->Wing4thElemental[WING4TH_ELEMENTAL_DANO_ELEMENTAL_BOSS_ELITE].level[15];
		lpObj->DecBossElementalDamage += this->Wing4thElemental[WING4TH_ELEMENTAL_DEFESA_ELEMENTAL_BOSS_ELITE].level[15];
		
		return;
	}

	if (lpItem->m_Index < GET_ITEM(12, 414) || lpItem->m_Index > GET_ITEM(12, 421)) //4th Wings
	{
		return;
	}

	int Option;
	int Level;

	bool existingOptions[15] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };

	for (int i = 0; i < 4; i++)
	{
		if (lpItem->m_SocketOption[i] != 254 && lpItem->m_SocketOption[i] != 255)
		{
			Option = (lpItem->m_SocketOption[i] / 16);
			Level = (lpItem->m_SocketOption[i] % 16);

			if (Level < 0 || Level > 9)
			{
				continue;
			}

			if (flag == 0)
			{
				if (Option >= WING4TH_OPTION_ADITIONAL_STRENGTH && Option <= WING4TH_OPTION_ADITIONAL_AGILITY)
				{
					continue;
				}
			}
			else
			{
				if (Option < WING4TH_OPTION_ADITIONAL_STRENGTH || Option > WING4TH_OPTION_ADITIONAL_AGILITY)
				{
					continue;
				}
			}

			if (Option < 0 || Option > 14 || existingOptions[Option] == true)
			{
				lpItem->m_SocketOption[i] = 255;
				gItemManager.GCItemModifySend(lpObj->Index, 7);
				continue;
			}

			existingOptions[Option] = true;

			switch (Option)
			{
			case WING4TH_OPTION_REDUCAO_DANO_FINAL: //OK
				lpObj->ReduceGeneralDamageRate += (float)this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_HP_RECOVERY: //OK
				lpObj->m_MPSkillOpt.OffensiveFullHPRestoreRate += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_REFLECT: //OK
				lpObj->DamageReflect += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_IGNORE_DEFENSE: //OK
				lpObj->m_MPSkillOpt.IgnoreDefenseRate += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_REDUCAO_DELAY_BOSS: //OK
				lpObj->PhysiSpeed += this->Wing4thOption[Option].level[Level];
				lpObj->MagicSpeed += this->Wing4thOption[Option].level[Level];
				lpObj->DecBossHitDelay += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_EXCELLENT_DAMAGE_RATE: //OK
				lpObj->m_MPSkillOpt.ExcellentDamageRate += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_DOUBLE_DAMAGE_RATE: //OK
				lpObj->m_MPSkillOpt.DoubleDamageRate += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_ADITIONAL_STRENGTH: //OK
				lpObj->AddStrength += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_ADITIONAL_VITALITY: //OK
				lpObj->AddVitality += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_ADITIONAL_ENERGY: //OK
				lpObj->AddEnergy += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_ADITIONAL_AGILITY: //OK
				lpObj->AddDexterity += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_ADICIONAL_DANO_FINAL: //OK
				lpObj->AddGeneralDamageRate += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_ADICIONAL_DANO_BOSS: //OK
				lpObj->AddBossDamage += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_RESISTENCIA_BOSS: //OK
				lpObj->BossSkillsResistance += this->Wing4thOption[Option].level[Level];
				break;
			case WING4TH_OPTION_BOSS_DROP_INCREASE: //OK
				lpObj->BossDropIncrease += this->Wing4thOption[Option].level[Level];
				break;
			}
		}
	}

	if (flag == 1)
	{
		return;
	}

	if (lpObj->Inventory[236].IsPentagramItem() == 0)
	{
		return;
	}

	if (lpItem->m_SocketOptionBonus >= 0 && lpItem->m_SocketOptionBonus <= 15)
	{
		lpObj->ReduceGeneralElementalDamageRate += this->Wing4thElementalBasic[lpItem->m_SocketOptionBonus];
	}

	if (lpItem->m_SocketOption[4] != 254 && lpItem->m_SocketOption[4] != 255)
	{
		Option = (lpItem->m_SocketOption[4] / 16);
		Level = (lpItem->m_SocketOption[4] % 16);

		if (Level >= 0 && Level <= 15)
		{		
			switch (Option)
			{
			case WING4TH_ELEMENTAL_DANO_ELEMENTAL_PVP: //OK
				lpObj->AddElementalDamageRate += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_CHANCE_PUNISH: //OK
				lpObj->AddPunishRate += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_RESISTENCIA_RADIANCE: //OK
				lpObj->AddRadianceResistance += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DANO_ELEMENTAL_PVP_ELITE: //OK
				lpObj->AddElementalDamageRate += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DEFESA_ELEMENTAL_PVP: //OK
				lpObj->ReduceGeneralElementalDamageRate += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DANO_ELEMENTAL_BOSS: //OK
				lpObj->AddBossElementalDamage += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DEFESA_ELEMENTAL_BOSS: //OK
				lpObj->DecBossElementalDamage += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DANO_CONTRA_FOGO: //OK
				lpObj->PentagramJewelOption.MulElementalDamageVsFire += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DANO_CONTRA_AGUA: //OK
				lpObj->PentagramJewelOption.MulElementalDamageVsWater += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DANO_CONTRA_TERRA: //OK
				lpObj->PentagramJewelOption.MulElementalDamageVsEarth += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DANO_CONTRA_VENTO: //OK
				lpObj->PentagramJewelOption.MulElementalDamageVsWind += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DANO_CONTRA_ESCURIDAO: //OK
				lpObj->PentagramJewelOption.MulElementalDamageVsDark += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DANO_ELEMENTAL_BOSS_ELITE: //OK
				lpObj->AddBossElementalDamage += this->Wing4thElemental[Option].level[Level];
				break;
			case WING4TH_ELEMENTAL_DEFESA_ELEMENTAL_BOSS_ELITE: //OK
				lpObj->DecBossElementalDamage += this->Wing4thElemental[Option].level[Level];
				break;
			}
		}
	}
}