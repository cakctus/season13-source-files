#pragma once
#include "Protocol.h"
#define MAX_FOR_MASTER_SKILL_LIST 120
#define MAX_PARENT_SKILL	3

enum eForMasterSkill
{
	FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE = 1001,
	FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1 = 1002,
	FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE2 = 1003,
	FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE3 = 1004,
	FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE4 = 1005,
	FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE5 = 1006,
	FOR_MASTER_SKILL_ADD_INCREASE_THE_CHANCE_OF_ADDITIONAL_DAMAGE = 1007,
	FOR_MASTER_SKILL_ADD_INCREASES_ATTACK_SPEED = 1008,
	FOR_MASTER_SKILL_ADD_INCREASES_RANGE = 1009,
	FOR_MASTER_SKILL_ADD_ADD_SPLASH_DAMAGE = 1010,
	FOR_MASTER_SKILL_ADD_INCREASES_DISTANCE = 1011,
	FOR_MASTER_SKILL_ADD_TARGET_INCREASE = 1012,
	FOR_MASTER_SKILL_ADD_BUFF_SYNERGY = 1013,
	FOR_MASTER_SKILL_ADD_BUFF_SYNERGY1 = 1014,
	FOR_MASTER_SKILL_ADD_INCREASE_SKILL_DURATION = 1015,
	FOR_MASTER_SKILL_ADD_IRON_DEFENSE = 1016,
	FOR_MASTER_SKILL_ADD_REINFORCED_IRON_DEFENSE = 1017,
	FOR_MASTER_SKILL_ADD_REDUCE_REUSE_TIME = 1018,
	FOR_MASTER_SKILL_ADD_ELIMINATE_REUSE_TIME = 1019,
	FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE = 1020,
	FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER = 1021,
	FOR_MASTER_SKILL_ADD_ADD_PENETRATION_EFFECT = 1022,
	FOR_MASTER_SKILL_ADD_ADD_ARROW_PROJECTILE = 1023,
	FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1 = 1024,
	FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1 = 1025,
	FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1 = 1026,
	FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING1 = 1027,
	FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1 = 1028,
	FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_EFFECT1 = 1029,
	FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1 = 1030,
	FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT1 = 1031,
	FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1 = 1032,
	FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1 = 1033,
	FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1 = 1034,
	FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION1 = 1035,
	FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_POISONING_EFFECT1 = 1036,
	FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_FREEZING_EFFECT1 = 1037,
	FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_BLEEDING_EFFECT1 = 1038,
	FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_SHOCKING_EFFECT1 = 1039,
	FOR_MASTER_SKILL_ADD_POISONING_UPGRADE_PROBABILITY_ADDICTION1 = 1040,
	FOR_MASTER_SKILL_ADD_FREEZING_UPGRADE_PROBABILITY_FREEZE1 = 1041,
	FOR_MASTER_SKILL_ADD_BLEEDING_UPGRADE_PROBABILITY_EXCESSIVE_BLEEDING1 = 1042,
	FOR_MASTER_SKILL_ADD_SHOCKING_UPGRADE_PROBABILITY_STUN1 = 1043,
	FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_EFFECT1 = 1044,
	FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION1 = 1045,
	FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2 = 1046,
	FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2 = 1047,
	FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2 = 1048,
	FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING2 = 1049,
	FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2 = 1050,
	FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_EFFECT2 = 1051,
	FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2 = 1052,
	FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT2 = 1053,
	FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2 = 1054,
	FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2 = 1055,
	FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2 = 1056,
	FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION2 = 1057,
	FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_POISONING_EFFECT2 = 1058,
	FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_FREEZING_EFFECT2 = 1059,
	FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_BLEEDING_EFFECT2 = 1060,
	FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_SHOCKING_EFFECT2 = 1061,
	FOR_MASTER_SKILL_ADD_POISONING_UPGRADE_PROBABILITY_ADDICTION2 = 1062,
	FOR_MASTER_SKILL_ADD_FREEZING_UPGRADE_PROBABILITY_FREEZE2 = 1063,
	FOR_MASTER_SKILL_ADD_BLEEDING_UPGRADE_PROBABILITY_EXCESSIVE_BLEEDING2 = 1064,
	FOR_MASTER_SKILL_ADD_SHOCKING_UPGRADE_PROBABILITY_STUN2 = 1065,
	FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_EFFECT2 = 1066,
	FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION2 = 1067,
	FOR_MASTER_SKILL_ADD_RAGEFUL_BLOW_ENHANCED = 1069,
	FOR_MASTER_SKILL_ADD_DEATH_STAB_ENHANCED = 1071,
	FOR_MASTER_SKILL_ADD_FIRE_BLOW_ENHANCED = 1072,
	FOR_MASTER_SKILL_ADD_METEOR_STRIKE_ENHANCED = 1075,
	FOR_MASTER_SKILL_ADD_METEOR_STORM_ENHANCED = 1076,
	FOR_MASTER_SKILL_ADD_EVIL_SPIRIT_ENHANCED_DW = 1078,
	FOR_MASTER_SKILL_ADD_TRIPLE_SHOT_ENHANCED = 1081,
	FOR_MASTER_SKILL_ADD_MULTI_SHOT_ENHANCED = 1083,
	FOR_MASTER_SKILL_ADD_FOCUS_SHOT_ENHANCED = 1085,
	FOR_MASTER_SKILL_ADD_GIGANTIC_STORM_ENHANCED = 1087,
	FOR_MASTER_SKILL_ADD_EVIL_SPIRIT_ENHANCED_MG = 1088,
	FOR_MASTER_SKILL_ADD_DARK_BLAST_ENHANCED = 1089,
	FOR_MASTER_SKILL_ADD_FIRE_SLASH_ENHANCED = 1092,
	FOR_MASTER_SKILL_ADD_FIRE_BLOOD_ENHANCED = 1094,
	FOR_MASTER_SKILL_ADD_ICE_BLOOD_ENHANCED = 1095,
	FOR_MASTER_SKILL_ADD_FIRE_BURST_ENHANCED = 1096,
	FOR_MASTER_SKILL_ADD_CHAOTIC_DISEIER_ENHANCED = 1098,
	FOR_MASTER_SKILL_ADD_WIND_SOUL_ENHANCED = 1099,
	FOR_MASTER_SKILL_ADD_FIRE_BEAST_ENHANCED = 1102,
	FOR_MASTER_SKILL_ADD_LIGHTNING_SHOCK_ENHANCED = 1103,
	FOR_MASTER_SKILL_ADD_AQUA_BEAST_ENHANCED = 1105,
	FOR_MASTER_SKILL_ADD_DRAGON_ROAR_ENHANCED = 1111,
	FOR_MASTER_SKILL_ADD_CHAIN_DRIVE_ENHANCED = 1112,
	FOR_MASTER_SKILL_ADD_DARK_SIDE_ENHANCED = 1113,
	FOR_MASTER_SKILL_ADD_MAGIC_PIN_ENHANCED = 1117,
	FOR_MASTER_SKILL_ADD_BRECHE_ENHANCED = 1118,
	FOR_MASTER_SKILL_ADD_SHINING_PEAK_ENHANCED = 1119,
	FOR_MASTER_SKILL_ADD_POISONING_STORM = 1125,
	FOR_MASTER_SKILL_ADD_FREEZING_SLAUGHTER = 1126,
	FOR_MASTER_SKILL_ADD_BLEEDING_HEART = 1127,
	FOR_MASTER_SKILL_ADD_SHOCKING_BOOM = 1128,
	FOR_MASTER_SKILL_ADD_POISONING_STORM_ENHANCED_DW = 1129,
	FOR_MASTER_SKILL_ADD_FREEZING_SLAUGHTER_ENHANCED = 1130,
	FOR_MASTER_SKILL_ADD_BLEEDING_HEART_ENHANCED = 1131,
	FOR_MASTER_SKILL_ADD_SHOCKING_BOOM_ENHANCED = 1132,
	FOR_MASTER_SKILL_ADD_POISONING_STORM_ENHANCED = 1133,
	FOR_MASTER_SKILL_ADD_FREEZING_SLAUGHTER_ENHANCED1 = 1134,
	FOR_MASTER_SKILL_ADD_BLEEDING_HEART_ENHANCED1 = 1135,
	FOR_MASTER_SKILL_ADD_SHOCKING_BOOM_ENHANCED1 = 1136,
	FOR_MASTER_SKILL_ADD_INCREASED_MAXIMUM_HEALTH = 1137,
	//FOR_MASTER_SKILL_ADD_FOURTH_STATS = 1138,
	//FOR_MASTER_SKILL_ADD_INCREASE_BASIC_ARMOR = 1139,
	//FOR_MASTER_SKILL_ADD_4TH_WING_DAMAGE_INCREASE = 1140,
	//FOR_MASTER_SKILL_ADD_INCREASED_ATTACK_POWER = 1141,
	////FOR_MASTER_SKILL_ADD_INCREASED_ATTACK_POWER = 1142,
	////FOR_MASTER_SKILL_ADD_FOURTH_STATS = 1143,
	//FOR_MASTER_SKILL_ADD_INCREASED_SKILL_DAMAGE = 1144,
	////FOR_MASTER_SKILL_ADD_4TH_WING_DAMAGE_INCREASE = 1145,
	//FOR_MASTER_SKILL_ADD_ATTACK_POWER_OR_HORSE_POWER_INCREASE = 1146,
	//FOR_MASTER_SKILL_ADD_SPIRIT_HOOK_ENHANCED_SKILL = 1147,



};

