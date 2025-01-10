#include "stdafx.h"
#include "OfflineMode.h"
#include "QueryManager.h"
#include "SocketManager.h"
#include "Util.h"
#include "GuildManager.h"
#include "GuildMatching.h"

int iRestoreCount = 0;

COfflineMode gOfflineMode;

COfflineMode::COfflineMode()
{
	this->m_RunActive = false;
	this->m_ConnectedUser = false;
	this->m_ConnectedTick = 0;
	this->m_users.clear();
}

COfflineMode::~COfflineMode()
{
}

void COfflineMode::GDReqAllPlayer(int index)
{
	this->m_users.clear();

	int iReturnCode = 0;

	char szQuery[512] = { 0 };

	int ServerCode = gServerManager[index].m_ServerCode;

	gQueryManager.ExecQuery("SELECT m.memb___id as AccountID, m.memb__pwd as memb__pwd, o.Name as Name FROM OfflineMode o, Character c, MEMB_INFO m	WHERE o.Name = c.name AND c.AccountID = m.memb___id AND o.Status = 1 AND o.ServerCode = %d", ServerCode);

	while (gQueryManager.Fetch() != SQL_NO_DATA)
	{
		char szBuffer[256] = { 0 };
		char szAccountID[11] = { 0 };
		char szPassword[21] = { 0 };
		char szName[11];

		gQueryManager.GetAsString("Name", szName, 11);
		gQueryManager.GetAsString("AccountID", szAccountID, 11);
		gQueryManager.GetAsString("memb__pwd", szPassword, 21);

		if (strlen(szName) < 4 || strlen(szAccountID) < 4)
		{
			continue;
		}

		OFFMODEUSER_DATA list;
		list.uIndex = index;
		memcpy(list.AccountID, szAccountID, sizeof(szAccountID));
		memcpy(list.Password, szPassword, sizeof(szPassword));
		memcpy(list.Name, szName, sizeof(szName));
		this->m_users.push_back(list);
	}

	gQueryManager.Close();

	this->m_RunActive = true;
	iRestoreCount = 0;
}

