#pragma once

#include "User.h"
#include "Protocol.h"
#include "DSProtocol.h"

#define OFFLINE_MODE_RESTORE	1
#define OFFLINEMODE_DBPRICE		544
#define OFFLINEMODE_DBCREDIT	128

struct PMSG_OFFMODE_START_RECV
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Password[21];
	char Name[11];
	DWORD TickCount;
};

struct PMSG_OFFMODE_CHAR
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short Number;
};

struct PMSG_OFFMODE_DATA
{
	PBMSG_HEAD h;
	short Number;
	BYTE Attack;
	BYTE Ghost;
	BYTE PShopOpen;
	char PShopText[36];
};

struct PMSG_OFFMODE_DATA_SAVE
{
	PSBMSG_HEAD h;
	char Name[11];
	bool Status;
	BYTE Attack;
	BYTE Ghost;
	BYTE PShopOpen;
	char PShopText[36];
};

class CCustomAttack
{
public:
	CCustomAttack();
	virtual ~CCustomAttack();
	void ReadCustomAttackInfo(char* section,char* path);
	void CommandCustomAttack(LPOBJ lpObj,char* arg);
	void CommandCustomAttackOffline(LPOBJ lpObj,char* arg);
	bool GetAttackSkill(LPOBJ lpObj,int* SkillNumber);
	bool GetTargetMonster(LPOBJ lpObj,int SkillNumber,int* MonsterIndex);
	bool CheckRequireMoney(LPOBJ lpObj);
	void OnAttackClose(LPOBJ lpObj, int type);
	void OnAttackSecondProc(LPOBJ lpObj);
	void OnAttackAlreadyConnected(LPOBJ lpObj);
	void OnAttackMonsterAndMsgProc(LPOBJ lpObj);
	void SendSkillAttack(LPOBJ lpObj,int aIndex,int SkillNumber);
	void SendMultiSkillAttack(LPOBJ lpObj,int aIndex,int SkillNumber);
	void SendDurationSkillAttack(LPOBJ lpObj,int aIndex,int SkillNumber);
	void FilaDurationSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber);
	void FilaPvPTest();
	void UseAllPotionAuto();
	void UsePotionAuto(LPOBJ lpObj);
	short AddIndexPlayer();
	void GDReqAllPlayer();
	void DGOfflineAccountRecv(PMSG_OFFMODE_START_RECV* Data);
	void GDReqOffCharacter(int aIndex);
	void DGAnsOffCharacter(SDHP_CHARACTER_INFO_RECV* lpMsg);
	void DGPSelectData(PMSG_OFFMODE_DATA* lpMsg);
	void GDUpdateData(int aIndex);
	void GDReqUpdateStatus(int aIndex);
public:
	int m_CustomAttackSwitch;
	int m_CustomAttackEnable[4];
	int m_CustomAttackRequireLevel[4];
	int m_CustomAttackRequireReset[4];
	char m_CustomAttackCommandSyntax[32];
	char m_CustomAttackText1[128];
	char m_CustomAttackText2[128];
	char m_CustomAttackText3[128];
	char m_CustomAttackText4[128];
	char m_CustomAttackText5[128];
	char m_CustomAttackText6[128];
	int m_CustomAttackOfflineSwitch;
	int m_CustomAttackOfflineGPGain;
	int m_CustomAttackOfflineEnable[4];
	int m_CustomAttackOfflineRequireLevel[4];
	int m_CustomAttackOfflineRequireReset[4];
	int m_CustomAttackOfflineRequireMoney[4];
	char m_CustomAttackOfflineCommandSyntax[32];
	char m_CustomAttackOfflineText1[128];
	char m_CustomAttackOfflineText2[128];
	char m_CustomAttackOfflineText3[128];
	char m_CustomAttackOfflineText4[128];
	char m_CustomAttackOfflineText5[128];
	char m_CustomAttackOfflineText6[128];
	bool OfflineRestored;
};

extern CCustomAttack gCustomAttack;
