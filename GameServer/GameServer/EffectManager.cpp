#include "stdafx.h"
#include "EffectManager.h"
#include "Attack.h"
#include "Fruit.h"
#include "IllusionTemple.h"
#include "ItemManager.h"
#include "Map.h"
#include "MemScript.h"
#include "ObjectManager.h"
#include "Party.h"
#include "Util.h"
#include "Viewport.h"
#include "SkillManager.h"
#include "MasterSkillTree.h"

CEffectManager gEffectManager;

CEffectManager::CEffectManager() // OK
{
	this->Init();
}

CEffectManager::~CEffectManager() // OK
{

}

void CEffectManager::Init() // OK
{
	for (int n = 0; n < MAX_EFFECT; n++)
	{
		this->m_EffectInfo[n].Index = -1;
	}
}

void CEffectManager::Load(char* path) // OK
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

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			EFFECT_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.Group = lpMemScript->GetAsNumber();

			info.ItemIndex = lpMemScript->GetAsNumber();

			strcpy_s(info.Name, lpMemScript->GetAsString());

			info.Save = lpMemScript->GetAsNumber();

			info.Type = lpMemScript->GetAsNumber();

			info.Flag = lpMemScript->GetAsNumber();

			info.Count = lpMemScript->GetAsNumber();

			info.Value[0] = lpMemScript->GetAsNumber();
			info.Value[1] = lpMemScript->GetAsNumber();
			info.Value[2] = lpMemScript->GetAsNumber();
			info.Value[3] = lpMemScript->GetAsNumber();

			this->SetInfo(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CEffectManager::SetInfo(EFFECT_INFO info) // OK
{
	if (EFFECT_RANGE(info.Index) == 0)
	{
		return;
	}

	this->m_EffectInfo[info.Index] = info;
}

EFFECT_INFO* CEffectManager::GetInfo(int index) // OK
{
	if (EFFECT_RANGE(index) == 0)
	{
		return 0;
	}

	if (this->m_EffectInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_EffectInfo[index];
}

EFFECT_INFO* CEffectManager::GetInfoByItem(int ItemIndex) // OK
{
	for (int n = 0; n < MAX_EFFECT; n++)
	{
		EFFECT_INFO* lpInfo = this->GetInfo(n);

		if (lpInfo == 0)
		{
			continue;
		}

		if (lpInfo->ItemIndex == ItemIndex)
		{
			return lpInfo;
		}
	}

	return 0;
}

void CEffectManager::MainProc() // OK
{
	for (int n = 0; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnected(n) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[n];

		for (int i = 0; i < MAX_EFFECT_LIST; i++)
		{
			CEffect* lpEffect = &lpObj->Effect[i];

			if (lpEffect->IsEffect() == 0)
			{
				continue;
			}

			EFFECT_INFO* lpInfo = this->GetInfo(lpEffect->m_index);

			if (lpInfo == 0)
			{
				this->RemoveEffect(lpObj, lpEffect);
				this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 2, 0, lpEffect->m_index);
				lpEffect->Clear();
				continue;
			}

			if (((lpEffect->m_count == 0) ? 0 : (lpEffect->m_count--)) > 0)
			{
				this->PeriodicEffect(lpObj, lpEffect);

				if (lpEffect->m_count == 0)
				{
					this->RemoveEffect(lpObj, lpEffect);
					this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 2, 0, lpEffect->m_index);
					lpEffect->Clear();
				}
			}
		}
	}
}

bool CEffectManager::AddEffect(LPOBJ lpObj, bool type, int index, int count, WORD value1, WORD value2, WORD value3, WORD value4) // OK
{
	EFFECT_INFO* lpInfo = this->GetInfo(index);

	/*if (lpObj->Class == 275 || lpObj->Class == 561 || lpObj->Class == 459 || lpObj->Class == 295 || lpObj->Class == 716 || lpObj->Class == 717 || lpObj->Class == 718 || lpObj->Class == 673 || lpObj->Class == 734)
	{
		if (index != EFFECT_POISON && index != EFFECT_POISON_ARROW && index != EFFECT_POISON_ARROW_IMPROVED)
		{
			return 0;
		}
	}*/

	/*if ((lpObj->Class == 275 || lpObj->Class == 561 || lpObj->Class == 459 || lpObj->Class == 295 || lpObj->Class == 716 || lpObj->Class == 717 || lpObj->Class == 718 || lpObj->Class == 673 || lpObj->Class == 734) &&
		(index == EFFECT_POISON || index == EFFECT_POISON_ARROW || index == EFFECT_POISON_ARROW_IMPROVED) && lpObj->Life <= (lpObj->MaxLife * 65) / 100)
	{
		return 0;
	}*/

	if (lpInfo == 0)
	{
		return 0;
	}

	if (type == 0 && lpInfo->Count != -1)
	{
		count = ((lpInfo->Type == 2) ? ((int)time(0) + lpInfo->Count) : lpInfo->Count);
	}

	if (lpInfo->Value[0] != -1)
	{
		value1 = lpInfo->Value[0];
	}

	if (lpInfo->Value[1] != -1)
	{
		value2 = lpInfo->Value[1];
	}

	if (lpInfo->Value[2] != -1)
	{
		value3 = lpInfo->Value[2];
	}

	if (lpInfo->Value[3] != -1)
	{
		value4 = lpInfo->Value[3];
	}

	CEffect* lpEffect = this->GetEffectByGroup(lpObj, lpInfo->Group);

	if (lpEffect == 0)
	{
		for (int n = 0; n < MAX_EFFECT_LIST; n++)
		{
			lpEffect = &lpObj->Effect[n];

			if (lpEffect->Set(index, lpInfo->Group, lpInfo->Save, lpInfo->Type, lpInfo->Flag, 0, count, value1, value2, value3, value4) != 0)
			{
				if (lpEffect->m_index == SKILL_OBSIDIAN)
				{
					this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, value1, 0, lpEffect->m_count, lpEffect->m_index);
				}
				else
				{
					this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 0, lpEffect->m_count, lpEffect->m_index);
				}

				this->InsertEffect(lpObj, lpEffect);
				return 1;
			}
		}
	}
	else
	{
		if (index != lpEffect->m_index || (value1 >= lpEffect->m_value[0] && value2 >= lpEffect->m_value[1] && value3 >= lpEffect->m_value[2] && value4 >= lpEffect->m_value[3]))
		{
			this->RemoveEffect(lpObj, lpEffect);
			this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 2, 0, lpEffect->m_index);
			lpEffect->Clear();

			if (lpEffect->Set(index, lpInfo->Group, lpInfo->Save, lpInfo->Type, lpInfo->Flag, 0, count, value1, value2, value3, value4) != 0)
			{
				this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 0, lpEffect->m_count, lpEffect->m_index);
				this->InsertEffect(lpObj, lpEffect);
				return 1;
			}
		}
	}

	return 0;
}

bool CEffectManager::DelEffect(LPOBJ lpObj, int index) // OK
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];

		if (lpEffect->IsEffect() == 0)
		{
			continue;
		}

		if (lpEffect->m_index == index)
		{
			this->RemoveEffect(lpObj, lpEffect);
			this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 2, 0, lpEffect->m_index);
			lpEffect->Clear();
			return 1;
		}
	}

	return 0;
}

bool CEffectManager::DelEffectByGroup(LPOBJ lpObj, int group) // OK
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];

		if (lpEffect->IsEffect() == 0)
		{
			continue;
		}

		if (lpEffect->m_group == group)
		{
			this->RemoveEffect(lpObj, lpEffect);
			this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 2, 0, lpEffect->m_index);
			lpEffect->Clear();
			return 1;
		}
	}

	return 0;
}

CEffect* CEffectManager::GetEffect(LPOBJ lpObj, int index) // OK
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];

		if (lpEffect->IsEffect() == 0)
		{
			continue;
		}

		if (lpEffect->m_index == index)
		{
			return lpEffect;
		}
	}

	return 0;
}

CEffect* CEffectManager::GetEffectByGroup(LPOBJ lpObj, int group) // OK
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];

		if (lpEffect->IsEffect() == 0)
		{
			continue;
		}

		if (lpEffect->m_group == group)
		{
			return lpEffect;
		}
	}

	return 0;
}

bool CEffectManager::CheckEffect(LPOBJ lpObj, int index) // OK
{
	if (this->GetEffect(lpObj, index) != 0)
	{
		return 1;
	}

	return 0;
}

bool CEffectManager::CheckEffectByGroup(LPOBJ lpObj, int group) // OK
{
	if (this->GetEffectByGroup(lpObj, group) != 0)
	{
		return 1;
	}

	return 0;
}

