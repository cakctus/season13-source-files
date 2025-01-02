#pragma once
#include "DataServerProtocol.h"
class CBlockChatUserDBSet
{
public:
	CBlockChatUserDBSet();
	~CBlockChatUserDBSet();

	int  DSDB_SelectBlockChatUser(char *Name, SDHP_ANS_BLOCK_CHAT_USER_INFO *lpSendMsg); // idb
	int  DSDB_AddBlockChatUser(SDHP_REQ_ADD_BLOCK_CHAT_USER_INFO *lpRecv); // idb
	int  DSDB_DelBlockChatUser(SDHP_REQ_DEL_BLOCK_CHAT_USER_INFO *lpRecv); // idb

};
extern CBlockChatUserDBSet g_BlockChatUserDBSet;
