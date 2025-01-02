#include "stdafx.h"
#include "GameMain.h"
#include "MemScript.h"
#include "RheaGold.h"
#include "ShopManager.h"
#include "Shop.h"
#include "Util.h"
#include "Notice.h"
#include "Message.h"
#include "ServerInfo.h"
#include "MasteryItemExtOptionSet.h"
#include "DSProtocol.h"
CRheaGold gRheaGold;


CRheaGold::CRheaGold() // OK
{
	this->Init();
}

CRheaGold::~CRheaGold() // OK
{

}
void CRheaGold::Init() // OK
{
	for (int n = 0; n < SHOP_SIZE; n++)
	{
		this->m_Item[n].Clear();
		this->m_InventoryMap[n] = -1;
	}
}
void CRheaGold::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->Init();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			int Index = SafeGetItem(GET_ITEM(lpMemScript->GetNumber(), lpMemScript->GetAsNumber()));
			int Level = lpMemScript->GetAsNumber();
			int Dur = lpMemScript->GetAsNumber();
			int Skill = lpMemScript->GetAsNumber();
			int Luck = lpMemScript->GetAsNumber();
			int Option = lpMemScript->GetAsNumber();
			int Excellent = lpMemScript->GetAsNumber();
			int Set = lpMemScript->GetAsNumber();
			int Price = lpMemScript->GetAsNumber();
			this->InsertItem(Index, Level, Dur, Skill, Luck, Option, Excellent, 0, Set, Price);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CRheaGold::ShopItemSet(int slot, BYTE type) // OK
{
	if (RHEA_SHOP_INVENTORY_RANGE(slot) == 0)
	{
		return;
	}

	ITEM_INFO ItemInfo;

	if (gItemManager.GetInfo(this->m_Item[slot].m_Index, &ItemInfo) == 0)
	{
		return;
	}

	int x = slot % 8;
	int y = slot / 8;

	if ((x + ItemInfo.Width) > 8 || (y + ItemInfo.Height) > 15)
	{
		return;
	}

	for (int sy = 0; sy < ItemInfo.Height; sy++)
	{
		for (int sx = 0; sx < ItemInfo.Width; sx++)
		{
			this->m_InventoryMap[(((sy + y) * 8) + (sx + x))] = type;
		}
	}
}

void CRheaGold::InsertItem(int ItemIndex, int ItemLevel, int ItemDurability, int ItemOption1, int ItemOption2, int ItemOption3, int ItemNewOption, int ItemValue, int Set, int Price) // OK
{
	ITEM_INFO ItemInfo;

	if (gItemManager.GetInfo(ItemIndex, &ItemInfo) == 0)
	{
		return;
	}
	for (int slot = 0; slot < RHEA_SHOP_SIZE; slot++)
	{
		if (ItemIndex != -1 && this->m_Item[slot].m_Index == -1)
		{
			this->m_Item[slot].m_Level = ItemLevel;
			this->m_Item[slot].m_Durability = (float)((ItemDurability == 0) ? gItemManager.GetItemDurability(ItemIndex, ItemLevel, ItemNewOption, 0) : ItemDurability);
			this->m_Item[slot].Convert(ItemIndex, ItemOption1, ItemOption2, ItemOption3, ItemNewOption, Set, 0, 0, 0, 0xFF);
			this->m_Item[slot].m_PcPointValue = ItemValue;
			this->m_Item[slot].m_BuyMoney = Price;
			return;
		}
	}
}

bool CRheaGold::GetItem(CItem* lpItem, int slot) // OK
{
	if (SHOP_INVENTORY_RANGE(slot) != 0)
	{
		if (this->m_Item[slot].IsItem() != 0)
		{
			(*lpItem) = this->m_Item[slot];
			return 1;
		}
	}

	return 0;
}

