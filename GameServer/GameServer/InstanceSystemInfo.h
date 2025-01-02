#pragma once
/* 3985 */
struct  _INSTANCE_SYSTEM_INFO
{
	_INSTANCE_SYSTEM_INFO()
	{
		this->Clear();
	}
	void Clear()
	{
		this->nContentsType = -1;
		this->nMapNumber = -1;
		this->nAvailableUser = 0;
		this->nAvailableMonster = 0;
		this->nInstanceMapBaseValue = 0;
	}


	int nContentsType;
	int nMapNumber;
	int nAvailableUser;
	int nAvailableMonster;
	int nInstanceMapBaseValue;
};

/* 4344 */
struct  InstanceSystemMapInfo
{
};
struct InstanceSystemIdx
{
	unsigned __int16 wMapNumber;
}; 

class InstanceSystemInfo
{
public:
	InstanceSystemInfo();
	~InstanceSystemInfo();

	void LoadInstanceSystemScript(char *lpszFileName); // idb
	int GetInstanceSystemInfo(int nContentsType, _INSTANCE_SYSTEM_INFO *stInstanceSystemInfo); // idb
	int GetMinMaxInstanceMapNumber(int *nMinMapValue, int *nMaxMapValue); // idb
	int ResolutionInstanceIndex(unsigned __int16 wMapNumber, int *nInstanceIdx); // idb
private:
	std::map<int , _INSTANCE_SYSTEM_INFO> m_mapInstanceSystemInfo;
};

extern InstanceSystemInfo g_InstanceSystemInfo;