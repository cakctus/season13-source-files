#include "stdafx.h"
#include "Path.h"
#include "Map.h"
#include "MapPath.h"
#include "InstanceObject.h"

#include "Util.h"
#include "Viewport.h"
InstanceObject::InstanceObject()
{
	this->m_mapUserInfo.clear();
	this->m_vecMonsterInfo.clear();
	this->m_nState = 0;
	this->m_nContentsType = 0;
	this->m_nOwnerIndex = -1;
	memset(this->m_szOwnerName, 0, sizeof(m_szOwnerName));
	this->m_nMaxUserCount = 0;
	this->m_nMaxMonsterCount = 0;
	this->m_lpMapCT = gMap;

}


InstanceObject::~InstanceObject()
{
	this->DeleteAllMonster();
	this->DeleteAllMapItem();

}


//----- (00EB0930) --------------------------------------------------------
void __thiscall InstanceObject::ClearInfo()
{

	ATL::CTime v5; // [sp+Ch] [bp-4Ch]@1

	this->m_nState = 0;
	this->m_nContentsType = 0;
	this->m_nOwnerIndex = -1;
	memset(this->m_szOwnerName, 0, sizeof(this->m_szOwnerName));
	this->m_nMaxUserCount = 0;
	this->m_nMaxMonsterCount = 0;
	this->DeleteAllMonster();
	this->DeleteAllMapItem();
	this->m_vecMonsterInfo.clear();
	this->m_mapUserInfo.clear();

}

//----- (00EB09F0) --------------------------------------------------------
BYTE  InstanceObject::SetOwnerUser(int nOwnerUserIndex, const char *szOwnerUserName)
{

	if (gObjIsConnected(nOwnerUserIndex) == 0)
	{
		LogAdd(LOG_BLACK, "[INSTANCE][FATAL] Out of range, owner user index value.(%d)", nOwnerUserIndex);
		return 0;
	}

	if (szOwnerUserName)
	{
		if (this->m_nOwnerIndex != -1)
			LogAdd(LOG_BLACK,"[INSTANCE][WARN] Already exist owner user info.(%d)(%s)", this->m_nOwnerIndex, this->m_szOwnerName);
		this->m_nOwnerIndex = nOwnerUserIndex;
		strcpy_s(this->m_szOwnerName, sizeof(this->m_szOwnerName), szOwnerUserName);
		return 1;
	}
	else
	{
		LogAdd(LOG_BLACK,"[INSTANCE][FATAL] Owner user name argument is null.");
		return 0;
	}

	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB0AB0) --------------------------------------------------------
int  InstanceObject::AddUserInfo(int nUserIndex, const char *szUserName)
{


	if (gObjIsConnected(nUserIndex) == 0)
	{
		LogAdd(LOG_BLACK, "[INSTANCE][FATAL] Out of range, user index value.(%d) - AddUser", nUserIndex);
		return 1;
	}

	if (!szUserName)
	{
		LogAdd(LOG_BLACK,"[INSTANCE][FATAL] User name argument is null. - AddUser");
		return 2;
	}
	std::string strUserName= szUserName;
	
	std::map<std::string, int>::iterator it = this->m_mapUserInfo.find(strUserName);
	if (it!= this->m_mapUserInfo.end())
	{
		if (nUserIndex==it->second)
		{
			LogAdd(LOG_BLACK, "[INSTANCE][WARN] Already in user info(map). (%d)(%s)", nUserIndex, szUserName);
			return 3;
		}
		
		LogAdd(LOG_BLACK, "[INSTANCE][WARN] Already in user, can't match user index.(%d -> %d)(%s)", it->second, nUserIndex, it->first.c_str());
		it->second = nUserIndex;
	}

	else
	{
		this->m_mapUserInfo.insert(std::pair<std::string, int>(strUserName, nUserIndex));
	}

	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB0DD0) --------------------------------------------------------
int  InstanceObject::AddMonsterInfo(int nMonsterIndex)
{
	if (OBJECT_MONSTER_RANGE(nMonsterIndex) == 0)
	{
		LogAdd(LOG_BLACK, "[INSTANCE][FATAL] Monster index is invalid.(%d) - AddMonster", nMonsterIndex);
		return 4;
	}
	if (this->m_vecMonsterInfo.empty() != 1)
	{
		int *iB = &(*this->m_vecMonsterInfo.begin());
		int *iE = &(*this->m_vecMonsterInfo.end());

		std::vector<int>::iterator _Itor = std::find(this->m_vecMonsterInfo.begin(), this->m_vecMonsterInfo.end(), nMonsterIndex);
		if (_Itor!=this->m_vecMonsterInfo.end())
		{
			LogAdd(LOG_BLACK, "[INSTANCE][FATAL] monster index, Already in.(%d)", nMonsterIndex);
			return 5;
		}
	}
	this->m_vecMonsterInfo.push_back(nMonsterIndex);
	LogAdd(LOG_BLACK,"[INSTANCE][DEBUG] Add instance monster.(%d)", nMonsterIndex);
	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB0FD0) --------------------------------------------------------
