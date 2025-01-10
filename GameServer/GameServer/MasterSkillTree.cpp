#include "stdafx.h"
#include "MasterSkillTree.h"
#include "GameMain.h"
#include "ItemManager.h"
#include "MemScript.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "Util.h"
#include "DSProtocol.h"
#include "Notice.h"

CMasterSkillTree gMasterSkillTree;

CMasterSkillTree::CMasterSkillTree() // OK
{
	this->m_MasterSkillTreeInfo.clear();
}

CMasterSkillTree::~CMasterSkillTree() // OK
{

}

void CMasterSkillTree::Load(char* path) // OK
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

	this->m_MasterSkillTreeInfo.clear();

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

			MASTER_SKILL_TREE_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.Group = lpMemScript->GetAsNumber();

			info.Rank = lpMemScript->GetAsNumber();

			info.MinLevel = lpMemScript->GetAsNumber();

			info.MaxLevel = lpMemScript->GetAsNumber();

			for (int n = 0; n < MAX_SKILL_TREE_LEVEL; n++) { info.MainValue[n] = lpMemScript->GetAsFloatNumber(); }

			info.RelatedSkill = lpMemScript->GetAsNumber();

			info.ReplaceSkill = lpMemScript->GetAsNumber();

			info.RequireSkill[0] = lpMemScript->GetAsNumber();

			info.RequireSkill[1] = lpMemScript->GetAsNumber();

			for (int n = 0; n < MAX_CLASS; n++) { info.RequireClass[n] = lpMemScript->GetAsNumber(); }

			this->m_MasterSkillTreeInfo.insert(std::pair<int, MASTER_SKILL_TREE_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CMasterSkillTree::GetInfo(int index, MASTER_SKILL_TREE_INFO* lpInfo) // OK
{
	std::map<int, MASTER_SKILL_TREE_INFO>::iterator it = this->m_MasterSkillTreeInfo.find(index);

	if (it == this->m_MasterSkillTreeInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

int CMasterSkillTree::GetMasterSkillRelated(int index) // OK
{
	std::map<int, MASTER_SKILL_TREE_INFO>::iterator it = this->m_MasterSkillTreeInfo.find(index);

	if (it == this->m_MasterSkillTreeInfo.end())
	{
		return 0;
	}
	else
	{
		return it->second.RelatedSkill;
	}
}

flt CMasterSkillTree::GetMasterSkillValue(int index, int level) // OK
{
	std::map<int, MASTER_SKILL_TREE_INFO>::iterator it = this->m_MasterSkillTreeInfo.find(index);

	if (it == this->m_MasterSkillTreeInfo.end())
	{
		return 0;
	}
	else
	{
		return it->second.MainValue[level];
	}
}

int CMasterSkillTree::GetMasterSkillValue(LPOBJ lpObj, int index) // OK
{
	CSkill* lpMasterSkill = ((lpObj->Type == OBJECT_USER) ? gSkillManager.GetMasterSkill(lpObj, index) : 0);

	if (lpMasterSkill == 0)
	{
		return 0;
	}
	else
	{
		return (int)GetRoundValue(this->GetMasterSkillValue(lpMasterSkill->m_index, lpMasterSkill->m_level));
	}
}

int CMasterSkillTree::GetMasterSkillLevel(LPOBJ lpObj, int index) // OK
{
	CSkill* lpMasterSkill = ((lpObj->Type == OBJECT_USER) ? gSkillManager.GetMasterSkill(lpObj, index) : 0);

	if (lpMasterSkill == 0)
	{
		return 0;
	}
	else
	{
		return (lpMasterSkill->m_level + 1);
	}
}

int CMasterSkillTree::GetMasterSkillWeapon(LPOBJ lpObj, int index) // OK
{
	int WeaponSkill = index;

	switch (index)
	{
	case SKILL_DEFENSE:
		break;
	case SKILL_FALLING_SLASH:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_FALLING_SLASH_IMPROVED) == 0) ? WeaponSkill : MASTER_SKILL_ADD_FALLING_SLASH_IMPROVED);
		break;
	case SKILL_LUNGE:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_LUNGE_IMPROVED) == 0) ? WeaponSkill : MASTER_SKILL_ADD_LUNGE_IMPROVED);
		break;
	case SKILL_UPPERCUT:
		break;
	case SKILL_CYCLONE:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_CYCLONE_IMPROVED1) == 0) ? WeaponSkill : MASTER_SKILL_ADD_CYCLONE_IMPROVED1);
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_CYCLONE_IMPROVED2) == 0) ? WeaponSkill : MASTER_SKILL_ADD_CYCLONE_IMPROVED2);
		break;
	case SKILL_SLASH:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_SLASH_IMPROVED) == 0) ? WeaponSkill : MASTER_SKILL_ADD_SLASH_IMPROVED);
		break;
	case SKILL_TRIPLE_SHOT:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_TRIPLE_SHOT_IMPROVED) == 0) ? WeaponSkill : MASTER_SKILL_ADD_TRIPLE_SHOT_IMPROVED);
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_TRIPLE_SHOT_ENHANCED) == 0) ? WeaponSkill : MASTER_SKILL_ADD_TRIPLE_SHOT_ENHANCED);
		break;
	case SKILL_FIRE_BREATH:
		break;
	case SKILL_POWER_SLASH:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_POWER_SLASH_IMPROVED) == 0) ? WeaponSkill : MASTER_SKILL_ADD_POWER_SLASH_IMPROVED);
		break;
	case SKILL_EARTHQUAKE:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_EARTHQUAKE_IMPROVED) == 0) ? WeaponSkill : MASTER_SKILL_ADD_EARTHQUAKE_IMPROVED);
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_ADD_EARTHQUAKE_ENHANCED) == 0) ? WeaponSkill : MASTER_SKILL_ADD_EARTHQUAKE_ENHANCED);
		break;
	case SKILL_PLASMA_STORM:
		break;
	case SKILL_SAHAMUTT:
		break;
	case SKILL_NEIL:
		break;
	case SKILL_GHOST_PHANTOM:
		break;
	case SKILL_KILLING_BLOW:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_KILLING_BLOW_STRENGTHENER) == 0) ? WeaponSkill : MASTER_SKILL_KILLING_BLOW_STRENGTHENER);
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_KILLING_BLOW_MASTERY) == 0) ? WeaponSkill : MASTER_SKILL_KILLING_BLOW_MASTERY);
		break;
	case SKILL_UPPER_BEAST:
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_BEAST_UPPERCUT_STRENGTHENER) == 0) ? WeaponSkill : MASTER_SKILL_BEAST_UPPERCUT_STRENGTHENER);
		WeaponSkill = ((gSkillManager.GetMasterSkill(lpObj, MASTER_SKILL_BEAST_UPPERCUT_MASTERY) == 0) ? WeaponSkill : MASTER_SKILL_BEAST_UPPERCUT_MASTERY);
		break;
	case SKILL_PHOENIX_SHOT:
		break;
	}

	return WeaponSkill;
}

