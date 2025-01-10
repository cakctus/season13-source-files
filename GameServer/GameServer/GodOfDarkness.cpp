#include "stdafx.h"
#include "User.h"
#include "Util.h"
#include "GodOfDarkness.h"
#include "Monster.h"
#include "Map.h"
#include "Party.h"
#include "ItemManager.h"
#include "Message.h"
#include "ServerInfo.h"

GodOfDarkness g_GodOfDarkness;


GodOfDarkness::GodOfDarkness()
{
	
}

GodOfDarkness::~GodOfDarkness()
{

}

void GodOfDarkness::RecvReqChkEnterBossZone(SWAMP_OF_DARKNESS_BOSS_ENTER_RESULT* lpMsg, int aIndex)
{
	if (lpMsg)
	{
		if (gObjIsConnected(aIndex) == 0)
		{
			return;
		}

		if (this->CountBossZonePlayers() >= gServerInfo.m_GodOfDarknessMaxPlayers)
		{
			SWAMP_OF_DARKNESS_BOSS_ENTER_RESULT pMsg;
			pMsg.h.set(0x76, 0x21, sizeof(pMsg));
			pMsg.result = 2;
			DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
		}
		else if (!this->CheckBossLive())
		{
			GCMessagePopupSend(&gObj[aIndex], "O God of Darkness nao esta presente no momento, e por isso voce nao pode entrar.");
		}
		else
		{
			gObjMoveGate(aIndex, 590);
		}
	}
}

int GodOfDarkness::CountBossZonePlayers()
{
	int count = 0;

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0)
		{
			LPOBJ lpObj = &gObj[n];
			if(this->IsBossZone(lpObj->Map, lpObj->X, lpObj->Y))
			{
				count++;
			}
		}
	}

	return count;
}

void GodOfDarkness::RemoveBossZonePlayers()
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0)
		{
			LPOBJ lpObj = &gObj[n];
			if (this->IsBossZone(lpObj->Map, lpObj->X, lpObj->Y))
			{
				gObjMoveGate(n, 567);
			}
		}
	}
}

bool GodOfDarkness::IsBossZone(int map, int x, int y)
{
	if (map == MAP_SWAMP_OF_DARKNESS && x >= 174 && x <= 194 && y >= 8 && y <= 28)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool GodOfDarkness::CheckBossLive()
{
	for (int n = 0; n < MAX_OBJECT_MONSTER; n++)
	{
		if (gObjIsConnected(n) != 0 && gObj[n].Type == OBJECT_MONSTER && gObj[n].Class == 794)
		{
			LPOBJ lpObj = &gObj[n];
			if (this->IsBossZone(lpObj->Map, lpObj->X, lpObj->Y))
			{
				if (lpObj->Live == 1)
				{
					return true;
				}
			}
		}
	}

	return false;
}

int GodOfDarkness::GetBossRegenTime()
{
	for (int n = 0; n < MAX_OBJECT_MONSTER; n++)
	{
		if (gObjIsConnected(n) != 0 && gObj[n].Type == OBJECT_MONSTER && gObj[n].Class == 794)
		{
			LPOBJ lpObj = &gObj[n];
			return (GetTickCount() - lpObj->RegenTime) / 1000 / 60;
		}
	}

	return 0;
}

void GodOfDarkness::Process()
{
	if (this->CheckBossLive())
	{
		this->SendStatus(0);
	}
	else if (this->GetBossRegenTime() > 0 && this->GetBossRegenTime() <= gServerInfo.m_GodOfDarknessMaxPlayers)
	{
		this->SendStatus(4);
		this->RemoveBossZonePlayers();
	}
	else
	{
		this->SendStatus(3);
	}
}

void GodOfDarkness::SendStatus(int status)
{
	SWAMP_OF_DARKNESS_BOSS_AREA pMsg;
	pMsg.header.set(0x76, 0x25, sizeof(pMsg));
	pMsg.state = status;

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0 && gObj[n].Type == OBJECT_USER && gObj[n].Map == MAP_SWAMP_OF_DARKNESS)
		{
			DataSend(n, (LPBYTE)&pMsg, pMsg.header.size);
		}
	}
}