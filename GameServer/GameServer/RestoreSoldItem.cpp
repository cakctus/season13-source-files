#include "stdafx.h"
#include "RestoreSoldItem.h"
#include "ItemManager.h"
#include "Util.h"
#include "PentagramSystem.h"
#include "Notice.h"
#include "Message.h"
#include "MapItem.h"
#include "MuunSystem.h"
#include "GameMain.h"
#include "ItemMove.h"
CRestoreSoldItem gCRestoreSoldItem;

CRestoreSoldItem::CRestoreSoldItem()
{
}


CRestoreSoldItem::~CRestoreSoldItem()
{
}


//----- (00CB45F0) --------------------------------------------------------
void  CRestoreSoldItem::SetRestoreItemUseFlag(int useRestoreItemFlag)
{
	if (useRestoreItemFlag)
		this->m_UseRestoreItemOnOff = 1;
	else
		this->m_UseRestoreItemOnOff = 0;
}

//----- (00CB4630) --------------------------------------------------------
bool  CRestoreSoldItem::GetRestoreItemUseFlag()
{
	return this->m_UseRestoreItemOnOff;
}

//----- (00CB4650) --------------------------------------------------------
void  CRestoreSoldItem::LoadRestoreItemFlag()
{
	UINT useRestoreItemFlag; // ST5C_4@1
	useRestoreItemFlag = GetPrivateProfileIntA("GameServerInfo", "UseRestoreItemFeature", 0, ".\\Data\\GameServerInfo - Common.dat");
	this->SetRestoreItemUseFlag(useRestoreItemFlag);
}

//----- (00CB46B0) --------------------------------------------------------
BYTE  CRestoreSoldItem::IsActivateRestoreInventory(LPOBJ lpObj)
{
	char result; // al@2

	if (this->m_UseRestoreItemOnOff)
	{
		if (lpObj->bRestoreInventoryLoad)
		{
			result = 1;
		}
		else
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] Restore item inventory didn't loaded.", lpObj->Account, lpObj->Name);
			result = 0;
		}
	}
	else
	{
		//LogAdd(LOG_BLACK,"[RestoreItem] Restore item feature flag is OFF (%d)", this->m_UseRestoreItemOnOff);
		result = 0;
	}
	return result;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00CB4740) --------------------------------------------------------
int  CRestoreSoldItem::CheckRestoreItemCondition(LPOBJ lpObj, int itemPos)
{
	int result; // eax@2
	ITEM_INFO lpItem; // [sp+4Ch] [bp-8h]@10

	if (lpObj)
	{
		if (itemPos <= 203 || itemPos == 236)
		{
			if (lpObj->Inventory[itemPos].IsItem())
			{
				if (lpObj->Inventory[itemPos].m_IsPeriodicItem == 1)
				{
					result = 2;
				}
				else
				{
					
					if (gItemManager.GetInfo(lpObj->Inventory[itemPos].m_Index, &lpItem))
					{
						if (lpObj->Inventory[itemPos].IsMuunItem() != 1
							&& lpObj->Inventory[itemPos].IsMuunUtil() != 1)
						{
							result = lpObj->Inventory[itemPos].m_Level >= 7
								|| lpObj->Inventory[itemPos].IsSetItem()
								|| lpObj->Inventory[itemPos].IsExcItem()
								||  lpObj->Inventory[itemPos].IsPentagramItem()
								&& lpObj->Inventory[itemPos].m_Level >= 2
								||gItemMove.CheckItemMoveAllowSell(lpObj->Inventory[itemPos].m_Index);
						}
						else
						{
							result = 0;
						}
					}
					else
					{
						result = -4;
					}
				}
			}
			else
			{
				result = -3;
			}
		}
		else
		{
			result = -2;
		}
	}
	else
	{
		result = -1;
	}
	return result;
}

//----- (00CB4980) --------------------------------------------------------
int  CRestoreSoldItem::RegisterRestoreSoldItem(LPOBJ lpObj, int itemPos, int sellMoney, bool *isRegisterPentagram)
{
	

	bool findDeletedRestoreItem; // [sp+8Bh] [bp-15h]@21
	int result; // [sp+8Ch] [bp-14h]@15

	*isRegisterPentagram = 0;
	if (this->IsActivateRestoreInventory(lpObj))
	{
		if (itemPos > 203 && itemPos != 236)
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] RegisterRestore Sold Item. Invalid item pos.(%d)", itemPos);
			return -3;
		}
		if (lpObj->Interface.use && lpObj->Interface.type!= 3)
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s](%d:%d/%d) Not used shop interface. (%d)(%d)",lpObj->Account,lpObj->Name,lpObj->Interface.use,lpObj->Interface.type);
			return -1;
		}
		CItem *item = &lpObj->Inventory[itemPos];
		int index = -1;
		result = 0;
		if (lpObj->btRestoreInterface == 1)
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s](%d:%d/%d) Already use restore item interface. wouldn't be register sold item. (%d)",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,lpObj->btRestoreInterface);
			return -2;
		}
		if (item->m_IsPeriodicItem == 1)
			return 1;
		if (this->CheckRestoreItemCondition(lpObj, itemPos) != 1)
			return 0;
		findDeletedRestoreItem = 0;
		for (int i = 0; i < 5; ++i)
		{
			if (lpObj->pRestoreInventory[i].restoreFlag == 1 || lpObj->pRestoreInventory[i].expireFlag == 1)
			{
				index = i;
				findDeletedRestoreItem = 1;
				break;
			}
			if (!lpObj->pRestoreInventory[i].restoreItem.IsItem())
			{
				index = i;
				break;
			}
		}
		if (index == -1)
		{
			index = this->GetLeastExpireTimeRestoreItemIndex(lpObj);
			if (index == -1)
				return 0xFC;
			findDeletedRestoreItem = 1;
		}

		if (findDeletedRestoreItem == 1)
		{			
			/*LogAdd(LOG_BLACK,
				"[%s][%s]RIREG_EX[%d,%s,%d,%d,%d,%d]serial:[%u] dur:[%d] Set[%d] 380:[%d] HO:[%d] SC"
				"[%d,%d,%d,%d,%d] BonusOption[%d]  ET[%04d-%02d-%02d %02d:%02d:%02d]",
				lpObj->Account,
				lpObj->Name,
				index,
				gItemManager.GetName(lpObj->pRestoreInventory[index].restoreItem.m_Index),
				lpObj->pRestoreInventory[index].restoreItem.m_Level,
				lpObj->pRestoreInventory[index].restoreItem.m_Option1,
				lpObj->pRestoreInventory[index].restoreItem.m_Option2,
				lpObj->pRestoreInventory[index].restoreItem.m_Option3,
				lpObj->pRestoreInventory[index].restoreItem.m_Index,
				lpObj->pRestoreInventory[index].restoreItem.m_Durability,
				lpObj->pRestoreInventory[index].restoreItem.m_SetOption,
				lpObj->pRestoreInventory[index].restoreItem.m_ItemOptionEx>>7,
				lpObj->pRestoreInventory[index].restoreItem.m_JewelOfHarmonyOption,
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[0],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[1],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[2],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[3],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[4],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption,
				lpObj->pRestoreInventory[index].expireTime.GetYear(),
				lpObj->pRestoreInventory[index].expireTime.GetMonth(),
				lpObj->pRestoreInventory[index].expireTime.GetDay(),
				lpObj->pRestoreInventory[index].expireTime.GetHour(),
				lpObj->pRestoreInventory[index].expireTime.GetMinute(),
				lpObj->pRestoreInventory[index].expireTime.GetSecond());*/
		}

		if (lpObj->pRestoreInventory[index].restoreItem.IsPentagramItem() == 1)
			gPentagramSystem.DelAllPentagramJewelInfo(lpObj, &lpObj->pRestoreInventory[index].restoreItem, PENTAGRAM_JEWEL_TYPE_INVENTORY);
		lpObj->pRestoreInventory[index].Clear();

		lpObj->pRestoreInventory[index].reqMoney = sellMoney + 10 * sellMoney / 0x64;
		if (lpObj->pRestoreInventory[index].reqMoney > MAX_MONEY)
			lpObj->pRestoreInventory[index].reqMoney = MAX_MONEY;
		
		CTimeSpan expireHour(0, 23, 59, 0);
		lpObj->pRestoreInventory[index].expireTime = CTime::GetTickCount() + expireHour.GetTimeSpan();

		memcpy(&lpObj->pRestoreInventory[index].restoreItem, item, sizeof(lpObj->pRestoreInventory[index].restoreItem));
		if (lpObj->pRestoreInventory[index].restoreItem.IsPentagramItem() == 1)
		{
			*isRegisterPentagram = 1;
			return 3;
		}
		else
		{
			return 2;
		}
	}
	else if (gObjIsConnected(lpObj->Index) == 1)
	{
		if (!this->GetRestoreItemUseFlag())
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "装备恢复被禁用");

		}
		if (!lpObj->bRestoreInventoryLoad)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "无法加载需要恢复的物品信息");
		}
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "您无法使用商品还原功能，并且无法恢复现在销售的商品。");
		return  0xF9;
	}
	else
	{
		return  0xFB;
	}
	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// 10839630: using guessed type char *__thiscall CMsg::Get(CMsg *__hidden this, _DWORD);

