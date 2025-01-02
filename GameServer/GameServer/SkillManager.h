#pragma once

#include "DefaultClassInfo.h"
#include "Protocol.h"
#include "User.h"

#define MAX_SKILL 1147
#define MAX_SKILL_LIST 60
#define MAX_MASTER_SKILL_LIST 120

#define CHECK_SKILL_ATTACK_COUNT(x) (((++x)>=10)?0:1)
#define CHECK_SKILL_ATTACK_EXTENDED_COUNT(x) (((++x)>=20)?0:1)

enum eSkillElement
{
	SKILL_ELEMENT_ICE = 0,
	SKILL_ELEMENT_POISON = 1,
	SKILL_ELEMENT_LIGHTNING = 2,
	SKILL_ELEMENT_FIRE = 3,
	SKILL_ELEMENT_EARTH = 4,
	SKILL_ELEMENT_WIND = 5,
	SKILL_ELEMENT_WATER = 6
};

enum eSkillNumber
{
	SKILL_NONE = 0,
	SKILL_POISON = 1,
	SKILL_METEORITE = 2,
	SKILL_LIGHTNING = 3,
	SKILL_FIRE_BALL = 4,
	SKILL_FLAME = 5,
	SKILL_TELEPORT = 6,
	SKILL_ICE = 7,
	SKILL_TWISTER = 8,
	SKILL_EVIL_SPIRIT = 9,
	SKILL_HELL_FIRE = 10,
	SKILL_POWER_WAVE = 11,
	SKILL_AQUA_BEAM = 12,
	SKILL_BLAST = 13,
	SKILL_INFERNO = 14,
	SKILL_TELEPORT_ALLY = 15,
	SKILL_MANA_SHIELD = 16,
	SKILL_ENERGY_BALL = 17,
	SKILL_DEFENSE = 18,
	SKILL_FALLING_SLASH = 19,
	SKILL_LUNGE = 20,
	SKILL_UPPERCUT = 21,
	SKILL_CYCLONE = 22,
	SKILL_SLASH = 23,
	SKILL_TRIPLE_SHOT = 24,
	SKILL_HEAL = 26,
	SKILL_GREATER_DEFENSE = 27,
	SKILL_GREATER_DAMAGE = 28,
	SKILL_SUMMON1 = 30,
	SKILL_SUMMON2 = 31,
	SKILL_SUMMON3 = 32,
	SKILL_SUMMON4 = 33,
	SKILL_SUMMON5 = 34,
	SKILL_SUMMON6 = 35,
	SKILL_SUMMON7 = 36,
	SKILL_DECAY = 38,
	SKILL_ICE_STORM = 39,
	SKILL_NOVA = 40,
	SKILL_TWISTING_SLASH = 41,
	SKILL_RAGEFUL_BLOW = 42,
	SKILL_DEATH_STAB = 43,
	SKILL_CRESCENT_MOON_SLASH = 44,
	SKILL_MANA_GLAIVE = 45,
	SKILL_STAR_FALL = 46,
	SKILL_IMPALE = 47,
	SKILL_GREATER_LIFE = 48,
	SKILL_FIRE_BREATH = 49,
	SKILL_MONSTER_AREA_ATTACK = 50,
	SKILL_ICE_ARROW = 51,
	SKILL_PENETRATION = 52,
	SKILL_FIRE_SLASH = 55,
	SKILL_POWER_SLASH = 56,
	SKILL_SPIRAL_SLASH = 57,
	SKILL_NOVA_START = 58,
	SKILL_COMBO = 59,
	SKILL_FORCE = 60,
	SKILL_FIRE_BURST = 61,
	SKILL_EARTHQUAKE = 62,
	SKILL_SUMMON_PARTY = 63,
	SKILL_GREATER_CRITICAL_DAMAGE = 64,
	SKILL_ELECTRIC_SPARK = 65,
	SKILL_FORCE_WAVE = 66,
	SKILL_STERN = 67,
	SKILL_REMOVE_STERN = 68,
	SKILL_GREATER_MANA = 69,
	SKILL_INVISIBILITY = 70,
	SKILL_REMOVE_INVISIBILITY = 71,
	SKILL_REMOVE_ALL_EFFECT = 72,
	SKILL_MANA_RAYS = 73,
	SKILL_FIRE_BLAST = 74,
	SKILL_BRAND = 75,
	SKILL_PLASMA_STORM = 76,
	SKILL_INFINITY_ARROW = 77,
	SKILL_FIRE_SCREAM = 78,
	SKILL_EXPLOSION = 79,
	SKILL_MONSTER_SUMMON = 200,
	SKILL_MAGIC_DAMAGE_IMMUNITY = 201,
	SKILL_PHYSI_DAMAGE_IMMUNITY = 202,
	SKILL_ORDER_OF_PROTECTION = 210,
	SKILL_ORDER_OF_RESTRAINT = 211,
	SKILL_ORDER_OF_TRACKING = 212,
	SKILL_ORDER_OF_WEAKEN = 213,
	SKILL_DRAIN_LIFE = 214,
	SKILL_CHAIN_LIGHTNING = 215,
	SKILL_ELECTRIC_SURGE = 216,
	SKILL_DAMAGE_REFLECT = 217,
	SKILL_SWORD_POWER = 218,
	SKILL_SLEEP = 219,
	SKILL_LESSER_DAMAGE = 221,
	SKILL_LESSER_DEFENSE = 222,
	SKILL_SAHAMUTT = 223,
	SKILL_NEIL = 224,
	SKILL_GHOST_PHANTOM = 225,
	SKILL_RED_STORM = 230,
	SKILL_FROZEN_STAB = 232,
	SKILL_MAGIC_CIRCLE = 233,
	SKILL_SHIELD_RECOVER = 234,
	SKILL_FIVE_SHOT = 235,
	SKILL_FLAME_STRIKE = 236,
	SKILL_LIGHTNING_STORM = 237,
	SKILL_BIRDS = 238,
	SKILL_SELF_EXPLOSION = 239,
	SKILL_RAKLION_SELUPAN1 = 250,
	SKILL_RAKLION_SELUPAN2 = 251,
	SKILL_RAKLION_SELUPAN3 = 252,
	SKILL_RAKLION_SELUPAN4 = 253,
	SKILL_KILLING_BLOW = 260,
	SKILL_UPPER_BEAST = 261,
	SKILL_CHAIN_DRIVER = 262,
	SKILL_DARK_SIDE = 263,
	SKILL_DRAGON_LORE = 264,
	SKILL_DRAGON_SLAYER = 265,
	SKILL_GREATER_IGNORE_DEFENSE_RATE = 266,
	SKILL_FITNESS = 267,
	SKILL_GREATER_DEFENSE_SUCCESS_RATE = 268,
	SKILL_CHARGE = 269,
	SKILL_PHOENIX_SHOT = 270,
	SKILL_SPIN_STEP = 271,
	SKILL_CIRCLE_SHIELD = 272,
	SKILL_OBSIDIAN = 273,
	SKILL_MAGIC_PIN = 274,
	SKILL_CLASH = 275,
	SKILL_HARSH_STRIKE = 276,
	SKILL_SHINING_PEAK = 277,
	SKILL_WRATH = 278,
	SKILL_BRECHE = 279,
	SKILL_FK_EXPLOSION = 280,
	SKILL_MAGIC_PIN_EXPLOSION = 281,
	SKILL_SPIRIT_HOOK = 282,
	SKILL_BLOOD_STORM = 344,
	SKILL_CURE = 425,
	SKILL_PARTY_HEAL = 426,
	SKILL_POISON_ARROW = 427,
	SKILL_BLESS = 430,
	SKILL_SUMMON_SATYROS = 432,
	SKILL_BLIND = 461,
	SKILL_EARTH_PRISON = 495,
	SKILL_IRON_DEFENSE = 521,
	SKILL_BLOOD_HOWLING = 565,
	SKILL_RUSH = 631,
	SKILL_IRON_DEFENSE1 = 632,
	SKILL_ILLUSION = 642,
	SKILL_SHADOW_STEP = 651,
	SKILL_EVASION = 652,
	SKILL_PARALYZE = 661,
	SKILL_BURST = 700,