void COfflineMode::GDReqOffCharacter(PMSG_GDREQ_OFFMODE_CHAR* lpMsg, int index)
{
	char szAccountID[11] = { 0 };
	char szName[11] = { 0 };

	memcpy(szAccountID, lpMsg->AccountID, sizeof(szAccountID));
	memcpy(szName, lpMsg->Name, sizeof(szName));

	if (strlen(szName) < 4 || strlen(szAccountID) < 4)
	{
		return;
	}

	SDHP_CHARACTER_INFO_SEND pMsg = { 0 };
	pMsg.header.set(0xEC, sizeof(SDHP_CHARACTER_INFO_SEND));

	pMsg.result = false;
	pMsg.index = lpMsg->Number;

	memcpy(pMsg.account, szAccountID, sizeof(szAccountID));
	memcpy(pMsg.name, szName, sizeof(szName));

	pMsg.result = ((CheckTextSyntax(lpMsg->Name, sizeof(lpMsg->Name)) == 0) ? 0 : 1);
	
	if (pMsg.result == 0 || gQueryManager.ExecQuery("SELECT * FROM Character WHERE AccountID='%s' AND Name='%s'", szAccountID, szName) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		LogAdd(LOG_RED, "Char Offliner não encontrado", szName, szAccountID);
		gQueryManager.Close();
	}
	else
	{
		pMsg.Level = (WORD)gQueryManager.GetAsInteger("cLevel");
		pMsg.Class = (BYTE)gQueryManager.GetAsInteger("Class");
		pMsg.LevelUpPoint = gQueryManager.GetAsInteger("LevelUpPoint");
		pMsg.Experience = gQueryManager.GetAsInteger("Experience");
		pMsg.Strength = gQueryManager.GetAsInteger("Strength");
		pMsg.Dexterity = gQueryManager.GetAsInteger("Dexterity");
		pMsg.Vitality = gQueryManager.GetAsInteger("Vitality");
		pMsg.Energy = gQueryManager.GetAsInteger("Energy");
		pMsg.Leadership = gQueryManager.GetAsInteger("Leadership");

		gQueryManager.GetAsBinary("Inventory", pMsg.Inventory[0], sizeof(pMsg.Inventory));

		gQueryManager.GetAsBinary("MagicList", pMsg.Skill[0], sizeof(pMsg.Skill));

		pMsg.Money = gQueryManager.GetAsInteger("Money");
		pMsg.Life = (DWORD)gQueryManager.GetAsFloat("Life");
		pMsg.MaxLife = (DWORD)gQueryManager.GetAsFloat("MaxLife");
		pMsg.Mana = (DWORD)gQueryManager.GetAsFloat("Mana");
		pMsg.MaxMana = (DWORD)gQueryManager.GetAsFloat("MaxMana");
		pMsg.BP = (DWORD)gQueryManager.GetAsFloat("BP");
		pMsg.MaxBP = (DWORD)gQueryManager.GetAsFloat("MaxBP");
		pMsg.Shield = (DWORD)gQueryManager.GetAsFloat("Shield");
		pMsg.MaxShield = (DWORD)gQueryManager.GetAsFloat("MaxShield");
		pMsg.Map = (BYTE)gQueryManager.GetAsInteger("MapNumber");
		pMsg.X = (BYTE)gQueryManager.GetAsInteger("MapPosX");
		pMsg.Y = (BYTE)gQueryManager.GetAsInteger("MapPosY");
		pMsg.Dir = (BYTE)gQueryManager.GetAsInteger("MapDir");
		pMsg.PKCount = gQueryManager.GetAsInteger("PkCount");
		pMsg.PKLevel = gQueryManager.GetAsInteger("PkLevel");
		pMsg.PKTime = gQueryManager.GetAsInteger("PkTime");
		pMsg.CtlCode = (BYTE)gQueryManager.GetAsInteger("CtlCode");

		gQueryManager.GetAsBinary("Quest", pMsg.Quest, sizeof(pMsg.Quest));
		gQueryManager.GetAsBinary("EffectList", pMsg.Effect[0], sizeof(pMsg.Effect));

		pMsg.FruitAddPoint = (WORD)gQueryManager.GetAsInteger("FruitAddPoint");
		pMsg.FruitSubPoint = (WORD)gQueryManager.GetAsInteger("FruitSubPoint");
		pMsg.ExtInventory = (BYTE)gQueryManager.GetAsInteger("ExtInventory");
		pMsg.Ruud = (DWORD)gQueryManager.GetAsInteger("Ruud");
		pMsg.ChatLimitTime = (DWORD)gQueryManager.GetAsInteger("ChatLimitTime");

		pMsg.AutoDt[0] = (DWORD)gQueryManager.GetAsInteger("AutoDt0");
		pMsg.AutoDt[1] = (DWORD)gQueryManager.GetAsInteger("AutoDt1");
		pMsg.AutoDt[2] = (DWORD)gQueryManager.GetAsInteger("AutoDt2");
		pMsg.AutoDt[3] = (DWORD)gQueryManager.GetAsInteger("AutoDt3");
		pMsg.AutoDt[4] = (DWORD)gQueryManager.GetAsInteger("AutoDt4");
		pMsg.AutoRr = (BYTE)gQueryManager.GetAsInteger("AutoRr");
		pMsg.AutoRe = (BYTE)gQueryManager.GetAsInteger("AutoRe");

		gQueryManager.Close();

		gQueryManager.ExecQuery("SELECT * FROM MasterSkillTree WHERE Name='%s'", lpMsg->Name);
		gQueryManager.Fetch();
		pMsg.MasterLevel = (DWORD)gQueryManager.GetAsInteger("MasterLevel");
		gQueryManager.Close();

		gQueryManager.ExecQuery("SELECT ExtWarehouse FROM AccountCharacter WHERE Id='%s'", szAccountID);
		gQueryManager.Fetch();
		pMsg.ExtWarehouse = (BYTE)gQueryManager.GetAsInteger("ExtWarehouse");
		gQueryManager.Close();

		gQueryManager.ExecQuery("EXEC WZ_GetResetInfo '%s','%s'", szAccountID, lpMsg->Name);
		gQueryManager.Fetch();
		pMsg.Reset = gQueryManager.GetAsInteger("Reset");
		gQueryManager.Close();

		gQueryManager.ExecQuery("EXEC WZ_GetMasterResetInfo '%s','%s'", szAccountID, lpMsg->Name);
		gQueryManager.Fetch();
		pMsg.MasterReset = gQueryManager.GetAsInteger("MasterReset");
		gQueryManager.Close();

		GUILD_MATCHING_INFO GuildMatchingInfo;
		GUILD_MATCHING_JOIN_INFO GuildMatchingJoinInfo;
		GUILD_INFO* lpGuildInfo = gGuildManager.GetMemberGuildInfo(lpMsg->Name);

		pMsg.UseGuildMatching = ((lpGuildInfo == 0) ? 0 : gGuildMatching.GetGuildMatchingInfo(&GuildMatchingInfo, lpGuildInfo->szName));
		pMsg.UseGuildMatchingJoin = ((lpGuildInfo != 0) ? 0 : gGuildMatching.GetGuildMatchingJoinInfo(&GuildMatchingJoinInfo, lpMsg->Name));

		gQueryManager.ExecQuery("UPDATE AccountCharacter SET GameIDC='%s' WHERE Id='%s'", lpMsg->Name, szAccountID);

		gQueryManager.Close();
	}
	
	gSocketManager.DataSend(index, (BYTE*)&pMsg, sizeof(pMsg));

	if(pMsg.result == 1)
	{
		this->SelectData(index, pMsg.index, pMsg.name);
	}
}

