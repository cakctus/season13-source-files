#include "stdafx.h"
#include "User.h"
#include "Util.h"
#include "GameMain.h"
#include "CBlockChatUserSystem.h"

CBlockChatUserSystem g_CBlockChatUserSystem;

CBlockChatUserSystem::CBlockChatUserSystem()
{
	memset(this->m_stBlockCharUser, -1, sizeof(this->m_stBlockCharUser));
}


CBlockChatUserSystem::~CBlockChatUserSystem()
{
}

//----- (00C88BF0) --------------------------------------------------------
void  CBlockChatUserSystem::AddBlockChattingUserInfo(LPOBJ lpObj)
{

	if (gObjIsConnected(lpObj->Index))
	{
		for (int i = 0; i < 1000; ++i)
		{
			if (!this->m_stBlockCharUser[i].bIsUsed)
			{
				this->m_stBlockCharUser[i].Clear();

				this->m_stBlockCharUser[i].bIsUsed = 1;
				this->m_stBlockCharUser[i].nUserIndex = lpObj->Index;
				this->m_stBlockCharUser[i].dwUserGUID = lpObj->DBClass;
				memcpy(this->m_stBlockCharUser[i].szCharName, lpObj->Name, sizeof(lpObj->Name));
				lpObj->m_nBlockChatUserIdx = i;
				return;
			}
		}
	}
}

//----- (00C88D70) --------------------------------------------------------
void  CBlockChatUserSystem::ClearBlockChatUserInfo(LPOBJ lpObj)
{
	signed int BCUIDX; // [sp+4Ch] [bp-8h]@1

	BCUIDX = lpObj->m_nBlockChatUserIdx;
	if (BCUIDX >= 0 && BCUIDX < 1000)
		this->m_stBlockCharUser[BCUIDX].bIsUsed = 0;
}