	SKILL_FIRE_BLOW = 723,
	SKILL_DW_METEOR_STRIKE = 724,
	SKILL_METEOR_STORM = 725,
	SKILL_SOUL_SEEKER = 726,
	SKILL_FOCUS_SHOT = 727,
	SKILL_FIRE_ARROW = 728,
	SKILL_FIRE_BEAST = 729,
	SKILL_AQUA_BEAST = 730,
	SKILL_ICE_BLOOD = 731,
	SKILL_FIRE_BLOOD = 732,
	SKILL_DARK_BLAST = 733,
	SKILL_MG_METEOR_STRIKE = 734,
	SKILL_FLAME_SPEAR = 735,
	SKILL_FIRE_WAVE = 736,
	SKILL_WIND_SOUL = 737,
	SKILL_VOLCANO = 738,
	SKILL_DARK_PHOENIX_SHOT = 739,
	SKILL_ARCHANGEL_S_WILL = 740,
	SKILL_INCREASE_THE_NUMBER_OF_DAMAGE1 = 1002,
	SKILL_INCREASE_THE_NUMBER_OF_DAMAGE2 = 1003,
	SKILL_INCREASE_THE_NUMBER_OF_DAMAGE3 = 1004,
	SKILL_INCREASE_THE_NUMBER_OF_DAMAGE4 = 1005,
	SKILL_INCREASE_THE_NUMBER_OF_DAMAGE5 = 1006,
	SKILL_INCREASE_THE_CHANCE_OF_ADDITIONAL_DAMAGE = 1007,
	SKILL_INCREASES_ATTACK_SPEED = 1008,
	SKILL_INCREASES_RANGE = 1009,
	SKILL_ADD_SPLASH_DAMAGE = 1010,
	SKILL_INCREASES_DISTANCE = 1011,
	SKILL_TARGET_INCREASE = 1012,
	SKILL_BUFF_SYNERGY = 1013,
	SKILL_BUFF_SYNERGY1 = 1014,
	SKILL_INCREASE_SKILL_DURATION = 1015,
	SKILL_IRON_DEFENSE_FOR = 1016,
	SKILL_REINFORCED_IRON_DEFENSE = 1017,
	SKILL_REDUCE_REUSE_TIME = 1018,
	SKILL_ELIMINATE_REUSE_TIME = 1019,
	SKILL_WEAPON_ATTACK_POWER_INCREASE = 1020,
	SKILL_INCREASED_WEAPON_POWER = 1021,
	SKILL_ADD_PENETRATION_EFFECT = 1022,
	SKILL_ADD_ARROW_PROJECTILE = 1023,
	SKILL_INCREASES_PROBABILITY_OF_POISONING1 = 1024,
	SKILL_INCREASES_PROBABILITY_OF_FREEZING1 = 1025,
	SKILL_INCREASES_PROBABILITY_OF_BLEEDING1 = 1026,
	SKILL_INCREASES_PROBABILITY_OF_SHOCKING1 = 1027,
	SKILL_INCREASES_POISONING_EFFECT1 = 1028,
	SKILL_INCREASES_FREEZING_EFFECT1 = 1029,
	SKILL_INCREASES_BLEEDING_EFFECT1 = 1030,
	SKILL_INCREASES_SHOCKING_EFFECT1 = 1031,
	SKILL_INCREASES_POISONING_DURATION1 = 1032,
	SKILL_INCREASES_FREEZING_DURATION1 = 1033,
	SKILL_INCREASES_BLEEDING_DURATION1 = 1034,
	SKILL_INCREASES_SHOCKING_DURATION1 = 1035,
	SKILL_INCREASES_UPGRADED_POISONING_EFFECT1 = 1036,
	SKILL_INCREASES_UPGRADED_FREEZING_EFFECT1 = 1037,
	SKILL_INCREASES_UPGRADED_BLEEDING_EFFECT1 = 1038,
	SKILL_INCREASES_UPGRADED_SHOCKING_EFFECT1 = 1039,
	SKILL_POISONING_UPGRADE_PROBABILITY_ADDICTION1 = 1040,
	SKILL_FREEZING_UPGRADE_PROBABILITY_FREEZE1 = 1041,
	SKILL_BLEEDING_UPGRADE_PROBABILITY_EXCESSIVE_BLEEDING1 = 1042,
	SKILL_SHOCKING_UPGRADE_PROBABILITY_STUN1 = 1043,
	SKILL_PROBABILITY_TO_INCREASE_MAXIMUM_EFFECT1 = 1044,
	SKILL_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION1 = 1045,
	SKILL_INCREASES_PROBABILITY_OF_POISONING2 = 1046,
	SKILL_INCREASES_PROBABILITY_OF_FREEZING2 = 1047,
	SKILL_INCREASES_PROBABILITY_OF_BLEEDING2 = 1048,
	SKILL_INCREASES_PROBABILITY_OF_SHOCKING2 = 1049,
	SKILL_INCREASES_POISONING_EFFECT2 = 1050,
	SKILL_INCREASES_FREEZING_EFFECT2 = 1051,
	SKILL_INCREASES_BLEEDING_EFFECT2 = 1052,
	SKILL_INCREASES_SHOCKING_EFFECT2 = 1053,
	SKILL_INCREASES_POISONING_DURATION2 = 1054,
	SKILL_INCREASES_FREEZING_DURATION2 = 1055,
	SKILL_INCREASES_BLEEDING_DURATION2 = 1056,
	SKILL_INCREASES_SHOCKING_DURATION2 = 1057,
	SKILL_INCREASES_UPGRADED_POISONING_EFFECT2 = 1058,
	SKILL_INCREASES_UPGRADED_FREEZING_EFFECT2 = 1059,
	SKILL_INCREASES_UPGRADED_BLEEDING_EFFECT2 = 1060,
	SKILL_INCREASES_UPGRADED_SHOCKING_EFFECT2 = 1061,
	SKILL_POISONING_UPGRADE_PROBABILITY_ADDICTION2 = 1062,
	SKILL_FREEZING_UPGRADE_PROBABILITY_FREEZE2 = 1063,
	SKILL_BLEEDING_UPGRADE_PROBABILITY_EXCESSIVE_BLEEDING2 = 1064,
	SKILL_SHOCKING_UPGRADE_PROBABILITY_STUN2 = 1065,
	SKILL_PROBABILITY_TO_INCREASE_MAXIMUM_EFFECT2 = 1066,
	SKILL_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION2 = 1067,
	SKILL_RAGEFUL_BLOW_ENHANCED = 1069,
	SKILL_DEATH_STAB_ENHANCED = 1071,
	SKILL_FIRE_BLOW_ENHANCED = 1072,
	SKILL_METEOR_STRIKE_ENHANCED = 1075,
	SKILL_METEOR_STORM_ENHANCED = 1076,
	SKILL_EVIL_SPIRIT_ENHANCED_DW = 1078,
	SKILL_TRIPLE_SHOT_ENHANCED = 1081,
	SKILL_MULTI_SHOT_ENHANCED = 1083,
	SKILL_FOCUS_SHOT_ENHANCED = 1085,
	SKILL_GIGANTIC_STORM_ENHANCED = 1087,
	SKILL_EVIL_SPIRIT_ENHANCED_MG = 1088,
	SKILL_DARK_BLAST_ENHANCED = 1089,
	SKILL_FIRE_SLASH_ENHANCED = 1092,
	SKILL_FIRE_BLOOD_ENHANCED = 1094,
	SKILL_ICE_BLOOD_ENHANCED = 1095,
	SKILL_FIRE_BURST_ENHANCED = 1096,
	SKILL_CHAOTIC_DISEIER_ENHANCED = 1098,
	SKILL_WIND_SOUL_ENHANCED = 1099,
	SKILL_FIRE_BEAST_ENHANCED = 1102,
	SKILL_LIGHTNING_SHOCK_ENHANCED = 1103,
	SKILL_AQUA_BEAST_ENHANCED = 1105,
	SKILL_DRAGON_ROAR_ENHANCED = 1111,
	SKILL_CHAIN_DRIVE_ENHANCED = 1112,
	SKILL_DARK_SIDE_ENHANCED = 1113,
	SKILL_MAGIC_PIN_ENHANCED = 1117,
	SKILL_BRECHE_ENHANCED = 1118,
	SKILL_SHINING_PEAK_ENHANCED = 1119,
	SKILL_POISONING_STORM = 1125,
	SKILL_FREEZING_SLAUGHTER = 1126,
	SKILL_BLEEDING_HEART = 1127,
	SKILL_SHOCKING_BOOM = 1128,
	SKILL_POISONING_STORM_ENHANCED_DW = 1129,
	SKILL_FREEZING_SLAUGHTER_ENHANCED = 1130,
	SKILL_BLEEDING_HEART_ENHANCED = 1131,
	SKILL_SHOCKING_BOOM_ENHANCED = 1132,
	SKILL_POISONING_STORM_ENHANCED = 1133,
	SKILL_FREEZING_SLAUGHTER_ENHANCED1 = 1134,
	SKILL_BLEEDING_HEART_ENHANCED1 = 1135,
	SKILL_SHOCKING_BOOM_ENHANCED1 = 1136,
};

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_MULTI_SKILL_ATTACK_RECV
{
	PBMSG_HEAD header; // C3:[PROTOCOL_CODE4]
	BYTE skillH;
	BYTE count;
	BYTE skillL;
	BYTE x;
	BYTE serial;
	BYTE y;
};