int CMasterSkillTree::GetMasterSkillDamageMin(LPOBJ lpObj, int index) // OK
{
	int damage = 0;

	switch (index)
	{
	/*case SKILL_POISON:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_POISON_IMPROVED);
		break;*/
	case SKILL_METEORITE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_METEORITE_IMPROVED);
		break;
	case SKILL_LIGHTNING:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LIGHTNING_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LIGHTNING_IMPROVED2);
		break;
	case SKILL_FLAME:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FLAME_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FLAME_IMPROVED2);
		break;
	case SKILL_ICE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_IMPROVED2);
		break;
	case SKILL_EVIL_SPIRIT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EVIL_SPIRIT_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EVIL_SPIRIT_IMPROVED2);
		break;
	case SKILL_HELL_FIRE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_HELL_FIRE_IMPROVED);
		break;
	case SKILL_BLAST:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLAST_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLAST_IMPROVED2);
		break;
	case SKILL_INFERNO:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_INFERNO_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_INFERNO_IMPROVED2);
		break;
	case SKILL_FALLING_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FALLING_SLASH_IMPROVED);
		break;
	case SKILL_LUNGE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LUNGE_IMPROVED);
		break;
	case SKILL_CYCLONE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CYCLONE_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CYCLONE_IMPROVED2);
		break;
	case SKILL_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SLASH_IMPROVED);
		break;
	case SKILL_TRIPLE_SHOT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_TRIPLE_SHOT_IMPROVED);
		break;
	case SKILL_DECAY:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DECAY_IMPROVED);
		break;
	case SKILL_ICE_STORM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_STORM_IMPROVED);
		break;
	case SKILL_NOVA:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_NOVA_IMPROVED);
		break;
	case SKILL_TWISTING_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_TWISTING_SLASH_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_TWISTING_SLASH_IMPROVED2);
		break;
	case SKILL_RAGEFUL_BLOW:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_RAGEFUL_BLOW_IMPROVED);
		break;
	case SKILL_DEATH_STAB:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DEATH_STAB_IMPROVED);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DEATH_STAB_ENHANCED);
		break;
	case SKILL_ICE_ARROW:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_ARROW_IMPROVED);
		break;
	case SKILL_PENETRATION:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_PENETRATION_IMPROVED);
		break;
	case SKILL_FIRE_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_SLASH_IMPROVED);
		break;
	case SKILL_POWER_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_POWER_SLASH_IMPROVED);
		break;
	case SKILL_FIRE_BURST:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_BURST_IMPROVED);
		break;
	case SKILL_EARTHQUAKE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EARTHQUAKE_IMPROVED);
		break;
	case SKILL_ELECTRIC_SPARK:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ELECTRIC_SPARK_IMPROVED);
		break;
	case SKILL_FORCE_WAVE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FORCE_WAVE_IMPROVED);
		break;
	case SKILL_FIRE_SCREAM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_SCREAM_IMPROVED);
		break;
	case SKILL_DRAIN_LIFE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAIN_LIFE_IMPROVED);
		break;
	case SKILL_CHAIN_LIGHTNING:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CHAIN_LIGHTNING_IMPROVED);
		break;
	case SKILL_SAHAMUTT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SAHAMUTT_IMPROVED);
		break;
	case SKILL_NEIL:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_NEIL_IMPROVED);
		break;
	case SKILL_GHOST_PHANTOM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GHOST_PHANTOM_IMPROVED);
		break;
	case SKILL_RED_STORM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_RED_STORM_IMPROVED);
		break;
	case SKILL_FROZEN_STAB:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FROZEN_STAB_IMPROVED);
		break;
	case SKILL_FIVE_SHOT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIVE_SHOT_IMPROVED);
		break;
	case SKILL_FLAME_STRIKE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_FLAME_STRIKE_STRENGTHENER);
		break;
	case SKILL_LIGHTNING_STORM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LIGHTNING_STORM_IMPROVED);
		break;
	case SKILL_BIRDS:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BIRDS_IMPROVED);
		break;
	case SKILL_KILLING_BLOW:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_KILLING_BLOW_STRENGTHENER);
		break;
	case SKILL_UPPER_BEAST:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_BEAST_UPPERCUT_STRENGTHENER);
		break;
	case SKILL_CHAIN_DRIVER:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CHAIN_DRIVER_IMPROVED);
		break;
	case SKILL_DARK_SIDE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DARK_SIDE_IMPROVED);
		break;
	case SKILL_DRAGON_LORE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAGON_LORE_IMPROVED);
		break;
	case SKILL_DRAGON_SLAYER:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAGON_SLAYER_IMPROVED);
		break;
	case SKILL_BLOOD_STORM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLOOD_STORM_IMPROVED);
		break;
	case SKILL_POISON_ARROW:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_POISON_ARROW_IMPROVED);
		break;
	case SKILL_EARTH_PRISON:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EARTH_PRISON_IMPROVED);
		break;
	case SKILL_PHOENIX_SHOT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ENHANCE_PHOENIX_SHOT);
		break;
	case SKILL_SPIN_STEP:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_SPIN_STEP_POW_UP);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GL_SPIN_SETP_MASTERY);
		break;
	case SKILL_HARSH_STRIKE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_HARSH_STRIKE_POW_UP);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_HARSH_STRIKE_MASTERY);
		break;
	case SKILL_MAGIC_PIN:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_MAGIC_PIN_POW_UP);
		//damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_MAGIC_PIN_PROFICIENCY);
		break;
	case SKILL_BRECHE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_BRECHE_POW_UP);
		//damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_BRECHE_PROFICIENCY);
		break;
	case SKILL_SHINING_PEAK:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_SHINING_PEAK_POW_UP);
		break;
	}

	return (int)(damage * 1.0f);
}

int CMasterSkillTree::GetMasterSkillDamageMax(LPOBJ lpObj, int index) // OK
{
	int damage = 0;

	switch (index)
	{
	/*case SKILL_POISON:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_POISON_IMPROVED);
		break;*/
	case SKILL_METEORITE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_METEORITE_IMPROVED);
		break;
	case SKILL_LIGHTNING:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LIGHTNING_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LIGHTNING_IMPROVED2);
		break;
	case SKILL_FLAME:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FLAME_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FLAME_IMPROVED2);
		break;
	case SKILL_ICE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_IMPROVED2);
		break;
	case SKILL_EVIL_SPIRIT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EVIL_SPIRIT_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EVIL_SPIRIT_IMPROVED2);
		break;
	case SKILL_HELL_FIRE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_HELL_FIRE_IMPROVED);
		break;
	case SKILL_BLAST:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLAST_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLAST_IMPROVED2);
		break;
	case SKILL_INFERNO:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_INFERNO_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_INFERNO_IMPROVED2);
		break;
	case SKILL_FALLING_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FALLING_SLASH_IMPROVED);
		break;
	case SKILL_LUNGE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LUNGE_IMPROVED);
		break;
	case SKILL_CYCLONE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CYCLONE_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CYCLONE_IMPROVED2);
		break;
	case SKILL_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SLASH_IMPROVED);
		break;
	case SKILL_TRIPLE_SHOT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_TRIPLE_SHOT_IMPROVED);
		break;
	case SKILL_DECAY:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DECAY_IMPROVED);
		break;
	case SKILL_ICE_STORM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_STORM_IMPROVED);
		break;
	case SKILL_NOVA:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_NOVA_IMPROVED);
		break;
	case SKILL_TWISTING_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_TWISTING_SLASH_IMPROVED1);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_TWISTING_SLASH_IMPROVED2);
		break;
	case SKILL_RAGEFUL_BLOW:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_RAGEFUL_BLOW_IMPROVED);
		break;
	case SKILL_DEATH_STAB:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DEATH_STAB_IMPROVED);
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DEATH_STAB_ENHANCED);
		break;
	case SKILL_ICE_ARROW:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_ARROW_IMPROVED);
		break;
	case SKILL_PENETRATION:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_PENETRATION_IMPROVED);
		break;
	case SKILL_FIRE_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_SLASH_IMPROVED);
		break;
	case SKILL_POWER_SLASH:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_POWER_SLASH_IMPROVED);
		break;
	case SKILL_FIRE_BURST:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_BURST_IMPROVED);
		break;
	case SKILL_EARTHQUAKE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EARTHQUAKE_IMPROVED);
		break;
	case SKILL_ELECTRIC_SPARK:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ELECTRIC_SPARK_IMPROVED);
		break;
	case SKILL_FORCE_WAVE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FORCE_WAVE_IMPROVED);
		break;
	case SKILL_FIRE_SCREAM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_SCREAM_IMPROVED);
		break;
	case SKILL_DRAIN_LIFE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAIN_LIFE_IMPROVED);
		break;
	case SKILL_CHAIN_LIGHTNING:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CHAIN_LIGHTNING_IMPROVED);
		break;
	case SKILL_SAHAMUTT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SAHAMUTT_IMPROVED);
		break;
	case SKILL_NEIL:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_NEIL_IMPROVED);
		break;
	case SKILL_GHOST_PHANTOM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GHOST_PHANTOM_IMPROVED);
		break;
	case SKILL_RED_STORM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_RED_STORM_IMPROVED);
		break;
	case SKILL_FROZEN_STAB:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FROZEN_STAB_IMPROVED);
		break;
	case SKILL_FIVE_SHOT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIVE_SHOT_IMPROVED);
		break;
	case SKILL_FLAME_STRIKE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_FLAME_STRIKE_STRENGTHENER);
		break;
	case SKILL_LIGHTNING_STORM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LIGHTNING_STORM_IMPROVED);
		break;
	case SKILL_BIRDS:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BIRDS_IMPROVED);
		break;
	case SKILL_KILLING_BLOW:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_KILLING_BLOW_STRENGTHENER);
		break;
	case SKILL_UPPER_BEAST:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_BEAST_UPPERCUT_STRENGTHENER);
		break;
	case SKILL_CHAIN_DRIVER:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CHAIN_DRIVER_IMPROVED);
		break;
	case SKILL_DARK_SIDE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DARK_SIDE_IMPROVED);
		break;
	case SKILL_DRAGON_LORE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAGON_LORE_IMPROVED);
		break;
	case SKILL_DRAGON_SLAYER:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAGON_SLAYER_IMPROVED);
		break;
	case SKILL_BLOOD_STORM:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLOOD_STORM_IMPROVED);
		break;
	case SKILL_POISON_ARROW:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_POISON_ARROW_IMPROVED);
		break;
	case SKILL_EARTH_PRISON:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EARTH_PRISON_IMPROVED);
		break;
	case SKILL_PHOENIX_SHOT:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ENHANCE_PHOENIX_SHOT);
		break;
	case SKILL_SPIN_STEP:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_SPIN_STEP_POW_UP);
		break;
	case SKILL_HARSH_STRIKE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_HARSH_STRIKE_POW_UP);
		break;
	case SKILL_MAGIC_PIN:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_MAGIC_PIN_POW_UP);
		break;
	case SKILL_BRECHE:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_BRECHE_POW_UP);
		break;
	case SKILL_SHINING_PEAK:
		damage += this->GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_SHINING_PEAK_POW_UP);
		break;
	}

	return (int)(damage * 1.5f);
}

