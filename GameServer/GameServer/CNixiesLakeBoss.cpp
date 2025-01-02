#include "stdafx.h"
#include "User.h"
#include "MonsterSkillManager.h"
#include "CNixiesLakeBoss.h"
#include "CNixiesLakeBossZone.h"
#include "Util.h"
#include "SkillManager.h"
#include "Viewport.h"
#include "Attack.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "Message.h"
#include "Map.h"
#include "ServerInfo.h"
CNixiesLakeBoss g_NixiesLakeBoss;

CNixiesLakeBoss::CNixiesLakeBoss()
{
	this->m_dwNixAttackDelay = 0;
	this->m_bNixFirstBelowLife90Per = 0;
	this->m_bNixFirstBelowLife30Per = 0;
	this->m_bNixFirstBelowLife10Per = 0;
	this->m_bUsedDevilIceLife80Per = 0;
	this->m_bUsedDevilIceLife50Per = 0;
	this->m_bUsedDevilIceLife5Per = 0;
	this->m_bUsedDevilIceDelay = GetTickCount64();
	this->m_bNixImmune = 0;
	this->m_dwIceRainDelay = 0;
	this->m_dwRefillLifeDelay = 0;
	this->m_stIceBreathInfo.Clear();
	this->m_stIceBoomInfo.Clear();
	this->m_stKnowAbsorbInfo.Clear();
	this->m_stDevilIceInfo.Clear();

	return;
}


CNixiesLakeBoss::~CNixiesLakeBoss()
{
	return;
}


//----- (00F3F600) --------------------------------------------------------
void  CNixiesLakeBoss::InitNixStatus()
{
	this->m_bNixFirstBelowLife90Per = 0;
	this->m_bNixFirstBelowLife30Per = 0;
	this->m_bNixFirstBelowLife10Per = 0;
	this->m_bUsedDevilIceLife80Per = 0;
	this->m_bUsedDevilIceLife50Per = 0;
	this->m_bUsedDevilIceLife5Per = 0;
	this->m_bUsedDevilIceDelay = GetTickCount64();
	this->m_bNixImmune = 0;
	this->m_dwIceRainDelay = 0;
	this->m_dwNixAttackDelay = 0;
	this->m_dwRefillLifeDelay = 0;
	this->m_stIceBreathInfo.Clear();
	this->m_stIceBoomInfo.Clear();
	this->m_stKnowAbsorbInfo.Clear();
	this->m_stDevilIceInfo.Clear();
}

BYTE __thiscall CNixiesLakeBoss::NixiesBossSkillRun(LPOBJ lpObj)
{
	if (this->m_bNixImmune == 1 && GetTickCount() >= this->m_stKnowAbsorbInfo.m_dwKnowledgeAbsorbTimer)
	{
		this->FailRemoveRestrictStone(lpObj);
	}
	else if (this->m_stKnowAbsorbInfo.m_dwKnowledgeAbsorbTimer && this->m_stKnowAbsorbInfo.m_nRestrictStoneCount <= 0)
	{
		this->SuccessRemoveRestrictStone(lpObj);
	}

	/*if(m_stDevilIceInfo.CountSent < 5 && (GetTickCount64() - m_stDevilIceInfo.LastSent) > 1000)
	{
		NotifyDevilIceMagicSquarePos();
	}*/

	if (m_stDevilIceInfo.m_bAttacking == false)
	{
		if (GetTickCount64() - this->m_bUsedDevilIceDelay > (gServerInfo.m_NixDevilIceDelay * 1000))
		{
			this->m_bUsedDevilIceDelay = GetTickCount64();
			SetDevilIceMagicSquare();
			NotifyDevilIceMagicSquarePos();
			CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, 5, 0);
			LogAdd(LOG_BLACK, "[NixiesBoss][DevilIce] Activated by time");

			//gMap[lpObj->Map].MonsterItemDrop(0, 0, 0, m_stDevilIceInfo.m_DevilIceMagicSquare[0].m_btPosX, m_stDevilIceInfo.m_DevilIceMagicSquare[0].m_btPosY, 0, 0, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, 0, 0);
			//gMap[lpObj->Map].MonsterItemDrop(0, 0, 0, m_stDevilIceInfo.m_DevilIceMagicSquare[1].m_btPosX, m_stDevilIceInfo.m_DevilIceMagicSquare[1].m_btPosY, 0, 0, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, 0, 0);
			//gMap[lpObj->Map].MonsterItemDrop(0, 0, 0, m_stDevilIceInfo.m_DevilIceMagicSquare[2].m_btPosX, m_stDevilIceInfo.m_DevilIceMagicSquare[2].m_btPosY, 0, 0, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, 0, 0);

			gMap[lpObj->Map].MoneyItemDrop(999999999, m_stDevilIceInfo.m_DevilIceMagicSquare[0].m_btPosX, m_stDevilIceInfo.m_DevilIceMagicSquare[0].m_btPosY);
			gMap[lpObj->Map].MoneyItemDrop(999999999, m_stDevilIceInfo.m_DevilIceMagicSquare[1].m_btPosX, m_stDevilIceInfo.m_DevilIceMagicSquare[1].m_btPosY);
			gMap[lpObj->Map].MoneyItemDrop(999999999, m_stDevilIceInfo.m_DevilIceMagicSquare[2].m_btPosX, m_stDevilIceInfo.m_DevilIceMagicSquare[2].m_btPosY);
		}
	}

	if (this->m_stIceBreathInfo.m_bAttack == 1)
	{
		this->IceBreathAttack(lpObj);
		return 1;
	}
	
	if (this->m_stIceBoomInfo.m_bAttack == 1)
	{
		if (this->m_stIceBoomInfo.m_bAttacking == 0)
		{
			this->IceBoomAttackReady(lpObj);
		}

		return 1;
	}

	if (this->m_bNixImmune == 1)
	{
		if (this->m_dwIceRainDelay < GetTickCount())
		{
			CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, 2, 0);
			this->m_dwIceRainDelay = GetTickCount() + 2000;
		}

		return 1;
	}

	return 0;
}

