#include "stdafx.h"
#include "User.h"
#include "Util.h"
#include "CNixiesLakeBossZone.h"
#include "CNixiesLakeBoss.h"
#include "Monster.h"
#include "Map.h"
#include "Party.h"
#include "ItemManager.h"
#include "Message.h"
#include "ServerInfo.h"
#include "Notice.h"

CNixiesLakeBossZone g_NixiesLakeBossZone;
BYTE g_btBossZoneMapXY[4] = { 14,202,47,236 };
BYTE g_btBossZoneMonSummonMapXY[4] = { 21,208,36,230 };
BYTE g_btBossZoneDevilIceSquareMapXY[12] = { 21,208,39,212,18,217,44,221,23,225,36,230 };

CNixiesLakeBossZone::CNixiesLakeBossZone()
{
	this->m_btBossZoneState = 2;
	this->m_bIsBossLive = 0;
	this->m_dwBossRegenTimer = 0;
	this->m_dwBossDieTimer = 0;
	this->m_stPartyPlayers.Clear();

	InitializeCriticalSection(&this->m_criNixiesBossZone);
}

CNixiesLakeBossZone::~CNixiesLakeBossZone()
{
	DeleteCriticalSection(&this->m_criNixiesBossZone);
	this->m_dwBossDieTimer = 0;
}

void __thiscall CNixiesLakeBossZone::Clear()
{
	this->ClearMonster();
	g_NixiesLakeBoss.InitNixStatus();
}

void  CNixiesLakeBossZone::InitNixiesBossZone(int nAttribute)
{
	double v2;
	signed int v3;
	int v4;

	signed int nAttr;

	this->Clear();

	nAttr = 0;

	if (nAttribute)
	{
		nAttr = nAttribute;
		v4 = this->AddMonster(746, 30, 218, 31, 219, nAttribute);
	}
	else
	{
		v2 = (double)rand() / 32767.0 * 9999.0;
		v3 = (int)sqrt(v2);
		if (v3 % 5)
		{
			switch (v3 % 5)
			{
			case 1:
				nAttr = 2;
				v4 = this->AddMonster(746, 30, 218, 31, 219, 2);
				break;
			case 2:
				nAttr = 3;
				v4 = this->AddMonster(746, 30, 218, 31, 219, 3);
				break;
			case 3:
				nAttr = 4;
				v4 = this->AddMonster(746, 30, 218, 31, 219, 4);
				break;
			default:
				if (v3 % 5 == 4)
					nAttr = 5;
				v4 = this->AddMonster(746, 30, 218, 31, 219, nAttr);
				break;
			}
		}
		else
		{
			nAttr = 1;
			v4 = this->AddMonster(746, 30, 218, 31, 219, 1);
		}
	}

	LogAdd(LOG_BLACK, "[NixiesBoss][Regen] Regen Boss Nix. [%d], Attr:[%d]", v4, nAttr);

	if (this->ChkBossZonePlayer())
	{
		this->SetBossZoneState(1);
	}
	else
	{
		this->SetBossZoneState(0);
	}

	if (v4 > 0)
	{
		this->m_bIsBossLive = 1;
		this->m_dwBossRegenTimer = 0;
		this->m_dwBossDieTimer = 0;
	}
}

void  CNixiesLakeBossZone::ChkBossNixRegenTimer()
{
	if (this->m_bIsBossLive != 1 && this->m_dwBossRegenTimer && this->m_dwBossRegenTimer < GetTickCount())
	{
		this->InitNixiesBossZone(0);
	}
}

void  CNixiesLakeBossZone::SetBossNixRegenTimer()
{
	this->m_dwBossRegenTimer = GetTickCount() + gServerInfo.m_NixRegenTime * 60 * 1000;
}

void  CNixiesLakeBossZone::SetBossZoneState(char state)
{
	this->m_btBossZoneState = state;
}

BYTE  CNixiesLakeBossZone::GetBossZoneState()
{
	return this->m_btBossZoneState;
}

