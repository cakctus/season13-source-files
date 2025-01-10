// EventInventory.cpp: implementation of the CEventInventory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventInventory.h"
#include "QueryManager.h"
#include "SocketManager.h"

CEventInventory gEventInventory;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventInventory::CEventInventory() // OK
{

}

CEventInventory::~CEventInventory() // OK
{

}

void CEventInventory::GDEventInventoryRecv(SDHP_EVENT_INVENTORY_RECV* lpMsg,int index) // OK
{
	SDHP_EVENT_INVENTORY_SEND pMsg;

	pMsg.header.set(0x26,0x00,sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	if(gQueryManager.ExecQuery("SELECT Items FROM EventInventory WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();

		memset(pMsg.EventInventory,0xFF,sizeof(pMsg.EventInventory));
	}
	else
	{
		gQueryManager.GetAsBinary("Items",pMsg.EventInventory[0],sizeof(pMsg.EventInventory));

		gQueryManager.Close();
	}

	gSocketManager.DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void CEventInventory::GDEventInventorySaveRecv(SDHP_EVENT_INVENTORY_SAVE_RECV* lpMsg) // OK
{
	if(gQueryManager.ExecQuery("SELECT Name FROM EventInventory WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		gQueryManager.BindParameterAsBinary(1,lpMsg->EventInventory[0],sizeof(lpMsg->EventInventory));
		gQueryManager.ExecQuery("INSERT INTO EventInventory (Name,Items) VALUES ('%s',?)",lpMsg->name);
		gQueryManager.Close();
	}
	else
	{
		gQueryManager.Close();
		gQueryManager.BindParameterAsBinary(1,lpMsg->EventInventory[0],sizeof(lpMsg->EventInventory));
		gQueryManager.ExecQuery("UPDATE EventInventory SET Items=? WHERE Name='%s'",lpMsg->name);
		gQueryManager.Close();
	}
}