void __thiscall CNixiesLakeBoss::NixAttack(LPOBJ lpObj)
{
	if (lpObj->TargetNumber < 0)
	{
		return;
	}

	if (gObjIsConnected(lpObj->TargetNumber) == FALSE)
	{
		return;
	}

	int	nLifePercentageValue = CheckNixLifeStatus(lpObj);
	/*bool bDevilice = false;

	if (m_stDevilIceInfo.m_bAttacking == false)
	{
		if (m_bUsedDevilIceLife80Per == false && nLifePercentageValue <= 80)
		{
			bDevilice = true;
			m_bUsedDevilIceLife80Per = true;
			this->m_bUsedDevilIceDelay = GetTickCount64();
			LogAdd(LOG_BLACK, "[NixiesBoss][DevilIce] Activated on 80 perc");
		}
		else if (m_bUsedDevilIceLife50Per == false && nLifePercentageValue <= 50)
		{
			bDevilice = true;
			m_bUsedDevilIceLife50Per = true;
			this->m_bUsedDevilIceDelay = GetTickCount64();
			LogAdd(LOG_BLACK, "[NixiesBoss][DevilIce] Activated on 50 perc");
		}
		else if (m_bUsedDevilIceLife5Per == false && nLifePercentageValue <= 5)
		{
			bDevilice = true;
			m_bUsedDevilIceLife5Per = true;
			this->m_bUsedDevilIceDelay = GetTickCount64();
			LogAdd(LOG_BLACK, "[NixiesBoss][DevilIce] Activated on 5 perc");
		}
		else
		{
			if ((GetTickCount64() - this->m_bUsedDevilIceDelay) > (gServerInfo.m_NixDevilIceDelay * 1000))
			{
				bDevilice = true;
				this->m_bUsedDevilIceDelay = GetTickCount64();
				LogAdd(LOG_BLACK, "[NixiesBoss][DevilIce] Activated by time");
			}
		}
	}

	if (bDevilice == true)
	{
		SetDevilIceMagicSquare();
		NotifyDevilIceMagicSquarePos();
		CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, 5, 0);
		return;
	}*/

	bool bImmune = false;

	if (m_bNixFirstBelowLife90Per == false && nLifePercentageValue <= 90)
	{
		bImmune = true;
		m_bNixFirstBelowLife90Per = true;
	}

	else if (m_bNixFirstBelowLife30Per == false && nLifePercentageValue <= 30)
	{
		bImmune = true;
		m_bNixFirstBelowLife30Per = true;
	}

	else if (m_bNixFirstBelowLife10Per == false && nLifePercentageValue <= 10)
	{
		bImmune = true;
		m_bNixFirstBelowLife10Per = true;
	}

	if (bImmune == true)
	{
		if (SetKnowledgeAbsorb(lpObj) == true)
		{
			return;
		}
	}

	if (m_dwNixAttackDelay <= GetTickCount())
	{
		m_dwNixAttackDelay = GetTickCount() + 2000;
		CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, -1, 0);
	}
}

