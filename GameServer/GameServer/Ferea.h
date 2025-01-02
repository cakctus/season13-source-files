#ifndef __FEREA_H__
#define __FEREA_H__
#pragma once

#include "protocol.h"

#define MAX_FEREA_BOSS_ZONE_PHASE 5
#define MAX_FEREA_SUMMON_MONSTER 20
#define MAX_FEREA_MINE 5

enum
{
	BOSS_ZONE_STATUS_OPEN = 0x0,
	BOSS_ZONE_STATUS_CLOSE = 0x1,
};

enum
{
	MSG_FEREA_BOSS_ZONE_NONE = 0x0,
	MSG_FEREA_BOSS_ZONE_START = 0x1,
	MSG_FEREA_BOSS_ZONE_FAIL = 0x2,
	MSG_FEREA_BOSS_ZONE_SUCCESS = 0x3,
};

enum
{
	MINE_STATUS_NONE = 0x0,
	MINE_STATUS_BURIED = 0x1,
	MINE_STATUS_EXPLOSION = 0x2,
};

struct _tagFereaBossZoneSumMonInfo
{
	int iGroup;
	int iMonIndex;
	int iMonCnt;
	int iMonAtt;
};

struct _tagFereaBossZoneSumMonPos
{
	int iBeginX;
	int iBeginY;
	int iEndX;
	int iEndY;
};

struct _stMinePos
{
	_stMinePos()
	{
		Clear();
	}

	void Clear()
	{
		m_iIndex = 0;
		m_iPosX = 0;
		m_iPosY = 0;
		m_dwTickCnt = 0;
		m_iSendPosCnt = 0;
		m_iStatus = MINE_STATUS_NONE;
	}

	int m_iIndex;
	int m_iPosX;
	int m_iPosY;
	int m_iSendPosCnt;
	int m_iStatus;
	DWORD m_dwTickCnt;
};

struct _tagPMSG_SEND_MINE_POS
{
	PSBMSG_HEAD h; // C1:F9:10 GS->C
	int m_iPosX;
	int m_iPosY;
	int m_iStatus;
};

struct _tagPMSG_SEND_FEREA_BOSS_ZONE_STATUS
{
	PSBMSG_HEAD h; // C1:F9:11 GS->C
	BYTE btStatus;
};

struct _tagPMSG_PMSG_SEND_SUMMON_FEREA_BOSS
{
	PSBMSG_HEAD h; // C1:F9:12 GS->C
};

class FereaBossZone
{
public:
	FereaBossZone();
	virtual ~FereaBossZone();

	BOOL LoadScriptFereaBossZone(char *lpszFileName);
	void FereaBossZoneInit();

	void Clear();
	void ClearMonster();

	bool IsFereaBossZone(int X, int Y);
	bool IsFereaBossZoneMobSummonArea(int X, int Y);
	bool IsUserFereaBossBattles(LPOBJ lpObj);
	bool IsUserFereaBossBattles(int X, int Y);
	bool IsUserFereaBossBattlesNearTheDoor(int X, int Y);

	void ChkCrystalBallRegen();
	void SetCrystalBallRegenTime(bool bSuccessResult);

	void BlockBossZoneDoor();
	void ReleaseBossZoneDoor();

	void SendBossZoneDoorBlockInfo(bool bLive);
	void SendToUserBossZoneDoorBlockInfo(int nUserIndex);

	void BossZoneDoorFrontUserSendToSafeZone();

	bool ChkTeleportSkillToUse(LPOBJ lpObj, BYTE MapX, BYTE MapY);
	bool ChkTalismanofResurrectionUse(LPOBJ lpObj);
	bool ChkTalismanofMobilityUse(LPOBJ lpObj);

	bool ChkBossZonePlayer();

	void FailBossBattles();

	void FereaBossTeleport(LPOBJ lpObj);
	void SummonMonsterTeleport(LPOBJ lpObj);