void CEffectManager::InsertEffect(LPOBJ lpObj, CEffect* lpEffect) // OK
{
	switch (lpEffect->m_index)
	{
		case EFFECT_GREATER_DAMAGE:
			if (lpObj->Map == MAP_CASTLE_SIEGE)
			{
				lpObj->EffectOption.IncreasePvPDamage += lpEffect->m_value[0];
			}
			/*else if ((lpObj->Map != MAP_ARENA && lpObj->Class == CLASS_FE))
			{
				lpObj->EffectOption.IncreasePvPDamage += 10;
				lpObj->EffectOption.IncreaseBossDamage += 10;
			}*/
			break;
		case EFFECT_GREATER_DEFENSE:
			if (lpObj->Map == MAP_CASTLE_SIEGE)
			{
				lpObj->EffectOption.DecreasePvPDamage += lpEffect->m_value[0];
			}
			/*else if ((lpObj->Map != MAP_ARENA && lpObj->Class == CLASS_FE))
			{
				lpObj->EffectOption.DecreasePvPDamage += 10;
				lpObj->EffectOption.DecreaseBossDamage += 10;
			}*/
			break;
	case EFFECT_ELF_BUFFER:
		if (lpObj->Map != MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
			lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
			lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[0];
			lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		}		
		break;
	case EFFECT_MANA_SHIELD:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.AddDamageReduction += lpEffect->m_value[0];
		}
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.AddCriticalDamage += lpEffect->m_value[0];
		}
		break;
	case EFFECT_INFINITY_ARROW:
		break;
	case EFFECT_BP_RECOVERY:
		break;
	case EFFECT_GREATER_LIFE:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.MulMaxHP += 10; // lpEffect->m_value[0];
		}
		break;
	case EFFECT_GREATER_MANA:
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_BLESS_POTION:
		break;
	case EFFECT_SOUL_POTION:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddBPRecovery += lpEffect->m_value[1];
		GCItemUseSpecialTimeSend(lpObj->Index, 2, 60);
		break;
	case EFFECT_DISABLE_MAGIC:
		break;
	case EFFECT_CASTLE_GATE_STATE:
		break;
	case EFFECT_GUILD_STATE1:
		break;
	case EFFECT_GUILD_STATE2:
		break;
	case EFFECT_GUILD_STATE3:
		break;
	case EFFECT_GUILD_STATE4:
		break;
	case EFFECT_INVISIBILITY:
		break;
	case EFFECT_GUILD_STATE5:
		break;
	case EFFECT_CASTLE_CROWN_STATE:
		break;
	case EFFECT_CRYWOLF_STATE1:
		break;
	case EFFECT_CRYWOLF_STATE2:
		break;
	case EFFECT_CRYWOLF_STATE3:
		break;
	case EFFECT_CRYWOLF_STATE4:
		break;
	case EFFECT_CRYWOLF_STATE5:
		break;
	case EFFECT_CRYWOLF_STATE6:
		break;
	case EFFECT_CRYWOLF_NPC_TRANSPARENCY:
		break;
	case EFFECT_GAME_MASTER:
		break;
	case EFFECT_SEAL_OF_ASCENSION1:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_WEALTH1:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_SUSTENANCE1:
		break;
	case EFFECT_ORDER_OF_SPEED:
		break;
	case EFFECT_ORDER_OF_SUBLIMATION:
		break;
	case EFFECT_ORDER_OF_PROTECTION:
		lpObj->EffectOption.AddDamageReduction += lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN1:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossHitDelay += 10;
		break;
	case EFFECT_HALLOWEEN2:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossDamage += 10;
		break;
	case EFFECT_HALLOWEEN3:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossDamage += 10;
		break;
	case EFFECT_HALLOWEEN4:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN5:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossSkillResistance += 20;
		break;
	case EFFECT_SEAL_OF_ASCENSION2:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_WEALTH2:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_SUSTENANCE2:
		break;
	case EFFECT_SEAL_OF_MOVEMENT:
		break;
	case EFFECT_SCROLL_OF_QUICK:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossHitDelay += lpEffect->m_value[1];
		break;
	case EFFECT_SCROLL_OF_DEFENSE:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossDamage += lpEffect->m_value[1];
		lpObj->EffectOption.DecreasePvPDamage += lpEffect->m_value[2];
		break;
	case EFFECT_SCROLL_OF_WRATH:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossDamage += lpEffect->m_value[1];
		lpObj->EffectOption.IncreasePvPDamage += lpEffect->m_value[2];
		break;
	case EFFECT_SCROLL_OF_MAGIC_DAMAGE:
		//lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossSkillResistance += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_LIFE:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_MANA:
		//lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		lpObj->EffectOption.BossDoubleDamage += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_STRENGTH:
		lpObj->EffectOption.AddStrength += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_DEXTERITY:
		lpObj->EffectOption.AddDexterity += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_VITALITY:
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_ENERGY:
		lpObj->EffectOption.AddEnergy += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_LEADERSHIP:
		lpObj->EffectOption.AddLeadership += lpEffect->m_value[0];
		break;
	case EFFECT_POISON:
		break;
	case EFFECT_ICE:
		lpObj->DelayLevel = 1;
		lpObj->DelayActionTime = 800;
		break;
	case EFFECT_ICE_ARROW:
		break;
	case EFFECT_FIRE_SLASH:
		//lpObj->EffectOption.DivDefense += lpEffect->m_value[0];
		break;
	case EFFECT_PHYSI_DAMAGE_IMMUNITY:
		break;
	case EFFECT_MAGIC_DAMAGE_IMMUNITY:
		break;
	case EFFECT_STERN:
		break;
	case EFFECT_MAGIC_DEFENSE:
		break;
	case EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY:
		break;
	case EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY:
		break;
	case EFFECT_ORDER_OF_RESTRAINT:
		break;
	case EFFECT_CRYWOLF_STATE8:
		break;
	case EFFECT_CRYWOLF_STATE9:
		break;
	case EFFECT_CRYWOLF_STATE10:
		break;
	case EFFECT_CRYWOLF_STATE11:
		break;
	case EFFECT_CRYWOLF_STATE12:
		break;
	case EFFECT_DAMAGE_REFLECT:
		lpObj->EffectOption.AddDamageReflect += lpEffect->m_value[0];
		break;
	case EFFECT_SLEEP:
		break;
	case EFFECT_BLIND:
		lpObj->EffectOption.DivAttackSuccessRate += lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivPhysiDamage += lpEffect->m_value[0];
		break;
	case EFFECT_NEIL:
		break;
	case EFFECT_SAHAMUTT:
		break;
	/*case EFFECT_LESSER_DAMAGE:
		lpObj->EffectOption.DivPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivCurseDamage += lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DEFENSE:
		lpObj->EffectOption.DivDefense += lpEffect->m_value[0];
		break;*/
	case EFFECT_CHERRY_BLOSSOM1:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM2:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM3:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		break;
	case EFFECT_SWORD_POWER:
		lpObj->EffectOption.AddSwordPowerDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate += lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[3];
		break;
	case EFFECT_MAGIC_CIRCLE:
		lpObj->EffectOption.AddMinMagicDamage += lpEffect->m_value[0];
		break;
	case EFFECT_SWORD_SLASH:
		break;
	case EFFECT_LIGHTNING_STORM:
		break;
	case EFFECT_RED_STORM:
		break;
	case EFFECT_FROZEN_STAB:
		lpObj->DelayLevel = 1;
		lpObj->DelayActionTime = 1000;
		break;
	case EFFECT_SEAL_OF_LIFE:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddHPRecoveryRate += lpEffect->m_value[1];
		//lpObj->AutoPotionBot = true;
		break;
	case EFFECT_SEAL_OF_MANA:
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMPRecoveryRate += lpEffect->m_value[1];
		break;
	case EFFECT_SCROLL_OF_BATTLE:
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseElementalCriticalRate += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_STRENGTH:
		//lpObj->EffectOption.AddExcellentDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseElementalTripleDamageRate += lpEffect->m_value[0];
		break;

	case EFFECT_CHUCK_NORRIS:
		if (lpObj->Map != MAP_ARENA)
		{
			lpObj->EffectOption.IncreasePvPDamage += lpEffect->m_value[0];
			lpObj->EffectOption.IncreaseBossDamage += lpEffect->m_value[0];
			lpObj->EffectOption.DecreasePvPDamage += lpEffect->m_value[1];
			lpObj->EffectOption.DecreaseBossDamage += lpEffect->m_value[1];
		}
		break;
	case EFFECT_CHRISTMAS2:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS3:
		lpObj->EffectOption.DecreaseBossDamage += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS4:
		lpObj->EffectOption.IncreaseBossDamage += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS5:
		lpObj->EffectOption.IncreaseBossSkillResistance += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS6:
		lpObj->EffectOption.DecreaseBossHitDelay += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS7:
		lpObj->EffectOption.BossDropIncrease += lpEffect->m_value[0];
		break;

	case EFFECT_DUEL_ARENA_WATCH:
		break;
	case EFFECT_TALISMAN_OF_GUARDIAN:
		break;
	case EFFECT_TALISMAN_OF_PROTECTION:
		break;
	case EFFECT_MASTER_SEAL_OF_ASCENSION:
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SEAL_OF_WEALTH:
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[1];
		break;
	case EFFECT_GLADIATORS_GLORY:
		lpObj->EffectOption.AddVolcanoExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_STRENGTH:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[0];
		break;
	case EFFECT_PK_PENALTY:
		break;
	case EFFECT_MUBLUE_TIRED:
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[1];
		break;
	case EFFECT_MUBLUE_EXHAUST:
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[1];
		break;
	case EFFECT_PARTY_EXPERIENCE_BONUS:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddPartyBonusExperienceRate += lpEffect->m_value[1];
		break;
	case EFFECT_MAX_AG_BOOST_AURA:
		lpObj->EffectOption.AddMaxBP += (lpEffect->m_value[0] = (lpEffect->m_value[0] * lpObj->Level));
		break;
	case EFFECT_MAX_SD_BOOST_AURA:
		lpObj->EffectOption.AddMaxSD += (lpEffect->m_value[0] = (lpEffect->m_value[0] * lpObj->Level));
		break;
	case EFFECT_MUBLUE_MINIMUMVITALITY:
		break;
	case EFFECT_MUBLUE_LOWVITALITY:
		break;
	case EFFECT_MUBLUE_MEDIUMVITALITY:
		break;
	case EFFECT_MUBLUE_HIGHVITALITY:
		break;
	case EFFECT_PCS_MARK7:
		break;
	case EFFECT_HACKTOOL_PENALTY:
		break;
	case EFFECT_SCROLL_OF_HEALING:
		lpObj->EffectOption.AddHPRecoveryRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[1];
		break;
	case EFFECT_HAWK_FIGURINE:
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_GOAT_FIGURINE:
		lpObj->EffectOption.AddExcellentDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_OAK_CHARM:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_MAPLE_CHARM:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_GOLDEN_OAK_CHARM:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxSD += lpEffect->m_value[1];
		break;
	case EFFECT_GOLDEN_MAPLE_CHARM:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxBP += lpEffect->m_value[1];
		break;
	case EFFECT_WORN_HORSESHOE:
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_IGNORE_DEFENSE_RATE:
		lpObj->EffectOption.AddIgnoreDefenseRate += lpEffect->m_value[0];
		break;
	case EFFECT_FITNESS:
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE:
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_DECREASE_DEFENSE_RATE:
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_IRON_DEFENSE:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_LIFE_ENHANCED:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.MulMaxHP += lpEffect->m_value[0];
			lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		}
		break;
	case EFFECT_GREATER_LIFE_MASTERED:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.MulMaxHP += lpEffect->m_value[0];
			lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
			lpObj->EffectOption.MulMaxBP += lpEffect->m_value[2];
		}
		break;
	case EFFECT_BLEEDING:
		break;
	case EFFECT_MAGIC_CIRCLE_IMPROVED:
		lpObj->EffectOption.AddMinMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxMagicDamage += lpEffect->m_value[1];
		break;
	case EFFECT_MAGIC_CIRCLE_ENHANCED:
		lpObj->EffectOption.AddMinMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxMagicDamage += lpEffect->m_value[1];
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[2];
		break;
	case EFFECT_MANA_SHIELD_MASTERED:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.AddDamageReduction += lpEffect->m_value[0];
			lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		}
		break;
	case EFFECT_FROZEN_STAB_MASTERED:
		lpObj->DelayLevel = 1;
		lpObj->DelayActionTime = 1000;
		break;
	case EFFECT_BLESS:
		lpObj->EffectOption.AddStrength += lpEffect->m_value[0];
		lpObj->EffectOption.AddDexterity += lpEffect->m_value[0];
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		lpObj->EffectOption.AddEnergy += lpEffect->m_value[0];
		break;
	case EFFECT_INFINITY_ARROW_IMPROVED:
		lpObj->EffectOption.MulPhysiDamage += lpEffect->m_value[0];
		break;
	case EFFECT_BLIND_IMPROVED:
		lpObj->EffectOption.DivAttackSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_DRAIN_LIFE_ENHANCED:
		break;
	case EFFECT_ICE_STORM_ENHANCED:
		break;
	case EFFECT_EARTH_PRISON:
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE_MASTERED:
		lpObj->EffectOption.AddCriticalDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE_EXTENDED:
		lpObj->EffectOption.AddCriticalDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[1];
		lpObj->EffectOption.AddExcellentDamageRate += lpEffect->m_value[2];
		break;
	case EFFECT_SWORD_POWER_IMPROVED:
		lpObj->EffectOption.AddSwordPowerDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate += lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[3];
		break;
	case EFFECT_SWORD_POWER_ENHANCED:
		lpObj->EffectOption.AddSwordPowerDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate += lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[3];
		break;
	case EFFECT_SWORD_POWER_MASTERED:
		lpObj->EffectOption.AddSwordPowerDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate += lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[3];
		break;
	/*case EFFECT_GREATER_DEFENSE_SUCCESS_RATE_IMPROVED:
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE_ENHANCED:
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		break;*/
	case EFFECT_FITNESS_IMPROVED:
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		break;
	case EFFECT_MONK_IGNORE_ENEMY_DEFENSE_STR:
	case EFFECT_MONK_IGNORE_ENEMY_DEFENSE_ENHANCED:
		lpObj->EffectOption.AddIgnoreDefenseRate += lpEffect->m_value[0];
		break;
	/*case EFFECT_PENETRATE_ARMOR:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		break;*/
	case EFFECT_DRAGON_ROAR_ENHANCED:
		break;
	case EFFECT_FREEZING_DAMAGE:
		break;
	case EFFECT_POISON_ARROW:
		break;
	case EFFECT_POISON_ARROW_IMPROVED:
		break;
	case EFFECT_BLESS_IMPROVED:
		lpObj->EffectOption.AddStrength += lpEffect->m_value[0];
		lpObj->EffectOption.AddDexterity += lpEffect->m_value[0];
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		lpObj->EffectOption.AddEnergy += lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DAMAGE_IMPROVED:
		lpObj->EffectOption.DivPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivCurseDamage += lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DEFENSE_IMPROVED:
		lpObj->EffectOption.DivDefense += lpEffect->m_value[0];
		break;
	case EFFECT_FIRE_SLASH_ENHANCED:
		//lpObj->EffectOption.DivDefense += lpEffect->m_value[0];
		break;
	case EFFECT_IRON_DEFENSE_IMPROVED:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		break;
	case EFFECT_BLOOD_HOWLING:
		break;
	case EFFECT_BLOOD_HOWLING_IMPROVED:
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_SD:
		lpObj->EffectOption.DivMaxSD += lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_MP:
		lpObj->EffectOption.DivMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_SPEED:
		lpObj->EffectOption.DivPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicSpeed += lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_HP:
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_STUN:
		break;
	case EFFECT_PENTAGRAM_JEWEL_SLOW:
		break;
	case EFFECT_TALISMAN_OF_ASCENSION1:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_TALISMAN_OF_ASCENSION2:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_TALISMAN_OF_ASCENSION3:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_ASCENSION3:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SEAL_OF_ASCENSION2:
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;

	case EFFECT_BOSS_DEFENSE:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossDamage += lpEffect->m_value[1];
		lpObj->EffectOption.DecreaseBossElementalDamage += lpEffect->m_value[1];
		lpObj->EffectOption.IncreaseBossSkillResistance += lpEffect->m_value[2];
		
		break;
	case EFFECT_PVP_DEFENSE:
		lpObj->EffectOption.DecreasePvPDamage += lpEffect->m_value[0];		
		lpObj->EffectOption.DecreasePvPElementalDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[1];		
		break;

	case EFFECT_PVP_DAMAGE:
		lpObj->EffectOption.IncreasePvPDamage += lpEffect->m_value[0];
		lpObj->EffectOption.IncreasePvPElementalDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[1];
		lpObj->EffectOption.IncreaseElementalCriticalRate += lpEffect->m_value[1];
		lpObj->EffectOption.IncreaseElementalTripleDamageRate += lpEffect->m_value[2];
		break;

	case EFFECT_MASTER_SCROLL_OF_MANA:
		//lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];		
		break;

	case EFFECT_BOSS_DAMAGE:
		lpObj->EffectOption.IncreaseBossDamage += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossElementalDamage += lpEffect->m_value[0];
		lpObj->EffectOption.BossDoubleDamage += lpEffect->m_value[1];	
		lpObj->EffectOption.DecreaseBossHitDelay += lpEffect->m_value[2];
		break;

	case EFFECT_MASTER_SCROLL_OF_HEALING:
		lpObj->EffectOption.AddHPRecoveryRate += lpEffect->m_value[0];
		lpObj->AutoPotionBot = true;
		//lpObj->EffectOption.AddMaxHP += lpEffect->m_value[1];
		break;

	case EFFECT_MASTER_SCROLL_OF_BATTLE:
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseElementalCriticalRate += lpEffect->m_value[0];
		break;

	case EFFECT_MASTER_SCROLL_OF_STRENGTH:
		//lpObj->EffectOption.AddExcellentDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseElementalTripleDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_QUICK:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossHitDelay += lpEffect->m_value[1];
		break;
	case EFFECT_MASTER_CAST_INVINCIBILITY:
		break;
	case EFFECT_USE_MOUNT_UNIRIA:
	case EFFECT_USE_MOUNT_DINORANT:
	case EFFECT_USE_MOUNT_DLHORSE:
	case EFFECT_DARK_SPIRITD:
		//gSkillManager.GCSkillAddSend(lpObj->Index,gSkillManager.AddSkill(lpObj,SKILL_EARTHQUAKE,0),SKILL_EARTHQUAKE,0,0);
	case EFFECT_USE_MOUNT_FENRIR:
		break;
	case EFFECT_EVASION:
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_PARALYZE:
		lpObj->MoveSpeed += lpEffect->m_value[0];
		break;
	case EFFECT_PARALYZE_POW_UP:
		lpObj->MoveSpeed += lpEffect->m_value[0];
		break;
	case EFFECT_BLESS_OF_LIGHT_50:
	case EFFECT_BLESS_OF_LIGHT_100:
	case EFFECT_BLESSING_OF_LIGHT:
	case EFFECT_BLESS_OF_LIGHT_GREATER:
	case EFFECT_BLESS_OF_LIGHT_MIDDLE:
	case EFFECT_BLESS_OF_LIGHT_LOW:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_CIRCLE_SHIELD:
	case EFFECT_CIRCLE_SHIELD_STR:
	case EFFECT_CIRCLE_SHIELD_MAS:
		lpObj->EffectOption.CircleShieldRate += lpEffect->m_value[0];
		break;
	case EFFECT_OBSIDIAN:
	case EFFECT_OBSIDIAN_STR:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[0];
		break;
	case EFFECT_WRATH:
	case EFFECT_WRATH_STR:
	case EFFECT_WRATH_PRO:
		/*lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[1];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[1];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[1];*/
		break;
	case EFFECT_WRATH_MAS:
		/*lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[1];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[1];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[1];*/
		break;
	case EFFECT_BURST:
	case EFFECT_BURST_STR:
		lpObj->EffectOption.AddIgnoreDefenseRate += lpEffect->m_value[0];
		break;
	case EFFECT_BASTION:
		lpObj->ReduceGeneralDamageRate += 20;
		lpObj->ReduceGeneralElementalDamageRate += 20;
		break;
	case EFFECT_HEMORRHAGE:
		break;
	case EFFECT_PARALYSIS:
		lpObj->DelayLevel = 1;
		lpObj->DelayActionTime = 800;
		break;
	case EFFECT_BONDAGE:
		break;
	/*case EFFECT_BLINDNES:
		lpObj->EffectOption.DivMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivPhysiDamage += lpEffect->m_value[0];
		break;*/
	case EFFECT_ARCHANGEL_WILL:
		/*lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[0];*/
		break;
	case EFFECT_FEREA_PARTY_EXP:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate += lpEffect->m_value[0];
		break;

	case EFFECT_SWELL_LIFE_ENHANCEMENT:
		break;
	case EFFECT_MANA_SHIELD_ENHANCEMENT:
		break;
	case EFFECT_BERSERKER_ENHANCEMENT:
		break;
	case EFFECT_BERSERKER_ENHANCEMENT_PROFI:
		break;
	case EFFECT_STAMINA_ENHANCEMENT:
		break;
	case EFFECT_WRATH_ENHANCEMENT:
		break;
	case EFFECT_WRATH_ENHANCEMENT_PROFI:
		break;
	case EFFECT_DECREASE_SD_RATIO:
		break;
	case EFFECT_SD_RISE:
		break;
	case EFFECT_PLAGUE:
		break;
	case EFFECT_FREEZING:
		lpObj->EffectOption.DivDefense += lpEffect->m_value[1];
		break;
	case EFFECT_BLEEDING2:
		break;
	case EFFECT_SHOCK:
		break;
	case EFFECT_ADDICTED:
		break;
	case EFFECT_FREEZING1:
		lpObj->EffectOption.DivPhysiSpeed += lpEffect->m_value[1];
		lpObj->EffectOption.DivMagicSpeed += lpEffect->m_value[1];
		break;
	case EFFECT_EXCESSIVE_BLEEDING:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[3];
		break;
	case EFFECT_SHOCK1:
		break;
	case EFFECT_INCREASED_SD_REDUCTION:
		break;
	case EFFECT_INCREASE_MAXIMUM_HEALTH:
		break;
	case EFFECT_FOURTH_STATS:
		break;
	case EFFECT_BASE_DEFENSE_INCREASE:
		break;
	case EFFECT_INCREASE_ATTACK_POWER_OF_4TH_WINGS:
		break;
	case EFFECT_DAMAGE_INCREASE:
		break;
	case EFFECT_WIZADRY_POWER_INCREASE:
		break;
	case EFFECT_FOURTH_STATS1:
		break;
	case EFFECT_SKILL_DAMAGE_INCREASE:
		break;
	case EFFECT_FOURTH_BLADE_DAMAGE_INCREASE:
		break;
	case EFFECT_ATTACK_OR_WIZADRY_POWER_INCREASE:
		break;
	}

	gObjectManager.CharacterCalcAttribute(lpObj->Index);
}

