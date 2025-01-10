#include "StdAfx.h"
//#include "..\Include\ReadScript.h"
//#include "..\common\winutil.h"
#include "Ferea.h"
#include "Notice.h"
#include "user.h"
#include "Gamemain.h"
//#include "InstanceSystem.h"
#include "MonsterSkillManager.h"
#include "Effect.h"
#include "protocol.h"

FereaBoss g_FereaBoss;
FereaBossZone g_FereaBossZone;

const BYTE g_btFereaBossZoneBlockMapXY[5][4] =
{
	0x31, 0x79, 0x34, 0x7C,
	0x35, 0x78, 0x39, 0x7D,
	0x3A, 0x78, 0x3C, 0x7C,
	0x3D, 0x77, 0x40, 0x7E,
	0x3C, 0x7D, 0x4C, 0x7D,
};

const BYTE g_btFereaBossZoneMapXY[4] =
{
	0x14, 0x6A, 0x30, 0x8C,
};

const BYTE g_btFereaBossZoneMobSummonMapXY[4] =
{
	0x17, 0x6A, 0x30, 0x89,
};

const BYTE g_btFereaBossZoneDoorFrontMapXY[4] =
{
	0x31, 0x77, 0x40, 0x7E,
};

int g_iFereaBossRefillLifeValue;
int g_iFereaBossWhirlwindDamageVal;
int g_iFereaBossWhirlwindDis;
int g_iFereaBossLandMineVal;
int g_iFereaBossRoarofLordDis;
int g_iFereaBossSummonMonMax;

FereaBossZone::FereaBossZone()
{
	for (int i = 0; i < MAX_FEREA_BOSS_ZONE_PHASE; i++)
	{
		m_bCBSummon[i] = FALSE;
	}

	m_iSumMonCnt = 0;
	m_iBossZoneStatus = 0;
	m_bChkRegenCB = FALSE;
}

FereaBossZone::~FereaBossZone()
{
}

void FereaBossZone::NotifyBossZoneStatus(int iNotifyNum, int aIndex)
{
	if (iNotifyNum == MSG_FEREA_BOSS_ZONE_START)
	{
		PMSG_NOTICE pMsg;

		TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3596));
		TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(255, 255, 200, 80), 1, 0, 20);
		TNotice::SendNoticeToAllUser(&pMsg);
		LogAddEvent("FEREA BOSS ZONE:Ferea Start", 0);
	}

	else if (iNotifyNum == MSG_FEREA_BOSS_ZONE_FAIL)
	{
		PMSG_NOTICE pMsg;

		TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3597));
		TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(255, 255, 200, 80), 1, 0, 20);
		TNotice::SendNoticeToAllUser(&pMsg);
		LogAddEvent("FEREA BOSS ZONE:Ferea Fail", 0);
	}

	else if (iNotifyNum == MSG_FEREA_BOSS_ZONE_SUCCESS)
	{
		PMSG_NOTICE pMsg;

		TNotice::MakeNoticeMsgEx(&pMsg, 0, lMsg.Get(3598), gObj[aIndex].Name);
		TNotice::SetNoticeProperty(&pMsg, 0, _ARGB(255, 255, 200, 80), 1, 0, 20);
		TNotice::SendNoticeToAllUser(&pMsg);
		LogAddEvent("FEREA BOSS ZONE:Ferea Success", 0);
	}
}

void FereaBossZone::SendBossZoneStatus(int aIndex)
{
	BYTE btStatus = m_iBossZoneStatus;

	_tagPMSG_SEND_FEREA_BOSS_ZONE_STATUS pMsg;
	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF9;
	pMsg.h.subcode = 0x11;
	pMsg.btStatus = btStatus;

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}

void FereaBossZone::SetBossZoneStatus(int iStatus)
{
	m_iBossZoneStatus = iStatus;

	for (int n = ALLOC_USEROBJECTSTART; n < MAX_OBJECT; n++)
	{
		if (gObj[n].Connected == CSS_GAMEPLAYING && gObj[n].Type == OBJTYPE_CHARACTER && gObj[n].MapNumber == MAP_INDEX_FEREA)
		{
			SendBossZoneStatus(n);
		}
	}
}

BOOL FereaBossZone::LoadScriptFereaBossZone(char *lpszFileName)
{
	if (lpszFileName == NULL || strcmp(lpszFileName, "") == 0)
	{
		MsgBox("[FereaBossZone] file load error - File Name Error");
		return FALSE;
	}

	if ((SMDFile = fopen(lpszFileName, "r")) == NULL)
	{
		MsgBox("[FereaBossZone] file load error - fopen() : %s", lpszFileName);
		return FALSE;
	}

	int type = -1;
	SMDToken Token;

	while (true)
	{
		Token = GetToken();

		if (Token == END)
		{
			break;
		}

		type = TokenNumber;

		if (Token == NUMBER)
		{
			if (type == 1)
			{
				int nPhaseCnt = 0;

				while (true)
				{
					Token = GetToken();

					if (Token == NAME && strcmp("end", TokenString) == 0)
					{
						break;
					}

					int nHp = TokenNumber;

					if (nPhaseCnt >= MAX_FEREA_BOSS_ZONE_PHASE)
					{
						MsgBox("[FereaBossZone] Out of Max type 1");
						break;
					}

					m_iCBHpPhase[nPhaseCnt] = nHp;
					nPhaseCnt++;
				}
			}

			else if (type == 2)
			{
				while (true)
				{
					Token = GetToken();

					if (Token == NAME && strcmp("end", TokenString) == 0)
					{
						break;
					}

					m_stSumMonPos.iBeginX = TokenNumber;
					Token = GetToken(); m_stSumMonPos.iBeginY = TokenNumber;
					Token = GetToken(); m_stSumMonPos.iEndX = TokenNumber;
					Token = GetToken(); m_stSumMonPos.iEndY = TokenNumber;
				}
			}

			else if (type == 3)
			{
				int nArrayIndex = 0;

				while (true)
				{
					Token = GetToken();

					if (Token == NAME && strcmp("end", TokenString) == 0)
					{
						break;
					}

					int nGroupNumber = TokenNumber;

					if (nGroupNumber < 0)
					{
						MsgBox("[FereaBossZone]\tInvalid monster groupNumber");
						return FALSE;
					}

					if (nArrayIndex >= MAX_FEREA_SUMMON_MONSTER)
					{
						MsgBox("[FereaBossZone] Out of Max type 2");
						break;
					}

					stSumMonInfo[nArrayIndex].iGroup = nGroupNumber;
					Token = GetToken(); stSumMonInfo[nArrayIndex].iMonIndex = TokenNumber;
					Token = GetToken(); stSumMonInfo[nArrayIndex].iMonCnt = TokenNumber;
					Token = GetToken(); stSumMonInfo[nArrayIndex].iMonAtt = TokenNumber;
					nArrayIndex++;
				}
			}

			else if (type == 4)
			{
				while (true)
				{
					Token = GetToken();

					if (Token == NAME && strcmp("end", TokenString) == 0)
					{
						break;
					}

					g_iFereaBossRefillLifeValue = TokenNumber;
				}
			}

			else if (type == 5)
			{
				while (true)
				{
					Token = GetToken();

					if (Token == NAME && strcmp("end", TokenString) == 0)
					{
						break;
					}

					g_iFereaBossWhirlwindDamageVal = TokenNumber;
					Token = GetToken(); g_iFereaBossWhirlwindDis = TokenNumber;
					Token = GetToken(); g_iFereaBossLandMineVal = TokenNumber;
					Token = GetToken(); g_iFereaBossRoarofLordDis = TokenNumber;
				}
			}

			else if (type == 6)
			{
				while (true)
				{
					Token = GetToken();

					if (Token == NAME && strcmp("end", TokenString) == 0)
					{
						break;
					}

					g_iFereaBossSummonMonMax = TokenNumber;
				}
			}
		}
	}

	fclose(SMDFile);
	return TRUE;
}

