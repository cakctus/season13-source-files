#pragma once
#include "Protocol.h"
#define MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST 100
#define MAX_HUNTING_RECORD_MAP_LIST 50

struct HUNTING_RECORD_INFO
{
	BYTE btMapIndex;
	int  iYear;
	BYTE btMonth;
	BYTE btDay;
	int iCurrentLevel;
	int iHuntingAccrueSecond;
	QWORD i64NormalAccrueDamage;
	QWORD i64PentagramAccrueDamage;
	int iHealAccrueValue;
	int iMonsterKillCount;
	QWORD i64AccrueExp;
	int iClass;
	int iMaxNormalDamage;
	int iMinNormalDamage;
	int iMaxPentagramDamage;
	int iMinPentagramDamage;
	int iGetNormalAccrueDamage;
	int iGetPentagramAccrueDamage;
};

struct  _HUNTING_RECORD_ACCRUE_DATA
{
	int iMapIndex;
	int iYear;
	BYTE btMonth;
	BYTE btDay;
	int iCurrentLevel;
	int iHuntingAccrueSecond;
	QWORD i64NormalAccrueDamage;
	QWORD i64PentagramAccrueDamage;
	int iHealAccrueValue;
	int iMonsterKillCount;
	BYTE btExp1;
	BYTE btExp2;
	BYTE btExp3;
	BYTE btExp4;
	BYTE btExp5;
	BYTE btExp6;
	BYTE btExp7;
	BYTE btExp8;
};
//**********************************************//
//************ GameServer -> DataServer ************//
//**********************************************//

struct SDHP_REQ_HUNTING_RECORD_INFO
{
	PSBMSG_HEAD h;//
	char AccountId[11];
	char szName[11];
	int iUserIndex;
	BYTE btMapIndex;
	BYTE btCallType;
	BYTE btAnotherUser;
};
struct SDHP_REQ_HUNTING_RECORD_INFO_SAVE
{
	PSWMSG_HEAD h;
	BYTE btListCnt;
	char AccountId[11];
	char szName[11];
};
struct SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN
{
	PSBMSG_HEAD h;//
	char AccountId[11];
	char szName[11];
	int iUserIndex;
};
struct SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN_SAVE
{
	PSBMSG_HEAD h;//
	char AccountId[11];
	char szName[11];
	int iUserIndex;
	BYTE btOpen;
};
struct SDHP_REQ_HUNTING_RECORD_INFO_CURRENT
{
	PSBMSG_HEAD h;//
	char AccountId[11];
	char szName[11];
	int iUserIndex;
	BYTE btMapIndex;
	int iYear;
	BYTE btMonth;
	BYTE btDay;
	BYTE btCallType;
};
struct SDHP_REQ_DELETE_HUNTING_RECORD_INFO
{
	PSBMSG_HEAD h;//
	char AccountId[11];
	char szName[11];
	int iUserIndex;
	BYTE btMapIndex;
	int iYear;
	BYTE btMonth;
	BYTE btDay;
};
//**********************************************//
//************ DataServer  -> GameServer ************//
//**********************************************//

struct SDHP_ANS_HUNTING_RECORD_COUNT
{
	PSWMSG_HEAD h;//
	BYTE btResult;
	BYTE btListCnt;
	BYTE btMapIndex;
	BYTE btCallType;
	BYTE btAnotherUser;
	int iUserIndex;

};
struct SDHP_ANS_HUNTING_RECORD_INFO_CURRENT
{
	PSWMSG_HEAD h;//
	int iUserIndex;
	BYTE btCallType;
	BYTE btListCnt;
	BYTE btMapIndex;
	int iYear;
	BYTE btMonth;
	BYTE btDay;
	int iCurrentLevel;
	int iHuntingAccrueSecond;
	QWORD i64NormalAccrueDamage;
	QWORD i64PentagramAccrueDamage;
	int iHealAccrueValue;
	int iMonsterKillCount;
	QWORD i64AccrueExp;
	int iClass;
	int iMaxNormalDamage;
	int iMinNormalDamage;
	int iMaxPentagramDamage;
	int iMinPentagramDamage;
	int iGetNormalAccrueDamage;
	int iGetPentagramAccrueDamage;
};
struct SDHP_ANS_HUNTING_RECORD_INFO_USER_OPEN
{
	PSBMSG_HEAD h;//
	BYTE btResult;
	int iUserIndex;
	BYTE btOpen;
};

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//
#pragma pack(1)
struct PMSG_REQ_HUNTING_RECORD_WND_OPEN
{
	PSBMSG_HEAD h;
	BYTE btMapIndex;
	int aIndex;
};
#pragma pack()