void CEffectManager::RemoveEffect(LPOBJ lpObj, CEffect* lpEffect) // OK
{
	switch (lpEffect->m_index)
	{
	case EFFECT_GREATER_DAMAGE:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.IncreasePvPDamage -= lpEffect->m_value[0];
		}
		/*else if ((lpObj->Map != MAP_ARENA && lpObj->Class == CLASS_FE))
		{
			lpObj->EffectOption.IncreasePvPDamage -= 10;
			lpObj->EffectOption.IncreaseBossDamage -= 10;
		}*/
		break;
	case EFFECT_GREATER_DEFENSE:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.DecreasePvPDamage -= lpEffect->m_value[0];
		}
		/*else if ((lpObj->Map != MAP_ARENA && lpObj->Class == CLASS_FE))
		{
			lpObj->EffectOption.DecreasePvPDamage -= 10;
			lpObj->EffectOption.DecreaseBossDamage -= 10;
		}*/
		break;
	case EFFECT_ELF_BUFFER:
		if (lpObj->Map != MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
			lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
			lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[0];
			lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		}		
		break;
	case EFFECT_MANA_SHIELD:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.AddDamageReduction -= lpEffect->m_value[0];
		}
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.AddCriticalDamage -= lpEffect->m_value[0];
		}
		break;
	case EFFECT_INFINITY_ARROW:
		break;
	case EFFECT_BP_RECOVERY:
		break;
	case EFFECT_GREATER_LIFE:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.MulMaxHP -= 10; // lpEffect->m_value[0];
		}
		break;
	case EFFECT_GREATER_MANA:
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_BLESS_POTION:
		break;
	case EFFECT_SOUL_POTION:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddBPRecovery -= lpEffect->m_value[1];
		break;
	case EFFECT_DISABLE_MAGIC:
		break;
	case EFFECT_CASTLE_GATE_STATE:
		break;
	case EFFECT_GUILD_STATE1:
		break;
	case EFFECT_GUILD_STATE2:
		break;
	case EFFECT_GUILD_STATE3:
		break;
	case EFFECT_GUILD_STATE4:
		break;
	case EFFECT_INVISIBILITY:
		break;
	case EFFECT_GUILD_STATE5:
		break;
	case EFFECT_CASTLE_CROWN_STATE:
		break;
	case EFFECT_CRYWOLF_STATE1:
		break;
	case EFFECT_CRYWOLF_STATE2:
		break;
	case EFFECT_CRYWOLF_STATE3:
		break;
	case EFFECT_CRYWOLF_STATE4:
		break;
	case EFFECT_CRYWOLF_STATE5:
		break;
	case EFFECT_CRYWOLF_STATE6:
		break;
	case EFFECT_CRYWOLF_NPC_TRANSPARENCY:
		break;
	case EFFECT_GAME_MASTER:
		break;
	case EFFECT_SEAL_OF_ASCENSION1:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_WEALTH1:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_SUSTENANCE1:
		break;
	case EFFECT_ORDER_OF_SPEED:
		break;
	case EFFECT_ORDER_OF_SUBLIMATION:
		break;
	case EFFECT_ORDER_OF_PROTECTION:
		lpObj->EffectOption.AddDamageReduction -= lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN1:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossHitDelay -= 10;
		break;
	case EFFECT_HALLOWEEN2:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossDamage -= 10;
		break;
	case EFFECT_HALLOWEEN3:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossDamage -= 10;
		break;
	case EFFECT_HALLOWEEN4:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN5:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossSkillResistance -= 20;
		break;
	case EFFECT_SEAL_OF_ASCENSION2:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_WEALTH2:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_SUSTENANCE2:
		break;
	case EFFECT_SEAL_OF_MOVEMENT:
		break;
	case EFFECT_SCROLL_OF_QUICK:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossHitDelay -= lpEffect->m_value[1];
		break;
	case EFFECT_SCROLL_OF_DEFENSE:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.DecreasePvPDamage -= lpEffect->m_value[2];
		break;
	case EFFECT_SCROLL_OF_WRATH:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.IncreasePvPDamage -= lpEffect->m_value[2];
		break;
	case EFFECT_SCROLL_OF_MAGIC_DAMAGE:
		//lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossSkillResistance -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_LIFE:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_MANA:
		//lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		lpObj->EffectOption.BossDoubleDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_STRENGTH:
		lpObj->EffectOption.AddStrength -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_DEXTERITY:
		lpObj->EffectOption.AddDexterity -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_VITALITY:
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_ENERGY:
		lpObj->EffectOption.AddEnergy -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_LEADERSHIP:
		lpObj->EffectOption.AddLeadership -= lpEffect->m_value[0];
		break;
	case EFFECT_POISON:
		break;
	case EFFECT_ICE:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_ICE_ARROW:
		break;
	case EFFECT_FIRE_SLASH:
		//lpObj->EffectOption.DivDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_PHYSI_DAMAGE_IMMUNITY:
		break;
	case EFFECT_MAGIC_DAMAGE_IMMUNITY:
		break;
	case EFFECT_STERN:
		break;
	case EFFECT_MAGIC_DEFENSE:
		break;
	case EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY:
		break;
	case EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY:
		break;
	case EFFECT_ORDER_OF_RESTRAINT:
		break;
	case EFFECT_CRYWOLF_STATE8:
		break;
	case EFFECT_CRYWOLF_STATE9:
		break;
	case EFFECT_CRYWOLF_STATE10:
		break;
	case EFFECT_CRYWOLF_STATE11:
		break;
	case EFFECT_CRYWOLF_STATE12:
		break;
	case EFFECT_DAMAGE_REFLECT:
		lpObj->EffectOption.AddDamageReflect -= lpEffect->m_value[0];
		break;
	case EFFECT_SLEEP:
		break;
	case EFFECT_BLIND:
		lpObj->EffectOption.DivAttackSuccessRate -= lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivPhysiDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_NEIL:
		break;
	case EFFECT_SAHAMUTT:
		break;
	/*case EFFECT_LESSER_DAMAGE:
		lpObj->EffectOption.DivPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivCurseDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DEFENSE:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[0];
		break;*/
	case EFFECT_CHERRY_BLOSSOM1:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM2:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM3:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_SWORD_POWER:
		lpObj->EffectOption.AddSwordPowerDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate -= lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[3];
		break;
	case EFFECT_MAGIC_CIRCLE:
		lpObj->EffectOption.AddMinMagicDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_SWORD_SLASH:
		break;
	case EFFECT_LIGHTNING_STORM:
		break;
	case EFFECT_RED_STORM:
		break;
	case EFFECT_FROZEN_STAB:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_SEAL_OF_LIFE:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddHPRecoveryRate -= lpEffect->m_value[1];
		//lpObj->AutoPotionBot = false;
		break;
	case EFFECT_SEAL_OF_MANA:
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMPRecoveryRate -= lpEffect->m_value[1];
		break;
	case EFFECT_SCROLL_OF_BATTLE:
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseElementalCriticalRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_STRENGTH:
		//lpObj->EffectOption.AddExcellentDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseElementalTripleDamageRate -= lpEffect->m_value[0];
		break;

	case EFFECT_CHUCK_NORRIS:
		if (lpObj->Map != MAP_ARENA)
		{
			lpObj->EffectOption.IncreasePvPDamage -= lpEffect->m_value[0];
			lpObj->EffectOption.IncreaseBossDamage -= lpEffect->m_value[0];
			lpObj->EffectOption.DecreasePvPDamage -= lpEffect->m_value[1];
			lpObj->EffectOption.DecreaseBossDamage -= lpEffect->m_value[1];
		}
		break;
	case EFFECT_CHRISTMAS2:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS3:
		lpObj->EffectOption.DecreaseBossDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS4:
		lpObj->EffectOption.IncreaseBossDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS5:
		lpObj->EffectOption.IncreaseBossSkillResistance -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS6:
		lpObj->EffectOption.DecreaseBossHitDelay -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS7:
		lpObj->EffectOption.BossDropIncrease -= lpEffect->m_value[0];
		break;

	case EFFECT_DUEL_ARENA_WATCH:
		break;
	case EFFECT_TALISMAN_OF_GUARDIAN:
		break;
	case EFFECT_TALISMAN_OF_PROTECTION:
		break;
	case EFFECT_MASTER_SEAL_OF_ASCENSION:
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SEAL_OF_WEALTH:
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[1];
		break;
	case EFFECT_GLADIATORS_GLORY:
		lpObj->EffectOption.AddVolcanoExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_STRENGTH:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_PK_PENALTY:
		break;
	case EFFECT_MUBLUE_TIRED:
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[1];
		break;
	case EFFECT_MUBLUE_EXHAUST:
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[1];
		break;
	case EFFECT_PARTY_EXPERIENCE_BONUS:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddPartyBonusExperienceRate -= lpEffect->m_value[1];
		lpObj->EffectOption.AddForMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MAX_AG_BOOST_AURA:
		lpObj->EffectOption.AddMaxBP -= lpEffect->m_value[0];
		break;
	case EFFECT_MAX_SD_BOOST_AURA:
		lpObj->EffectOption.AddMaxSD -= lpEffect->m_value[0];
		break;
	case EFFECT_MUBLUE_MINIMUMVITALITY:
		break;
	case EFFECT_MUBLUE_LOWVITALITY:
		break;
	case EFFECT_MUBLUE_MEDIUMVITALITY:
		break;
	case EFFECT_MUBLUE_HIGHVITALITY:
		break;
	case EFFECT_PCS_MARK7:
		break;
	case EFFECT_HACKTOOL_PENALTY:
		break;
	case EFFECT_SCROLL_OF_HEALING:
		lpObj->EffectOption.AddHPRecoveryRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[1];
		break;
	case EFFECT_HAWK_FIGURINE:
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_GOAT_FIGURINE:
		lpObj->EffectOption.AddExcellentDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_OAK_CHARM:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_MAPLE_CHARM:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_GOLDEN_OAK_CHARM:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxSD -= lpEffect->m_value[1];
		break;
	case EFFECT_GOLDEN_MAPLE_CHARM:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxBP -= lpEffect->m_value[1];
		break;
	case EFFECT_WORN_HORSESHOE:
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_IGNORE_DEFENSE_RATE:
		lpObj->EffectOption.AddIgnoreDefenseRate -= lpEffect->m_value[0];
		break;
	case EFFECT_FITNESS:
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE:
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_DECREASE_DEFENSE_RATE:
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_IRON_DEFENSE:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_LIFE_ENHANCED:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.MulMaxHP -= lpEffect->m_value[0];
			lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		}
		break;
	case EFFECT_GREATER_LIFE_MASTERED:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.MulMaxHP -= lpEffect->m_value[0];
			lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
			lpObj->EffectOption.MulMaxBP -= lpEffect->m_value[2];
		}
		break;
	case EFFECT_BLEEDING:
		break;
	case EFFECT_MAGIC_CIRCLE_IMPROVED:
		lpObj->EffectOption.AddMinMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxMagicDamage -= lpEffect->m_value[1];
		break;
	case EFFECT_MAGIC_CIRCLE_ENHANCED:
		lpObj->EffectOption.AddMinMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxMagicDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[2];
		break;
	case EFFECT_MANA_SHIELD_MASTERED:
		if (lpObj->Map == MAP_CASTLE_SIEGE)
		{
			lpObj->EffectOption.AddDamageReduction -= lpEffect->m_value[0];
			lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		}
		break;
	case EFFECT_FROZEN_STAB_MASTERED:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_BLESS:
		lpObj->EffectOption.AddStrength -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDexterity -= lpEffect->m_value[0];
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		lpObj->EffectOption.AddEnergy -= lpEffect->m_value[0];
		gFruit.GCFruitResultSend(lpObj, 18, lpEffect->m_value[0], 7);
		break;
	case EFFECT_INFINITY_ARROW_IMPROVED:
		lpObj->EffectOption.MulPhysiDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_BLIND_IMPROVED:
		lpObj->EffectOption.DivAttackSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_DRAIN_LIFE_ENHANCED:
		break;
	case EFFECT_ICE_STORM_ENHANCED:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_EARTH_PRISON:
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE_MASTERED:
		lpObj->EffectOption.AddCriticalDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE_EXTENDED:
		lpObj->EffectOption.AddCriticalDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[1];
		lpObj->EffectOption.AddExcellentDamageRate -= lpEffect->m_value[2];
		break;
	case EFFECT_SWORD_POWER_IMPROVED:
		lpObj->EffectOption.AddSwordPowerDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate -= lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[3];
		break;
	case EFFECT_SWORD_POWER_ENHANCED:
		lpObj->EffectOption.AddSwordPowerDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate -= lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[3];
		break;
	case EFFECT_SWORD_POWER_MASTERED:
		lpObj->EffectOption.AddSwordPowerDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate -= lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[3];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE_IMPROVED:
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE_ENHANCED:
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_FITNESS_IMPROVED:
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		break;
	case EFFECT_MONK_IGNORE_ENEMY_DEFENSE_STR:
	case EFFECT_MONK_IGNORE_ENEMY_DEFENSE_ENHANCED:
		lpObj->EffectOption.AddIgnoreDefenseRate -= lpEffect->m_value[0];
		break;
	/*case EFFECT_PENETRATE_ARMOR:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		break;*/
	case EFFECT_DRAGON_ROAR_ENHANCED:
		break;
	case EFFECT_FREEZING_DAMAGE:
		break;
	case EFFECT_POISON_ARROW:
		break;
	case EFFECT_POISON_ARROW_IMPROVED:
		break;
	case EFFECT_BLESS_IMPROVED:
		lpObj->EffectOption.AddStrength -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDexterity -= lpEffect->m_value[0];
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		lpObj->EffectOption.AddEnergy -= lpEffect->m_value[0];
		gFruit.GCFruitResultSend(lpObj, 18, lpEffect->m_value[0], 7);
		break;
	case EFFECT_LESSER_DAMAGE_IMPROVED:
		lpObj->EffectOption.DivPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivCurseDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DEFENSE_IMPROVED:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_FIRE_SLASH_ENHANCED:
		//lpObj->EffectOption.DivDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_IRON_DEFENSE_IMPROVED:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_BLOOD_HOWLING:
		break;
	case EFFECT_BLOOD_HOWLING_IMPROVED:
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_SD:
		lpObj->EffectOption.DivMaxSD -= lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_MP:
		lpObj->EffectOption.DivMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_SPEED:
		lpObj->EffectOption.DivPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicSpeed -= lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_HP:
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_STUN:
		break;
	case EFFECT_PENTAGRAM_JEWEL_SLOW:
		break;
	case EFFECT_TALISMAN_OF_ASCENSION1:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_TALISMAN_OF_ASCENSION2:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_TALISMAN_OF_ASCENSION3:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_ASCENSION3:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SEAL_OF_ASCENSION2:
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		break;

	case EFFECT_BOSS_DEFENSE:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.DecreaseBossElementalDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.IncreaseBossSkillResistance -= lpEffect->m_value[2];
		break;

	case EFFECT_PVP_DEFENSE:
		lpObj->EffectOption.DecreasePvPDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DecreasePvPElementalDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[1];
		break;

	case EFFECT_PVP_DAMAGE:
		lpObj->EffectOption.IncreasePvPDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreasePvPElementalDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[1];
		lpObj->EffectOption.IncreaseElementalCriticalRate -= lpEffect->m_value[1];
		lpObj->EffectOption.IncreaseElementalTripleDamageRate -= lpEffect->m_value[2];
		break;

	case EFFECT_MASTER_SCROLL_OF_MANA:
		//lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;

	case EFFECT_BOSS_DAMAGE:
		lpObj->EffectOption.IncreaseBossDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseBossElementalDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.BossDoubleDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.DecreaseBossHitDelay -= lpEffect->m_value[2];
		break;

	case EFFECT_MASTER_SCROLL_OF_HEALING:
		lpObj->EffectOption.AddHPRecoveryRate -= lpEffect->m_value[0];
		lpObj->AutoPotionBot = false;
		//lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[1];
		break;
	case EFFECT_MASTER_SCROLL_OF_BATTLE:
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseElementalCriticalRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_STRENGTH:
		//lpObj->EffectOption.AddExcellentDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.IncreaseElementalTripleDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_QUICK:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.DecreaseBossHitDelay -= lpEffect->m_value[1];
		break;
	case EFFECT_MASTER_CAST_INVINCIBILITY:
		break;
	case EFFECT_USE_MOUNT_UNIRIA:
	case EFFECT_USE_MOUNT_DINORANT:
	case EFFECT_USE_MOUNT_DLHORSE:
	case EFFECT_DARK_SPIRITD:
		//gSkillManager.GCSkillDelSend(lpObj->Index,gSkillManager.DelSkill(lpObj,SKILL_EARTHQUAKE),SKILL_EARTHQUAKE,0,0);
		break;
	case EFFECT_USE_MOUNT_FENRIR:
		break;
	case EFFECT_EVASION:
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_PARALYZE:
		lpObj->MoveSpeed -= lpEffect->m_value[0];
		break;
	case EFFECT_PARALYZE_POW_UP:
		lpObj->MoveSpeed -= lpEffect->m_value[0];
		break;
	case EFFECT_BLESS_OF_LIGHT_50:
	case EFFECT_BLESS_OF_LIGHT_100:
	case EFFECT_BLESSING_OF_LIGHT:
	case EFFECT_BLESS_OF_LIGHT_GREATER:
	case EFFECT_BLESS_OF_LIGHT_MIDDLE:
	case EFFECT_BLESS_OF_LIGHT_LOW:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_CIRCLE_SHIELD:
	case EFFECT_CIRCLE_SHIELD_STR:
	case EFFECT_CIRCLE_SHIELD_MAS:
		lpObj->EffectOption.CircleShieldRate -= lpEffect->m_value[0];
		break;
	case EFFECT_OBSIDIAN:
	case EFFECT_OBSIDIAN_STR:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_WRATH:
	case EFFECT_WRATH_STR:
	case EFFECT_WRATH_PRO:
		/*lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[1];*/
		break;
	case EFFECT_WRATH_MAS:
		/*lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[1];*/
		break;
	case EFFECT_BURST:
	case EFFECT_BURST_STR:
		lpObj->EffectOption.AddIgnoreDefenseRate -= lpEffect->m_value[0];
		break;
	case EFFECT_BASTION:
		lpObj->ReduceGeneralDamageRate -= 20;
		lpObj->ReduceGeneralElementalDamageRate -= 20;
		break;
	case EFFECT_HEMORRHAGE:
		break;
	case EFFECT_PARALYSIS:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_BONDAGE:
		break;
	/*case EFFECT_BLINDNES:
		lpObj->EffectOption.DivMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivPhysiDamage -= lpEffect->m_value[0];
		break;*/
	case EFFECT_ARCHANGEL_WILL:
		/*lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[0];*/
		break;
	case EFFECT_FEREA_PARTY_EXP:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddForMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SWELL_LIFE_ENHANCEMENT:
		break;
	case EFFECT_MANA_SHIELD_ENHANCEMENT:
		break;
	case EFFECT_BERSERKER_ENHANCEMENT:
		break;
	case EFFECT_BERSERKER_ENHANCEMENT_PROFI:
		break;
	case EFFECT_STAMINA_ENHANCEMENT:
		break;
	case EFFECT_WRATH_ENHANCEMENT:
		break;
	case EFFECT_WRATH_ENHANCEMENT_PROFI:
		break;
	case EFFECT_DECREASE_SD_RATIO:
		break;
	case EFFECT_SD_RISE:
		break;
	case EFFECT_PLAGUE:
		break;
	case EFFECT_FREEZING:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_BLEEDING2:
		break;
	case EFFECT_SHOCK:
		break;
	case EFFECT_ADDICTED:
		break;
	case EFFECT_FREEZING1:
		lpObj->EffectOption.DivPhysiSpeed -= lpEffect->m_value[1];
		lpObj->EffectOption.DivMagicSpeed -= lpEffect->m_value[1];
		break;
	case EFFECT_EXCESSIVE_BLEEDING:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[3];
		break;
	case EFFECT_SHOCK1:
		break;
	case EFFECT_INCREASED_SD_REDUCTION:
		break;
	case EFFECT_INCREASE_MAXIMUM_HEALTH:
		break;
	case EFFECT_FOURTH_STATS:
		break;
	case EFFECT_BASE_DEFENSE_INCREASE:
		break;
	case EFFECT_INCREASE_ATTACK_POWER_OF_4TH_WINGS:
		break;
	case EFFECT_DAMAGE_INCREASE:
		break;
	case EFFECT_WIZADRY_POWER_INCREASE:
		break;
	case EFFECT_FOURTH_STATS1:
		break;
	case EFFECT_SKILL_DAMAGE_INCREASE:
		break;
	case EFFECT_FOURTH_BLADE_DAMAGE_INCREASE:
		break;
	case EFFECT_ATTACK_OR_WIZADRY_POWER_INCREASE:
		break;
	}

	if (lpEffect->m_index == EFFECT_ORDER_OF_PROTECTION || lpEffect->m_index == EFFECT_ORDER_OF_RESTRAINT)
	{
		if (IT_MAP_RANGE(lpObj->Map) != 0)
		{
			gIllusionTemple.GCIllusionTempleDelSkillSend(lpObj->Index, lpEffect->m_value[0]);
		}
	}

	gObjectManager.CharacterCalcAttribute(lpObj->Index);
}

