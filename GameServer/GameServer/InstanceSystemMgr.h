#pragma once
/* 3986 */
#include "InstanceCounter.h"
#include "InstanceObject.h"
struct  _INSTANCE_REG_LIST
{
	int nInstanceIndex;
	int nContentsType;
};

class InstanceSystemMgr
{
public:
	InstanceSystemMgr();
	~InstanceSystemMgr();

	int RequestAllocInstanceResource( int aIndex, int nContentsType); // idb
	int RequestFreeInstanceResource( int nInstanceIdx, int aIndex, int nContentsType); // idb
	int SearchInstanceIndex(); // idb
	CMap *GetInstanceMap( int nInstanceIdx); // idb
	int GetInstanceCount(); // idb
	int GetInstanceReservedUserCount(); // idb
	int GetInstanceReservedMonsterCount(); // idb
	void GM_SendMessageMonsterInfo( int aIndex, int nInstanceIdx, int nMonsterClass); // idb
	int FindRegistUser(  char *szName, int *nInstanceIndex, int nContentsType); // idb
	void SetInstanceState( int nInstanceIndex, int nInstanceState); // idb
	int AddInstanceMonster( int nInstanceIdx, int nMonsterIndex); // idb
	void DeleteAllInstanceMonster( int nInstanceIdx); // idb
	void DeleteAllInstanceMapItem( int nInstanceIdx); // idb
	void StateSetInstanceMap(); // idb

//private:

	//mu::Mutex m_csInstanceSystem;
	InstanceCounter m_cInstanceCounter;
	InstanceObject m_cInstanceObject[1000];
	std::map<std::string, _INSTANCE_REG_LIST>m_mapRegisterList;

};

extern InstanceSystemMgr *g_InstanceSystemMgr;