//----- (00CB54D0) --------------------------------------------------------
int  CRestoreSoldItem::RegisterRestoreSoldMuunItem(LPOBJ lpObj, int itemPos, int sellMoney, bool *isRegisterPentagram)
{

	bool findDeletedRestoreItem; // [sp+8Bh] [bp-15h]@23

	int index; // [sp+90h] [bp-10h]@16

	*isRegisterPentagram = 0;
	if (this->IsActivateRestoreInventory(lpObj))
	{
		if (itemPos > 101)
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] RegisterRestore Sold Muun Item. Invalid item pos.(%d)", itemPos);
			return -3;
		}
		if (lpObj->Interface.use&&lpObj->Interface.type  != 3)
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s](%d:%d/%d) Not used Shop Interface. (%d)(%d)",lpObj->Account,lpObj->Name,lpObj->Interface.use,lpObj->Interface.type);
			return -1;
		}
		if (lpObj->btRestoreInterface == 1)
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s](%d:%d/%d) Already use restore item interface. wouldn't be register sold item. (%d)",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,lpObj->btRestoreInterface);
			return -2;
		}
		CItem* item = &lpObj->MuunInventory[itemPos];
		index = -1;

		if (item->IsMuunItem() != 1
			&& item->IsMuunUtil() != 1
			&& item->m_Index != GET_ITEM(16,239)
			&& item->m_Index != GET_ITEM(16, 240))
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] (%d:%d/%d) Register restore sold muun item. - Not muun item.(%d)",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,item->m_Index);
			return -4;
		}
		if (item->m_IsPeriodicItem == 1)
			return 1;
		findDeletedRestoreItem = 0;
		for (int i = 0; i < 5; ++i)
		{
			if (lpObj->pRestoreInventory[i].restoreFlag >= 1 || lpObj->pRestoreInventory[i].expireFlag >= 1)
			{
				index = i;
				findDeletedRestoreItem = 1;
				break;
			}
			if (!lpObj->pRestoreInventory[i].restoreItem.IsItem())
			{
				index = i;
				break;
			}
		}
		if (index == -1)
		{
			index = this->GetLeastExpireTimeRestoreItemIndex(lpObj);
			if (index == -1)
				return -4;
			findDeletedRestoreItem = 1;
		}
		if (findDeletedRestoreItem == 1)
		{
			/*LogAdd(LOG_BLACK,
				"[%s][%s]RIREG_EX[%d,%s,%d,%d,%d,%d]serial:[%u] dur:[%d] Set[%d] 380:[%d] HO:[%d] SC"
				"[%d,%d,%d,%d,%d] BonusOption[%d]  ET[%04d-%02d-%02d %02d:%02d:%02d]",
				lpObj->Account,
				lpObj->Name,
				index,
				gItemManager.GetName(lpObj->pRestoreInventory[index].restoreItem.m_Index),
				lpObj->pRestoreInventory[index].restoreItem.m_Level,
				lpObj->pRestoreInventory[index].restoreItem.m_Option1,
				lpObj->pRestoreInventory[index].restoreItem.m_Option2,
				lpObj->pRestoreInventory[index].restoreItem.m_Option3,
				lpObj->pRestoreInventory[index].restoreItem.m_Index,
				lpObj->pRestoreInventory[index].restoreItem.m_Durability,
				lpObj->pRestoreInventory[index].restoreItem.m_SetOption,
				lpObj->pRestoreInventory[index].restoreItem.m_ItemOptionEx >> 7,
				lpObj->pRestoreInventory[index].restoreItem.m_JewelOfHarmonyOption,
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[0],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[1],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[2],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[3],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption[4],
				lpObj->pRestoreInventory[index].restoreItem.m_SocketOption,
				lpObj->pRestoreInventory[index].expireTime.GetYear(),
				lpObj->pRestoreInventory[index].expireTime.GetMonth(),
				lpObj->pRestoreInventory[index].expireTime.GetDay(),
				lpObj->pRestoreInventory[index].expireTime.GetHour(),
				lpObj->pRestoreInventory[index].expireTime.GetMinute(),
				lpObj->pRestoreInventory[index].expireTime.GetSecond());*/

		}
		if (lpObj->pRestoreInventory[index].restoreItem.IsPentagramItem() == 1)
			gPentagramSystem.DelAllPentagramJewelInfo(lpObj, &lpObj->pRestoreInventory[index].restoreItem, PENTAGRAM_JEWEL_TYPE_INVENTORY);
		lpObj->pRestoreInventory[index].Clear();

		lpObj->pRestoreInventory[index].reqMoney = sellMoney + 10 * sellMoney / 0x64;
		if (lpObj->pRestoreInventory[index].reqMoney > MAX_MONEY)
			lpObj->pRestoreInventory[index].reqMoney = MAX_MONEY;

		CTimeSpan expireHour(0, 23, 59, 0);
		lpObj->pRestoreInventory[index].expireTime = CTime::GetTickCount() + expireHour.GetTimeSpan();

		memcpy(&lpObj->pRestoreInventory[index].restoreItem, item, sizeof(lpObj->pRestoreInventory[index].restoreItem));
		if (lpObj->pRestoreInventory[index].restoreItem.IsPentagramItem() == 1)
		{
			*isRegisterPentagram = 1;
			return 3;
		}
		else
		{
			return 2;
		}
	}
	else if (gObjIsConnected(lpObj->Index) == 1)
	{
		if (!this->GetRestoreItemUseFlag())
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "装备恢复被禁用");

		}
		if (!lpObj->bRestoreInventoryLoad)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "无法加载需要恢复的物品信息");
		}
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "您无法使用商品还原功能，并且无法恢复现在销售的商品。");

		return  0xF9;
	}
	else
	{
		return  0xFB;
	}

	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// 10839630: using guessed type char *__thiscall CMsg::Get(CMsg *__hidden this, _DWORD);

