#include "stdafx.h"
#include "User.h"
#include "Util.h"
#include"ServerInfo.h"
#include "ItemManager.h"
#include "EventInventory.h"
#include "GremoryCase.h"
#include "GameMain.h"
#include "CBombHuntMng.h"

CBombHuntMng g_CBombHuntMng;

CBombHuntMng::CBombHuntMng()
{
	this->m_iRecusiveCnt = 0;
}


CBombHuntMng::~CBombHuntMng()
{
}


//----- (00A646A0) --------------------------------------------------------
void  CBombHuntMng::SendBombHuntEventInfo(int aIndex)
{
	CTime ExpireTime(gServerInfo.m_EventInventoryExpireYear, gServerInfo.m_EventInventoryExpireMonth, gServerInfo.m_EventInventoryExpireDay, 0, 0, 0, -1);

	DWORD time = (DWORD)(ExpireTime.GetTime() - CTime::GetTickCount().GetTime());

	_tagPMSG_SEND_BOMB_HUNT_GAME_SET pMsg;

	pMsg.h.set(0x4D, 0x29, sizeof(pMsg));

	pMsg.btEventTime[0] = SET_NUMBERHB(SET_NUMBERHW(time));

	pMsg.btEventTime[1] = SET_NUMBERLB(SET_NUMBERHW(time));

	pMsg.btEventTime[2] = SET_NUMBERHB(SET_NUMBERLW(time));

	pMsg.btEventTime[3] = SET_NUMBERLB(SET_NUMBERLW(time));

	pMsg.btWidth = 8;

	pMsg.btHeight= 6;

	pMsg.btRemainBombCard = 11;

	DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
}

//----- (00A647A0) --------------------------------------------------------
char  CBombHuntMng::IsUserPlay(int aIndex)
{


		if (gObjIsConnected(aIndex))
		{
			LPOBJ lpObj = &gObj[aIndex];

			if (lpObj->Interface.type & 3&& (lpObj->Interface.type == 6|| lpObj->Interface.type == 3|| lpObj->Interface.type == 12))
			{

				return 0;
			}
			else if (lpObj->CloseType == -1)
			{
				//if (CNewPVP::IsObserver(&g_NewPVP, &gObj[aIndex]))
				//{
				//	v2 = CMsg::Get((CMsg *)&lMsg, 3428);
				//	GCServerMsgStringSend(v2, aIndex, 1);
				//	result = 0;
				//}

				if (lpObj->MapServerMoveRequest == 1)
				{
					LogAdd(LOG_BLACK,
						"[BombHunt] MapServerMove User. return!! [%s], IP [%s] ",
						lpObj->Account,
						lpObj->IpAddr);
					return 0;
				}
				else if (lpObj->State == 32)
				{
					LogAdd(LOG_BLACK,
						"[BombHunt] MapServerMove User. return!! [%s], IP [%s] ",
						lpObj->Account,
						lpObj->IpAddr);
					return 0;
				}
				else if (gObj[aIndex].MapServerMoveQuit == 1)
				{
					LogAdd(LOG_BLACK,
						"[BombHunt] MapServerMove User. return!! [%s], IP [%s] ",
						lpObj->Account,
						lpObj->IpAddr);
					return 0;
				}
				else
				{
					return 1;
				}
			}
			
		}	

	return 0;
}
// 139746C: using guessed type int `CBombHuntMng::IsUserPlay'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// A72693C: using guessed type void (*LogAdd)(char *, ...);
// 10839630: using guessed type char *__thiscall CMsg::Get(CMsg *__hidden this, _DWORD);

//----- (00A64BA0) --------------------------------------------------------
void  CBombHuntMng::CheckTileAround(int indexRow, int indexColumn, int *tempTile)
{
	int index; // [sp+4Ch] [bp-8h]@1

	index = indexColumn + 8 * indexRow;
	if (indexColumn - 1 >= 0 && indexRow - 1 >= 0)
		tempTile[index - 9] = 1;
	if (indexColumn >= 0 && indexRow - 1 >= 0)
		tempTile[index - 8] = 1;
	if (indexColumn + 1 < 8 && indexRow - 1 >= 0)
		tempTile[index - 7] = 1;
	if (indexColumn - 1 >= 0 && indexRow >= 0)
		tempTile[index - 1] = 1;
	if (indexColumn + 1 < 8 && indexRow >= 0)
		tempTile[index + 1] = 1;
	if (indexColumn - 1 >= 0 && indexRow + 1 < 6)
		tempTile[index + 7] = 1;
	if (indexColumn >= 0 && indexRow + 1 < 6)
		tempTile[index + 8] = 1;
	if (indexColumn + 1 < 8 && indexRow + 1 < 6)
		tempTile[index + 9] = 1;
}

//----- (00A64D10) --------------------------------------------------------
char  CBombHuntMng::GenerateBomb( int tileIndex, CBombHuntInfo *pCBombHuntInfo)
{
	int i; // [sp+4Ch] [bp-E0h]@10
	int tempTile[48]; // [sp+50h] [bp-DCh]@1
	int tempMineCount; // [sp+110h] [bp-1Ch]@1
	int indexColumn; // [sp+114h] [bp-18h]@1
	int indexRow; // [sp+118h] [bp-14h]@1
	int tempIndex; // [sp+11Ch] [bp-10h]@1
	int nGenratedBomb; // [sp+120h] [bp-Ch]@1
	int nLoopCnt; // [sp+124h] [bp-8h]@1

	nLoopCnt = 0;
	nGenratedBomb = 0;
	tempIndex = 0;
	tempMineCount = 0;
	pCBombHuntInfo->m_nClosedTile = 48;
	memset(&tempTile, 0, sizeof(tempTile));
	indexRow = tileIndex / 8;
	indexColumn = tileIndex % 8;
	this->CheckTileAround(tileIndex / 8, tileIndex % 8, tempTile);
	while (nGenratedBomb != 11)
	{
		if (++nLoopCnt > 100)
		{
			LogAdd(LOG_BLACK,"[BombHunt][Error] GenerateBomb() return!!");
			return 0;
		}
		tempIndex = GetLargeRand() % 0x30;
		if (tempIndex != tileIndex && !pCBombHuntInfo->m_bBombExist[tempIndex] && !tempTile[tempIndex])
		{
			pCBombHuntInfo->m_bBombExist[tempIndex] = 1;
			++nGenratedBomb;
		}
	}
	for (i = 0; i < 48; ++i)
	{
		indexRow = i / 8;
		indexColumn = i % 8;
		tempMineCount = this->GetAdjustMineCount(i / 8, i % 8, pCBombHuntInfo);
		pCBombHuntInfo->m_tileAdjustBombCount[i] = tempMineCount;
	}
	return 1;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A64F10) --------------------------------------------------------