bool CEffectManager::ConvertEffectByte(CEffect* lpEffect, BYTE* lpMsg) // OK
{
	if (MAKE_NUMBERW(lpMsg[0], lpMsg[1]) == -1 || MAKE_NUMBERW(lpMsg[0], lpMsg[1]) == 65535)
	{
		return 0;
	}

	lpEffect->m_index = MAKE_NUMBERW(lpMsg[0], lpMsg[1]);

	lpEffect->m_count = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg[2], lpMsg[3]), MAKE_NUMBERW(lpMsg[4], lpMsg[5]));

	lpEffect->m_value[0] = MAKE_NUMBERW(lpMsg[6], lpMsg[7]);
	lpEffect->m_value[1] = MAKE_NUMBERW(lpMsg[8], lpMsg[9]);
	lpEffect->m_value[2] = MAKE_NUMBERW(lpMsg[10], lpMsg[11]);
	lpEffect->m_value[3] = MAKE_NUMBERW(lpMsg[12], lpMsg[13]);

	return 1;
}

void CEffectManager::EffectByteConvert(BYTE* lpMsg, CEffect* lpEffect) // OK
{
	if (lpEffect->IsEffect() == 0 || lpEffect->m_save == 0)
	{
		memset(lpMsg, 0xFF, 14);
		return;
	}

	//WORD m_index;
	lpMsg[0] = SET_NUMBERHB(lpEffect->m_index);
	lpMsg[1] = SET_NUMBERLB(lpEffect->m_index);

	if (lpEffect->m_type == 2)
	{
		//DWORD m_time;
		lpMsg[2] = SET_NUMBERHB(SET_NUMBERHW(lpEffect->m_time));
		lpMsg[3] = SET_NUMBERLB(SET_NUMBERHW(lpEffect->m_time));
		lpMsg[4] = SET_NUMBERHB(SET_NUMBERLW(lpEffect->m_time));
		lpMsg[5] = SET_NUMBERLB(SET_NUMBERLW(lpEffect->m_time));
	}
	else
	{
		//DWORD m_count;
		lpMsg[2] = SET_NUMBERHB(SET_NUMBERHW(lpEffect->m_count));
		lpMsg[3] = SET_NUMBERLB(SET_NUMBERHW(lpEffect->m_count));
		lpMsg[4] = SET_NUMBERHB(SET_NUMBERLW(lpEffect->m_count));
		lpMsg[5] = SET_NUMBERLB(SET_NUMBERLW(lpEffect->m_count));
	}

	//DWORD m_value[4];
	lpMsg[6] = SET_NUMBERHB(lpEffect->m_value[0]);
	lpMsg[7] = SET_NUMBERLB(lpEffect->m_value[0]);

	lpMsg[8] = SET_NUMBERHB(lpEffect->m_value[1]);
	lpMsg[9] = SET_NUMBERLB(lpEffect->m_value[1]);

	lpMsg[10] = SET_NUMBERHB(lpEffect->m_value[2]);
	lpMsg[11] = SET_NUMBERLB(lpEffect->m_value[2]);

	lpMsg[12] = SET_NUMBERHB(lpEffect->m_value[3]);
	lpMsg[13] = SET_NUMBERLB(lpEffect->m_value[3]);
}

