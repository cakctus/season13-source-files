#include "stdafx.h"
#include "User.h"
#include "DefaultClassInfo.h"
#include "StatSpecialize.h"
#include "GameMain.h"
#include "MemScript.h"
#include "Util.h"

CStatSpecialize g_StatSpec;

CStatSpecialize::CStatSpecialize(void)
{
	memset(this->m_StatOptions, 0x00, sizeof(this->m_StatOptions));
}

CStatSpecialize::~CStatSpecialize(void)
{
}

void CStatSpecialize::Load(char* path) // OK
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

	int cout = 0;
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

			this->m_StatOptions[cout].StatOptionID = lpMemScript->GetNumber();

			this->m_StatOptions[cout].StatOptionClass[CLASS_DW] = lpMemScript->GetAsNumber();
			this->m_StatOptions[cout].StatOptionClass[CLASS_DK] = lpMemScript->GetAsNumber();
			this->m_StatOptions[cout].StatOptionClass[CLASS_FE] = lpMemScript->GetAsNumber();
			this->m_StatOptions[cout].StatOptionClass[CLASS_MG] = lpMemScript->GetAsNumber();
			this->m_StatOptions[cout].StatOptionClass[CLASS_DL] = lpMemScript->GetAsNumber();
			this->m_StatOptions[cout].StatOptionClass[CLASS_SU] = lpMemScript->GetAsNumber();
			this->m_StatOptions[cout].StatOptionClass[CLASS_RF] = lpMemScript->GetAsNumber();
			this->m_StatOptions[cout].StatOptionClass[CLASS_GL] = lpMemScript->GetAsNumber();

			cout++;
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

	this->m_bFileLoad = true;
}

void CStatSpecialize::ClearUserOption(LPOBJ lpObj)
{
	for (int i = 0; i < MAX_USER_STAT_OPTIONS; i++)
	{
		lpObj->m_StatSpecOption[i].Clear();
	}
}

void CStatSpecialize::CalcStatOption(LPOBJ lpObj, int StatOptionID)
{
	return;

	if (this->m_bFileLoad == false)
	{
		return;
	}

	double Percent = 0.0;

	Percent = this->GetPercent(lpObj, StatOptionID);

	if (Percent == 0.0)
	{
		return;
	}

	this->SetStatOption(lpObj, StatOptionID, Percent);
}

void CStatSpecialize::SetStatOption(LPOBJ lpObj, int StatOptionID, double StatOptionPercent)
{
	return;

	int ArrayIndex = this->GetUserArrayIndex(lpObj, StatOptionID);

	if (ArrayIndex == -1)
	{
		return;
	}

	lpObj->m_StatSpecOption[ArrayIndex].StatOptionID = StatOptionID;

	switch (StatOptionID)
	{
	case STAT_OPTION_INC_ATTACK_POWER:
	{
		int iEffectValueMin = lpObj->PhysiDamageMinLeft * StatOptionPercent / 100.0;
		int iEffectValueMax = lpObj->PhysiDamageMaxLeft * StatOptionPercent / 100.0;
		lpObj->PhysiDamageMinLeft += iEffectValueMin;
		lpObj->PhysiDamageMinRight += iEffectValueMin;
		lpObj->PhysiDamageMaxLeft += iEffectValueMax;
		lpObj->PhysiDamageMaxRight += iEffectValueMax;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValueMin;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMax = iEffectValueMax;
	}
	break;

	case STAT_OPTION_INC_ATTACK_RATE:
	{
		int iEffectValue = lpObj->m_MPSkillOpt.AttackSuccessRate * StatOptionPercent / 100.0;
		lpObj->m_MPSkillOpt.AttackSuccessRate += iEffectValue;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
	}
	break;

	case STAT_OPTION_INC_ATTACK_RATE_PVP:
	{
		int iEffectValue = lpObj->m_MPSkillOpt.AttackSuccessRatePvP * StatOptionPercent / 100.0;
		lpObj->m_MPSkillOpt.AttackSuccessRatePvP += iEffectValue;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
	}
	break;

	case STAT_OPTION_INC_DEFENSE:
	{
		int iEffectValue = lpObj->Defense * StatOptionPercent / 100.0;
		lpObj->Defense += iEffectValue;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
	}
	break;

	case STAT_OPTION_INC_ATTACK_SPEED:
	{
		int iEffectValue = lpObj->PhysiSpeed * StatOptionPercent / 100.0;
		lpObj->PhysiSpeed += iEffectValue;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
	}
	break;

	case STAT_OPTION_INC_DEFENSE_RATE:
	{
		int iEffectValue = lpObj->m_MPSkillOpt.DefenseSuccessRate * StatOptionPercent / 100.0;
		lpObj->m_MPSkillOpt.DefenseSuccessRate += iEffectValue;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
	}
	break;

	case STAT_OPTION_INC_DEFENSE_RATE_PVP:
	{
		int iEffectValue = lpObj->m_MPSkillOpt.DefenseSuccessRatePvP * StatOptionPercent / 100.0;
		lpObj->m_MPSkillOpt.DefenseSuccessRatePvP += iEffectValue;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
	}
	break;

	case STAT_OPTION_INC_MAGIC_DAMAGE:
	{
		int iEffectValueMin = lpObj->MagicDamageMin * StatOptionPercent / 100.0;
		int iEffectValueMax = lpObj->MagicDamageMax * StatOptionPercent / 100.0;
		lpObj->MagicDamageMin += iEffectValueMin;
		lpObj->MagicDamageMax += iEffectValueMax;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValueMin;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMax = iEffectValueMax;
	}
	break;

	case STAT_OPTION_INC_CURSE_DAMAGE:
	{
		int iEffectValueMin = lpObj->CurseDamageMin * StatOptionPercent / 100.0;
		int iEffectValueMax = lpObj->CurseDamageMax * StatOptionPercent / 100.0;
		lpObj->CurseDamageMin += iEffectValueMin;
		lpObj->CurseDamageMax += iEffectValueMax;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValueMin;
		lpObj->m_StatSpecOption[ArrayIndex].StatOptionValMax = iEffectValueMax;
	}
	break;
	}
}