double  CNixiesLakeBoss::CheckNixLifeStatus(LPOBJ lpObj)
{
	return lpObj->Life / lpObj->MaxLife * 100.0;
}

BYTE  CNixiesLakeBoss::SummonRestrictStone(LPOBJ lpObj)
{
	char result; // al@3
	int v3; // eax@7
	int i; // [sp+50h] [bp-68h]@4
	int ty2; // [sp+54h] [bp-64h]@7
	int tx2; // [sp+58h] [bp-60h]@7
	bool bIsSuccess; // [sp+67h] [bp-51h]@4
	//int StonePos[4][4] = { -2,0,2,0,-1,1,1,-1,0,2,0,-2,1,1,-1,-1 }; // [sp+68h] [bp-50h]@4
	int StonePos[4][4] = { -2,0,2,0,-1,1,1,-1,0,2,0,-2,1,1,-1,-1 }; // [sp+68h] [bp-50h]@4

	int aIndex = lpObj->Index;
	int nRestrictStone1 = -1;
	int nRestrictStone2 = -1;

	if (aIndex < 0 || lpObj->Class != 746)
	{
		LogAdd(LOG_BLACK, "[NixiesBoss][SummonRestrictStrone] AddMonster failed!! [INDEX]:%d", aIndex);
		return 0;
	}

	bIsSuccess = 0;

	for (i = 0; i < 4; ++i)
	{
		v3 = rand() % 4;
		tx2 = StonePos[v3][2];
		ty2 = StonePos[v3][3];
		nRestrictStone1 = g_NixiesLakeBossZone.AddMonster(747, StonePos[v3][0] + gObj[aIndex].X, StonePos[v3][1] + gObj[aIndex].Y, StonePos[v3][0] + gObj[aIndex].X, StonePos[v3][1] + gObj[aIndex].Y, 0);

		if (nRestrictStone1 >= 0)
		{
			nRestrictStone2 = g_NixiesLakeBossZone.AddMonster(747, tx2 + gObj[aIndex].X, ty2 + gObj[aIndex].Y, tx2 + gObj[aIndex].X, ty2 + gObj[aIndex].Y, 0);

			if (nRestrictStone2 >= 0)
			{
				bIsSuccess = 1;
				break;
			}

			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[nRestrictStone1].Class);
			gObjDel(nRestrictStone1);
		}
	}
	if (!bIsSuccess)
	{
		nRestrictStone1 = g_NixiesLakeBossZone.AddMonster(747, gObj[aIndex].X - 3, gObj[aIndex].Y - 3, gObj[aIndex].X + 3, gObj[aIndex].Y + 3, 0);

		if (nRestrictStone1 < 0)
		{
			return 0;
		}

		nRestrictStone2 = g_NixiesLakeBossZone.AddMonster(747, gObj[aIndex].X - 3, gObj[aIndex].Y - 3, gObj[aIndex].X + 3, gObj[aIndex].Y + 3, 0);

		if (nRestrictStone2 < 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[nRestrictStone1].Class);
			gObjDel(nRestrictStone1);
			return 0;
		}

		bIsSuccess = 1;
	}

	if (bIsSuccess)
	{
		this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1 = nRestrictStone1;
		this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2 = nRestrictStone2;
		this->m_stKnowAbsorbInfo.m_nRestrictStoneCount = 2;

		GCMonsterSkillSend(lpObj, lpObj, 123);

		LogAdd(LOG_BLACK, "[NixiesBoss][SummonRestrictStone] Create. Index1 : %d, Index2  : %d)", nRestrictStone1, nRestrictStone2);

		result = 1;
	}
	else
	{
		result = 0;
	}

	return result;
}

BYTE  CNixiesLakeBoss::SetDevilIceMagicSquare()
{
	int nRangeX; // ST50_4@3
	int nRangeY; // ST4C_4@3
	int i; // [sp+54h] [bp-8h]@1

	this->m_stDevilIceInfo.m_bAttacking = 1;
	m_stDevilIceInfo.CountSent = 0;

	for (i = 0; i < 3; ++i)
	{
		nRangeX = g_btBossZoneDevilIceSquareMapXY[4 * i + 2] - g_btBossZoneDevilIceSquareMapXY[4 * i];
		nRangeY = g_btBossZoneDevilIceSquareMapXY[4 * i + 3] - g_btBossZoneDevilIceSquareMapXY[4 * i + 1];

		this->m_stDevilIceInfo.m_DevilIceMagicSquare[i].m_btPosX = rand() % nRangeX + g_btBossZoneDevilIceSquareMapXY[4 * i];
		this->m_stDevilIceInfo.m_DevilIceMagicSquare[i].m_btPosY = rand() % nRangeY + g_btBossZoneDevilIceSquareMapXY[4 * i + 1];
	}

	return 1;
}

