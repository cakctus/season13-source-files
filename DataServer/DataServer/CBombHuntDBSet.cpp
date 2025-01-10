#include "stdafx.h"
#include "QueryManager.h"
#include "Util.h"
#include "CBombHuntDBSet.h"

CBombHuntDBSet g_BombHuntDBSet;

CBombHuntDBSet::CBombHuntDBSet()
{
}


CBombHuntDBSet::~CBombHuntDBSet()
{
}


//----- (0042D140) --------------------------------------------------------
int  CBombHuntDBSet::DBSelectBombHunt(char *szAccountID, char *szName, unsigned __int16 *wOutScore, BYTE *btOutGameState, char *szOutTileState)
{
	
	if (gQueryManager.ExecQuery("EXEC WZ_BombHuntSetSelect '%s', '%s'", szAccountID, szName))
	{
		__int16 sqlReturn = gQueryManager.Fetch();
		if (sqlReturn== SQL_NO_DATA)
		{
			gQueryManager.Close();
			return 0;
		}
		else
		{
			*wOutScore = gQueryManager.GetAsInteger("Score");
			*btOutGameState = gQueryManager.GetAsInteger("GameState");
			gQueryManager.GetAsString("TileState", szOutTileState, sizeof(szOutTileState));
			gQueryManager.Close();
			return 1;
		}
	}

	else
	{
		gQueryManager.Close();
		LogAdd(LOG_BLACK,"error-L3 : [BombHuntDBSet] DBSelectBombHuntInfo #1  ");
		return 0;
	}
	return 0;
}
// 5CD868: using guessed type int `CBombHuntDBSet::DBSelectBombHunt'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (0042D350) --------------------------------------------------------
int  CBombHuntDBSet::DBInsertBombHunt(char *szAccountID, char *szName, unsigned __int16 wScore, unsigned __int16 wGameState, BYTE *szTileState)
{
	if (gQueryManager.ExecQuery("EXEC WZ_BombHuntSetSave '%s', '%s', %d, %d, '%s'", szAccountID, szName, wGameState, wScore, szTileState))
	{
		gQueryManager.Close();
		return 1;
	}

	else
	{
		gQueryManager.Close();

		LogAdd(LOG_BLACK,
			"error-L3 : [BombHunt] DBInsertBombHunt #1 [%s][%s]",
			szAccountID,
			szName);
		return 0;

	}
	return 0;
}
// 5CD86C: using guessed type int `CBombHuntDBSet::DBInsertBombHunt'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (0042D4D0) --------------------------------------------------------
int  CBombHuntDBSet::DBDeleteBombHunt(char *szAccountID, char *szName)
{
	

	if (gQueryManager.ExecQuery("EXEC WZ_BombHuntSetDelete '%s', '%s'", szAccountID, szName))
	{
		gQueryManager.Close();
		return 1;
	}

	else
	{
		gQueryManager.Close();

		LogAdd(LOG_BLACK,
			"error-L3 : [BombHunt] DBDeleteBombHunt #1 [%s][%s] ",
			szAccountID,
			szName);
		return 0;
	}
	return 0;
}
// 5CD870: using guessed type int `CBombHuntDBSet::DBDeleteBombHunt'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (0042D630) --------------------------------------------------------
int  CBombHuntDBSet::DBInsertBombHuntLog(char *szAccountID, char *szName, unsigned __int16 wScore, BYTE btClear)
{

	if (gQueryManager.ExecQuery("EXEC WZ_BombHuntLogSetSave  '%s', '%s', %d, %d", szAccountID, szName, wScore, btClear))
	{
		gQueryManager.Close();
		return 1;
	}

	else
	{
		gQueryManager.Close();

		LogAdd(LOG_BLACK,
			"error-L3 : [MuRummy] DBInsertBombHuntLog #1 [%s][%s]",
			szAccountID,
			szName);
		return 0;
	}
	return 0;
}