int CStatSpecialize::GetUserArrayIndex(LPOBJ lpObj, int StatOptionID)
{
	int ArrayIndex = -1;

	for (int i = 0; i < MAX_USER_STAT_OPTIONS; i++)
	{
		if (lpObj->m_StatSpecOption[i].StatOptionID == 0)
		{
			ArrayIndex = i;
			break;
		}
		if (lpObj->m_StatSpecOption[i].StatOptionID == StatOptionID)
		{
			ArrayIndex = i;
			break;
		}
	}

	return ArrayIndex;
}

void CStatSpecialize::SendOptionList(OBJECTSTRUCT* lpObj)
{
	PMSG_STAT_SPEC_OPTION pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x59, 0x00, sizeof(pMsg));
	int OptionCount = 0;

	for (int i = 0; i < MAX_USER_STAT_OPTIONS; i++)
	{
		if (lpObj->m_StatSpecOption[i].StatOptionID != 0)
		{
			pMsg.m_StatOption[OptionCount].StatOptionID = lpObj->m_StatSpecOption[i].StatOptionID;
			pMsg.m_StatOption[OptionCount].StatOptionValMin = lpObj->m_StatSpecOption[i].StatOptionValMin;
			pMsg.m_StatOption[OptionCount].StatOptionValMax = lpObj->m_StatSpecOption[i].StatOptionValMax;
			OptionCount++;
		}
	}

	DataSend(lpObj->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void CStatSpecialize::ReadCharacterSpecialize(char* path)
{
	this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMinPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMinPVP1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMaxPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMaxPVP1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityLevelPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityLevelPVP1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityValPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityValPVP1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMinPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMinPVP2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMaxPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMaxPVP2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityLevelPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityLevelPVP2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityValPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityValPVP2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMinPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMinPVP3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMaxPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMaxPVP3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityLevelPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityLevelPVP3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityValPVP = GetPrivateProfileInt("CLASS_DW", "AtkAgilityValPVP3", 0, path);

	this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMinPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMinPVM1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMaxPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMaxPVM1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityLevelPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityLevelPVM1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityValPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityValPVM1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMinPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMinPVM2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMaxPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMaxPVM2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityLevelPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityLevelPVM2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityValPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityValPVM2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMinPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMinPVM3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMaxPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityMaxPVM3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityLevelPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityLevelPVM3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityValPVM = GetPrivateProfileInt("CLASS_DW", "AtkAgilityValPVM3", 0, path);

	this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_DW", "MagicEnergyMin1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_DW", "MagicEnergyMax1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_DW", "MagicEnergyLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_DW", "MagicEnergyVal1", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_DW", "MagicEnergyMin2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_DW", "MagicEnergyMax2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_DW", "MagicEnergyLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_DW", "MagicEnergyVal2", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_DW", "MagicEnergyMin3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_DW", "MagicEnergyMax3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_DW", "MagicEnergyLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_DW", "MagicEnergyVal3", 0, path);

	this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_DK", "AtkStrengthMin1", 0, path);
	this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_DK", "AtkStrengthMax1", 0, path);
	this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_DK", "AtkStrengthLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_DK", "AtkStrengthVal1", 0, path);
	this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_DK", "AtkStrengthMin2", 0, path);
	this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_DK", "AtkStrengthMax2", 0, path);
	this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_DK", "AtkStrengthLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_DK", "AtkStrengthVal2", 0, path);
	this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_DK", "AtkStrengthMin3", 0, path);
	this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_DK", "AtkStrengthMax3", 0, path);
	this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_DK", "AtkStrengthLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_DK", "AtkStrengthVal3", 0, path);

	this->m_StatCharacterOption[CLASS_DK][0].m_DefAgilityMin = GetPrivateProfileInt("CLASS_DK", "DefAgilityMin1", 0, path);
	this->m_StatCharacterOption[CLASS_DK][0].m_DefAgilityMax = GetPrivateProfileInt("CLASS_DK", "DefAgilityMax1", 0, path);
	this->m_StatCharacterOption[CLASS_DK][0].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_DK", "DefAgilityLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_DK][0].m_DefAgilityVal = GetPrivateProfileInt("CLASS_DK", "DefAgilityVal1", 0, path);
	this->m_StatCharacterOption[CLASS_DK][1].m_DefAgilityMin = GetPrivateProfileInt("CLASS_DK", "DefAgilityMin2", 0, path);
	this->m_StatCharacterOption[CLASS_DK][1].m_DefAgilityMax = GetPrivateProfileInt("CLASS_DK", "DefAgilityMax2", 0, path);
	this->m_StatCharacterOption[CLASS_DK][1].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_DK", "DefAgilityLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_DK][1].m_DefAgilityVal = GetPrivateProfileInt("CLASS_DK", "DefAgilityVal2", 0, path);
	this->m_StatCharacterOption[CLASS_DK][2].m_DefAgilityMin = GetPrivateProfileInt("CLASS_DK", "DefAgilityMin3", 0, path);
	this->m_StatCharacterOption[CLASS_DK][2].m_DefAgilityMax = GetPrivateProfileInt("CLASS_DK", "DefAgilityMax3", 0, path);
	this->m_StatCharacterOption[CLASS_DK][2].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_DK", "DefAgilityLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_DK][2].m_DefAgilityVal = GetPrivateProfileInt("CLASS_DK", "DefAgilityVal3", 0, path);

	this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityMin = GetPrivateProfileInt("CLASS_FE", "AtkAgilityMin1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityMax = GetPrivateProfileInt("CLASS_FE", "AtkAgilityMax1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityLevel = GetPrivateProfileInt("CLASS_FE", "AtkAgilityLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityVal = GetPrivateProfileInt("CLASS_FE", "AtkAgilityVal1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityMin = GetPrivateProfileInt("CLASS_FE", "AtkAgilityMin2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityMax = GetPrivateProfileInt("CLASS_FE", "AtkAgilityMax2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityLevel = GetPrivateProfileInt("CLASS_FE", "AtkAgilityLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityVal = GetPrivateProfileInt("CLASS_FE", "AtkAgilityVal2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityMin = GetPrivateProfileInt("CLASS_FE", "AtkAgilityMin3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityMax = GetPrivateProfileInt("CLASS_FE", "AtkAgilityMax3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityLevel = GetPrivateProfileInt("CLASS_FE", "AtkAgilityLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityVal = GetPrivateProfileInt("CLASS_FE", "AtkAgilityVal3", 0, path);

	this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMinPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityMinPVP1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMaxPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityMaxPVP1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityLevelPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityLevelPVP1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityValPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityValPVP1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMinPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityMinPVP2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMaxPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityMaxPVP2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityLevelPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityLevelPVP2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityValPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityValPVP2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMinPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityMinPVP3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMaxPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityMaxPVP3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityLevelPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityLevelPVP3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityValPVP = GetPrivateProfileInt("CLASS_FE", "DefAgilityValPVP3", 0, path);

	this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMinPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityMinPVM1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMaxPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityMaxPVM1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityLevelPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityLevelPVM1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityValPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityValPVM1", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMinPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityMinPVM2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMaxPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityMaxPVM2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityLevelPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityLevelPVM2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityValPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityValPVM2", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMinPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityMinPVM3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMaxPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityMaxPVM3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityLevelPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityLevelPVM3", 0, path);
	this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityValPVM = GetPrivateProfileInt("CLASS_FE", "DefAgilityValPVM3", 0, path);

	this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_MG", "AtkStrengthMin1", 0, path);
	this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_MG", "AtkStrengthMax1", 0, path);
	this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_MG", "AtkStrengthLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_MG", "AtkStrengthVal1", 0, path);
	this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_MG", "AtkStrengthMin2", 0, path);
	this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_MG", "AtkStrengthMax2", 0, path);
	this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_MG", "AtkStrengthLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_MG", "AtkStrengthVal2", 0, path);
	this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_MG", "AtkStrengthMin3", 0, path);
	this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_MG", "AtkStrengthMax3", 0, path);
	this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_MG", "AtkStrengthLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_MG", "AtkStrengthVal3", 0, path);

	this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_MG", "MagicEnergyMin1", 0, path);
	this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_MG", "MagicEnergyMax1", 0, path);
	this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_MG", "MagicEnergyLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_MG", "MagicEnergyVal1", 0, path);
	this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_MG", "MagicEnergyMin2", 0, path);
	this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_MG", "MagicEnergyMax2", 0, path);
	this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_MG", "MagicEnergyLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_MG", "MagicEnergyVal2", 0, path);
	this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_MG", "MagicEnergyMin3", 0, path);
	this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_MG", "MagicEnergyMax3", 0, path);
	this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_MG", "MagicEnergyLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_MG", "MagicEnergyVal3", 0, path);

	this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_DL", "AtkStrengthMin1", 0, path);
	this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_DL", "AtkStrengthMax1", 0, path);
	this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_DL", "AtkStrengthLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_DL", "AtkStrengthVal1", 0, path);
	this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_DL", "AtkStrengthMin2", 0, path);
	this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_DL", "AtkStrengthMax2", 0, path);
	this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_DL", "AtkStrengthLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_DL", "AtkStrengthVal2", 0, path);
	this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_DL", "AtkStrengthMin3", 0, path);
	this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_DL", "AtkStrengthMax3", 0, path);
	this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_DL", "AtkStrengthLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_DL", "AtkStrengthVal3", 0, path);

	this->m_StatCharacterOption[CLASS_DL][0].m_DefAgilityMin = GetPrivateProfileInt("CLASS_DL", "DefAgilityMin1", 0, path);
	this->m_StatCharacterOption[CLASS_DL][0].m_DefAgilityMax = GetPrivateProfileInt("CLASS_DL", "DefAgilityMax1", 0, path);
	this->m_StatCharacterOption[CLASS_DL][0].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_DL", "DefAgilityLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_DL][0].m_DefAgilityVal = GetPrivateProfileInt("CLASS_DL", "DefAgilityVal1", 0, path);
	this->m_StatCharacterOption[CLASS_DL][1].m_DefAgilityMin = GetPrivateProfileInt("CLASS_DL", "DefAgilityMin2", 0, path);
	this->m_StatCharacterOption[CLASS_DL][1].m_DefAgilityMax = GetPrivateProfileInt("CLASS_DL", "DefAgilityMax2", 0, path);
	this->m_StatCharacterOption[CLASS_DL][1].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_DL", "DefAgilityLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_DL][1].m_DefAgilityVal = GetPrivateProfileInt("CLASS_DL", "DefAgilityVal2", 0, path);
	this->m_StatCharacterOption[CLASS_DL][2].m_DefAgilityMin = GetPrivateProfileInt("CLASS_DL", "DefAgilityMin3", 0, path);
	this->m_StatCharacterOption[CLASS_DL][2].m_DefAgilityMax = GetPrivateProfileInt("CLASS_DL", "DefAgilityMax3", 0, path);
	this->m_StatCharacterOption[CLASS_DL][2].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_DL", "DefAgilityLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_DL][2].m_DefAgilityVal = GetPrivateProfileInt("CLASS_DL", "DefAgilityVal3", 0, path);

	this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_SU", "MagicEnergyMin1", 0, path);
	this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_SU", "MagicEnergyMax1", 0, path);
	this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_SU", "MagicEnergyLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_SU", "MagicEnergyVal1", 0, path);
	this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_SU", "MagicEnergyMin2", 0, path);
	this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_SU", "MagicEnergyMax2", 0, path);
	this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_SU", "MagicEnergyLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_SU", "MagicEnergyVal2", 0, path);
	this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyMin = GetPrivateProfileInt("CLASS_SU", "MagicEnergyMin3", 0, path);
	this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyMax = GetPrivateProfileInt("CLASS_SU", "MagicEnergyMax3", 0, path);
	this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyLevel = GetPrivateProfileInt("CLASS_SU", "MagicEnergyLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyVal = GetPrivateProfileInt("CLASS_SU", "MagicEnergyVal3", 0, path);

	this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyMin = GetPrivateProfileInt("CLASS_SU", "CurseEnergyMin1", 0, path);
	this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyMax = GetPrivateProfileInt("CLASS_SU", "CurseEnergyMax1", 0, path);
	this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyLevel = GetPrivateProfileInt("CLASS_SU", "CurseEnergyLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyVal = GetPrivateProfileInt("CLASS_SU", "CurseEnergyVal1", 0, path);
	this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyMin = GetPrivateProfileInt("CLASS_SU", "CurseEnergyMin2", 0, path);
	this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyMax = GetPrivateProfileInt("CLASS_SU", "CurseEnergyMax2", 0, path);
	this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyLevel = GetPrivateProfileInt("CLASS_SU", "CurseEnergyLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyVal = GetPrivateProfileInt("CLASS_SU", "CurseEnergyVal2", 0, path);
	this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyMin = GetPrivateProfileInt("CLASS_SU", "CurseEnergyMin3", 0, path);
	this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyMax = GetPrivateProfileInt("CLASS_SU", "CurseEnergyMax3", 0, path);
	this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyLevel = GetPrivateProfileInt("CLASS_SU", "CurseEnergyLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyVal = GetPrivateProfileInt("CLASS_SU", "CurseEnergyVal3", 0, path);

	this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_RF", "AtkStrengthMin1", 0, path);
	this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_RF", "AtkStrengthMax1", 0, path);
	this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_RF", "AtkStrengthLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_RF", "AtkStrengthVal1", 0, path);
	this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_RF", "AtkStrengthMin2", 0, path);
	this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_RF", "AtkStrengthMax2", 0, path);
	this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_RF", "AtkStrengthLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_RF", "AtkStrengthVal2", 0, path);
	this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_RF", "AtkStrengthMin3", 0, path);
	this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_RF", "AtkStrengthMax3", 0, path);
	this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_RF", "AtkStrengthLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_RF", "AtkStrengthVal3", 0, path);

	this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_GL", "AtkStrengthMin1", 0, path);
	this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_GL", "AtkStrengthMax1", 0, path);
	this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_GL", "AtkStrengthLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_GL", "AtkStrengthVal1", 0, path);
	this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_GL", "AtkStrengthMin2", 0, path);
	this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_GL", "AtkStrengthMax2", 0, path);
	this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_GL", "AtkStrengthLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_GL", "AtkStrengthVal2", 0, path);
	this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthMin = GetPrivateProfileInt("CLASS_GL", "AtkStrengthMin3", 0, path);
	this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthMax = GetPrivateProfileInt("CLASS_GL", "AtkStrengthMax3", 0, path);
	this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthLevel = GetPrivateProfileInt("CLASS_GL", "AtkStrengthLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthVal = GetPrivateProfileInt("CLASS_GL", "AtkStrengthVal3", 0, path);

	this->m_StatCharacterOption[CLASS_GL][0].m_DefAgilityMin = GetPrivateProfileInt("CLASS_GL", "DefAgilityMin1", 0, path);
	this->m_StatCharacterOption[CLASS_GL][0].m_DefAgilityMax = GetPrivateProfileInt("CLASS_GL", "DefAgilityMax1", 0, path);
	this->m_StatCharacterOption[CLASS_GL][0].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_GL", "DefAgilityLevel1", 0, path);
	this->m_StatCharacterOption[CLASS_GL][0].m_DefAgilityVal = GetPrivateProfileInt("CLASS_GL", "DefAgilityVal1", 0, path);
	this->m_StatCharacterOption[CLASS_GL][1].m_DefAgilityMin = GetPrivateProfileInt("CLASS_GL", "DefAgilityMin2", 0, path);
	this->m_StatCharacterOption[CLASS_GL][1].m_DefAgilityMax = GetPrivateProfileInt("CLASS_GL", "DefAgilityMax2", 0, path);
	this->m_StatCharacterOption[CLASS_GL][1].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_GL", "DefAgilityLevel2", 0, path);
	this->m_StatCharacterOption[CLASS_GL][1].m_DefAgilityVal = GetPrivateProfileInt("CLASS_GL", "DefAgilityVal2", 0, path);
	this->m_StatCharacterOption[CLASS_GL][2].m_DefAgilityMin = GetPrivateProfileInt("CLASS_GL", "DefAgilityMin3", 0, path);
	this->m_StatCharacterOption[CLASS_GL][2].m_DefAgilityMax = GetPrivateProfileInt("CLASS_GL", "DefAgilityMax3", 0, path);
	this->m_StatCharacterOption[CLASS_GL][2].m_DefAgilityLevel = GetPrivateProfileInt("CLASS_GL", "DefAgilityLevel3", 0, path);
	this->m_StatCharacterOption[CLASS_GL][2].m_DefAgilityVal = GetPrivateProfileInt("CLASS_GL", "DefAgilityVal3", 0, path);
}

double CStatSpecialize::StatSpec_AtkPowerInc_Knight(int Strength)
{
	double Percent = 0;

	if (Strength > this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthMin && Strength <= this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_DK][0].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthMin && Strength <= this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_DK][1].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthMin && Strength <= this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_DK][2].m_AtkStrengthLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_DefenseInc_Knight(int Agility)
{
	double Percent = 0;

	if (Agility > m_StatCharacterOption[CLASS_DK][0].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_DK][0].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DK][0].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_DK][0].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_DK][0].m_DefAgilityLevel;
	}
	else if (Agility > m_StatCharacterOption[CLASS_DK][1].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_DK][1].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DK][1].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_DK][1].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_DK][1].m_DefAgilityLevel;
	}
	else if (Agility > m_StatCharacterOption[CLASS_DK][2].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_DK][2].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DK][2].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_DK][2].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_DK][2].m_DefAgilityLevel;
	}
	return Percent;
}