int  CNixiesLakeBossZone::AddMonster(int iMonIndex, int iBeginX, int iBeginY, int iEndX, int iEndY, int iMonAttr)
{
	__int16 index;
	int result, nResult;

	index = gObjAddMonster(MAP_KNICKS);
	nResult = index;

	if (index < 0)
	{
		LogAdd(LOG_BLACK, "[NixiesBoss][SummonMon] AddMonster failed! [INDEX]: %d", iMonIndex);
		result = -1;
	}
	else if (this->SetPosMonster(index, MAP_KNICKS, iBeginX, iBeginY, iEndX, iEndY))
	{
		if (gObjSetMonster(nResult, iMonIndex))
		{
			if (iMonAttr > 0)
			{
				gObj[nResult].ElementalAttribute = iMonAttr;
			}

			gObj[nResult].Attribute = 60;
			LogAdd(LOG_BLACK, "[NixiesBoss][SummonMonster] MonsterIndex: %d/%s, MapNumber: %d, X: %d, Y: %d, Attr : %d ",gObj[nResult].Class,gObj[nResult].Name,MAP_KNICKS,gObj[nResult].X,gObj[nResult].Y,gObj[nResult].ElementalAttribute);
			result = nResult;
		}
		else
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[nResult].Class);
			gObjDel(nResult);
			LogAdd(LOG_BLACK,"[BossZone][Error] error : %s %d","cNixiesLakeBossZone.cpp",iMonAttr);
			result = -1;
		}
	}
	else
	{
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[nResult].Class);
		gObjDel(nResult);
		LogAdd(LOG_BLACK,"[BossZone][Error] error : CNixiesLakeBossZone.cpp %d", index);
		result = -1;
	}

	return result;
}

void  CNixiesLakeBossZone::ClearMonster()
{
	LPOBJ lpTargetObj;

	for (int i = 0; i < MAX_OBJECT_MONSTER; ++i)
	{
		lpTargetObj = &gObj[i];

		if (gObj[i].Map == MAP_KNICKS && gObj[i].Connected == 3 && gObj[i].Type == OBJECT_MONSTER && this->IsBossZone(lpTargetObj->X, lpTargetObj->Y))
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[i].Class);
			gObjDel(lpTargetObj->Index);
		}
	}
}

bool  CNixiesLakeBossZone::IsBossZone(int X, int Y)
{
	return X >= g_btBossZoneMapXY[0] && X <= g_btBossZoneMapXY[2] && Y >= g_btBossZoneMapXY[1] && Y <= g_btBossZoneMapXY[3];
}

bool  CNixiesLakeBossZone::IsBossZoneMonSummonArea(int X, int Y)
{
	return X >= g_btBossZoneMonSummonMapXY[0] && X <= g_btBossZoneMonSummonMapXY[2] && Y >= g_btBossZoneMonSummonMapXY[1] && Y <= g_btBossZoneMonSummonMapXY[3];
}

BYTE CNixiesLakeBossZone::SetPosMonster(int aIndex, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY)
{
	BYTE result;
	LPOBJ lpObj;

	if (OBJECT_MONSTER_RANGE(aIndex) == 0)
	{
		LogAdd(LOG_BLACK,"[BossZone][Error] CNixiesLakeBossZone.cpp %d", nMapNumber);
		return 0;
	}

	lpObj = &gObj[aIndex];
	gObj[aIndex].PosNum = -1;
	lpObj->Map = nMapNumber;

	if (this->GetBoxPosition(nMapNumber, nBeginX, nBeginY, nEndX, nEndY, &lpObj->X, &lpObj->Y))
	{
		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;
		lpObj->OldX = lpObj->X;
		lpObj->OldY = lpObj->Y;
		lpObj->Dir = rand() % 8;
		lpObj->StartX = (BYTE)lpObj->X;
		lpObj->StartY = (BYTE)lpObj->Y;
		lpObj->DieRegen = 0;
		lpObj->MaxRegenTime = 0;
		return 1;
	}

	return 0;
}

BYTE CNixiesLakeBossZone::GetBoxPosition(int mapnumber, int bx, int by, int ex, int ey, __int16* mx, __int16* my)
{
	int v8;

	unsigned int ty;
	unsigned int tx;
	int h;
	int count;

	count = 100;
	while (1)
	{
		v8 = count--;

		if (!v8)
			break;

		h = ey - by;

		if (ex - bx > 0)
			tx = bx + GetLargeRand() % (ex - bx);
		else
			tx = bx;

		if (h > 0)
			ty = by + GetLargeRand() % h;
		else
			ty = by;

		if (this->IsBossZone(tx, ty) && gMap[mapnumber].CheckAttr(tx, tx, 4) == 0 && gMap[mapnumber].CheckAttr(tx, tx, 4) == 0 && gMap[mapnumber].CheckAttr(tx, tx, 8) == 0)
		{
			*mx = tx;
			*my = ty;
			return 1;
		}
	}

	return 0;
}