int  CBombHuntMng::GetAdjustMineCount(int indexRow, int indexColumn, CBombHuntInfo *pCBombHuntInfo)
{
	int index; // [sp+4Ch] [bp-Ch]@1
	int nBombCount; // [sp+50h] [bp-8h]@1

	nBombCount = 0;
	index = indexColumn + 8 * indexRow;
	if (indexColumn - 1 >= 0 && indexRow - 1 >= 0 && *((BYTE *)&pCBombHuntInfo->m_tileState[45] + index + 3))
		nBombCount = 1;
	if (indexColumn >= 0 && indexRow - 1 >= 0 && *((BYTE *)&pCBombHuntInfo->m_tileState[46] + index))
		++nBombCount;
	if (indexColumn + 1 < 8 && indexRow - 1 >= 0 && *((BYTE *)&pCBombHuntInfo->m_tileState[46] + index + 1))
		++nBombCount;
	if (indexColumn - 1 >= 0 && indexRow >= 0 && *((BYTE *)&pCBombHuntInfo->m_tileState[47] + index + 3))
		++nBombCount;
	if (indexColumn + 1 < 8 && indexRow >= 0 && pCBombHuntInfo->m_bBombExist[index + 1])
		++nBombCount;
	if (indexColumn - 1 >= 0 && indexRow + 1 < 6 && pCBombHuntInfo->m_bBombExist[index + 7])
		++nBombCount;
	if (indexColumn >= 0 && indexRow + 1 < 6 && pCBombHuntInfo->m_bBombExist[index + 8])
		++nBombCount;
	if (indexColumn + 1 < 8 && indexRow + 1 < 6 && pCBombHuntInfo->m_bBombExist[index + 9])
		++nBombCount;
	return nBombCount;
}

