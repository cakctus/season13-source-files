#include "stdafx.h"
#include "Map.h"

#include "InstanceSystemMgr.h"
#include "InstanceSystemInfo.h"

#include "Util.h"
InstanceSystemMgr *g_InstanceSystemMgr;
InstanceSystemMgr::InstanceSystemMgr()
{
	//mu::Mutex::Mutex(&this->m_csInstanceSystem);
	this->m_cInstanceCounter;
	this->m_mapRegisterList.clear();
}


InstanceSystemMgr::~InstanceSystemMgr()
{
}


//----- (00EB2CB0) --------------------------------------------------------
int  InstanceSystemMgr::RequestAllocInstanceResource(int aIndex, int nContentsType)
{

	_INSTANCE_REG_LIST stInstanceRegList; // [sp+A8h] [bp-3Ch]@9
	int nInstanceIndex; // [sp+B0h] [bp-34h]@1

	int nRet; // [sp+B8h] [bp-2Ch]@1
	_INSTANCE_SYSTEM_INFO stInstanceSystemInfo; // [sp+BCh] [bp-28h]@1
	//mu::ScopedLock sl; // [sp+D0h] [bp-14h]@1

	//mu::ScopedLock::ScopedLock(&sl, (mu::Lockable *)&this->m_csInstanceSystem.vfptr);
	

	nRet = g_InstanceSystemInfo.GetInstanceSystemInfo(nContentsType, &stInstanceSystemInfo);

	LPOBJ lpObj = &gObj[aIndex];
	nInstanceIndex = -1;
	if (nRet)
	{
		LogAdd(LOG_BLACK,
			"[INSTACE][ERROR] [%s][%s] not found instance contents type.(%d)",
			lpObj->Account,
			lpObj->Name,
			nContentsType);

		//mu::ScopedLock::~ScopedLock(&sl);
		return nRet;
	}
	else
	{
		nRet = this->FindRegistUser(lpObj->Name, &nInstanceIndex, nContentsType);
		if (nRet)
		{
			nRet =this->m_cInstanceCounter.CheckAvailableInstanceCounter(
				stInstanceSystemInfo.nAvailableUser,
				stInstanceSystemInfo.nAvailableMonster);
			if (nRet)
			{
				//mu::ScopedLock::~ScopedLock(&sl);
				return nRet;

			}
			else
			{
				nInstanceIndex = this->SearchInstanceIndex();
				if (nInstanceIndex == -1)
				{
					LogAdd(LOG_BLACK,"[INSTANCE][ERROR] [%s][%s] not found empty instance object.", lpObj->Account, lpObj->Name);

					//mu::ScopedLock::~ScopedLock(&sl);
					return 12;
				}
				else
				{
					this->m_cInstanceObject[nInstanceIndex].RegisterInstanceObject(
						lpObj->Index,
						lpObj->Name,
						nContentsType,
						stInstanceSystemInfo.nAvailableUser,
						stInstanceSystemInfo.nAvailableMonster);

					stInstanceRegList.nInstanceIndex = nInstanceIndex;
					stInstanceRegList.nContentsType = nContentsType;
					this->m_mapRegisterList.insert(std::pair<std::string, _INSTANCE_REG_LIST>(lpObj->Name, stInstanceRegList));
					this->m_cInstanceObject[nInstanceIndex].MakeInstanceMapInfo(nContentsType, nInstanceIndex);
					this->m_cInstanceCounter.IncInstanceCounter(
						stInstanceSystemInfo.nAvailableUser,
						stInstanceSystemInfo.nAvailableMonster);

					//mu::ScopedLock::~ScopedLock(&sl);
					return 0;
				}
			}
		}
		else
		{
			LogAdd(LOG_BLACK,"[INSTACE][WARN] Already regist user.[%s][%s]", lpObj->Account, lpObj->Name);

			//mu::ScopedLock::~ScopedLock(&sl);
			return 0;
		}
	}

}