void COfflineMode::SelectData(int index, int Number, char* Name)
{
	PMSG_GDLOAD_OFFMODE_DATA pMsg = { 0 };
	pMsg.h.set(0xED, sizeof(pMsg));
	pMsg.Number = Number;

	char szQuery[256] = { 0 };
		
	if (gQueryManager.ExecQuery("SELECT ShopOpen, ShopText, Attack, Ghost FROM OfflineMode WHERE Name = '%s'", Name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		return;
	}

	pMsg.Attack = gQueryManager.GetAsInteger("Attack");
	pMsg.Ghost = gQueryManager.GetAsInteger("Ghost");
	pMsg.PShopOpen = gQueryManager.GetAsInteger("ShopOpen");
	gQueryManager.GetAsString("ShopText", pMsg.PShopText, sizeof(pMsg.PShopText));

	gQueryManager.Close();

	gSocketManager.DataSend(index, (BYTE*)&pMsg, sizeof(pMsg));
}

void COfflineMode::UpdateData(PMSG_GDSAVE_OFFMODE_DATA* lpMsg, int uIndex)
{
	char szPSName[36] = { 0 };
	memcpy(szPSName, lpMsg->PShopText, sizeof(szPSName));

	int ServerCode = gServerManager[uIndex].m_ServerCode;
	int iMaxLen = strlen(szPSName);

	for(int i = 0; i < iMaxLen; i++)
	{
		if(szPSName[i] == (char)"'" || szPSName[i] == (char)"\'" || szPSName[i] == (char)"%")
		{
			szPSName[i] = (char)" ";
		}
	}

	if (strlen(lpMsg->Name) < 4)
	{
		return;
	}

	if (gQueryManager.ExecQuery("SELECT Name FROM OfflineMode WHERE Name = '%s'", lpMsg->Name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
		gQueryManager.ExecQuery("INSERT INTO OfflineMode (Name, Status, Attack, Ghost, ShopOpen, ShopText, ServerCode) VALUES ('%s', %d, %d, %d, %d, '%s', %d)", lpMsg->Name, lpMsg->Status, lpMsg->Attack, lpMsg->Ghost, lpMsg->PShopOpen, lpMsg->PShopText, ServerCode);
		gQueryManager.Close();
	}
	else
	{
		gQueryManager.Close();
		gQueryManager.ExecQuery("UPDATE OfflineMode SET Status = %d, ShopOpen = %d, ShopText = '%s', ServerCode = %d, Attack = %d, Ghost = %d WHERE Name = '%s'", lpMsg->Status, lpMsg->PShopOpen, lpMsg->PShopText, ServerCode, lpMsg->Attack, lpMsg->Ghost, lpMsg->Name);
		gQueryManager.Close();
	}
}

void COfflineMode::Run()
{
	if(!this->m_RunActive)
	{
		return;
	}

	DWORD ConnectTime = GetTickCount() - this->m_ConnectedTick;

	if(ConnectTime > 1000)
	{
		this->m_ConnectedUser = false;
		this->m_ConnectedTick = 0;
	}

	if(this->m_ConnectedUser)
	{
		return;
	}

	std::vector<OFFMODEUSER_DATA>::iterator it = this->m_users.begin(); 

	if(it != this->m_users.end())
	{
		PMSG_DGANS_OFFMODE_START pMsg;
		pMsg.h.set(0xEB, 0x11, sizeof(pMsg));
		memcpy(pMsg.AccountID, it->AccountID, sizeof(it->AccountID));
		memcpy(pMsg.Password, it->Password, sizeof(it->Password));
		memcpy(pMsg.Name, it->Name, sizeof(it->Name));
		pMsg.TickCount = GetTickCount();

		gSocketManager.DataSend(it->uIndex, (BYTE*)&pMsg, sizeof(pMsg));

		this->m_users.erase(this->m_users.begin());

		this->m_ConnectedUser = true;
		this->m_ConnectedTick = GetTickCount();

		iRestoreCount++;
	}
	else
	{
		this->m_RunActive = false;
	}
}