double CStatSpecialize::StatSpec_MagicPowerInc_Wizard(int Energy)
{
	double Percent = 0;

	if (Energy > this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_DW][0].m_MagicEnergyLevel;
	}
	else if (Energy > this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_DW][1].m_MagicEnergyLevel;
	}
	else if (Energy > this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_DW][2].m_MagicEnergyLevel;
	}
	return Percent;

}

double CStatSpecialize::StatSpec_AtkRatePvMInc_Wizard(int Agility)
{
	double Percent = 0;

	if (Agility > this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMinPVM&& Agility <= this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMaxPVM)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityValPVM + (Agility - this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMinPVM) / this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityLevelPVM;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMinPVM&& Agility <= this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMaxPVM)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityValPVM + (Agility - this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMinPVM) / this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityLevelPVM;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMinPVM&& Agility <= this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMaxPVM)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityValPVM + (Agility - this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMinPVM) / this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityLevelPVM;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_AtkRatePvPInc_Wizard(int Agility)
{
	double Percent = 0;

	if (Agility > this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMinPVP&& Agility <= this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMaxPVP)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityValPVP + (Agility - this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityMinPVP) / this->m_StatCharacterOption[CLASS_DW][0].m_AtkAgilityLevelPVP;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMinPVP&& Agility <= this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMaxPVP)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityValPVP + (Agility - this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityMinPVP) / this->m_StatCharacterOption[CLASS_DW][1].m_AtkAgilityLevelPVP;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMinPVP&& Agility <= this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMaxPVP)
	{
		Percent = this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityValPVP + (Agility - this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityMinPVP) / this->m_StatCharacterOption[CLASS_DW][2].m_AtkAgilityLevelPVP;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_AtkPowerInc_Elf(int Agility)
{
	double Percent = 0;

	if (Agility > this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityVal + (Agility - this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityMin) / this->m_StatCharacterOption[CLASS_FE][0].m_AtkAgilityLevel;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityVal + (Agility - this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityMin) / this->m_StatCharacterOption[CLASS_FE][1].m_AtkAgilityLevel;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityVal + (Agility - this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityMin) / this->m_StatCharacterOption[CLASS_FE][2].m_AtkAgilityLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_DefRatePvMInc_Elf(int Agility)
{
	double Percent = 0;

	if (Agility > this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMinPVM&& Agility <= this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMaxPVM)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityValPVM + (Agility - this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMinPVM) / this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityLevelPVM;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMinPVM&& Agility <= this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMaxPVM)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityValPVM + (Agility - this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMinPVM) / this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityLevelPVM;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMinPVM&& Agility <= this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMaxPVM)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityValPVM + (Agility - this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMinPVM) / this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityLevelPVM;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_DefRatePvPInc_Elf(int Agility)
{
	double Percent = 0;

	if (Agility > this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMinPVP&& Agility <= this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMaxPVP)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityValPVP + (Agility - this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityMinPVP) / this->m_StatCharacterOption[CLASS_FE][0].m_DefAgilityLevelPVP;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMinPVP&& Agility <= this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMaxPVP)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityValPVP + (Agility - this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityMinPVP) / this->m_StatCharacterOption[CLASS_FE][1].m_DefAgilityLevelPVP;
	}
	else if (Agility > this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMinPVP&& Agility <= this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMaxPVP)
	{
		Percent = this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityValPVP + (Agility - this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityMinPVP) / this->m_StatCharacterOption[CLASS_FE][2].m_DefAgilityLevelPVP;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_MagicPowerInc_Summoner(int Energy)
{
	double Percent = 0;

	if (Energy > this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_SU][0].m_MagicEnergyLevel;
	}
	else if (Energy > this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_SU][1].m_MagicEnergyLevel;
	}
	else if (Energy > this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_SU][2].m_MagicEnergyLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_CursePowerInc_Summoner(int Energy)
{
	double Percent = 0;

	if (Energy > this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyMin) / this->m_StatCharacterOption[CLASS_SU][0].m_CurseEnergyLevel;
	}
	else if (Energy > this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyMin) / this->m_StatCharacterOption[CLASS_SU][1].m_CurseEnergyLevel;
	}
	else if (Energy > this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyMin) / this->m_StatCharacterOption[CLASS_SU][2].m_CurseEnergyLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_AtkPowerInc_Gladiator(int Strength)
{
	double Percent = 0;

	if (Strength > this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_MG][0].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_MG][1].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_MG][2].m_AtkStrengthLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_MagicPowerInc_Gladiator(int Energy)
{
	double Percent = 0;
	if (Energy > this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_MG][0].m_MagicEnergyLevel;
	}
	else if (Energy > this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_MG][1].m_MagicEnergyLevel;
	}
	else if (Energy > this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyMin&& Energy <= this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyVal + (Energy - this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyMin) / this->m_StatCharacterOption[CLASS_MG][2].m_MagicEnergyLevel;
	}
	return Percent;
}