//----- (00EB3170) --------------------------------------------------------
int  InstanceSystemMgr::RequestFreeInstanceResource(int nInstanceIdx, int aIndex, int nContentsType)
{

	int nRet; // [sp+B4h] [bp-2Ch]@1
	_INSTANCE_SYSTEM_INFO stInstanceSystemInfo; // [sp+B8h] [bp-28h]@1
	//mu::ScopedLock sl; // [sp+CCh] [bp-14h]@1

	//mu::ScopedLock::ScopedLock(&sl, (mu::Lockable *)&this->m_csInstanceSystem.vfptr);


	nRet = g_InstanceSystemInfo.GetInstanceSystemInfo(nContentsType, &stInstanceSystemInfo);
	if (nRet)
	{
		LogAdd(LOG_BLACK,
			"[INSTANCE][ERROR] %s, %d",
			"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\instancesystem.cpp",
			__LINE__);

		//mu::ScopedLock::~ScopedLock(&sl);
		return nRet;
	}
	else
	{
		LPOBJ lpObj = &gObj[aIndex];
		nRet = this->FindRegistUser(gObj[aIndex].Name, &nInstanceIdx, nContentsType);
		if (nRet)
		{
			LogAdd(LOG_BLACK,
				"[INSTANCE][ERROR] %s [%s][%s](Contents:%d) %s, %d",
				"InstanceSystemMgr::RequestFreeInstanceResource",
				lpObj->Account,
				lpObj->Name,
				nContentsType,
				"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\instancesystem.cpp",
				__LINE__);

			//mu::ScopedLock::~ScopedLock(&sl);
			return nRet;
		}
		else
		{
			if (this->m_cInstanceObject[nInstanceIdx].GetCurrentUserCount() != 1)
			{

				//v9 = nContentsType;
				LogAdd(LOG_BLACK,
					"[INSTANCE][ERROR] %s, instance obj, current user count.[%s][%s](Contents:%d)(Inst idx:%d) (%d)",
					"InstanceSystemMgr::RequestFreeInstanceResource",
					lpObj->Account,
					lpObj->Name,
					nContentsType,
					nInstanceIdx,
					this->m_cInstanceObject[nInstanceIdx].GetCurrentUserCount());
			}
			this->m_cInstanceObject[nInstanceIdx].DeleteAllMonster();
			this->m_cInstanceObject[nInstanceIdx].DeleteAllMapItem();
			this->m_cInstanceObject[nInstanceIdx].ClearInfo();
			this->m_cInstanceCounter.DecInstanceCounter(
				stInstanceSystemInfo.nAvailableUser,
				stInstanceSystemInfo.nAvailableMonster);
			std::map<std::string, _INSTANCE_REG_LIST>::iterator it = this->m_mapRegisterList.find(lpObj->Name);
			if (it==this->m_mapRegisterList.end())
			{
				LogAdd(LOG_BLACK,
					"[INSTANCE][ERROR] Not found deleted user in reg-list [%s][%s]. %s, %d",
					lpObj->Account,
					lpObj->Name,
					"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\instancesystem.cpp",
					__LINE__);
			}

			else
			{
				LogAdd(LOG_BLACK,
					"[INSTANCE] [INFO] delete in reg-list [%s][%s](%d)(%d)=[%s]. (reg map size: %d)",
					lpObj->Account,
					lpObj->Name,
					it->second.nContentsType,
					it->second.nInstanceIndex,
					it->first.c_str(),
					this->m_mapRegisterList.size());
				it = this->m_mapRegisterList.erase(it);
			}
			//mu::ScopedLock::~ScopedLock(&sl);

		}
	}
	return 0;
}
// 13AD7E8: using guessed type int `InstanceSystemMgr::RequestFreeInstanceResource'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);


//----- (00EB3650) --------------------------------------------------------
int  InstanceSystemMgr::SearchInstanceIndex()
{
	int count = this->m_cInstanceCounter.GetUsingInstanceObjCounter();
	int totcount = 0;
	do
	{
		if (!this->m_cInstanceObject[count].GetState())
			return count;
		if (++count >= 1000)
			count = 0;
		++totcount;
	} while (totcount < 1000);
	return -1;
}