int __thiscall InstanceObject::DeleteMonsterInfo(int nMonsterIndex)
{
	
	bool bFind = 0;
	std::vector<int>::iterator it = this->m_vecMonsterInfo.begin();

	while (1)
	{
		if (it==this->m_vecMonsterInfo.end())
		{
			break;
		}
		if (*it==nMonsterIndex)
		{
			this->m_vecMonsterInfo.erase(it);
			bFind = 1;
			break;
		}
		it++;
	}
	if (bFind)
	{

		LogAdd(LOG_BLACK,"[INSTANCE][DEBUG] Delete instance monster.(Idx:%d)", nMonsterIndex);
	}
	else
	{

		LogAdd(LOG_BLACK,"[INSTANCE][DEBUG] Can't found instance monster info.(Idx:%d)", nMonsterIndex);
	}

	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB1170) --------------------------------------------------------
int  InstanceObject::DeleteAllMonster()
{

	LogAdd(LOG_BLACK,"[INSTANCE][DEBUG] Delete all instance monster.(cur mon cnt:%d)", this->m_vecMonsterInfo.size());

	for (int i = 0; ; ++i)
	{
		if (i >= this->m_vecMonsterInfo.size())
			break;

		LogAdd(LOG_BLACK,"[INSTANCE][DEBUG] Delete monster.(%d)", this->m_vecMonsterInfo.at(i));

		gObjViewportListProtocolDestroy(&gObj[this->m_vecMonsterInfo.at(i)]);

		gViewport.gObjViewportClose(&gObj[this->m_vecMonsterInfo.at(i)]);

		gObjDel(this->m_vecMonsterInfo.at(i));
	}
	this->m_vecMonsterInfo.clear();

	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB13C0) --------------------------------------------------------
int  InstanceObject::DeleteAllMapItem()
{

	LogAdd(LOG_BLACK,"[INSTANCE][INFO] Delete all instance map item, dropped.");
	if (this->m_lpMapCT)
	{
		this->m_lpMapCT->StateSetDestroy();

	}

	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB1410) --------------------------------------------------------
int  InstanceObject::SetAttrInstanceData(int nMapAttrValue, BYTE btLTPointX, BYTE btLTPointY, BYTE btRBPointX, BYTE btRBPointY)
{
	int result; // eax@2
	int y; // [sp+4Ch] [bp-Ch]@15
	BYTE btAXIS_X_LENG; // [sp+53h] [bp-5h]@13

	LogAdd(LOG_BLACK,
		"[INSTANCE][DEBUG] SetAttrInstanceData. (Attr:%d, LT(%d, %d) - RB(%d, %d)",
		nMapAttrValue,
		btLTPointX,
		btLTPointY,
		btRBPointX,
		btRBPointY);
	if (this->m_lpMapCT)
	{
		if (btLTPointX <= 256 &&btLTPointY <= 256)
		{
			if (btRBPointX <= 256 &&btRBPointY <= 256)
			{
				if (btRBPointX >=btLTPointX)
				{
					if (btRBPointY >= btLTPointY)
					{
						btAXIS_X_LENG = btRBPointX - btLTPointX;
						if (btRBPointX == btLTPointX)
							btAXIS_X_LENG = 1;
						for (y = btLTPointY; y <= btRBPointY; ++y)
							this->SetAttrInstanceData(
								nMapAttrValue,
								btLTPointX + (btLTPointY << 8),
								btAXIS_X_LENG);
						return 0;
					}
					else
					{
						LogAdd(LOG_BLACK,"[INSTANCE][FATAL] Left, right Y value is invalid.(LT:%d, RB:%d)",btLTPointY,btRBPointY);
						return 7;
					}
				}
				else
				{
					LogAdd(LOG_BLACK,"[INSTANCE][FATAL] Left, right X value is invalid.(LT:%d, RB:%d)",btLTPointX,btRBPointX);
					return 7;
				}
			}
			else
			{
				LogAdd(LOG_BLACK,"[INSTANCE][FATAL] Right bottom point coord is invalid.(%d, %d)",btRBPointX,btRBPointY);
				return 7;
			}
		}
		else
		{
			LogAdd(LOG_BLACK,"[INSTANCE][FATAL] Left top point coord is invalid.(%d, %d)",btLTPointX,btLTPointY);
			return 7;
		}
	}
	else
	{
		LogAdd(LOG_BLACK,"[INSTANCE][FATAL] Instance map data is null.");
		return 6;
	}

}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB15F0) --------------------------------------------------------
int  InstanceObject::SetAttrInstanceData(int nMapAttrValue, int nAttrBeginPos, int nLength)
{
	memset(&this->m_lpMapCT->m_MapAttr[nAttrBeginPos], nMapAttrValue, nLength);
	return 0;
}