//----- (00CB60C0) --------------------------------------------------------
int  CRestoreSoldItem::GetLeastExpireTimeRestoreItemIndex(LPOBJ lpObj)
{
	int index = -1;
	__int64 smallerExpireTime = lpObj->pRestoreInventory->expireTime.GetTime();

	for (int i = 0; i < 5; ++i)
	{
		if (lpObj->pRestoreInventory[i].expireTime.GetTime()<= smallerExpireTime)
		{
			index = i;
			smallerExpireTime = lpObj->pRestoreInventory[i].expireTime.GetTime();
		}
	}
	return index;
}

//----- (00CB6190) --------------------------------------------------------
int  CRestoreSoldItem::GetRestoreItemCount(CItem *item)
{//需要重新修改
	ITEM_INFO pItemAttribute; // [sp+4Ch] [bp-Ch]@3
	int itemCount = -1;
	if (item)
	{
		if (item->IsItem())
		{
			
			if (gItemManager.GetInfo(item->m_Index, &pItemAttribute))
			{
				if (pItemAttribute.Value <= 0)
				{
					if (item->m_Index == GET_ITEM(12, 30)
						|| item->m_Index == GET_ITEM(12, 31)
						|| item->m_Index == GET_ITEM(12, 136)
						|| item->m_Index == GET_ITEM(12, 137)
						|| item->m_Index == GET_ITEM(12, 138)
						|| item->m_Index == GET_ITEM(12, 139)
						|| item->m_Index == GET_ITEM(12, 140)
						|| item->m_Index == GET_ITEM(12, 141)
						|| item->m_Index == GET_ITEM(12, 142)
						|| item->m_Index == GET_ITEM(12, 143))
					{

						if (item->m_Level)
						{
							if (item->m_Level == 1)
								itemCount = 20;
							else if (item->m_Level == 2)
							{
								itemCount = 30;
							}
						}
						else
						{
							itemCount = 10;
						}
					}
					else
					{
						itemCount = 1;
					}
				}
				else
				{
					itemCount=item->m_Durability;
				}
			}
		}
	}
	return itemCount;
}