BYTE CNixiesLakeBossZone::ChkBossZonePlayer()
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
	{
		if (gObjIsConnectedGP(n) && gObj[n].Map == MAP_KNICKS && this->IsBossZone(gObj[n].X, gObj[n].Y))
		{
			//LogAdd(LOG_RED, "Jogador no Nix: %s", gObj[n].Name);
			return 1;
		}
	}

	return 0;
}

int  CNixiesLakeBossZone::GetBossZonePlayerCnt()
{
	int UserCnt = 0;

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
	{
		if (gObj[n].Map == MAP_KNICKS && gObjIsConnected(n) == 1 && this->IsBossZone(gObj[n].X, gObj[n].Y))
		{
			++UserCnt;
		}
	}

	return UserCnt;
}

BYTE CNixiesLakeBossZone::ChkEnterBossZone(int aIndex)
{
	BYTE result, btResult, btCurrentState;
	int n;
	bool isPartyPlayer;

	if (gObj[aIndex].Map == MAP_KNICKS)
	{
		isPartyPlayer = 0;

		if (this->m_stPartyPlayers.nPlayersIdx[0] <= -1)
		{
			goto LABEL_28;
		}

		for (n = 0; n < this->m_stPartyPlayers.nPlayerCount; ++n)
		{
			if (this->m_stPartyPlayers.nPlayersIdx[n] == aIndex)
			{
				isPartyPlayer = 1;
			}
		}

		if (isPartyPlayer)
		{
		LABEL_28:
			btCurrentState = this->GetBossZoneState();

			if (btCurrentState == 2)
			{
				LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Don't Regen Nixies Boss yet");
				gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Nix off. Tente novamente em %d minutos.", ((this->m_dwBossRegenTimer - GetTickCount()) / 1000 / 60));
				btResult = 2;
			}
			else if (btCurrentState == 1)
			{
				LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Preoccupy Nixies Boss Zone");
				btResult = 1;
			}
			else if (this->HavePieceOfNixi(aIndex) == -1)
			{
				btResult = 3;

				if (gObj[aIndex].PartyNumber > -1 && !gParty.IsLeader(gObj[aIndex].PartyNumber, aIndex))
				{
					btResult = 4;
				}

				if (btResult == 3)
				{
					LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Applicant has no Item . [%d][%s]", aIndex, gObj[aIndex].Name);
				}
				else if (btResult == 4)
				{
					LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Party member has no Item . [%d][%s]", aIndex, gObj[aIndex].Name);
				}
			}
			else
			{
				btResult = 0;
			}

			result = btResult;
		}
		else
		{
			LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Other Character Applies for Nixies Boss Zone already [%d][%s]",this->m_stPartyPlayers.nPlayersIdx[0],gObj[this->m_stPartyPlayers.nPlayersIdx[0]].Name);
			result = 1;
		}
	}
	else
	{
		LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. No NIXIES LAKE MAP [%d][%s][%d]",aIndex,gObj[aIndex].Name,gObj[aIndex].Map);
		result = -1;
	}

	return result;
}

void  CNixiesLakeBossZone::EnterBossZone(int aIndex)
{
	int itemPos;
	BYTE btResult;

	btResult = 0;
	itemPos = this->HavePieceOfNixi(aIndex);

	if (itemPos == -1)
	{
		this->SendAnsEnterBossZone(aIndex, -1);
	}
	else
	{
		if (gObjMoveGate(aIndex, 531))
		{
			this->UsePieceOfNixi(aIndex, itemPos);
			LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Success [%d][%s][%d]", aIndex, gObj[aIndex].Name, gObj[aIndex].Class);
			g_NixiesLakeBossZone.m_dwBossLastHitTimer = GetTickCount64();
		}
		else
		{
			btResult = -1;
			LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Error gObjMoveGate [%d][%s]", aIndex, gObj[aIndex].Name);
		}

		this->SendAnsEnterBossZone(aIndex, btResult);
	}
}