//----- (00EB3710) --------------------------------------------------------
CMap * InstanceSystemMgr::GetInstanceMap(int nInstanceIdx)
{
	return this->m_cInstanceObject[nInstanceIdx].m_lpMapCT;
}

//----- (00EB3740) --------------------------------------------------------
int  InstanceSystemMgr::GetInstanceCount()
{
	return this->m_cInstanceCounter.GetUsingInstanceObjCounter();
}

//----- (00EB3770) --------------------------------------------------------
int __thiscall InstanceSystemMgr::GetInstanceReservedUserCount()
{
	return this->m_cInstanceCounter.GetReservedUserCounter();
}

//----- (00EB37A0) --------------------------------------------------------
int __thiscall InstanceSystemMgr::GetInstanceReservedMonsterCount()
{
	return this->m_cInstanceCounter.GetReservedMonsterCounter();
}

//----- (00EB37D0) --------------------------------------------------------
void  InstanceSystemMgr::GM_SendMessageMonsterInfo(int aIndex, int nInstanceIdx, int nMonsterClass)
{

	char szMsg[260]; // [sp+50h] [bp-110h]@5

	int nMonIndex = this->m_cInstanceObject[nInstanceIdx].FindMonsterIndex(nMonsterClass);
	if (OBJECT_MONSTER_RANGE(nMonIndex) == 0)
	{
		return;
	}

		memset(&szMsg, 0, sizeof(szMsg));

		//_wsprintfA(szMsg, "---------- MonsterClass( %d ) ----------", nMonsterClass);
		//GCServerMsgStringSend(szMsg, aIndex, 1);
		//v4 = gObj[nMonIndex].m_nMonsterExpLevel;
		//j___ftol2_sse(gObj[nMonIndex].MaxLife);
		//_wsprintfA(szMsg, "     Life( %d ), Exp( %d )", v5, v4);
		//GCServerMsgStringSend(szMsg, aIndex, 1);
		//_wsprintfA(
		//	szMsg,
		//	"     Damage ( %d ~ %d ), Defense( %d )",
		//	gObj[nMonIndex].m_AttackDamageMin,
		//	gObj[nMonIndex].m_AttackDamageMax,
		//	gObj[nMonIndex].m_Defense);
		//GCServerMsgStringSend(szMsg, aIndex, 1);

}

//----- (00EB39C0) --------------------------------------------------------
int  InstanceSystemMgr::FindRegistUser( char *szName, int *nInstanceIndex, int nContentsType)
{
	//mu::ScopedLock sl; // [sp+9Ch] [bp-14h]@1

	//mu::ScopedLock::ScopedLock(&sl, (mu::Lockable *)&this->m_csInstanceSystem.vfptr);

	std::map<std::string, _INSTANCE_REG_LIST >::iterator it = this->m_mapRegisterList.find(szName);
	if (it!=this->m_mapRegisterList.end()&&it->second.nContentsType== nContentsType)
	{
		*nInstanceIndex = it->second.nInstanceIndex;
		//mu::ScopedLock::~ScopedLock(&sl);
		return 0;
	}
	//mu::ScopedLock::~ScopedLock(&sl);
	return 1;
}

//----- (00EB3B60) --------------------------------------------------------
void  InstanceSystemMgr::SetInstanceState(int nInstanceIndex, int nInstanceState)
{
	this->m_cInstanceObject[nInstanceIndex].SetState(nInstanceState);
}



//----- (00EB3BD0) --------------------------------------------------------
int  InstanceSystemMgr::AddInstanceMonster(int nInstanceIdx, int nMonsterIndex)
{
	this->m_cInstanceObject[nInstanceIdx].AddMonsterInfo(nMonsterIndex);
	return 0;
}

//----- (00EB3C10) --------------------------------------------------------
void  InstanceSystemMgr::DeleteAllInstanceMonster(int nInstanceIdx)
{
	this->m_cInstanceObject[nInstanceIdx].DeleteAllMonster();
}

