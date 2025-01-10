#pragma once
#include "Protocol.h"

enum EN_BOMB_HUNT_TILE_STATE
{
	EN_TILE_STATE_CLOSE = 0x0,
	EN_TILE_STATE_CARD = 0x1,
	EN_TILE_STATE_OPEN = 0x2,
	EN_TILE_STATE_BOMB = 0x4,
	EN_TILE_STATE_CARD_BOMB = 0x5,
	EN_TILE_STATE_OPEN_BOMB = 0x6,
};
/* 426 */
enum EN_MURUMMYCARD_SP_TYPE
{
	EN_MURUMMYCARD_SP_TYPE_NUMBER = 0x4,
	EN_MURUMMYCARD_SP_TYPE_FUNCTION = 0x5,
};
/* 427 */
enum EN_BOMB_HUNT_GAME_STATE
{
	EN_GAME_NONE = 0x0,
	EN_GAME_READY = 0x1,
	EN_GAME_PLAY = 0x2,
	EN_GAME_OVER = 0x3,
	EN_GAME_RESULT = 0x4,
	EN_GAME_REWARD = 0x5,
};

/* 6237 */
struct _tagPMSG_REQ_BOMB_HUNT_LB_CLICK
{
	PSBMSG_HEAD h;
	BYTE btPos;
};
/* 6248 */
struct _tagPMSG_ANS_BOMB_HUNT_RB_CLICK
{
	PSBMSG_HEAD h;
	BYTE btPos;
	BYTE btTileState;
	BYTE btRemainBombCard;
};
struct _tagPMSG_REQ_BOMB_HUNT_SELECT_DS
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	unsigned __int16 aIndex;
};
/* 6418 */
struct _tagPMS_REQ_BOMB_HUNT_GAME_END
{
	PSBMSG_HEAD h;
};
struct  _tagPMSG_ANS_BOMB_HUNT_START
{
	PSBMSG_HEAD h;
	BYTE btResult;
};
/* 6453 */
struct __declspec(align(2)) _tagPMSG_REQ_BOMB_HUNT_LOG_INSERT_DS
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD wScore;
	BYTE btClear;
};
/* 6581 */
struct _tagPMSG_ANS_BOMB_HUNT_SELECT_DS
{
	PSBMSG_HEAD h;
	unsigned __int16 aIndex;
	BYTE btResult;
	unsigned __int16 wScore;
	BYTE btGameState;
	BYTE szTileState[49];
};

/* 6962 */
struct  _tagPMSG_SEND_BOMB_HUNT_GAME_SET
{
	PSBMSG_HEAD h;
	BYTE btEventTime[4];
	BYTE btWidth;
	BYTE btHeight;
	BYTE btRemainBombCard;
};
/* 6981 */
struct  _tagTileState
{
	BYTE btPos;
	BYTE btTileState;
};
struct _tagPMSG_REQ_BOMB_HUNT_DELETE_DS
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};

/* 7114 */
struct _tagPMSG_REQ_BOMB_HUNT_RB_CLICK
{
	PSBMSG_HEAD h;
	BYTE btPos;
};
/* 7117 */
struct _tagTileInfo
{
	BYTE btPos;
	BYTE btTileInfo;
};
/* 7058 */
struct  _tagPMSG_ANS_BOMB_HUNT_BANNER
{
	PSBMSG_HEAD h;
	BYTE btResult;
	BYTE btRemainBombCard;
	BYTE btCnt;
	WORD wScore;
};
struct  _tagPMS_ANS_BOMB_HUNT_GAME_END
{
	PSBMSG_HEAD h;
	BYTE btResult;
	BYTE btCnt;
	WORD wNumScore;
	WORD wBombFind;
	WORD wBombFail;
	WORD wBombTread;
	WORD wClear;
	WORD wTotScore;
};
struct _tagPMSG_ANS_BOMB_HUNT_REWARD
{
	PSBMSG_HEAD h;
	BYTE btResult;
};
/* 7577 */
struct _tagPMSG_REQ_BOMB_HUNT_INSERT_DS
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
	WORD wScore;
	BYTE btGameState;
	BYTE szTileState[49];
};
/* 7585 */
struct  _tagPMSG_ANS_BOMB_HUNT_LB_CLICK
{
	PSBMSG_HEAD h;
	BYTE btPos;
	BYTE btScoreH;
	BYTE btScoreL;
	BYTE btCnt;
};
class CBombHuntMng
{
public:
	CBombHuntMng();
	~CBombHuntMng();

	void SendBombHuntEventInfo(int aIndex); // idb
	char IsUserPlay(int aIndex);
	void CheckTileAround(int indexRow, int indexColumn, int *tempTile);
	char GenerateBomb(int tileIndex, CBombHuntInfo *pCBombHuntInfo);
	int GetAdjustMineCount(int indexRow, int indexColumn, CBombHuntInfo *pCBombHuntInfo);
	void CheckTileState(int indexRow, int indexColumn, CBombHuntInfo *pCBombHuntInfo); // idb
	void CGReqBombHuntBanner(int aIndex); // idb
	void CGReqBombHuntStart(int aIndex); // idb
	void CGReqLBClick(_tagPMSG_REQ_BOMB_HUNT_LB_CLICK *lpMsg, int aIndex); // idb
	void CGReqRBClick(_tagPMSG_REQ_BOMB_HUNT_RB_CLICK *lpMsg, int aIndex); // idb
	void CGReqGameClose(_tagPMS_REQ_BOMB_HUNT_GAME_END *lpMsg, int aIndex); // idb
	void GameClose(int aIndex); // idb
	void SendGameResult(int aIndex); // idb
	void CGReqReward(int aIndex); // idb
	void GDReqBombHuntSelect(LPOBJ lpObj); // idb
	void GDAnsBombHuntSelect(_tagPMSG_ANS_BOMB_HUNT_SELECT_DS *lpMsg); // idb
	void GDReqBombHuntInsert(LPOBJ lpObj); // idb
	void GDReqBombHuntDelete(LPOBJ lpObj); // idb
	void GDReqBombHuntDBLog(LPOBJ lpObj, int iScore, char btClear); // idb
	void ReStart(int aIndex); // idb
	void SendTileInfo(int aIndex); // idb
private:
	int m_iRecusiveCnt;
	int m_OpenTileIndex[48];
};

extern CBombHuntMng g_CBombHuntMng;
