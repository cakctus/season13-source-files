#include "stdafx.h"
#include "ItemBagManager.h"
#include "MemScript.h"
#include "Path.h"
#include "Util.h"

CItemBagManager gItemBagManager;

CItemBagManager::CItemBagManager() // OK
{
	this->m_ItemBagManagerInfo.clear();
}

CItemBagManager::~CItemBagManager() // OK
{
}

void CItemBagManager::Load(char* path) // OK
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

	this->m_ItemBagManagerInfo.clear();

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

			ITEM_BAG_MANAGER_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.ItemIndex = SafeGetItem(GET_ITEM(lpMemScript->GetAsNumber(), lpMemScript->GetAsNumber()));

			info.ItemLevel = lpMemScript->GetAsNumber();

			info.MonsterClass = lpMemScript->GetAsNumber();

			info.SpecialValue = lpMemScript->GetAsNumber();

			//LogAdd(LOG_BLUE, "Item bag loaded %d", info.Index);

			this->m_ItemBagManagerInfo.insert(std::pair<int,ITEM_BAG_MANAGER_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CItemBagManager::LoadEventItemBag() // OK
{
	std::map<int,int> LoadEventItemBag;

	char wildcard_path[MAX_PATH];

	wsprintf(wildcard_path,"%s*",gPath.GetRightPath("EventItemBag\\"));

	WIN32_FIND_DATA data;

	HANDLE file = FindFirstFile(wildcard_path,&data);

	if(file == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if(isdigit(data.cFileName[0]) != 0 && isdigit(data.cFileName[1]) != 0 && isdigit(data.cFileName[2]) != 0)
			{
				if(data.cFileName[3] == ' ' && data.cFileName[4] == '-' && data.cFileName[5] == ' ')
				{
					std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.find(atoi(data.cFileName));

					if(it != this->m_ItemBagManagerInfo.end())
					{
						if(LoadEventItemBag.find(it->first) == LoadEventItemBag.end())
						{
							char path[MAX_PATH];

							wsprintf(path,"EventItemBag\\%s",data.cFileName);

							it->second.ItemBag.Load(gPath.GetRightPath(path));

							LoadEventItemBag.insert(std::pair<int,int>(it->first,1));
						}
					}
				}
			}
		}
	}
	while(FindNextFile(file,&data) != 0);
}

bool CItemBagManager::GetItemByItemIndex(int ItemIndex,int ItemLevel,LPOBJ lpObj,CItem* lpItem) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.ItemIndex != -1 && it->second.ItemIndex == ItemIndex && (it->second.ItemLevel == -1 || it->second.ItemLevel == ItemLevel))
		{
			return it->second.ItemBag.GetItem(lpObj,lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::GetItemByMonsterClass(int MonsterClass,LPOBJ lpObj,CItem* lpItem) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.MonsterClass != -1 && it->second.MonsterClass == MonsterClass)
		{
			return it->second.ItemBag.GetItem(lpObj,lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::GetItemBySpecialValue(int SpecialValue,LPOBJ lpObj,CItem* lpItem) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.SpecialValue != -1 && it->second.SpecialValue == SpecialValue)
		{
			return it->second.ItemBag.GetItem(lpObj,lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemByItemIndex(int ItemIndex,int ItemLevel,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.ItemIndex != -1 && it->second.ItemIndex == ItemIndex && (it->second.ItemLevel == -1 || it->second.ItemLevel == ItemLevel))
		{
			return it->second.ItemBag.DropItem(lpObj,map,x,y);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemByMonsterClass(LPOBJ lpMonster,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.MonsterClass != -1 && it->second.MonsterClass == lpMonster->Class)
		{
			return it->second.ItemBag.DropItem(lpObj,map,x,y,lpMonster->Index);
		}
	}

	return 0;
}

bool CItemBagManager::GremoryCaseDropItemBySpecialValue(int SpecialValue,BYTE btStorageType,BYTE btRewardSource,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.SpecialValue != -1 && it->second.SpecialValue == SpecialValue)
		{
			return it->second.ItemBag.GremoryCaseDropItem(lpObj,map,x,y,btStorageType,btRewardSource,30);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemBySpecialValue(int SpecialValue,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::map<int,ITEM_BAG_MANAGER_INFO>::iterator it=this->m_ItemBagManagerInfo.begin();it != this->m_ItemBagManagerInfo.end();it++)
	{
		if(it->second.SpecialValue != -1 && it->second.SpecialValue == SpecialValue)
		{
			return it->second.ItemBag.DropItem(lpObj,map,x,y);
		}
	}

	return 0;
}