//----- (00EB3C40) --------------------------------------------------------
void  InstanceSystemMgr::DeleteAllInstanceMapItem(int nInstanceIdx)
{
	this->m_cInstanceObject[nInstanceIdx].DeleteAllMapItem();
}

//----- (00EB3C70) --------------------------------------------------------
void  InstanceSystemMgr::StateSetInstanceMap()
{
	
	ATL::CTime *v2; // eax@4
	int v3; // esi@4
	ATL::CTime *v4; // eax@4
	ATL::CTime v21; // eax@4
	ATL::CTime v22; // eax@4
	unsigned int nElapse; // [sp+B8h] [bp-4Ch]@4
	std::string OwnerName=""; // [sp+BCh] [bp-48h]@1
	int i; // [sp+DCh] [bp-28h]@3
		//mu::ScopedLock sl; // [sp+ECh] [bp-18h]@1

	int v35; // [sp+100h] [bp-4h]@1


	//mu::ScopedLock::ScopedLock(&sl, (mu::Lockable *)&this->m_csInstanceSystem.vfptr);
	v35 = 0;
	std::map<std::string, _INSTANCE_REG_LIST>::iterator it = this->m_mapRegisterList.begin();

	while (1)
	{
	
		if (it==this->m_mapRegisterList.end())
			break;

		i = it->second.nInstanceIndex;
		OwnerName = it->first;

		if (this->m_cInstanceObject[i].GetState() == 1)
		{
			v2 = &v21.GetTickCount();
			v3 = v2->GetTime();
			v4 = this->m_cInstanceObject[i].GetAllocTime(&v22);
			nElapse = v3 - v4->GetTime();;
			if (nElapse <= 0x12C || this->m_cInstanceObject[i].GetCurrentUserCount() > 1)
			{
				it++;
			}
			else
			{

				LogAdd(LOG_BLACK,
					"[INSTANCE] [INFO] Expired, reserved instance obj time. this instance obj delete.[%s](%d)[Contents:%d](%d)",
					this->m_cInstanceObject[i].GetOwnerUserName(),
					this->m_cInstanceObject[i].GetOwnerUserIndex(),
					this->m_cInstanceObject[i].GetContentsType(),
					i);

				this->m_cInstanceCounter.DecInstanceCounter(this->m_cInstanceObject[i].GetMaxUserCount(), this->m_cInstanceObject[i].GetMaxMonsterCount());
				this->m_cInstanceObject[i].ClearInfo();
				it = this->m_mapRegisterList.erase(it);
				it++;
			}
		}
		else if (this->m_cInstanceObject[i].GetState() == 4)
		{

			this->m_cInstanceObject[i].m_lpMapCT->StateSetDestroy();

			if (!this->m_cInstanceObject[i].GetCurrentUserCount())
			{


				LogAdd(LOG_BLACK,
					"[INSTANCE][ERROR] Invalid State!! - play user count zero. [%s](contents:%d)(index:%d)(RU:%d)(RM:%d)",
					OwnerName.c_str(),
					it->second.nContentsType,
					it->second.nInstanceIndex,
					this->m_cInstanceObject[i].GetMaxUserCount(),
					this->m_cInstanceObject[i].GetMaxMonsterCount());
				if ((signed int)this->m_cInstanceObject[i].GetMaxMonsterCount() > 0
					|| (signed int)this->m_cInstanceObject[i].GetMaxUserCount() > 0)
				{

					this->m_cInstanceCounter.DecInstanceCounter(this->m_cInstanceObject[i].GetMaxUserCount(), this->m_cInstanceObject[i].GetMaxMonsterCount());
				}
				this->m_cInstanceObject[i].ClearInfo();
				it = this->m_mapRegisterList.erase(it);

			}
			it++;
		}
		else
		{
			it++;
		}
	}

	//mu::ScopedLock::~ScopedLock(&sl);
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