//----- (00A65100) --------------------------------------------------------
void  CBombHuntMng::CheckTileState(int indexRow, int indexColumn, CBombHuntInfo *pCBombHuntInfo)
{
	int index; // [sp+50h] [bp-Ch]@10

	if (++this->m_iRecusiveCnt <= 385)
	{
		if (indexRow >= 0 && indexRow < 6 && indexColumn >= 0 && indexColumn < 8)
		{
			if (pCBombHuntInfo)
			{
				index = indexColumn + 8 * indexRow;
				if (!pCBombHuntInfo->m_tileState[indexColumn + 8 * indexRow] && index >= 0 && index < 48)
				{
					pCBombHuntInfo->m_tileState[index] = 2;
					--pCBombHuntInfo->m_nClosedTile;
					this->m_OpenTileIndex[index] = 2;
					if (pCBombHuntInfo->m_bBombExist[index])
					{
						pCBombHuntInfo->SetGameState(4);

						pCBombHuntInfo->m_tileState[index] = 6;
						LogAdd(LOG_BLACK,
							"[BombHunt][Error] CheckTileState() #01 ");
					}
					else if (this->GetAdjustMineCount(indexRow, indexColumn, pCBombHuntInfo) <= 0)
					{
						this->CheckTileState(indexRow - 1, indexColumn - 1, pCBombHuntInfo);
						this->CheckTileState(indexRow - 1, indexColumn, pCBombHuntInfo);
						this->CheckTileState(indexRow - 1, indexColumn + 1, pCBombHuntInfo);
						this->CheckTileState(indexRow, indexColumn - 1, pCBombHuntInfo);
						this->CheckTileState(indexRow, indexColumn + 1, pCBombHuntInfo);
						this->CheckTileState(indexRow + 1, indexColumn - 1, pCBombHuntInfo);
						this->CheckTileState(indexRow + 1, indexColumn, pCBombHuntInfo);
						this->CheckTileState(indexRow + 1, indexColumn + 1, pCBombHuntInfo);
					}
				}
			}
			else
			{
				LogAdd(LOG_BLACK,
					"[BombHunt][Error] pCBombHuntInfo is NULL ");
			}
		}
	}
	else
	{
		LogAdd(LOG_BLACK,
			"[BombHunt][Error] CheckTileState()");
	}
}
// 1397470: using guessed type int `CBombHuntMng::CheckTileState'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A653D0) --------------------------------------------------------
void  CBombHuntMng::CGReqBombHuntBanner(int aIndex)
{

	BYTE nCnt; // [sp+50h] [bp-414h]@16
	int lOfs; // [sp+54h] [bp-410h]@16
	_tagTileInfo strTileInfo; // [sp+58h] [bp-40Ch]@20
	BYTE sendbuf[1000]; // [sp+5Ch] [bp-408h]@23
	int nScore; // [sp+444h] [bp-20h]@16
	CBombHuntInfo *pCBombHuntInfo; // [sp+448h] [bp-1Ch]@4
	_tagPMSG_ANS_BOMB_HUNT_BANNER pMsg; // [sp+44Ch] [bp-18h]@4


	if (this->IsUserPlay(aIndex))
	{
		LPOBJ lpObj = &gObj[aIndex];
		if (&gObj[aIndex])
		{

			PHeadSubSetB((LPBYTE)&pMsg, 0x4D, 0x27, sizeof(pMsg));
			pCBombHuntInfo = 0;
			pCBombHuntInfo = lpObj->m_pCBombHuntInfo;
			if (pCBombHuntInfo)
			{
				if (pCBombHuntInfo->GetBombHuntInfoLoad())
				{
					if (pCBombHuntInfo->GetGameState())
					{
						if (pCBombHuntInfo->GetGameState() != 5 && pCBombHuntInfo->GetGameState() != 4)
						{
							if (pCBombHuntInfo->GetGameState() != 1 || pCBombHuntInfo->m_nRemainBombCard != 11)
							{
								nScore = 0;
								nCnt = 0;
								lOfs = sizeof(pMsg);
								for (int i = 0; i < 48; ++i)
								{
									if (pCBombHuntInfo->m_tileState[i] == 2 || pCBombHuntInfo->m_tileState[i] == 1)
									{
										strTileInfo.btPos = i;
										if (pCBombHuntInfo->m_tileState[i] == 1)
											strTileInfo.btTileInfo = -2;
										else
											strTileInfo.btTileInfo = pCBombHuntInfo->m_tileAdjustBombCount[i];
										memcpy(&sendbuf[lOfs], &strTileInfo.btPos, sizeof(strTileInfo));
										lOfs += 2;
										++nCnt;
									}
								}
								pMsg.btRemainBombCard = pCBombHuntInfo->m_nRemainBombCard;
								pMsg.btResult = 2;
								pMsg.wScore = pCBombHuntInfo->GetScore();
								pMsg.btCnt = nCnt;
								pMsg.h.size = lOfs;
								memcpy(sendbuf, (BYTE *)&pMsg, sizeof(pMsg));
								DataSend(aIndex, sendbuf, lOfs);
							}
							else
							{
								pMsg.btResult = 2;
								DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
							}
						}
						else
						{
							pMsg.btResult = 3;
							DataSend(aIndex, (BYTE *)&pMsg, (unsigned __int8)pMsg.h.size);
							this->SendGameResult(aIndex);
						}
					}
					else
					{
						pMsg.btResult = 1;
						DataSend(aIndex, (BYTE *)&pMsg, (unsigned __int8)pMsg.h.size);
					}
				}
				else
				{
					LogAdd(LOG_BLACK,"[BombHunt][Error] BombHuntInfoLoad fail [%s][%s]", lpObj->Account, lpObj->Name);
				}
			}
			else
			{
				LogAdd(LOG_BLACK,"[BombHunt][Error] pCBombHuntInfo is NULL");
			}
		}
		else
		{
			LogAdd(LOG_BLACK,"[BombHunt][Error] lpObj is NULL ");
		}
	}
}
// 1397474: using guessed type int `CBombHuntMng::CGReqBombHuntBanner'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A65790) --------------------------------------------------------
void  CBombHuntMng::CGReqBombHuntStart(int aIndex)
{

	unsigned int dwRummyItemSerialNumber; // [sp+50h] [bp-20h]@7
	signed int nRummyItemPos; // [sp+54h] [bp-1Ch]@7
	bool bRummyItem; // [sp+5Bh] [bp-15h]@7
	_tagPMSG_ANS_BOMB_HUNT_START pMsg; // [sp+5Ch] [bp-14h]@7
	CBombHuntInfo *pCBombHuntInfo; // [sp+64h] [bp-Ch]@4

	if (this->IsUserPlay(aIndex))
	{
		LPOBJ lpObj = &gObj[aIndex];
		if (&gObj[aIndex])
		{

			pCBombHuntInfo = lpObj->m_pCBombHuntInfo;
			if (pCBombHuntInfo)
			{
				if (!pCBombHuntInfo->GetGameState())
				{

					PHeadSubSetB((LPBYTE )&pMsg, 0x4D, 0x23, 5);
					bRummyItem = 0;
					nRummyItemPos = -1;
					dwRummyItemSerialNumber = 0;
					for (int i = 0; i < 32; ++i)
					{
						if (lpObj->EventInventory[i].IsItem() == 1 && lpObj->EventInventory[i].m_Index == GET_ITEM(14,216))
						{
							bRummyItem = 1;
							nRummyItemPos = i;
							dwRummyItemSerialNumber = lpObj->EventInventory[i].m_Serial ;
							break;
						}
					}
					if (bRummyItem)
					{
						gEventInventory.EventInventoryDelItem(aIndex, nRummyItemPos);
						gEventInventory.GCEventItemDeleteSend(aIndex, nRummyItemPos, 1);

						LogAdd(LOG_BLACK,"[%s][%s][BombHunt] MiniGameStart", lpObj->Account, lpObj->Name);
						LogAdd(LOG_BLACK,
							"[%s][%s][BombHunt] MiniGame Card Deck Item Delete (%lu)",
							lpObj->Account,
							lpObj->Name,
							dwRummyItemSerialNumber);

						pMsg.btResult = 0;
						DataSend(aIndex, (BYTE *)&pMsg, (unsigned __int8)pMsg.h.size);
						pCBombHuntInfo->Clear();
						pCBombHuntInfo->SetGameState(1);
					}
					else
					{
						LogAdd(LOG_BLACK, "[%s][%s][BombHunt] Event Start Fali (0)", lpObj->Account, lpObj->Name);
						pMsg.btResult = 1;
						DataSend(aIndex, (BYTE *)&pMsg, (unsigned __int8)pMsg.h.size);
					}
				}
			}
			else
			{
				LogAdd(LOG_BLACK, 
					"[BombHunt][Error] pCBombHuntInfo is NULL ");
			}
		}
		else
		{
			LogAdd(LOG_BLACK, 
				"[BombHunt][Error] lpObj is NULL");
		}
	}
}
// 1397478: using guessed type int `CBombHuntMng::CGReqBombHuntStart'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A65A90) --------------------------------------------------------
void  CBombHuntMng::CGReqLBClick( _tagPMSG_REQ_BOMB_HUNT_LB_CLICK *lpMsg, int aIndex)
{

	int nTileBombCnt; // [sp+54h] [bp-C34h]@25

	BYTE nCnt; // [sp+5Ch] [bp-C2Ch]@22
	int lOfs; // [sp+60h] [bp-C28h]@22
	_tagTileInfo strTileInfo; // [sp+64h] [bp-C24h]@25
	BYTE sendbuf[1000]; // [sp+68h] [bp-C20h]@25
	int nScore; // [sp+450h] [bp-838h]@22
	int nColumn; // [sp+454h] [bp-834h]@22
	int nRow; // [sp+458h] [bp-830h]@22
	char szBuffer2[1024]; // [sp+45Ch] [bp-82Ch]@22
	char szBuffer1[1024]; // [sp+85Ch] [bp-42Ch]@22
	_tagPMSG_ANS_BOMB_HUNT_LB_CLICK pMsg; // [sp+C5Ch] [bp-2Ch]@11

	int indexColumn; // [sp+C68h] [bp-20h]@8
	int indexRow; // [sp+C6Ch] [bp-1Ch]@8
	int nGameState; // [sp+C70h] [bp-18h]@6
	CBombHuntInfo *pCBombHuntInfo; // [sp+C74h] [bp-14h]@4
	char btPos; // [sp+C7Bh] [bp-Dh]@4

	if (this->IsUserPlay(aIndex))
	{
		LPOBJ lpObj = &gObj[aIndex];
		if (!&gObj[aIndex])
		{
			LogAdd(LOG_BLACK, 
				"[BombHunt][Error] lpObj is NULL");
			return;
		}
		btPos = lpMsg->btPos;

		pCBombHuntInfo = lpObj->m_pCBombHuntInfo;
		if (!pCBombHuntInfo)
		{
			LogAdd(LOG_BLACK, 
				"[BombHunt][Error] pCBombHuntInfo is NULL ");
			return;
		}
		nGameState = pCBombHuntInfo->GetGameState();
		if (nGameState == 1 || nGameState == 2)
		{
			indexRow = (signed int)(unsigned __int8)btPos >> 3;
			indexColumn = (unsigned __int8)btPos % 8;
			this->m_iRecusiveCnt = 0;
			for (int i = 0; i < 48; ++i)
				this->m_OpenTileIndex[i] = 0;

			PHeadSubSetB((LPBYTE )&pMsg.h, 0x4D, 0x24, sizeof(pMsg));
			if (nGameState == 1)
			{
				if (!this->GenerateBomb((unsigned __int8)btPos, pCBombHuntInfo))
				{
					LogAdd(LOG_BLACK, "[BombHunt][Error] GenerateBomb Fail!! [%s][%s]", lpObj->Account, lpObj->Name);
					return;
				}
				this->CheckTileState(indexRow, indexColumn, pCBombHuntInfo);
				pCBombHuntInfo->SetGameState(2);
				this->SendTileInfo(aIndex);
			}
			else
			{
				if (nGameState != 2 || pCBombHuntInfo->m_tileState[(unsigned __int8)btPos])
					return;
				if (pCBombHuntInfo->m_bBombExist[(unsigned __int8)btPos] == 1)
				{
					LogAdd(LOG_BLACK, 
						"[%s][%s][BombHunt][LBClick] Pos: %d:%d OpenTile: Bomb",
						lpObj->Account,
						lpObj->Name,
						indexColumn + 1,
						indexRow + 1);
					pCBombHuntInfo->m_tileState[(unsigned __int8)btPos] = 6;
					pMsg.btPos = btPos;
					DataSend(aIndex, (BYTE *)&pMsg.h, (unsigned __int8)pMsg.h.size);
					this->SendGameResult(aIndex);
					this->GDReqBombHuntInsert(lpObj);
					return;
				}
				this->CheckTileState(indexRow, indexColumn, pCBombHuntInfo);
			}
			memset(&szBuffer1, 0, sizeof(szBuffer1));
			memset(&szBuffer2, 0, sizeof(szBuffer2));
			nRow = 0;
			nColumn = 0;
			nScore = 0;
			nCnt = 0;
			lOfs = sizeof(pMsg);;
			for (int j = 0; j < 48; ++j)
			{
				if (this->m_OpenTileIndex[j] == 2)
				{
					strTileInfo.btPos = j;
					strTileInfo.btTileInfo = pCBombHuntInfo->m_tileAdjustBombCount[j];
					memcpy(&sendbuf[lOfs], &strTileInfo.btPos, sizeof(strTileInfo));
					lOfs += sizeof(strTileInfo);
					++nCnt;
					nRow = j / 8;
					nColumn = j % 8;
					sprintf_s(szBuffer1, "%d:%d[%d] ", j % 8 + 1, j / 8 + 1, pCBombHuntInfo->m_tileAdjustBombCount[j]);
					strcat_s(szBuffer2, szBuffer1);
					nTileBombCnt = pCBombHuntInfo->m_tileAdjustBombCount[j];
					if (nTileBombCnt)
						nScore += 10 * nTileBombCnt;
				}
			}

			pCBombHuntInfo->SetScore(nScore + pCBombHuntInfo->GetScore());
			pMsg.btPos = btPos;
			pMsg.btScoreH = SET_NUMBERHB(nScore);
			pMsg.btScoreL = SET_NUMBERLB(nScore);
			pMsg.btCnt = nCnt;
			pMsg.h.size = lOfs;
			memcpy(sendbuf, (BYTE *)&pMsg, sizeof(pMsg));
			DataSend(aIndex, sendbuf, lOfs);
			this->GDReqBombHuntInsert( lpObj);
			nRow = (signed int)(unsigned __int8)btPos >> 3;
			nColumn = (unsigned __int8)btPos % 8;
			LogAdd(LOG_BLACK, 
				"[%s][%s][BombHunt][LBClick] Pos: %d:%d OpenTile: %s",
				lpObj->Account,
				lpObj->Name,
				nColumn + 1,
				nRow + 1,
				szBuffer2);
			if (!pCBombHuntInfo->m_nClosedTile && !pCBombHuntInfo->m_nRemainBombCard)
				this->GameClose(aIndex);
		}
	}
}
// 139747C: using guessed type int `CBombHuntMng::CGReqLBClick'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A66180) --------------------------------------------------------
void  CBombHuntMng::CGReqRBClick(_tagPMSG_REQ_BOMB_HUNT_RB_CLICK *lpMsg, int aIndex)
{
	_tagPMSG_ANS_BOMB_HUNT_RB_CLICK pMsg; // [sp+4Ch] [bp-1Ch]@8
	int nGameState; // [sp+54h] [bp-14h]@5
	CBombHuntInfo *pCBombHuntInfo; // [sp+58h] [bp-10h]@3
	char index; // [sp+5Fh] [bp-9h]@2

	if (this->IsUserPlay(aIndex))
	{
		LPOBJ lpObj = &gObj[aIndex];
		index = lpMsg->btPos;
		if ((signed int)(unsigned __int8)index < 48)
		{

			pCBombHuntInfo = lpObj->m_pCBombHuntInfo;
			if (!pCBombHuntInfo)
			{
				LogAdd(LOG_BLACK, 
					"[BombHunt][Error] pCBombHuntInfo is NULL");
				return;
			}
			nGameState = pCBombHuntInfo->GetGameState();
			if ((nGameState == EN_GAME_READY || nGameState == EN_GAME_PLAY) && pCBombHuntInfo->m_tileState[(unsigned __int8)index] != EN_TILE_STATE_OPEN)
			{
				PHeadSubSetB((LPBYTE )&pMsg.h, 0x4D, 0x25, sizeof(pMsg));
				if (pCBombHuntInfo->m_tileState[(unsigned __int8)index])
				{
					if (pCBombHuntInfo->m_tileState[(unsigned __int8)index] == EN_TILE_STATE_CARD)
					{
						++pCBombHuntInfo->m_nRemainBombCard;
						++pCBombHuntInfo->m_nClosedTile;
						pCBombHuntInfo->m_tileState[(unsigned __int8)index] = EN_TILE_STATE_CLOSE;
						pMsg.btTileState = EN_TILE_STATE_CLOSE;
					}
				}
				else
				{
					if (pCBombHuntInfo->m_nRemainBombCard <= 0)
						return;
					pCBombHuntInfo->m_tileState[(unsigned __int8)index] = EN_TILE_STATE_CARD;
					--pCBombHuntInfo->m_nClosedTile;
					--pCBombHuntInfo->m_nRemainBombCard;
					pMsg.btTileState = EN_TILE_STATE_CARD;
				}
				pMsg.btPos = index;
				pMsg.btRemainBombCard = pCBombHuntInfo->m_nRemainBombCard;
				DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
				if (!pCBombHuntInfo->m_nClosedTile && !pCBombHuntInfo->m_nRemainBombCard)
					this->GameClose(aIndex);
			}
		}
	}
}
// 1397480: using guessed type int `CBombHuntMng::CGReqRBClick'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A663E0) --------------------------------------------------------
void  CBombHuntMng::CGReqGameClose(_tagPMS_REQ_BOMB_HUNT_GAME_END *lpMsg, int aIndex)
{

	int nGameState; // [sp+50h] [bp-10h]@7
	CBombHuntInfo *pCBombHuntInfo; // [sp+54h] [bp-Ch]@4

	if (this->IsUserPlay(aIndex))
	{
		LPOBJ lpObj = &gObj[aIndex];
		if (!&gObj[aIndex])
		{
			LogAdd(LOG_BLACK, 
				"[BombHunt][Error] lpObj is NULL");
			return;
		}
		pCBombHuntInfo = lpObj->m_pCBombHuntInfo;
		if (!pCBombHuntInfo)
		{
			LogAdd(LOG_BLACK, 
				"[BombHunt][Error] pCBombHuntInfo is NULL ");
			return;
		}
		if (!pCBombHuntInfo->m_nRemainBombCard)
		{
			nGameState = pCBombHuntInfo->GetGameState();
			if (nGameState == EN_GAME_READY)
			{

				if (!this->GenerateBomb(GetLargeRand() % 0x30, pCBombHuntInfo))
				{
					LogAdd(LOG_BLACK, "[BombHunt][Error] GenerateBomb Fail!! [%s][%s]", lpObj->Account, lpObj->Name);
					return;
				}
				pCBombHuntInfo->SetGameState(EN_GAME_PLAY);
			}
			if (nGameState == EN_GAME_PLAY)
				this->SendGameResult( aIndex);
		}
	}
}
// 1397484: using guessed type int `CBombHuntMng::CGReqGameClose'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A66550) --------------------------------------------------------
void  CBombHuntMng::GameClose(int aIndex)
{
	CBombHuntInfo *pCBombHuntInfo; // [sp+50h] [bp-Ch]@4

	if (this->IsUserPlay(aIndex))
	{
		if (&gObj[aIndex])
		{
			pCBombHuntInfo = gObj[aIndex].m_pCBombHuntInfo;
			if (pCBombHuntInfo)
			{
				if (!pCBombHuntInfo->m_nRemainBombCard && pCBombHuntInfo->GetGameState() == 2)
					this->SendGameResult(aIndex);
			}
			else
			{
				LogAdd(LOG_BLACK, 
					"[BombHunt][Error] pCBombHuntInfo is NULL");
			}
		}
		else
		{
			LogAdd(LOG_BLACK, 
				"[BombHunt][Error] lpObj is NULL ");
		}
	}
}
// 1397488: using guessed type int `CBombHuntMng::GameClose'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A66650) --------------------------------------------------------
void  CBombHuntMng::SendGameResult(int aIndex)
{
	BYTE btClear; // [sp+4Fh] [bp-C55h]@19
	int nTileBombCnt; // [sp+50h] [bp-C54h]@16

	int nBombFail; // [sp+5Ch] [bp-C48h]@5
	int nBombCardFail; // [sp+60h] [bp-C44h]@5
	int nBombCard; // [sp+64h] [bp-C40h]@5
	int nNumScore; // [sp+68h] [bp-C3Ch]@5
	int nScore; // [sp+6Ch] [bp-C38h]@5
	int nScorea; // [sp+6Ch] [bp-C38h]@21
	int nCnt; // [sp+70h] [bp-C34h]@5
	_tagTileState tileState[11]; // [sp+78h] [bp-C2Ch]@5
	BYTE sendbuf[1000]; // [sp+90h] [bp-C14h]@5
	int nColumn; // [sp+478h] [bp-82Ch]@5
	int nRow; // [sp+47Ch] [bp-828h]@5
	char szBuffer2[1024]; // [sp+480h] [bp-824h]@5
	char szBuffer1[1024]; // [sp+880h] [bp-424h]@5
	CBombHuntInfo *pCBombHuntInfo; // [sp+C80h] [bp-24h]@1

	_tagPMS_ANS_BOMB_HUNT_GAME_END pMsg; // [sp+C88h] [bp-1Ch]@1


	PHeadSubSetB((LPBYTE )&pMsg.h, 0x4D, 0x26, sizeof(pMsg));
	LPOBJ lpObj = &gObj[aIndex];

	pCBombHuntInfo = gObj[aIndex].m_pCBombHuntInfo;
	if (pCBombHuntInfo)
	{
		if (pCBombHuntInfo->GetGameState() == EN_GAME_PLAY || pCBombHuntInfo->GetGameState() == EN_GAME_RESULT)
		{

			memset(&szBuffer1, 0, sizeof(szBuffer1));
			memset(&szBuffer2, 0, sizeof(szBuffer2));
			nRow = 0;
			nColumn = 0;

			memset(&sendbuf, 0, sizeof(sendbuf));

			nCnt = 0;
			nScore = 0;
			nNumScore = 0;
			nBombCard = 0;
			nBombCardFail = 0;
			nBombFail = 0;
			for (int i = 0; i < 48; ++i)
			{
				if (pCBombHuntInfo->m_bBombExist[i])
				{
					if (pCBombHuntInfo->m_tileState[i] == EN_TILE_STATE_CARD)
					{
						tileState[nCnt].btPos = i;
						tileState[nCnt++].btTileState = EN_TILE_STATE_CARD_BOMB;
						++nBombCard;
						nRow = i / 8;
						nColumn = i % 8;
						sprintf_s(szBuffer1, "%d:%d ", i % 8 + 1, i / 8 + 1);
						strcat_s(szBuffer2, szBuffer1);
					}
					else if (pCBombHuntInfo->m_tileState[i] == EN_TILE_STATE_OPEN_BOMB)
					{
						++nBombFail;
						pMsg.btResult = 1;
					}
				}
				else if (pCBombHuntInfo->m_tileState[i] == EN_TILE_STATE_CARD)
				{
					tileState[nCnt].btPos = i;
					tileState[nCnt++].btTileState = EN_TILE_STATE_CARD;
					++nBombCardFail;
					pMsg.btResult = 1;
					nRow = i / 8;
					nColumn = i % 8;
					sprintf_s(szBuffer1, "%d:%d ", i % 8 + 1, i / 8 + 1);
					strcat_s(szBuffer2, szBuffer1);
				}
				if (pCBombHuntInfo->m_tileState[i] == EN_TILE_STATE_OPEN)
				{
					nTileBombCnt = pCBombHuntInfo->m_tileAdjustBombCount[i];
					if (nTileBombCnt)
						nNumScore += 10 * nTileBombCnt;
				}
			}
			btClear = 0;
			if (nBombCard == 11)
			{
				nScore = 500;
				pMsg.wClear = 500;
				btClear = 1;
			}
			nScorea = nNumScore + nScore + 50 * nBombCard - 20 * nBombCardFail - 50 * nBombFail;
			if (nScorea < 0)
				nScorea = 0;
			pMsg.wNumScore = nNumScore;
			pMsg.wBombFind = 50 * nBombCard;
			pMsg.wBombFail = 20 * nBombCardFail;
			pMsg.wBombTread = 50 * nBombFail;
			pMsg.wTotScore = nScorea;
			memcpy(&sendbuf[sizeof(pMsg)], (BYTE *)tileState, 2 * nCnt);
			pMsg.btCnt = nCnt;

			pMsg.h.size = 2 * nCnt + 18;

			memcpy(sendbuf, (char *)&pMsg.h, sizeof(pMsg));

			DataSend(aIndex, sendbuf, pMsg.h.size);
			LogAdd(LOG_BLACK, 
				"[%s][%s][BombHunt][Result]RBClick: %s Score: %d:%d:-%d:-%d:%d[%d]",
				lpObj->Account,
				lpObj->Name,
				szBuffer2,
				nNumScore,
				pMsg.wBombFind,
				pMsg.wBombFail,
				pMsg.wBombTread,
				pMsg.wClear,
				pMsg.wTotScore);
			if (pCBombHuntInfo->GetGameState() == EN_GAME_PLAY)
				this->GDReqBombHuntDBLog( lpObj, nScorea, btClear);
			pCBombHuntInfo->SetScore(nScorea);
			pCBombHuntInfo->SetGameState(EN_GAME_RESULT);
		}
	}
	else
	{
		LogAdd(LOG_BLACK, 
			"[BombHunt][Error] pCBombHuntInfo is NULL ");
	}
}
// 139748C: using guessed type int `CBombHuntMng::SendGameResult'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A66D60) --------------------------------------------------------
void  CBombHuntMng::CGReqReward(int aIndex)
{

	unsigned __int16 iItemType; // [sp+58h] [bp-1Ch]@6
	int nGameState; // [sp+5Ch] [bp-18h]@4
	_tagPMSG_ANS_BOMB_HUNT_REWARD pMsg; // [sp+60h] [bp-14h]@4
	CBombHuntInfo *pCBombHuntInfo; // [sp+68h] [bp-Ch]@2

	if (this->IsUserPlay(aIndex))
	{
		LPOBJ lpObj = &gObj[aIndex];

		pCBombHuntInfo = lpObj->m_pCBombHuntInfo;
		if (pCBombHuntInfo)
		{
			PHeadSubSetB((LPBYTE )&pMsg.h, 0x4D, 0x28, sizeof(pMsg));
			pMsg.btResult = 0;
			nGameState = pCBombHuntInfo->GetGameState();
			if (nGameState == EN_GAME_RESULT)
			{
				iItemType = 0;
				if (pCBombHuntInfo->GetScore() > 150)
				{
					if (pCBombHuntInfo->GetScore() <= 150 || pCBombHuntInfo->GetScore() > 850)
					{
						if (pCBombHuntInfo->GetScore() <= 850 || pCBombHuntInfo->GetScore() > 1100)
						{
							if (pCBombHuntInfo->GetScore() > 1100)
								iItemType = GET_ITEM(14,377);
							_stGremoryCaseItem stItem;
							stItem.btRewardSource = 2;
							stItem.btStorageType = 17;
							stItem.ItemInfo.Clear();
							stItem.ItemInfo.m_Index = iItemType;

							g_GremoryCase.GDReqAddItemToGremoryCase(lpObj->Index, stItem, 30);

							
						}
						else
						{
							_stGremoryCaseItem stItem;
							stItem.btRewardSource = 2;
							stItem.btStorageType = 17;
							stItem.ItemInfo.Clear();
							stItem.ItemInfo.m_Index = GET_ITEM(14, 376);

							g_GremoryCase.GDReqAddItemToGremoryCase(lpObj->Index, stItem, 30);
						}
					}
					else
					{
						_stGremoryCaseItem stItem;
						stItem.btRewardSource = 2;
						stItem.btStorageType = 17;
						stItem.ItemInfo.Clear();
						stItem.ItemInfo.m_Index = GET_ITEM(14, 375);

						g_GremoryCase.GDReqAddItemToGremoryCase(lpObj->Index, stItem, 30);
					}
				}
				else
				{
					_stGremoryCaseItem stItem;
					stItem.btRewardSource = 2;
					stItem.btStorageType = 17;
					stItem.ItemInfo.Clear();
					stItem.ItemInfo.m_Index = GET_ITEM(14, 374);

					g_GremoryCase.GDReqAddItemToGremoryCase(lpObj->Index, stItem, 30);
				}

				LogAdd(LOG_BLACK, "[%s][%s][BombHunt] Event Reward Result [Point : %d]", lpObj->Account, lpObj->Name, pCBombHuntInfo->GetScore());
				LogAdd(LOG_BLACK, "[%s][%s][BombHunt] Event end", lpObj->Account, lpObj->Name);
				pCBombHuntInfo->Clear();
				this->GDReqBombHuntDelete(lpObj);
				DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
			}
			else
			{
				LogAdd(LOG_BLACK, "[BombHunt][GameStae Error][%s][%s][%d]", lpObj->Account, lpObj->Name, nGameState);
			}
		}
		else
		{
			LogAdd(LOG_BLACK, 
				"[BombHunt][Error] pCBombHuntInfo is NULL ");
		}
	}
}
// 1397490: using guessed type int `CBombHuntMng::CGReqReward'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A67010) --------------------------------------------------------
void  CBombHuntMng::GDReqBombHuntSelect(LPOBJ lpObj)
{

	_tagPMSG_REQ_BOMB_HUNT_SELECT_DS pMsg; // [sp+50h] [bp-24h]@7


	if (gObjIsConnected(lpObj->Index))
	{
		if (gServerInfo.m_BombHuntEventOn)
		{
			memcpy(pMsg.AccountID, lpObj->Account, sizeof(lpObj->Account));
			memcpy(pMsg.Name, lpObj->Name, sizeof(lpObj->Name));
			pMsg.aIndex = lpObj->Index;
			pMsg.h.set(0xE8, 0x20, sizeof(pMsg));
			gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);

		}
	}
}
// B576EA0: using guessed type int g_bBombHuntEventOn;

