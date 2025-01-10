#pragma once
class CBombHuntDBSet
{
public:
	CBombHuntDBSet();
	~CBombHuntDBSet();

	int  CBombHuntDBSet::DBSelectBombHunt(char *szAccountID, char *szName, unsigned __int16 *wOutScore, BYTE *btOutGameState, char *szOutTileState); // idb
	int  CBombHuntDBSet::DBInsertBombHunt(char *szAccountID, char *szName, unsigned __int16 wScore, unsigned __int16 wGameState, BYTE *szTileState); // idb
	int  CBombHuntDBSet::DBDeleteBombHunt(char *szAccountID, char *szName); // idb
	int  CBombHuntDBSet::DBInsertBombHuntLog(char *szAccountID, char *szName, unsigned __int16 wScore, BYTE btClear); // idb

};

extern CBombHuntDBSet g_BombHuntDBSet;