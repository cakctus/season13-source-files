#pragma once
class CBombHuntInfo
{
public:
	CBombHuntInfo();
	~CBombHuntInfo();
	void Init(); // idb
	void Clear(); // idb
	void SetScore(int iScore); // idb
	void SetGameState(int iGameState); // idb
	int GetScore(); // idb
	int GetGameState(); // idb
	void SetBombHuntInfoLoad(bool bBombHuntInfoLoad); // idb
	bool GetBombHuntInfoLoad(); // idb
public:
	int m_tileAdjustBombCount[48];
	int m_tileState[48];
	bool m_bBombExist[48];
	int m_nRemainBombCard;
	int m_nClosedTile;
	int m_iGameState;
	int m_iTotScore;
	bool m_bBombHuntInfoLoad;
};