BYTE CRheaGold::ShopRectCheck(int x, int y, int width, int height) // OK
{
	if ((x + width) > 8 || (y + height) > 15)
	{
		return 0xFF;
	}

	for (int sy = 0; sy < height; sy++)
	{
		for (int sx = 0; sx < width; sx++)
		{
			if (this->m_InventoryMap[(((sy + y) * 2) + (sx + x))] != 0xFF)
			{
				return 0xFF;
			}
		}
	}

	return ((y * 8) + x);
}

void CRheaGold::RheaGoldItemList(LPOBJ lpObj)
{
	BYTE send[2048];

	PMSG_RHEA_SHOP_ITEM_LIST_SEND pMsg;
	pMsg.header.set(0x31, 0);
	int size = sizeof(pMsg);
	pMsg.type = 0x17;
	pMsg.count = 0;

	PMSG_RHEA_SHOP_ITEM_LIST info;

	ITEM_INFO ItemInfo;

	for (int n = 0; n < SHOP_SIZE; n++)
	{
		if (this->m_Item[n].IsItem() != 0)
		{
			if (gItemManager.GetInfo(this->m_Item[n].m_Index, &ItemInfo) == 0)
			{
				return;
			}

			if (ItemInfo.RequireClass[lpObj->Class] > 0 || (ItemInfo.Index >= GET_ITEM(14, 0) && ItemInfo.Index <= GET_ITEM(14, 510)))
			{
				info.slot = n;

				gItemManager.ItemByteConvert(info.ItemInfo, this->m_Item[n]);

				memcpy(&send[size], &info, sizeof(info));
				size += sizeof(info);

				pMsg.count++;
			}
		}
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(lpObj->Index, send, size);

	//BYTE a[]={0xC2,0x01,0x7F,0x31,0x17,0x1D,0x01,0x60,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x02,0x48,0x00,0xFF,0x00,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x03,0x61,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x04,0x49,0x00,0xFF,0x00,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x05,0x62,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x06,0x4A,0x00,0xFF,0x00,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x11,0x67,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x13,0x68,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x15,0x69,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x18,0x4B,0x00,0xFF,0x00,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x19,0x29,0x00,0x01,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x1E,0x07,0x00,0x01,0x00,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x07,0x08,0x01,0x00,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x20,0x6A,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x22,0x6B,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x23,0x31,0x20,0x01,0x00,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x24,0x6C,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x26,0x6D,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x2B,0x32,0x30,0x01,0x00,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x30,0x6E,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x31,0x00,0x0D,0x24,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x32,0x73,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x34,0x74,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x36,0x75,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x40,0x76,0x00,0x00,0x80,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x56,0x1D,0x00,0x01,0xFF,0x00,0xD0,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x7A,0xB0,0xFF,0x00,0x56,0xFF,0x00,0x07,0xFF,0xFF,0xFF,0xFF,0x64,0x7B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7D,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00};
	//DataSend(lpObj->Index,(BYTE*)&a,sizeof(a));
}

void CRheaGold::RheaItemRecv(PMSG_RHEA_ITEM_BUY_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Interface.type != INTERFACE_RUUDSHOP)
	{
		LogAdd(LOG_RED, "[%s][%s] Espertinho do ruud.",lpObj->Account, lpObj->Name);
		return;
	}

	PMSG_RHEA_ITEM_BUY_SEND pMsg;
	pMsg.header.set(0xD0, 0xF0, sizeof(pMsg));
	pMsg.result = 0xFF;

	gObjFixInventoryPointer(aIndex);

	if (lpObj->Transaction == 1 || lpObj->RuudItemSlot > -1)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	CItem item;

	if (this->GetItem(&item, lpMsg->slot) == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (lpObj->Ruud < item.m_BuyMoney)
	{
		pMsg.result = 0xFC;
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		GCMessagePopupSend(lpObj, "Voce nao tem Ruud o suficiente para comprar este item.\nVoce precisa de mais %d .", (item.m_BuyMoney - lpObj->Ruud));
		return;
	}

	item.m_Level = ((item.m_Level == 0) ? 0 : (GetLargeRand() % item.m_Level));
	item.m_Option3 = ((item.m_Option3 == 0) ? 0 : (GetLargeRand() % item.m_Option3));

	if (item.m_Option2 > 1)
	{
		item.m_Option2 = (((GetLargeRand() % 100) <= item.m_Option2) ? 1 : 0);
	}

	BYTE btExtOption[6] = { -1,-1,-1,-1,-1,-1 };
	int iSlotCount = 0;

	item.m_NewOption = 1;

	if (item.m_Index < GET_ITEM(14, 0))
	{
		item.m_SocketOptionBonus = 0;
	}

	lpObj->RuudItemSlot = lpMsg->slot;

	GDCreateItemSend(aIndex, 0xEB, 254, 0, item.m_Index, item.m_Level, item.m_Durability, 0, item.m_Option2, item.m_Option3, -1, item.m_NewOption, 0, 0, 0, item.m_SocketOption, item.m_SocketOptionBonus,  0);
}

void CRheaGold::RheaItemSend(int aIndex, CItem lpItem, BYTE Result)
{
	PMSG_RHEA_ITEM_BUY_SEND pMsg;
	ITEM_INFO ItemInfo;

	pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

	pMsg.result = Result;
	gItemManager.ItemByteConvert(pMsg.ItemInfo, lpItem);
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

	gItemManager.GetInfo(lpItem.m_Index, &ItemInfo);
	gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "%s obtido", ItemInfo.Name);
}