void  CNixiesLakeBossZone::PartyEnterBossZone()
{
	int n;
	BYTE btResult;
	int nLeaderIdx;

	nLeaderIdx = this->m_stPartyPlayers.nPlayersIdx[0];

	if (gObjIsConnected(nLeaderIdx) == 0)
	{
		return;
	}

	btResult = this->ChkEnterBossZone(nLeaderIdx);

	if (gObj[nLeaderIdx].PartyNumber > -1)
	{
		if (!gParty.IsLeader(gObj[nLeaderIdx].PartyNumber, nLeaderIdx))
		{
			btResult = 5;
			LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Applicant is Party Member . [%d][%s]",nLeaderIdx,gObj[nLeaderIdx].Name);
		}

		if (!btResult && !this->ChkEnterInviteMemberToBossZone(nLeaderIdx))
		{
			btResult = -1;
			LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Etc . [%d][%s]", nLeaderIdx, gObj[nLeaderIdx].Name);
		}
	}

	if (!btResult)
	{
		for (n = 0; n < this->m_stPartyPlayers.nPlayerCount; ++n)
		{
			this->EnterBossZone(this->m_stPartyPlayers.nPlayersIdx[n]);
		}

		this->ClearMonster();
		this->InitNixiesBossZone(0);
		this->SetBossZoneState(1);
	}
}

int  CNixiesLakeBossZone::HavePieceOfNixi(int aIndex)
{
	int n;
	LPOBJ lpObj;

	lpObj = &gObj[aIndex];

	for (n = INVENTORY_WEAR_SIZE; n < INVENTORY_SIZE; ++n)
	{
		if (lpObj->Inventory[n].IsItem() == 1 && lpObj->Inventory[n].m_Index == GET_ITEM(14, 390))
			return n;
	}

	return -1;
}

void  CNixiesLakeBossZone::UsePieceOfNixi(int aIndex, int nPos)
{
	LPOBJ lpObj;

	lpObj = &gObj[aIndex];
	gObj[aIndex].Inventory[nPos].m_Durability = gObj[aIndex].Inventory[nPos].m_Durability - 1.0f;

	if (lpObj->Inventory[nPos].m_Durability > 0.0)
	{
		gItemManager.GCItemDurSend(aIndex, nPos, (signed int)gObj[aIndex].Inventory[nPos].m_Durability, 0);
	}
	else
	{
		gItemManager.InventoryDelItem(aIndex, nPos);
		gItemManager.GCItemDeleteSend(aIndex, nPos, 1);
	}
}

BYTE  CNixiesLakeBossZone::ChkEnterPartyMemberToBossZone(int aIndex)
{
	int n; // [sp+4Ch] [bp-20h]@1
	BYTE btResult; // [sp+53h] [bp-19h]@1
	int nPartyNum; // [sp+58h] [bp-14h]@1
	int nPartyUserIndex; // [sp+5Ch] [bp-10h]@4
	LPOBJ lpPartyObj; // [sp+60h] [bp-Ch]@5
	LPOBJ lpObj; // [sp+64h] [bp-8h]@1

	lpObj = &gObj[aIndex];
	nPartyNum = gObj[aIndex].PartyNumber;
	btResult = 0;
	for (n = 1; n < MAX_PARTY_USER; ++n)
	{
		nPartyUserIndex = gParty.m_PartyInfo[nPartyNum].Index[n];
		if (nPartyUserIndex >= 0)
		{
			lpPartyObj = &gObj[nPartyUserIndex];
			btResult = this->ChkEnterBossZone(gObj[nPartyUserIndex].Index);

			if (lpObj->Map == lpPartyObj->Map)
			{
				if (gObjCalcDistance(lpObj, lpPartyObj) > 12)
				{
					btResult = 6;
					LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Party member don't exist nearby. [%d][%s]", lpPartyObj->Index, lpPartyObj->Name);
				}
			}
			else
			{
				btResult = 6;
				LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Party member don't exist nearby. [%d][%s][%d]", lpPartyObj->Index, lpPartyObj->Name, lpPartyObj->Map);
			}

			if (btResult)
			{
				return btResult;
			}
		}
	}

	return btResult;
}

