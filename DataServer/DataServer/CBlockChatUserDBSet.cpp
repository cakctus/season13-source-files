#include "stdafx.h"
#include "QueryManager.h"
#include "CBlockChatUserDBSet.h"

CBlockChatUserDBSet g_BlockChatUserDBSet;
CBlockChatUserDBSet::CBlockChatUserDBSet()
{
}


CBlockChatUserDBSet::~CBlockChatUserDBSet()
{
}


//----- (0042CA60) --------------------------------------------------------
int  CBlockChatUserDBSet::DSDB_SelectBlockChatUser(char *Name, SDHP_ANS_BLOCK_CHAT_USER_INFO *lpSendMsg)
{

	int nCnt = 0;
	BYTE btSlotNum = 0;
	if (gQueryManager.ExecQuery("WZ_ChattingBlockUserLoad '%s'", Name))
	{
		while (true)
		{
			__int16 sqlReturn = gQueryManager.Fetch();
			if (sqlReturn == SQL_NULL_DATA)
			{
				break;
			}
			if (sqlReturn == SQL_NO_DATA)
			{
				break;
			}
			btSlotNum = gQueryManager.GetAsInteger("SlotNum");
			lpSendMsg->BlockCharList[(unsigned __int8)btSlotNum].bIsUsed = 1;
			lpSendMsg->BlockCharList[(unsigned __int8)btSlotNum].btSlotNum = btSlotNum;
			gQueryManager.GetAsString("BlockUser", lpSendMsg->BlockCharList[(unsigned __int8)btSlotNum].szCharName, sizeof(lpSendMsg->BlockCharList[(unsigned __int8)btSlotNum].szCharName));
			if (++nCnt >= 20)
				break;
		}
		lpSendMsg->btUserCnt = nCnt;
		gQueryManager.Close();
		return 1;

	}

	else
	{
		gQueryManager.Close();
		return 0;

	}
	return 0;
}
// 5CD83C: using guessed type int `CBlockChatUserDBSet::DSDB_SelectBlockChatUser'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (0042CC80) --------------------------------------------------------
int  CBlockChatUserDBSet::DSDB_AddBlockChatUser(SDHP_REQ_ADD_BLOCK_CHAT_USER_INFO *lpRecv)
{

	if (gQueryManager.ExecQuery("EXEC WZ_ChattingBlockUserInsert '%s', %d ,'%s'", lpRecv->szCharName, lpRecv->szBlockChar.btSlotNum, lpRecv->szBlockChar.szCharName))
	{
		gQueryManager.Close();
		return 1;
	}

	else
	{
		gQueryManager.Close();
		return 0;
	}
	return 0;
}
// 5CD840: using guessed type int `CBlockChatUserDBSet::DSDB_AddBlockChatUser'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (0042CDE0) --------------------------------------------------------
int  CBlockChatUserDBSet::DSDB_DelBlockChatUser(SDHP_REQ_DEL_BLOCK_CHAT_USER_INFO *lpRecv)
{
	if (gQueryManager.ExecQuery("EXEC WZ_ChattingBlockUserDel '%s', '%s'", lpRecv->szCharName, lpRecv->szBlockChar.szCharName))
	{
		gQueryManager.Close();
		return 1;
	}
	else
	{
		gQueryManager.Close();
		return 0;
	}
	
}