void FereaBossZone::ChkCrystalBallRegen()
{
	if (g_FereaBossZoneEventOnOff == FALSE)
	{
		return;
	}

	if (g_bIsArcaBattleServer == TRUE)
	{
		return;
	}

	if (g_bIsUnityBattleFieldServer == TRUE)
	{
		return;
	}

	if (g_bIsMarketServer == TRUE)
	{
		return;
	}

	if (m_bChkRegenCB == false)
	{
		return;
	}

	CTime tCurrentTime = CTime::GetTickCount();
	__int64 tCurrent = tCurrentTime.GetTime();

	CString sTime = tCurrentTime.Format("%Y-%m-%d %p%I:%M");

	if (tCurrent > m_lRegenCBTime)
	{
		m_lRegenCBTime = 0;
		FereaBossZoneInit();
		LogAddTD("[FereaBossZone][ChkCrystalBallRegen] %s", sTime);
	}
}

void FereaBossZone::SetCrystalBallRegenTime(bool bSuccessResult)
{
	CTime tExpireTime;
	CTime tCurrentTime = CTime::GetTickCount();

	int nRandHour = 0;
	int nMin = 0;
	
	if (bSuccessResult == TRUE)
	{
		nRandHour = rand() % 12 + 12;
	}

	else
	{
		nMin = 5;
	}

	CTimeSpan tLeftTime(0, nRandHour, nMin, 0);
	tExpireTime = tCurrentTime + tLeftTime;

	m_lRegenCBTime = tExpireTime.GetTime();
	m_bChkRegenCB = true;

	__int64 tCurTime = tCurrentTime.GetTime();

	CTime tRegenTime(m_lRegenCBTime);
	CString sTime = tRegenTime.Format("%Y-%m-%d %p%I:%M");

	LogAddTD("[FereaBossZone][CrystalBallRegenTime] %s", sTime);
}

void FereaBossZone::FereaBossZoneInit()
{
	if (g_FereaBossZoneEventOnOff == FALSE)
	{
		return;
	}

	if (g_bIsArcaBattleServer == TRUE)
	{
		return;
	}

	if (g_bIsUnityBattleFieldServer == TRUE)
	{
		return;
	}

	if (g_bIsMarketServer == TRUE)
	{
		return;
	}

	if (IsInstanceSystemServer() == TRUE)
	{
		return;
	}

	if (AddMonster(735, 24, 123, 25, 124, 0) < 0)
	{
		LogAddTD("[FereaBossZone][CrystalBallRegen] FAIL!!!");
		return;
	}

	NotifyBossZoneStatus(MSG_FEREA_BOSS_ZONE_START, 0);
	LogAddTD("[FereaBossZone][CrystalBallRegen] Create");
	m_bChkRegenCB = false;
}

void FereaBossZone::Clear()
{
	ClearMonster();

	for (int i = 0; i < MAX_FEREA_BOSS_ZONE_PHASE; i++)
	{
		m_bCBSummon[i] = false;
	}

	m_iSumMonCnt = 0;
	m_iBossZoneStatus = BOSS_ZONE_STATUS_OPEN;
	m_bChkRegenCB = false;

	g_FereaBoss.Clear();
}

void FereaBossZone::ClearMonster()
{
	for (int i = 0; i < MAX_MONSTER; i++)
	{
		LPOBJ lpTargetObj = &gObj[i];

		if (lpTargetObj->MapNumber == MAP_FEREA && lpTargetObj->Connected == CSS_GAMEPLAYING && lpTargetObj->Type == OBJTYPE_MONSTER)
		{
			if (IsFereaBossZone(lpTargetObj->X, lpTargetObj->Y) == true)
			{
				gObjDel(lpTargetObj->m_Index);
			}
		}
	}
}

bool FereaBossZone::IsFereaBossZone(int X, int Y)
{
	return X >= g_btFereaBossZoneMapXY[0] && X <= g_btFereaBossZoneMapXY[2] && Y >= g_btFereaBossZoneMapXY[1] && Y <= g_btFereaBossZoneMapXY[3];
}

bool FereaBossZone::IsFereaBossZoneMobSummonArea(int X, int Y)
{
	return X >= g_btFereaBossZoneMobSummonMapXY[0] && X <= g_btFereaBossZoneMobSummonMapXY[2] && Y >= g_btFereaBossZoneMobSummonMapXY[1] && Y <= g_btFereaBossZoneMobSummonMapXY[3];
}

bool FereaBossZone::IsUserFereaBossBattles(int X, int Y)
{
	return X >= g_btFereaBossZoneMapXY[0] && X <= g_btFereaBossZoneMapXY[2] && Y >= g_btFereaBossZoneMapXY[1] && Y <= g_btFereaBossZoneMapXY[3] && m_iBossZoneStatus == BOSS_ZONE_STATUS_CLOSE;
}

bool FereaBossZone::IsUserFereaBossBattles(LPOBJ lpObj)
{
	int X = lpObj->X;
	int Y = lpObj->Y;

	if (gObjIsConnected(lpObj->m_Index) == FALSE)
	{
		return false;
	}

	if (lpObj->MapNumber != MAP_INDEX_FEREA)
	{
		return false;
	}

	return IsUserFereaBossBattles(X, Y);
}

bool FereaBossZone::IsUserFereaBossBattlesNearTheDoor(int X, int Y)
{
	return X >= g_btFereaBossZoneDoorFrontMapXY[0] && X <= g_btFereaBossZoneDoorFrontMapXY[2] && Y >= g_btFereaBossZoneDoorFrontMapXY[1] && Y <= g_btFereaBossZoneDoorFrontMapXY[3] && m_iBossZoneStatus == BOSS_ZONE_STATUS_CLOSE;
}

void FereaBossZone::BlockBossZoneDoor()
{
	for (int n = 0; n < 5; n++)
	{
		for (int i = g_btFereaBossZoneBlockMapXY[n][0]; i <= g_btFereaBossZoneBlockMapXY[n][2]; i++)
		{
			for (int j = g_btFereaBossZoneBlockMapXY[n][1]; j <= g_btFereaBossZoneBlockMapXY[n][3]; j++)
			{
				MapC[MAP_INDEX_FEREA].m_attrbuf[j * MAX_TERRAIN_SIZE + i] |= 0x04;
			}
		}
	}

	SendBossZoneDoorBlockInfo(true);
	BossZoneDoorFrontUserSendToSafeZone();
}

