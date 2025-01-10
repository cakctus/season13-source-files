#pragma once

#include "Item.h"
#include "User.h"
#include "SetItemType.h"
#include "DefaultClassInfo.h"

#define MAX_SET_ITEM_OPTION 254
#define MAX_SET_ITEM_OPTION_TABLE 6
#define MAX_SET_ITEM_FULL_OPTION_TABLE 6

enum eSetItemOption
{
	SET_ITEM_OPTION_ADD_STRENGTH = 0,
	SET_ITEM_OPTION_ADD_DEXTERITY = 1,
	SET_ITEM_OPTION_ADD_ENERGY = 2,
	SET_ITEM_OPTION_ADD_VITALITY = 3,
	SET_ITEM_OPTION_ADD_LEADERSHIP = 4,
	SET_ITEM_OPTION_ADD_MIN_PHYSI_DAMAGE = 5,
	SET_ITEM_OPTION_ADD_MAX_PHYSI_DAMAGE = 6,
	SET_ITEM_OPTION_MUL_MAGIC_DAMAGE = 7,
	SET_ITEM_OPTION_ADD_PHYSI_DAMAGE = 8,
	SET_ITEM_OPTION_MUL_ATTACK_SUCCESS_RATE = 9,
	SET_ITEM_OPTION_ADD_DEFENSE = 10,
	SET_ITEM_OPTION_ADD_MAX_LIFE = 11,
	SET_ITEM_OPTION_ADD_MAX_MANA = 12,
	SET_ITEM_OPTION_ADD_MAX_BP = 13,
	SET_ITEM_OPTION_ADD_BP_RECOVERY = 14,
	SET_ITEM_OPTION_ADD_CRITICAL_DAMAGE_RATE = 15,
	SET_ITEM_OPTION_ADD_CRITICAL_DAMAGE = 16,
	SET_ITEM_OPTION_ADD_EXCELLENT_DAMAGE_RATE = 17,
	SET_ITEM_OPTION_ADD_EXCELLENT_DAMAGE = 18,
	SET_ITEM_OPTION_ADD_SKILL_DAMAGE = 19,
	SET_ITEM_OPTION_ADD_DOUBLE_DAMAGE_RATE = 20,
	SET_ITEM_OPTION_ADD_IGNORE_DEFENSE_RATE = 21,
	SET_ITEM_OPTION_MUL_SHIELD_DEFENSE = 22,
	SET_ITEM_OPTION_MUL_TWO_HAND_PHYSI_DAMAGE = 23,
	SET_ITEM_OPTION_INCREASE_ALL_STATS = 24,
	SET_ITEM_OPTION_INCREASE_BASE_DEFENSE = 25,
	SET_ITEM_OPTION_INCREASE_DAMAGE = 26,
	SET_ITEM_OPTION_INCREASE_MAGIC_DAMAGE = 27,
	SET_ITEM_OPTION_ELF_GREATER_DAMAGE_REINFORCEMENT = 28,
	SET_ITEM_OPTION_ELF_GREATER_DEFENSE_REINFORCEMENT = 29,
	SET_ITEM_OPTION_ELF_BLESS_REINFORCEMENT = 30,
	SET_ITEM_OPTION_INCREASE_TRIPLE_DAMAGE_RATE = 31,

	SET_ITEM_OPTION_AUMENTO_DANO_MAESTRAL = 100,
	SET_ITEM_OPTION_AUMENTO_DANO_MAGICO_MAESTRAL = 101,
	SET_ITEM_OPTION_REDUCAO_DANO_MAESTRAL = 102,
	SET_ITEM_OPTION_AUMENTO_DANO_EXCELENTE_MAESTRAL = 103,
	SET_ITEM_OPTION_AUMENTO_DANO_CRITICO_MAESTRAL = 104,