int CEffectManager::GenerateEffectList(LPOBJ lpObj, BYTE* lpMsg, int* size) // OK
{
	int count = 0;

	PMSG_VIEWPORT_STATE info;

	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		if (lpObj->Effect[n].IsEffect() != 0)
		{
			info.effect = lpObj->Effect[n].m_index;

			memcpy(&lpMsg[(*size)], &info, sizeof(info));
			(*size) += sizeof(info);

			count++;
		}
	}

	return count;
}

int CEffectManager::GenerateEffectList(LPOBJ lpObj, BYTE* lpMsg) // OK
{
	int count = 0;

	PMSG_VIEWPORT_STATE info;

	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		if (lpObj->Effect[n].IsEffect() != 0)
		{
			info.effect = lpObj->Effect[n].m_index;
			memcpy(&lpMsg[count], &info, sizeof(info));
			count++;
		}
	}

	return count;
}

int CEffectManager::GenerateEffectList(LPOBJ lpObj, WORD* lpMsg, bool antilag) // OK
{
	int count = 0;

	PMSG_VIEWPORT_STATE info;

	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		if (lpObj->Effect[n].IsEffect() != 0)
		{
			if (antilag)
			{
				if ((lpObj->Effect[n].m_index >= 1 && lpObj->Effect[n].m_index <= 12) ||
					(lpObj->Effect[n].m_index >= 29 && lpObj->Effect[n].m_index <= 55) ||
					(lpObj->Effect[n].m_index >= 58 && lpObj->Effect[n].m_index <= 60) ||
					(lpObj->Effect[n].m_index >= 71 && lpObj->Effect[n].m_index <= 204) ||
					(lpObj->Effect[n].m_index >= 209)
					)
					continue;
			}

			//LogAdd(LOG_DEBUG,"Effect %d de %s", lpObj->Effect[n].m_index, lpObj->Name);

			info.effect = lpObj->Effect[n].m_index;
			memcpy(&lpMsg[count], &info, sizeof(info));
			count++;
		}
	}

	return count;
}

