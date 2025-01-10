#pragma once

#include "Item.h"
#include "User.h"

#define MAX_SOCKET_ITEM_OPTION_TYPE 6
#define MAX_SOCKET_ITEM_OPTION 50
#define MAX_SOCKET_ITEM_OPTION_TABLE 5
#define MAX_SOCKET_ITEM_OPTION_LEVEL 10


enum eSocketItemOptionType
{
	SOCKET_ITEM_OPTION_TYPE_NONE = 0,
	SOCKET_ITEM_OPTION_TYPE_FIRE = 1,
	SOCKET_ITEM_OPTION_TYPE_WATER = 2,
	SOCKET_ITEM_OPTION_TYPE_ICE = 3,
	SOCKET_ITEM_OPTION_TYPE_WIND = 4,
	SOCKET_ITEM_OPTION_TYPE_LIGHTNING = 5,
	SOCKET_ITEM_OPTION_TYPE_EARTH = 6,
};

enum eSocketItemOption
{
	SOCKET_ITEM_OPTION_DANO_ADICIONAL = 0,
	SOCKET_ITEM_OPTION_CHANCE_ELEMENTAL_TRIPLO = 1,
	SOCKET_ITEM_OPTION_CHANCE_ABSORV_HP = 2,
	SOCKET_ITEM_OPTION_DANO_MIN_MAX_ADICIONAL = 3,
	SOCKET_ITEM_OPTION_DANO_ADICIONAL_ELITE = 4,
	SOCKET_ITEM_OPTION_DANO_ELEMENTAL_ADICIONAL = 5,

	SOCKET_ITEM_OPTION_RESISTENCIA_RADIANCE = 10,
	SOCKET_ITEM_OPTION_DEFESA_ADICIONAL = 11,
	SOCKET_ITEM_OPTION_REDUCAO_DANO_ESCUDO = 12,
	SOCKET_ITEM_OPTION_REDUCAO_DANO_GERAL = 13,
	SOCKET_ITEM_OPTION_ADICIONAL_DANO_REFLECT = 14,

	SOCKET_ITEM_OPTION_DANO_ADICIONAL_BOSS = 16,
	SOCKET_ITEM_OPTION_ACELERACAO_HITS_BOSS = 17,
	SOCKET_ITEM_OPTION_DANO_VERDADEIRO = 18,
	SOCKET_ITEM_OPTION_CHANCE_DANO_30PERC_HP = 19,
	SOCKET_ITEM_OPTION_CHANCE_REDUZIR_DURABILIDADE = 20,

	SOCKET_ITEM_OPTION_CHANCE_RECUPERAR_20PERC_HP = 21,
	SOCKET_ITEM_OPTION_ADICIONAL_HP = 22,
	SOCKET_ITEM_OPTION_REDUCAO_DANO_BOSS = 23,
	SOCKET_ITEM_OPTION_REDUCAO_DANO_ELEMENTAL = 24,	
	SOCKET_ITEM_OPTION_RESISTENCIA_DANO_TRIPLO = 25,
	SOCKET_ITEM_OPTION_RESISTENCIA_DANO_DUPLO = 26,

	SOCKET_ITEM_OPTION_ADICIONAL_DANO_EXCELENTE = 29,
	SOCKET_ITEM_OPTION_ADICIONAL_CHANCE_DANO_EXCELENTE = 30,
	SOCKET_ITEM_OPTION_ADICIONAL_DANO_CRITICO = 31,
	SOCKET_ITEM_OPTION_ADICIONAL_CHANCE_DANO_CRITICO = 32,

	SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_FORCA = 34,
	SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_AGILIDADE = 35,
	SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_VITALIDADE = 36,
	SOCKET_ITEM_OPTION_ADICIONAL_PONTOS_ENERGIA = 37,

	SOCKET_ITEM_OPTION_EMPTY = 254,
	SOCKET_ITEM_OPTION_NONE = 255,
};

enum eSocketItemBonusOption
{
	SOCKET_ITEM_WEAPON_BONUS_OPTION_DANO_PVP = 0,
	SOCKET_ITEM_WEAPON_BONUS_OPTION_DANO_BOSS = 1,
	SOCKET_ITEM_STAFF_BONUS_OPTION_DANO_PVP = 2,
	SOCKET_ITEM_STAFF_BONUS_OPTION_DANO_BOSS = 3,
	SOCKET_ITEM_ARMOR_BONUS_OPTION_REDUZ_DANO_BOSS_PVP = 4,
	SOCKET_ITEM_ARMOR_BONUS_OPTION_AUMENTA_HP_MAXIMO = 5,
	SOCKET_ITEM_BONUS_OPTION_NONE = 255
};

