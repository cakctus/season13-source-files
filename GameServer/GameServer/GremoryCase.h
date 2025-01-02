
#pragma once
#include "Protocol.h"
#include "Item.h"
#include "ItemManager.h"
#define MAX_ACCOUNT_LEN	10
#define MAX_ITEM_INFO_CASE 16
#define MAX_GREMORYCASE_STORAGE_TYPES 2
#define MAX_GREMORYCASE_STORAGE_ITEMS 50

enum GREMORYCASE_STORAGE_TYPE
{
	GC_STORAGE_SERVER = 1,
	GC_STORAGE_CHARACTER = 2
};

enum GREMORYCASE_REWARD_SOURCES //need to find imperial id
{
	GC_REWARD_CHAOS_CASTLE = 1,
	GC_REWARD_BLOOD_CASTLE = 2,
	GC_REWARD_ILLUSION_TEMPLE = 3,
	GC_REWARD_DOPPELGANGER = 4,
	GC_REWARD_CHAOS_CASTLE_SURVIVAL = 5,
	GC_REWARD_TORMENTED_SQUARE = 6,
	GC_REWARD_ILLUSION_TEMPLE_LEAGUE = 7,
	GC_REWARD_CHAOS_CASTLE_SURVIVAL_2 = 8,
	GC_REWARD_TORMENTED_SQUARE_LEAGUE = 9,
	GC_REWARD_CHAOS_CASTLE_UBF = 10,
	GC_REWARD_LABYRITH=11,
	GC_REWARD_SUMMONEVOMON=12,
	GC_REWARD_DEVIL_SQUARE = 13,
	GC_REWARD_VARKA_MAP = 14,
	GC_REWARD_UNKNOWN = 15,
	GC_REWARD_PROVIDE_ELEMENTAL = 16,
	GC_REWARD_EVENT = 17,
	GC_REWARD_MAZE_DIMENSIONS = 18,
	GC_REWARD_QUEST = 19,
	GC_REWARD_GM_REWARD = 100
};

//struct GREMORYCASE_ITEM_DATA
//{
//	GREMORYCASE_ITEM_DATA()
//	{
//		this->Clear();
//	}
//
//	void Clear()
//	{
//		this->btRewardInventory = 0;
//		this->btRewardSource = 0;
//		this->dwItemGUID = (DWORD)-1;
//		this->ItemInfo.Clear();
//		this->dwAuthCode = 0;
//		this->dwReceiveDate = 0;
//		this->dwExpireTime = 0;
//	}
//
//	BYTE btRewardInventory;
//	BYTE btRewardSource;
//	DWORD dwItemGUID;
//	CItem ItemInfo;
//	DWORD dwAuthCode;
//	DWORD dwReceiveDate;
//	DWORD dwExpireTime;
//};

struct PMSG_GREMORYCASE_OPEN_ANS // 4F:05 GS->CL
{
	PSBMSG_HEAD h;
	BYTE btResult;
};

struct _stAnsGremoryCaseItemList
{
	PSWMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	BYTE btResult;
	BYTE btCount;
};

struct _stReqGremoryCaseItemList
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
};

struct _stDSGremoryCaseItem
{
	DWORD ItemGUID;
	time_t iReceiveDate;
	time_t iExpireDate;
	BYTE btRewardSource;
	BYTE btStorageType;
	DWORD dwAuthCode;
	BYTE ItemInfo[MAX_ITEM_INFO_CASE];
};

struct _stGremoryCaseItem
{
	_stGremoryCaseItem()
	{
	this->ItemGUID;
	//this->m_Index = 0;
	//this->m_Level = 0;
	//this->m_Durability = 0;
	//this->m_Option1 = 0;
	//this->m_Option2 = 0;
	//this->m_Option3 = 0;
	//this->m_NewOption = 0;
	//this->m_SetOption = 0;
	//this->m_JewelOfHarmonyOption = 0;
	//this->m_ItemOptionEx = 0;
	//this->m_SocketOption[5] = 0;

	this->iReceiveDate = 0;
	this->iExpireDate = 0;
	this->btRewardSource=0;
	this->btStorageType=0;
	this->dwAuthCode=0;
	this->ItemInfo.Clear();
	}

	
	
	DWORD ItemGUID;
	//WORD m_Index;
	//short m_Level;
	//float m_Durability;
	//BYTE m_Option1;
	//BYTE m_Option2;
	//BYTE m_Option3;
	//BYTE m_NewOption;
	//BYTE m_SetOption;
	//BYTE m_JewelOfHarmonyOption;
	//BYTE m_ItemOptionEx;
	//BYTE m_SocketOption[5];
	time_t iReceiveDate;
	time_t iExpireDate;
	BYTE btRewardSource;
	BYTE btStorageType;
	DWORD dwAuthCode;
	CItem ItemInfo;
};
struct _stReqDeleteItemFromGremoryCase
{
	PSBMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	DWORD dwItemGUID;
	DWORD dwAuthCode;
};