int CEffectManager::GeneratePartyEffectList(LPOBJ lpObj, BYTE* lpMsg, int* size) // OK
{
	int count = 0;

	PMSG_PARTY_EFFECT_LIST info;

	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		if (lpObj->Effect[n].IsEffect() != 0)
		{
			info.effect = lpObj->Effect[n].m_index;

			info.count = lpObj->Effect[n].m_count;

			memcpy(&lpMsg[(*size)], &info, sizeof(info));
			(*size) += sizeof(info);

			count++;
		}
	}

	return count;
}

bool CEffectManager::CheckStunEffect(LPOBJ lpObj) // OK
{
	if (this->CheckEffect(lpObj, EFFECT_STERN) != 0 || this->CheckEffect(lpObj, EFFECT_SLEEP) != 0 || this->CheckEffect(lpObj, EFFECT_EARTH_PRISON) != 0 || this->CheckEffect(lpObj, EFFECT_PENTAGRAM_JEWEL_STUN) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CEffectManager::CheckImmobilizeEffect(LPOBJ lpObj) // OK
{
	if (this->CheckEffect(lpObj, EFFECT_ICE_ARROW) != 0 || this->CheckEffect(lpObj, EFFECT_ORDER_OF_RESTRAINT) != 0 || this->CheckEffect(lpObj, EFFECT_ICE_STORM_ENHANCED) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void CEffectManager::ClearAllEffect(LPOBJ lpObj) // OK
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];

		if (lpEffect->IsEffect() == 0 || lpEffect->m_type != 0)
		{
			continue;
		}

		//LogAdd(LOG_DEBUG, "limpando %d de %s", lpEffect->m_index, lpObj->Name);

		this->RemoveEffect(lpObj, lpEffect);
		this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 2, 0, lpEffect->m_index);
		lpEffect->Clear();
	}
}