	SET_ITEM_OPTION_INCREASE_MASTERY_DAMAGE_1 = 120,
	SET_ITEM_OPTION_INCREASE_MASTERY_MAGIC_DAMAGE_1 = 121,
	SET_ITEM_OPTION_INCREASE_MASTERY_BASE_DEFENSE_1 = 122,
	SET_ITEM_OPTION_INCREASE_MASTERY_EXCELLENT_DAMAGE_1 = 123,
	SET_ITEM_OPTION_INCREASE_MASTERY_CRITICAL_DAMAGE_1 = 124,
	SET_ITEM_OPTION_INCREASE_MASTERY_DAMAGE_2 = 125,
	SET_ITEM_OPTION_INCREASE_MASTERY_MAGIC_DAMAGE_2 = 126,
	SET_ITEM_OPTION_INCREASE_MASTERY_BASE_DEFENSE_2 = 127,
	SET_ITEM_OPTION_INCREASE_MASTERY_EXCELLENT_DAMAGE_2 = 128,
	SET_ITEM_OPTION_INCREASE_MASTERY_CRITICAL_DAMAGE_2 = 129,
	SET_ITEM_OPTION_INCREASE_MASTERY_DAMAGE_3 = 130,
	SET_ITEM_OPTION_INCREASE_MASTERY_MAGIC_DAMAGE_3 = 131,
	SET_ITEM_OPTION_INCREASE_MASTERY_BASE_DEFENSE_3 = 132,
	SET_ITEM_OPTION_INCREASE_MASTERY_EXCELLENT_DAMAGE_3 = 133,
	SET_ITEM_OPTION_INCREASE_MASTERY_CRITICAL_DAMAGE_3 = 134,

	SET_ITEM_OPTION_INCREASE_DAMAGE_BY_STRENGTH = 243,
	SET_ITEM_OPTION_INCREASE_DAMAGE_BY_AGILITY = 244,
	SET_ITEM_OPTION_INCREASE_DEFENSE_BY_AGILITY = 245,
	SET_ITEM_OPTION_INCREASE_DEFENSE_BY_VITALITY = 246,
	SET_ITEM_OPTION_INCREASE_DAMAGE_BY_ENERGY = 247,
	SET_ITEM_OPTION_INCREASE_ICE_DAMAGE = 248,
	SET_ITEM_OPTION_INCREASE_POISON_DAMAGE = 249,
	SET_ITEM_OPTION_INCREASE_LIGHTNING_DAMAGE = 250,
	SET_ITEM_OPTION_INCREASE_FIRE_DAMAGE = 251,
	SET_ITEM_OPTION_INCREASE_EARTH_DAMAGE = 252,
	SET_ITEM_OPTION_INCREASE_WIND_DAMAGE = 253,
	SET_ITEM_OPTION_INCREASE_WATER_DAMAGE = 254
};

struct SET_ITEM_OPTION_TABLE
{
	int Index;
	int Value;
};

struct SET_ITEM_OPTION_INFO
{
	int Index;
	char FullName[64];
	char TypeName[64];
	SET_ITEM_OPTION_TABLE OptionTable[MAX_SET_ITEM_OPTION_TABLE][MAX_SET_ITEM_OPTION_I];
	int Ex[4];
	int Flag;
	SET_ITEM_OPTION_TABLE FullOptionTable[MAX_SET_ITEM_FULL_OPTION_TABLE];
	BYTE Class[MAX_CLASS];
};

class CSetItemOption
{
public:
	CSetItemOption();
	virtual ~CSetItemOption();
	void Init();
	void Load(char* path);
	void SetInfo(SET_ITEM_OPTION_INFO info);
	SET_ITEM_OPTION_INFO* GetInfo(int index);
	bool IsSetItem(CItem* lpItem);
	int GetSetItemMaxOptionCount(int index);
	int GetInventorySetItemOptionCount(LPOBJ lpObj,int index);
	void CalcSetItemStat(LPOBJ lpObj);
	void CalcSetItemOption(LPOBJ lpObj,bool flag);
	void InsertOption(LPOBJ lpObj,int index,int value,bool flag);
private:
	SET_ITEM_OPTION_INFO m_SetItemOptionInfo[MAX_SET_ITEM_OPTION];
};

extern CSetItemOption gSetItemOption;