bool CMasterSkillTree::CheckMasterLevel(LPOBJ lpObj) // OK
{
	if (gServerInfo.m_MasterSkillTree == 0)
	{
		return 0;
	}

	if (lpObj->Level >= MAX_CHARACTER_LEVEL && lpObj->ChangeUp >= 2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CMasterSkillTree::CheckMasterReplaceSkill(LPOBJ lpObj, int index) // OK
{
	CSkill* lpSkill = gSkillManager.GetSkill(lpObj, index);

	if (index != 0 && lpSkill == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool CMasterSkillTree::CheckMasterRequireSkill(LPOBJ lpObj, int index) // OK
{
	CSkill* lpMasterSkill = gSkillManager.GetMasterSkill(lpObj, index);

	if (index != 0 && (lpMasterSkill == 0 || (lpMasterSkill->IsMasterSkill() != 0 && (lpMasterSkill->m_level + 1) < MIN_SKILL_TREE_LEVEL)))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool CMasterSkillTree::CheckMasterRequireGroup(LPOBJ lpObj, int group, int rank) // OK
{
	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() != 0)
		{
			MASTER_SKILL_TREE_INFO MasterSkillTreeInfo;

			if (this->GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				if (MasterSkillTreeInfo.Group == group && MasterSkillTreeInfo.Rank == rank && (lpObj->MasterSkill[n].m_level + 1) >= MIN_SKILL_TREE_LEVEL)
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

void CMasterSkillTree::SetMasterLevelExperienceTable() // OK
{
	this->m_MasterLevelExperienceTable[0] = 0;
	QWORD var1 = 0;
	QWORD var2 = 0;
	QWORD var3 = 0;

	for (int n = 1; n < (MAX_CHARACTER_MASTER_LEVEL + 1); n++)
	{
		var1 = n + MAX_CHARACTER_LEVEL;

		var3 = ((((var1 + 9) * var1) * var1) * 10);

		var2 = var1 - 255;

		var3 += ((((var2 + 9) * var2) * var2) * 1000);

		var3 = (var3 - 3892250000) / 2;

		this->m_MasterLevelExperienceTable[n] = (QWORD)((n > 200) ? (var3 * (1 + (((var1 - 600) * (var1 - 600)) * 1.2) / 100000)) : var3);
	}
}

void CMasterSkillTree::CalcMasterLevelNextExperience(LPOBJ lpObj) // OK
{
	lpObj->MasterExperience = ((lpObj->MasterExperience < this->m_MasterLevelExperienceTable[lpObj->MasterLevel]) ? this->m_MasterLevelExperienceTable[lpObj->MasterLevel] : lpObj->MasterExperience);
	lpObj->MasterNextExperience = this->m_MasterLevelExperienceTable[((lpObj->MasterLevel >= MAX_CHARACTER_MASTER_LEVEL) ? MAX_CHARACTER_MASTER_LEVEL : (lpObj->MasterLevel + 1))];
}

void CMasterSkillTree::CalcMasterSkillTreeOption(LPOBJ lpObj, bool flag) // OK
{
	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() != 0)
		{
			MASTER_SKILL_TREE_INFO MasterSkillTreeInfo;

			if (this->GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				this->InsertOption(lpObj, MasterSkillTreeInfo.Index, (int)GetRoundValue(this->GetMasterSkillValue(MasterSkillTreeInfo.Index, (((lpObj->MasterSkill[n].m_level + 0) >= MasterSkillTreeInfo.MaxLevel) ? (MasterSkillTreeInfo.MaxLevel - 1) : (lpObj->MasterSkill[n].m_level + 0)))), flag);
			}
		}
	}
}

void CMasterSkillTree::InsertOption(LPOBJ lpObj, int index, int value, bool flag) // OK
{
	if (flag == 0)
	{
		if (index == MASTER_SKILL_ADD_ENERGY1 || index == MASTER_SKILL_ADD_VITALITY1 || index == MASTER_SKILL_ADD_DEXTERITY1 || index == MASTER_SKILL_ADD_STRENGTH1 || index == MASTER_SKILL_ADD_LEADERSHIP || index == MASTER_SKILL_ADD_ENERGY2 || index == MASTER_SKILL_ADD_VITALITY2 || index == MASTER_SKILL_ADD_DEXTERITY2 || index == MASTER_SKILL_ADD_STRENGTH2)
		{
			return;
		}
	}
	else
	{
		if (index != MASTER_SKILL_ADD_ENERGY1 && index != MASTER_SKILL_ADD_VITALITY1 && index != MASTER_SKILL_ADD_DEXTERITY1 && index != MASTER_SKILL_ADD_STRENGTH1 && index != MASTER_SKILL_ADD_LEADERSHIP && index != MASTER_SKILL_ADD_ENERGY2 && index != MASTER_SKILL_ADD_VITALITY2 && index != MASTER_SKILL_ADD_DEXTERITY2 && index != MASTER_SKILL_ADD_STRENGTH2)
		{
			return;
		}
	}

	switch (index)
	{
	case MASTER_SKILL_ADD_ITEM_DURABILITY_RATE1:
	case MASTER_SKILL_ADD_ITEM_DURABILITY_RATE2:
		lpObj->m_MPSkillOpt.WeaponDurabilityRate += value;
		lpObj->m_MPSkillOpt.ArmorDurabilityRate += value;
		break;
	case MASTER_SKILL_ADD_DEFENSE_SUCCESS_RATE_PVP1:
	case MASTER_SKILL_ADD_DEFENSE_SUCCESS_RATE_PVP2:
		lpObj->m_MPSkillOpt.DefenseSuccessRatePvP += value;
		break;
	case MASTER_SKILL_ADD_MAX_SD1:
	case MASTER_SKILL_ADD_MAX_SD2:
		lpObj->m_MPSkillOpt.AddShield += value;
		break;
	case MASTER_SKILL_ADD_MP_RECOVERY_RATE1:
	case MASTER_SKILL_ADD_MP_RECOVERY_RATE2:
		lpObj->m_MPSkillOpt.MPRecoveryRate += value;
		break;
		/*case MASTER_SKILL_ADD_POISON_RESISTANCE1:    DISABLED TO SEASON 13
			lpObj->m_MPSkillOpt.Resistance[1] += value;
			break;*/
	case MASTER_SKILL_ADD_JEWELRY_DURABILITY_RATE1:
	case MASTER_SKILL_ADD_JEWELRY_DURABILITY_RATE2:
		lpObj->m_MPSkillOpt.PendantDurabilityRate += value;
		lpObj->m_MPSkillOpt.RingDurabilityRate += value;
		break;
	case MASTER_SKILL_ADD_SD_RECOVERY_RATE1:
	case MASTER_SKILL_ADD_SD_RECOVERY_RATE2:
		lpObj->m_MPSkillOpt.SDRecoveryRate += value;
		break;
	case MASTER_SKILL_ADD_HP_RECOVERY_RATE1:
	case MASTER_SKILL_ADD_HP_RECOVERY_RATE2:
		lpObj->m_MPSkillOpt.HPRecoveryRate += value;
		break;
		/*case MASTER_SKILL_ADD_LIGHTNING_RESISTANCE1:    DISABLED TO SEASON 13
			lpObj->m_MPSkillOpt.Resistance[2] += value;
			break;*/
	case MASTER_SKILL_ADD_DEFENSE1:
	case MASTER_SKILL_ADD_DEFENSE2:
		lpObj->Defense += value;
		break;
	case MASTER_SKILL_ADD_BP_RECOVERY_RATE1:
	case MASTER_SKILL_ADD_BP_RECOVERY_RATE2:
		lpObj->m_MPSkillOpt.BPRecoveryRate += value;
		break;
		/*case MASTER_SKILL_ADD_ICE_RESISTANCE1:    DISABLED TO SEASON 13
			lpObj->m_MPSkillOpt.Resistance[0] += value;
			break;*/
	case MASTER_SKILL_ADD_GUARDIAN_DURABILITY_RATE1:
	case MASTER_SKILL_ADD_GUARDIAN_DURABILITY_RATE2:
		lpObj->m_MPSkillOpt.GuardianDurabilityRate += value;
		break;
	case MASTER_SKILL_ADD_DEFENSE_SUCCESS_RATE1:
	case MASTER_SKILL_ADD_DEFENSE_SUCCESS_RATE3:
		lpObj->m_MPSkillOpt.DefenseSuccessRate += (lpObj->m_MPSkillOpt.DefenseSuccessRate * value) / 100;
		break;
	case MASTER_SKILL_ADD_ARMOR_SET_BONUS1:
	case MASTER_SKILL_ADD_ARMOR_SET_BONUS2:
		if (lpObj->ArmorSetBonus != 0)
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_REFLECT_DAMAGE1:
	case MASTER_SKILL_ADD_REFLECT_DAMAGE2:
		lpObj->m_MPSkillOpt.FullDamageReflectRate += value;
		break;
	case MASTER_SKILL_ADD_ENERGY1:
	case MASTER_SKILL_ADD_ENERGY2:
		lpObj->AddEnergy += value;
		//LogAdd(LOG_DEBUG, "F: %s L: %d AddEnergy: %d", __FUNCTION__, __LINE__, lpObj->AddEnergy);
		break;
	case MASTER_SKILL_ADD_VITALITY1:
	case MASTER_SKILL_ADD_VITALITY2:
		lpObj->AddVitality += value;
		break;
	case MASTER_SKILL_ADD_DEXTERITY1:
	case MASTER_SKILL_ADD_DEXTERITY2:
		lpObj->AddDexterity += value;
		break;
	case MASTER_SKILL_ADD_STRENGTH1:
	case MASTER_SKILL_ADD_STRENGTH2:
		lpObj->AddStrength += value;
		break;

	case MASTER_SKILL_ADD_DK_WING_DEFENSE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 36) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 431))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_DK_WING_DAMAGE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 36) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 431))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_ATTACK_SUCCESS_RATE1:
	case MASTER_SKILL_ADD_ATTACK_SUCCESS_RATE2:
		lpObj->m_MPSkillOpt.AttackSuccessRate += value;
		break;
	case MASTER_SKILL_ADD_MAX_HP1:
	case MASTER_SKILL_ADD_MAX_HP2:
	case MASTER_SKILL_ADD_MAX_HP3:
		lpObj->AddLife += value;
		break;
	case MASTER_SKILL_ADD_WEAPON_DAMAGE_BK:
	case MASTER_SKILL_ADD_WEAPON_DAMAGE_ELF:
	case MASTER_SKILL_ADD_WEAPON_DAMAGE_MG:
	case MASTER_SKILL_ADD_WEAPON_DAMAGE4:
	case MASTER_SKILL_ADD_WEAPON_DAMAGE_RF:
		lpObj->PhysiDamageMinLeft += value;
		lpObj->PhysiDamageMinRight += value;
		lpObj->PhysiDamageMaxLeft += value;
		lpObj->PhysiDamageMaxRight += value;
		break;
	case MASTER_SKILL_ADD_MAX_MP1:
	case MASTER_SKILL_ADD_MAX_MP2:
		lpObj->AddMana += value;
		break;
	case MASTER_SKILL_ADD_MAX_BP1:
	case MASTER_SKILL_ADD_MAX_BP2:
		lpObj->AddBP += value;
		break;
	case MASTER_SKILL_ADD_BLOOD_STORM:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_BLOOD_STORM, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_BLOOD_STORM, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_ATTACK_SUCCESS_RATE_PVP1:
	case MASTER_SKILL_ADD_ATTACK_SUCCESS_RATE_PVP2:
		lpObj->m_MPSkillOpt.AttackSuccessRatePvP += value;
		break;
	case MASTER_SKILL_ADD_TWO_HAND_SWORD_DAMAGE:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(0, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(1, 0) && lpObj->Inventory[0].m_TwoHand != 0)
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_ONE_HAND_SWORD_DAMAGE:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(0, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(1, 0) && lpObj->Inventory[0].m_TwoHand == 0)
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_MACE_DAMAGE:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(2, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(3, 0))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_SPEAR_DAMAGE:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(3, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(4, 0))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_TWO_HAND_SWORD_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(0, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(1, 0) && lpObj->Inventory[0].m_TwoHand != 0)
		{
			lpObj->DamagePvP += value;
		}
		break;
	case MASTER_SKILL_ADD_ONE_HAND_SWORD_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(0, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(1, 0) && lpObj->Inventory[0].m_TwoHand == 0)
		{
			lpObj->PhysiSpeed += value;
			lpObj->MagicSpeed += value;
		}
		break;
	case MASTER_SKILL_ADD_MACE_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(2, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(3, 0))
		{
			lpObj->m_MPSkillOpt.IgnoreDefenseRate += value;
		}
		break;
	case MASTER_SKILL_ADD_SPEAR_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(3, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(4, 0))
		{
			lpObj->m_MPSkillOpt.DoubleDamageRate += value;
		}
		break;
	case MASTER_SKILL_ADD_MP_CONSUMPTION_RATE1:
	case MASTER_SKILL_ADD_MP_CONSUMPTION_RATE2:
		lpObj->MPConsumptionRate -= value;
		break;
	case MASTER_SKILL_ADD_HUNT_SD1:
	case MASTER_SKILL_ADD_HUNT_SD2:
		lpObj->HuntSD += value;
		break;
	case MASTER_SKILL_ADD_HUNT_HP1:
	case MASTER_SKILL_ADD_HUNT_HP2:
		lpObj->HuntHP += value;
		break;
	case MASTER_SKILL_ADD_MIN_PHYSI_DAMAGE1:
	case MASTER_SKILL_ADD_MIN_PHYSI_DAMAGE2:
	case MASTER_SKILL_ADD_MIN_PHYSI_DAMAGE3:
		lpObj->PhysiDamageMinLeft += value;
		lpObj->PhysiDamageMinRight += value;
		break;
	case MASTER_SKILL_ADD_HUNT_MP1:
	case MASTER_SKILL_ADD_HUNT_MP2:
		lpObj->HuntMP += value;
		break;
	case MASTER_SKILL_ADD_MAX_PHYSI_DAMAGE1:
	case MASTER_SKILL_ADD_MAX_PHYSI_DAMAGE2:
	case MASTER_SKILL_ADD_MAX_PHYSI_DAMAGE3:
		lpObj->PhysiDamageMaxLeft += value;
		lpObj->PhysiDamageMaxRight += value;
		break;
	case MASTER_SKILL_ADD_CRITICAL_DAMAGE_RATE1:
	case MASTER_SKILL_ADD_CRITICAL_DAMAGE_RATE2:
		lpObj->m_MPSkillOpt.CriticalDamageRate += value;
		break;
	case MASTER_SKILL_ADD_RESTORE_MP_RATE1:
	case MASTER_SKILL_ADD_RESTORE_MP_RATE2:
		lpObj->m_MPSkillOpt.OffensiveFullMPRestoreRate += value;
		break;
	case MASTER_SKILL_ADD_RESTORE_HP_RATE1:
	case MASTER_SKILL_ADD_RESTORE_HP_RATE2:
		lpObj->m_MPSkillOpt.OffensiveFullHPRestoreRate += value;
		break;
	case MASTER_SKILL_ADD_EXCELLENT_DAMAGE_RATE1:
	case MASTER_SKILL_ADD_EXCELLENT_DAMAGE_RATE2:
		lpObj->m_MPSkillOpt.ExcellentDamageRate += value;
		break;
	case MASTER_SKILL_ADD_DOUBLE_DAMAGE_RATE1:
	case MASTER_SKILL_ADD_DOUBLE_DAMAGE_RATE2:
		lpObj->m_MPSkillOpt.DoubleDamageRate += value;
		break;
	case MASTER_SKILL_ADD_IGNORE_DEFENSE_RATE1:
	case MASTER_SKILL_ADD_IGNORE_DEFENSE_RATE2:
		lpObj->m_MPSkillOpt.IgnoreDefenseRate += value;
		break;
	case MASTER_SKILL_ADD_RESTORE_SD_RATE1:
	case MASTER_SKILL_ADD_RESTORE_SD_RATE2:
		lpObj->m_MPSkillOpt.OffensiveFullSDRestoreRate += value;
		break;
	case MASTER_SKILL_ADD_DW_WING_DEFENSE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 37) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 430))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_DW_WING_DAMAGE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 37) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 430))
		{
			lpObj->MagicDamageMin += value;
			lpObj->MagicDamageMax += value;
		}
		break;
	case MASTER_SKILL_ADD_MAGIC_DAMAGE1:
	case MASTER_SKILL_ADD_MAGIC_DAMAGE2:
	case MASTER_SKILL_ADD_MAGIC_DAMAGE3:
		lpObj->MagicDamageMin += value;
		lpObj->MagicDamageMax += value;
		break;
	case MASTER_SKILL_ADD_ONE_HAND_STAFF_DAMAGE:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 0) && lpObj->Inventory[0].m_TwoHand == 0)
		{
			lpObj->MagicDamageMin += value;
			lpObj->MagicDamageMax += value;
		}
		break;
	case MASTER_SKILL_ADD_TWO_HAND_STAFF_DAMAGE:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 0) && lpObj->Inventory[0].m_TwoHand != 0)
		{
			lpObj->MagicDamageMin += value;
			lpObj->MagicDamageMax += value;
		}
		break;
	case MASTER_SKILL_ADD_SHIELD_DEFENSE1:
	case MASTER_SKILL_ADD_SHIELD_DEFENSE2:
	case MASTER_SKILL_ADD_SHIELD_DEFENSE3:
	case MASTER_SKILL_ADD_SHIELD_MASTERY1:	
	case MASTER_SKILL_ADD_SHIELD_MASTERY2:
	case MASTER_SKILL_ADD_SHIELD_MASTERY3:
		if (lpObj->Inventory[1].m_Index >= GET_ITEM(6, 0) && lpObj->Inventory[1].m_Index < GET_ITEM(7, 0))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_ONE_HAND_STAFF_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 0) && lpObj->Inventory[0].m_TwoHand == 0)
		{
			lpObj->PhysiSpeed += value;
			lpObj->MagicSpeed += value;
		}
		break;
	case MASTER_SKILL_ADD_TWO_HAND_STAFF_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 0) && lpObj->Inventory[0].m_TwoHand != 0)
		{
			lpObj->DamagePvP += value;
		}
		break;
	case MASTER_SKILL_ADD_MIN_MAGIC_DAMAGE1:
		lpObj->MagicDamageMin += value;
		break;
	case MASTER_SKILL_ADD_MAX_MAGIC_DAMAGE1:
		lpObj->MagicDamageMax += value;
		break;
	case MASTER_SKILL_ADD_FE_WING_DEFENSE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 38) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 432))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_FE_WING_DAMAGE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 38) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 432))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_CURE:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_CURE, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_CURE, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_PARTY_HEAL:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_PARTY_HEAL, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_PARTY_HEAL, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_POISON_ARROW:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_POISON_ARROW, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_POISON_ARROW, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_BLESS:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_BLESS, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_BLESS, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_SUMMON_SATYROS:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_SUMMON_SATYROS, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_SUMMON_SATYROS, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_BOW_DAMAGE:
		if (lpObj->Class == CLASS_FE && lpObj->Inventory[0].m_Index >= GET_ITEM(4, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(5, 0) && lpObj->Inventory[0].m_TwoHand == 0)
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_CROSS_BOW_DAMAGE:
		if (lpObj->Class == CLASS_FE && lpObj->Inventory[0].m_Index >= GET_ITEM(4, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(5, 0) && lpObj->Inventory[0].m_TwoHand == 1)
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_BOW_MASTERY:
		if (lpObj->Class == CLASS_FE && lpObj->Inventory[0].m_Index >= GET_ITEM(4, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(5, 0) && lpObj->Inventory[0].m_TwoHand == 0)
		{
			lpObj->PhysiSpeed += value;
			lpObj->MagicSpeed += value;
		}
		break;
	case MASTER_SKILL_ADD_CROSS_BOW_MASTERY:
		if (lpObj->Class == CLASS_FE && lpObj->Inventory[0].m_Index >= GET_ITEM(4, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(5, 0) && lpObj->Inventory[0].m_TwoHand == 1)
		{
			lpObj->DamagePvP += value;
		}
		break;
	case MASTER_SKILL_ADD_SU_WING_DEFENSE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 43))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_SU_WING_DAMAGE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 43))
		{
			lpObj->MagicDamageMin += value;
			lpObj->MagicDamageMax += value;
		}
		break;
	case MASTER_SKILL_ADD_BLIND:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_BLIND, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_BLIND, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_STICK_DAMAGE:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 0))
		{
			lpObj->MagicDamageMin += value;
			lpObj->MagicDamageMax += value;
		}
		break;
	case MASTER_SKILL_ADD_BOOK_DAMAGE:
		if (lpObj->Inventory[1].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[1].m_Index < GET_ITEM(6, 0))
		{
			lpObj->CurseDamageMin += value;
			lpObj->CurseDamageMax += value;
		}
		break;
	case MASTER_SKILL_ADD_STICK_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 0))
		{
			lpObj->DamagePvP += value;
		}
		break;
	case MASTER_SKILL_ADD_BOOK_MASTERY:
		if (lpObj->Inventory[1].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[1].m_Index < GET_ITEM(6, 0))
		{
			lpObj->PhysiSpeed += value;
			lpObj->MagicSpeed += value;
		}
		break;
	case MASTER_SKILL_ADD_MIN_MAGIC_DAMAGE2:
		lpObj->MagicDamageMin += value;
		lpObj->CurseDamageMin += value;
		break;
	case MASTER_SKILL_ADD_MAX_MAGIC_DAMAGE2:
		lpObj->MagicDamageMax += value;
		lpObj->CurseDamageMax += value;
		break;
	case MASTER_SKILL_ADD_MG_WING_DEFENSE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 39) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 433))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_MG_WING_DAMAGE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 39) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 433))
		{
			lpObj->MagicDamageMin += value;
			lpObj->MagicDamageMax += value;
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_EARTH_PRISON:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_EARTH_PRISON, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_EARTH_PRISON, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_DL_WING_DEFENSE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 40))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_LEADERSHIP:
		lpObj->AddLeadership += value;
		break;
	case MASTER_SKILL_ADD_DL_WING_DAMAGE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 40))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_DARK_HORSE_DEFENSE:
		if (lpObj->Inventory[0].m_Index == GET_ITEM(13, 4))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_IRON_DEFENSE:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_IRON_DEFENSE, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_IRON_DEFENSE, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_SCEPTER_DAMAGE:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(2, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(3, 0))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_SCEPTER_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(2, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(3, 0))
		{
			lpObj->DamagePvP += value;
		}
		break;
	case MASTER_SKILL_ADD_DEFENSE_BY_LEADERSHIP:
		lpObj->Defense += (lpObj->Leadership + lpObj->AddLeadership) / value;
		break;
	case MASTER_SKILL_ADD_PET_DURABILITY_RATE:
		lpObj->m_MPSkillOpt.PetDurabilityRate += value;
		break;
	case MASTER_SKILL_ADD_RF_WING_DEFENSE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 50) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 436))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_RF_WING_DAMAGE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 50) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 436))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
		/*case MASTER_SKILL_ADD_DEFENSE_SUCCESS_RATE2:
			lpObj->m_MPSkillOpt.DefenseSuccessRate += (lpObj->m_MPSkillOpt.DefenseSuccessRate*value)/100;
			break;*/
	case MASTER_SKILL_ADD_BLOOD_HOWLING:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_BLOOD_HOWLING, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_BLOOD_HOWLING, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_GLOVE_DAMAGE:
		if (lpObj->Class == CLASS_RF && lpObj->Inventory[0].m_Index >= GET_ITEM(0, 32) && lpObj->Inventory[0].m_Index < GET_ITEM(1, 0))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_GLOVE_MASTERY:
		if (lpObj->Class == CLASS_RF && lpObj->Inventory[0].m_Index >= GET_ITEM(0, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(1, 0))
		{
			lpObj->m_MPSkillOpt.DoubleDamageRate += value;
		}
		break;
		/*case MASTER_SKILL_ADD_TRIPLE_DAMAGE_RATE:
			lpObj->m_MPSkillOpt.TripleDamageRate += value;
			break;*/
	case MASTER_SKILL_ADD_DK_PROTECTION_SHIELD:
	case MASTER_SKILL_ADD_DW_PROTECTION_SHIELD:
	case MASTER_SKILL_ADD_FE_PROTECTION_SHIELD:
	case MASTER_SKILL_ADD_MG_PROTECTION_SHIELD:
	case MASTER_SKILL_ADD_DL_PROTECTION_SHIELD:
	case MASTER_SKILL_ADD_SU_PROTECTION_SHIELD:
	case MASTER_SKILL_ADD_FK_PROTECTION_SHIELD:
		lpObj->AbsorbRate += value;
		break;
	case MASTER_SKILL_ADD_DK_WEAPON_BLOCK:
	case MASTER_SKILL_ADD_DW_WEAPON_BLOCK:
	case MASTER_SKILL_ADD_FE_WEAPON_BLOCK:
	case MASTER_SKILL_ADD_MG_WEAPON_BLOCK:
	case MASTER_SKILL_ADD_DL_WEAPON_BLOCK:
	case MASTER_SKILL_ADD_SU_WEAPON_BLOCK:
	case MASTER_SKILL_ADD_FK_WEAPON_BLOCK:
		lpObj->ParryRate += value;
		break;
	case MASTER_SKILL_ADD_DK_SHIELD_BLOCK:
	case MASTER_SKILL_ADD_DW_SHIELD_BLOCK:
	case MASTER_SKILL_ADD_FE_SHIELD_BLOCK:
	case MASTER_SKILL_ADD_MG_SHIELD_BLOCK:
	case MASTER_SKILL_ADD_DL_SHIELD_BLOCK:
	case MASTER_SKILL_ADD_SU_SHIELD_BLOCK:
	case MASTER_SKILL_ADD_FK_SHIELD_BLOCK:
		lpObj->BlockRate += value;
		break;
	case MASTER_SKILL_ADD_DK_STEEL_ARMOR:
	case MASTER_SKILL_ADD_DW_STEEL_ARMOR:
	case MASTER_SKILL_ADD_FE_STEEL_ARMOR:
	case MASTER_SKILL_ADD_MG_STEEL_ARMOR:
	case MASTER_SKILL_ADD_SU_STEEL_ARMOR:
	case MASTER_SKILL_ADD_DL_STEEL_ARMOR:
	case MASTER_SKILL_ADD_RF_STEEL_ARMOR:
	case MASTER_SKILL_ADD_FK_STEEL_ARMOR:
		lpObj->ImpenetrableDefense += value;
		break;
	case MASTER_SKILL_ADD_DK_STRONGG_MIND:
		lpObj->ResistStunRate += value;
		break;
	case MASTER_SKILL_ADD_DK_ABSORB_LIFE:
	case MASTER_SKILL_ADD_DW_ABSORB_LIFE:
	case MASTER_SKILL_ADD_FE_ABSORB_LIFE:
	case MASTER_SKILL_ADD_SU_ABSORB_LIFE:
	case MASTER_SKILL_ADD_MG_ABSORB_LIFE:
	case MASTER_SKILL_ADD_DL_ABSORB_LIFE:
	case MASTER_SKILL_ADD_RF_ABSORB_LIFE:
	case MASTER_SKILL_ADD_FK_ABSORB_LIFE:
		lpObj->AbsorbLife += value;
		break;
	case MASTER_SKILL_ADD_DK_ABSORB_SHIELD:
	case MASTER_SKILL_ADD_DW_ABSORB_SHIELD:
	case MASTER_SKILL_ADD_FE_ABSORB_SHIELD:
	case MASTER_SKILL_ADD_MG_ABSORB_SHIELD:
	case MASTER_SKILL_ADD_DL_ABSORB_SHIELD:
	case MASTER_SKILL_ADD_SU_ABSORB_SHIELD:
	case MASTER_SKILL_ADD_RF_ABSORB_SHIELD:
	case MASTER_SKILL_ADD_FK_ABSORB_SHIELD:
		lpObj->AbsorbSD += value;
		break;
		//case MASTER_SKILL_ADD_DK_BATTLE_MIND:
	case MASTER_SKILL_ADD_DK_RUSH:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_RUSH, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_RUSH, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_DK_IRON_DEFENSE:
		/*if ((value = gSkillManager.AddSkill(lpObj, SKILL_IRON_DEFENSE1, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_IRON_DEFENSE1, 0, 0);
		}*/
		break;
	case MASTER_SKILL_ADD_DK_IRON_DEFENSE_POW_IP:
		break;
	case MASTER_SKILL_ADD_DW_STRONGG_MIND:
		lpObj->ResistStunRate += value;
		break;
		//case MASTER_SKILL_ADD_DW_GRAND_MAGIC_POW_UP:
		//	break;
	case MASTER_SKILL_ADD_DW_ILLUSION:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_ILLUSION, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_ILLUSION, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_FE_STRONGG_MIND:
		lpObj->ResistStunRate += value;
		break;
		//case MASTER_SKILL_ADD_FE_MARKSMAN:
		//	break;
	case MASTER_SKILL_ADD_FE_SHADOW_STEP:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_SHADOW_STEP, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_SHADOW_STEP, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_FE_EVASION:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_EVASION, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_EVASION, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_SU_STRONGG_MIND:
		lpObj->ResistStunRate += value;
		break;
	case MASTER_SKILL_ADD_SU_PAIN_OF_CURSE:
		lpObj->CurseDamageMin += value;
		lpObj->CurseDamageMax += value;
		break;
	case MASTER_SKILL_ADD_SU_PARALYZE:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_PARALYZE, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_PARALYZE, 0, 0);
		}
		break;
	case MASTER_SKILL_ADD_MG_STRONGG_MIND:
		lpObj->ResistStunRate += value;
		break;
	case MASTER_SKILL_ADD_DL_STRONGG_MIND:
		lpObj->ResistStunRate += value;
		break;
	case MASTER_SKILL_ADD_RF_WEAPON_BLOCK:
	case MASTER_SKILL_ADD_RF_SHIELD_BLOCK:
		lpObj->ImpenetrableDefense += value;
		break;
	case MASTER_SKILL_ADD_RF_STRONGG_MIND:
		lpObj->ResistStunRate += value;
		break;
	case MASTER_SKILL_ADD_FK_WING_DEFENSE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 270) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 437))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_FK_WING_DAMAGE:
		if (lpObj->Inventory[7].m_Index == GET_ITEM(12, 270) || lpObj->Inventory[7].m_Index == GET_ITEM(12, 437))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
	case MASTER_SKILL_ADD_FK_WEAPON_MASTERY:
		lpObj->PhysiDamageMinLeft += value;
		lpObj->PhysiDamageMinRight += value;
		lpObj->PhysiDamageMaxLeft += value;
		lpObj->PhysiDamageMaxRight += value;
		break;
	case MASTER_SKILL_ADD_FK_BURST:
	case MASTER_SKILL_ADD_FK_BURST_POW_UP:
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_BURST, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_BURST, 0, 0);
		}
		break;

	case MASTER_SKILL_ADD_FK_LANCE_POW_UP:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(3, 13) && lpObj->Inventory[0].m_Index < GET_ITEM(3, 19))
		{
			lpObj->PhysiDamageMinLeft += value;
			lpObj->PhysiDamageMinRight += value;
			lpObj->PhysiDamageMaxLeft += value;
			lpObj->PhysiDamageMaxRight += value;
		}
		break;
		//case MASTER_SKILL_ADD_GL_CIRCLE_SHIELD_POW_UP:
	case MASTER_SKILL_ADD_FK_SHIELD_POW_UP:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(6, 29) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 32))
		{
			lpObj->Defense += value;
		}
		break;
	case MASTER_SKILL_ADD_FK_LANCE_MASTERY:
		lpObj->m_MPSkillOpt.DoubleDamageRate += value;
		break;
	case MASTER_SKILL_ADD_GL_CIRCLE_SHIELD_MASTERY:
	case MASTER_SKILL_ADD_FK_SHIELD_MASTERY:
		if (lpObj->Inventory[0].m_Index >= GET_ITEM(6, 29) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 32))
		{
			lpObj->Defense += value;
		}
		break;
		//case MASTER_SKILL_ADD_FK_WRATH_POW_UP:
		//case MASTER_SKILL_ADD_FK_WRATH_PROFICIENCY:
		//case MASTER_SKILL_ADD_FK_WRATH_MASTERY:
	case MASTER_SKILL_ADD_FK_INCREASES_RETALIATION_DMG:
		lpObj->GLDamageMultiplierRate[0] += value;
		break;
	case MASTER_SKILL_ADD_FK_INCREASES_RAGE_DMG:
		lpObj->GLDamageMultiplierRate[1] += value;
		break;
	case MASTER_SKILL_ADD_FK_STRONGG_MIND:
		lpObj->ResistStunRate += value;
		break;
	}
}

