#pragma once

#define MAX_CLASS 8

enum eClassNumber
{
	CLASS_DW = 0,
	CLASS_DK = 1,
	CLASS_FE = 2,
	CLASS_MG = 3,
	CLASS_DL = 4,
	CLASS_SU = 5,
	CLASS_RF = 6,
	CLASS_GL = 7,
};

enum eDBClassNumber
{
	DB_CLASS_DW = 0,
	DB_CLASS_SM = 8,
	DB_CLASS_GM = 12,
	DB_CLASS_FS = 14,

	DB_CLASS_DK = 16,
	DB_CLASS_BK = 24,
	DB_CLASS_BM = 28,
	DB_CLASS_D = 30,

	DB_CLASS_FE = 32,
	DB_CLASS_ME = 40,
	DB_CLASS_HE = 44,
	DB_CLASS_SF = 46,

	DB_CLASS_MG = 48,
	DB_CLASS_DM = 60,
	DB_CLASS_MK = 62,

	DB_CLASS_DL = 64,
	DB_CLASS_LE = 76,
	DB_CLASS_HP = 78,

	DB_CLASS_SU = 80,
	DB_CLASS_BS = 88,
	DB_CLASS_DS = 92,
	DB_CLASS_WZ = 94,

	DB_CLASS_RF = 96,
	DB_CLASS_FM = 108,
	DB_CLASS_FG = 110,

	DB_CLASS_FK = 112,
	DB_CLASS_PK = 124,
	DB_CLASS_SY = 126,
};

struct DEFAULT_CLASS_INFO
{
	int Class;
	int Strength;
	int Dexterity;
	int Vitality;
	int Energy;
	int Leadership;
	float MaxLife;
	float MaxMana;
	float LevelLife;
	float LevelMana;
	float VitalityToLife;
	float EnergyToMana;
};

class CDefaultClassInfo
{
public:
	CDefaultClassInfo();
	virtual ~CDefaultClassInfo();
	void Init();
	void Load(char* path);
	void SetInfo(DEFAULT_CLASS_INFO info);
	int GetCharacterDefaultStat(int Class,int stat);
public:
	DEFAULT_CLASS_INFO m_DefaultClassInfo[MAX_CLASS];
};

extern CDefaultClassInfo gDefaultClassInfo;
