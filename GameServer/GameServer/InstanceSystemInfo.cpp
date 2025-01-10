#include "stdafx.h"
#include "InstanceSystemInfo.h"
#include "MemScript.h"
#include "Util.h"

InstanceSystemInfo g_InstanceSystemInfo;
InstanceSystemInfo::InstanceSystemInfo()
{
	this->m_mapInstanceSystemInfo.clear();
}


InstanceSystemInfo::~InstanceSystemInfo()
{
}



//----- (00EB1A80) --------------------------------------------------------
void  InstanceSystemInfo::LoadInstanceSystemScript(char *lpszFileName)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, lpszFileName);
		return;
	}

	if (lpMemScript->SetBuffer(lpszFileName) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_mapInstanceSystemInfo.clear();

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
			_INSTANCE_SYSTEM_INFO info;

			info.nContentsType = lpMemScript->GetNumber();

			lpMemScript->GetAsString();

			info.nMapNumber = lpMemScript->GetAsNumber();

			info.nAvailableUser = lpMemScript->GetAsNumber();

			info.nAvailableMonster = lpMemScript->GetAsNumber();

			info.nInstanceMapBaseValue = lpMemScript->GetAsNumber();

			this->m_mapInstanceSystemInfo.insert(std::pair<int, _INSTANCE_SYSTEM_INFO>(info.nContentsType, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}





//----- (00EB1DD0) --------------------------------------------------------
int  InstanceSystemInfo::GetInstanceSystemInfo(int nContentsType, _INSTANCE_SYSTEM_INFO *stInstanceSystemInfo)
{
	std::map<int, _INSTANCE_SYSTEM_INFO>::iterator it = this->m_mapInstanceSystemInfo.find(nContentsType);
	if (it!= this->m_mapInstanceSystemInfo.end())
	{
		stInstanceSystemInfo->nContentsType = it->second.nContentsType;
		stInstanceSystemInfo->nMapNumber = it->second.nMapNumber;
		stInstanceSystemInfo->nAvailableUser = it->second.nAvailableUser;
		stInstanceSystemInfo->nAvailableMonster = it->second.nAvailableMonster;
		stInstanceSystemInfo->nInstanceMapBaseValue = it->second.nInstanceMapBaseValue;
		return 0;
	}
	return 11;

}

//----- (00EB1F20) --------------------------------------------------------
int  InstanceSystemInfo::GetMinMaxInstanceMapNumber(int *nMinMapValue, int *nMaxMapValue)
{

	/*std::map<int, _INSTANCE_SYSTEM_INFO>::iterator Min = std::min_element( this->m_mapInstanceSystemInfo.begin(), this->m_mapInstanceSystemInfo.end());
	std::map<int, _INSTANCE_SYSTEM_INFO>::iterator Max = std::max_element(this->m_mapInstanceSystemInfo.begin(), this->m_mapInstanceSystemInfo.end());
	std::map<int, _INSTANCE_SYSTEM_INFO>::iterator it = this->m_mapInstanceSystemInfo.end();
	if (this->m_mapInstanceSystemInfo.begin()== this->m_mapInstanceSystemInfo.end())
	{
		Min = this->m_mapInstanceSystemInfo.end();
	}
	else
	{
		std::map<int, _INSTANCE_SYSTEM_INFO>::iterator smallest = this->m_mapInstanceSystemInfo.begin();

	}
	if (Min== this->m_mapInstanceSystemInfo.end())
	{
		LogAdd(LOG_BLACK,"[INSTANCE][ERROR] GetMinMaxInstanceMapNumber, not found min map number");
		return 11;
	}
	else
	{
		if (Max == this->m_mapInstanceSystemInfo.end())
		{
			LogAdd(LOG_BLACK,"[INSTANCE][ERROR] GetMinMaxInstanceMapNumber, not found max map number");
			return 11;
		}
		else
		{
			*nMinMapValue = Min->second.nInstanceMapBaseValue;
			*nMaxMapValue = Max->second.nInstanceMapBaseValue;
		}
	}*/
	return 0;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EB2240) --------------------------------------------------------
int  InstanceSystemInfo::ResolutionInstanceIndex(unsigned __int16 wMapNumber, int *nInstanceIdx)
{


	for (std::map<int, _INSTANCE_SYSTEM_INFO>::iterator it= this->m_mapInstanceSystemInfo.begin(); it!= this->m_mapInstanceSystemInfo.end(); it++)
	{
		if (it == this->m_mapInstanceSystemInfo.end())
		{

			LogAdd(LOG_BLACK, "[INSTANCE][ERROR] Failed, Resolution MapNum -> InstanceIndex. (%d)", wMapNumber);
			return 11;
		}
		else
		{
			*nInstanceIdx = wMapNumber - it->second.nInstanceMapBaseValue;
			break;
		}


	}
	
	return 0;
}