//----- (00A67110) --------------------------------------------------------
void  CBombHuntMng::GDAnsBombHuntSelect(_tagPMSG_ANS_BOMB_HUNT_SELECT_DS *lpMsg)
{

	char szTileTemp[49]; // [sp+60h] [bp-10Ch]@8
	int TileTemp[48]; // [sp+94h] [bp-D8h]@8
	int nGameState; // [sp+154h] [bp-18h]@7
	CBombHuntInfo *pCBombHuntInfo; // [sp+158h] [bp-14h]@4

	int aIndex = lpMsg->aIndex;
	if (this->IsUserPlay(aIndex))
	{
		LPOBJ lpObj = &gObj[aIndex];
		if (&gObj[aIndex])
		{
			this->SendBombHuntEventInfo(aIndex);

			pCBombHuntInfo = lpObj->m_pCBombHuntInfo;
			if (pCBombHuntInfo)
			{
				pCBombHuntInfo->SetBombHuntInfoLoad(1);
				if (!lpMsg->btResult)
				{
					nGameState = lpMsg->btGameState;
					if (nGameState)
					{
						pCBombHuntInfo->Clear();
						pCBombHuntInfo->SetGameState(nGameState);
						pCBombHuntInfo->SetScore(lpMsg->wScore);
						pCBombHuntInfo->m_nClosedTile = 48;
						pCBombHuntInfo->m_nRemainBombCard = 11;
						memset((char *)&TileTemp, 0, sizeof(TileTemp));
						memset(&szTileTemp, 0, sizeof(szTileTemp));
						memcpy(szTileTemp, lpMsg->szTileState, sizeof(lpMsg->szTileState));
						for (int i = 0; i < 48; ++i)
						{
							TileTemp[i] = szTileTemp[i] - 48;
							if (TileTemp[i] == EN_TILE_STATE_BOMB)
							{
								if (TileTemp[i] == EN_TILE_STATE_BOMB)
								{
									pCBombHuntInfo->m_bBombExist[i] = 1;
									pCBombHuntInfo->m_tileState[i] = EN_TILE_STATE_CLOSE;
								}
							}
							else
							{
								pCBombHuntInfo->m_tileState[i] = TileTemp[i];
								if (TileTemp[i] == EN_TILE_STATE_OPEN)
									--pCBombHuntInfo->m_nClosedTile;
								if (TileTemp[i] == EN_TILE_STATE_CARD)
								{
									--pCBombHuntInfo->m_nClosedTile;
									--pCBombHuntInfo->m_nRemainBombCard;
								}
								if (TileTemp[i] == EN_TILE_STATE_CARD_BOMB)
								{
									--pCBombHuntInfo->m_nClosedTile;
									--pCBombHuntInfo->m_nRemainBombCard;
									pCBombHuntInfo->m_bBombExist[i] = 1;
									pCBombHuntInfo->m_tileState[i] = EN_TILE_STATE_CARD;
								}
								if (TileTemp[i] == EN_TILE_STATE_OPEN_BOMB)
								{
									pCBombHuntInfo->m_bBombExist[i] = 1;
									pCBombHuntInfo->m_tileState[i] = EN_TILE_STATE_OPEN_BOMB;
								}
							}
						}
						for (int j = 0; j < 48; ++j)
							pCBombHuntInfo->m_tileAdjustBombCount[j] = this->GetAdjustMineCount(j / 8,j % 8,pCBombHuntInfo);
					}
				}
			}
			else
			{
				LogAdd(LOG_BLACK, 
					"[BombHunt][Error] pCBombHuntInfo is NULL ");
			}
		}
		else
		{
			LogAdd(LOG_BLACK, 
				"[BombHunt][Error] lpObj is NULL ");
		}
	}
}
// 1397494: using guessed type int `CBombHuntMng::GDAnsBombHuntSelect'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A675C0) --------------------------------------------------------
void  CBombHuntMng::GDReqBombHuntInsert(LPOBJ lpObj)
{

	char szTemp[2]; // [sp+50h] [bp-94h]@3
	char szBuff[49]; // [sp+54h] [bp-90h]@3
	_tagPMSG_REQ_BOMB_HUNT_INSERT_DS pMsg; // [sp+88h] [bp-5Ch]@3
	CBombHuntInfo *pCBombHuntInfo; // [sp+D8h] [bp-Ch]@1

	pCBombHuntInfo = lpObj->m_pCBombHuntInfo;
	if (pCBombHuntInfo)
	{
		pMsg.szTileState[48] = 0;

		memset(&szBuff, 0, sizeof(szBuff));
		szTemp[0] = 0;
		szTemp[1] = 0;
		for (int i = 0; i < 48; ++i)
		{
		
			/*_itoa_s(pCBombHuntInfo->m_tileState[i], szTemp, 10);
			if (pCBombHuntInfo->m_bBombExist[i] == 1 && !pCBombHuntInfo->m_tileState[i])
				_itoa_s(4, szTemp, 10);
			if (pCBombHuntInfo->m_bBombExist[i] == 1 && pCBombHuntInfo->m_tileState[i] == 1)
				_itoa_s(5, szTemp, 10);*/
			strcat_s(szBuff, szTemp);
		}
		memcpy(pMsg.szTileState, szBuff, sizeof(szBuff));
		pMsg.btGameState = pCBombHuntInfo->GetGameState();
		pMsg.wScore = pCBombHuntInfo->GetScore();
		memcpy(pMsg.AccountID, lpObj->Account, sizeof(lpObj->Account));
		memcpy(pMsg.Name, lpObj->Name, sizeof(lpObj->Name));
		pMsg.aIndex = lpObj->Index;
		pMsg.h.set(0xE8, 0x21, sizeof(pMsg));
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);

	}
	else
	{
		LogAdd(LOG_BLACK, 
			"[BombHunt][Error] pCBombHuntInfo is NULL ");
	}
}
// 1397498: using guessed type int `CBombHuntMng::GDReqBombHuntInsert'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00A67830) --------------------------------------------------------
void  CBombHuntMng::GDReqBombHuntDelete(LPOBJ lpObj)
{
	_tagPMSG_REQ_BOMB_HUNT_DELETE_DS pMsg; // [sp+4Ch] [bp-24h]@1

	memcpy(pMsg.AccountID, lpObj->Account, sizeof(lpObj->Account));
	memcpy(pMsg.Name, lpObj->Name, sizeof(lpObj->Name));
	pMsg.aIndex = lpObj->Index;
	pMsg.h.set(0xE8, 0x22, sizeof(pMsg));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
}