struct PMSG_REQ_HUNTING_RECORD_INFO_USER_OPEN
{
	PSBMSG_HEAD h;
	BYTE btOpen;
};
struct PMSG_REQ_HUNTING_RECORD_INFO_USER_OPEN_GET
{
	PSBMSG_HEAD h;
	int iUserIndex;
};

//**********************************************//
//************  GameServer -> Client************//
//**********************************************//
struct PMSG_ANS_HUNTING_RECORD_INFO_USER_OPEN_GET
{
	PSBMSG_HEAD h;
	BYTE btResult;
};
struct PMSG_ANS_HUNTING_RECORD_DATA
{
	PSBMSG_HEAD h;
	BYTE btResult;
	int iYear;
	BYTE btMonth;
	BYTE btDay;
	int iCurrentLevel;
	int iHuntingAccrueSecond;
	QWORD i64NormalAccrueDamage;
	QWORD i64PentagramAccrueDamage;
	int iHealAccrueValue;
	int iMonsterKillCount;
	BYTE btExp1;
	BYTE btExp2;
	BYTE btExp3;
	BYTE btExp4;
	BYTE btExp5;
	BYTE btExp6;
	BYTE btExp7;
	BYTE btExp8;
};

struct PMSG_ANS_HUNTING_RECORD_COUNT
{
	PSWMSG_HEAD head;
	BYTE btResult;
	BYTE btRecordCnt;
	BYTE btUserOpen;
};

struct PMSG_ANS_HUNTING_RECORD_CACHE_DATA
{
	PSBMSG_HEAD h;
	int iHuntingAccrueSecond;
	QWORD i64NormalAccrueDamage;
	QWORD i64PentagramAccrueDamage;
	int iHealAccrueValue;
	int iMonsterKillCount;
	BYTE btExp1;
	BYTE btExp2;
	BYTE btExp3;
	BYTE btExp4;
	BYTE btExp5;
	BYTE btExp6;
	BYTE btExp7;
	BYTE btExp8;
};