enum eSocketItemPackageOption
{
	SOCKET_ITEM_PACKAGE_OPTION_CHANCE_DANO_EXCELENTE = 0,
	SOCKET_ITEM_PACKAGE_OPTION_CHANCE_DANO_DUPLO = 1,
	SOCKET_ITEM_PACKAGE_OPTION_CHANCE_DANO_TRIPLO = 2,
};

struct SOCKET_ITEM_OPTION_INFO
{
	int Index;
	int Type;
	int SubIndex;
	char Name[64];
	int Rate;
	float ValueTable[MAX_SOCKET_ITEM_OPTION_LEVEL];
};

struct SOCKET_ITEM_BONUS_OPTION_INFO
{
	int Index;
	int MinItemSection;
	int MaxItemSection;
	char Name[64];
	int Rate;
	float Value;
	int RequireOptionTypeTable[MAX_SOCKET_ITEM_OPTION_TABLE];
};

struct SOCKET_ITEM_PACKAGE_OPTION_INFO
{
	int Index;
	char Name[64];
	float Value;
	int RequireOptionTypeCountTable[MAX_SOCKET_ITEM_OPTION_TYPE];
};

class CSocketItemOption
{
public:
	CSocketItemOption();
	virtual ~CSocketItemOption();
	void Init();
	void Load(char* path);
	void SetInfo(SOCKET_ITEM_OPTION_INFO info);
	void SetBonusInfo(SOCKET_ITEM_BONUS_OPTION_INFO info);
	void SetPackageInfo(SOCKET_ITEM_PACKAGE_OPTION_INFO info);
	SOCKET_ITEM_OPTION_INFO* GetInfo(int index);
	SOCKET_ITEM_BONUS_OPTION_INFO* GetBonusInfo(int index);
	SOCKET_ITEM_PACKAGE_OPTION_INFO* GetPackageInfo(int index);
	bool IsSocketItem(CItem* lpItem);
	int GetSocketItemOptionCount(CItem* lpItem);
	int GetSocketItemExtraMoney(CItem* lpItem);
	int GetSocketItemSeedOption(int index,int type);
	int GetSocketItemRandomOption(int type);
	BYTE GetSocketItemOption(CItem* lpItem,int type,int option,int level);
	int GetSocketItemOptionSXI(CItem* lpItem,int type,int option,int level);//s11-2
	BYTE GetSocketItemBonusOption(CItem* lpItem);
	bool CheckSocketItemOption(int index,CItem* lpItem);
	bool CheckSocketItemBonusOption(int index,CItem* lpItem);
	bool CheckSocketItemPackageOption(int index,int* ItemOptionTypeCount);
	void CalcSocketItemOption(LPOBJ lpObj,bool flag);
	void CalcSocketItemPackageOption(LPOBJ lpObj,int* ItemOptionTypeCount,bool flag);
	void InsertOption(LPOBJ lpObj,int index,float value,bool flag);
	void InsertBonusOption(LPOBJ lpObj,int index,float value,bool flag);
	void InsertPackageOption(LPOBJ lpObj,int index,float value,bool flag);
	short Get10BitSocketOptionIndex(BYTE btSocketSlotIndex, BYTE btSocketOption, BYTE btSetOption, BYTE btExcellentOption);
	void Convert10BitSocketOptionValue(short nSeedSphereIndex, BYTE btSocketSlotIndex, BYTE& btSockeOption, BYTE& btSetOption, BYTE& btExcellentOption);
	int GetSeedIndexFromSphereIndexOrderScript(short nSeedSphearIndex);
	BYTE GetSeedSphearLevel(short btSeedSphereIndex);
	bool ClearSocketSlot(CItem* lpTargetItem, BYTE btSocketSlotIndex);
	CItem GetSocketExtractSeedSphere(CItem* lpItem,int type);

private:
	SOCKET_ITEM_OPTION_INFO m_SocketItemOptionInfo[MAX_SOCKET_ITEM_OPTION];
	SOCKET_ITEM_BONUS_OPTION_INFO m_SocketItemBonusOptionInfo[MAX_SOCKET_ITEM_OPTION];
	SOCKET_ITEM_PACKAGE_OPTION_INFO m_SocketItemPackageOptionInfo[MAX_SOCKET_ITEM_OPTION];
};

extern CSocketItemOption gSocketItemOption;
