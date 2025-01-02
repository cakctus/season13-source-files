#pragma once
#include "Protocol.h"
/* 6572 */
struct PMSG_ANS_RESTORE_ITEM
{
	PSBMSG_HEAD h;
	BYTE result;
};
/* 6659 */
struct PMSG_REQ_RESTOREITEM_LIST
{
	PSBMSG_HEAD h;
};
/* 6660 */
struct PMSG_REQ_QUIT_RESTORE_INTERFACE
{
	PSBMSG_HEAD h;
};
/* 6661 */
//#pragma pack (2)
struct  PMSG_REQ_RESTORE_ITEM
{
	PSBMSG_HEAD h;
	BYTE btIndex;
	unsigned int dwExpireSec;
	int iRequireMoney;
	WORD wItemType;
	BYTE btIndexCode;
};
//#pragma pack()
/* 6662 */
struct _SDHP_ANS_RESTORE_ITEM_LIST
{
	PSWMSG_HEAD h;
	int userIndex;
	BYTE result;
	BYTE dbRestoreItemData[150];
	BYTE dbVersion;
};
struct   _SDHP_REQ_RESTORE_ITEM_LIST
{
	PSWMSG_HEAD h;
	int userIndex;
	char AccountID[11];
	char Name[11];
};
/* 7498 */
struct _SDHP_REQ_RESTORE_ITEM_LIST_SAVE
{
	PSWMSG_HEAD h;
	int userIndex;
	char AccountID[11];
	char Name[11];
	char dbRestoreItemData[150];
};
/* 7552 */
struct PMSG_ANS_RESTOREITEM_LIST_COUNT
{
	PSWMSG_HEAD h;
	BYTE btResult;
	BYTE btItemCnt;
};

/* 7586 */
#pragma pack (1)
struct _RESTORE_ITEM_INFO
{
	_RESTORE_ITEM_INFO()
	{
		this->Clear();
	}
	void Clear()
	{
		this->expireSec = 0;
		memset(this->itemInfo, 0, sizeof(itemInfo));
		this->requireMoney = 0;
		this->itemCount = 0;
	}
	unsigned int expireSec;
	BYTE itemInfo[12];
	WORD itemCount;
	int requireMoney;
	BYTE indexCode;
};
#pragma pack()
class CRestoreSoldItem
{
public:
	CRestoreSoldItem();
	~CRestoreSoldItem();

	void SetRestoreItemUseFlag(int useRestoreItemFlag); // idb
	bool GetRestoreItemUseFlag(); // idb
	void LoadRestoreItemFlag(); // idb
	BYTE IsActivateRestoreInventory(LPOBJ lpObj);
	int CheckRestoreItemCondition(LPOBJ lpObj, int itemPos);
	int RegisterRestoreSoldItem(LPOBJ lpObj, int itemPos, int sellMoney, bool *isRegisterPentagram); // idb
	int RegisterRestoreSoldMuunItem(LPOBJ lpObj, int itemPos, int sellMoney, bool *isRegisterPentagram); // idb
	int GetLeastExpireTimeRestoreItemIndex(LPOBJ lpObj);
	int GetRestoreItemCount(CItem *item);
	void CGReqRestoreItemList(PMSG_REQ_RESTOREITEM_LIST *lpMsg, int aIndex, bool resend); // idb
	void CGReqQuitRestoreItemInterface(PMSG_REQ_QUIT_RESTORE_INTERFACE *lpMsg, int aIndex);
	void CGReqRestoreItem(PMSG_REQ_RESTORE_ITEM *lpMsg, int aIndex); // idb
	void GDReqRestoreItemListLoad(LPOBJ lpObj); // idb
	void DGAnsRestoreItemListInfo(_SDHP_ANS_RESTORE_ITEM_LIST *lpMsg); // idb
	char GDReqSaveRestoreItemListInven(LPOBJ lpObj, int restoreItemCnt);
private:
	bool m_UseRestoreItemOnOff;
};

extern CRestoreSoldItem gCRestoreSoldItem;