int CNixiesLakeBoss::GetAvailableAttackSkillCount(LPOBJ lpObj)
{
	int nCountOfAvailableAttackSkill;

	if (this->CheckNixLifeStatus(lpObj) > 90 || this->m_stDevilIceInfo.m_bAttacking)
	{
		nCountOfAvailableAttackSkill = 3;
	}
	else
	{
		nCountOfAvailableAttackSkill = 4;
	}

	return nCountOfAvailableAttackSkill;
}

void CNixiesLakeBoss::InitIceBreathAttack(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	int v4;

	v4 = gSkillManager.GetSkillAngle(lpTargetObj->X, lpTargetObj->Y, lpObj->X, lpObj->Y);

	this->m_stIceBreathInfo.m_bAttack = 1;
	this->m_stIceBreathInfo.m_nAttackStartAngle = (v4 + 90) % 360;
	this->m_stIceBreathInfo.m_nAttackCount = 0;

	this->NotifyIceBreathDir(1, lpTargetObj);

	this->IceBreathAttack(lpObj);

	LogAdd(LOG_BLACK, "[NixiesBoss][UseSKill] Bafo congelante. Alvo: [%d][%s]", lpTargetObj->Index, lpTargetObj->Name);
}

void  CNixiesLakeBoss::IceBreathAttack(LPOBJ lpObj)
{
	this->m_stIceBreathInfo.m_nAttackCount++;

	if (this->m_stIceBreathInfo.m_nAttackCount >= 12)
	{
		this->m_stIceBreathInfo.Clear();
		this->NotifyIceBreathDir(0, 0);
		this->InitIceBoom();
	}
}

bool  CNixiesLakeBoss::IsIceBreathAttacking()
{
	return this->m_stIceBreathInfo.m_bAttack;
}

void  CNixiesLakeBoss::InitIceBoom()
{
	this->m_stIceBoomInfo.m_bAttack = 1;
	this->m_stIceBreathInfo.m_nAttackCount = 0;
}

void  CNixiesLakeBoss::IceBoomAttackReady(LPOBJ lpObj)
{
	if (lpObj->TargetNumber >= 0)
	{
		this->m_stIceBoomInfo.m_bAttacking = 1;
		this->SetIceBoomMagicSquare(lpObj);
		this->NotifyIceBoomMagicSquarePos();
		CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, 4, 0);
	}
	else
	{
		this->m_stIceBoomInfo.m_bAttack = 0;
	}
}