bool CMasterSkillTree::ReplaceMasterSkill(LPOBJ lpObj, int index, int skill, int level) // OK
{
	CSkill* lpSkill = gSkillManager.GetSkill(lpObj, index);

	if (lpSkill == 0)
	{
		return ((index > 0) ? ((index == skill) ? 1 : 0) : 1);
	}
	else
	{
		return lpSkill->Set(lpObj->Index, skill, level);
	}
}

void CMasterSkillTree::CGMasterSkillRecv(PMSG_MASTER_SKILL_RECV* lpMsg, int aIndex) // OK
{
	LogAdd(LOG_RED, "MasterSkill: %d | MasterEmpty: %d", lpMsg->MasterSkill, lpMsg->MasterEmpty);

	if (gServerInfo.m_MasterSkillTree == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	MASTER_SKILL_TREE_INFO MasterSkillTreeInfo;

	if (this->GetInfo(lpMsg->MasterSkill, &MasterSkillTreeInfo) == 0)
	{
		LogAdd(LOG_RED, "[%s] [Skill: %d] Master Skill Tree info not found", lpObj->Name, lpMsg->MasterSkill);
		return;
	}

	if (lpObj->MasterPoint < MasterSkillTreeInfo.MinLevel)
	{
		LogAdd(LOG_RED, "[%s] [Skill: %d] Not enough points to activate (%d/%d)", lpObj->Name, lpMsg->MasterSkill, lpObj->MasterPoint, MasterSkillTreeInfo.MinLevel);
		return;
	}

	CSkill* lpMasterSkill = gSkillManager.GetMasterSkill(lpObj, MasterSkillTreeInfo.Index);

	if (lpMasterSkill == 0)
	{
		if (this->CheckMasterReplaceSkill(lpObj, MasterSkillTreeInfo.ReplaceSkill) == 0)
		{
			LogAdd(LOG_RED, "[%d] [Res: %d]", __LINE__, this->CheckMasterReplaceSkill(lpObj, MasterSkillTreeInfo.ReplaceSkill));
			return;
		}

		//TESTE SKILL TREE
		if (this->CheckMasterRequireSkill(lpObj, MasterSkillTreeInfo.RequireSkill[0]) == 0)
		{
			LogAdd(LOG_RED, "[%d] [Res: %d]", __LINE__, this->CheckMasterRequireSkill(lpObj, MasterSkillTreeInfo.RequireSkill[0]));
			return;
		}

		if (this->CheckMasterRequireSkill(lpObj, MasterSkillTreeInfo.RequireSkill[1]) == 0)
		{
			LogAdd(LOG_RED, "[%d] [Res: %d]", __LINE__, this->CheckMasterRequireSkill(lpObj, MasterSkillTreeInfo.RequireSkill[1]));
			return;
		}

		if (MasterSkillTreeInfo.Rank > 1 && this->CheckMasterRequireGroup(lpObj, MasterSkillTreeInfo.Group, (MasterSkillTreeInfo.Rank - 1)) == 0)
		{
			LogAdd(LOG_RED, "[%d] [Res: %d]", __LINE__, this->CheckMasterRequireGroup(lpObj, MasterSkillTreeInfo.Group, (MasterSkillTreeInfo.Rank - 1)));
			return;
		}

		if (this->ReplaceMasterSkill(lpObj, MasterSkillTreeInfo.ReplaceSkill, MasterSkillTreeInfo.Index, (MasterSkillTreeInfo.MinLevel - 1)) == 0)
		{
			LogAdd(LOG_RED, "[%d] [Res: %d]", __LINE__, this->ReplaceMasterSkill(lpObj, MasterSkillTreeInfo.ReplaceSkill, MasterSkillTreeInfo.Index, (MasterSkillTreeInfo.MinLevel - 1)));
			return;
		}

		int res = gSkillManager.AddMasterSkill(lpObj, MasterSkillTreeInfo.Index, (MasterSkillTreeInfo.MinLevel - 1));
		LogAdd(LOG_RED, "[%d] [Res: %d]", __LINE__, res);
	}
	else
	{
		if ((lpMasterSkill->m_level + 1) >= MasterSkillTreeInfo.MaxLevel)
		{
			LogAdd(LOG_RED, "[%d] [Res: %d %d]", __LINE__, (lpMasterSkill->m_level + 1), MasterSkillTreeInfo.MaxLevel);
			return;
		}

		if (this->ReplaceMasterSkill(lpObj, MasterSkillTreeInfo.Index, MasterSkillTreeInfo.Index, lpMasterSkill->m_level) == 0)
		{
			LogAdd(LOG_RED, "[%d] [Res: %d]", __LINE__, this->ReplaceMasterSkill(lpObj, MasterSkillTreeInfo.Index, MasterSkillTreeInfo.Index, lpMasterSkill->m_level));
			return;
		}

		int res = lpMasterSkill->Set(lpObj->Index, MasterSkillTreeInfo.Index, (lpMasterSkill->m_level + 1));
		LogAdd(LOG_RED, "[%d] [Res: %d]", __LINE__, res);
	}

	//gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Skill id: %d", lpMsg->MasterSkill);

	this->GCMasterSkillListSend(aIndex);
	gSkillManager.GCSkillListSend(lpObj, 0);
	lpObj->MasterPoint -= MasterSkillTreeInfo.MinLevel;
	gObjectManager.CharacterCalcAttribute(aIndex);
	this->GCMasterSkillSend(aIndex, 0, 0, lpObj->MasterPoint, lpMsg->MasterSkill, lpMsg->MasterEmpty);
	this->GCMasterInfoSend(lpObj);
}

void CMasterSkillTree::GCMasterInfoSend(LPOBJ lpObj) // OK
{
	PMSG_MASTER_INFO_SEND pMsg;

	pMsg.header.set(0xF3, 0x50, sizeof(pMsg));

	pMsg.MasterLevel = lpObj->MasterLevel;

	pMsg.Experience[0] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->MasterExperience)));
	pMsg.Experience[1] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->MasterExperience)));
	pMsg.Experience[2] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->MasterExperience)));
	pMsg.Experience[3] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->MasterExperience)));
	pMsg.Experience[4] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->MasterExperience)));
	pMsg.Experience[5] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->MasterExperience)));
	pMsg.Experience[6] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->MasterExperience)));
	pMsg.Experience[7] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->MasterExperience)));

	pMsg.NextExperience[0] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->MasterNextExperience)));
	pMsg.NextExperience[1] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpObj->MasterNextExperience)));
	pMsg.NextExperience[2] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->MasterNextExperience)));
	pMsg.NextExperience[3] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpObj->MasterNextExperience)));
	pMsg.NextExperience[4] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->MasterNextExperience)));
	pMsg.NextExperience[5] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpObj->MasterNextExperience)));
	pMsg.NextExperience[6] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->MasterNextExperience)));
	pMsg.NextExperience[7] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpObj->MasterNextExperience)));

	pMsg.MasterPoint = lpObj->MasterPoint;

	pMsg.MaxLife = GET_MAX_WORD_VALUE((lpObj->MaxLife + lpObj->AddLife));

	pMsg.MaxMana = GET_MAX_WORD_VALUE((lpObj->MaxMana + lpObj->AddMana));

	pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield + lpObj->m_MPSkillOpt.AddShield));

	pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP + lpObj->AddBP));

	/*#if(GAMESERVER_EXTRA==1)
	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->m_MPSkillOpt.AddShield);
	#endif*/

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void CMasterSkillTree::GCMasterLevelUpSend(LPOBJ lpObj) // OK
{
	PMSG_MASTER_LEVEL_UP_SEND pMsg;

	pMsg.header.set(0xF3, 0x51, sizeof(pMsg));
	pMsg.MasterLevel = lpObj->MasterLevel;
	pMsg.MinMasterLevel = 1;
	pMsg.MasterPoint = lpObj->MasterPoint;
	pMsg.MaxMasterLevel = gServerInfo.m_MasterSkillTreeMaxLevel;
	pMsg.MaxLife = GET_MAX_WORD_VALUE((lpObj->MaxLife + lpObj->AddLife));
	pMsg.MaxMana = GET_MAX_WORD_VALUE((lpObj->MaxMana + lpObj->AddMana));
	pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield + lpObj->m_MPSkillOpt.AddShield));
	pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP + lpObj->AddBP));

	pMsg.Level = lpObj->Level;
	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->m_MPSkillOpt.AddShield);
	pMsg.ViewMasterExperience = lpObj->MasterExperience;
	pMsg.ViewMasterNextExperience = lpObj->MasterNextExperience;

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

	//GCEffectInfoSend(lpObj->Index, 16);
}

