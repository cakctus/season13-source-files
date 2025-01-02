#include "stdafx.h"
#include "AccountManager.h"
#include "Log.h"
#include "QueryManager.h"
#include "ServerManager.h"
#include "Util.h"

CAccountManager gAccountManager;

CAccountManager::CAccountManager() // OK
{

}

CAccountManager::~CAccountManager() // OK
{

}

void CAccountManager::DisconnectProc() // OK
{
	this->m_critical.lock();

	for(std::map<std::string,ACCOUNT_INFO>::iterator it=this->m_AccountInfo.begin();it != this->m_AccountInfo.end();)
	{
		if(it->second.MapServerMove == 0 || (GetTickCount()-it->second.MapServerMoveTime) < 30000)
		{
			it++;
			continue;
		}

		gLog.Output(LOG_ACCOUNT,"[AccountInfo] Account disconnected by proc (Account: %s, IpAddress: %s, GameServerCode: %d)",it->second.Account,it->second.IpAddress,it->second.GameServerCode);

		gQueryManager.ExecQuery("EXEC WZ_DISCONNECT_MEMB '%s'",it->second.Account);

		gQueryManager.Close();

		JGExternalDisconnectAccountSend(it->second.GameServerCode,it->second.UserIndex,it->second.Account);

		it = this->m_AccountInfo.erase(it);
	}

	this->m_critical.unlock();
}

void CAccountManager::ClearServerAccountInfo(WORD ServerCode) // OK
{
	this->m_critical.lock();

	for(std::map<std::string,ACCOUNT_INFO>::iterator it=this->m_AccountInfo.begin();it != this->m_AccountInfo.end();)
	{
		if(it->second.GameServerCode != ServerCode)
		{
			it++;
			continue;
		}

		gLog.Output(LOG_ACCOUNT,"[AccountInfo] Account disconnected by clear (Account: %s, IpAddress: %s, GameServerCode: %d)",it->second.Account,it->second.IpAddress,it->second.GameServerCode);

		gQueryManager.ExecQuery("EXEC WZ_DISCONNECT_MEMB '%s'",it->second.Account);

		gQueryManager.Close();

		it = this->m_AccountInfo.erase(it);
	}

	this->m_critical.unlock();
}

bool CAccountManager::GetAccountInfo(ACCOUNT_INFO* lpAccountInfo,char* account) // OK
{
	this->m_critical.lock();

	std::string acc(account);

	std::transform(acc.begin(),acc.end(),acc.begin(),CheckAccountCaseSensitive);

	std::map<std::string,ACCOUNT_INFO>::iterator it = this->m_AccountInfo.find(acc);

	if(it != this->m_AccountInfo.end())
	{
		(*lpAccountInfo) = it->second;
		this->m_critical.unlock();
		return 1;
	}

	this->m_critical.unlock();
	return 0;
}

void CAccountManager::InsertAccountInfo(ACCOUNT_INFO AccountInfo) // OK
{
	this->m_critical.lock();

	std::string acc(AccountInfo.Account);

	std::transform(acc.begin(),acc.end(),acc.begin(),CheckAccountCaseSensitive);

	std::map<std::string,ACCOUNT_INFO>::iterator it = this->m_AccountInfo.find(acc);

	if(it == this->m_AccountInfo.end())
	{
		this->m_AccountInfo.insert(std::pair<std::string,ACCOUNT_INFO>(acc,AccountInfo));
	}
	else
	{
		it->second = AccountInfo;
	}

	this->m_critical.unlock();
}

void CAccountManager::RemoveAccountInfo(ACCOUNT_INFO AccountInfo) // OK
{
	this->m_critical.lock();

	std::string acc(AccountInfo.Account);

	std::transform(acc.begin(),acc.end(),acc.begin(),CheckAccountCaseSensitive);

	std::map<std::string,ACCOUNT_INFO>::iterator it = this->m_AccountInfo.find(acc);

	if(it != this->m_AccountInfo.end())
	{
		this->m_AccountInfo.erase(it);
		this->m_critical.unlock();
		return;
	}

	this->m_critical.unlock();
}

long CAccountManager::GetAccountCount() // OK
{
	long AccountCount = 0;

	this->m_critical.lock();

	AccountCount = this->m_AccountInfo.size();

	this->m_critical.unlock();

	return AccountCount;
}

void CAccountManager::GetAccountsToDisconnect()
{
	LogAdd(LOG_BLUE, "Disconnect routine");

	if (gQueryManager.ExecQuery("SELECT TOP 10 memb___id FROM MEMB_STAT WHERE Disconnect = 1") != 0)
	{
		ACCOUNT_INFO AccountInfo[10];
		char account[11] = { 0 };
		int count = 0;

		while (gQueryManager.Fetch() != SQL_NO_DATA)
		{
			gQueryManager.GetAsString("memb___id", account, sizeof(account));

			LogAdd(LOG_BLUE, "Conta obtida: %s", account);

			if (gAccountManager.GetAccountInfo(&AccountInfo[count], account))
			{
				LogAdd(LOG_BLUE, "Conta online, para lista de dc: %s", account);
				count++;
			}
		}

		gQueryManager.Close();

		for (int i = 0; i < count; i++)
		{
			for (int n = 0; n < MAX_SERVER; n++)
			{
				if (gServerManager[n].CheckState() != 0)
				{
					LogAdd(LOG_BLUE, "Disconnecting [%s][%d] from server %d", AccountInfo[i].Account, AccountInfo[i].UserIndex, gServerManager[n].m_ServerCode);
					JGExternalDisconnectAccountSend(gServerManager[n].m_ServerCode, AccountInfo[i].UserIndex, AccountInfo[i].Account);
				}
			}
		}
	}
	else
	{
		gQueryManager.Close();
	}
}
