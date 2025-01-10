#include "Protocol.h"


struct PMSG_SXUPPRONT
{
	PSBMSG_HEAD h;
	WORD Str;
	WORD AddStr;
	WORD Dex;
	WORD AddDex;
	WORD Vit;
	WORD AddVit;
	WORD Ene;
	WORD AddEne;
	WORD Leadership;
	WORD AddLeadership;
	float mPrec;
};
enum Attribute
{
SPL_ATK_PWR = 1,//特殊攻击力
SPL_ATK_PWR_RATE = 2,//特殊攻击率
SPL_PVP_ATK_PWR_RATE = 3,//特殊PVP攻击率
SPL_DEF	= 4,//特殊防御力
SPL_DEF_RATE = 6,//特殊防御率
SPL_PVP_DEF_RATE = 7,//PVP特殊防御率
SPL_MAG = 9,//特殊魔力
SPL_CURSE = 10,//特殊诅咒
};
struct PMSG_SXSPECIAL
{

	PSBMSG_HEAD h;
	WORD TypeA;
	WORD NumA;
	WORD NumA1;
	WORD TypeB;
	WORD NumB;
	WORD NumB1;	
	WORD TypeC;
	WORD NumC;
	WORD NumC1;	
	WORD TypeD;
	WORD NumD;
	WORD NumD1;	
	WORD TypeE;
	WORD NumE;
	WORD NumE1;

};
struct PMSG_SXMOSAIC
{
	PSBMSG_HEAD h;

	int nAttributeAttackPowerMin_info;
	int nAttributeAttackPowerMax_info;
	int nAttributeAttackPowerPVP_Min_info;
	int nAttributeAttackPowerPVP_Max_info;
	int nAttributeAttackSuccessRatePVE_info;
	int nAttributeAttackSuccessRatePVP_info;
	int nAttributeDefencePowerPVE_info;
	int nAttributeDefencePowerPVP_info;
	int nAttributeDefenceSuccessRatePVE_info;
	int nAttributeDefenceSuccessRatePVP_info;
	int nRubyAttackType;
	int nRubyAttackValue;
	int nEmeAttackType;
	int nEmeAttackValue;
	int nSapphDefeseType;
	int nSapphDefeseValue;
	int nTopaAttackType;
	int nTopaAttackAttack;
	int nAttributeCriticalDamageRate_info;

	DWORD IncreaseBossElementalDamage;
	DWORD IncreasePVPElementalDamage;

	DWORD lixo;
	DWORD ReduceBossElementalDamage;

	int AbsorbDamage;
	int AbsorbDamageAsLife;
	int BleedingDamage;
	int BindingRate;
	int ParalyzingRate;
	int PunishRate;
	int BlidingRate;
	int ReducePVPElementalDamage;
	int nAttributeResistantToAttributeAttacks_info;
	int nAttributeAddBunNoErtelOptionEffect_info;
	int nAttributeAddGaHoErtelOptionEffect_info;
};

struct PMSG_SXCHARINFO
{
	PSBMSG_HEAD h;  //0-3
	short CriticalDamage;//ok
	short ExcellentDamage;//ok
	short SkillDamageBonus;//ok
	short Defense;
	//all ok
	short Str;
	short AddStr;
	short Dex;
	short AddDex;
	short Vit;
	short AddVit;
	short Energy;
	short AddEnergy;
	short Leadership;
	short AddLeadership;
	//
	short EraDoubleEnaoFunfa;
	short IgnoreShieldGaugeRate;
	short SDAttack1;
	short MoneyAmountDropRate;

	float IgnoreDefenseRate;//ok
	float HPRecovery;
	
	float ANULADO;
	float StunRate;//substituir
	float ResistStunRate;//substituir
	
	short unk_data_1;	//60
	short unk_data_2;	//62
	
	float ShieldDamageReduction;//substituir

	float HP50percDmgRate;//substituir BOSS
	float TripleDamageResist;//substituir BOSS
	float RedBossDamage;//substituir BOSS

	float DecBossHitDelay;//substituir
	float DoubleDamageResist;//substituir
	
	float FullHPRestoreRate; //verificar ou substituir
	float ResistBossSkills; //substituir

	float AddBossDamage;//substituir
	float DamageReductionRate; //verificar pra fazer funcionar
	
	float BlockRate; //OK
	float ParryRate; //OK
	float AbsorbLife;//OK
	float DoubleDamageBoss; //substituir
	
	float FullDamageReflectRate; //OK
	float DamageIncreaseRate; //OK
	
	float HP30percDmgRate;//substituir

	float CriticalDamageRate;//OK
	float ExcellentDamageRate;//OK
	float DoubleDamageRate;//OK
	float TripleDamageRate;	//OK
	
	BYTE FinalDamageReductionRate; //OK
	BYTE DamageReduction[2]; //OK
	BYTE BPConsumptionRate;//
	BYTE DamageReflect;//
};

void C3TwoDecryp(BYTE* m_byBuffer);
void TwoDecryp(BYTE* m_byBuffer);
void XorData(int iStart, int iEnd,BYTE *m_byBuffer);
void SXGetCharacterInfo(int aIndex);
void GCCharacterInfo(LPOBJ lpObj);

//S12
void S12Decryp(BYTE* m_byBuffer);
//WebS11
void WebDecryp(BYTE* m_byBuffer);

void SXGetSpecial(int aIndex);
void SXGetMosaic(int aIndex);
void EventIcon(BYTE type,BYTE Switch);