//----- (00EB1640) --------------------------------------------------------
void  InstanceObject::SetAllocTime(ATL::CTime *cAllocTime)
{

	this->m_cAllocTime = cAllocTime->GetTime();

}

//----- (00EB1670) --------------------------------------------------------
ATL::CTime *__thiscall InstanceObject::GetAllocTime(ATL::CTime *result)
{

	//int v2 = this->m_cAllocTime.GetTime();
	//result = (ATL::CTime *)v2;
	//return result;
	return &this->m_cAllocTime;
}

//----- (00EB16B0) --------------------------------------------------------
int  InstanceObject::RegisterInstanceObject(int aIndex, const char *szName, int nContentsType, unsigned __int16 nAvailableUserCount, unsigned __int16 nAvailableMonsterCount)
{

	ATL::CTime result; // [sp+Ch] [bp-4Ch]@1

	this->m_nState = 1;
	this->m_nContentsType = nContentsType;
	this->m_nOwnerIndex = aIndex;
	strcpy_s(this->m_szOwnerName, sizeof(this->m_szOwnerName), szName);
	this->m_nMaxUserCount = nAvailableUserCount;
	this->m_nMaxMonsterCount = nAvailableMonsterCount;
	this->AddUserInfo(aIndex, szName);
	this->m_cAllocTime = result.GetTickCount();;

	LogAdd(LOG_BLACK,
		"[INSTANCE] Register instance object by %s(%d), ContentsType:%d, (RU %d / RM %d)",
		szName,
		aIndex,
		nContentsType,
		nAvailableUserCount,
		nAvailableMonsterCount);
	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB1790) --------------------------------------------------------
void  InstanceObject::MakeInstanceMapInfo(int nContentsType,int index)
{


	if (nContentsType == 1)
	{
		//v2 = CDirPath::GetNewPath(&gDirPath, "terrain115.att");
		//MapClassType::ReLoadMapAttr(thisa->m_lpMapCT, v2, 0x73u);
		//if (fopen_s(&file, gPath.GetFullPath(temp), "r") == 0)
		//{
		//	strcpy_s(path, temp);
		//	fclose(file);
		//}
		this->m_lpMapCT[500+ index].ReLoadMapAttr(gPath.GetFullPath("Terrain\\terrain116.att"), 115);
		//this->m_lpMapCT->ReLoadMapAttr(gPath.GetFullPath("Terrain\\terrain116.att"), 115);
	
	}
}

//----- (00EB17E0) --------------------------------------------------------
unsigned int  InstanceObject::GetCurrentUserCount()
{
	return this->m_mapUserInfo.size();
}

//----- (00EB1810) --------------------------------------------------------
int __thiscall InstanceObject::FindMonsterIndex(int nClass)
{

	for (int i = 0; i < this->m_vecMonsterInfo.size(); ++i)
	{
		if (this->m_vecMonsterInfo[i] >= 0
			&& gObj[this->m_vecMonsterInfo[i]].Class == nClass)
		{
			return gObj[this->m_vecMonsterInfo[i]].Index;
		}
	}
	return -1;
}

//----- (00EB36F0) --------------------------------------------------------
int  InstanceObject::GetState()
{
	return this->m_nState;
}
//----- (00EB3BA0) --------------------------------------------------------
void  InstanceObject::SetState(int nState)
{
	this->m_nState = nState;
}
//----- (00EB4200) --------------------------------------------------------
int  InstanceObject::GetContentsType()
{
	return this->m_nContentsType;
}

//----- (00EB4220) --------------------------------------------------------
int  InstanceObject::GetOwnerUserIndex()
{
	return this->m_nOwnerIndex;
}

//----- (00EB4240) --------------------------------------------------------
char * InstanceObject::GetOwnerUserName()
{
	return this->m_szOwnerName;
}

//----- (00EB4260) --------------------------------------------------------
unsigned __int16  InstanceObject::GetMaxUserCount()
{
	return this->m_nMaxUserCount;
}

//----- (00EB4280) --------------------------------------------------------
unsigned __int16  InstanceObject::GetMaxMonsterCount()
{
	return this->m_nMaxMonsterCount;
}