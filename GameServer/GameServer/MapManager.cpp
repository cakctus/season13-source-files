#include "stdafx.h"
#include "MapManager.h"
#include "MemScript.h"
#include "ServerInfo.h"
#include "Util.h"

CMapManager gMapManager;

CMapManager::CMapManager() // OK
{
	this->m_MapManagerInfo.clear();
	this->isLoaded = false;
}

CMapManager::~CMapManager() // OK
{

}

void CMapManager::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->isLoaded = false;
	this->m_MapManagerInfo.clear();	

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			MAP_MANAGER_INFO info;

			memset(&info,0,sizeof(info));

			info.Index			= lpMemScript->GetNumber();
			info.NonPK			= lpMemScript->GetAsNumber();
			info.ViewRange		= lpMemScript->GetAsNumber();
			info.ExperienceRate = lpMemScript->GetAsNumber();
			info.ItemDropRate	= lpMemScript->GetAsNumber();
			info.PvPDamageRate	= lpMemScript->GetAsNumber();
			info.SetItemDropRate= lpMemScript->GetAsNumber();
			info.SocketItemDrop = lpMemScript->GetAsNumber();
			info.HelperEnable	= lpMemScript->GetAsNumber();
			info.GensBattle		= lpMemScript->GetAsNumber();

			this->m_MapManagerInfo.insert(std::pair<int,MAP_MANAGER_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	this->isLoaded = true;

	delete lpMemScript;
}

void CMapManager::LoadPvPZone(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->isPvPZoneLoaded = false;
	this->m_PvPZone.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			PVPZONE_DATA info;

			memset(&info, 0, sizeof(info));

			info.MapNumber	= lpMemScript->GetNumber();
			info.X1			= lpMemScript->GetAsNumber();
			info.Y1			= lpMemScript->GetAsNumber();
			info.X2			= lpMemScript->GetAsNumber();
			info.Y2			= lpMemScript->GetAsNumber();

			this->m_PvPZone.insert(std::pair<int, PVPZONE_DATA>(info.MapNumber, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	this->isPvPZoneLoaded = true;

	delete lpMemScript;
}

int CMapManager::GetMapNonPK(int index) // OK
{
	if (!this->isLoaded)
	{
		return gServerInfo.m_NonPK;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return gServerInfo.m_NonPK;
	}
	else
	{
		return ((it->second.NonPK==-1)?gServerInfo.m_NonPK:((it->second.NonPK==1)?1:0));
	}
}

int CMapManager::GetPvPZone(LPOBJ lpObj) // OK
{
	if (!this->isPvPZoneLoaded)
	{
		return gServerInfo.m_NonPK;
	}

	for (std::map<int, PVPZONE_DATA>::iterator it = this->m_PvPZone.begin(); it != this->m_PvPZone.end(); it++)
	{
		if (it->second.MapNumber == lpObj->Map &&
			lpObj->X >= it->second.X1 && lpObj->X <= it->second.X2 &&
			lpObj->Y >= it->second.Y1 && lpObj->Y <= it->second.Y2)
		{
			return 0;
		}
	}

	return gServerInfo.m_NonPK;
}







int CMapManager::GetMapNonOutlaw(int index) // OK
{
	if (!this->isLoaded)
	{
		return 0;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.NonPK==-1)?0:((it->second.NonPK==2)?1:0));
	}
}

int CMapManager::GetMapViewRange(int index) // OK
{
	if (!this->isLoaded)
	{
		return 15;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 15;
	}
	else
	{
		return ((it->second.ViewRange==-1)?15:it->second.ViewRange);
	}
}

int CMapManager::GetMapExperienceRate(int index) // OK
{
	if (!this->isLoaded)
	{
		return 100;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 100;
	}
	else
	{
		return ((it->second.ExperienceRate==-1)?100:it->second.ExperienceRate);
	}
}

int CMapManager::GetMapMasterExperienceRate(int index) // OK
{
	if (!this->isLoaded)
	{
		return 100;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 100;
	}
	else
	{
		return ((it->second.ExperienceRate==-1)?100:it->second.ExperienceRate);
	}
}

int CMapManager::GetMapItemDropRate(int index) // OK
{
	if (!this->isLoaded)
	{
		return 100;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 100;
	}
	else
	{
		return ((it->second.ItemDropRate==-1)?100:it->second.ItemDropRate);
	}
}

int CMapManager::GetMapPvPDamageRate(int index) // OK
{
	if (!this->isLoaded)
	{
		return 0;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.PvPDamageRate==-1)?0:it->second.PvPDamageRate);
	}
}

int CMapManager::GetMapSetItemDropRate(int index) // OK
{
	if (!this->isLoaded)
	{
		return 0;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.SetItemDropRate==-1)?0:it->second.SetItemDropRate);
	}
}

int CMapManager::GetMapSocketItemDrop(int index) // OK
{
	if (!this->isLoaded)
	{
		return 0;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.SocketItemDrop==-1)?0:it->second.SocketItemDrop);
	}
}

int CMapManager::GetMapHelperEnable(int index) // OK
{
	if (!this->isLoaded)
	{
		return 0;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.HelperEnable==-1)?0:it->second.HelperEnable);
	}
}

int CMapManager::GetMapGensBattle(int index) // OK
{
	if (!this->isLoaded)
	{
		return 0;
	}

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.GensBattle==-1)?0:it->second.GensBattle);
	}
}