double CStatSpecialize::StatSpec_AtkPowerInc_Lord(int Strength)
{
	double Percent = 0;

	if (Strength > this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_DL][0].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_DL][1].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_DL][2].m_AtkStrengthLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_DefenseInc_Lord(int Agility)
{
	double Percent = 0;
	if (Agility > m_StatCharacterOption[CLASS_DL][0].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_DL][0].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DL][0].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_DL][0].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_DL][0].m_DefAgilityLevel;
	}
	else if (Agility > m_StatCharacterOption[CLASS_DL][1].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_DL][1].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DL][1].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_DL][1].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_DL][1].m_DefAgilityLevel;
	}
	else if (Agility > m_StatCharacterOption[CLASS_DL][2].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_DL][2].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_DL][2].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_DL][2].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_DL][2].m_DefAgilityLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_AtkPowerInc_Fighter(int Strength)
{
	double Percent = 0;

	if (Strength > this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthMin && Strength <= this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_RF][0].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthMin && Strength <= this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_RF][1].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthMin && Strength <= this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_RF][2].m_AtkStrengthLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_AtkPowerInc_Lancer(int Strength)
{
	double Percent = 0;

	if (Strength > this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_GL][0].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_GL][1].m_AtkStrengthLevel;
	}
	else if (Strength > this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthMin&& Strength <= this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthVal + (Strength - this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthMin) / this->m_StatCharacterOption[CLASS_GL][2].m_AtkStrengthLevel;
	}

	return Percent;
}

