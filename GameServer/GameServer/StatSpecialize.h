#ifndef STATSPECIALIZE_H
#define STATSPECIALIZE_H
#include "Protocol.h"

#define MAX_STAT_OPTIONS 12
#define MAX_USER_STAT_OPTIONS 5
#define MAX_STAT_LEVLE 3

enum STAT_SPECIALIZE_OPTIONS
{
	STAT_OPTION_INC_ATTACK_POWER = 1,
	STAT_OPTION_INC_ATTACK_RATE = 2,
	STAT_OPTION_INC_ATTACK_RATE_PVP = 3,
	STAT_OPTION_INC_DEFENSE = 4,
	STAT_OPTION_INC_ATTACK_SPEED = 5,
	STAT_OPTION_INC_DEFENSE_RATE = 6,
	STAT_OPTION_INC_DEFENSE_RATE_PVP = 7,
	STAT_OPTION_INC_LIFE = 8,
	STAT_OPTION_INC_MAGIC_DAMAGE = 9,
	STAT_OPTION_INC_CURSE_DAMAGE = 10,
	STAT_OPTION_INC_MANA = 11,
	STAT_OPTION_INC_PET_DAMAGE = 12
};

struct STAT_OPTION
{
	int StatOptionID;
	BYTE StatOptionClass[MAX_CLASS];
};

struct STAT_CHARACTER_OPTION
{
	STAT_CHARACTER_OPTION()
	{
		this->Clear();
	}
	void Clear() 
	{
		this->m_AtkAgilityMin = 0;
		this->m_AtkAgilityMax = 0;
		this->m_AtkAgilityLevel = 0;
		this->m_AtkAgilityVal = 0;
		this->m_AtkAgilityMinPVM = 0;
		this->m_AtkAgilityMaxPVM = 0;
		this->m_AtkAgilityLevelPVM = 0;
		this->m_AtkAgilityValPVM = 0;
		this->m_AtkAgilityMinPVP = 0;
		this->m_AtkAgilityMaxPVP = 0;
		this->m_AtkAgilityLevelPVP = 0;
		this->m_AtkAgilityValPVP = 0;
		this->m_MagicEnergyMin = 0;
		this->m_MagicEnergyMax = 0;
		this->m_MagicEnergyLevel = 0;
		this->m_MagicEnergyVal = 0;
		this->m_AtkStrengthMin = 0;
		this->m_AtkStrengthMax = 0;
		this->m_AtkStrengthLevel = 0;
		this->m_AtkStrengthVal = 0;
		this->m_DefAgilityMin = 0;
		this->m_DefAgilityMax = 0;
		this->m_DefAgilityLevel = 0;
		this->m_DefAgilityVal = 0;
		this->m_DefAgilityMinPVP = 0;
		this->m_DefAgilityMaxPVP = 0;
		this->m_DefAgilityLevelPVP = 0;
		this->m_DefAgilityValPVP = 0;
		this->m_DefAgilityMinPVM = 0;
		this->m_DefAgilityMaxPVM = 0;
		this->m_DefAgilityLevelPVM = 0;
		this->m_DefAgilityValPVM = 0;
		this->m_CurseEnergyMin = 0;
		this->m_CurseEnergyMax = 0;
		this->m_CurseEnergyLevel = 0;
		this->m_CurseEnergyVal = 0;
	}

	int m_AtkAgilityMin;
	int m_AtkAgilityMax;
	int m_AtkAgilityLevel;
	int m_AtkAgilityVal;
	int m_AtkAgilityMinPVM;
	int m_AtkAgilityMaxPVM;
	int m_AtkAgilityLevelPVM;
	int m_AtkAgilityValPVM;
	int m_AtkAgilityMinPVP;
	int	m_AtkAgilityMaxPVP;
	int m_AtkAgilityLevelPVP;
	int m_AtkAgilityValPVP;
	int m_MagicEnergyMin;
	int m_MagicEnergyMax;
	int m_MagicEnergyLevel;
	int m_MagicEnergyVal;
	int m_AtkStrengthMin;
	int m_AtkStrengthMax;
	int m_AtkStrengthLevel;
	int m_AtkStrengthVal;
	int m_DefAgilityMin;
	int m_DefAgilityMax;
	int m_DefAgilityLevel;
	int m_DefAgilityVal;
	int m_DefAgilityMinPVP;
	int m_DefAgilityMaxPVP;
	int m_DefAgilityLevelPVP;
	int m_DefAgilityValPVP;
	int m_DefAgilityMinPVM;
	int m_DefAgilityMaxPVM;
	int m_DefAgilityLevelPVM;
	int m_DefAgilityValPVM;
	int m_CurseEnergyMin;
	int m_CurseEnergyMax;
	int m_CurseEnergyLevel;
	int m_CurseEnergyVal;
};

struct PMSG_STAT_SPEC_OPTION
{
	PSBMSG_HEAD h;
	STAT_USER_OPTION m_StatOption[5];
};

#pragma once
class CStatSpecialize
{
public:
	CStatSpecialize(void);
	virtual ~CStatSpecialize(void);

	void Load(char* path);
	void ReadCharacterSpecialize(char* path);
	void CalcStatOption(LPOBJ lpObj, int StatOptionID);
	void SendOptionList(LPOBJ lpObj);
	void ClearUserOption(LPOBJ lpObj);

private:
	void SetStatOption(LPOBJ lpObj, int StatOptionID, double StatOptionPercent);
	int GetUserArrayIndex(LPOBJ lpObj, int StatOptionID);
	double GetPercent(LPOBJ lpObj, int StatOptionID);
	double StatSpec_AtkPowerInc_Knight(int Strength);
	double StatSpec_DefenseInc_Knight(int Agility);
	double StatSpec_MagicPowerInc_Wizard(int Energy);
	double StatSpec_AtkRatePvMInc_Wizard(int Agility);
	double StatSpec_AtkRatePvPInc_Wizard(int Agility);
	double StatSpec_AtkPowerInc_Elf(int Agility);
	double StatSpec_DefRatePvMInc_Elf(int Agility);
	double StatSpec_DefRatePvPInc_Elf(int Agility);
	double StatSpec_MagicPowerInc_Summoner(int Energy);
	double StatSpec_CursePowerInc_Summoner(int Energy);
	double StatSpec_AtkPowerInc_Gladiator(int Strength);
	double StatSpec_MagicPowerInc_Gladiator(int Energy);
	double StatSpec_AtkPowerInc_Lord(int Strength);
	double StatSpec_DefenseInc_Lord(int Agility);
	double StatSpec_AtkPowerInc_Fighter(int Strength);
	double StatSpec_AtkPowerInc_Lancer(int Strength);
	double StatSpec_DefenseInc_Lancer(int Agility);

	bool m_bFileLoad;

	STAT_OPTION m_StatOptions[MAX_STAT_OPTIONS];

	STAT_CHARACTER_OPTION m_StatCharacterOption[MAX_CLASS][MAX_STAT_LEVLE];
};

extern CStatSpecialize g_StatSpec;

#endif