void  CNixiesLakeBoss::SetIceBoomMagicSquare(LPOBJ lpObj)
{
	int tObjNum;
	int count = 0;
	int nTarIndex[5] = { 0, 0, 0, 0, 0 };
	int nTarUserCnt = 0;

	while (true)
	{
		if (lpObj->VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj->VpPlayer2[count].index;

			if (CHECK_RANGE(tObjNum, MAX_OBJECT) != FALSE)
			{
				if (lpObj->VpPlayer2[count].type == OBJECT_USER)
				{
					if (gObjCalDistance(lpObj->X, lpObj->Y, gObj[tObjNum].X, gObj[tObjNum].Y) <= 9)
					{
						nTarIndex[nTarUserCnt] = tObjNum;
						nTarUserCnt++;

						if (nTarUserCnt >= 5)
						{
							break;
						}
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}

	short nPosX = 0;
	short nPosY = 0;

	if (nTarUserCnt >= 5)
	{
		nTarUserCnt = 5;
	}

	if (nTarUserCnt <= 0)
	{
		return;
	}

	int nTarUserIndex;

	for (int i = 0; i < nTarUserCnt; i++)
	{
		if (gObjIsConnected(nTarIndex[i]) != FALSE)
		{
			nPosX = gObj[nTarIndex[i]].X;
			nPosY = gObj[nTarIndex[i]].Y;

			//LogAdd(LOG_BLACK, "IceBoomMagicSquare %d %d", nPosX, nPosY);

			if (g_NixiesLakeBossZone.IsBossZone(nPosX, nPosY) == false)
			{
				if (g_NixiesLakeBossZone.GetBoxPosition(MAP_KNICKS, lpObj->X - 4, lpObj->Y - 4, lpObj->X + 4, lpObj->Y + 4, &nPosX, &nPosY) == false)
				{
					nPosX = lpObj->X;
					nPosY = lpObj->Y;
					LogAdd(LOG_RED, "[NixiesBoss][SetIceBoomSquare] GetBoxPosition #1 FAIL!!! %s %d", __FILE__, __LINE__);
				}

				LogAdd(LOG_RED, "[NixiesBoss][Error][SetIceBoomSquare] Error!!! %s/%d Pos: %d/%d ", gObj[nTarUserIndex].Name, nTarUserIndex, nPosX, nPosY);
			}

			m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btIndex = i;
			m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosX = nPosX;
			m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosY = nPosY;
			m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_wTarget = nTarUserIndex;
			m_stIceBoomInfo.m_nMagicSquareCnt++;
		}
	}
}

void CNixiesLakeBoss::IceBoomAttackDamage(LPOBJ lpObj)
{
	int i;
	LPOBJ lpTargetObj;
	int PosY;
	int PosX;
	char Index;

	if (this->m_stIceBoomInfo.m_bAttacking)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTargetObj = &gObj[n];

			if (gObjIsConnected(n) && lpTargetObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpTargetObj->X, lpTargetObj->Y))
			{
				for (i = 0; i < this->m_stIceBoomInfo.m_nMagicSquareCnt; ++i)
				{
					Index = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btIndex;
					PosX = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosX;
					PosY = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosY;

					if (gViewport.CheckViewportObjectPosition(n, lpTargetObj->Map, PosX, PosY, 15) == 1)
					{
						this->SendIceBoomMagicSquarePos(n, 1, Index, PosX, PosY);
					}

					if (gObjCalDistance(lpTargetObj->X, lpTargetObj->Y, PosX, PosY) <= 2)
					{
						gAttack.Attack(lpObj, lpTargetObj, 0, 1, 0, 999, 0, 0);
					}
				}
			}
		}

		this->m_stIceBoomInfo.m_nAttackCount++;
		this->m_stIceBoomInfo.m_bAttacking = 0;
		this->m_stIceBoomInfo.m_nMagicSquareCnt = 0;

		for (int j = 0; j < 5; ++j)
		{
			this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[j].m_btIndex = 0;
			this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[j].m_btPosX = 0;
			this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[j].m_btPosY = 0;
			this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[j].m_wTarget = 0;
		}

		if (this->m_stIceBoomInfo.m_nAttackCount >= 3)
		{
			this->m_stIceBoomInfo.Clear();
		}
	}
}

BYTE  CNixiesLakeBoss::SetKnowledgeAbsorb(LPOBJ lpObj)
{
	if (this->SummonRestrictStone(lpObj) == 1)
	{
		LogAdd(LOG_BLACK, "[NixiesBoss][UseSKill] Absorver conhecimento");
		this->StartImmune(lpObj);
		this->NotifyKnowledgeAbsorbInfo(1);
		return 1;
	}

	return 0;
}

void  CNixiesLakeBoss::FailRemoveRestrictStone(LPOBJ lpObj)
{
	this->ClearRestrictStone();
	this->EndImmune(lpObj);
	this->SummonMonster();
	this->NotifyKnowledgeAbsorbInfo(0);
}

void  CNixiesLakeBoss::SuccessRemoveRestrictStone(LPOBJ lpObj)
{
	this->ClearRestrictStone();
	this->EndImmune(lpObj);
	this->NotifyKnowledgeAbsorbInfo(0);
}

void __thiscall CNixiesLakeBoss::RemoveRestrictStone(LPOBJ lpObj)
{
	this->m_stKnowAbsorbInfo.m_nRestrictStoneCount--;

	if (this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1 == lpObj->Index)
	{
		this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1 = -1;
	}
	else if (this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2 == lpObj->Index)
	{
		this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2 = -1;
	}
}

void  CNixiesLakeBoss::ClearRestrictStone()
{
	if (this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1 != -1)
	{
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1].Class);
		gObjDel(this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1);
	}

	if (this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2 != -1)
	{
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2].Class);
		gObjDel(this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2);
	}

	this->m_stKnowAbsorbInfo.Clear();
}

void CNixiesLakeBoss::SummonMonster()
{
	int iMonIndex;
	signed int v2;
	int nIndex;
	int i;

	for (i = 0; i < 20; ++i)
	{
		iMonIndex = rand() % 3 + 743;
		v2 = rand();
		nIndex = g_NixiesLakeBossZone.AddMonster(iMonIndex,(unsigned __int8)g_btBossZoneMonSummonMapXY[0],(unsigned __int8)g_btBossZoneMonSummonMapXY[1],(unsigned __int8)g_btBossZoneMonSummonMapXY[2],(unsigned __int8)g_btBossZoneMonSummonMapXY[3],v2 % 6);

		if (nIndex < 0)
		{
			LogAdd(LOG_BLACK, "[BossZone][SummonMon] AddMonster fail!! [nIndex]:%d", nIndex);
			return;
		}
	}
}