BYTE  CNixiesLakeBossZone::ChkEnterInviteMemberToBossZone(int aIndex)
{
	BYTE result; // al@2
	int v3; // [sp+Ch] [bp-70h]@19
	signed int j; // [sp+50h] [bp-2Ch]@15
	int i; // [sp+54h] [bp-28h]@7
	bool bMatching; // [sp+5Bh] [bp-21h]@7
	int n; // [sp+5Ch] [bp-20h]@3
	BYTE btResult; // [sp+63h] [bp-19h]@22
	int nPartyNum; // [sp+68h] [bp-14h]@1
	int nPartyUserIndex; // [sp+6Ch] [bp-10h]@6
	LPOBJ lpPartyObj; // [sp+70h] [bp-Ch]@22
	LPOBJ lpObj; // [sp+74h] [bp-8h]@1

	lpObj = &gObj[aIndex];
	nPartyNum = gObj[aIndex].PartyNumber;

	if (gParty.m_PartyInfo[nPartyNum].Count == this->m_stPartyPlayers.nPlayerCount)
	{
		for (n = 1; n < 5; ++n)
		{
			nPartyUserIndex = gParty.m_PartyInfo[nPartyNum].Index[n];

			if (nPartyUserIndex != -1)
			{
				bMatching = 0;
				for (i = 0; i < this->m_stPartyPlayers.nPlayerCount; ++i)
				{
					if (nPartyUserIndex == this->m_stPartyPlayers.nPlayersIdx[i])
					{
						bMatching = 1;
						break;
					}
				}

				if (!bMatching)
				{
					LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Change Party Info. ");
					return 0;
				}
			}
		}

		for (j = 1; j < this->m_stPartyPlayers.nPlayerCount; ++j)
		{
			if (this->m_stPartyPlayers.nPlayersIdx[j] >= 0)
			{
				v3 = this->m_stPartyPlayers.nPlayersIdx[j] <= MAX_OBJECT;
			}
			else
			{
				v3 = 0;
			}

			if (v3)
			{
				lpPartyObj = &gObj[this->m_stPartyPlayers.nPlayersIdx[j]];
				btResult = this->ChkEnterBossZone(lpPartyObj->Index);

				if (lpObj->Map == lpPartyObj->Map)
				{
					if (gObjCalcDistance(lpObj, lpPartyObj) > 12)
					{
						btResult = 6;
						LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Party member don't exist nearby. [%d][%s]", lpPartyObj->Index, lpPartyObj->Name);
					}
				}
				else
				{
					btResult = 6;
					LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Party member don't exist nearby. [%d][%s][%d]", lpPartyObj->Index, lpPartyObj->Name, lpPartyObj->Map);
				}

				if (btResult)
				{
					LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Etc. [%d][%s][%d]", lpPartyObj->Index, lpPartyObj->Name, (unsigned __int8)btResult);
					return 0;
				}
			}
		}
		result = 1;
	}
	else
	{
		LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Change Party Info.");
		result = 0;
	}

	return result;
}

void  CNixiesLakeBossZone::InvitePartyMemberToBossZone(int aIndex)
{
	int n; // [sp+50h] [bp-1Ch]@1
	int PartySlot; // [sp+54h] [bp-18h]@1
	int nPartyNum; // [sp+58h] [bp-14h]@1
	int nPartyUserIndex; // [sp+5Ch] [bp-10h]@4

	nPartyNum = gObj[aIndex].PartyNumber;
	this->m_stPartyPlayers.nPlayersIdx[0] = aIndex;
	PartySlot = 1;
	this->m_stPartyPlayers.nPlayerCount = gParty.m_PartyInfo[nPartyNum].Count;
	this->m_stPartyPlayers.nAcceptCount = 1;
	this->m_stPartyPlayers.dwAnsWaitTimer = GetTickCount() + 60000;

	for (n = 1; n < MAX_PARTY_USER; ++n)
	{
		nPartyUserIndex = gParty.m_PartyInfo[nPartyNum].Index[n];
		if (gObjIsConnected(nPartyUserIndex) == 0)
		{
			break;
		}

		this->m_stPartyPlayers.nPlayersIdx[PartySlot++] = nPartyUserIndex;
		this->SendInvitePartyMemberToBossZone(gObj[nPartyUserIndex].Index);
	}
}