//----- (00A678D0) --------------------------------------------------------
void  CBombHuntMng::GDReqBombHuntDBLog(LPOBJ lpObj, int iScore, char btClear)
{

	_tagPMSG_REQ_BOMB_HUNT_LOG_INSERT_DS pMsg; // [sp+50h] [bp-28h]@8

	if (gObjIsConnected(lpObj->Index) && gServerInfo.m_BombHuntEventOn)
	{
		if (iScore)
		{
			memcpy(pMsg.AccountID, lpObj->Account, sizeof(lpObj->Account));
			memcpy(pMsg.Name, lpObj->Name, sizeof(lpObj->Name));
			pMsg.wScore = iScore;
			pMsg.btClear = btClear;
			pMsg.h.set(0xE8, 0x23, sizeof(pMsg));

			gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
		}
	}
}
// B576EA0: using guessed type int g_bBombHuntEventOn;

//----- (00A679E0) --------------------------------------------------------
void  CBombHuntMng::ReStart(int aIndex)//GMCMD NOT
{
	CBombHuntInfo *pCBombHuntInfo; // ST5C_4@2
	_tagPMSG_ANS_BOMB_HUNT_START pMsg; // [sp+50h] [bp-Ch]@2

	if (gObj[aIndex].CloseType == -1)
	{

		PHeadSubSetB((LPBYTE)&pMsg.h, 0x4D, 0x23, sizeof(pMsg));
		DataSend(aIndex, (BYTE *)&pMsg.h, (unsigned __int8)pMsg.h.size);
		pCBombHuntInfo = gObj[aIndex].m_pCBombHuntInfo;
		pCBombHuntInfo->Clear();
		pCBombHuntInfo->SetGameState(EN_GAME_READY);
	}
}