void __thiscall CNixiesLakeBoss::StartImmune(LPOBJ lpObj)
{
	this->m_stKnowAbsorbInfo.m_dwKnowledgeAbsorbTimer = GetTickCount() + 180000;
	this->m_bNixImmune = 1;

	gEffectManager.AddEffect(lpObj, 0, EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY, 236, 0, 0, 0, 0);
	gEffectManager.AddEffect(lpObj, 0, EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY, 236, 0, 0, 0, 0);

	LogAdd(LOG_BLACK, "[NixiesBoss][Immune] Start Nix Immune");
}

void __thiscall CNixiesLakeBoss::EndImmune(LPOBJ lpObj)
{
	this->m_bNixImmune = 0;
	gEffectManager.DelEffect(lpObj, EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY);
	gEffectManager.DelEffect(lpObj, EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY);
	this->m_stKnowAbsorbInfo.Clear();

	LogAdd(LOG_BLACK, "[NixiesBoss][Immune] End Nix Immune");
}

void __thiscall CNixiesLakeBoss::NotifyIceBoomMagicSquarePos()
{
	int i;
	LPOBJ lpTargetObj;

	BYTE PosY;
	BYTE PosX;
	BYTE Index;

	if (this->m_stIceBoomInfo.m_bAttack)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTargetObj = &gObj[n];

			if (gObjIsConnected(n) && lpTargetObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpTargetObj->X, lpTargetObj->Y))
			{
				for (i = 0; i < this->m_stIceBoomInfo.m_nMagicSquareCnt; ++i)
				{
					Index = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btIndex;
					PosX = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosX;
					PosY = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosY;

					if (gViewport.CheckViewportObjectPosition(n, lpTargetObj->Map, this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosX, this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosY, 15) == 1)
					{
						this->SendIceBoomMagicSquarePos(n, 0, Index, PosX, PosY);
					}
				}
			}
		}
	}
}

void __thiscall CNixiesLakeBoss::NotifyDevilIceMagicSquarePos()
{
	if (this->m_stDevilIceInfo.m_bAttacking)
	{
		LPOBJ lpTargetObj;

		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTargetObj = &gObj[n];

			if (gObjIsConnected(n) && lpTargetObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpTargetObj->X, lpTargetObj->Y))
			{
				this->SendDevilIceMagicSquarePos(n, 0, this->m_stDevilIceInfo.m_DevilIceMagicSquare);
			}
		}
	}
}

void __thiscall CNixiesLakeBoss::NotifyIceBreathDir(BYTE status, LPOBJ lpTargetObj)
{
	LPOBJ lpTempObj;

	if (this->m_stIceBreathInfo.m_bAttack)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTempObj = &gObj[n];

			if (gObjIsConnected(n) && lpTempObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpTempObj->X, lpTempObj->Y))
			{
				if (status)
				{
					this->SendIceBreathDir(n, status, lpTargetObj->Index, (BYTE)lpTargetObj->X, (BYTE)lpTargetObj->Y);
				}
				else
				{
					this->SendIceBreathDir(n, 0, 0, 0, 0);
				}
			}
		}
	}
}

void __thiscall CNixiesLakeBoss::DevilIceAttackDamage(LPOBJ lpObj)
{
	LPOBJ lpTargetObj;

	//LogAdd(LOG_RED, "m_bAttacking %d", this->m_stDevilIceInfo.m_bAttacking);

	if (this->m_stDevilIceInfo.m_bAttacking)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTargetObj = &gObj[n];

			if (gObjIsConnected(n))
			{
				if (lpTargetObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpTargetObj->X, lpTargetObj->Y))
				{
					this->SendDevilIceMagicSquarePos(n, 1, this->m_stDevilIceInfo.m_DevilIceMagicSquare);

					if (gObjCalDistance(lpTargetObj->X, lpTargetObj->Y, this->m_stDevilIceInfo.m_DevilIceMagicSquare[0].m_btPosX, this->m_stDevilIceInfo.m_DevilIceMagicSquare[0].m_btPosY) > 2
						&& gObjCalDistance(lpTargetObj->X, lpTargetObj->Y, this->m_stDevilIceInfo.m_DevilIceMagicSquare[1].m_btPosX, this->m_stDevilIceInfo.m_DevilIceMagicSquare[1].m_btPosY) > 2
						&& gObjCalDistance(lpTargetObj->X, lpTargetObj->Y, this->m_stDevilIceInfo.m_DevilIceMagicSquare[2].m_btPosX, this->m_stDevilIceInfo.m_DevilIceMagicSquare[2].m_btPosY) > 2)
					{
						lpTargetObj->Life = 0.0;

						gObjectManager.CharacterLifeCheck(lpObj, lpTargetObj, 999999, 0, 0, 0, 0, 0);
						LogAdd(LOG_BLACK, "[NixiesBoss][ApplyDebuff] Instance Death. Target : [%d][%s]", lpTargetObj->Index, lpTargetObj->Name);
					}
				}
			}
		}

		this->m_stDevilIceInfo.m_bAttacking = 0;
	}
}