void FereaBossZone::ReleaseBossZoneDoor()
{
	for (int n = 0; n < 5; n++)
	{
		for (int i = g_btFereaBossZoneBlockMapXY[n][0]; i <= g_btFereaBossZoneBlockMapXY[n][2]; i++)
		{
			for (int j = g_btFereaBossZoneBlockMapXY[n][1]; j <= g_btFereaBossZoneBlockMapXY[n][3]; j++)
			{
				MapC[MAP_INDEX_FEREA].m_attrbuf[j * MAX_TERRAIN_SIZE + i] &= ~0x04;
			}
		}
	}

	SendBossZoneDoorBlockInfo(false);
}

void FereaBossZone::SendBossZoneDoorBlockInfo(bool bLive)
{
	char cTEMP_BUF[256];
	LPPMSG_SETMAPATTR_COUNT lpMsg = (LPPMSG_SETMAPATTR_COUNT)cTEMP_BUF;

	PHeadSetB((LPBYTE)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT) + sizeof(PMSG_SETMAPATTR) * 10);
	LPPMSG_SETMAPATTR lpMsgBody = (LPPMSG_SETMAPATTR)&cTEMP_BUF[7];

	lpMsg->btType = 0;
	lpMsg->btCount = 5;
	lpMsg->btMapAttr = 4;
	(bLive) ? (lpMsg->btMapSetType = 0) : (lpMsg->btMapSetType = 1);

	int nIndex = 0;

	for (int n = 0; n < 5; n++)
	{
		lpMsgBody[nIndex].btX = g_btFereaBossZoneBlockMapXY[n][0];
		lpMsgBody[nIndex].btY = g_btFereaBossZoneBlockMapXY[n][1];
		nIndex += 1;

		lpMsgBody[nIndex].btX = g_btFereaBossZoneBlockMapXY[n][2];
		lpMsgBody[nIndex].btY = g_btFereaBossZoneBlockMapXY[n][3];
		nIndex += 1;
	}

	for (int aIndex = ALLOC_USEROBJECTSTART; aIndex < MAX_OBJECT; aIndex++)
	{
		if (gObj[aIndex].MapNumber == MAP_INDEX_FEREA)
		{
			if (gObj[aIndex].Connected > CSS_CHARACTERVIEW)
			{
				DataSend(aIndex, (LPBYTE)cTEMP_BUF, cTEMP_BUF[1]);
			}
		}
	}
}

void FereaBossZone::SendToUserBossZoneDoorBlockInfo(int nUserIndex)
{
	if (m_iBossZoneStatus == BOSS_ZONE_STATUS_OPEN)
	{
		return;
	}

	bool bLive = false;

	char cTEMP_BUF[256];
	LPPMSG_SETMAPATTR_COUNT lpMsg = (LPPMSG_SETMAPATTR_COUNT)cTEMP_BUF;

	PHeadSetB((LPBYTE)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT) + sizeof(PMSG_SETMAPATTR) * 10);
	LPPMSG_SETMAPATTR lpMsgBody = (LPPMSG_SETMAPATTR)&cTEMP_BUF[7];

	lpMsg->btType = 0;
	lpMsg->btCount = 5;
	lpMsg->btMapAttr = 4;

	if (m_iBossZoneStatus == BOSS_ZONE_STATUS_CLOSE)
	{
		bLive = true;
	}

	(bLive) ? (lpMsg->btMapSetType = 0) : (lpMsg->btMapSetType = 1);

	int nIndex = 0;

	for (int n = 0; n < 5; n++)
	{
		lpMsgBody[nIndex].btX = g_btFereaBossZoneBlockMapXY[n][0];
		lpMsgBody[nIndex].btY = g_btFereaBossZoneBlockMapXY[n][1];
		nIndex += 1;

		lpMsgBody[nIndex].btX = g_btFereaBossZoneBlockMapXY[n][2];
		lpMsgBody[nIndex].btY = g_btFereaBossZoneBlockMapXY[n][3];
		nIndex += 1;
	}

	if (gObj[nUserIndex].Connected > CSS_CHARACTERVIEW)
	{
		DataSend(nUserIndex, (LPBYTE)cTEMP_BUF, cTEMP_BUF[1]);
	}
}

void FereaBossZone::BossZoneDoorFrontUserSendToSafeZone()
{
	for (int nIndex = ALLOC_USEROBJECTSTART; nIndex < MAX_OBJECT; nIndex++)
	{
		if (gObj[nIndex].MapNumber == MAP_INDEX_FEREA && gObj[nIndex].Connected > CSS_CHARACTERVIEW)
		{
			int X = gObj[nIndex].X;
			int Y = gObj[nIndex].Y;

			if (X >= g_btFereaBossZoneDoorFrontMapXY[0] && X <= g_btFereaBossZoneDoorFrontMapXY[2] &&
				Y >= g_btFereaBossZoneDoorFrontMapXY[1] && Y <= g_btFereaBossZoneDoorFrontMapXY[3])
			{
				gObjMoveGate(nIndex, 509);
			}
		}
	}
}

bool FereaBossZone::ChkBossZonePlayer()
{
	for (int nIndex = ALLOC_USEROBJECTSTART; nIndex < MAX_OBJECT; nIndex++)
	{
		if (gObj[nIndex].MapNumber == MAP_INDEX_FEREA && gObjIsConnected(nIndex) == TRUE)
		{
			if (IsFereaBossZone(gObj[nIndex].X, gObj[nIndex].Y) == true)
			{
				return true;
			}
		}
	}

	return false;
}

void FereaBossZone::FailBossBattles()
{
	Clear();
	SetBossZoneStatus(BOSS_ZONE_STATUS_OPEN);
	ReleaseBossZoneDoor();
	SetCrystalBallRegenTime(false);
	NotifyBossZoneStatus(MSG_FEREA_BOSS_ZONE_FAIL, 0);
}