//----- (00CB6360) --------------------------------------------------------
void  CRestoreSoldItem::CGReqRestoreItemList(PMSG_REQ_RESTOREITEM_LIST *lpMsg, int aIndex, bool resend)
{

	ATL::CTime *v8; // eax@34
	CItem v11; // [sp-D8h] [bp-604h]@34
	ATL::CTime result; // [sp+18h] [bp-514h]@34
	unsigned int nowTime; // [sp+74h] [bp-4B8h]@34
	unsigned int expireTime; // [sp+78h] [bp-4B4h]@34
	char btResult; // [sp+97h] [bp-495h]@25
	int itemListCnt; // [sp+98h] [bp-494h]@25
	int lOfs; // [sp+9Ch] [bp-490h]@25
	_RESTORE_ITEM_INFO restoreItemInfo[5]; // [sp+A0h] [bp-48Ch]@25
	PMSG_ANS_RESTOREITEM_LIST_COUNT pMsg; // [sp+114h] [bp-418h]@41
	BYTE sendBuf[1024]; // [sp+11Ch] [bp-410h]@25
	int nRet; // [sp+51Ch] [bp-10h]@25

	if (gObjIsConnected(aIndex) == 0)
	{
		return ;
	}

	LPOBJ lpObj = &gObj[aIndex];
		if (this->IsActivateRestoreInventory(&gObj[aIndex]))
		{
			if (lpObj->MapServerMoveQuit != 1
				&& lpObj->MapServerMoveRequest != 1
				&& (!lpObj->Interface.use|| lpObj->Interface.type == 3)
				&& lpObj->CloseCount <= 0)
			{
				if (resend == 1)
				{
					if (lpObj->btRestoreInterface != 1)
					{
						//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s](Map:%d)(%d/%d) - Not use 'restore item interface' state.(%d)(%d)",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,lpObj->Interface.use,lpObj->Interface.type);
						return;
					}
				}
				else if (lpObj->btRestoreInterface)
				{
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s](Map:%d)(%d/%d) - Already use restore item interface.(%d)(%d)",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,lpObj->Interface.use,lpObj->Interface.type);
					return;
				}
				nRet = 0;

				memset(sendBuf, 0, sizeof(sendBuf));


					itemListCnt = 0;
					lOfs = sizeof(pMsg);
					btResult = -1;
					memset(restoreItemInfo, 0, sizeof(restoreItemInfo));
					for (int i = 0; i < 5; ++i)
					{
						if (lpObj->pRestoreInventory[i].restoreFlag <= 0 && lpObj->pRestoreInventory[i].expireFlag <= 0)
						{
							if (lpObj->pRestoreInventory[i].restoreItem.IsItem())
							{
								memcpy(&v11, &lpObj->pRestoreInventory[i].restoreItem, sizeof(v11));
								gItemManager.ItemByteConvert(restoreItemInfo[itemListCnt].itemInfo, v11);

								expireTime = lpObj->pRestoreInventory[i].expireTime.GetTime();
								v8 = &result.GetTickCount();
								nowTime = v8->GetTime();
								if (nowTime < expireTime)
								{
									restoreItemInfo[itemListCnt].expireSec = expireTime - time(0);
									lpObj->pRestoreInventory[i].secTimeKey = restoreItemInfo[itemListCnt].expireSec;
									restoreItemInfo[itemListCnt].indexCode = i;
									restoreItemInfo[itemListCnt].requireMoney = lpObj->pRestoreInventory[i].reqMoney;
									restoreItemInfo[itemListCnt].itemCount = this->GetRestoreItemCount(&lpObj->pRestoreInventory[i].restoreItem);
									if (restoreItemInfo[itemListCnt].itemCount == -1)
										restoreItemInfo[itemListCnt].itemCount = 1;
									memcpy(&sendBuf[lOfs], (char *)&restoreItemInfo[itemListCnt], sizeof(restoreItemInfo[itemListCnt]));
									lOfs += sizeof(restoreItemInfo[itemListCnt]);
									++itemListCnt;
								}
								else
								{
									LogAdd(LOG_BLACK,
										"[%s][%s]RILIST_EX[%d,%s,%d,%d,%d,%d]serial:[%u] dur:[%d] Set[%d] 380:[%d] HO:[%d] SC"
										"[%d,%d,%d,%d,%d] BonusOption[%d]  ET[%04d-%02d-%02d %02d:%02d:%02d]",
										lpObj->Account,
										lpObj->Name,
										i,
										gItemManager.GetName(lpObj->pRestoreInventory[i].restoreItem.m_Index),
										lpObj->pRestoreInventory[i].restoreItem.m_Level,
										lpObj->pRestoreInventory[i].restoreItem.m_Option1,
										lpObj->pRestoreInventory[i].restoreItem.m_Option2,
										lpObj->pRestoreInventory[i].restoreItem.m_Option3,
										lpObj->pRestoreInventory[i].restoreItem.m_Index,
										lpObj->pRestoreInventory[i].restoreItem.m_Durability,
										lpObj->pRestoreInventory[i].restoreItem.m_SetOption,
										lpObj->pRestoreInventory[i].restoreItem.m_ItemOptionEx >> 7,
										lpObj->pRestoreInventory[i].restoreItem.m_JewelOfHarmonyOption,
										lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[0],
										lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[1],
										lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[2],
										lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[3],
										lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[4],
										lpObj->pRestoreInventory[i].restoreItem.m_SocketOption,
										lpObj->pRestoreInventory[i].expireTime.GetYear(),
										lpObj->pRestoreInventory[i].expireTime.GetMonth(),
										lpObj->pRestoreInventory[i].expireTime.GetDay(),
										lpObj->pRestoreInventory[i].expireTime.GetHour(),
										lpObj->pRestoreInventory[i].expireTime.GetMinute(),
										lpObj->pRestoreInventory[i].expireTime.GetSecond());
									if (lpObj->pRestoreInventory[i].restoreItem.IsPentagramItem())
									{

										gPentagramSystem.DelAllPentagramJewelInfo(lpObj, &lpObj->pRestoreInventory[i].restoreItem, PENTAGRAM_JEWEL_TYPE_INVENTORY);

									}

									lpObj->pRestoreInventory[i].Clear();
								}
							}
						}
						else
						{
							LogAdd(LOG_BLACK,
								"[%s][%s]RILIST_EX[%d,%s,%d,%d,%d,%d]serial:[%u] dur:[%d] Set[%d] 380:[%d] HO:[%d] SC"
								"[%d,%d,%d,%d,%d] BonusOption[%d]  ET[%04d-%02d-%02d %02d:%02d:%02d]",
								lpObj->Account,
								lpObj->Name,
								i,
								gItemManager.GetName(lpObj->pRestoreInventory[i].restoreItem.m_Index),
								lpObj->pRestoreInventory[i].restoreItem.m_Level,
								lpObj->pRestoreInventory[i].restoreItem.m_Option1,
								lpObj->pRestoreInventory[i].restoreItem.m_Option2,
								lpObj->pRestoreInventory[i].restoreItem.m_Option3,
								lpObj->pRestoreInventory[i].restoreItem.m_Index,
								lpObj->pRestoreInventory[i].restoreItem.m_Durability,
								lpObj->pRestoreInventory[i].restoreItem.m_SetOption,
								lpObj->pRestoreInventory[i].restoreItem.m_ItemOptionEx >> 7,
								lpObj->pRestoreInventory[i].restoreItem.m_JewelOfHarmonyOption,
								lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[0],
								lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[1],
								lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[2],
								lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[3],
								lpObj->pRestoreInventory[i].restoreItem.m_SocketOption[4],
								lpObj->pRestoreInventory[i].restoreItem.m_SocketOption,
								lpObj->pRestoreInventory[i].expireTime.GetYear(),
								lpObj->pRestoreInventory[i].expireTime.GetMonth(),
								lpObj->pRestoreInventory[i].expireTime.GetDay(),
								lpObj->pRestoreInventory[i].expireTime.GetHour(),
								lpObj->pRestoreInventory[i].expireTime.GetMinute(),
								lpObj->pRestoreInventory[i].expireTime.GetSecond());
							if (lpObj->pRestoreInventory[i].restoreItem.IsPentagramItem())
							{
								gPentagramSystem.DelAllPentagramJewelInfo(lpObj, &lpObj->pRestoreInventory[i].restoreItem, PENTAGRAM_JEWEL_TYPE_INVENTORY);

							}
							lpObj->pRestoreInventory[i].Clear();
						}
					}
					pMsg.h.set(0x6F, 0, sizeof(pMsg));

					pMsg.btResult = resend == 1;
					pMsg.btItemCnt = itemListCnt;
					pMsg.h.size[0] = SET_NUMBERHB(lOfs);
					pMsg.h.size[1] = SET_NUMBERLB(lOfs);
					memcpy(sendBuf, &pMsg, sizeof(pMsg));

					DataSend(lpObj->Index, sendBuf, lOfs);
					lpObj->btRestoreInterface = 1;
					lpObj->dwRestoreInterfaceTime = GetTickCount();
			}
		}
		else
		{
			if (!this->GetRestoreItemUseFlag())
			{
				//v44 = (CItem *)1;
				//v43 = (char *)lpObj->m_Index;
				//v4 = CMsg::Get((CMsg *)&lMsg, 3583);
				//GCServerMsgStringSend(v4, (int)v43, 1);
			}
			if (!lpObj->bRestoreInventoryLoad)
			{
				//v44 = (CItem *)1;
				//v43 = (char *)lpObj->m_Index;
				//v5 = CMsg::Get((CMsg *)&lMsg, 3582);
				//GCServerMsgStringSend(v5, (int)v43, 1);
			}
			//v44 = (CItem *)1;
			//v43 = (char *)lpObj->m_Index;
			//v6 = CMsg::Get((CMsg *)&lMsg, 3584);
			//GCServerMsgStringSend(v6, (int)v43, 1);
		}
	
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// 10839630: using guessed type char *__thiscall CMsg::Get(CMsg *__hidden this, _DWORD);