void __thiscall CNixiesLakeBossZone::ChkPartyWaitTime()
{
	if (this->m_stPartyPlayers.dwAnsWaitTimer)
	{
		if (this->m_stPartyPlayers.dwAnsWaitTimer < GetTickCount())
		{
			this->NotifyInviteResultPartyMemberToBossZone(2);
		}
	}
}

void  CNixiesLakeBossZone::SetBossDieTimer()
{
	this->m_dwBossDieTimer = GetTickCount64() + gServerInfo.m_NixDieTime * 60 * 1000;
	this->m_bIsBossLive = 0;
}

void  CNixiesLakeBossZone::ChkBossDieTime()
{
	if (this->m_bIsBossLive == 1)
	{
		if (this->ChkBossZonePlayer() == 0)
		{
			this->SetBossZoneState(0);
		}
		else
		{
			if ((GetTickCount64() - this->m_dwBossLastHitTimer) > 60000)
			{
				this->LeaveBossZone();
				this->SetBossZoneState(0);
			}
		}
	}

	if (this->m_dwBossDieTimer)
	{
		if (this->m_dwBossDieTimer < GetTickCount64())
		{
			this->LeaveBossZone();
			this->m_dwBossDieTimer = 0;
		}
	}
}

void __thiscall CNixiesLakeBossZone::LeaveBossZone()
{
	if (this->ChkBossZonePlayer() == 1)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			if (gObj[n].Map == MAP_KNICKS && this->IsBossZone(gObj[n].X, gObj[n].Y))
			{
				LogAdd(LOG_BLACK, "[NixiesLake][LeaveBossZone] 5 minute Time Over Boss Die: %s", gObj[n].Name);
				gObjMoveGate(n, 522);
				this->SendKickOutUserPopupInBossZone(n);
			}
		}
	}
}

bool  CNixiesLakeBossZone::IsAbleNixiesBossViewPort(LPOBJ lpObj)
{
	if (this->m_bIsBossLive && lpObj->Type == OBJECT_USER && lpObj->Map == MAP_KNICKS)
	{
		return this->IsBossZone(lpObj->X, lpObj->Y) != 0;
	}

	return false;
}

void  CNixiesLakeBossZone::RecvReqChkEnterBossZone(_tagPMSG_REQ_CHK_ENTER_ON_NIXIES_NIX_BOSS_ZONE* lpMsg, int aIndex)
{
	signed int partyNum;
	BYTE btResult;

	if (lpMsg)
	{
		if (gObjIsConnected(aIndex) == 0)
		{
			return;
		}

		/*if (this->ChkBossZonePlayer() == 0 && this->GetBossZoneState() == 1)
		{
			this->SetBossZoneState(0);
		}*/

		btResult = this->ChkEnterBossZone(aIndex);

		partyNum = gObj[aIndex].PartyNumber;

		if (partyNum > -1 && gParty.m_PartyInfo[partyNum].Count > 1)
		{
			if (!gParty.IsLeader(gObj[aIndex].PartyNumber, aIndex))
			{
				btResult = 5;
				LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Applicant is Party Member . [%d][%s]", aIndex, gObj[aIndex].Name);
			}

			if (!btResult)
			{
				btResult = this->ChkEnterPartyMemberToBossZone(aIndex);
			}
		}

		this->SendAnsChkEnterBossZone(aIndex, btResult);
	}
}