void FereaBossZone::FereaBossTeleport(LPOBJ lpObj)
{
	int iMapNum = MAP_INDEX_FEREA;
	int nUserCnt = 0;
	int ArrIndex[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int nUserIndex = 0;

	if (lpObj->m_State != OBJST_PLAYING)
	{
		return;
	}

	if (lpObj->Teleport != 0)
	{
		return;
	}

	for (int nIndex = ALLOC_USEROBJECTSTART; nIndex < MAX_OBJECT; nIndex++)
	{
		if (gObj[nIndex].MapNumber == iMapNum && gObjIsConnected(nIndex) == TRUE)
		{
			if (IsFereaBossZone(gObj[nIndex].X, gObj[nIndex].Y) == true)
			{
				if (nUserCnt >= 10)
				{
					break;
				}

				ArrIndex[nUserCnt] = nIndex;
				nUserCnt++;
			}
		}
	}

	if (nUserCnt > 0)
	{
		int nRand = GetLargeRand() % nUserCnt;

		if (nRand >= 10)
		{
			nRand = 9;
		}

		nUserIndex = ArrIndex[nRand];

		short nMapX = 0;
		short nMapY = 0;

		if (gObj[nUserIndex].MapNumber == iMapNum && gObjIsConnected(nUserIndex) == TRUE)
		{
			if (GetBoxPosition(iMapNum, gObj[nUserIndex].X - 4, gObj[nUserIndex].Y - 4, gObj[nUserIndex].X + 4, gObj[nUserIndex].Y + 4, nMapX, nMapY) != FALSE)
			{
				g_FereaBoss.Teleport(lpObj->m_Index, nMapX, nMapY);
				SummonMonsterTeleport(&gObj[nUserIndex]);
			}
		}
	}
}

void FereaBossZone::SummonMonsterTeleport(LPOBJ lpObj)
{
	int nMapNumber = MAP_INDEX_FEREA;
	int nMonCnt = 0;
	int nDis = 5;
	int nBeginX = lpObj->X - 5;
	int nBeginY = lpObj->Y - 5;
	int nEndX = lpObj->X + 5;
	int nEndY = lpObj->Y + 5;

	for (int nMonIdx = 0; nMonIdx < MAX_MONSTER; nMonIdx++)
	{
		if (gObj[nMonIdx].MapNumber == nMapNumber && gObj[nMonIdx].Connected == CSS_GAMEPLAYING && gObj[nMonIdx].Type == OBJTYPE_MONSTER)
		{
			if (IsFereaBossZone(gObj[nMonIdx].X, gObj[nMonIdx].Y) == true)
			{
				if (gObj[nMonIdx].Class >= 736 && gObj[nMonIdx].Class <= 738)
				{
					if (gObj[nMonIdx].Live == TRUE)
					{
						short nMapX = 0;
						short nMapY = 0;

						if (GetBoxPosition(nMapNumber, nBeginX, nBeginY, nEndX, nEndY, nMapX, nMapY) != FALSE)
						{
							g_FereaBoss.Teleport(nMonIdx, nMapX, nMapY);
						}

						nMonCnt++;
					}
				}
			}
		}
	}

	if (nMonCnt > g_iFereaBossSummonMonMax)
	{
		LogAddTD("[FereaBoss][Error][MonsterRegen] over max - nMonCnt %d ", nMonCnt);
		LogAddTD("[FereaBoss][Error][MonsterRegen] error : %s %d", __FILE__, __LINE__);
		return;
	}

	for (int i = nMonCnt; i < g_iFereaBossSummonMonMax; i++)
	{
		int nRegenMonIndex = rand() % 3 + 736;
		int nIndex = AddMonster(nRegenMonIndex, nBeginX, nBeginY, nEndX, nEndY, 0);

		if (nIndex < 0)
		{
			LogAddTD("[FereaBossZone][SummonMon] AddMonster fail!! [nIndex]:%d / %s %d", nIndex, __FILE__, __LINE__);
			return;
		}
	}
}

bool FereaBossZone::ChkTeleportSkillToUse(LPOBJ lpObj, BYTE MapX, BYTE MapY)
{
	if (gObjIsConnected(lpObj->m_Index) == FALSE)
	{
		return false;
	}

	if (m_iBossZoneStatus == BOSS_ZONE_STATUS_OPEN)
	{
		return true;
	}

	if (IsUserFereaBossBattles(lpObj->X, lpObj->Y) == true)
	{
		if (IsUserFereaBossBattles(MapX, MapY) == true)
		{
			return true;
		}
	}

	else
	{
		if (IsUserFereaBossBattles(MapX, MapY) == false)
		{
			return true;
		}
	}

	return false;
}

bool FereaBossZone::ChkTalismanofResurrectionUse(LPOBJ lpObj)
{
	if (gObjIsConnected(lpObj->m_Index) == FALSE)
	{
		return false;
	}

	if (m_iBossZoneStatus == BOSS_ZONE_STATUS_OPEN)
	{
		return true;
	}

	if (lpObj->m_siDieRegenMapIndex != MAP_INDEX_FEREA)
	{
		return true;
	}

	if (IsUserFereaBossBattles(lpObj->m_siDieRegenX, lpObj->m_siDieRegenY) == true)
	{
		return false;
	}

	if (IsUserFereaBossBattlesNearTheDoor(lpObj->m_siDieRegenX, lpObj->m_siDieRegenY) == true)
	{
		return false;
	}

	return true;
}

bool FereaBossZone::ChkTalismanofMobilityUse(LPOBJ lpObj)
{
	if (gObjIsConnected(lpObj->m_Index) == FALSE)
	{
		return false;
	}

	if (m_iBossZoneStatus == BOSS_ZONE_STATUS_OPEN)
	{
		return true;
	}

	if (lpObj->MapNumber == MAP_INDEX_FEREA && IsUserFereaBossBattles(lpObj->X, lpObj->Y) == true)
	{
		return false;
	}

	if (lpObj->m_siMoveMapIndex != MAP_INDEX_FEREA)
	{
		return true;
	}

	if (IsUserFereaBossBattles(lpObj->m_siMoveX, lpObj->m_siMoveY) == true)
	{
		return false;
	}

	if (IsUserFereaBossBattlesNearTheDoor(lpObj->m_siMoveX, lpObj->m_siMoveY) == true)
	{
		return false;
	}

	return true;
}

int FereaBossZone::CheckLifeStatus(LPOBJ lpObj)
{
	return (lpObj->Life / lpObj->MaxLife * 100);
}

void FereaBossZone::CheckCrystalBallLife(LPOBJ lpObj)
{
	int nLifePercentageValue = CheckLifeStatus(lpObj);

	for (int i = 0; i < MAX_FEREA_BOSS_ZONE_PHASE - 1; i++)
	{
		if (m_iCBHpPhase[i] <= nLifePercentageValue)
		{
			continue;
		}

		if (m_iCBHpPhase[i + 1] >= nLifePercentageValue)
		{
			continue;
		}

		if (m_bCBSummon[i] == TRUE)
		{
			return;
		}

		if (m_bCBSummon[i] == FALSE)
		{
			SummonMonster(i, 0);
			m_bCBSummon[i] = TRUE;
			return;
		}
	}
}

void FereaBossZone::SummonMonster(int nGroupNumber, int iGenMonCnt)
{
	for (int i = 0; i < MAX_FEREA_SUMMON_MONSTER; i++)
	{
		if (stSumMonInfo[i].iGroup == nGroupNumber)
		{
			for (int n = 0; n < stSumMonInfo[i].iMonCnt; n++)
			{
				if (m_iSumMonCnt >= (MAX_FEREA_SUMMON_MONSTER * 2))
				{
					LogAddTD("[FereaBossZone][SummonMon] AddMonster fail!! [GROUP]:%d", nGroupNumber);
					return;
				}

				int nIndex = AddMonster(stSumMonInfo[i].iMonIndex, m_stSumMonPos.iBeginX, m_stSumMonPos.iBeginY, m_stSumMonPos.iEndX, m_stSumMonPos.iEndY, stSumMonInfo[i].iMonAtt);
				
				if (nIndex < 0)
				{
					LogAddTD("[FereaBossZone][SummonMon] AddMonster fail!! [nIndex]:%d", nIndex);
					return;			
				}

				m_iSumMonCnt++;
			}
		}
	}
}

BOOL FereaBossZone::SetPosMonster(int aIndex, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY)
{
	if (!CHECK_LIMIT(aIndex, MAX_OBJECT))
	{
		LogAdd("[FereaBossZone][Error] error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	LPOBJ lpObj = &gObj[aIndex];

	lpObj->m_PosNum = -1;
	lpObj->MapNumber = nMapNumber;
	GetBoxPosition(nMapNumber, nBeginX, nBeginY, nEndX, nEndY, lpObj->X, lpObj->Y);
	lpObj->TX = lpObj->X;
	lpObj->TY = lpObj->Y;
	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	lpObj->Dir = rand() % 8;
	lpObj->StartX = lpObj->X;
	lpObj->StartY = lpObj->Y;
	lpObj->DieRegen = FALSE;
	lpObj->MaxRegenTime = 0;

	return TRUE;
}

BOOL FereaBossZone::GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, short & mx, short & my)
{
	int count = 100;
	int w;
	int h;
	int tx;
	int ty;
	BYTE attr;

	while (count-- != 0)
	{
		w = aw - ax;
		h = ah - ay;

		__try
		{
			tx = ax + (GetLargeRand() % w);
			ty = ay + (GetLargeRand() % h);
		}
		__except (w = 1, h = 1, EXCEPTION_EXECUTE_HANDLER)
		{

		}

		attr = MapC[mapnumber].GetAttr(tx, ty);

		if (IsFereaBossZoneMobSummonArea(tx, ty) == true)
		{
			if ((attr & 0x01) != 0x01)
			{
				mx = tx;
				my = ty;
				return TRUE;
			}
		}
	}

	return FALSE;
}

int FereaBossZone::AddMonster(int iMonIndex, int iBeginX, int iBeginY, int iEndX, int iEndY, int iMonAttr)
{
	int nResult = gObjAddMonster(MAP_INDEX_FEREA);

	if (nResult < 0)
	{
		LogAddTD("[FereaBossZone][SummonMon] AddMonster fail!! [INDEX]:%d", iMonIndex);
		return -1;
	}

	if (SetPosMonster(nResult, MAP_INDEX_FEREA, iBeginX, iBeginY, iEndX, iEndY) == FALSE)
	{
		gObjDel(nResult);
		LogAdd("[FereaBossZone][Error] error : %s %d", __FILE__, __LINE__);
		return -1;
	}

	if (gObjSetMonster(nResult, iMonIndex) == FALSE)
	{
		gObjDel(nResult);
		LogAdd("[FereaBossZone][Error] error : %s %d", __FILE__, __LINE__);
		return -1;
	}

	if (iMonAttr > 0)
	{
		gObj[nResult].m_iPentagramMainAttribute = iMonAttr;
	}

	gObj[nResult].m_Attribute = 60;
	LogAddTD("[FereaBossZone][SummonMonster] MonsterIndex: %d/%s, MapNumber: %d, X: %d, Y: %d ", gObj[nResult].Class, gObj[nResult].Name, MAP_INDEX_FEREA, gObj[nResult].X, gObj[nResult].Y);

	return nResult;
}

void FereaBossZone::SummonBoss()
{
	g_FereaBoss.Clear();
	int nBossMonIndex = AddMonster(734, 27, 122, 28, 123, 0);

	if (nBossMonIndex < 0)
	{
		LogAddTD("[FereaBossZone][SummonBossMon] SummonBoss fail!! [INDEX]:%d", nBossMonIndex);
		return;
	}

	gObj[nBossMonIndex].Dir = 3;

	FereaBossZone::SetBossZoneStatus(BOSS_ZONE_STATUS_CLOSE);
	FereaBossZone::BlockBossZoneDoor();
	LogAddTD("[FereaBossZone] SummonBoss");

	_tagPMSG_PMSG_SEND_SUMMON_FEREA_BOSS pMsg;

	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF9;
	pMsg.h.subcode = 0x12;

	for (int n = ALLOC_USEROBJECTSTART; n < MAX_OBJECT; n++)
	{
		LPOBJ lpTargetObj = &gObj[n];

		if (gObjIsConnectedEx(n) == FALSE)
		{
			continue;
		}

		if (lpTargetObj->MapNumber != MAP_INDEX_FEREA)
		{
			continue;
		}

		if (gObjCheckViewport(n, gObj[nBossMonIndex].X, gObj[nBossMonIndex].Y) == TRUE)
		{
			DataSend(n, (LPBYTE)&pMsg, pMsg.h.size);
		}

		if (IsFereaBossZone(lpTargetObj->X, lpTargetObj->Y) == true)
		{
			LogAddEvent("FEREA BOSS ZONE:Start User Info", lpTargetObj,":Class[%d]:Str[%d]:Dex[%d]:Vit[%d]:Ene[%d]:LS[%d]", lpTargetObj->Class,
				lpTargetObj->Strength, lpTargetObj->Dexterity, lpTargetObj->Vitality, lpTargetObj->Energy, lpTargetObj->Leadership);
		}

		if (IsFereaBossZone(lpTargetObj->X, lpTargetObj->Y) == true)
		{
			LogAddTD("[FereaBossZone] FEREA BOSS ZONE:Start User Info %s:%s:Map[%d:%d:%d]:Class[%d]:Str[%d]:Dex[%d]:Vit[%d]:Ene[%d]:LS[%d]", lpTargetObj->AccountID, lpTargetObj->Name,
				lpTargetObj->MapNumber, lpTargetObj->X, lpTargetObj->Y, lpTargetObj->Class, lpTargetObj->Strength, lpTargetObj->Dexterity, lpTargetObj->Vitality, lpTargetObj->Energy, lpTargetObj->Leadership);
		}
	}
}

void FereaBossZone::DieBoss(LPOBJ lpTargetObj)
{
	Clear();
	ReleaseBossZoneDoor();
	SetBossZoneStatus(BOSS_ZONE_STATUS_OPEN);
	NotifyBossZoneStatus(MSG_FEREA_BOSS_ZONE_SUCCESS, lpTargetObj->m_Index);
	SetCrystalBallRegenTime(true);
}

bool FereaBossZone::IsFereaRewardItemBag(int iItemNum, int iMapNum, BYTE btMapX, BYTE btMapY)
{
	if (pFereaBossItemBag154 == NULL)
	{
		LogAddTD("[FereaBossZone] pFereaBossItemBag154 is NULL");
		return false;
	}

	if (iMapNum != MAP_INDEX_FEREA)
	{
		return false;
	}

	if (IsFereaBossZone(btMapX, btMapY) == false)
	{
		return false;
	}

	return pFereaBossItemBag154->IsItemBag(iItemNum);
}

void FereaBossZone::SendCBRegenTime(LPOBJ lpObj)
{
	char szMsg[260] = { 0 };

	CTime tRegenTime(m_lRegenCBTime);
	CString sTime = tRegenTime.Format("%Y-%m-%d %p%I:%M");

	wsprintf(szMsg, "¼öÁ¤±¸ ¸®Á¨ ½Ã°£ %s", sTime);
	GCServerMsgStringSend(szMsg, lpObj->m_Index, 1);
}

FereaBoss::FereaBoss()
{
	Clear();
}

FereaBoss::~FereaBoss()
{
}

void FereaBoss::Clear()
{
	m_iCurrentPhase = 0;
	m_bSkillUsed = false;
	m_iCurrentSummonPhase = 0;

	for (int i = 0; i < (MAX_FEREA_BOSS_ZONE_PHASE * 2); i++)
	{
		m_iSummonIndex[i] = -1;
	}

	m_bBuriedLandMine = false;
	m_iPosCnt = 0;
	m_iMineCnt = 0;
	m_iExplosionCnt = 0;
	m_dwMineChkTick = 0;
	m_bRoarofLord = false;
	m_iRecoverLifeInc = 0;
	m_iBossIndex = 0;

	for (int i = 0; i < MAX_FEREA_MINE; i++)
	{
		m_stMinePos[i].Clear();
	}
}

void FereaBoss::CheckFereaBossLife(LPOBJ lpObj)
{
	int nLifePercentageValue = CheckLifeStatus(lpObj);

	if (nLifePercentageValue <= 0)
	{
		return;
	}

	int nSummonPhase = nLifePercentageValue / 10;

	if (nSummonPhase >= 9)
	{
		return;
	}

	if (m_iCurrentSummonPhase == nSummonPhase)
	{
		return;
	}

	m_iCurrentSummonPhase = nSummonPhase;
	MonsterRegen(lpObj);
}

void FereaBoss::MonsterRegen(LPOBJ lpObj)
{
	if (lpObj == NULL)
	{
		return;
	}

	int nBeginX = lpObj->X - 5;
	int nBeginY = lpObj->Y - 5;
	int nEndX = lpObj->X + 5;
	int nEndY = lpObj->Y + 5;
	int nMonCnt = 0;

	for (int nMonIdx = 0; nMonIdx < MAX_MONSTER; nMonIdx++)
	{
		if (gObj[nMonIdx].MapNumber == MAP_INDEX_FEREA && gObj[nMonIdx].Connected == CSS_GAMEPLAYING && gObj[nMonIdx].Type == OBJTYPE_MONSTER && 
			g_FereaBossZone.IsFereaBossZone(gObj[nMonIdx].X, gObj[nMonIdx].Y) == true &&
			gObj[nMonIdx].Class >= 736 &&  gObj[nMonIdx].Class <= 738 && gObj[nMonIdx].Live == TRUE)
		{
			nMonCnt++;
		}
	}

	if (nMonCnt > g_iFereaBossSummonMonMax)
	{
		LogAddTD("[FereaBoss][Error][MonsterRegen] over max - nMonCnt %d ", nMonCnt);
		LogAddTD("[FereaBoss][Error][MonsterRegen] error : %s %d", __FILE__, __LINE__);
		return;
	}

	for (int i = nMonCnt; i < g_iFereaBossSummonMonMax; i++)
	{
		int nRegenMonIndex = rand() % 3 + 736;
		int nIndex = g_FereaBossZone.AddMonster(nRegenMonIndex, nBeginX, nBeginY, nEndX, nEndY, 0);

		if (nIndex < 0)
		{
			LogAddTD("[FereaBossZone][SummonMon] AddMonster fail!! [nIndex]:%d", nIndex);
			return;
		}
	}
}

bool FereaBoss::FereaBossSkillRun(LPOBJ lpObj)
{
	if (SkillMineRun(lpObj) == true)
	{
		return true;
	}
	
	if (IsRoarofLord() == true)
	{
		return true;
	}

	return false;
}

int FereaBoss::GetAttackSkillCount(LPOBJ lpObj)
{
	int nLifePercentageValue = CheckLifeStatus(lpObj);
	int nAttackPhase = 0;
	int nSkillCnt = 2;
	int nMineCnt = 0;

	if (nLifePercentageValue > 60 && nLifePercentageValue <= 100)
	{
		nAttackPhase = 1;
		nMineCnt = 1;
	}

	else if (nLifePercentageValue > 30 && nLifePercentageValue <= 60)
	{
		nAttackPhase = 2;
		nMineCnt = 2;
	}

	else if (nLifePercentageValue > 20 && nLifePercentageValue <= 30)
	{
		nAttackPhase = 3;
		nMineCnt = 3;

		if (m_bSkillUsed == true)
		{
			nSkillCnt = 3;
		}
	}

	else if (nLifePercentageValue > 10 && nLifePercentageValue <= 20)
	{
		nAttackPhase = 4;
		nMineCnt = 4;

		if (m_bSkillUsed == true)
		{
			nSkillCnt = 3;
		}
	}

	else if (nLifePercentageValue <= 10)
	{
		nAttackPhase = 5;
		nMineCnt = 5;

		if (m_bSkillUsed == true)
		{
			nSkillCnt = 3;
		}
	}

	if (nAttackPhase != m_iCurrentPhase)
	{
		m_iCurrentPhase = nAttackPhase;
		m_bSkillUsed = true;
	}

	m_iMineCnt = nMineCnt;
	return nSkillCnt;
}

void FereaBoss::Attack(LPOBJ lpObj)
{
	if (gObjIsConnected(lpObj->TargetNumber) == FALSE)
	{
		return;
	}

	if (lpObj->TargetNumber < 0)
	{
		return;
	}

	if (g_FereaBossZone.IsFereaBossZone(gObj[lpObj->TargetNumber].X, gObj[lpObj->TargetNumber].Y) == false)
	{
		LogAddTD("[FereaBoss][Attack] Outside FereaBossZone %s/%d", gObj[lpObj->TargetNumber].Name, gObj[lpObj->TargetNumber].m_Index);
		return;
	}

	if (gObjCalDistance(lpObj, &gObj[lpObj->TargetNumber]) <= 3 && rand() % 100 < 40)
	{
		CMagicInf Magic;
		Magic.Set(AT_SKILL_DEFENSEDOWN, 1);

		GCMagicAttackNumberSend(lpObj, AT_SKILL_DEFENSEDOWN, lpObj->TargetNumber, TRUE);
		gObjAttack(lpObj, &gObj[lpObj->TargetNumber], &Magic, TRUE, TRUE, 0, 0, 0, 0, 0);
		GCUseMonsterSkillSend(lpObj, &gObj[lpObj->TargetNumber], 112);
	}

	else
	{
		TMonsterSkillManager::UseMonsterSkill(lpObj->m_Index, lpObj->TargetNumber, 0, -1, 0);
	}
}

int FereaBoss::CheckLifeStatus(LPOBJ lpObj)
{
	return (lpObj->Life / lpObj->MaxLife * 100);
}

bool FereaBoss::SkillMineRun(LPOBJ lpObj)
{
	if (m_bBuriedLandMine == false)
	{
		return false;
	}

	if (GetTickCount() - m_dwMineChkTick >= 1000)
	{
		ChkTimeExplosion();
		BuriedLandMines(lpObj);
		NotifyMinePos();
		m_dwMineChkTick = GetTickCount();
	}

	return true;
}

void FereaBoss::ClearMinePosInfo()
{
	for (int i = 0; i < MAX_FEREA_MINE; i++)
	{
		m_stMinePos[i].Clear();
	}

	m_bBuriedLandMine = false;
	m_iPosCnt = 0;
	m_iMineCnt = 0;
	m_dwMineChkTick = 0;
	m_iExplosionCnt = 0;
}

void FereaBoss::InitBuriedLandMines()
{
	for (int i = 0; i < MAX_FEREA_MINE; i++)
	{
		m_stMinePos[i].Clear();
	}

	m_dwMineChkTick = GetTickCount() + 5000;
	m_bBuriedLandMine = true;
}

bool FereaBoss::BuriedLandMines(LPOBJ lpObj)
{
	if (m_iPosCnt == m_iMineCnt)
	{
		return false;
	}

	if (m_bBuriedLandMine == false)
	{
		return false;
	}

	int tObjNum;
	int count = 0;
	int nTarIndex[MAXVIEWPORTOBJECT] = { 0 };
	int nTarUserCnt = 0;

	while (true)
	{
		if (lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if (CHECK_LIMIT(tObjNum, MAX_OBJECT) != FALSE)
			{
				if (lpObj->VpPlayer2[count].type == OBJTYPE_CHARACTER)
				{
					if (gObjCalDistance(lpObj, &gObj[tObjNum]) <= 9)
					{
						nTarIndex[nTarUserCnt] = tObjNum;
						nTarUserCnt++;
					}
				}
			}
		}

		count++;

		if (count > MAXVIEWPORTOBJECT - 1)
		{
			break;
		}
	}

	short nPosX = 0;
	short nPosY = 0;

	int nTarUserIndex = 0;
	bool bSelTar = false;

	if (nTarUserCnt > (MAXVIEWPORTOBJECT - 1))
	{
		nTarUserCnt = MAXVIEWPORTOBJECT - 1;
	}

	if (nTarUserCnt > 0)
	{
		for (int i = 0; i < nTarUserCnt; i++)
		{
			nTarUserIndex = nTarIndex[rand() % nTarUserCnt];

			if (gObjIsConnected(nTarUserIndex) == TRUE)
			{
				bSelTar = true;
				break;
			}
		}

		if (bSelTar == true)
		{
			nPosX = gObj[nTarUserIndex].X;
			nPosY = gObj[nTarUserIndex].Y;
		}

		if (bSelTar == false || g_FereaBossZone.IsFereaBossZone(nPosX, nPosY) == false || gObjIsConnected(nTarUserIndex) == FALSE)
		{
			if (g_FereaBossZone.GetBoxPosition(MAP_INDEX_FEREA, lpObj->X - 4, lpObj->Y - 4, lpObj->X + 4, lpObj->Y + 4, nPosX, nPosY) == FALSE)
			{
				nPosX = lpObj->X;
				nPosY = lpObj->Y;
				LogAddTD("[FereaBoss][BuriedLandMines] GetBoxPosition #1 FAIL!!! %s %d", __FILE__, __LINE__);
			}

			LogAddTD("[FereaBoss][Error][BuriedLandMines] Error!!! %s/%d Pos: %d/%d bSelTar:%d", gObj[nTarUserIndex].Name, nTarUserIndex, nPosX, nPosY, bSelTar);
		}
	}

	else
	{
		if (g_FereaBossZone.GetBoxPosition(MAP_INDEX_FEREA, lpObj->X - 4, lpObj->Y - 4, lpObj->X + 4, lpObj->Y + 4, nPosX, nPosY) == FALSE)
		{
			nPosX = lpObj->X;
			nPosY = lpObj->Y;
			LogAddTD("[FereaBoss][BuriedLandMines] GetBoxPosition #2 FAIL!!! %s %d", __FILE__, __LINE__);
		}
	}

	m_stMinePos[m_iPosCnt].m_iIndex = lpObj->m_Index;
	m_stMinePos[m_iPosCnt].m_iPosX = nPosX;
	m_stMinePos[m_iPosCnt].m_iPosY = nPosY;
	m_stMinePos[m_iPosCnt].m_iStatus = MINE_STATUS_BURIED;
	m_stMinePos[m_iPosCnt].m_dwTickCnt = GetTickCount();
	m_iPosCnt++;

	if (m_iPosCnt > MAX_FEREA_MINE)
	{
		LogAddTD("[FereaBoss][Error] m_iMineCnt:%d m_iPosCnt:%d", m_iMineCnt, m_iPosCnt);
		LogAddC(LOGC_RED, "[FereaBoss][Error][LandMines] Error!!! over mine count m_iMineCnt:%d m_iPosCnt:%d", m_iMineCnt, m_iPosCnt);
		m_iPosCnt = MAX_FEREA_MINE;
	}

	return TRUE;
}

void FereaBoss::SendMinePos(int aIndex, int x, int y, int iStatus)
{
	_tagPMSG_SEND_MINE_POS pMsg;

	pMsg.h.c = PMHC_BYTE;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF9;
	pMsg.h.subcode = 0x10;
	pMsg.m_iPosX = x;
	pMsg.m_iPosY = y;
	pMsg.m_iStatus = iStatus;

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}

void FereaBoss::ChkTimeExplosion()
{
	if (m_bBuriedLandMine == false)
	{
		return;
	}

	for (int i = 0; i < MAX_FEREA_MINE; i++)
	{
		if (m_stMinePos[i].m_iStatus == MINE_STATUS_BURIED && m_stMinePos[i].m_iSendPosCnt == 3)
		{
			MineExplosion(i);
		}
	}
}

void FereaBoss::MineExplosion(int nMineNum)
{
	if (m_bBuriedLandMine == false)
	{
		return;
	}

	for (int n = ALLOC_USEROBJECTSTART; n < MAX_OBJECT; n++)
	{
		LPOBJ lpTargetObj = &gObj[n];

		if (gObjIsConnected(n) == TRUE && lpTargetObj->MapNumber == MAP_INDEX_FEREA)
		{
			if (gObjCheckViewport(n, m_stMinePos[nMineNum].m_iPosX, m_stMinePos[nMineNum].m_iPosY) == TRUE)
			{
				SendMinePos(n, m_stMinePos[nMineNum].m_iPosX, m_stMinePos[nMineNum].m_iPosY, MINE_STATUS_EXPLOSION);

#if (TESTSERVER == 1)
				char temp[1024] = { 0 };
				wsprintf(temp, "Áö·ÚÆø¹ß MineNum:%d XY:%d/%d ", nMineNum, m_stMinePos[nMineNum].m_iPosX, m_stMinePos[nMineNum].m_iPosY);
				GCServerMsgStringSend(temp, aIndex, 1);
#endif
			}

			OBJECTSTRUCT MineObj;
			int nDamage = g_iFereaBossLandMineVal * lpTargetObj->Life / 100;

			MineObj.X = m_stMinePos[nMineNum].m_iPosX;
			MineObj.Y = m_stMinePos[nMineNum].m_iPosY;

			if (gObjCheckTileArea(lpTargetObj->m_Index, MineObj.X, MineObj.Y, 2) == TRUE)
			{
				gObjAttack(&gObj[m_stMinePos[nMineNum].m_iIndex], lpTargetObj, 0, 0, 0, (g_iFereaBossLandMineVal * lpTargetObj->Life / 100.0), 0, 0, 0, 0);
			}
		}
	}

	m_iExplosionCnt++;
	m_stMinePos[nMineNum].Clear();

	if (m_iExplosionCnt >= m_iMineCnt)
	{
		ClearMinePosInfo();
	}
}

void FereaBoss::NotifyMinePos()
{
	if (m_bBuriedLandMine == false)
	{
		return;
	}

	for (int n = ALLOC_USEROBJECTSTART; n < MAX_OBJECT; n++)
	{
		LPOBJ lpTargetObj = &gObj[n];

		if (gObjIsConnected(n) == TRUE && lpTargetObj->MapNumber == MAP_INDEX_FEREA)
		{
			for (int i = 0; i < MAX_FEREA_MINE; i++)
			{
				if (m_stMinePos[i].m_iStatus == MINE_STATUS_BURIED)
				{
					if (gObjCheckViewport(n, m_stMinePos[i].m_iPosX, m_stMinePos[i].m_iPosY) == TRUE)
					{
						SendMinePos(n, m_stMinePos[i].m_iPosX, m_stMinePos[i].m_iPosY, MINE_STATUS_BURIED);
					}
				}
			}
		}
	}

	for (int i = 0; i < MAX_FEREA_MINE; i++)
	{
		if (m_stMinePos[i].m_iStatus == MINE_STATUS_BURIED)
		{
			m_stMinePos[i].m_iSendPosCnt++;
		}
	}
}

void FereaBoss::Whirlwind(LPOBJ lpObj, int iTargetIndex)
{
	int tObjNum = 0;
	int count = 0;
	int nIndex = iTargetIndex;

	lpObj->MTX = gObj[iTargetIndex].X;
	lpObj->MTY = gObj[nIndex].Y;

	BYTE x = gObj[nIndex].X;
	BYTE y = gObj[nIndex].Y;

	lpObj->m_ActState.Move = 0;

	PMSG_POSISTION_SET pMove;

	pMove.h.c = PMHC_BYTE;
	pMove.h.headcode = 0x10;
	pMove.h.size = sizeof(pMove);
	pMove.X = x;
	pMove.Y = y;

	lpObj->m_Rest = 0;

	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(x, y);

	lpObj->Dir = gObj[nIndex].Dir;
	RecvPositionSetProc(&pMove, lpObj->m_Index);

	while (true)
	{
		if (lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if (CHECK_LIMIT(tObjNum, MAX_OBJECT))
			{
				if (lpObj->VpPlayer2[count].type == OBJTYPE_CHARACTER)
				{
					if (gObjCalDistance(lpObj, &gObj[tObjNum]) <= g_iFereaBossWhirlwindDis)
					{
						int nDamage = g_iFereaBossWhirlwindDamageVal * gObj[tObjNum].Life / 100;
						gObjAttack(lpObj, &gObj[tObjNum], 0, 0, 0, nDamage, 0, 0, 0, 0);
					}
				}
			}
		}

		count++;

		if (count > MAXVIEWPORTOBJECT - 1)
		{
			break;
		}
	}
}

bool FereaBoss::IsRoarofLord()
{
	return m_bRoarofLord;
}

void FereaBoss::SetRoarofLord(bool bRoarofLord)
{
	m_bRoarofLord = bRoarofLord;
}

void FereaBoss::RoarofLord(LPOBJ lpObj)
{
	int tObjNum;
	int count = 0;

	m_iRecoverLifeInc = 0;

	while (true)
	{
		if (lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].number;

			if (CHECK_LIMIT(tObjNum, MAX_OBJECT))
			{
				if (lpObj->VpPlayer2[count].type == OBJTYPE_CHARACTER)
				{
					if (gObjCalDistance(lpObj, &gObj[tObjNum]) <= g_iFereaBossRoarofLordDis)
					{
						gObj[tObjNum].Mana = 0;
						gObj[tObjNum].BP = 0;
						GCManaSend(gObj[tObjNum].m_Index, gObj[tObjNum].Mana, 0xFF, 0, gObj[tObjNum].BP);
						m_iRecoverLifeInc++;
					}
				}
			}
		}

		count++;

		if (count > MAXVIEWPORTOBJECT - 1)
		{
			break;
		}
	}

	if (m_iRecoverLifeInc == 0)
	{
		m_iRecoverLifeInc = 1;
	}

	gObjAddBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE, 0, 0, 0, 0, -10, 0, -1);
	gObjAddBuffEffect(lpObj, BUFFTYPE_MONSTER_MELEE_IMMUNE, 0, 0, 0, 0, -10, 0, -1);

	SummonDarkKnight(lpObj->m_Index);
	Teleport(lpObj->m_Index, 27, 122);

	lpObj->Dir = 3;
	lpObj->m_ActState.Emotion = 0;
	lpObj->m_ActState.Move = 0;

	m_iBossIndex = lpObj->m_Index;
	m_bRoarofLord = true;
}

