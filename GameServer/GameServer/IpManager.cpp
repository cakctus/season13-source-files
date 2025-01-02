#include "stdafx.h"
#include "IpManager.h"
#include "ServerInfo.h"
#include "Util.h"

CIpManager gIpManager;

CIpManager::CIpManager() // OK
{

}

CIpManager::~CIpManager() // OK
{

}

bool CIpManager::CheckIpAddress(char* IpAddress) // OK
{
	std::map<std::string,IP_ADDRESS_INFO>::iterator it = this->m_IpAddressInfo.find(std::string(IpAddress));

	if(it == this->m_IpAddressInfo.end())
	{
		return ((gServerInfo.m_MaxIpConnection==0)?0:1);
	}
	else
	{
		return ((it->second.IpAddressCount>=gServerInfo.m_MaxIpConnection)?0:1);
	}
}

bool CIpManager::CheckIpTimer(char* IpAddress) // OK
{
	for (std::vector<IP_ADDRESS_TIMER>::iterator it = this->m_IpAddressTimer.begin(); it != this->m_IpAddressTimer.end(); it++)
	{
		if (strcmp(it->IpAddress, IpAddress) == 0)
		{
			if (((GetTickCount() - it->tick) < 2000))
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}

	return 1;
}

void CIpManager::InsertIpAddress(char* IpAddress) // OK
{
	IP_ADDRESS_INFO info;

	strcpy_s(info.IpAddress,IpAddress);

	info.IpAddressCount = 1;

	std::map<std::string,IP_ADDRESS_INFO>::iterator it = this->m_IpAddressInfo.find(std::string(IpAddress));

	if(it == this->m_IpAddressInfo.end())
	{
		this->m_IpAddressInfo.insert(std::pair<std::string,IP_ADDRESS_INFO>(std::string(IpAddress),info));
	}
	else
	{
		it->second.IpAddressCount++;
	}
}

void CIpManager::InsertIpAddressTimer(char* IpAddress) // OK
{
	for (std::vector<IP_ADDRESS_TIMER>::iterator it = this->m_IpAddressTimer.begin(); it != this->m_IpAddressTimer.end(); it++)
	{
		if (strcmp(it->IpAddress, IpAddress) == 0)
		{
			it->tick = GetTickCount();
			return;
		}
	}

	IP_ADDRESS_TIMER info;

	strcpy_s(info.IpAddress, IpAddress);

	info.tick = GetTickCount();

	this->m_IpAddressTimer.push_back(info);
}

void CIpManager::RemoveIpAddress(char* IpAddress) // OK
{
	std::map<std::string,IP_ADDRESS_INFO>::iterator it = this->m_IpAddressInfo.find(std::string(IpAddress));

	if(it != this->m_IpAddressInfo.end())
	{
		if((--it->second.IpAddressCount) == 0)
		{
			this->m_IpAddressInfo.erase(it);
		}
	}
}