//----- (00CB78D0) --------------------------------------------------------
void  CRestoreSoldItem::CGReqQuitRestoreItemInterface(PMSG_REQ_QUIT_RESTORE_INTERFACE *lpMsg, int aIndex)
{

	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnected(lpObj->Index))
	{

		if (lpObj->btRestoreInterface <= 0)
		{
			//LogAdd(LOG_BLACK,"[RestoreItem] Already Not use RestoreItem Interface [%s][%s](%d:%d/%d)",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y);
		}
		else
		{
			lpObj->btRestoreInterface = 0;
			lpObj->dwRestoreInterfaceTime = 0;
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00CB79B0) --------------------------------------------------------
void  CRestoreSoldItem::CGReqRestoreItem(PMSG_REQ_RESTORE_ITEM *lpMsg, int aIndex)
{
	//CMapItem *Item;
	CMapItem *Itema;
	PMSG_ANS_RESTORE_ITEM pMsg; // [sp+2A8h] [bp-14h]@12
	ATL::CTime *v3; // eax@33
	ATL::CTime result; // [sp+18h] [bp-2A4h]@33
	CItem *tmpItem; // [sp+288h] [bp-34h]@37
	LPOBJ lpObj = &gObj[aIndex];


		if (gObjIsConnected(lpObj->Index))
		{
			if ((!lpObj->Interface.use || lpObj->Interface.type == 3)
				&& lpObj->btRestoreInterface
				&& lpObj->MapServerMoveQuit != 1
				&& lpObj->MapServerMoveRequest != 1)
			{
				pMsg.h.set(0x6f, 2, sizeof(pMsg));

				pMsg.result = -1;
				if (!gObjFixInventoryPointer(lpObj->Index))
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s] Fix.Inv Error state", lpObj->Account, lpObj->Name);
				if (!gObjFixMuunInventoryPointer(lpObj->Index))
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s] Fix.Muun.Inv Error state", lpObj->Account, lpObj->Name);
				if (lpObj->Transaction == 1)
				{
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s] Inventory Transaction State(%d), Use Interface(%d) - Use Value(%d)",lpObj->Account,lpObj->Name,lpObj->Transaction,lpObj->Interface.type,lpObj->Interface.use);
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
					return;
				}
				pMsg.result = -2;
				if (lpMsg->btIndexCode >= 0)
					Itema = (CMapItem *)(lpMsg->btIndexCode <= 4);
				else
					Itema = 0;
				if (!Itema)
				{
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
					return;
				}
				if (lpObj->pRestoreInventory[lpMsg->btIndexCode].secTimeKey != lpMsg->dwExpireSec)
				{
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s](%d:%d/%d) MissMatch, Restore Item TimeKey Info. (Item:%u)(Client:%u), ItemPos:%d",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,lpObj->pRestoreInventory[lpMsg->btIndexCode].secTimeKey,lpMsg->dwExpireSec,lpMsg->btIndexCode);
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
					return;
				}
				if (lpObj->pRestoreInventory[lpMsg->btIndexCode].reqMoney != lpMsg->iRequireMoney)
				{
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s](%d:%d/%d) MissMatch, Restore Item Require Money. (Item:%u)(Client:%u), ItemPos:%d",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,lpObj->pRestoreInventory[lpMsg->btIndexCode].reqMoney,lpMsg->iRequireMoney,lpMsg->btIndexCode);
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
					return;
				}
				if (!lpObj->pRestoreInventory[lpMsg->btIndexCode].restoreItem.IsItem())
				{
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s](%d:%d/%d) Restore item, Invalid. ItemPos:%d",lpObj->Account,lpObj->Name,lpMsg->btIndexCode);
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
					return;
				}
				if (lpObj->pRestoreInventory[lpMsg->btIndexCode].restoreItem.m_Index != lpMsg->wItemType)
				{
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s](%d:%d/%d) Restore item, Miss match Item Type. (Item:%d)(Client:%d), ItemPos:%d",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,lpObj->pRestoreInventory[lpMsg->btIndexCode].restoreItem.m_Index,lpMsg->wItemType,lpMsg->btIndexCode);
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
					return;
				}
				if (lpObj->Money < lpObj->pRestoreInventory[lpMsg->btIndexCode].reqMoney)
				{
					//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s](%d:%d/%d) Not enough Zen. (User Zen:%d)(Require Zen:%d)",lpObj->Account,lpObj->Name,lpObj->Map,lpObj->X,lpObj->Y,lpObj->Money,lpObj->pRestoreInventory[lpMsg->btIndexCode].reqMoney);
					pMsg.result = 1;
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
					return;
				}
				unsigned int expireTime = lpObj->pRestoreInventory[lpMsg->btIndexCode].expireTime.GetTime();
				v3 = &result.GetTickCount();// ATL::CTime::GetTickCount(&result);
				unsigned int nowTime = v3->GetTime();
				if (nowTime <= expireTime)
				{
					tmpItem = &lpObj->pRestoreInventory[lpMsg->btIndexCode].restoreItem;
					if (tmpItem->IsMuunItem() != 1
						&& tmpItem->m_Index != GET_ITEM(16,239)
						&& tmpItem->m_Index != GET_ITEM(16, 240))
					{
						BYTE blank = -1;

						blank = gItemManager.InventoryInsertItem(lpObj->Index, lpObj->pRestoreInventory[lpMsg->btIndexCode].restoreItem);
						if ((unsigned __int8)blank == 255)
						{
							gItemManager.GCItemListSend(lpObj->Index);

							pMsg.result = 2;
							DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
						}
						else
						{

							
							int v98 = lpObj->Money - lpObj->pRestoreInventory[lpMsg->btIndexCode].reqMoney;
							if (v98 < 0)
								v98 = 0;
							
							memcpy(&lpObj->Inventory[(unsigned __int8)blank],&lpObj->pRestoreInventory[lpMsg->btIndexCode].restoreItem,sizeof(lpObj->Inventory[(unsigned __int8)blank]));
							lpObj->pRestoreInventory[lpMsg->btIndexCode].Clear();
							gItemManager.InventoryItemSet(lpObj->Index, blank, lpObj->Inventory[(unsigned __int8)blank].m_Index);

							lpObj->Money = v98;

							gItemManager.GCItemListSend(lpObj->Index);
							this->CGReqRestoreItemList(0, aIndex, 1);

							GCMoneySend(aIndex, lpObj->Money);
							pMsg.result = 0;
							DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
						}
					}
					else
					{
						BYTE invenChkResult = -1;
						invenChkResult = gMuunSystem.CheckMuunInventoryEmpty(lpObj);
						if ((unsigned __int8)invenChkResult == 255)
						{
							pMsg.result = 2;
							//LogAdd(LOG_BLACK,"[RestoreItem][%s][%s] Not enough Muun inventory storage space", lpObj->Account, lpObj->Name);
							DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
						}
						else
						{
							//if (CMuunSystem::IsMuunExpireDate(&g_CMuunSystem, tmpItem.exp) == 1)
							//	CItem::SetMuunItmePeriodExpire(tmpItem);
							int ret = gMuunSystem.CheckMuunInventoryEmpty(lpObj);
							if ((unsigned __int8)ret == 255)
							{
								gMuunSystem.GCMuunItemListSend(lpObj->Index);

								pMsg.result = 2;
								DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
								return;
							}

							int userZen = lpObj->Money - lpObj->pRestoreInventory[lpMsg->btIndexCode].reqMoney;
							if (userZen < 0)
								userZen = 0;

							lpObj->pRestoreInventory[lpMsg->btIndexCode].Clear();
							memcpy(
								&lpObj->MuunInventory[(unsigned __int8)ret],
								&tmpItem,
								sizeof(lpObj->MuunInventory[(unsigned __int8)ret]));
							lpObj->Money = userZen;

							
							//if (tmpRestoreItem.IsMuunItem() == 1)
							//{
							//	
							//	wPetItemNumber = _byteswap_ushort(*(unsigned __int16 *)&tmpRestoreItem.m_SocketOption[0]);
							//	pItemAttribute =  GetItemAttr(wPetItemNumber);
							//	if (!pItemAttribute)
							//	{
							//		//LogAdd(
							//		//	"[MuunSystem][Error] pItemAttribute is NULL : %s %d",
							//		//	"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\restoreitem.cpp",
							//		//	`CRestoreSoldItem::CGReqRestoreItem'::`2'::__LINE__Var + 335);
							//		return;
							//	}

							//}
							gMuunSystem.GCMuunItemListSend(lpObj->Index);

							this->CGReqRestoreItemList(0, aIndex, 1);
							GCMoneySend(aIndex, lpObj->Money);
							pMsg.result = 0;
							DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
						}
					}
				}
				else
				{

					pMsg.result = 3;
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);
					if (lpObj->pRestoreInventory[lpMsg->btIndexCode].restoreItem.IsPentagramItem() == 1)
						gPentagramSystem.DelAllPentagramJewelInfo(lpObj, &lpObj->pRestoreInventory[lpMsg->btIndexCode].restoreItem, PENTAGRAM_JEWEL_TYPE_INVENTORY);

					lpObj->pRestoreInventory[lpMsg->btIndexCode].Clear();
				}
			}
		}
	
}