void CMasterSkillTree::GCMasterSkillSend(int aIndex, BYTE type, BYTE flag, int MasterPoint, int MasterSkill, int MasterEmpty) // OK
{
	PMSG_MASTER_SKILL_SEND pMsg;

	pMsg.header.set(0xF3, 0x52, sizeof(pMsg));

	pMsg.type = type;

	pMsg.MasterPoint = MasterPoint;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CMasterSkillTree::GCMasterSkillListSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[2048];

	PMSG_MASTER_SKILL_LIST_SEND pMsg;

	pMsg.header.set(0xF3, 0x53, 0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_MASTER_SKILL_LIST info;

	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() != 0)
		{
			MASTER_SKILL_TREE_INFO MasterSkillTreeInfo;

			if (this->GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				info.skill = MasterSkillTreeInfo.RequireClass[lpObj->Class];

				info.level = lpObj->MasterSkill[n].m_level + 1;

				info.MainValue = this->GetMasterSkillValue(MasterSkillTreeInfo.Index, (((lpObj->MasterSkill[n].m_level + 0) >= MasterSkillTreeInfo.MaxLevel) ? (MasterSkillTreeInfo.MaxLevel - 1) : (lpObj->MasterSkill[n].m_level + 0)));

				info.NextValue = this->GetMasterSkillValue(MasterSkillTreeInfo.Index, (((lpObj->MasterSkill[n].m_level + 1) >= MasterSkillTreeInfo.MaxLevel) ? (MasterSkillTreeInfo.MaxLevel - 1) : (lpObj->MasterSkill[n].m_level + 1)));

				memcpy(&send[size], &info, sizeof(info));
				size += sizeof(info);

				pMsg.count++;
			}
		}
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(lpObj->Index, send, size);
}

