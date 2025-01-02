// IpManager.h: interface for the CIpManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct IP_ADDRESS_INFO
{
	char IpAddress[16];
	WORD IpAddressCount;
};

struct IP_ADDRESS_TIMER
{
	char IpAddress[16];
	DWORD tick;
};

class CIpManager
{
public:
	CIpManager();
	virtual ~CIpManager();
	bool CheckIpAddress(char* IpAddress);
	bool CheckIpTimer(char* IpAddress);
	void InsertIpAddress(char* IpAddress);
	void InsertIpAddressTimer(char* IpAddress);
	void RemoveIpAddress(char* IpAddress);
private:
	std::map<std::string,IP_ADDRESS_INFO> m_IpAddressInfo;
	std::vector<IP_ADDRESS_TIMER> m_IpAddressTimer;
};

extern CIpManager gIpManager;