struct MASTERSKILL_4TH_DATA
{
	int Line;
	int Group;
	int UIgroup;
	int unk;
	int SkillID;
	int BuffIndex;
	int AttributeType;
	int FormulaDateID;
	int LinkSlot1;
	int LinkSlot2;
	int LinkSlot3;
	int LinkSlot4;
	int LinkSlot5;
	int NeedPoint;
	int MaxPoint;
	int ActivateNextSlotPoint;
	int ParentSkill[MAX_PARENT_SKILL];
	int ParentSkillNeedPoint[MAX_PARENT_SKILL];
};

struct FOR_MASTER_SKILL_INFO
{
	int Class;
	MASTERSKILL_4TH_DATA m_ForMasterSkillData;
};	
//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_FOR_MASTER_SKILL_TREE_SEND
{
	PSBMSG_HEAD header; // C1:7E:00
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_FOR_MASTER_SKILL_TREE_SAVE_SEND
{
	PSWMSG_HEAD header; // C2:7E:01
	WORD index;
	char account[11];
	char name[11];
	DWORD SkillEnhanceTreePoints;
	BYTE ForMasterSkill[MAX_FOR_MASTER_SKILL_LIST][4];
};
//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_FOR_MASTER_SKILL_TREE_RECV
{
	PSWMSG_HEAD header; // C2:7E:00
	WORD index;
	char account[11];
	char name[11];
	DWORD SkillEnhanceTreePoints;
	BYTE ForMasterSkill[MAX_FOR_MASTER_SKILL_LIST][4];
};
//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//
#pragma pack(1)
struct PMSG_FOR_MASTER_SKILL_RECV
{
	PSBMSG_HEAD header; // C1:7E:01
	BYTE Group;
	WORD ForMasterSkill;
	WORD Line;
};
#pragma pack()
//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//
#pragma pack(1)
struct FOR_MASTER_SKILL_LIST_INFO//size 12
{
	BYTE Group;
	WORD Line;
	BYTE level;
	float Value;
	float NextValue;
};

struct PMSG_FOR_MASTER_INFO_SEND
{
	PSWMSG_HEAD header; // C1:7E:02
	WORD SkillEnhanceTreePoints;
	DWORD count;
};
#pragma pack()

#pragma pack(1)
struct MyStruct22 //size 11
{
	BYTE UNK1;
	BYTE UNK2;
	BYTE UNK3;
	float Value;
	float NextValue;
};

struct MyStruct2 //size 9
{
	PSWMSG_HEAD header; // C1:7E:06
	DWORD count;
};
#pragma pack()

#pragma pack(1)
struct PMSG_ADD_FOR_MASTERSKILL_SEND
{
	PSBMSG_HEAD header;// C1:7E:01
	BYTE Type; //1 4 7
	WORD SkillEnhanceTreePoints;
	BYTE Group;//??
	BYTE Level;
	WORD Line;
	float Value;
	float NextValue;

};
#pragma pack()

#pragma pack(1)
struct PMSG_SKILL_ENHANCE_TREE_POINT_SEND
{
	PSBMSG_HEAD header; //  C1:7E:03
	WORD SkillEnhanceTreePoint;
};
#pragma pack()

class MasterSkillTree_4th
{
public:
	MasterSkillTree_4th();
	~MasterSkillTree_4th();
	void Load(char* path); // OK
	void GCSkillTreeEnchancePointSend(LPOBJ lpObj);
	void GDForMasterSkillTreeSend(int aIndex); // OK
	void GDForMasterSkillTreeSaveSend(int aIndex); // OK
	void DGForMasterSkillTreeRecv(SDHP_FOR_MASTER_SKILL_TREE_RECV* lpMsg); // OK
	void GCForMasterInfoSend(LPOBJ lpObj); // OK
	int GetForMasterSkillParentSkill(int Class, int index);
	void CGMasterSkillRecv(PMSG_FOR_MASTER_SKILL_RECV* lpMsg, int aIndex); // OK
	bool GetInfo(int Class, int index,int group, FOR_MASTER_SKILL_INFO* lpInfo); // OK
	bool CheckParentSkill(LPOBJ lpObj, FOR_MASTER_SKILL_INFO*lpIofo);
	bool CheckForMasterSkillGroup(LPOBJ lpObj, int Group, int Line, FOR_MASTER_SKILL_INFO*lpIofo);
	bool ReplaceSkill(LPOBJ lpObj, int level, FOR_MASTER_SKILL_INFO*lpIofo); // OK
	void GCAddForMasterSkill(LPOBJ lpObj, FOR_MASTER_SKILL_INFO*lpIofo);
	void CalcMasterSkillTreeOption(LPOBJ lpObj, bool flag);
	void InsertOption(LPOBJ lpObj, int index, float value, bool flag,int group); // OK
	int ForMasterSkillAddDamage(LPOBJ lpObj, int Damage, int skill);
	bool GetParentSkill(LPOBJ lpObj, int Skill, FOR_MASTER_SKILL_INFO*lpIofo);
	float GetForMasterSkillValue(LPOBJ lpObj, int index,int group); // OK
	bool CheckForMasterSkill(LPOBJ lpObj, int index, int group);
	BOOL CheckForMaster(LPOBJ lpObj);
private:
	std::vector<FOR_MASTER_SKILL_INFO>m_ForMasterSkillTreeInfo;
};

extern MasterSkillTree_4th gMasterSkillTree_4th;