BOOL FereaBoss::Teleport(int iIndex, BYTE btMapX, BYTE btMapY)
{
	LPOBJ lpObj = &gObj[iIndex];

	BYTE x = btMapX;
	BYTE y = btMapY;

	PMSG_MAGICATTACK_RESULT pAttack;
	PHeadSetBE((LPBYTE)&pAttack, 0x19, sizeof(pAttack));
	pAttack.MagicNumberH = HIBYTE(AT_SKILL_TELEPORT);
	pAttack.MagicNumberL = LOBYTE(AT_SKILL_TELEPORT);
	pAttack.SourceNumberH = HIBYTE(iIndex);
	pAttack.SourceNumberL = LOBYTE(iIndex);
	pAttack.TargetNumberH = HIBYTE(iIndex);
	pAttack.TargetNumberL = LOBYTE(iIndex);

	MsgSendV2(lpObj, (LPBYTE)&pAttack, pAttack.h.size);

	LogAddC(LOGC_RED, "[%s] Warp [%d,%d] -> [%d,%d]", lpObj->Name, lpObj->X, lpObj->Y, x, y);

	gObjTeleportMagicUse(iIndex, x, y);
	lpObj->TargetNumber = -1;
	lpObj->m_ActState.Attack = 0;
	lpObj->m_ActState.Move = 0;
	return FALSE;
}