struct PMSG_MULTI_SKILL_ATTACK
{
	BYTE indexH;
	BYTE MagicKey;
	BYTE indexL;
};

struct PMSG_SKILL_ATTACK_RECV
{
	PBMSG_HEAD header; // C3:19
	BYTE indexH;
	BYTE skillH;
	BYTE indexL;
	BYTE skillL;
	BYTE dis;
};

struct PMSG_SKILL_CANCEL_RECV
{
	PBMSG_HEAD header; // C3:1B
	BYTE skill[2];
	BYTE index[2];
};

struct PMSG_DURATION_SKILL_ATTACK_RECV
{
	PBMSG_HEAD header; // C3:1E
	BYTE x;
	BYTE skillH;
	BYTE y;
	BYTE skillL;
	BYTE dir;
	BYTE indexH;
	BYTE dis;
	BYTE indexL;
	BYTE angle;
	BYTE MagicKey;
};

struct PMSG_RAGE_FIGHTER_SKILL_ATTACK_RECV
{
	PBMSG_HEAD header; // C1:4A
	BYTE indexH;
	BYTE skillH;
	BYTE indexL;
	BYTE skillL;
	BYTE dis;
};

struct PMSG_SKILL_DARK_SIDE_RECV
{
	PBMSG_HEAD header; // C1:4B
	BYTE skill[2];
	BYTE index[2];
};

