#include "stdafx.h"
#include "MasterSkillTree_4th.h"
#include "QueryManager.h"
#include "SocketManager.h"

MasterSkillTree_4th gMasterSkillTree_4th;

MasterSkillTree_4th::MasterSkillTree_4th()
{
}

MasterSkillTree_4th::~MasterSkillTree_4th()
{
}

void MasterSkillTree_4th::GDMasterSkillTreeRecv(SDHP_FOR_MASTER_SKILL_TREE_RECV* lpMsg, int index) // OK
{
	SDHP_FOR_MASTER_SKILL_TREE_SEND pMsg;
	pMsg.header.set(0x7E, 0x00, sizeof(pMsg));
	pMsg.index = lpMsg->index;
	memcpy(pMsg.account, lpMsg->account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpMsg->name, sizeof(pMsg.name));
	if (gQueryManager.ExecQuery("SELECT * FROM MasterSkillTree_4th WHERE Name='%s'", lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		pMsg.ForMasterPoint = 0;
		memset(pMsg.ForMasterSkill, 0xFF, sizeof(pMsg.ForMasterSkill));
	}
	else
	{
		pMsg.ForMasterPoint = gQueryManager.GetAsInteger("MasterPoint");
		gQueryManager.GetAsBinary("MasterSkill", pMsg.ForMasterSkill[0], sizeof(pMsg.ForMasterSkill));
		gQueryManager.Close();
	}

	gSocketManager.DataSend(index, (BYTE*)&pMsg, sizeof(pMsg));
}

void MasterSkillTree_4th::GDMasterSkillTreeSaveRecv(SDHP_FOR_MASTER_SKILL_TREE_SAVE_RECV* lpMsg) // OK
{
	if (gQueryManager.ExecQuery("SELECT Name FROM MasterSkillTree_4th WHERE Name='%s'", lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		gQueryManager.BindParameterAsBinary(1, lpMsg->ForMasterSkill[0], sizeof(lpMsg->ForMasterSkill));
		gQueryManager.ExecQuery("INSERT INTO MasterSkillTree_4th (Name,MasterPoint,MasterSkill) VALUES ('%s', %d, ?)", lpMsg->name, lpMsg->ForMasterPoint);
		gQueryManager.Close();
	}
	else
	{
		gQueryManager.Close();
		gQueryManager.BindParameterAsBinary(1, lpMsg->ForMasterSkill[0], sizeof(lpMsg->ForMasterSkill));
		gQueryManager.ExecQuery("UPDATE MasterSkillTree_4th SET MasterPoint=%d,MasterSkill=? WHERE Name='%s'", lpMsg->ForMasterPoint, lpMsg->name);
		gQueryManager.Close();
	}
}