void FereaBoss::RefillLife(LPOBJ lpObj)
{
	if (m_bRoarofLord == false)
	{
		return;
	}

	int	nAddLife = m_iRecoverLifeInc * g_iFereaBossRefillLifeValue;

	if (lpObj->Life >= lpObj->MaxLife)
	{
		return;
	}

	if (lpObj->MaxLife < lpObj->Life + nAddLife)
	{
		lpObj->Life = lpObj->MaxLife;
	}

	else
	{
		lpObj->Life += nAddLife;
	}

	if (lpObj->Type == OBJTYPE_MONSTER)
	{
		for (int n = 0; n < MAXVIEWPORTOBJECT; n++)
		{
			if (lpObj->VpPlayer2[n].state != 0)
			{
				if (lpObj->VpPlayer[n].type == OBJTYPE_CHARACTER)
				{
					GCObjectLifeInfo(lpObj->VpPlayer[n].number, lpObj->m_Index, (lpObj->AddLife + lpObj->MaxLife), lpObj->Life);
				}
			}
		}
	}
}

void FereaBoss::DieDarkKnight()
{
	FereaBoss::SetRoarofLord(false);
	gObjRemoveBuffEffect(&gObj[m_iBossIndex], BUFFTYPE_MONSTER_MAGIC_IMMUNE);
	gObjRemoveBuffEffect(&gObj[m_iBossIndex], BUFFTYPE_MONSTER_MELEE_IMMUNE);
}

void FereaBoss::SummonDarkKnight(int aIndex)
{
	if (aIndex < 0 || gObj[aIndex].Class != 734)
	{
		LogAddTD("[FereaBoss][SummonDarkNight] AddMonster fail!! [INDEX]:%d", aIndex);
		return;
	}

	g_FereaBossZone.AddMonster(733, gObj[aIndex].X - 3, gObj[aIndex].Y - 3, gObj[aIndex].X + 3, gObj[aIndex].Y + 3, 0);
	LogAddTD("[FereaBoss][SummonDarkNight]");
}