//----- (00C88DD0) --------------------------------------------------------
void  CBlockChatUserSystem::GD_ReqLoadBlockChattingUser(int aIndex)
{

	SDHP_REQ_BLOCK_CHAT_USER_INFO pMsg; // [sp+50h] [bp-1Ch]@6

	if (gObjIsConnected(aIndex))
	{
		PHeadSubSetB((LPBYTE)&pMsg, 0x6E, 0, sizeof(pMsg));
		pMsg.iUserIndex = aIndex;
		memcpy(pMsg.szCharName, gObj[aIndex].Name, sizeof(gObj[aIndex].Name));
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);

	}
	else
	{
		LogAdd(LOG_BLACK,"[BlockCharUser][Error] CGReqLoadBlockChattingUser, Invalid Index(%d)", aIndex);
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00C88EB0) --------------------------------------------------------
void  CBlockChatUserSystem::DG_ResLoadBlockChattingUser(SDHP_ANS_BLOCK_CHAT_USER_INFO *lpMsg)
{

	if (gObjIsConnected(lpMsg->nUserIndex))
	{
		LPOBJ lpObj = &gObj[lpMsg->nUserIndex];
		if (lpMsg->btUserCnt >= 1)
		{
			int BCUIDX = lpObj->m_nBlockChatUserIdx;
			if (BCUIDX >= 0 && BCUIDX < 1000)
			{
				this->m_stBlockCharUser[BCUIDX].btUserCnt = lpMsg->btUserCnt;
				memcpy((char *)this->m_stBlockCharUser[BCUIDX].szBlockCharList, (char *)lpMsg->szBlockCharList, sizeof(lpMsg->szBlockCharList));
				LogAdd(LOG_BLACK,
					"[BlockChatUser][STARTINFO](%s)(%s)  Send Chatting Block User List Info. (Cnt : %d)",
					lpObj->Account,lpObj->Name,this->m_stBlockCharUser[BCUIDX].btUserCnt);
				this->GC_SendBlockUserListInfo(lpObj, 1, 1);
			}
		}
		else
		{
			LogAdd(LOG_BLACK,"[BlockChatUser][STARTINFO](%s)(%s)  Chatting Block UserCount is Zero.", lpObj->Account, lpObj->Name);
			this->GC_SendBlockUserListInfo(lpObj, 1, 0);
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00C89060) --------------------------------------------------------
void  CBlockChatUserSystem::GC_SendBlockUserListInfo(LPOBJ lpObj, char btResType, char btResult)
{

	int nSlot; // [sp+50h] [bp-120h]@7
	signed int BCUIDX; // [sp+58h] [bp-118h]@3
	PMSG_ANS_BLOCK_CHAT_USER_INFO pMsg; // [sp+5Ch] [bp-114h]@1

	PHeadSubSetW((LPBYTE)&pMsg, 0x6e, 0, sizeof(pMsg));
	pMsg.btResType = btResType;
	pMsg.btResult = btResult;
	if (btResult == 1)
	{
		BCUIDX = lpObj->m_nBlockChatUserIdx;
		if (BCUIDX >= 0 && BCUIDX < 1000 && this->m_stBlockCharUser[BCUIDX].btUserCnt > 0)
		{
			nSlot = 0;
			for (int i = 0; i < 20; ++i)
			{
				if (this->m_stBlockCharUser[BCUIDX].szBlockCharList[i].bIsUsed == 1)
				{
					pMsg.BlockUserList[nSlot].bIsUsed = 1;
					pMsg.BlockUserList[nSlot].btSlotNum = this->m_stBlockCharUser[BCUIDX].szBlockCharList[i].btSlotNum;
					memcpy(
						pMsg.BlockUserList[nSlot++].szCharName,
						this->m_stBlockCharUser[BCUIDX].szBlockCharList[i].szCharName,
						sizeof(this->m_stBlockCharUser[BCUIDX].szBlockCharList[i].szCharName));
				}
			}
			pMsg.btUserCnt = nSlot;
			//DataSend(lpObj->Index, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else
	{
		//DataSend(lpObj->Index, (BYTE*)&pMsg, sizeof(pMsg));
	}
}


//----- (00C89310) --------------------------------------------------------
void  CBlockChatUserSystem::CG_ReqAddBlockChattingUser( PMSG_REQ_ADD_BLOCK_CHAT_USER *lpMsg, int aIndex)
{
	char btResult; // [sp+5Bh] [bp-11h]@6
	signed int BCUIDX; // [sp+60h] [bp-Ch]@6

		if (gObjIsConnected(aIndex))
		{
			LPOBJ lpObj = &gObj[aIndex];
			BCUIDX = gObj[aIndex].m_nBlockChatUserIdx;
			btResult = 0;
			if (BCUIDX >= 0 && BCUIDX < 1000)
			{
				for (int i = 0; i < 20; ++i)
				{
					if (this->m_stBlockCharUser[BCUIDX].szBlockCharList[i].bIsUsed == 1)
					{
						if (!strcmp(this->m_stBlockCharUser[BCUIDX].szBlockCharList[i].szCharName, lpMsg->szCharName))
						{
							LogAdd(LOG_BLACK,"[BlockChatUser][ADDUSER](%s)(%s) Fail. Already Block Character. AddUser:(%s) (Cnt : %d)",
								lpObj->Account,lpObj->Name,lpMsg->szCharName,this->m_stBlockCharUser[BCUIDX].btUserCnt);
							this->GC_SendBlockUserListInfo(lpObj, 2, 3);
							return;
						}
					}
				}
				for (int j = 0; j < 20; ++j)
				{
					if (!this->m_stBlockCharUser[BCUIDX].szBlockCharList[j].bIsUsed)
					{
						this->m_stBlockCharUser[BCUIDX].szBlockCharList[j].bIsUsed = 1;
						this->m_stBlockCharUser[BCUIDX].szBlockCharList[j].btSlotNum = j;
						++this->m_stBlockCharUser[BCUIDX].btUserCnt;
						memcpy(this->m_stBlockCharUser[BCUIDX].szBlockCharList[j].szCharName, lpMsg->szCharName, sizeof(lpMsg->szCharName));
						this->m_stBlockCharUser[BCUIDX].szBlockCharList[j].szCharName[10] = 0;
						btResult = 1;
						LogAdd(LOG_BLACK,
							"[BlockChatUser][ADDUSER](%s)(%s) Success. Add Chatting Block User. AddUser:(%s) (Cnt : %d)",
							lpObj->Account,
							lpObj->Name,
							lpMsg->szCharName,
							this->m_stBlockCharUser[BCUIDX].btUserCnt);
						this->GD_ReqAddBlockChatUserInfo(lpObj,&this->m_stBlockCharUser[BCUIDX].szBlockCharList[j]);
						break;
					}
					if (j >= 19)
					{
						btResult = 2;
						LogAdd(LOG_BLACK,
							"[BlockChatUser][ADDUSER](%s)(%s) Fail. Block User is full. AddUser:(%s) (Cnt : %d)",
							lpObj->Account,
							lpObj->Name,
							lpMsg->szCharName,
							this->m_stBlockCharUser[BCUIDX].btUserCnt);
						break;
					}
				}
				this->GC_SendBlockUserListInfo(lpObj, 2, btResult);
			}
		}
	
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00C896D0) --------------------------------------------------------
void  CBlockChatUserSystem::CG_ReqDelBlockChattingUser(PMSG_REQ_DEL_BLOCK_CHAT_USER *lpMsg, int aIndex)
{

	char btResult; // [sp+53h] [bp-11h]@12
	int nSlotNum; // [sp+54h] [bp-10h]@6
	signed int BCUIDX; // [sp+58h] [bp-Ch]@6

		if (gObjIsConnected(aIndex))
		{
			LPOBJ lpObj = &gObj[aIndex];
			BCUIDX = gObj[aIndex].m_nBlockChatUserIdx;
			nSlotNum = lpMsg->btSlotNum;
			if (nSlotNum < 20 && BCUIDX >= 0 && BCUIDX < 1000)
			{
				if (this->m_stBlockCharUser[BCUIDX].szBlockCharList[nSlotNum].bIsUsed == 1)
				{
					if (strcmp(this->m_stBlockCharUser[BCUIDX].szBlockCharList[nSlotNum].szCharName, lpMsg->szCharName))
					{
						btResult = 2;
						LogAdd(LOG_BLACK,
							"[BlockChatUser][DELUSER](%s)(%s) Fail. Not Matching Delete User Name. DelUser:(%s)(Cnt : %d)",
							lpObj->Account,lpObj->Name,lpMsg->szCharName,this->m_stBlockCharUser[BCUIDX].btUserCnt);
					}
					else
					{
						this->GD_ReqDelBlockChatUserInfo(lpObj,&this->m_stBlockCharUser[BCUIDX].szBlockCharList[nSlotNum]);
						this->m_stBlockCharUser[BCUIDX].szBlockCharList[nSlotNum].bIsUsed = 0;
						this->m_stBlockCharUser[BCUIDX].szBlockCharList[nSlotNum].btSlotNum = -1;
						memset(this->m_stBlockCharUser[BCUIDX].szBlockCharList[nSlotNum].szCharName, 0, sizeof(this->m_stBlockCharUser[BCUIDX].szBlockCharList[nSlotNum].szCharName));
						btResult = 1;
						LogAdd(LOG_BLACK,
							"[BlockChatUser][DELUSER](%s)(%s) Success. Delete Chatting Block User. DelUser:(%s)(Cnt : %d)",
							lpObj->Account,
							lpObj->Name,
							lpMsg->szCharName,
							this->m_stBlockCharUser[BCUIDX].btUserCnt);
					}
				}
				else
				{
					btResult = 3;
					LogAdd(LOG_BLACK,
						"[BlockChatUser][DELUSER](%s)(%s) Fail. Not Matching Delete User SlotNumber. DelUser:(%s)(Cnt : %d)",
						lpObj->Account,
						lpObj->Name,
						lpMsg->szCharName,
						this->m_stBlockCharUser[BCUIDX].btUserCnt);
				}
				this->GC_SendBlockUserListInfo(lpObj, 3, btResult);
			}
		}
	
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00C899A0) --------------------------------------------------------
void  CBlockChatUserSystem::GD_ReqAddBlockChatUserInfo(LPOBJ lpObj, _ST_BLOCK_CHATTING_USER *stBCUL)
{
	SDHP_REQ_ADD_BLOCK_CHAT_USER_INFO pMsg; // [sp+4Ch] [bp-28h]@1

	pMsg.h.set(0x6e, 1, sizeof(pMsg));

	pMsg.nUserIndex = lpObj->Index;
	memcpy(pMsg.szCharName, lpObj->Name, sizeof(lpObj->Name));
	memcpy((char *)&pMsg.szBlockChar.bIsUsed, (char *)&stBCUL->bIsUsed, sizeof(stBCUL->bIsUsed));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);

}


//----- (00C89A80) --------------------------------------------------------
void  CBlockChatUserSystem::GD_ReqDelBlockChatUserInfo(LPOBJ lpObj, _ST_BLOCK_CHATTING_USER *stBCUL)
{
	SDHP_REQ_DEL_BLOCK_CHAT_USER_INFO pMsg; // [sp+4Ch] [bp-28h]@1

	pMsg.h.set(0x6e, 2, sizeof(pMsg));

	pMsg.nUserIndex = lpObj->Index;
	memcpy(pMsg.szCharName, lpObj->Name, sizeof(lpObj->Name));
	memcpy((char *)&pMsg.szBlockChar.bIsUsed, (char *)&stBCUL->bIsUsed, sizeof(stBCUL->bIsUsed));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);

}