double CStatSpecialize::StatSpec_DefenseInc_Lancer(int Agility)
{
	double Percent = 0;

	if (Agility > m_StatCharacterOption[CLASS_GL][0].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_GL][0].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_GL][0].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_GL][0].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_GL][0].m_DefAgilityLevel;
	}
	else if (Agility > m_StatCharacterOption[CLASS_GL][1].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_GL][1].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_GL][1].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_GL][1].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_GL][1].m_DefAgilityLevel;
	}
	else if (Agility > m_StatCharacterOption[CLASS_GL][2].m_DefAgilityMin&& Agility <= this->m_StatCharacterOption[CLASS_GL][2].m_DefAgilityMax)
	{
		Percent = this->m_StatCharacterOption[CLASS_GL][2].m_DefAgilityVal + (Agility - m_StatCharacterOption[CLASS_GL][2].m_DefAgilityMin) / this->m_StatCharacterOption[CLASS_GL][2].m_DefAgilityLevel;
	}
	return Percent;
}

double CStatSpecialize::GetPercent(LPOBJ lpObj, int StatOptionID)
{
	double Percent = 0;

	if (lpObj->Class == CLASS_DW && StatOptionID == STAT_OPTION_INC_ATTACK_RATE)
	{
		Percent = this->StatSpec_AtkRatePvMInc_Wizard(lpObj->Dexterity);
	}
	else if (lpObj->Class == CLASS_DW && StatOptionID == STAT_OPTION_INC_ATTACK_RATE_PVP)
	{
		Percent = this->StatSpec_AtkRatePvPInc_Wizard(lpObj->Dexterity);
	}
	else if (lpObj->Class == CLASS_DW && StatOptionID == STAT_OPTION_INC_MAGIC_DAMAGE)
	{
		Percent = this->StatSpec_MagicPowerInc_Wizard(lpObj->Energy);
	}
	else if (lpObj->Class == CLASS_DK && StatOptionID == STAT_OPTION_INC_ATTACK_POWER)
	{
		Percent = this->StatSpec_AtkPowerInc_Knight(lpObj->Strength);
	}
	else if (lpObj->Class == CLASS_DK && StatOptionID == STAT_OPTION_INC_DEFENSE)
	{
		Percent = this->StatSpec_DefenseInc_Knight(lpObj->Dexterity);
	}
	else if (lpObj->Class == CLASS_FE && StatOptionID == STAT_OPTION_INC_ATTACK_POWER)
	{
		Percent = this->StatSpec_AtkPowerInc_Elf(lpObj->Dexterity);
	}
	else if (lpObj->Class == CLASS_FE && StatOptionID == STAT_OPTION_INC_DEFENSE_RATE)
	{
		Percent = this->StatSpec_DefRatePvMInc_Elf(lpObj->Dexterity);
	}
	else if (lpObj->Class == CLASS_FE && StatOptionID == STAT_OPTION_INC_DEFENSE_RATE_PVP)
	{
		Percent = this->StatSpec_DefRatePvPInc_Elf(lpObj->Dexterity);
	}
	else if (lpObj->Class == CLASS_MG && StatOptionID == STAT_OPTION_INC_ATTACK_POWER)
	{
		Percent = this->StatSpec_AtkPowerInc_Gladiator(lpObj->Strength);
	}
	else if (lpObj->Class == CLASS_MG && StatOptionID == STAT_OPTION_INC_MAGIC_DAMAGE)
	{
		Percent = this->StatSpec_MagicPowerInc_Gladiator(lpObj->Energy);
	}
	else if (lpObj->Class == CLASS_DL && StatOptionID == STAT_OPTION_INC_ATTACK_POWER)
	{
		Percent = this->StatSpec_AtkPowerInc_Lord(lpObj->Strength);
	}
	else if (lpObj->Class == CLASS_DL && StatOptionID == STAT_OPTION_INC_DEFENSE)
	{
		Percent = this->StatSpec_DefenseInc_Lord(lpObj->Dexterity);
	}
	else if (lpObj->Class == CLASS_SU && StatOptionID == STAT_OPTION_INC_MAGIC_DAMAGE)
	{
		Percent = this->StatSpec_MagicPowerInc_Summoner(lpObj->Energy);
	}
	else if (lpObj->Class == CLASS_SU && StatOptionID == STAT_OPTION_INC_CURSE_DAMAGE)
	{
		Percent = this->StatSpec_CursePowerInc_Summoner(lpObj->Energy);
	}
	else if (lpObj->Class == CLASS_RF && StatOptionID == STAT_OPTION_INC_ATTACK_POWER)
	{
		Percent = this->StatSpec_AtkPowerInc_Fighter(lpObj->Strength);
	}
	else if (lpObj->Class == CLASS_GL && StatOptionID == STAT_OPTION_INC_ATTACK_POWER)
	{
		Percent = this->StatSpec_AtkPowerInc_Lancer(lpObj->Strength);
	}
	else if (lpObj->Class == CLASS_GL && StatOptionID == STAT_OPTION_INC_DEFENSE)
	{
		Percent = this->StatSpec_DefenseInc_Lancer(lpObj->Dexterity);
	}

	return Percent;
}