int  CRheaGold::BloodItemMixList(int ItemIndex)
{
	switch (ItemIndex)
	{

	//BloodAngel Fury (BK)
	case GET_ITEM(13, 351):
		ItemIndex = GET_ITEM(8, 98);
		break;
	case GET_ITEM(13, 352):
		ItemIndex = GET_ITEM(7, 98);
		break;	
	case GET_ITEM(13, 353):
		ItemIndex = GET_ITEM(9, 98);
		break;
	case GET_ITEM(13, 354):
		ItemIndex = GET_ITEM(11, 98);
		break;

	//BloodAngel Transcendence (SM)
	case GET_ITEM(13, 359):
		ItemIndex = GET_ITEM(8, 99);
		break;
	case GET_ITEM(13, 360):
		ItemIndex = GET_ITEM(7, 99);
		break;
	case GET_ITEM(13, 361):
		ItemIndex = GET_ITEM(9, 99);
		break;
	case GET_ITEM(13, 362):
		ItemIndex = GET_ITEM(10, 99);
		break;

	//BloodAngel Flurry (ELF)
	case GET_ITEM(13, 363):
		ItemIndex = GET_ITEM(8, 100);
		break;
	case GET_ITEM(13, 364):
		ItemIndex = GET_ITEM(7, 100);
		break;
	case GET_ITEM(13, 365):
		ItemIndex = GET_ITEM(9, 100);
		break;
	case GET_ITEM(13, 366):
		ItemIndex = GET_ITEM(10, 100);
		break;
	
	//BloodAngel Flurry (ELF)
	case GET_ITEM(13, 367):
		ItemIndex = GET_ITEM(8, 158);
		break;
	case GET_ITEM(13, 368):
		ItemIndex = GET_ITEM(7, 158);
		break;
	case GET_ITEM(13, 369):
		ItemIndex = GET_ITEM(9, 158);
		break;
	case GET_ITEM(13, 370):
		ItemIndex = GET_ITEM(10, 158);
		break;

	//BloodAngel Extermity (MG)
	case GET_ITEM(13, 371):
		ItemIndex = GET_ITEM(8, 102);
		break;
	case GET_ITEM(13, 372):
		ItemIndex = GET_ITEM(11, 102);
		break;
	case GET_ITEM(13, 373):
		ItemIndex = GET_ITEM(9, 102);
		break;
	case GET_ITEM(13, 374):
		ItemIndex = GET_ITEM(10, 102);
		break;

	//BloodAngel Captivation (MG)
	case GET_ITEM(13, 375):
		ItemIndex = GET_ITEM(8, 161);
		break;
	case GET_ITEM(13, 376):
		ItemIndex = GET_ITEM(11, 161);
		break;
	case GET_ITEM(13, 377):
		ItemIndex = GET_ITEM(9, 161);
		break;
	case GET_ITEM(13, 378):
		ItemIndex = GET_ITEM(10, 161);
		break;

	//BloodAngel Conquest (DL)
	case GET_ITEM(13, 379):
		ItemIndex = GET_ITEM(8, 103);
		break;
	case GET_ITEM(13, 380):
		ItemIndex = GET_ITEM(7, 103);
		break;
	case GET_ITEM(13, 381):
		ItemIndex = GET_ITEM(9, 103);
		break;
	case GET_ITEM(13, 382):
		ItemIndex = GET_ITEM(10, 103);
		break;

	//BloodAngel Honor (SUM)
	case GET_ITEM(13, 387):
		ItemIndex = GET_ITEM(8, 101);
		break;
	case GET_ITEM(13, 388):
		ItemIndex = GET_ITEM(7, 101);
		break;
	case GET_ITEM(13, 389):
		ItemIndex = GET_ITEM(9, 101);
		break;
	case GET_ITEM(13, 390):
		ItemIndex = GET_ITEM(10, 101);
		break;

	//BloodAngel Destruction (RF)
	case GET_ITEM(13, 391):
		ItemIndex = GET_ITEM(8, 104);
		break;
	case GET_ITEM(13, 392):
		ItemIndex = GET_ITEM(7, 104);
		break;
	case GET_ITEM(13, 393):
		ItemIndex = GET_ITEM(9, 104);
		break;
	case GET_ITEM(13, 394):
		ItemIndex = GET_ITEM(11, 104);
		break;
	
	//BloodAngel Tenacity (GL)
	case GET_ITEM(13, 399):
		ItemIndex = GET_ITEM(8, 105);
		break;
	case GET_ITEM(13, 400):
		ItemIndex = GET_ITEM(7, 105);
		break;
	case GET_ITEM(13, 401):
		ItemIndex = GET_ITEM(9, 105);
		break;
	case GET_ITEM(13, 402):
		ItemIndex = GET_ITEM(10, 105);
		break;

	case GET_ITEM(7, 158):
		ItemIndex = GET_ITEM(7, 159);
		break;
	case GET_ITEM(7, 159):
		ItemIndex = GET_ITEM(7, 160);
		break;
	case GET_ITEM(8, 158):
		ItemIndex = GET_ITEM(8, 159);
		break;
	case GET_ITEM(8, 159):
		ItemIndex = GET_ITEM(8, 160);
		break;
	case GET_ITEM(9, 158):
		ItemIndex = GET_ITEM(9, 159);
		break;
	case GET_ITEM(9, 159):
		ItemIndex = GET_ITEM(9, 160);
		break;
	case GET_ITEM(10, 158):
		ItemIndex = GET_ITEM(10, 159);
		break;
	case GET_ITEM(10, 159):
		ItemIndex = GET_ITEM(10, 160);
		break;
	case GET_ITEM(11, 158):
		ItemIndex = GET_ITEM(11, 159);
		break;
	case GET_ITEM(11, 159):
		ItemIndex = GET_ITEM(11, 160);
		break;

	case GET_ITEM(8, 161):
		ItemIndex = GET_ITEM(8, 162);
		break;
	case GET_ITEM(8, 162):
		ItemIndex = GET_ITEM(8, 163);
		break;
	case GET_ITEM(9, 161):
		ItemIndex = GET_ITEM(9, 162);
		break;
	case GET_ITEM(9, 162):
		ItemIndex = GET_ITEM(9, 163);
		break;
	case GET_ITEM(10, 161):
		ItemIndex = GET_ITEM(10, 162);
		break;
	case GET_ITEM(10, 162):
		ItemIndex = GET_ITEM(10, 163);
		break;
	case GET_ITEM(11, 161):
		ItemIndex = GET_ITEM(11, 162);
		break;
	case GET_ITEM(11, 162):
		ItemIndex = GET_ITEM(11, 163);
		break;

	//Awakening Soul
	case GET_ITEM(7, 160):
		ItemIndex = GET_ITEM(7, 114);
		break;
	case GET_ITEM(8, 160):
		ItemIndex = GET_ITEM(8, 114);
		break;
	case GET_ITEM(9, 160):
		ItemIndex = GET_ITEM(9, 114);
		break;
	case GET_ITEM(10, 160):
		ItemIndex = GET_ITEM(10, 114);
		break;
	case GET_ITEM(11, 160):
		ItemIndex = GET_ITEM(11, 114);
		break;

	case GET_ITEM(7, 163):
		ItemIndex = GET_ITEM(7, 115);
		break;
	case GET_ITEM(8, 163):
		ItemIndex = GET_ITEM(8, 115);
		break;
	case GET_ITEM(9, 163):
		ItemIndex = GET_ITEM(9, 115);
		break;
	case GET_ITEM(10, 163):
		ItemIndex = GET_ITEM(10, 115);
		break;
	case GET_ITEM(11, 163):
		ItemIndex = GET_ITEM(11, 115);
		break;
	}

	return ItemIndex;
}