void CEffectManager::ClearDebuffEffect(LPOBJ lpObj, int count) // OK
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];

		if (lpEffect->IsEffect() == 0 || lpEffect->m_type != 0 || lpEffect->m_flag == 0 || 
			lpEffect->m_index == EFFECT_STERN || lpEffect->m_index == EFFECT_BLINDNES || lpEffect->m_index == EFFECT_BLEEDING)
		{
			continue;
		}

		if ((--count) >= 0)
		{
			this->RemoveEffect(lpObj, lpEffect);
			this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 2, 0, lpEffect->m_index);
			lpEffect->Clear();
		}
	}
}

void CEffectManager::PeriodicEffect(LPOBJ lpObj, CEffect* lpEffect) // OK
{
	if (
		lpEffect->m_index != EFFECT_PLAGUE &&
		lpEffect->m_index != EFFECT_FREEZING &&
		lpEffect->m_index != EFFECT_BLEEDING &&
		lpEffect->m_index != EFFECT_SHOCK &&
		lpEffect->m_index != EFFECT_ADDICTED &&
		lpEffect->m_index != EFFECT_FREEZING1 &&
		lpEffect->m_index != EFFECT_EXCESSIVE_BLEEDING &&
		lpEffect->m_index != EFFECT_SHOCK1 &&
		lpEffect->m_index != EFFECT_HEMORRHAGE &&
		lpEffect->m_index != EFFECT_POISON && 
		lpEffect->m_index != EFFECT_NEIL && 
		lpEffect->m_index != EFFECT_SAHAMUTT && 
		lpEffect->m_index != EFFECT_BLEEDING2 && 
		lpEffect->m_index != EFFECT_DRAIN_LIFE_ENHANCED && 
		lpEffect->m_index != EFFECT_DRAGON_ROAR_ENHANCED && 
		lpEffect->m_index != EFFECT_FREEZING_DAMAGE && 
		lpEffect->m_index != EFFECT_POISON_ARROW && 
		lpEffect->m_index != EFFECT_POISON_ARROW_IMPROVED)
	{
		return;
	}

	if (lpEffect->m_value[1] == 0 || (lpEffect->m_count % lpEffect->m_value[1]) != 0)
	{
		return;
	}

	if (lpObj->Type == OBJECT_NPC || lpObj->Live == 0 || OBJECT_RANGE(lpEffect->m_value[0]) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[lpEffect->m_value[0]];

	int damage = 0;
	int shield = 0;
	int absorb = 0;
	int effect = 0;

	switch (lpEffect->m_index)
	{
	case EFFECT_POISON:
		damage = (((damage = (int)((lpObj->Life * lpEffect->m_value[2]) / 100)) > lpEffect->m_value[3]) ? ((lpEffect->m_value[3] == 0) ? damage : lpEffect->m_value[3]) : damage);
		effect = 2;
		break;
	case EFFECT_NEIL:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		effect = 3;
		break;
	case EFFECT_SAHAMUTT:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		effect = 3;
		break;
	case EFFECT_BLEEDING:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_DRAIN_LIFE_ENHANCED:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		effect = 3;
		break;
	case EFFECT_DRAGON_ROAR_ENHANCED:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_FREEZING_DAMAGE:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		gObjAddAttackProcMsgSendDelay(lpTarget, 100, lpObj->Index, 60, damage, 1);
		gObjAddAttackProcMsgSendDelay(lpTarget, 100, lpObj->Index, 120, damage, 2);
		gObjAddAttackProcMsgSendDelay(lpTarget, 100, lpObj->Index, 180, damage, 3);
		gObjAddAttackProcMsgSendDelay(lpTarget, 100, lpObj->Index, 240, damage, 4);
		if (GetLargeRand() % 5 == 0)
		{
			gEffectManager.AddEffect(lpObj, 0, EFFECT_ICE_ARROW, 3, 0, 0, 0, 0);
		}
		return;
		break;
	case EFFECT_POISON_ARROW:
		damage = lpEffect->m_value[2];
		absorb = 0;
		effect = 2;
		break;
	case EFFECT_POISON_ARROW_IMPROVED:
		damage = lpEffect->m_value[2] + gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_POISON_ARROW_IMPROVED);
		absorb = 0;
		effect = 2;
		break;
	case EFFECT_HEMORRHAGE:
		damage =MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_PLAGUE:
		damage = lpEffect->m_value[2];
		absorb = 0;
		effect = 2;
		break;
	case EFFECT_FREEZING:
		break;
	case EFFECT_BLEEDING2:
		damage = lpEffect->m_value[2];
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_SHOCK:
		break;
	case EFFECT_ADDICTED:
		damage = lpEffect->m_value[2];
		absorb = 0;
		effect = 2;
		break;
	case EFFECT_FREEZING1:
		break;
	case EFFECT_EXCESSIVE_BLEEDING:
		damage = lpEffect->m_value[2];
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_SHOCK1:
		break;
	}

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER && effect == 3)
	{
		if(!lpObj->isGhost)
		{
			lpObj->Life -= damage;
		}

		/*shield = gAttack.GetShieldDamage(lpTarget, lpObj, damage);

		if (lpObj->Life < (damage - shield))
		{
			lpObj->Life = 0;
		}
		else
		{
			lpObj->Life -= damage - shield;
		}

		if (lpObj->Shield < shield)
		{
			lpObj->Shield = 0;
		}
		else
		{
			lpObj->Shield -= shield;
		}*/
	}
	else
	{
		if (!lpObj->isGhost)
		{
			if (lpObj->Life < damage)
			{
				lpObj->Life = 0;
			}
			else
			{
				lpObj->Life -= damage;
			}
		}		
	}

	if ((lpTarget->Life + absorb) > (lpTarget->MaxLife + lpTarget->AddLife))
	{
		lpTarget->Life = lpTarget->MaxLife + lpTarget->AddLife;
	}
	else
	{
		lpTarget->Life += absorb;
	}

	if (damage > 0)
	{
		gObjectManager.CharacterLifeCheck(lpTarget, lpObj, (damage - shield), effect, 0, 0, 0, shield);
	}

	if (absorb > 0)
	{
		GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
	}
}

