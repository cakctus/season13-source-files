#include "stdafx.h"
#include "CashShop.h"
#include "BonusManager.h"
#include "CSProtocol.h"
#include "DSProtocol.h"
#include "EffectManager.h"
#include "GameMain.h"
#include "ItemManager.h"
#include "JSProtocol.h"
#include "Log.h"
#include "Map.h"
#include "MemScript.h"
#include "Message.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "SocketItemOption.h"
#include "Util.h"
#include "ItemStack.h"

CCashShop gCashShop;

CCashShop::CCashShop() // OK
{
	this->m_CashShopPackageCount = 0;
	this->m_CashShopProductCount = 0;
}

CCashShop::~CCashShop() // OK
{

}

void CCashShop::LoadPackage(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_CashShopPackageCount = 0;

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			CASH_SHOP_PACKAGE_INFO info;

			memset(&info,0,sizeof(info));

			info.Category = lpMemScript->GetNumber();
			info.BaseIndex = lpMemScript->GetAsNumber();
			info.MainIndex = lpMemScript->GetAsNumber();
			info.ItemIndex = lpMemScript->GetAsNumber();
			info.CoinIndex = lpMemScript->GetAsNumber();
			info.CoinValue = lpMemScript->GetAsNumber();
			info.BonusGP = lpMemScript->GetAsNumber();

			for(int n=0;n < MAX_CASH_SHOP_PACKAGE_PRODUCT;n++)
			{
				info.ProductBaseIndex[n] = lpMemScript->GetAsNumber();
			}

			for(int n=0;n < MAX_CASH_SHOP_PACKAGE_PRODUCT;n++)
			{
				info.ProductMainIndex[n] = lpMemScript->GetAsNumber();
			}

			this->SetPackageInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CCashShop::LoadProduct(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_CashShopProductCount = 0;

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			CASH_SHOP_PRODUCT_INFO info;

			memset(&info,0,sizeof(info));

			info.BaseIndex = lpMemScript->GetNumber();
			info.MainIndex = lpMemScript->GetAsNumber();
			info.CoinValue = lpMemScript->GetAsNumber();
			info.ItemIndex = lpMemScript->GetAsNumber();
			info.ItemLevel = lpMemScript->GetAsNumber();
			info.ItemOption1 = lpMemScript->GetAsNumber();
			info.ItemOption2 = lpMemScript->GetAsNumber();
			info.ItemOption3 = lpMemScript->GetAsNumber();
			info.ItemNewOption = lpMemScript->GetAsNumber();
			info.ItemSetOption = lpMemScript->GetAsNumber();
			info.ItemHarmonyOption = lpMemScript->GetAsNumber();
			info.ItemOptionEx = lpMemScript->GetAsNumber();
			info.ItemSocketOption1 = lpMemScript->GetAsNumber();
			info.ItemSocketOption2 = lpMemScript->GetAsNumber();
			info.ItemSocketOption3 = lpMemScript->GetAsNumber();
			info.ItemSocketOption4 = lpMemScript->GetAsNumber();
			info.ItemSocketOption5 = lpMemScript->GetAsNumber();
			info.ItemQuantity = lpMemScript->GetAsNumber();
			info.ItemDuration = lpMemScript->GetAsNumber();

			this->SetProductInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CCashShop::SetPackageInfo(CASH_SHOP_PACKAGE_INFO info) // OK
{
	if(this->m_CashShopPackageCount < 0 || this->m_CashShopPackageCount >= MAX_CASH_SHOP_ITEM)
	{
		return;
	}

	this->m_CashShopPackageInfo[this->m_CashShopPackageCount++] = info;
}

void CCashShop::SetProductInfo(CASH_SHOP_PRODUCT_INFO info) // OK
{
	if(this->m_CashShopProductCount < 0 || this->m_CashShopProductCount >= MAX_CASH_SHOP_ITEM)
	{
		return;
	}

	this->m_CashShopProductInfo[this->m_CashShopProductCount++] = info;
}

CASH_SHOP_PACKAGE_INFO* CCashShop::GetPackageInfo(int index) // OK
{
	for(int n=0;n < MAX_CASH_SHOP_ITEM;n++)
	{
		if(this->m_CashShopPackageInfo[n].MainIndex == index)
		{
			return &this->m_CashShopPackageInfo[n];
		}
	}

	return 0;
}

CASH_SHOP_PRODUCT_INFO* CCashShop::GetProductInfo(int index) // OK
{
	for(int n=0;n < MAX_CASH_SHOP_ITEM;n++)
	{
		if(this->m_CashShopProductInfo[n].MainIndex == index)
		{
			return &this->m_CashShopProductInfo[n];
		}
	}

	return 0;
}

CASH_SHOP_PRODUCT_INFO* CCashShop::GetProductInfoByBaseIndex(int index) // OK
{
	for(int n=0;n < MAX_CASH_SHOP_ITEM;n++)
	{
		if(this->m_CashShopProductInfo[n].BaseIndex == index)
		{
			return &this->m_CashShopProductInfo[n];
		}
	}

	return 0;
}

void CCashShop::MainProc() // OK
{
	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) == 0 && gObj[n].AttackCustomOffline == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[n];

		/*if ((GetTickCount() - lpObj->CashShopGoblinPointTime) >= ((DWORD)gServerInfo.m_CashShopGoblinPointDelay * 60000))
		{
			lpObj->CashShopGoblinPointTime = GetTickCount();
			this->GDCashShopAddPointSaveSend(lpObj->Index,0,0,0,gBonusManager.GetBonusValue(lpObj,BONUS_INDEX_GLOBIN_POINT,gServerInfo.m_CashShopGoblinPointValue[lpObj->AccountLevel],-1,-1,-1,-1), "TimeBonus");
			this->CGCashShopPointRecv(lpObj->Index);
		}*/

		for(int i=0;i < INVENTORY_SIZE;i++)
		{
			if(lpObj->Inventory[i].IsItem() == 0)
			{
				continue;
			}

			if(lpObj->Inventory[i].m_IsPeriodicItem == 0)
			{
				continue;
			}

			if(lpObj->Inventory[i].m_LoadPeriodicItem == 0)
			{
				continue;
			}

			if(lpObj->Inventory[i].m_PeriodicItemTime <= ((int)time(0)))
			{
				gItemManager.InventoryDelItem(lpObj->Index,i);
				gItemManager.GCItemDeleteSend(lpObj->Index,i,1);
				gItemManager.UpdateInventoryViewport(lpObj->Index,i);
			}
		}
	}
}

int CCashShop::GetPackageProductBaseIndexCount(CASH_SHOP_PACKAGE_INFO* lpPackageInfo) // OK
{
	int count = 0;

	for(int n=0;n < MAX_CASH_SHOP_PACKAGE_PRODUCT;n++)
	{
		if(lpPackageInfo->ProductBaseIndex[n] != 0)
		{
			count++;
		}
	}

	return count;
}

int CCashShop::GetPackageProductMainIndexCount(CASH_SHOP_PACKAGE_INFO* lpPackageInfo) // OK
{
	int count = 0;

	for(int n=0;n < MAX_CASH_SHOP_PACKAGE_PRODUCT;n++)
	{
		if(lpPackageInfo->ProductMainIndex[n] != 0)
		{
			count++;
		}
	}

	return count;
}

