#include "StdAfx.h"
#include "NewQuestWorld.h"
#include "SocketManager.h"
#include "QueryManager.h"

NewQuestWorld gNewQuestWorld;

NewQuestWorld::NewQuestWorld(void)
{
}


NewQuestWorld::~NewQuestWorld(void)
{
}
void NewQuestWorld::GDNewQuestWorldRecv(SDHP_NEW_QUEST_WORLD_RECV* lpMsg,int index) // OK
{
	SDHP_NEW_QUEST_WORLD_SEND pMsg;
	pMsg.header.set(0x10,0x01,sizeof(pMsg));
	pMsg.index = lpMsg->index;
	memcpy(pMsg.account,lpMsg->account,sizeof(pMsg.account));
	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	if(gQueryManager.ExecQuery("SELECT NewQuestWorldList FROM NewQuestWorld WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		memset(&pMsg.Quest,0xFF,sizeof(pMsg.Quest));
	}
	else
	{
		gQueryManager.GetAsBinary("NewQuestWorldList",(BYTE*)&pMsg.Quest,sizeof(pMsg.Quest));
		gQueryManager.Close();
	}

	gSocketManager.DataSend(index,(BYTE*)&pMsg,sizeof(pMsg));
}

void NewQuestWorld::GDNewQuestWorldSaveRecv(SDHP_NEW_QUEST_WORLD_SAVE_RECV* lpMsg) // OK
{
	if(gQueryManager.ExecQuery("SELECT Name FROM NewQuestWorld WHERE Name='%s'",lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		gQueryManager.BindParameterAsBinary(1,(BYTE*)&lpMsg->Quest,sizeof(lpMsg->Quest));
		gQueryManager.ExecQuery("INSERT INTO NewQuestWorld (Name,NewQuestWorldList) VALUES ('%s',?)",lpMsg->name);
		gQueryManager.Close();
	}
	else
	{
		gQueryManager.Close();
		gQueryManager.BindParameterAsBinary(1,(BYTE*)&lpMsg->Quest,sizeof(lpMsg->Quest));
		gQueryManager.ExecQuery("UPDATE NewQuestWorld SET NewQuestWorldList=? WHERE Name='%s'",lpMsg->name);
		gQueryManager.Close();
	}
}