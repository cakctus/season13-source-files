#pragma once

struct HUNTING_RECORD_INFO
{
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

struct SDHP_REQ_HUNTING_RECORD_INFO
{
	PSBMSG_HEAD h;
	char AccountId[11];
	char szName[11];
	int iUserIndex;
	BYTE btMapIndex;
	BYTE btCallType;
	BYTE btAnotherUser;
};

struct SDHP_ANS_HUNTING_RECORD_COUNT
{
	PSWMSG_HEAD h;

	BYTE btResult;
	BYTE btListCnt;
	BYTE btMapIndex;
	BYTE btCallType;
	BYTE btAnotherUser;
	int iUserIndex;
};
struct SDHP_REQ_HUNTING_RECORD_INFO_SAVE
{
	PSWMSG_HEAD h;
	char btListCnt;
	char AccountId[11];
	char szName[11];
};
struct SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN
{
	PSBMSG_HEAD h;
	char AccountId[11];
	char szName[11];
	int iUserIndex;
};
struct  SDHP_ANS_HUNTING_RECORD_INFO_USER_OPEN
{
	PSBMSG_HEAD h;
	BYTE btResult;
	int iUserIndex;
	BYTE btOpen;
};
struct SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN_SAVE
{
	PSBMSG_HEAD h;
	char AccountId[11];
	char szName[11];
	int iUserIndex;
	BYTE btOpen;
};
struct  SDHP_REQ_HUNTING_RECORD_INFO_CURRENT
{
	PSBMSG_HEAD h;
	char AccountId[11];
	char szName[11];
	int iUserIndex;
	BYTE btMapIndex;
	int iYear;
	BYTE btMonth;
	BYTE btDay;
	BYTE btCallType;
};
struct  SDHP_ANS_HUNTING_RECORD_INFO_CURRENT
{
	PSWMSG_HEAD h;
	int iUserIndex;
	BYTE btCallType;
	BYTE btListCnt;
	BYTE btMapIndex;
	int iYear;
	char btMonth;
	char btDay;
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
struct  SDHP_REQ_DELETE_HUNTING_RECORD_INFO
{
	PSBMSG_HEAD h;
	char AccountId[11];
	char szName[11];
	int iUserIndex;
	BYTE btMapIndex;
	int iYear;
	BYTE btMonth;
	BYTE btDay;
};
class HuntingRecord
{
public:
	HuntingRecord();
	virtual ~HuntingRecord();
	int LoadHuntingRecordInfo( char *szAccountID, char *Name, HUNTING_RECORD_INFO *pHuntingRecordInfo, SDHP_ANS_HUNTING_RECORD_COUNT *pMsg, int iMapIndex); // idb
	int HuntingRecordInfoSave(SDHP_REQ_HUNTING_RECORD_INFO_SAVE *lpRecv); // idb
	int LoadHuntingRecordInfoUserOpen(char *szAccountID, char *Name, SDHP_ANS_HUNTING_RECORD_INFO_USER_OPEN *pMsg); // idb
	int HuntingRecordInfoUserOpenSave(SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN_SAVE *lpRecv); // idb
	int LoadHuntingRecordInfo_Current(char *szAccountID, char *Name, SDHP_ANS_HUNTING_RECORD_INFO_CURRENT *pMsg, int iMapIndex, int iYear, char btMonth, char btDay); // idb
	int DeleteHuntingRecordInfo(char *szAccountID, char *Name, int iMapIndex, int iYear, char btMonth, char btDay); // idb

	void  GDReqHuntingRecordInfo(SDHP_REQ_HUNTING_RECORD_INFO *lpRecv, int aIndex); // idb
	void  GDReqHuntingRecordInfoSave(SDHP_REQ_HUNTING_RECORD_INFO_SAVE *lpRecv);
	void  GDReqHuntingRecordInfoUserOpen(SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN *lpRecv, int aIndex); // idb
	void  GDReqHuntingRecordInfoUserOpenSave(SDHP_REQ_HUNTING_RECORD_INFO_USER_OPEN_SAVE *lpRecv);
	void  GDReqHuntingRecordInfo_Current(SDHP_REQ_HUNTING_RECORD_INFO_CURRENT *lpRecv, int aIndex); // idb
	void  GDReqHuntingRecordInfoDelete(SDHP_REQ_DELETE_HUNTING_RECORD_INFO *lpRecv);
};

extern HuntingRecord gHuntingRecord;