void CMasterSkillTree::DGMasterSkillTreeRecv(SDHP_MASTER_SKILL_TREE_RECV* lpMsg) // OK
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGMasterSkillTreeRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if (lpObj->LoadMasterLevel != 0)
	{
		return;
	}

	lpObj->LoadMasterLevel = 1;

	lpObj->MasterLevel = lpMsg->MasterLevel;

	lpObj->MasterPoint = lpMsg->MasterPoint;

	lpObj->MasterExperience = lpMsg->MasterExperience;

	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		CSkill skill;

		lpObj->MasterSkill[n].Clear();

		if (gSkillManager.ConvertSkillByte(&skill, lpMsg->MasterSkill[n]) != 0) { gSkillManager.AddMasterSkill(lpObj, skill.m_index, skill.m_level); }
	}

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	this->GCMasterInfoSend(lpObj);

	this->GCMasterSkillListSend(lpObj->Index);
}

void CMasterSkillTree::GDMasterSkillTreeSend(int aIndex) // OK
{
	if (gObjIsAccountValid(aIndex, gObj[aIndex].Account) == 0)
	{
		return;
	}

	if (gObj[aIndex].LoadMasterLevel != 0)
	{
		return;
	}

	SDHP_MASTER_SKILL_TREE_SEND pMsg;

	pMsg.header.set(0x0D, 0x00, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, gObj[aIndex].Account, sizeof(pMsg.account));

	memcpy(pMsg.name, gObj[aIndex].Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CMasterSkillTree::GDMasterSkillTreeSaveSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->LoadMasterLevel == 0)
	{
		return;
	}

	SDHP_MASTER_SKILL_TREE_SAVE_SEND pMsg;

	pMsg.header.set(0x0D, 0x30, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	pMsg.MasterLevel = lpObj->MasterLevel;

	pMsg.MasterPoint = lpObj->MasterPoint;

	pMsg.MasterExperience = lpObj->MasterExperience;

	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++) { gSkillManager.SkillByteConvert(pMsg.MasterSkill[n], &lpObj->MasterSkill[n]); }

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

void CMasterSkillTree::GetMasterSkillTreeDel(LPOBJ lpObj, int ItemIndex, int slot)
{
	MASTER_SKILL_TREE_INFO MasterSkillTreeInfo;

	if (ItemIndex == GET_ITEM(13, 152))
	{
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			if (this->GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				if (MasterSkillTreeInfo.Group == 1)
				{
					for (int i = 0; i < MAX_SKILL_LIST; i++)
					{
						if (lpObj->Skill[i].m_skill == lpObj->MasterSkill[n].m_skill)
						{
							lpObj->Skill[i].m_index = lpObj->Skill[i].m_skill;
						}
					}
					lpObj->MasterPoint += lpObj->MasterSkill[n].m_level + 1;
					lpObj->MasterSkill[n].Clear();
				}
			}
		}
	}
	else if (ItemIndex == GET_ITEM(13, 153))
	{
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			if (this->GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				if (MasterSkillTreeInfo.Group == 2)
				{
					for (int i = 0; i < MAX_SKILL_LIST; i++)
					{
						if (lpObj->Skill[i].m_skill == lpObj->MasterSkill[n].m_skill)
						{
							lpObj->Skill[i].m_index = lpObj->Skill[i].m_skill;
						}
					}
					lpObj->MasterPoint += lpObj->MasterSkill[n].m_level + 1;
					lpObj->MasterSkill[n].Clear();
				}
			}
		}
	}
	else if (ItemIndex == GET_ITEM(13, 154))
	{
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			if (this->GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				if (MasterSkillTreeInfo.Group == 3)
				{
					for (int i = 0; i < MAX_SKILL_LIST; i++)
					{
						if (lpObj->Skill[i].m_skill == lpObj->MasterSkill[n].m_skill)
						{
							lpObj->Skill[i].m_index = lpObj->Skill[i].m_skill;
						}
					}
					lpObj->MasterPoint += lpObj->MasterSkill[n].m_level + 1;
					lpObj->MasterSkill[n].Clear();
				}
			}
		}
	}
	else if (ItemIndex == GET_ITEM(13, 155))
	{
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			if (this->GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				for (int i = 0; i < MAX_SKILL_LIST; i++)
				{
					if (lpObj->Skill[i].m_skill == lpObj->MasterSkill[n].m_skill)
					{
						lpObj->Skill[i].m_index = lpObj->Skill[i].m_skill;
					}
				}
				lpObj->MasterPoint += lpObj->MasterSkill[n].m_level + 1;
				lpObj->MasterSkill[n].Clear();
			}
		}
	}

	gItemManager.InventoryDelItem(lpObj->Index, slot);

	gItemManager.GCItemDeleteSend(lpObj->Index, slot, 1);
	GDCharacterInfoSaveSend(lpObj->Index);
	//GCCloseClientSend(lpObj->Index, 2);
	gObjectManager.CharacterGameCloseSet(lpObj->Index, 1);
}

int CMasterSkillTree::GetMasterSkillTreeTotalPoints(LPOBJ lpObj)
{
	MASTER_SKILL_TREE_INFO MasterSkillTreeInfo;

	int points = 0;

	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (this->GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
		{
			points += lpObj->MasterSkill[n].m_level + 1;
		}
	}

	return points;
}

