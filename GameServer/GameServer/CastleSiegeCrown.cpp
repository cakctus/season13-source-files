#include "stdafx.h"
#include "CastleSiegeCrown.h"
#include "CastleSiege.h"
#include "Map.h"
#include "Util.h"
#include "DSProtocol.h"

CCastleSiegeCrown gCastleSiegeCrown;

CCastleSiegeCrown::CCastleSiegeCrown() // OK
{

}

CCastleSiegeCrown::~CCastleSiegeCrown() // OK
{

}

void CCastleSiegeCrown::CastleSiegeCrownAct(int aIndex) // OK
{
	#if(GAMESERVER_TYPE==1)

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_NPC || lpObj->Class != 216)
	{
		return;
	}

	int bIndex = gCastleSiege.GetCrownUserIndex();

	if(gObjIsConnected(bIndex) == 0)
	{
		gCastleSiege.SetCrownAccessUserX(0);
		gCastleSiege.SetCrownAccessUserY(0);
		gCastleSiege.ResetCrownUserIndex();
		gCastleSiege.ResetCrownAccessTickCount();
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Map != MAP_CASTLE_SIEGE || lpTarget->CsJoinSide < 2 || lpTarget->CsGuildInvolved == 0)
	{
		GCAnsCsAccessCrownState(bIndex,2);
		gCastleSiege.SetCrownAccessUserX(0);
		gCastleSiege.SetCrownAccessUserY(0);
		gCastleSiege.ResetCrownUserIndex();
		gCastleSiege.ResetCrownAccessTickCount();
		LogAdd(LOG_BLACK, "[CastleSiege] [%s][%s][%s] Crown Canceled: Fora do mapa ou da guild adequada", lpTarget->Account, lpTarget->Name, lpTarget->GuildName);
		return;
	}

	if(lpTarget->Live == 0 || lpTarget->X != gCastleSiege.GetCrownAccessUserX() || lpTarget->Y != gCastleSiege.GetCrownAccessUserY())
	{
		GCAnsCsAccessCrownState(bIndex,2);
		gCastleSiege.SetCrownAccessUserX(0);
		gCastleSiege.SetCrownAccessUserY(0);
		gCastleSiege.ResetCrownUserIndex();
		gCastleSiege.ResetCrownAccessTickCount();
		LogAdd(LOG_BLACK, "[CastleSiege] [%s][%s][%s] Crown Canceled: Usuario movido", lpTarget->Account, lpTarget->Name, lpTarget->GuildName);
		
		if (lpTarget->LastBackSpring > 0 && gObjIsConnected(lpTarget->LastBackSpring))
		{
			gCastleSiege.SaveSiegeCharInfo(&gObj[lpTarget->LastBackSpring], 14);
			GDBackSpringSave(&gObj[lpTarget->LastBackSpring], lpTarget, 1); //coroa
		}

		return;
	}

	if(gObjIsConnected(gCastleSiege.GetCrownSwitchUserIndex(217)) == 0 || gObjIsConnected(gCastleSiege.GetCrownSwitchUserIndex(218)) == 0)
	{
		GCAnsCsAccessCrownState(bIndex,2);
		gCastleSiege.SetCrownAccessUserX(0);
		gCastleSiege.SetCrownAccessUserY(0);
		gCastleSiege.ResetCrownUserIndex();
		gCastleSiege.ResetCrownAccessTickCount();
		LogAdd(LOG_BLACK, "[CastleSiege] [%s][%s][%s] Crown Canceled: Usuario da Switch desconectado", lpTarget->Account, lpTarget->Name, lpTarget->GuildName);
		return;
	}

	if(lpTarget->CsJoinSide != gObj[gCastleSiege.GetCrownSwitchUserIndex(217)].CsJoinSide || lpTarget->CsJoinSide != gObj[gCastleSiege.GetCrownSwitchUserIndex(218)].CsJoinSide)
	{
		GCAnsCsAccessCrownState(bIndex,2);
		gCastleSiege.SetCrownAccessUserX(0);
		gCastleSiege.SetCrownAccessUserY(0);
		gCastleSiege.ResetCrownUserIndex();
		gCastleSiege.ResetCrownAccessTickCount();
		LogAdd(LOG_BLACK,"[CastleSiege] [%s][%s][%s] Register Castle Crown Canceled",lpTarget->Account,lpTarget->Name, lpTarget->GuildName);
		return;
	}

	LogAdd(LOG_BLACK,"[CastleSiege] [Reg. Accumulating] Accumulated Crown AccessTime : acc(%d) + %d [%s](%s)(%s)",lpTarget->AccumulatedCrownAccessTime,(GetTickCount()-gCastleSiege.GetCrownAccessTickCount()),lpTarget->GuildName,lpTarget->Account,lpTarget->Name);
	
	gCastleSiege.SaveSiegeCharInfo(lpTarget, 12, lpTarget->AccumulatedCrownAccessTime + (GetTickCount() - gCastleSiege.GetCrownAccessTickCount())); //status de tempo acumulado para live
	
	#endif
}