struct PMSG_SKILL_TELEPORT_ALLY_RECV
{
#pragma pack(1)
	PBMSG_HEAD header; // C3:B0
	WORD index;
	BYTE x;
	BYTE y;
#pragma pack()
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_SKILL_ATTACK_SEND
{
	PBMSG_HEAD header; // C3:19
	BYTE index[2];
	BYTE skill[2];
	BYTE target[2];
};

struct PMSG_SKILL_CANCEL_SEND
{
	PBMSG_HEAD header; // C1:1B
	BYTE skill[2];
	BYTE index[2];
};

struct PMSG_DURATION_SKILL_ATTACK_SEND
{
	PBMSG_HEAD header; // C3:1E
	BYTE x;
	BYTE y;
	BYTE dir;
	BYTE skillH;
	BYTE indexH;
	BYTE skillL;
	BYTE indexL;
};

struct PMSG_RAGE_FIGHTER_SKILL_ATTACK_SEND
{
	PBMSG_HEAD header; // C3:4A
	BYTE index[2];
	BYTE skill[2];
	BYTE target[2];
};

struct PMSG_SKILL_DARK_SIDE_SEND
{
	PBMSG_HEAD header; // C3:4B
	WORD skill;
	WORD index[5];
};

struct PMSG_SKILL_NOVA_SEND
{
	PBMSG_HEAD header; // C1:BA
	BYTE index[2];
	BYTE type;
	BYTE count;
};

struct PMSG_SKILL_CHAIN_SKILL_SEND
{
	PSBMSG_HEAD header; // C1:BF:[0A:18]
	BYTE skill[2];
	WORD index;
	BYTE count;
};

struct PMSG_SKILL_CHAIN_SKILL
{
	WORD index;
};

struct PMSG_SKILL_LIST_SEND
{
	PSWMSG_HEAD header; // C2:F3:11
	BYTE count;
	BYTE type;
	//BYTE btListType;
};

struct PMSG_SKILL_LIST
{
#pragma pack(1)
	BYTE slot;
	WORD skill;
	BYTE level;
#pragma pack()
};

//**********************************************//
//**********************************************//
//**********************************************//

struct SKILL_INFO
{
	int Index;
	char Name[64];
	int Damage;
	int Mana;
	int BP;
	int Range;
	int Radio;
	int Delay;
	int Type;
	int Effect;
	int RequireLevel;
	int RequireEnergy;
	int RequireLeadership;
	int RequireKillCount;
	int RequireGuildStatus;
	int Element;
	int RequireClass[MAX_CLASS];
};

struct SKILL_BLOCK
{
	int Skill;
	int Map;
	int X1;
	int X2;
	int Y1;
	int Y2;
};

class CSkillManager
{
public:
	CSkillManager();
	virtual ~CSkillManager();
	void Load(char* path);
	void LoadSkillDamageRate(char* path);
	void LoadSkillBlock(char* path);
	bool GetInfo(int index, SKILL_INFO* lpInfo);
	int GetSkillDamage(int index);
	int GetSkillMana(int index);
	int GetSkillBP(int index);
	int GetSkillType(int index);
	int GetSkillEffect(int index);
	int GetSkillElement(int index);//2017/04/23
	int GetSkillAngle(int x, int y, int tx, int ty);
	int GetSkillFrustrum(int* SkillFrustrumX, int* SkillFrustrumY, BYTE angle, int x, int y, float sx, float sy, float tx, float ty);
	int GetSkillNumber(int index, int level);
	bool CheckSkillMana(LPOBJ lpObj, int index);
	bool CheckSkillBP(LPOBJ lpObj, int index);
	bool CheckSkillRange(int index, int x, int y, int tx, int ty, int add = 0);
	bool CheckSkillRadio(int index, int x, int y, int tx, int ty, int add = 0);
	bool CheckSkillFrustrum(int* SkillFrustrumX, int* SkillFrustrumY, int x, int y);
	bool CheckSkillDelay(LPOBJ lpObj, int index);
	bool CheckSkillTarget(LPOBJ lpObj, int aIndex, int bIndex, int type);
	bool CheckSkillRequireLevel(LPOBJ lpObj, int index);
	bool CheckSkillRequireEnergy(LPOBJ lpObj, int index);
	bool CheckSkillRequireLeadership(LPOBJ lpObj, int index);
	bool CheckSkillRequireKillPoint(LPOBJ lpObj, int index);
	bool CheckSkillRequireGuildStatus(LPOBJ lpObj, int index);
	bool CheckSkillRequireClass(LPOBJ lpObj, int index);
	bool CheckSkillRequireWeapon(LPOBJ lpObj, int index);
	int AddSkillWeapon(LPOBJ lpObj, int index, int level);
	int DelSkillWeapon(LPOBJ lpObj, int index, int level);
	int AddSkill(LPOBJ lpObj, int index, int level);
	int DelSkill(LPOBJ lpObj, int index);
	CSkill* GetSkill(LPOBJ lpObj, int index);
	int AddMasterSkill(LPOBJ lpObj, int index, int level);
	int DelMasterSkill(LPOBJ lpObj, int index);
	CSkill* GetMasterSkill(LPOBJ lpObj, int index);
	bool ConvertSkillByte(CSkill* lpSkill, BYTE* lpMsg);
	void SkillByteConvert(BYTE* lpMsg, CSkill* lpSkill);
	void UseAttackSkill(int aIndex, int bIndex, CSkill* lpSkill);
	void UseDurationSkillAttack(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, BYTE dir, BYTE angle);
	bool RunningSkill(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, BYTE angle, bool combo);
	bool BasicSkillAttack(int aIndex, int bIndex, CSkill* lpSkill);
	bool MultiSkillAttack(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillChangeUse(int aIndex);
	bool SkillEvilSpirit(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillManaShield(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillDefense(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillHeal(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillGreaterDefense(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillGreaterDamage(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillSummon(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillDecay(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y);
	bool SkillIceStorm(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y);
	bool SkillNova(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillTwistingSlash(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillRagefulBlow(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillDeathStab(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillGreaterLife(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillMonsterAreaAttack(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillPowerSlash(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle);
	bool SkillFireSlash(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillForce(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillFireBurst(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillEarthquake(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillSummonParty(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillGreaterCriticalDamage(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillElectricSpark(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle);
	bool SkillCastleSiege(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillStern(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle);
	bool SkillRemoveStern(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillGreaterMana(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillInvisibility(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillRemoveInvisibility(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillRemoveAllEffect(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillBrand(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillPlasmaStorm(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillInfinityArrow(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillMonsterSummon(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillMagicDamageImmunity(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillPhysiDamageImmunity(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillOrderOfProtection(int aIndex, int bIndex);
	bool SkillOrderOfRestraint(int aIndex, int bIndex);
	bool SkillOrderOfTracking(int aIndex);
	bool SkillOrderOfWeaken(int aIndex, int bIndex);
	bool SkillChainLightning(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillDamageReflect(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillSwordPower(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillSwordPowerGetDefense(int aIndex, int* defense);
	bool SkillSwordPowerGetPhysiDamage(int aIndex, int* DamageMin, int* DamageMax);
	bool SkillSwordPowerGetMagicDamage(int aIndex, int* DamageMin, int* DamageMax);
	bool SkillSwordPowerGetCurseDamage(int aIndex, int* DamageMin, int* DamageMax);
	bool SkillSleep(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillLesserDefense(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillLesserDamage(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillSahamutt(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y);
	bool SkillNeil(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y);
	bool SkillGhostPhantom(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y);
	bool SkillGhostPhantomAreaAttack(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y);
	bool SkillRedStorm(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillFrozenStab(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo);
	bool SkillMagicCircle(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillShieldRecover(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillFiveShot(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle);
	bool SkillTripleShot(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillSwordSlash(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle);
	bool SkillLightningStorm(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillBirds(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle);
	bool SkillSelfExplosion(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillKillingBlow(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillUpperBeast(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillChainDriver(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillDarkSide(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillDarkSideGetTargetIndex(int aIndex, int bIndex, CSkill* lpSkill, WORD* target);
	bool SkillDragonLore(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillGreaterIgnoreDefenseRate(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillFitness(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillGreaterDefenseSuccessRate(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillPhoenixShot(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillBloodStorm(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo);
	bool SkillCure(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillPartyHeal(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillPoisonArrow(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillBless(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillBlind(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillEarthPrison(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillIronDefense(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillBloodHowling(int aIndex, int bIndex, CSkill* lpSkill);
	void ApplyMeteoriteEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyIceStormEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyRagefulBlowEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyDeathStabEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyFireSlashEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyFireBurstEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyPlasmaStormEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyFireScreamEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyEarthquakeEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyDrainLifeEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplySahamuttEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyNeilEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyGhostPhantomEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyFrozenStabEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyFiveShotEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplySwordSlashEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyKillingBlowEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplySpinStepExplosionEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyBeastUppercutEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyChainDriverEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyDragonLoreEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyDragonSlayerEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyPhoenixShotEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyEarthPrisonEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void CGMultiSkillAttackRecv(PMSG_MULTI_SKILL_ATTACK_RECV* lpMsg, int aIndex, bool type);
	void CGSkillAttackRecv(PMSG_SKILL_ATTACK_RECV* lpMsg, int aIndex);
	void CGDurationSkillAttackRecv(PMSG_DURATION_SKILL_ATTACK_RECV* lpMsg, int aIndex);
	void CGSkillCancelRecv(PMSG_SKILL_CANCEL_RECV* lpMsg, int aIndex);
	void CGRageFighterSkillAttackRecv(PMSG_RAGE_FIGHTER_SKILL_ATTACK_RECV* lpMsg, int aIndex);
	void CGSkillDarkSideRecv(PMSG_SKILL_DARK_SIDE_RECV* lpMsg, int aIndex);
	void CGSkillTeleportAllyRecv(PMSG_SKILL_TELEPORT_ALLY_RECV* lpMsg, int aIndex);
	void GCSkillAttackSend(LPOBJ lpObj, int skill, int aIndex, BYTE type);
	void GCSkillCancelSend(LPOBJ lpObj, int skill);
	void GCDurationSkillAttackSend(LPOBJ lpObj, int skill, BYTE x, BYTE y, BYTE dir);
	void GCRageFighterSkillAttackSend(LPOBJ lpObj, int skill, int aIndex, BYTE type);
	void GCSkillAddSend(int aIndex, BYTE slot, int skill, BYTE level, BYTE type);
	void GCSkillDelSend(int aIndex, BYTE slot, int skill, BYTE level, BYTE type);
	void GCSkillListSend(LPOBJ lpObj, BYTE type);

	bool SkillSpinStep(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillCircleShield(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillObsidian(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillMagicPin(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillClash(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillHarshStrike(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillShiningPeak(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillBreche(int aIndex, int bIndex, CSkill* lpSkill, bool combo, BYTE angle);
	bool SkillWrath(int aIndex, int bIndex, CSkill* lpSkill);
	void ApplyMagicPinEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	bool SkillBurst(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillRush(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillIllusion(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillShadowStep(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillEvasion(int aIndex, int bIndex, CSkill* lpSkill);
	bool SkillParalyze(int aIndex, int bIndex, CSkill* lpSkill);

	bool SkillFireBlow(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool MeteorStrike_DW(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool MeteorStorm(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SoulSeeker(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool FocusShot(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool MeteorStrike_MG(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool DarkBlast(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool IceBlood(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool FireBlood(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool FireBeast(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool AquaBeast(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool SkillWindSoul(int aIndex, int bIndex, CSkill* lpSkill, bool combo, BYTE angle);
	bool Volcano(int aIndex, int bIndex, CSkill* lpSkill, bool combo);
	bool DarkPhoenxiShot(int aIndex, int bIndex, CSkill* lpSkill);
	bool ArchangelWill(int aIndex, int bIndex, CSkill* lpSkill);

	void ApplyIceBloodEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyFireBloodEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);

	int GetSkillSlot(LPOBJ lpObj, int index);
	bool ConvertForSkillByte(CSkill* lpSkill, BYTE* lpMsg);
	void ForSkillByteConvert(BYTE* lpMsg, CSkill* lpSkill);
	int AddForMasterSkill(LPOBJ lpObj, int index, int level, int group);
	CSkill* GetForMasterSkill(LPOBJ lpObj, int index, int group); // OK
	void ApplyMeteoriteEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyShockingEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyEvilSpiritEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyFreezingSlaughterEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyMeteorStormEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyPoisoningStormEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyBleedingHeartEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyFireBlowEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyTripleShotEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyFocusShotEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyDarkBlastEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyLightningStormEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyBirdsEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyWindSoulEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyRedStormEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage);
	void ApplyAquaBeastEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyFireBeastEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyDarkSideEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyBrecheEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void ApplyShiningPeakEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage); // OK
	void CGSkillAttack(int aIndex, int bIndex, int skill);
	void CGDurationSkillAttack(int aIndex, int Skill, int X, int Y, int Dir, int Dis, int Pos, int Targ);
	void CGSkillDarkSide(int aIndex, int bIndex, int skill);
	void CGRageFighterSkillAttack(int aIndex, int bIndex, int skill);
	bool SkillSpiritHook(int aIndex, int bIndex, CSkill* lpSkill);
	int GetBaseSkill(int skill);
	int GetDamageRatePvP(int skill);
	int GetDamageRatePvM(int skill);
	bool CheckSkillBlock(LPOBJ lpObj, int Skill);

	void DecreasePartyMemberHP(LPOBJ lpObj);

private:
	std::map<int, SKILL_INFO> m_SkillInfo;
	int DamageRatePvP[MAX_SKILL];
	int DamageRatePvM[MAX_SKILL];
	SKILL_BLOCK SkillBlock[100];
};

extern CSkillManager gSkillManager;