void CCashShop::CGCashShopPointRecv(int aIndex) // OK
{
	if(gServerInfo.m_CashShopSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(((lpObj->CashShopTransaction[0]==0)?(lpObj->CashShopTransaction[0]++):lpObj->CashShopTransaction[0]) != 0)
	{
		return;
	}

	SDHP_CASH_SHOP_POINT_SEND pMsg;

	pMsg.header.set(0x18,0x00,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::CGCashShopOpenRecv(PMSG_CASH_SHOP_OPEN_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo.m_CashShopSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_CASH_SHOP_OPEN_SEND pMsg;

	pMsg.header.set(0xD2,0x02,sizeof(pMsg));

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		pMsg.result = 0;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) == 0)
	{
		GCMessagePopupSend(lpObj, "Vá para uma área segura antes de abrir o Game Shop.");
		pMsg.result = 0;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if (lpObj->PersonalCodeChecked == false)
	{
		GCMessagePopupSend(lpObj, "Você não pode abrir o Game Shop sem antes autenticar o PIN. Utilize o comando /pin seguido de seu PIN para faze-lo agora mesmo.");
		pMsg.result = 0;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if(lpMsg->OpenType == 0)
	{
		if(lpObj->Interface.use == 0)
		{
			lpObj->Interface.use = 1;
			lpObj->Interface.type = INTERFACE_CASH_SHOP;
			lpObj->Interface.state = 0;
			pMsg.result = 1;
			DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		}
		else
		{
			pMsg.result = 8;
			DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
	else
	{
		lpObj->Interface.use = 0;
		lpObj->Interface.type = INTERFACE_NONE;
		lpObj->Interface.state = 0;
		pMsg.result = 0;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void CCashShop::CGCashShopItemBuyRecv(PMSG_CASH_SHOP_ITEM_BUY_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo.m_CashShopSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(((lpObj->CashShopTransaction[1]==0)?(lpObj->CashShopTransaction[1]++):lpObj->CashShopTransaction[1]) != 0)
	{
		return;
	}

	SDHP_CASH_SHOP_ITEM_BUY_SEND pMsg;

	pMsg.header.set(0x18,0x01,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.PackageMainIndex = lpMsg->PackageMainIndex;

	pMsg.Category = lpMsg->Category;

	pMsg.ProductMainIndex = lpMsg->ProductMainIndex;

	pMsg.ItemIndex = lpMsg->ItemIndex;

	pMsg.CoinIndex = lpMsg->CoinIndex;

	pMsg.MileageFlag = lpMsg->MileageFlag;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::CGCashShopItemGifRecv(PMSG_CASH_SHOP_ITEM_GIF_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo.m_CashShopSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(((lpObj->CashShopTransaction[1]==0)?(lpObj->CashShopTransaction[1]++):lpObj->CashShopTransaction[1]) != 0)
	{
		return;
	}

	SDHP_CASH_SHOP_ITEM_GIF_SEND pMsg;

	pMsg.header.set(0x18,0x02,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.PackageMainIndex = lpMsg->PackageMainIndex;

	pMsg.Category = lpMsg->Category;

	pMsg.ProductMainIndex = lpMsg->ProductMainIndex;

	pMsg.SaleZone = lpMsg->SaleZone;

	memcpy(pMsg.GiftName,lpMsg->GiftName,sizeof(pMsg.GiftName));

	memcpy(pMsg.GiftText,lpMsg->GiftText,sizeof(pMsg.GiftText));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CCashShop::CGCashShopItemNumRecv(PMSG_CASH_SHOP_ITEM_NUM_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo.m_CashShopSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(((lpObj->CashShopTransaction[2]==0)?(lpObj->CashShopTransaction[2]++):lpObj->CashShopTransaction[2]) != 0)
	{
		return;
	}

	SDHP_CASH_SHOP_ITEM_NUM_SEND pMsg;

	pMsg.header.set(0x18,0x03,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.InventoryPage = lpMsg->InventoryPage;
	pMsg.InventoryType = lpMsg->InventoryType;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::CGCashShopItemUseRecvSXI(PMSG_CASH_SHOP_ITEM_USE_RECV_SXI* lpMsg,int aIndex) // OK
{
	if(gServerInfo.m_CashShopSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_CASH_SHOP_ITEM_USE_SEND_SXI pMsg;
	pMsg.header.set(0xD2,0x0F,sizeof(pMsg));
	pMsg.item = lpMsg->ItemIndex;
	pMsg.unk = 0;
	DataSend(lpObj->Index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CCashShop::CGCashShopItemUseRecv(PMSG_CASH_SHOP_ITEM_USE_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo.m_CashShopSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(((lpObj->CashShopTransaction[3]==0)?(lpObj->CashShopTransaction[3]++):lpObj->CashShopTransaction[3]) != 0)
	{
		return;
	}

	SDHP_CASH_SHOP_ITEM_USE_SEND pMsg;

	pMsg.header.set(0x18,0x04,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.BaseItemCode = lpMsg->BaseItemCode;
	pMsg.MainItemCode = lpMsg->MainItemCode;
	pMsg.ItemIndex = lpMsg->ItemIndex;
	pMsg.ProductType = lpMsg->ProductType;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::ReloadVersionToAll()
{
	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		LPOBJ lpObj = &gObj[i];

		if (lpObj->Connected < OBJECT_ONLINE)
			continue;

		gCashShop.GCCashShopInitSend(lpObj);
	}
}

void CCashShop::GCCashShopInitSend(LPOBJ lpObj) // OK
{
	if(gServerInfo.m_CashShopSwitch == 0)
	{
		return;
	}

	PSBMSG_HEAD pMsg;

	pMsg.set(0xD2,0x00,sizeof(pMsg));

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.size);

	this->GCCashShopScriptVersionSend(lpObj);
	this->GCCashShopBannerVersionSend(lpObj);
}

void CCashShop::GCCashShopScriptVersionSend(LPOBJ lpObj) // OK
{
	PMSG_CASH_SHOP_VERSION_SEND pMsg;

	pMsg.header.set(0xD2,0x0C,sizeof(pMsg));

	pMsg.version[0] = gServerInfo.m_CashShopScriptVersion1;
	pMsg.version[1] = gServerInfo.m_CashShopScriptVersion2;
	pMsg.version[2] = gServerInfo.m_CashShopScriptVersion3;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::GCCashShopBannerVersionSend(LPOBJ lpObj) // OK
{
	PMSG_CASH_SHOP_VERSION_SEND pMsg;

	pMsg.header.set(0xD2,0x15,sizeof(pMsg));

	pMsg.version[0] = gServerInfo.m_CashShopBannerVersion1;
	pMsg.version[1] = gServerInfo.m_CashShopBannerVersion2;
	pMsg.version[2] = gServerInfo.m_CashShopBannerVersion3;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::GCCashShopItemSend(LPOBJ lpObj,int PageCount,CASH_SHOP_INVENTORY* CashInventory) // OK
{
	PMSG_CASH_SHOP_ITEM_SEND pMsg;

	pMsg.header.set(0xD2,0x0D,sizeof(pMsg));

	for(int n=0;n < PageCount;n++)
	{
		pMsg.BaseItemCode = CashInventory[n].BaseItemCode;

		pMsg.MainItemCode = CashInventory[n].MainItemCode;

		pMsg.PackageMainIndex = CashInventory[n].PackageMainIndex;

		pMsg.ProductBaseIndex = CashInventory[n].ProductBaseIndex;

		pMsg.ProductMainIndex = CashInventory[n].ProductMainIndex;

		pMsg.CoinValue = CashInventory[n].CoinValue;

		pMsg.ProductType = CashInventory[n].ProductType;

		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void CCashShop::GCCashShopGiftSend(LPOBJ lpObj,int PageCount,CASH_SHOP_INVENTORY* CashInventory) // OK
{
	PMSG_CASH_SHOP_GIFT_SEND pMsg;

	pMsg.header.set(0xD2,0x0E,sizeof(pMsg));

	for(int n=0;n < PageCount;n++)
	{
		pMsg.BaseItemCode = CashInventory[n].BaseItemCode;

		pMsg.MainItemCode = CashInventory[n].MainItemCode;

		pMsg.PackageMainIndex = CashInventory[n].PackageMainIndex;

		pMsg.ProductBaseIndex = CashInventory[n].ProductBaseIndex;

		pMsg.ProductMainIndex = CashInventory[n].ProductMainIndex;

		pMsg.CoinValue = CashInventory[n].CoinValue;

		pMsg.ProductType = CashInventory[n].ProductType;

		memcpy(pMsg.GiftName,CashInventory[n].GiftName,sizeof(pMsg.GiftName));

		memcpy(pMsg.GiftText,CashInventory[n].GiftText,sizeof(pMsg.GiftText));

		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void CCashShop::GCCashShopPeriodicItemSend(int aIndex,int index,int slot,int time) // OK
{
	PMSG_CASH_SHOP_PERIODIC_ITEM_SEND pMsg;

	pMsg.header.set(0xD2,0x12,sizeof(pMsg));

	pMsg.index = index;

	pMsg.slot = slot;

	pMsg.time = time;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::DGCashShopPointRecv(SDHP_CASH_SHOP_POINT_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCashShopPointRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CashShopTransaction[0] = 0;

	PMSG_CASH_SHOP_POINT_SEND pMsg;

	pMsg.header.set(0xD2,0x01,sizeof(pMsg));

	pMsg.result = 0;

	if (lpMsg->WCoinC < 0 || lpMsg->WCoinC > 50000)
	{
		lpMsg->WCoinC = 0;
	}

	pMsg.WCoinC[0] = lpMsg->WCoinC;
	pMsg.WCoinC[1] = lpMsg->WCoinC;
	pMsg.WCoinP[0] = lpMsg->WCoinP;
	pMsg.WCoinP[1] = lpMsg->WCoinP;
	pMsg.GoblinPoint = lpMsg->GoblinPoint;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::DGCashShopItemBuyRecv(SDHP_CASH_SHOP_ITEM_BUY_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCashShopItemBuyRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CashShopTransaction[1] = 0;

	PMSG_CASH_SHOP_ITEM_BUY_SEND pMsg;

	pMsg.header.set(0xD2,0x03,sizeof(pMsg));

	CASH_SHOP_PACKAGE_INFO* lpPackageInfo = this->GetPackageInfo(lpMsg->PackageMainIndex);

	if(lpPackageInfo == 0)
	{
		pMsg.result = 3;
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->result != 0)
	{
		pMsg.result = 3;
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->ProductMainIndex == 0)
	{
		DWORD WCoinC = ((lpPackageInfo->CoinIndex==508)?lpPackageInfo->CoinValue:0);

		DWORD WCoinP = ((lpPackageInfo->CoinIndex==509)?lpPackageInfo->CoinValue:0);

		DWORD GoblinPoint = ((lpPackageInfo->CoinIndex==0)?lpPackageInfo->CoinValue:0);

		if(WCoinC > lpMsg->WCoinC)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if(WCoinP > lpMsg->WCoinP)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if(GoblinPoint > lpMsg->GoblinPoint)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if((lpMsg->ItemCount+this->GetPackageProductBaseIndexCount(lpPackageInfo)) > (MAX_CASH_SHOP_PAGE*MAX_CASH_SHOP_PAGE_ITEM))
		{
			pMsg.result = 2;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		gLog.Output(LOG_CASH_SHOP,"[CashShopItemBuy][PackageInfo][%s][%s] - (Category: %d, BaseIndex: %d, MainIndex: %d, ItemIndex: %d, CoinIndex: %d, CoinValue: %d, BonusGP: %d)",lpObj->Account,lpObj->Name,lpPackageInfo->Category,lpPackageInfo->BaseIndex,lpPackageInfo->MainIndex,lpPackageInfo->ItemIndex,lpPackageInfo->CoinIndex,lpPackageInfo->CoinValue,lpPackageInfo->BonusGP);

		for(int n=0;n < MAX_CASH_SHOP_PACKAGE_PRODUCT;n++)
		{
			if(lpPackageInfo->ProductBaseIndex[n] != 0)
			{
				CASH_SHOP_PRODUCT_INFO* lpProductInfo = ((lpPackageInfo->ProductMainIndex[n]==0)?this->GetProductInfoByBaseIndex(lpPackageInfo->ProductBaseIndex[n]):this->GetProductInfo(lpPackageInfo->ProductMainIndex[n]));
				this->GDCashShopInsertItemSaveSend(lpObj->Index,0,83,lpPackageInfo->MainIndex,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,80,0,0);
				gLog.Output(LOG_CASH_SHOP,"[CashShopItemBuy][ProductInfo][%s][%s] - (BaseIndex: %d, MainIndex: %d, CoinValue: %d, ItemIndex: %d, ItemLevel: %d, ItemOption1: %d, ItemOption2: %d, ItemOption3: %d, ItemNewOption: %d, ItemSetOption: %d, ItemHarmonyOption: %d, ItemOptionEx: %d, ItemSocketOption: %d, %d, %d, %d, %d, ItemQuantity: %d, ItemDuration: %d)",lpObj->Account,lpObj->Name,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,lpProductInfo->ItemIndex,lpProductInfo->ItemLevel,lpProductInfo->ItemOption1,lpProductInfo->ItemOption2,lpProductInfo->ItemOption3,lpProductInfo->ItemNewOption,lpProductInfo->ItemSetOption,lpProductInfo->ItemHarmonyOption,lpProductInfo->ItemOptionEx,lpProductInfo->ItemSocketOption1,lpProductInfo->ItemSocketOption2,lpProductInfo->ItemSocketOption3,lpProductInfo->ItemSocketOption4,lpProductInfo->ItemSocketOption5,lpProductInfo->ItemQuantity,lpProductInfo->ItemDuration);
			}
		}

		pMsg.result = 0;

		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

		this->GDCashShopSubPointSaveSend(lpObj->Index,0,WCoinC,WCoinP,GoblinPoint,"BuyingItem");

		if(lpPackageInfo->BonusGP > 0)
		{
			this->GDCashShopAddPointSaveSend(lpObj->Index,0,0,0,lpPackageInfo->BonusGP, "BuyingBonus");
		}
	}
	else
	{
		CASH_SHOP_PRODUCT_INFO* lpProductInfo = this->GetProductInfo(lpMsg->ProductMainIndex);

		if(lpProductInfo == 0)
		{
			return;
		}

		DWORD WCoinC = ((lpPackageInfo->CoinIndex==508)?lpProductInfo->CoinValue:0);
		DWORD WCoinP = ((lpPackageInfo->CoinIndex==509)?lpProductInfo->CoinValue:0);
		DWORD GoblinPoint = ((lpPackageInfo->CoinIndex==0)?lpProductInfo->CoinValue:0);

		if(WCoinC > lpMsg->WCoinC)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if(WCoinP > lpMsg->WCoinP)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if(GoblinPoint > lpMsg->GoblinPoint)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if((lpMsg->ItemCount+1) > (MAX_CASH_SHOP_PAGE*MAX_CASH_SHOP_PAGE_ITEM))
		{
			pMsg.result = 2;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		gLog.Output(LOG_CASH_SHOP,"[CashShopItemBuy][PackageInfo][%s][%s] - (Category: %d, BaseIndex: %d, MainIndex: %d, ItemIndex: %d, CoinIndex: %d, CoinValue: %d, BonusGP: %d)",lpObj->Account,lpObj->Name,lpPackageInfo->Category,lpPackageInfo->BaseIndex,lpPackageInfo->MainIndex,lpPackageInfo->ItemIndex,lpPackageInfo->CoinIndex,lpPackageInfo->CoinValue,lpPackageInfo->BonusGP);

		this->GDCashShopInsertItemSaveSend(lpObj->Index,0,83,lpPackageInfo->MainIndex,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,80,0,0);

		gLog.Output(LOG_CASH_SHOP,"[CashShopItemBuy][ProductInfo][%s][%s] - (BaseIndex: %d, MainIndex: %d, CoinValue: %d, ItemIndex: %d, ItemLevel: %d, ItemOption1: %d, ItemOption2: %d, ItemOption3: %d, ItemNewOption: %d, ItemSetOption: %d, ItemHarmonyOption: %d, ItemOptionEx: %d, ItemSocketOption: %d, %d, %d, %d, %d, ItemQuantity: %d, ItemDuration: %d)",lpObj->Account,lpObj->Name,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,lpProductInfo->ItemIndex,lpProductInfo->ItemLevel,lpProductInfo->ItemOption1,lpProductInfo->ItemOption2,lpProductInfo->ItemOption3,lpProductInfo->ItemNewOption,lpProductInfo->ItemSetOption,lpProductInfo->ItemHarmonyOption,lpProductInfo->ItemOptionEx,lpProductInfo->ItemSocketOption1,lpProductInfo->ItemSocketOption2,lpProductInfo->ItemSocketOption3,lpProductInfo->ItemSocketOption4,lpProductInfo->ItemSocketOption5,lpProductInfo->ItemQuantity,lpProductInfo->ItemDuration);

		pMsg.result = 0;

		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

		this->GDCashShopSubPointSaveSend(lpObj->Index,0,WCoinC,WCoinP,GoblinPoint, "BuyingItem");

		if(lpPackageInfo->BonusGP > 0)
		{
			this->GDCashShopAddPointSaveSend(lpObj->Index,0,0,0,lpPackageInfo->BonusGP, "BuyingBonus");
		}
	}
}

void CCashShop::DGCashShopItemGifRecv(SDHP_CASH_SHOP_ITEM_GIF_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCashShopItemGifRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CashShopTransaction[1] = 0;

	PMSG_CASH_SHOP_ITEM_GIF_SEND pMsg;

	pMsg.header.set(0xD2,0x04,sizeof(pMsg));

	CASH_SHOP_PACKAGE_INFO* lpPackageInfo = this->GetPackageInfo(lpMsg->PackageMainIndex);

	if(lpPackageInfo == 0)
	{
		pMsg.result = 3;
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->result != 0)
	{
		pMsg.result = 3;
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(lpMsg->ProductMainIndex == 0)
	{
		DWORD WCoinC = ((lpPackageInfo->CoinIndex==508)?lpPackageInfo->CoinValue:0);

		DWORD WCoinP = ((lpPackageInfo->CoinIndex==509)?lpPackageInfo->CoinValue:0);

		DWORD GoblinPoint = ((lpPackageInfo->CoinIndex==0)?lpPackageInfo->CoinValue:0);

		if(WCoinC > lpMsg->WCoinC)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if(WCoinP > lpMsg->WCoinP)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if(GoblinPoint > lpMsg->GoblinPoint)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if((lpMsg->ItemCount+this->GetPackageProductBaseIndexCount(lpPackageInfo)) > (MAX_CASH_SHOP_PAGE*MAX_CASH_SHOP_PAGE_ITEM))
		{
			pMsg.result = 2;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		gLog.Output(LOG_CASH_SHOP,"[CashShopItemGif][PackageInfo][%s][%s] - (GiftAccount: %s, GiftName: %s, Category: %d, BaseIndex: %d, MainIndex: %d, ItemIndex: %d, CoinIndex: %d, CoinValue: %d, BonusGP: %d)",lpObj->Account,lpObj->Name,lpMsg->GiftAccount,lpMsg->GiftName,lpPackageInfo->Category,lpPackageInfo->BaseIndex,lpPackageInfo->MainIndex,lpPackageInfo->ItemIndex,lpPackageInfo->CoinIndex,lpPackageInfo->CoinValue,lpPackageInfo->BonusGP);

		for(int n=0;n < MAX_CASH_SHOP_PACKAGE_PRODUCT;n++)
		{
			if(lpPackageInfo->ProductBaseIndex[n] != 0)
			{
				CASH_SHOP_PRODUCT_INFO* lpProductInfo = ((lpPackageInfo->ProductMainIndex[n]==0)?this->GetProductInfoByBaseIndex(lpPackageInfo->ProductBaseIndex[n]):this->GetProductInfo(lpPackageInfo->ProductMainIndex[n]));
				this->GDCashShopInsertItemSaveSend(lpObj->Index,lpMsg->GiftAccount,71,lpPackageInfo->MainIndex,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,80,lpObj->Name,lpMsg->GiftText);
				gLog.Output(LOG_CASH_SHOP,"[CashShopItemGif][ProductInfo][%s][%s] - (BaseIndex: %d, MainIndex: %d, CoinValue: %d, ItemIndex: %d, ItemLevel: %d, ItemOption1: %d, ItemOption2: %d, ItemOption3: %d, ItemNewOption: %d, ItemSetOption: %d, ItemHarmonyOption: %d, ItemOptionEx: %d, ItemSocketOption: %d, %d, %d, %d, %d, ItemQuantity: %d, ItemDuration: %d)",lpObj->Account,lpObj->Name,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,lpProductInfo->ItemIndex,lpProductInfo->ItemLevel,lpProductInfo->ItemOption1,lpProductInfo->ItemOption2,lpProductInfo->ItemOption3,lpProductInfo->ItemNewOption,lpProductInfo->ItemSetOption,lpProductInfo->ItemHarmonyOption,lpProductInfo->ItemOptionEx,lpProductInfo->ItemSocketOption1,lpProductInfo->ItemSocketOption2,lpProductInfo->ItemSocketOption3,lpProductInfo->ItemSocketOption4,lpProductInfo->ItemSocketOption5,lpProductInfo->ItemQuantity,lpProductInfo->ItemDuration);
			}
		}

		pMsg.result = 0;

		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

		this->GDCashShopSubPointSaveSend(lpObj->Index,0,WCoinC,WCoinP,GoblinPoint,"GiftBuy");

		if(lpPackageInfo->BonusGP > 0)
		{
			this->GDCashShopAddPointSaveSend(lpObj->Index,0,0,0,lpPackageInfo->BonusGP, "BuyingBonus");
		}

		GEMailMessageSend(lpObj->Name,lpMsg->GiftName,gMessage.GetMessage(276),2,ACTION_COLD1,lpObj->CharSet,lpMsg->GiftText);
	}
	else
	{
		CASH_SHOP_PRODUCT_INFO* lpProductInfo = this->GetProductInfo(lpMsg->ProductMainIndex);

		if(lpProductInfo == 0)
		{
			return;
		}

		DWORD WCoinC = ((lpPackageInfo->CoinIndex==508)?lpProductInfo->CoinValue:0);

		DWORD WCoinP = ((lpPackageInfo->CoinIndex==509)?lpProductInfo->CoinValue:0);

		DWORD GoblinPoint = ((lpPackageInfo->CoinIndex==0)?lpProductInfo->CoinValue:0);

		if(WCoinC > lpMsg->WCoinC)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if(WCoinP > lpMsg->WCoinP)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if(GoblinPoint > lpMsg->GoblinPoint)
		{
			pMsg.result = 1;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		if((lpMsg->ItemCount+1) > (MAX_CASH_SHOP_PAGE*MAX_CASH_SHOP_PAGE_ITEM))
		{
			pMsg.result = 2;
			DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
			return;
		}

		gLog.Output(LOG_CASH_SHOP,"[CashShopItemGif][PackageInfo][%s][%s] - (TargetAccount: %s, TargetName: %s, Category: %d, BaseIndex: %d, MainIndex: %d, ItemIndex: %d, CoinIndex: %d, CoinValue: %d, BonusGP: %d)",lpObj->Account,lpObj->Name,lpMsg->GiftAccount,lpMsg->GiftName,lpPackageInfo->Category,lpPackageInfo->BaseIndex,lpPackageInfo->MainIndex,lpPackageInfo->ItemIndex,lpPackageInfo->CoinIndex,lpPackageInfo->CoinValue,lpPackageInfo->BonusGP);

		this->GDCashShopInsertItemSaveSend(lpObj->Index,lpMsg->GiftAccount,71,lpPackageInfo->MainIndex,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,80,lpObj->Name,lpMsg->GiftText);

		gLog.Output(LOG_CASH_SHOP,"[CashShopItemGif][ProductInfo][%s][%s] - (BaseIndex: %d, MainIndex: %d, CoinValue: %d, ItemIndex: %d, ItemLevel: %d, ItemOption1: %d, ItemOption2: %d, ItemOption3: %d, ItemNewOption: %d, ItemSetOption: %d, ItemHarmonyOption: %d, ItemOptionEx: %d, ItemSocketOption: %d, %d, %d, %d, %d, ItemQuantity: %d, ItemDuration: %d)",lpObj->Account,lpObj->Name,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,lpProductInfo->ItemIndex,lpProductInfo->ItemLevel,lpProductInfo->ItemOption1,lpProductInfo->ItemOption2,lpProductInfo->ItemOption3,lpProductInfo->ItemNewOption,lpProductInfo->ItemSetOption,lpProductInfo->ItemHarmonyOption,lpProductInfo->ItemOptionEx,lpProductInfo->ItemSocketOption1,lpProductInfo->ItemSocketOption2,lpProductInfo->ItemSocketOption3,lpProductInfo->ItemSocketOption4,lpProductInfo->ItemSocketOption5,lpProductInfo->ItemQuantity,lpProductInfo->ItemDuration);

		pMsg.result = 0;

		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

		this->GDCashShopSubPointSaveSend(lpObj->Index,0,WCoinC,WCoinP,GoblinPoint,"GiftBuy");

		if(lpPackageInfo->BonusGP > 0)
		{
			this->GDCashShopAddPointSaveSend(lpObj->Index,0,0,0,lpPackageInfo->BonusGP,"BuyingBonus");
		}

		GEMailMessageSend(lpObj->Name,lpMsg->GiftName,gMessage.GetMessage(276),2,ACTION_COLD1,lpObj->CharSet,lpMsg->GiftText);
	}
}

void CCashShop::DGCashShopItemNumRecv(SDHP_CASH_SHOP_ITEM_NUM_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCashShopItemNumRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CashShopTransaction[2] = 0;

	if(lpMsg->result == 0 && lpMsg->InventoryType == 71)
	{
		DWORD CashShopInventoryCurPage = lpMsg->InventoryPage;

		DWORD CashShopInventoryMaxPage = ((lpMsg->ItemCount==0)?0:(((lpMsg->ItemCount-1)/MAX_CASH_SHOP_PAGE_ITEM)+1));

		CashShopInventoryCurPage = ((CashShopInventoryCurPage==0&&CashShopInventoryMaxPage>0)?1:CashShopInventoryCurPage);

		CashShopInventoryCurPage = ((CashShopInventoryCurPage>CashShopInventoryMaxPage)?CashShopInventoryMaxPage:CashShopInventoryCurPage);

		PMSG_CASH_SHOP_ITEM_NUM_SEND pMsg;

		pMsg.header.set(0xD2,0x06,sizeof(pMsg));

		pMsg.ItemCount = (WORD)lpMsg->ItemCount;

		pMsg.PageCount = (WORD)lpMsg->PageCount;

		pMsg.CurPage = (WORD)CashShopInventoryCurPage;

		pMsg.MaxPage = (WORD)CashShopInventoryMaxPage;

		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

		this->GCCashShopGiftSend(lpObj,lpMsg->PageCount,(CASH_SHOP_INVENTORY*)lpMsg->ProductInfo);
	}

	if(lpMsg->result == 0 && lpMsg->InventoryType == 83)
	{
		DWORD CashShopInventoryCurPage = lpMsg->InventoryPage;

		DWORD CashShopInventoryMaxPage = ((lpMsg->ItemCount==0)?0:(((lpMsg->ItemCount-1)/MAX_CASH_SHOP_PAGE_ITEM)+1));

		CashShopInventoryCurPage = ((CashShopInventoryCurPage==0&&CashShopInventoryMaxPage>0)?1:CashShopInventoryCurPage);

		CashShopInventoryCurPage = ((CashShopInventoryCurPage>CashShopInventoryMaxPage)?CashShopInventoryMaxPage:CashShopInventoryCurPage);

		PMSG_CASH_SHOP_ITEM_NUM_SEND pMsg;

		pMsg.header.set(0xD2,0x06,sizeof(pMsg));

		pMsg.ItemCount = (WORD)lpMsg->ItemCount;

		pMsg.PageCount = (WORD)lpMsg->PageCount;

		pMsg.CurPage = (WORD)CashShopInventoryCurPage;

		pMsg.MaxPage = (WORD)CashShopInventoryMaxPage;

		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

		this->GCCashShopItemSend(lpObj,lpMsg->PageCount,(CASH_SHOP_INVENTORY*)lpMsg->ProductInfo);
	}
}

void CCashShop::DGCashShopItemUseRecv(SDHP_CASH_SHOP_ITEM_USE_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCashShopItemUseRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CashShopTransaction[3] = 0;

	PMSG_CASH_SHOP_ITEM_USE_SEND pMsg;

	pMsg.header.set(0xD2,0x0B,sizeof(pMsg));

	if(lpMsg->result != 0)
	{
		pMsg.result = 1;
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	CASH_SHOP_PRODUCT_INFO* lpProductInfo = this->GetProductInfo(lpMsg->ProductInfo.ProductMainIndex);

	if(lpProductInfo == 0)
	{
		pMsg.result = 1;
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpProductInfo->ItemIndex);

	if(lpInfo == 0)
	{
		if(lpProductInfo->ItemDuration == 0)
		{
			if(lpProductInfo->ItemIndex == GET_ITEM(14,91) || lpProductInfo->ItemIndex == GET_ITEM(14,97) || lpProductInfo->ItemIndex == GET_ITEM(14,98) || lpProductInfo->ItemIndex == GET_ITEM(14,114) || lpProductInfo->ItemIndex == GET_ITEM(14,162) || lpProductInfo->ItemIndex == GET_ITEM(14,163) || lpProductInfo->ItemIndex == GET_ITEM(14,169)|| lpProductInfo->ItemIndex == GET_ITEM(14,281))
			{
				CItem item;

				item.m_Index = lpProductInfo->ItemIndex;

				switch(item.m_Index)
				{
					case GET_ITEM(14,91):
						if(gObjectManager.CharacterUseCreationCard(lpObj,&item) == 0)
						{
							pMsg.result = 1;
							DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
							return;
						}
						break;
					case GET_ITEM(14,97):
						if(gObjectManager.CharacterUseCreationCard(lpObj,&item) == 0)
						{
							pMsg.result = 1;
							DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
							return;
						}
						break;
					case GET_ITEM(14,98):
						if(gObjectManager.CharacterUseCreationCard(lpObj,&item) == 0)
						{
							pMsg.result = 1;
							DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
							return;
						}
						break;
					case GET_ITEM(14,114):
						if(gObjRebuildMasterSkillTree(lpObj) == 0)
						{
							pMsg.result = 1;
							DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
							return;
						}
						break;
					case GET_ITEM(14,162):
						if(gObjectManager.CharacterUseInventoryExpansion(lpObj,&item) == 0)
						{
							pMsg.result = 1;
							DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
							return;
						}
						break;
					case GET_ITEM(14,163):
						if(gObjectManager.CharacterUseWarehouseExpansion(lpObj,&item) == 0)
						{
							pMsg.result = 1;
							DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
							return;
						}
						break;
					case GET_ITEM(14,169):
						if(gObjectManager.CharacterUseCreationCard(lpObj,&item) == 0)
						{
							pMsg.result = 1;
							DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
							return;
						}
						break;
					case GET_ITEM(14,281):
						if(gObjectManager.CharacterUseCreationCard(lpObj,&item) == 0)
						{
							pMsg.result = 1;
							DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
							return;
						}
						break;
				}
			}
			else
			{
				if (lpProductInfo->ItemQuantity > 1 && gItemStack.GetItemMaxStack(lpProductInfo->ItemIndex) == 0)
				{
					if (gItemManager.CheckItemInventorySpace(lpObj, ((lpProductInfo->ItemQuantity / 2) + 1), 2) == 0)
					{
						pMsg.result = 1;
						DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
						return;
					}
				}

				if(gItemManager.CheckItemInventorySpace(lpObj,lpProductInfo->ItemIndex) == 0)
				{
					pMsg.result = 1;
					DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
					return;
				}

				CItem item;

				item.m_Index = lpProductInfo->ItemIndex;

				item.m_SocketOption[0] = lpProductInfo->ItemSocketOption1;
				item.m_SocketOption[1] = lpProductInfo->ItemSocketOption2;
				item.m_SocketOption[2] = lpProductInfo->ItemSocketOption3;
				item.m_SocketOption[3] = lpProductInfo->ItemSocketOption4;
				item.m_SocketOption[4] = lpProductInfo->ItemSocketOption5;
				
				item.m_JewelOfHarmonyOption = lpProductInfo->ItemHarmonyOption;

				if (item.IsPentagramItem() || item.IsPentagramJewel() || item.IsSocketItem() || (item.m_Index >= GET_ITEM(12, 414) && item.m_Index <= GET_ITEM(12, 421)))
				{
					item.m_SocketOptionBonus = item.m_JewelOfHarmonyOption;
					item.m_JewelOfHarmonyOption = 0;
				}
				else
				{
					item.m_SocketOptionBonus = gSocketItemOption.GetSocketItemBonusOption(&item);
				}

				if (lpProductInfo->ItemQuantity > 1 && (gItemStack.GetItemMaxStack(lpProductInfo->ItemIndex) == 0))
				{
					for (int i = 0; i < lpProductInfo->ItemQuantity; i++)
					{
						GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, lpProductInfo->ItemIndex, lpProductInfo->ItemLevel, 0, lpProductInfo->ItemOption1, lpProductInfo->ItemOption2, lpProductInfo->ItemOption3, -1, lpProductInfo->ItemNewOption, lpProductInfo->ItemSetOption, item.m_JewelOfHarmonyOption, lpProductInfo->ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, ((lpProductInfo->ItemDuration > 0) ? ((DWORD)time(0) + lpProductInfo->ItemDuration) : 0));
					}
				}
				else
				{
					GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, lpProductInfo->ItemIndex, lpProductInfo->ItemLevel, lpProductInfo->ItemQuantity, lpProductInfo->ItemOption1, lpProductInfo->ItemOption2, lpProductInfo->ItemOption3, -1, lpProductInfo->ItemNewOption, lpProductInfo->ItemSetOption, item.m_JewelOfHarmonyOption, lpProductInfo->ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, ((lpProductInfo->ItemDuration > 0) ? ((DWORD)time(0) + lpProductInfo->ItemDuration) : 0));
				}
			}
		}
		else
		{
			if(lpProductInfo->ItemIndex == GET_ITEM(13,124))
			{
				GJAccountLevelSaveSend(lpObj->Index,lpProductInfo->ItemLevel,lpProductInfo->ItemDuration);
				GJAccountLevelSend(lpObj->Index);
			}
			else
			{
				if (lpProductInfo->ItemQuantity > 1 && (gItemStack.GetItemMaxStack(lpProductInfo->ItemIndex) == 0))
				{
					if(gItemManager.CheckItemInventorySpace(lpObj, ((lpProductInfo->ItemQuantity/2)+1), 2) == 0)
					{
						pMsg.result = 1;
						DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
						return;
					}
				}

				if(gItemManager.CheckItemInventorySpace(lpObj,lpProductInfo->ItemIndex) == 0)
				{
					pMsg.result = 1;
					DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
					return;
				}

				CItem item;

				item.m_Index = lpProductInfo->ItemIndex;

				item.m_SocketOption[0] = lpProductInfo->ItemSocketOption1;
				item.m_SocketOption[1] = lpProductInfo->ItemSocketOption2;
				item.m_SocketOption[2] = lpProductInfo->ItemSocketOption3;
				item.m_SocketOption[3] = lpProductInfo->ItemSocketOption4;
				item.m_SocketOption[4] = lpProductInfo->ItemSocketOption5;

				item.m_JewelOfHarmonyOption = lpProductInfo->ItemHarmonyOption;

				if (item.IsPentagramItem() || item.IsPentagramJewel() || item.IsSocketItem() || (item.m_Index >= GET_ITEM(12, 414) && item.m_Index <= GET_ITEM(12, 421)))
				{
					item.m_SocketOptionBonus = item.m_JewelOfHarmonyOption;
					item.m_JewelOfHarmonyOption = 0;
				}
				else
				{
					item.m_SocketOptionBonus = gSocketItemOption.GetSocketItemBonusOption(&item);
				}

				if (lpProductInfo->ItemQuantity > 1 && (gItemStack.GetItemMaxStack(lpProductInfo->ItemIndex) == 0))
				{
					for (int i = 0; i < lpProductInfo->ItemQuantity; i++)
					{
						GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, lpProductInfo->ItemIndex, lpProductInfo->ItemLevel, 0, lpProductInfo->ItemOption1, lpProductInfo->ItemOption2, lpProductInfo->ItemOption3, -1, lpProductInfo->ItemNewOption, lpProductInfo->ItemSetOption, lpProductInfo->ItemHarmonyOption, lpProductInfo->ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, ((lpProductInfo->ItemDuration > 0) ? ((DWORD)time(0) + lpProductInfo->ItemDuration) : 0));
					}
				}
				else
				{
					GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, lpProductInfo->ItemIndex, lpProductInfo->ItemLevel, lpProductInfo->ItemQuantity, lpProductInfo->ItemOption1, lpProductInfo->ItemOption2, lpProductInfo->ItemOption3, -1, lpProductInfo->ItemNewOption, lpProductInfo->ItemSetOption, lpProductInfo->ItemHarmonyOption, lpProductInfo->ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, ((lpProductInfo->ItemDuration > 0) ? ((DWORD)time(0) + lpProductInfo->ItemDuration) : 0));
				}
			}
		}
	}
	else
	{
		if(lpProductInfo->ItemIndex == GET_ITEM(13, 37) || lpProductInfo->ItemIndex == GET_ITEM(13, 4) || lpProductInfo->ItemIndex == GET_ITEM(13, 5) || (lpProductInfo->ItemIndex >= GET_ITEM(13, 128) && lpProductInfo->ItemIndex <= GET_ITEM(13, 134)))
		{
			if (gItemManager.CheckItemInventorySpace(lpObj, lpProductInfo->ItemIndex) == 0)
			{
				pMsg.result = 1;
				DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
				return;
			}

			CItem item;

			item.m_Index = lpProductInfo->ItemIndex;

			item.m_SocketOption[0] = lpProductInfo->ItemSocketOption1;
			item.m_SocketOption[1] = lpProductInfo->ItemSocketOption2;
			item.m_SocketOption[2] = lpProductInfo->ItemSocketOption3;
			item.m_SocketOption[3] = lpProductInfo->ItemSocketOption4;
			item.m_SocketOption[4] = lpProductInfo->ItemSocketOption5;

			if (item.IsPentagramItem() || item.IsPentagramJewel() || item.IsSocketItem())
			{
				item.m_SocketOptionBonus = lpProductInfo->ItemHarmonyOption;
				lpProductInfo->ItemHarmonyOption = 0;
			}
			else
			{
				item.m_SocketOptionBonus = gSocketItemOption.GetSocketItemBonusOption(&item);
			}

			GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, lpProductInfo->ItemIndex, lpProductInfo->ItemLevel, lpProductInfo->ItemQuantity, lpProductInfo->ItemOption1, lpProductInfo->ItemOption2, lpProductInfo->ItemOption3, -1, lpProductInfo->ItemNewOption, lpProductInfo->ItemSetOption, lpProductInfo->ItemHarmonyOption, lpProductInfo->ItemOptionEx, item.m_SocketOption, item.m_SocketOptionBonus, ((lpProductInfo->ItemDuration>0) ? ((DWORD)time(0) + lpProductInfo->ItemDuration) : 0));
		}
		else
		{
			CEffect* lpEffect;
			int bufftime = 0;
			if (gEffectManager.GetEffect(lpObj, lpInfo->Index) != 0)
			{
				for (int n = 0; n < MAX_EFFECT_LIST; n++)
				{
					lpEffect = &lpObj->Effect[n];

					if (lpEffect->m_index == lpInfo->Index)
					{
						bufftime = lpEffect->m_count;
						break;
					}
				}

				if ((lpEffect->m_count + lpProductInfo->ItemDuration) > (50 * 24 * 60 * 60))
				{
					pMsg.result = 3;
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
					return;
				}
			}

			//gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, (int)(time(0) + lpProductInfo->ItemDuration + bufftime), 0, 0, 0, 0);

			if (!gEffectManager.CheckEffect(lpObj, lpInfo->Index))
			{
				gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, (int)(time(0) + lpProductInfo->ItemDuration), 0, 0, 0, 0);
			}
			else
			{
				CEffect* lpEffect = gEffectManager.GetEffect(lpObj, lpInfo->Index);
				gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, (int)(lpEffect->m_time + lpProductInfo->ItemDuration), 0, 0, 0, 0);
			}
		}
	}

	gLog.Output(LOG_CASH_SHOP,"[CashShopItemUse][ProductInfo][%s][%s] - (BaseIndex: %d, MainIndex: %d, CoinValue: %d, ItemIndex: %d, ItemLevel: %d, ItemOption1: %d, ItemOption2: %d, ItemOption3: %d, ItemNewOption: %d, ItemSetOption: %d, ItemHarmonyOption: %d, ItemOptionEx: %d, ItemSocketOption: %d, %d, %d, %d, %d, ItemQuantity: %d, ItemDuration: %d)",lpObj->Account,lpObj->Name,lpProductInfo->BaseIndex,lpProductInfo->MainIndex,lpProductInfo->CoinValue,lpProductInfo->ItemIndex,lpProductInfo->ItemLevel,lpProductInfo->ItemOption1,lpProductInfo->ItemOption2,lpProductInfo->ItemOption3,lpProductInfo->ItemNewOption,lpProductInfo->ItemSetOption,lpProductInfo->ItemHarmonyOption,lpProductInfo->ItemOptionEx,lpProductInfo->ItemSocketOption1,lpProductInfo->ItemSocketOption2,lpProductInfo->ItemSocketOption3,lpProductInfo->ItemSocketOption4,lpProductInfo->ItemSocketOption5,lpProductInfo->ItemQuantity,lpProductInfo->ItemDuration);

	pMsg.result = 0;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	this->GDCashShopDeleteItemSaveSend(lpObj->Index,0,lpMsg->BaseItemCode,lpMsg->MainItemCode);
}

void CCashShop::DGCashShopPeriodicItemRecv(SDHP_CASH_SHOP_PERIODIC_ITEM_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCashShopPeriodicItemRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	for(int n=0;n < lpMsg->count;n++)
	{
		SDHP_CASH_SHOP_PERIODIC_ITEM1* lpInfo = (SDHP_CASH_SHOP_PERIODIC_ITEM1*)(((BYTE*)lpMsg)+sizeof(SDHP_CASH_SHOP_PERIODIC_ITEM_RECV)+(sizeof(SDHP_CASH_SHOP_PERIODIC_ITEM1)*n));

		if(INVENTORY_RANGE(lpInfo->slot) == 0)
		{
			continue;
		}

		if(lpObj->Inventory[lpInfo->slot].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->Inventory[lpInfo->slot].m_Serial != lpInfo->serial)
		{
			continue;
		}

		if(lpObj->Inventory[lpInfo->slot].m_IsPeriodicItem == 0)
		{
			continue;
		}

		if(lpObj->Inventory[lpInfo->slot].m_LoadPeriodicItem != 0)
		{
			continue;
		}

		lpObj->Inventory[lpInfo->slot].m_LoadPeriodicItem = 1;

		lpObj->Inventory[lpInfo->slot].m_PeriodicItemTime = lpInfo->time;

		this->GCCashShopPeriodicItemSend(lpObj->Index,lpObj->Inventory[lpInfo->slot].m_Index,lpInfo->slot,lpObj->Inventory[lpInfo->slot].m_PeriodicItemTime);
	}
}

void CCashShop::DGCashShopRecievePointRecv(SDHP_CASH_SHOP_RECIEVE_POINT_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCashShopRecievePointRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	((void(*)(LPOBJ,DWORD,DWORD,DWORD,DWORD,DWORD))lpMsg->CallbackFunc)(lpObj,lpMsg->CallbackArg1,lpMsg->CallbackArg2,lpMsg->WCoinC,lpMsg->WCoinP,lpMsg->GoblinPoint);
}

void CCashShop::GDCashShopPeriodicItemSend(int aIndex) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[4096];

	SDHP_CASH_SHOP_PERIODIC_ITEM_SEND pMsg;

	pMsg.header.set(0x18,0x05,0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.count = 0;

	SDHP_CASH_SHOP_PERIODIC_ITEM2 info;

	for(int n=0;n < INVENTORY_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->Inventory[n].m_IsPeriodicItem == 0)
		{
			continue;
		}

		info.slot = n;

		info.serial = lpObj->Inventory[n].m_Serial;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send,&pMsg,sizeof(pMsg));

		gDataServerConnection.DataSend(send,size);
	}
}

void CCashShop::GDCashShopRecievePointSend(int aIndex,DWORD CallbackFunc,DWORD CallbackArg1,DWORD CallbackArg2) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	SDHP_CASH_SHOP_RECIEVE_POINT_SEND pMsg;

	pMsg.header.set(0x18,0x06,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.CallbackFunc = CallbackFunc;

	pMsg.CallbackArg1 = CallbackArg1;

	pMsg.CallbackArg2 = CallbackArg2;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCashShop::GDCashShopAddPointSaveSend(int aIndex,char* GiftAccount,DWORD AddWCoinC,DWORD AddWCoinP,DWORD AddGoblinPoint, char* source) // OK
{
	SDHP_CASH_SHOP_ADD_POINT_SAVE_SEND pMsg;

	pMsg.header.set(0x18,0x30,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	if(GiftAccount == 0)
	{
		memset(pMsg.GiftAccount,0,sizeof(pMsg.GiftAccount));
	}
	else
	{
		memcpy(pMsg.GiftAccount,GiftAccount,sizeof(pMsg.GiftAccount));
	}

	pMsg.AddWCoinC = AddWCoinC;

	pMsg.AddWCoinP = AddWCoinP;

	pMsg.AddGoblinPoint = AddGoblinPoint;

	memcpy(pMsg.source, source, sizeof(pMsg.source));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CCashShop::GDCashShopSubPointSaveSend(int aIndex,char* GiftAccount,DWORD SubWCoinC,DWORD SubWCoinP,DWORD SubGoblinPoint, char* source) // OK
{
	SDHP_CASH_SHOP_SUB_POINT_SAVE_SEND pMsg;

	pMsg.header.set(0x18,0x31,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	if(GiftAccount == 0)
	{
		memset(pMsg.GiftAccount,0,sizeof(pMsg.GiftAccount));
	}
	else
	{
		memcpy(pMsg.GiftAccount,GiftAccount,sizeof(pMsg.GiftAccount));
	}

	pMsg.SubWCoinC = SubWCoinC;

	pMsg.SubWCoinP = SubWCoinP;

	pMsg.SubGoblinPoint = SubGoblinPoint;

	memcpy(pMsg.source, source, sizeof(pMsg.source));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CCashShop::GDCashShopInsertItemSaveSend(int aIndex,char* GiftAccount,BYTE InventoryType,DWORD PackageMainIndex,DWORD ProductBaseIndex,DWORD ProductMainIndex,double CoinValue,BYTE ProductType,char* GiftName,char* GiftText) // OK
{
	SDHP_CASH_SHOP_INSERT_ITEM_SAVE_SEND pMsg;

	pMsg.header.set(0x18,0x32,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	if(GiftAccount == 0)
	{
		memset(pMsg.GiftAccount,0,sizeof(pMsg.GiftAccount));
	}
	else
	{
		memcpy(pMsg.GiftAccount,GiftAccount,sizeof(pMsg.GiftAccount));
	}

	pMsg.InventoryType = InventoryType;

	pMsg.PackageMainIndex = PackageMainIndex;

	pMsg.ProductBaseIndex = ProductBaseIndex;

	pMsg.ProductMainIndex = ProductMainIndex;

	pMsg.CoinValue = CoinValue;

	pMsg.ProductType = ProductType;

	if(GiftName == 0)
	{
		memset(pMsg.GiftName,0,sizeof(pMsg.GiftName));
	}
	else
	{
		memcpy(pMsg.GiftName,GiftName,sizeof(pMsg.GiftName));
	}

	if(GiftText == 0)
	{
		memset(pMsg.GiftText,0,sizeof(pMsg.GiftText));
	}
	else
	{
		memcpy(pMsg.GiftText,GiftText,sizeof(pMsg.GiftText));
	}

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CCashShop::GDCashShopDeleteItemSaveSend(int aIndex,char* GiftAccount,DWORD BaseItemCode,DWORD MainItemCode) // OK
{
	SDHP_CASH_SHOP_DELETE_ITEM_SAVE_SEND pMsg;

	pMsg.header.set(0x18,0x33,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	if(GiftAccount == 0)
	{
		memset(pMsg.GiftAccount,0,sizeof(pMsg.GiftAccount));
	}
	else
	{
		memcpy(pMsg.GiftAccount,GiftAccount,sizeof(pMsg.GiftAccount));
	}

	pMsg.BaseItemCode = BaseItemCode;

	pMsg.MainItemCode = MainItemCode;

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CCashShop::GDCashShopPeriodicItemSaveSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[4096];

	SDHP_CASH_SHOP_PERIODIC_ITEM_SAVE_SEND pMsg;

	pMsg.header.set(0x18,0x34,0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.count = 0;

	SDHP_CASH_SHOP_PERIODIC_ITEM_SAVE info;

	for(int n=0;n < INVENTORY_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->Inventory[n].m_IsPeriodicItem == 0)
		{
			continue;
		}

		if(lpObj->Inventory[n].m_LoadPeriodicItem == 0)
		{
			continue;
		}

		info.serial = lpObj->Inventory[n].m_Serial;

		info.time = lpObj->Inventory[n].m_PeriodicItemTime;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send,&pMsg,sizeof(pMsg));

		gDataServerConnection.DataSend(send,size);
	}
}