void  CNixiesLakeBoss::RefillLife(LPOBJ lpObj)
{
	if (lpObj->Life >= lpObj->MaxLife)
	{
		return;
	}

	int nAddLife = lpObj->Life / 2;

	if (lpObj->MaxLife < lpObj->Life + nAddLife)
	{
		lpObj->Life = lpObj->MaxLife;
	}

	else
	{
		lpObj->Life += nAddLife;
	}

	LogAdd(LOG_BLACK, "[NixiesBoss][UseSKill] Cure. Before Life: %d -> After Life: %d", (int)(lpObj->Life - nAddLife), (int)lpObj->Life);

	if (lpObj->Type == OBJECT_MONSTER)
	{
		for (int n = 0; n < MAX_VIEWPORT; n++)
		{
			if (lpObj->VpPlayer2[n].state != 0)
			{
				if (lpObj->VpPlayer[n].type == OBJECT_USER)
				{
					GCLifeUpdateSend(&gObj[lpObj->VpPlayer2[n].index]);
				}
			}
		}
	}
}

void  CNixiesLakeBoss::FailBossBattles(LPOBJ lpObj)
{
	g_NixiesLakeBossZone.Clear();
	this->EndImmune(lpObj);
	lpObj->Life = lpObj->MaxLife;
	g_NixiesLakeBossZone.SetBossZoneState(0);
}

void CNixiesLakeBoss::DieNixiesBoss()
{
	g_NixiesLakeBossZone.Clear();
	g_NixiesLakeBossZone.SetBossDieTimer();
	g_NixiesLakeBossZone.SetBossNixRegenTimer();
	g_NixiesLakeBossZone.SetBossZoneState(2);
}

void  CNixiesLakeBoss::NotifyKnowledgeAbsorbInfo(BYTE status)
{
	LPOBJ lpTargetObj;

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
	{
		lpTargetObj = &gObj[n];

		if (lpTargetObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpTargetObj->X, lpTargetObj->Y))
		{
			this->SendKnowledgeAbsorb(lpTargetObj->Index, status);
		}
	}
}

void __thiscall CNixiesLakeBoss::DisableCountSkill()
{
	if (this->m_stIceBreathInfo.m_bAttack == 1)
	{
		this->m_stIceBreathInfo.Clear();
	}
}

int  CNixiesLakeBoss::SearchTarget(LPOBJ lpObj, int Distance)
{
	int count;
	int tObjNum;

	count = 0;
	do
	{
		if (lpObj->VpPlayer[count].state)
		{
			tObjNum = lpObj->VpPlayer[count].index;

			if (gObjIsConnected(tObjNum) != 0 && lpObj->VpPlayer[count].type == 1 && gObj[tObjNum].Live == 1 && gObjCalcDistance(lpObj, &gObj[tObjNum]) <= Distance)
			{
				return tObjNum;
			}
		}
		count++;

	} while (count < MAX_VIEWPORT);

	return -1;
}

void __thiscall CNixiesLakeBoss::SetRefillLifeDelay()
{
	this->m_dwRefillLifeDelay = GetTickCount() + 4000;
}

BOOL __thiscall CNixiesLakeBoss::ChkRefillLifeDelay()
{
	return this->m_dwRefillLifeDelay && this->m_dwRefillLifeDelay <= GetTickCount();
}