void  CNixiesLakeBossZone::SendAnsChkEnterBossZone(int aIndex, BYTE btResult)
{
	_tagPMSG_ANS_CHK_ENTER_ON_NIXIES_NIX_BOSS_ZONE pMsg;
	pMsg.h.set(0x76, 0x01, sizeof(pMsg));
	pMsg.btResult = btResult;
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void  CNixiesLakeBossZone::RecvReqEnterBossZone(_tagPMSG_REQ_ENTER_ON_NIXIES_NIX_BOSS_ZONE* lpMsg, int aIndex)
{
	signed int partyNum;

	if (lpMsg)
	{
		if (gObjIsConnected(aIndex) == 0)
		{
			return;
		}

		/*if (this->ChkBossZonePlayer() == 0 && this->GetBossZoneState() == 1)
		{
			this->SetBossZoneState(0);
		}*/

		if (!this->ChkEnterBossZone(aIndex))
		{
			partyNum = gObj[aIndex].PartyNumber;

			if (partyNum <= -1 || gParty.m_PartyInfo[partyNum].Count <= 1)
			{
				this->EnterBossZone(aIndex);
				this->ClearMonster();
				this->InitNixiesBossZone(0);
				this->SetBossZoneState(1);
			}
			else
			{
				gParty.IsLeader(partyNum, aIndex);

				if (this->ChkEnterPartyMemberToBossZone(aIndex))
				{
					this->SendAnsEnterBossZone(aIndex, -1);
				}
				else
				{
					this->InvitePartyMemberToBossZone(aIndex);
					this->SendAnsEnterBossZone(aIndex, 1);
				}
			}
		}
	}
}

void  CNixiesLakeBossZone::SendAnsEnterBossZone(int aIndex, BYTE btResult)
{
	_tagPMSG_ANS_ENTER_ON_NIXIES_NIX_BOSS_ZONE pMsg;
	pMsg.h.set(0x76, 0x02, sizeof(pMsg));
	pMsg.btResult = btResult;
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void  CNixiesLakeBossZone::SendInvitePartyMemberToBossZone(int aIndex)
{
	_tagPMSG_REQ_INVITE_ON_NIXIES_NIX_BOSS_ZONE_TO_PARTYMEMBER pMsg;
	pMsg.h.set(0x76, 0x03, sizeof(pMsg));
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void  CNixiesLakeBossZone::RecvReqEnterBossZoneByInvite(_tagPMSG_ANS_INVITE_ON_NIXIES_NIX_BOSS_ZONE_WITH_PARTYMEMBER* lpMsg, int aIndex)
{
	BYTE btResponse; // [sp+53h] [bp-5h]@6

	if (lpMsg)
	{
		if (gObjIsConnected(aIndex) == 0)
		{
			return;
		}

		btResponse = lpMsg->btResponse;

		EnterCriticalSection(&this->m_criNixiesBossZone);

		if (this->m_stPartyPlayers.nPlayerCount >= 1)
		{
			if (btResponse == 1)
			{
				if (++this->m_stPartyPlayers.nAcceptCount >= this->m_stPartyPlayers.nPlayerCount)
				{
					this->PartyEnterBossZone();
					this->NotifyInviteResultPartyMemberToBossZone(1);
				}
			}
			else
			{
				this->NotifyInviteResultPartyMemberToBossZone(0);
				LogAdd(LOG_BLACK, "[NixiesLake][EnterBossZone] Fail. Reject Invite. %s", gObj[aIndex].Name);
			}

			LeaveCriticalSection(&this->m_criNixiesBossZone);
		}
		else
		{
			LeaveCriticalSection(&this->m_criNixiesBossZone);
		}
	}
}

void  CNixiesLakeBossZone::NotifyInviteResultPartyMemberToBossZone(BYTE btResult)
{
	int i;

	for (i = 0; i < MAX_PARTY_USER; ++i)
	{
		if (gObjIsConnected(this->m_stPartyPlayers.nPlayersIdx[i]))
		{
			this->SendInviteResultPartyMemberToBossZone(this->m_stPartyPlayers.nPlayersIdx[i], btResult);
		}
	}

	this->m_stPartyPlayers.Clear();
}

void  CNixiesLakeBossZone::SendInviteResultPartyMemberToBossZone(int aIndex, BYTE btResult)
{
	_tagPMSG_REQ_RESULT_ON_NIXIES_NIX_BOSS_ZONE_TO_PARTYMEMBER pMsg; // [sp+4Ch] [bp-Ch]@1
	pMsg.h.set(0x76, 0x04, sizeof(pMsg));
	pMsg.btResult = btResult;
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void  CNixiesLakeBossZone::SendKickOutUserPopupInBossZone(int aIndex)
{
	_tagPMSG_SEND_KICK_OUT_USER_IN_BOSSZONE pMsg;
	pMsg.h.set(0x76, 0x09, sizeof(pMsg));
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}