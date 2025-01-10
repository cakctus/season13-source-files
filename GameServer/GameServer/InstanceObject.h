#pragma once
class InstanceObject
{
public:
	InstanceObject();
	~InstanceObject();

	void ClearInfo(); // idb
	BYTE SetOwnerUser(int nOwnerUserIndex, const char *szOwnerUserName);
	int AddUserInfo(int nUserIndex, const char *szUserName); // idb
	int AddMonsterInfo(int nMonsterIndex); // idb
	int DeleteMonsterInfo(int nMonsterIndex); // idb
	int DeleteAllMonster(); // idb
	int DeleteAllMapItem(); // idb
	int SetAttrInstanceData(int nMapAttrValue, BYTE btLTPointX, BYTE btLTPointY, BYTE btRBPointX, BYTE btRBPointY); // idb
	int SetAttrInstanceData(int nMapAttrValue, int nAttrBeginPos, int nLength); // idb
	void SetAllocTime(ATL::CTime *cAllocTime); // idb
	ATL::CTime *GetAllocTime(ATL::CTime *result); // idb
	int RegisterInstanceObject(int aIndex, const char *szName, int nContentsType, unsigned __int16 nAvailableUserCount, unsigned __int16 nAvailableMonsterCount); // idb
	void MakeInstanceMapInfo(int nContentsType,int index); // idb
	unsigned int GetCurrentUserCount();
	int FindMonsterIndex(int nClass); // idb
	int GetState(); // idb
	void SetState(int nState); // idb
	int GetContentsType(); // idb
	int GetOwnerUserIndex(); // idb
	char *GetOwnerUserName();
	unsigned __int16 GetMaxUserCount(); // idb
	unsigned __int16 GetMaxMonsterCount(); // idb
//private:

		int m_nState;
		int m_nContentsType;
		int m_nOwnerIndex;
		char m_szOwnerName[11];
		__declspec(align(2)) unsigned __int16 m_nMaxUserCount;
		unsigned __int16 m_nMaxMonsterCount;
		std::map<std::string, int> m_mapUserInfo;
		std::vector<int> m_vecMonsterInfo;
		CMap *m_lpMapCT;
		ATL::CTime m_cAllocTime;

};