void  CNixiesLakeBoss::SendIceBoomMagicSquarePos(int aIndex, BYTE status, BYTE index, BYTE x, BYTE y)
{
	_tagPMSG_SEND_ICE_BOOM_POS pMsg;
	pMsg.h.set(0x76, 0x05, sizeof(pMsg));

	pMsg.btStatus = status;
	pMsg.btIndex = index;
	pMsg.btPosX = x;
	pMsg.btPosY = y;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void  CNixiesLakeBoss::SendDevilIceMagicSquarePos(int aIndex, BYTE status, _stMagicSquarePos* squarePos)
{
	_tagPMSG_SEND_DEVILICE_MAGIC_SQUARE_POS pMsg;
	pMsg.h.set(0x76, 0x06, sizeof(pMsg));

	pMsg.btStatus = status;
	pMsg.btPosX1 = squarePos[0].m_btPosX;
	pMsg.btPosY1 = squarePos[0].m_btPosY;
	pMsg.btPosX2 = squarePos[1].m_btPosX;
	pMsg.btPosY2 = squarePos[1].m_btPosY;
	pMsg.btPosX3 = squarePos[2].m_btPosX;
	pMsg.btPosY3 = squarePos[2].m_btPosY;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);

	_tagPMSG_SEND_ICE_BOOM_POS pMsg2;
	pMsg2.h.set(0x76, 0x05, sizeof(pMsg2));

	pMsg2.btStatus = 0;
	pMsg2.btIndex = 0;
	pMsg2.btPosX = squarePos[0].m_btPosX;
	pMsg2.btPosY = squarePos[0].m_btPosY;

	DataSend(aIndex, (BYTE*)&pMsg2, pMsg2.h.size);

	pMsg2.btIndex = 1;
	pMsg2.btPosX = squarePos[1].m_btPosX;
	pMsg2.btPosY = squarePos[1].m_btPosY;

	DataSend(aIndex, (BYTE*)&pMsg2, pMsg2.h.size);

	pMsg2.btIndex = 2;
	pMsg2.btPosX = squarePos[2].m_btPosX;
	pMsg2.btPosY = squarePos[2].m_btPosY;

	DataSend(aIndex, (BYTE*)&pMsg2, pMsg2.h.size);
}

void  CNixiesLakeBoss::SendIceBreathDir(int aIndex, BYTE status, unsigned __int16 TargetIndex, BYTE x, BYTE y)
{
	_tagPMSG_SEND_ICE_BREATH_DIR pMsg; // [sp+4Ch] [bp-10h]@1
	pMsg.h.set(0x76, 0x07, sizeof(pMsg));

	pMsg.btState = status;
	pMsg.wTargetIndex = TargetIndex;
	pMsg.btTargetX = x;
	pMsg.btTargetY = y;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void  CNixiesLakeBoss::SendKnowledgeAbsorb(int aIndex, BYTE status)
{
	_tagPMSG_SEND_KNOWLEDGEABSORBINFO pMsg; // [sp+4Ch] [bp-Ch]@1
	pMsg.h.set(0x76, 0x08, sizeof(pMsg));

	pMsg.btStatus = status;
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void  CNixiesLakeBoss::RecvSendIceBreathAttack(_tagPMSG_ICE_BREATH_ATTACK* lpMsg, int nTargetIndex)
{
	int NewTarget;
	LPOBJ lpTargetObj;
	LPOBJ lpBossObj;

	if (this->m_stIceBreathInfo.m_bAttack)
	{
		if (gObjIsConnected(nTargetIndex) == 0)
		{
			return;
		}

		if (gObjIsConnected(lpMsg->wAttackerIndex) == 0)
		{
			return;
		}

		lpBossObj = &gObj[lpMsg->wAttackerIndex];
		lpTargetObj = &gObj[nTargetIndex];

		if (lpBossObj->Class == 746 && lpTargetObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpTargetObj->X, lpTargetObj->Y))
		{
			if (!gEffectManager.CheckEffect(lpTargetObj, EFFECT_ICE))
			{
				gEffectManager.AddEffect(lpTargetObj, 0, EFFECT_ICE, 10, 0, 0, 0, 0);
				LogAdd(LOG_BLACK, "[NixiesBoss][ApplyDebuff] Velocidade diminuida. Alvo: [%d][%s]", lpTargetObj->Index, lpTargetObj->Name);
			}

			gAttack.Attack(lpBossObj, lpTargetObj, 0, 0, 0, 0, 0, 0);

			DataSend(nTargetIndex, (BYTE*)&lpMsg, lpMsg->h.size);
			MsgSendV2(lpTargetObj, (BYTE*)&lpMsg, lpMsg->h.size);

			if (lpTargetObj->Life <= 0.0 || !lpTargetObj->Live)
			{
				NewTarget = this->SearchTarget(lpBossObj, 15);
				if (NewTarget <= -1)
				{
					this->NotifyIceBreathDir(0, 0);
					this->m_stIceBreathInfo.Clear();
				}
				else
				{
					this->NotifyIceBreathDir(2, &gObj[NewTarget]);
				}
			}
		}
	}
}