void  CRestoreSoldItem::GDReqRestoreItemListLoad(LPOBJ lpObj)
{
	_SDHP_REQ_RESTORE_ITEM_LIST pMsg;

	if (this->GetRestoreItemUseFlag())
	{
		if (gObjIsConnected(lpObj->Index))
		{
			memcpy(pMsg.AccountID, lpObj->Account, 0xBu);
			memcpy(pMsg.Name, lpObj->Name, 0xBu);
			pMsg.userIndex = lpObj->Index;
			pMsg.h.set(0x6F, 0, sizeof(pMsg));
			gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
		}
		/*else
		{
			LogAdd(LOG_BLACK,"[RestoreItem] Restore item list load fail. Not Connected user (%d)(Connected:%d)",lpObj->Index,lpObj->Connected);
		}*/
	}
	//else
	//{
	//	LogAdd(LOG_BLACK,"[RestoreItem] Restore item list load fail.  Restore item feature flag is OFF (%d)",this->m_UseRestoreItemOnOff);
	//}
}

void  CRestoreSoldItem::DGAnsRestoreItemListInfo(_SDHP_ANS_RESTORE_ITEM_LIST *lpMsg)
{
	if (this->GetRestoreItemUseFlag())
	{
		LPOBJ lpObj = &gObj[lpMsg->userIndex];

		if (gObjIsConnected(lpObj->Index) == 0)
		{
			return;
		}

		/*if (lpObj->MapServerMoveRequest == 1)
		{
			LogAdd(LOG_BLACK,"[RestoreItem] DGAnsRestoreItemListInfo. MapServer move user. Can't open restore item list. [%s][%s] IP[%s]",lpObj->Account,lpObj->Name,lpObj->IpAddr);
		}
		else if (lpObj->State == OBJECT_DELCMD)
		{
			LogAdd(LOG_BLACK,"[RestoreItem] DGAnsRestoreItemListInfo. MapServer move user. Can't open restore item list. [%s][%s] IP[%s]",lpObj->Account,lpObj->Name,lpObj->IpAddr);
		}
		else if (lpObj->MapServerMoveQuit == 1)
		{
			LogAdd(LOG_BLACK,"[RestoreItem] DGAnsRestoreItemListInfo. MapServer move user. Can't open restore item list. [%s][%s] IP[%s]",lpObj->Account,lpObj->Name,lpObj->IpAddr);
		}
		else if (lpMsg->result)
		{
			LogAdd(LOG_BLACK,"[RestoreItem] DGAnsRestoreItemListInfo. Result code error [%s][%s] IP[%s]",lpObj->Account,lpObj->Name,lpObj->IpAddr);
		}
		else
		{*/
			int nRestoreItemCount = 0;
			int nRestoreItemExpireCnt = 0;
			int n = 0;

			CItem item;
			ATL::CTime result;
			result.GetTickCount();
			__int64 nowTime = result.GetTime();

			int dbSize = 7;
			if (lpMsg->dbVersion < 3)
			{
				if (lpMsg->dbVersion >= 1)
					dbSize = 10;
			}
			else
			{
				dbSize = 16;
			}
			for (int i = 0; i < 5; ++i)
			{

				item.Clear();
				lpObj->pRestoreInventory[i].Clear();
				BYTE restoreFlag = lpMsg->dbRestoreItemData[n++];
				BYTE expireFlag = lpMsg->dbRestoreItemData[n++];
				unsigned int reqMoney;
				memcpy((char *)&reqMoney, &lpMsg->dbRestoreItemData[n], 4u);
				n += 4;
				__int64 expireTimeSec;
				memcpy((char *)&expireTimeSec, &lpMsg->dbRestoreItemData[n], 8u);
				n += 8;
				int iType = lpMsg->dbRestoreItemData[n];
				int _type;
				if (dbSize == 16)
				{ 
					iType |= 32 * (unsigned __int8)(lpMsg->dbRestoreItemData[n + 9] & 0xF0);
					iType |= 2 * (unsigned __int8)(lpMsg->dbRestoreItemData[n + 7] & 0x80);
					iType |= (lpMsg->dbRestoreItemData[n + 9] & 1) << 13;
					int iItemType = ITEM_GET_TYPE(iType);
					int iItemIndex = ITEM_GET_INDEX(iType);
					if (iItemType == 13
						&& ( gMuunSystem.IsMuunItem(iType) == 1
							/*|| CMuunSystem::IsStoneofEvolution(&g_CMuunSystem, iType) == 1*/
							|| iType == GET_ITEM(13,239)
							|| iType == GET_ITEM(13, 240)))
					{
						iType = iItemIndex + 0x2000;
					}
					_type = iType;
				}
				else if (dbSize == 10)
				{
					if ((lpMsg->dbRestoreItemData[n + 7] >> 7) & 1)
						iType += 256;
					_type = iType % 32 + (iType / 32 << 9);
				}
				else
				{

					_type = iType % 16 + (iType / 16 << 9);
					iType = iType % 16 + (iType / 16 << 9);
				}
				if (dbSize == 16)
				{
					if (lpMsg->dbRestoreItemData[n] == 255
						&& (lpMsg->dbRestoreItemData[n + 9] & 0xF0) == 240
						&& lpMsg->dbRestoreItemData[n + 7] & 0x80
						&& lpMsg->dbRestoreItemData[n + 9] & 1)
					{
						iType = -1;
					}
				}
				else if (dbSize < 10)
				{
					if (lpMsg->dbRestoreItemData[n] == 255)
						iType = -1;
				}
				else if (lpMsg->dbRestoreItemData[n] == 255 && lpMsg->dbRestoreItemData[n + 7] & 0x80)
				{
					iType = -1;
				}
				if (!CHECK_RANGE(_type, MAX_ITEM))
					iType = -1;
				if ((_type == GET_ITEM(14, 17) || _type == GET_ITEM(14, 18)) && !lpMsg->dbRestoreItemData[n + 2])
					_type = -1;
				if (_type == GET_ITEM(14, 19) && lpMsg->dbRestoreItemData[n + 2] == 255)
					iType = -1;
				if (iType != -1)
				{
					item.m_Level = (lpMsg->dbRestoreItemData[n + 1] >> 3) & 0xF;
					if (dbSize == 7 && item.m_Level > 11)
						item.m_Level = 0;
					if (lpMsg->dbVersion < 2)
					{
						lpMsg->dbRestoreItemData[n + 8] = -1;
						lpMsg->dbRestoreItemData[n + 9] = -1;
					}
					if (_type != GET_ITEM(14, 11) && item.m_Level > 15)
						item.m_Level = 0;
					BYTE OptionData = lpMsg->dbRestoreItemData[n + 1];
					item.m_Option1 = (signed int)(unsigned __int8)OptionData >> 7;
					item.m_Option2 = ((signed int)(unsigned __int8)OptionData >> 2) & 1;
					item.m_Option3 = OptionData & 3;
					if (dbSize >= 10)
					{
						if (_type == GET_ITEM(13, 3))
						{
							item.m_Option3 |= (lpMsg->dbRestoreItemData[n + 7] & 0x40) >> 4;
						}
						else if (lpMsg->dbRestoreItemData[n + 7] & 0x40)
						{
							item.m_Option3 = 4;
						}
					}
					item.m_Durability = (float)lpMsg->dbRestoreItemData[n + 2];
					if (dbSize >= 16)
						item.m_JewelOfHarmonyOption = lpMsg->dbRestoreItemData[n + 10];
					if (dbSize >= 16)
					{
						item.m_ItemOptionEx = 16 * (lpMsg->dbRestoreItemData[n + 9] & 8);
						if (item.m_ItemOptionEx)
						{
							item.m_Index = iType;

							if (!item.Is380Item())
							{
								item.m_ItemOptionEx = 0;
								//LogAdd(LOG_BLACK,"[380Item][%s][%s] Invalid 380 Item Option in RestoreInventory pos[%d]",lpObj->Account,lpObj->Name,n);
							}
						}
					}
					BYTE SocketBonusOption = -1;
					int iSocketSlotIndex = 0;
					BYTE SocketOption[5] = { -1,-1,-1,-1,-1 };

					if (item.IsSocketItem() == 1)
					{
						SocketBonusOption = lpMsg->dbRestoreItemData[n + 10];
						for (iSocketSlotIndex = 0; iSocketSlotIndex < 5; ++iSocketSlotIndex)
							SocketOption[iSocketSlotIndex] = *(&lpMsg->dbRestoreItemData[n + 11] + iSocketSlotIndex);
					}
					else if (item.IsPentagramItem() != 1&& item.IsPentagramJewel() != 1)
					{
						SocketBonusOption = 0;
						for (iSocketSlotIndex = 0; iSocketSlotIndex < 5; ++iSocketSlotIndex)
						{
							SocketOption[iSocketSlotIndex] = *(&lpMsg->dbRestoreItemData[n + 11] + iSocketSlotIndex);
							if (!SocketOption[iSocketSlotIndex])
								SocketOption[iSocketSlotIndex] = -1;
						}
					}
					else
					{
						SocketBonusOption = lpMsg->dbRestoreItemData[n + 10];
						for (iSocketSlotIndex = 0; iSocketSlotIndex < 5; ++iSocketSlotIndex)
							SocketOption[iSocketSlotIndex] = *(&lpMsg->dbRestoreItemData[n + 11] + iSocketSlotIndex);
					}

					if (dbSize >= 16)
						item.m_IsPeriodicItem = (lpMsg->dbRestoreItemData[n + 9] & 6) >> 1;

					if (lpMsg->dbVersion)
					{
						if (lpMsg->dbVersion > 2)
							item.Convert(iType,item.m_Option1,item.m_Option2,item.m_Option3,lpMsg->dbRestoreItemData[n + 7] & 0x3F,lpMsg->dbRestoreItemData[n + 8],item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,SocketOption,SocketBonusOption);
						else
							item.Convert(iType,item.m_Option1,item.m_Option2,item.m_Option3,lpMsg->dbRestoreItemData[n + 7] & 0x3F,lpMsg->dbRestoreItemData[n + 8],item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,0,-1);
					}
					else
					{
						item.Convert(iType,item.m_Option1,item.m_Option2,item.m_Option3,0,0,0,0,0,-1);
					}
					if (_type != GET_ITEM(14, 7))
					{
						if ((_type < GET_ITEM(14, 0) || _type > GET_ITEM(14, 8)) && (_type < GET_ITEM(14, 35) || _type > GET_ITEM(14, 40)))
						{
							switch (_type)
							{
							case GET_ITEM(14, 215):
								if (item.m_Durability == 0.0 || item.m_Durability == 255.0)
									item.m_Durability = 1.0;
								if (item.m_Durability >= 24.0)
									item.m_Durability = 1.0;
								break;
							case GET_ITEM(14, 277):
								if (item.m_Durability == 0.0 || item.m_Durability == 255.0)
									item.m_Durability = 1.0;
								if (item.m_Durability > 100.0)
									item.m_Durability = 1.0;
								break;
							case GET_ITEM(13, 145):
								if (item.m_Durability == 0.0 || item.m_Durability == 255.0)
									item.m_Durability = 1.0;
								if (item.m_Durability >= 5.0)
									item.m_Durability = 1.0;
								break;
							case GET_ITEM(12, 401):
								if (item.m_Durability == 0.0 || item.m_Durability == 255.0)
									item.m_Durability = 1.0;
								if (item.m_Durability > 30.0)
									item.m_Durability = 1.0;
								break;
							default:
								if (_type != GET_ITEM(14, 378) && _type != GET_ITEM(14, 379) && _type != GET_ITEM(14, 380) && _type != GET_ITEM(14, 381) && _type != GET_ITEM(14, 382) && _type != GET_ITEM(14, 383) && _type != GET_ITEM(14, 384))
								{
									if (_type != GET_ITEM(12, 144) && _type != GET_ITEM(12, 146))
									{
										if (_type == GET_ITEM(14, 278))
										{
											if (item.m_Durability == 0.0 || item.m_Durability == 255.0)
												item.m_Durability = 1.0;
											if (item.m_Durability >= 25.0)
												item.m_Durability = 1.0;
										}
										else if (_type != GET_ITEM(13, 10) && _type != GET_ITEM(14, 11) && _type != GET_ITEM(14, 21) && item.m_Level == 3 && _type != GET_ITEM(14, 100) && _type != GET_ITEM(14, 101) && item.m_BaseDurability < (double)item.m_Durability)
										{
											item.m_Durability = item.m_BaseDurability;
										}
									}
									else
									{
										if (item.m_Durability == 0.0 || item.m_Durability == 255.0)
											item.m_Durability = 1.0;
										if (item.m_Durability >= 5.0)
											item.m_Durability = 1.0;
									}
								}
								else
								{
									if (item.m_Durability == 0.0 || item.m_Durability == 255.0)
										item.m_Durability = 1.0;
									if (item.m_Durability > 30.0)
										item.m_Durability = 1.0;
								}
								break;
							}
						}
						else
						{
							if (item.m_Durability == 0.0)
								item.m_Durability = 1.0;
						}
					}
					lpObj->pRestoreInventory[i].restoreItem.m_Option1 = item.m_Option1;
					lpObj->pRestoreInventory[i].restoreItem.m_Option2 = item.m_Option2;
					lpObj->pRestoreInventory[i].restoreItem.m_Option3 = item.m_Option3;
					lpObj->pRestoreInventory[i].restoreItem.m_JewelOfHarmonyOption = item.m_JewelOfHarmonyOption;
					lpObj->pRestoreInventory[i].restoreItem.m_ItemOptionEx = item.m_ItemOptionEx;
					WORD hiWord = _byteswap_ushort(*(WORD *)&lpMsg->dbRestoreItemData[n + 3]);
					WORD loWord = _byteswap_ushort(*(WORD *)&lpMsg->dbRestoreItemData[n + 5]);
					item.m_Serial = (hiWord << 16) | loWord;
					memcpy(&lpObj->pRestoreInventory[i].restoreItem,&item,sizeof(lpObj->pRestoreInventory[i].restoreItem));
					lpObj->pRestoreInventory[i].restoreFlag = restoreFlag;
					lpObj->pRestoreInventory[i].expireFlag = expireFlag;
					lpObj->pRestoreInventory[i].reqMoney = reqMoney;
					if (nowTime >= expireTimeSec)
					{
						lpObj->pRestoreInventory[i].expireFlag = 1;
					}

					lpObj->pRestoreInventory[i].expireTime = expireTimeSec;
					++nRestoreItemCount;
					n += 16;
				}
			}

			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] DB load count: %d", lpObj->Account, lpObj->Name, nRestoreItemCount);
			
			for (int j = 0; j < 5; ++j)
			{
				if (lpObj->pRestoreInventory[j].restoreItem.IsItem() != 1)
				{
					if (lpObj->pRestoreInventory[j].restoreFlag == 1 && lpObj->pRestoreInventory[j].expireFlag == 1)
					{
						++nRestoreItemExpireCnt;
					}
				}
			}
			//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] After count that delete expired item : (%d) - (%d) = (%d)",lpObj->Account,lpObj->Name,nRestoreItemCount,nRestoreItemExpireCnt,nRestoreItemCount - nRestoreItemExpireCnt);
			lpObj->bRestoreInventoryLoad = 1;
		}
	//}
	/*else
	{
		LogAdd(LOG_BLACK,"[RestoreItem] DGAnsRestoreItemListInfo.  Restore item feature flag is OFF (%d)",this->m_UseRestoreItemOnOff);
	}*/
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00CBB2C0) --------------------------------------------------------
char  CRestoreSoldItem::GDReqSaveRestoreItemListInven(LPOBJ lpObj, int restoreItemCnt)
{
	
	_SDHP_REQ_RESTORE_ITEM_LIST_SAVE pMsg; // [sp+70h] [bp-ECh]@25
	ATL::CTime result;

	if (lpObj)
	{
		
			if (!gObjIsConnected(lpObj->Index))
				//LogAdd(LOG_BLACK,"[RestoreItem] GDReqSaveRestoreItemListInven error. Not Connected user. (%d)(%d)(Connected: %d)",lpObj->Index,lpObj->Type,lpObj->Connected);

			if (restoreItemCnt == 5)
			{
				if (lpObj->bRestoreInventoryLoad)
				{
					if (this->IsActivateRestoreInventory(lpObj))
					{
						result = ATL::CTime::GetTickCount();
						
						__int64 nowTime = result.GetTime();
						int restoreRealItemCnt = 0;
						for (int i = 0; i < 5; ++i)
						{
							if (lpObj->pRestoreInventory[i].restoreItem.IsItem() == 1)
							{

								__int64 expireTimeSec = lpObj->pRestoreInventory[i].expireTime.GetTime();
								
								if (nowTime > expireTimeSec && lpObj->pRestoreInventory[i].restoreFlag == 1)
								{
									lpObj->pRestoreInventory[i].expireFlag = 1;
								}

								++restoreRealItemCnt;
							}
						}
						//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] Restore item list save complete. (ItemCnt: %d)",lpObj->Account,lpObj->Name,restoreRealItemCnt);
						memcpy(pMsg.AccountID, lpObj->Account, sizeof(pMsg.AccountID));
						memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
						pMsg.h.set(0x6f, 1, sizeof(pMsg));
						memset(pMsg.dbRestoreItemData, -1, sizeof(pMsg.dbRestoreItemData));
						int n = 0;
						__int64 saveExpireTime;
						for (int j = 0; j < 5; ++j)
						{
							if (lpObj->pRestoreInventory[j].restoreItem.IsItem())
							{
								pMsg.dbRestoreItemData[n++] = lpObj->pRestoreInventory[j].restoreFlag;
								pMsg.dbRestoreItemData[n++] = lpObj->pRestoreInventory[j].expireFlag;
								memcpy(&pMsg.dbRestoreItemData[n], (char *)&lpObj->pRestoreInventory[j].reqMoney, sizeof(lpObj->pRestoreInventory[j].reqMoney));
								n += 4;
								saveExpireTime = lpObj->pRestoreInventory[j].expireTime.GetTime();
								memcpy(&pMsg.dbRestoreItemData[n], (char *)&saveExpireTime, sizeof(saveExpireTime));
								n += 8;
								gItemManager.ItemByteConvert16(&pMsg.dbRestoreItemData[n], &lpObj->pRestoreInventory[j].restoreItem, 1);
								n += 16;
							}

						}
						gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));

						return 1;
					}
					else
					{
						//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] GDReqSaveRestoreItemListInven - Not save restoreItem info.",lpObj->Account,lpObj->Name);
						return 0;
					}
				}
				else
				{
					//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] GDReqSaveRestoreItemListInven - Restore inventory list is not loaded. (%d)",lpObj->Account,lpObj->Name,lpObj->bRestoreInventoryLoad);
					return 0;
				}
			}
			else
			{
				//LogAdd(LOG_BLACK,"[RestoreItem] [%s][%s] GDReqSaveRestoreItemListInven - Restore inventory list count is not valid. (%d)",lpObj->Account,lpObj->Name,restoreItemCnt);
				return 0;
			}

	}
	else
	{
		//LogAdd(LOG_BLACK,"[RestoreItem] GDReqSaveRestoreItemListInven error. user object is null.");
		return 0;
	}
	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
