#include"stdafx.h"
#include "Util.h"
#include"mmItemSlot.h"
#include "MemScript.h"
#include "Message.h"

GItemSlot gGItemSlot;

GItemSlot::GItemSlot()
{
}

GItemSlot::~GItemSlot()
{
}

void GItemSlot::Load(char* path)
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
	this->m_ItemLocation.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					_ITEM_SLOT info;
					info.MinItemIndex = lpMemScript->GetNumber();
					info.MaxItemIndex = lpMemScript->GetAsNumber();
					info.ItemSlot = lpMemScript->GetAsNumber();
					this->m_ItemLocation.push_back(info);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

BOOL GItemSlot::GetItemByLocation(int Index)
{
	for (std::vector<_ITEM_SLOT>::iterator it = this->m_ItemLocation.begin(); it != this->m_ItemLocation.end(); it++)
	{
		if(Index >= it->MinItemIndex && Index <= it->MaxItemIndex)
		{
			return 1;
		}
	}

	return 0;
}

BOOL GItemSlot::GetItemBySlot(int Index, int Slot)
{
	for (std::vector<_ITEM_SLOT>::iterator it = this->m_ItemLocation.begin(); it != this->m_ItemLocation.end(); it++)
	{
		if ((Index >= it->MinItemIndex && Index <= it->MaxItemIndex) && Slot == it->ItemSlot)
		{
			return 1;
		}
	}

	return 0;
}