//----- (00A67AA0) --------------------------------------------------------
void  CBombHuntMng::SendTileInfo(int aIndex)
{

	char szSendBuffer[1024]; // [sp+50h] [bp-C1Ch]@1
	char szBufferTmp[1024]; // [sp+450h] [bp-81Ch]@1
	char szBuffer[1024]; // [sp+850h] [bp-41Ch]@1

	int indexRow; // [sp+C54h] [bp-18h]@3



	LPOBJ lpObj = &gObj[aIndex];

	CBombHuntInfo *pCBombHuntInfo = gObj[aIndex].m_pCBombHuntInfo;
	int tempMineCount = 0;
	int indexColumn = 0;

	memset(&szBuffer, 0, sizeof(szBuffer));

	memset(&szBufferTmp, 0, sizeof(szBufferTmp));

	memset(&szSendBuffer, 0, sizeof(szSendBuffer));
	for (int i = 0; i < 48; ++i)
	{
		indexRow = i / 8;
		indexColumn = i % 8;
		tempMineCount = this->GetAdjustMineCount(i / 8, i % 8, pCBombHuntInfo);
		pCBombHuntInfo->m_tileAdjustBombCount[i] = tempMineCount;
		if (!indexColumn)
			strcat_s(szBufferTmp, "\n");
		if (pCBombHuntInfo->m_bBombExist[i] == 1)
			sprintf_s(szBuffer, "X ");
		else
			sprintf_s(szBuffer, "%d ", pCBombHuntInfo->m_tileAdjustBombCount[i]);
		strcat_s(szBufferTmp, szBuffer);
		strcat_s(szSendBuffer, szBuffer);
		if (indexColumn == 7)
		{
			//GCServerMsgStringSend(szSendBuffer, aIndex, 1);
			memset(szSendBuffer, 0, sizeof(szSendBuffer));
		}
	}
	strcat_s(szBufferTmp, "\n");
	OutputDebugStringA(szBufferTmp);
}