struct PMSG_REQ_HUNTING_RECORD_WND_CLOSE
{
	PSBMSG_HEAD h;//
};
struct HUNTING_RECORD_EXCEPT_MONSTER_LIST
{
	HUNTING_RECORD_EXCEPT_MONSTER_LIST()
	{
		this->Clear();
	}
	void Clear()
	{
		this->iMonsterIndex = -1;
	}
	int iMonsterIndex;
};
struct HUNTING_RECORD_MAP_LIST
{
	HUNTING_RECORD_MAP_LIST()
	{
		this->Clear();
	}
	void Clear()
	{
		this->iMapIndex = -1;
	}
	int iMapIndex;

};
class CGHuntingGrounds
{	
public:
	CGHuntingGrounds(void);
	virtual ~CGHuntingGrounds(void);
	void  Initialize_HuntingRecordData(); // idb
	void  LoadHuntingRecordScript( char *pchFileName);
	void  HuntingRecordWndOpen(int aIndex, char btMapIndex, int iTargetIndex);
	void  HuntingRecordWndClose(int aIndex);
	void  HuntingRecordInfoUserOpen(int aIndex, char btOpen);
	void  HuntingRecordInfoUserOpenGet( int aIndex, int iUserIndex); // idb
	void  HuntingRecordMacroStart(int aIndex); // idb
	void  HuntingRecordMacroEnd(int aIndex);
	void  AddMacroSecond( int aIndex); // idb
	void  AddNormalDamage(int aIndex, int aTargetIndex, int iDamage); // idb
	void  AddPentagramDamage(int aIndex, int aTargetIndex, int iDamage); // idb
	void  AddGetNormalDamage(int aIndex, int aTargetIndex, int iDamage); // idb
	void  AddGetPentagramDamage(int aIndex, int aTargetIndex, int iDamage); // idb
	void  AddHealValue(int aIndex, int iValue); // idb
	void  AddMonsterKillCount(int aIndex, LPOBJ lpMonsterObj); // idb
	void  AddGainExp(int aIndex, int iMonsterType, __int64 i64Exp); // idb
	void  GCHuntingRecordInfo(int aIndex); // idb
	void  GCHuntingRecordInfo_Today(int aIndex, int iMapIndex, int iAnotherUser); // idb
	void  GCHuntingRecordAccrueInfo(int aIndex); // idb
	void  LoadHuntingRecord(LPOBJ lpObj, int iMapIndex);
	void  MessageHuntingRecordAccrueInfo(int aIndex); // idb
	void  GCHuntingCacheRecordInfo(int aIndex); // idb
	void  DGAnsHuntingRecordInfo_Current(SDHP_ANS_HUNTING_RECORD_INFO_CURRENT *lpRecv); // idb
	void  CGReqHuntingRecordInfoUserOpen(PMSG_REQ_HUNTING_RECORD_INFO_USER_OPEN *lpMsg, int aIndex);
	void  CGReqHuntingRecordWndtOpen(PMSG_REQ_HUNTING_RECORD_WND_OPEN *lpMsg, int aIndex);
	void  CGReqHuntingRecordInfoUserOpenGet(PMSG_REQ_HUNTING_RECORD_INFO_USER_OPEN_GET *lpMsg, int aIndex);
	void  DGAnsHuntingRecordInfoUserOpen(SDHP_ANS_HUNTING_RECORD_INFO_USER_OPEN *lpRecv);
	void  DGAnsHuntingRecordInfo(SDHP_ANS_HUNTING_RECORD_COUNT *lpRecv); // idb
	void  GDReqHuntingRecordInfoSave(LPOBJ lpObj); // idb
	void  GDReqHuntingRecordInfoUserOpen(LPOBJ lpObj, char *szAccountID); // idb
	void  GDReqHuntingRecordInfoUserOpenSave(LPOBJ lpObj); // idb
	void  GDReqHuntingRecordInfo_Current(LPOBJ lpObj, LPOBJ lpTargetObj, char btMapIndex, int iYear, char btMonth, char btDay, char btCallType); // idb
	void  GDReqDeleteHuntingRecordInfo(LPOBJ lpObj, char btMapIndex, int iYear, char btMonth, char btDay); // idb
	void  GDReqHuntingRecordInfoSave_GM(LPOBJ lpObj, int iMapIndex, int iYear, int btMonth, int btDay, int iSecond, __int64 i64NormalAccrueDamage, __int64 i64PentagramAccrueDamage, __int64 i64GainExp, int iHealAccrueValue, int iMonsterKillCount, int iMaxNormalDamage, int iMinNormalDamage, int iMaxPentagramDamage, int iMinPentagramDamage, int iGetNormalAccrueDamage, int iGetPentagramAccrueDamage); // idb
	void  CGReqHuntingRecordWndtClose(PMSG_REQ_HUNTING_RECORD_WND_CLOSE *lpMsg, int aIndex); // idb
	void  GDReqHuntingRecordInfo(LPOBJ lpObj, LPOBJ lpTargetObj, char btMapIndex, char btCallType);

private:
	HUNTING_RECORD_EXCEPT_MONSTER_LIST m_HuntingRecordExceptMonsterList[MAX_HUNTING_RECORD_EXCEPT_MONSTER_LIST];
	HUNTING_RECORD_MAP_LIST m_HuntingRecordMapList[MAX_HUNTING_RECORD_MAP_LIST];
};
extern CGHuntingGrounds gCGHuntingGrounds;