	void NotifyBossZoneStatus(int iNotifyNum, int aIndex);
	void SendBossZoneStatus(int aIndex);

	void SetBossZoneStatus(int iStatus);
	int GetBossZoneStatus();

	int AddMonster(int iMonIndex, int iBeginX, int iBeginY, int iEndX, int iEndY, int iMonAttr);

	void CheckCrystalBallLife(LPOBJ lpObj);
	int CheckLifeStatus(LPOBJ lpObj);

	void SummonMonster(int nGroupNumber, int iGenMonCnt);

	BOOL SetPosMonster(int aIndex, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY);
	BOOL GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, short & mx, short & my);

	void SummonBoss();
	void DieBoss(LPOBJ lpTargetObj);

	bool IsFereaRewardItemBag(int iItemNum, int iMapNum, BYTE btMapX, BYTE btMapY);

	void SendCBRegenTime(LPOBJ lpObj);

private:
	int m_iCBHpPhase[MAX_FEREA_BOSS_ZONE_PHASE];
	bool m_bCBSummon[MAX_FEREA_BOSS_ZONE_PHASE];

	_tagFereaBossZoneSumMonInfo stSumMonInfo[20];
	_tagFereaBossZoneSumMonPos m_stSumMonPos;

	int m_iSumMonCnt;
	int m_iBossZoneStatus;

	bool m_bChkRegenCB;
	long m_lRegenCBTime;
};

class FereaBoss
{
public:
	FereaBoss();
	virtual ~FereaBoss();

	int CheckLifeStatus(LPOBJ lpObj);
	void CheckFereaBossLife(LPOBJ lpObj);

	bool FereaBossSkillRun(LPOBJ lpObj);

	int GetAttackSkillCount(LPOBJ lpObj);
	void Attack(LPOBJ lpObj);

	void SetSkillUsed(bool bSkillUsed) { m_bSkillUsed = bSkillUsed; }

	void MonsterRegen(LPOBJ lpObj);

	void Clear();

	void InitBuriedLandMines();
	bool BuriedLandMines(LPOBJ lpObj);

	void ChkTimeExplosion();
	void MineExplosion(int nMineNum);

	void SendMinePos(int aIndex, int x, int y, int iStatus);
	void NotifyMinePos();

	bool SkillMineRun(LPOBJ lpObj);

	void ClearMinePosInfo();

	void Whirlwind(LPOBJ lpObj, int iTargetIndex);
	void RoarofLord(LPOBJ lpObj);

	void SetRoarofLord(bool bRoarofLord);
	bool IsRoarofLord();

	void SummonDarkKnight(int aIndex);
	void DieDarkKnight();

	BOOL Teleport(int iIndex, BYTE btMapX, BYTE btMapY);

	void RefillLife(LPOBJ lpObj);

private:
	bool m_bSkillUsed;
	int m_iCurrentPhase;
	int m_iCurrentSummonPhase;
	_tagFereaBossZoneSumMonInfo m_stSumMonInfo[MAX_FEREA_SUMMON_MONSTER];
	int m_iSummonIndex[MAX_FEREA_SUMMON_MONSTER * 2];
	_stMinePos m_stMinePos[MAX_FEREA_MINE];
	int m_iMineCnt;
	int m_iPosCnt;
	int m_iExplosionCnt;
	bool m_bBuriedLandMine;
	bool m_bRoarofLord;
	DWORD m_dwMineChkTick;
	int m_iRecoverLifeInc;
	int m_iBossIndex;
};

extern int g_iFereaBossRefillLifeValue;
extern int g_iFereaBossWhirlwindDamageVal;
extern int g_iFereaBossWhirlwindDis;
extern int g_iFereaBossLandMineVal;
extern int g_iFereaBossRoarofLordDis;
extern int g_iFereaBossSummonMonMax;

extern FereaBoss g_FereaBoss;
extern FereaBossZone g_FereaBossZone;
#endif