struct PMSG_GREMORYCASE_SENT_NOTICE // 4F:04 GS->CL
{
	PSBMSG_HEAD h;
	BYTE btNoticeIndex;
};

struct _stAnsAddItemToGremoryCase
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	BYTE btResult;
	//_stGremoryCaseItem m_GremoryCaseItem;
	DWORD ItemGUID;
	time_t iReceiveDate;
	time_t iExpireDate;
	BYTE btRewardSource;
	BYTE btStorageType;
	DWORD dwAuthCode;
	BYTE ItemInfo[MAX_ITEM_INFO_CASE];
};

struct _stAnsCheckUseItemGremoryCase
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	BYTE btResult;
	DWORD dwItemGUID;
	DWORD dwAuthCode;
};

struct _stReqAddItemToGremoryCase
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	//_stGremoryCaseItem m_GremoryCaseItem;
	time_t iReceiveDate;
	time_t iExpireDate;
	BYTE btRewardSource;
	BYTE btStorageType;
	DWORD dwAuthCode;
	BYTE ItemInfo[MAX_ITEM_INFO_CASE];
};

struct PMSG_REMOVE_ITEM_FROM_GREMORYCASE // 4F:03 GS->CL
{
	PSBMSG_HEAD h;
	BYTE btStorageType;
	DWORD dwAuthCode;
	DWORD dwItemGUID;
}; 

struct PMSG_GREMORYCASE_ITEM
{
	BYTE btRewardInventory;
	BYTE btRewardSource;
	DWORD dwItemGUID;
	BYTE btItemInfo[MAX_ITEM_INFO];
	DWORD dwAuthCode;
	DWORD dwExpireTime;
};

struct PMSG_RECEIVE_GREMORYCASE_ITEMLIST // 4F:00 GS->CL
{
	PSWMSG_HEAD h;
	BYTE btCount;
};

#pragma pack (1)
struct PMSG_RECEIVE_GREMORYCASE_ITEM // 4F:01 GS->CL
{
	PSBMSG_HEAD h;
	PMSG_GREMORYCASE_ITEM m_ReceivedItem;
};
#pragma pack ()

struct PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_REQ // 4F:02 CL->GS
{
	PSBMSG_HEAD h;
	WORD wItemID;
	DWORD dwItemGUID;
	DWORD dwAuthCode;
};

struct PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_ANS // 4F:02 GS->CL
{
	PSBMSG_HEAD h;
	BYTE btResult;
	BYTE btStorageType;
	DWORD dwAuthCode;
	DWORD dwItemGUID;
};

struct _stReqCheckUseItemGremoryCase
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	DWORD dwItemGUID;
	DWORD dwAuthCode;
};

class CGremoryCase
{
public:
	CGremoryCase(void);
	virtual ~CGremoryCase(void);

	void CheckStorageExpiredItems();

	void GDReqStorageItemList(int iIndex);
	void DGAnsStorageItemList(LPBYTE lpRecv);
	void GDReqAddItemToGremoryCase(int iIndex, _stGremoryCaseItem stItem, int iDaysToExpire);
	void DGAnsAddItemToGremoryCase(_stAnsAddItemToGremoryCase * lpMsg);
	void GDReqCheckItemUseGremoryCase(int iIndex,WORD wItemID, DWORD dwItemGUID, DWORD dwAuthCode);
	void DGAnsCheckItemUseGremoryCase(_stAnsCheckUseItemGremoryCase * lpMsg);
	void GDReqDeleteItemFromGremoryCase(int iIndex, DWORD dwItemGUID, DWORD dwAuthCode);

	void GCSendStorageItemList(int iIndex);
	void GCSendAddItemToGremoryCase(int iIndex, BYTE btStorageType, BYTE btItemArrayIndex);
	void CGReqGetItemFromGremoryCase(PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_REQ * lpMsg, int iIndex);
	void GCSendDeleteItemFromGremoryCase(int iIndex, BYTE btStorageType, int iItemArrayIndex);
	void CGReqOpenGremoryCase(int iIndex);


private:
	void CheckIsInStorageItemAboutToExpire(int iIndex);
	void CheckIsStorageFull(int iIndex);

};

extern CGremoryCase g_GremoryCase;
struct PMSG_GREMORY_CASE
{
	PSBMSG_HEAD h;
	char type;	
};

void GremoryCase(PMSG_GREMORY_CASE*lpMsg,int aIndex);
void ItemByteConvert(unsigned char* buf, CItem item);