#include "Item.h"
#include "Protocol.h"
#include "ItemManager.h"
#define RHEA_SHOP_SIZE 120

#define RHEA_SHOP_INVENTORY_RANGE(x) (((x)<0)?0:((x)>=RHEA_SHOP_SIZE)?0:1)
struct PMSG_RHEA_SHOP_ITEM_LIST_SEND
{
	PWMSG_HEAD header; // C2:31
	BYTE type;
	BYTE count;
};

struct PMSG_RHEA_SHOP_ITEM_LIST
{
	BYTE slot;
	BYTE ItemInfo[MAX_ITEM_INFO];
};
struct PMSG_RHEA_ITEM_BUY_RECV
{
	PSBMSG_HEAD header; // C1:32
	BYTE slot;
};
struct PMSG_RHEA_ITEM_BUY_SEND
{
	PSBMSG_HEAD header; // C1:32
	BYTE result;
	BYTE ItemInfo[MAX_ITEM_INFO];
};
/* 6906 */
struct SDHP_RUUDTOKENUPDATE
{
	PBMSG_HEAD h;
	char AccountID[11];
	char CharName[11];
	unsigned int dwRuud;
};
/* 7448 */
struct PMSG_RUUD_TOKEN_UPDATE
{
	PSBMSG_HEAD h;
	unsigned int dwRuudPoint;
	unsigned int dwAddPoint;
	bool bGet;
};
class CRheaGold
{
public:
	CRheaGold();
	virtual ~CRheaGold();
	void Init();
	void Load(char* path);
	bool GetItem(CItem* lpItem,int slot);
	void ShopItemSet(int slot,BYTE type);
	void RheaGoldItemList(LPOBJ lpObj);
	BYTE ShopRectCheck(int x,int y,int width,int height);
	void RheaItemRecv(PMSG_RHEA_ITEM_BUY_RECV *lpMsg,int aIndex);
	void RheaItemSend(int aIndex,CItem lpItem,BYTE Result);
	void InsertItem(int ItemIndex,int ItemLevel,int ItemDurability,int ItemOption1,int ItemOption2,int ItemOption3,int ItemNewOption,int ItemValue,int set,int Price); // OK
	BYTE ItemNewOptionR(int type);
	int BloodItemMixList(int ItemIndex);
	int GetItemNewOption(int ItemNewOption,int Type);
	void  GD_RuudTokenCountUpdate(LPOBJ lpObj, unsigned int dwRuud);
	void  GC_Notify_RuudUpdate(int aIndex, unsigned int dwRuud, unsigned int dwAddRuud, bool bGet);
private:
	CItem m_Item[RHEA_SHOP_SIZE];
	BYTE m_InventoryMap[RHEA_SHOP_SIZE];

};

extern CRheaGold gRheaGold;