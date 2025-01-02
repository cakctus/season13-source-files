#include "Protocol.h"
/* 6373 */
/* 397 */
enum BLOCK_CHAT_USER_RES_TYPE
{
	START_INFO_SEND = 0x1,
	ADD_INFO_SEND = 0x2,
	DEL_INFO_SEND = 0x3,
};
struct  _ST_BLOCK_CHATTING_USER
{
	bool bIsUsed;
	BYTE btSlotNum;
	char szCharName[11];
};
/* 6374 */
struct  SDHP_REQ_DEL_BLOCK_CHAT_USER_INFO
{
	PSBMSG_HEAD h;
	int nUserIndex;
	char szCharName[11];
	_ST_BLOCK_CHATTING_USER szBlockChar;
};

/* 6849 */
struct  SDHP_REQ_ADD_BLOCK_CHAT_USER_INFO
{
	PSBMSG_HEAD h;
	int nUserIndex;
	char szCharName[11];
	_ST_BLOCK_CHATTING_USER szBlockChar;
};
/* 7356 */
struct  PMSG_ANS_BLOCK_CHAT_USER_INFO
{
	PSWMSG_HEAD h;
	char btResType;
	char btResult;
	char btUserCnt;
	_ST_BLOCK_CHATTING_USER BlockUserList[20];
};
/* 7408 */
struct  SDHP_REQ_BLOCK_CHAT_USER_INFO
{
	PBMSG_HEAD h;
	int iUserIndex;
	char szCharName[11];
};
/* 6580 */
struct  _ST_BLOCK_CHAT_USER_INFO
{
	_ST_BLOCK_CHAT_USER_INFO()
	{
		this->Clear();
	}
	void Clear()
	{
		this->bIsUsed = 0;
		this->btUserCnt = 0;
		this->dwUserGUID = 0;
		memset(this->szCharName, 0, sizeof(this->szCharName));
		memset(this->szBlockCharList, 0, sizeof(this->szBlockCharList));
	}
	bool bIsUsed;
	int nUserIndex;
	unsigned int dwUserGUID;
	char szCharName[11];
	BYTE btUserCnt;
	_ST_BLOCK_CHATTING_USER szBlockCharList[20];
};
/* 6593 */
struct PMSG_REQ_ADD_BLOCK_CHAT_USER
{
	PSBMSG_HEAD h;
	char szCharName[11];
};
/* 6831 */
struct  SDHP_ANS_BLOCK_CHAT_USER_INFO
{
	PSWMSG_HEAD h;
	int nUserIndex;
	char szCharName[11];
	BYTE btUserCnt;
	_ST_BLOCK_CHATTING_USER szBlockCharList[20];
};
/* 6887 */
struct PMSG_REQ_DEL_BLOCK_CHAT_USER
{
	PSBMSG_HEAD h;
	BYTE btSlotNum;
	char szCharName[11];
};
class CBlockChatUserSystem
{
public:
	CBlockChatUserSystem();
	~CBlockChatUserSystem();

	void  AddBlockChattingUserInfo(LPOBJ lpObj); // idb
	void  ClearBlockChatUserInfo(LPOBJ lpObj); // idb
	void  GD_ReqLoadBlockChattingUser(int aIndex); // idb
	void  DG_ResLoadBlockChattingUser(SDHP_ANS_BLOCK_CHAT_USER_INFO *lpMsg); // idb
	void  GC_SendBlockUserListInfo(LPOBJ lpObj, char btResType, char btResult); // idb

	void  CG_ReqAddBlockChattingUser(PMSG_REQ_ADD_BLOCK_CHAT_USER *lpMsg, int aIndex); // idb
	void  CG_ReqDelBlockChattingUser(PMSG_REQ_DEL_BLOCK_CHAT_USER *lpMsg, int aIndex); // idb
	void  GD_ReqAddBlockChatUserInfo(LPOBJ lpObj, _ST_BLOCK_CHATTING_USER *stBCUL); // idb

	void  GD_ReqDelBlockChatUserInfo(LPOBJ lpObj, _ST_BLOCK_CHATTING_USER *stBCUL); // idb
private:
	_ST_BLOCK_CHAT_USER_INFO m_stBlockCharUser[1000];
};
extern CBlockChatUserSystem g_CBlockChatUserSystem;