int CRheaGold::GetItemNewOption(int ItemNewOption, int Type) // OK
{
	int count = 0;

	int NewOption = 0;

	if (Type == 0)
	{
		NewOption = 32;

		while (true)
		{
			if (count >= (ItemNewOption - 1) || count >= MAX_EXC_OPTION)
			{
				break;
			}

			int value = 1 << (GetLargeRand() % MAX_EXC_OPTION);

			if ((NewOption & value) == 0)
			{
				NewOption |= value;
				count++;
			}
		}
	}
	else
	{
		NewOption = 0;
		while (true)
		{
			if (count >= ItemNewOption || count >= MAX_EXC_OPTION)
			{
				break;
			}

			int value = 1 << (GetLargeRand() % MAX_EXC_OPTION);

			if ((NewOption & value) == 0)
			{
				NewOption |= value;
				count++;
			}
		}
	}

	return NewOption;
}

void  CRheaGold::GD_RuudTokenCountUpdate(LPOBJ lpObj, unsigned int dwRuud)
{
	SDHP_RUUDTOKENUPDATE pMsg; // [sp+4Ch] [bp-24h]@3

	if (lpObj)
	{
		pMsg.h.set(0x19, sizeof(pMsg));

		memcpy(pMsg.AccountID, lpObj->Account, sizeof(pMsg.AccountID));
		memcpy(pMsg.CharName, lpObj->Name, sizeof(pMsg.CharName));
		pMsg.dwRuud = dwRuud;
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);

		LogAdd(LOG_BLACK, "[Ruud][GD_RuudTokenCountUpdate] ACC:%s,NAME:%s,Ruud:%d", lpObj->Account, lpObj->Name, dwRuud);
	}
	else
	{
		LogAdd(LOG_BLACK, "[GD_RuudTokenCountUpdate] lpObj is null");
	}
}

void  CRheaGold::GC_Notify_RuudUpdate(int aIndex, unsigned int dwRuud, unsigned int dwAddRuud, bool bGet)
{
	PMSG_RUUD_TOKEN_UPDATE pMsg;
	pMsg.h.set(0xD0, 0xF1, sizeof(pMsg));

	pMsg.dwRuudPoint = dwRuud;
	pMsg.dwAddPoint = dwAddRuud;
	pMsg.bGet = bGet;
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.h.size);
}