void CEffectManager::GCEffectStateSend(LPOBJ lpObj, BYTE state, WORD effect) // OK
{
	PMSG_EFFECT_STATE_SEND pMsg;

	pMsg.header.set(0x07, sizeof(pMsg));
	pMsg.state = state;
	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
	pMsg.effect = effect;

	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}

	if (CC_MAP_RANGE(lpObj->Map) == 0)
	{
		MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
	}
}

void CEffectManager::GCPeriodicEffectSend(LPOBJ lpObj, WORD group, WORD value, BYTE state, DWORD time, WORD effect) // OK
{
	//c1:24:2d:00:00:00:75:00:01:00:00:00:00:00:00:00:d0:00:25:80:ff:04:00:d0:00:ff:ff:ff:ff:ff:00:00:00:00:00:00
	PMSG_PERIODIC_EFFECT_SEND pMsg;

	pMsg.header.set(0x2D, sizeof(pMsg));

	pMsg.group = group;
	pMsg.value = value;
	pMsg.state = state;
	pMsg.time = time;
	pMsg.effect = effect;
	pMsg.wEffectValue = value;

	if (effect == EFFECT_DARK_SPIRITD)
	{
		gItemManager.ItemByteConvert(pMsg.ItemInfo, lpObj->Inventory[lpObj->m_btInvenPetDkSpiritPos]);
	}
	else if ((effect == EFFECT_USE_MOUNT_UNIRIA || effect == EFFECT_USE_MOUNT_DINORANT || effect == EFFECT_USE_MOUNT_DLHORSE || effect == EFFECT_USE_MOUNT_FENRIR) && lpObj->Inventory[lpObj->m_btInvenPetPos].m_JewelOfHarmonyOption == 1)
	{		
		gItemManager.ItemByteConvert(pMsg.ItemInfo, lpObj->Inventory[lpObj->m_btInvenPetPos]);
	}

	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}

	this->GCEffectStateSend(lpObj, ((state == 0) ? 1 : 0), effect);

	GCCharacterAttackSpeedSend(lpObj);
	gEffectManager.GCPartyEffectListSend(lpObj);
}

void CEffectManager::GCPartyEffectListSend(LPOBJ lpObj) // OK
{
#if(GAMESERVER_UPDATE>=802)

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		return;
	}

	BYTE send[1024];

	PMSG_PARTY_EFFECT_LIST_SEND pMsg;

	pMsg.header.set(0x2E, 0);

	int size = sizeof(pMsg);

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	pMsg.count = this->GeneratePartyEffectList(lpObj, send, &size);

	pMsg.header.size = size;

	memcpy(send, &pMsg, sizeof(pMsg));

	for (int n = 0; n < MAX_PARTY_USER; n++)
	{
		if (OBJECT_RANGE(gParty.m_PartyInfo[lpObj->PartyNumber].Index[n]) != 0)
		{
			DataSend(gParty.m_PartyInfo[lpObj->PartyNumber].Index[n], send, size);
		}
	}

#endif
}
