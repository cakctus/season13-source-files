#include "stdafx.h"
#include "ItemDrop.h"
#include "BonusManager.h"
#include "CrywolfSync.h"
#include "DSProtocol.h"
#include "ItemManager.h"
#include "ItemOptionRate.h"
#include "MemScript.h"
#include "Monster.h"
#include "RandomManager.h"
#include "Util.h"
#include "Log.h"
#include "ServerInfo.h"
#include "MapManager.h"

CItemDrop gItemDrop;

CItemDrop::CItemDrop() // OK
{
	this->m_ItemDropInfo.clear();
	this->isLoaded = false;
}

CItemDrop::~CItemDrop() // OK
{

}

void CItemDrop::Load(char* path) // OK
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

	this->m_ItemDropInfo.clear();

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

			ITEM_DROP_INFO info;

			info.Index = lpMemScript->GetNumber();
			info.Level = lpMemScript->GetAsNumber();
			info.Grade = lpMemScript->GetAsNumber();
			info.Option0 = lpMemScript->GetAsNumber();
			info.Option1 = lpMemScript->GetAsNumber();
			info.Option2 = lpMemScript->GetAsNumber();
			info.Option3 = lpMemScript->GetAsNumber();
			info.Option4 = lpMemScript->GetAsNumber();
			info.Option5 = lpMemScript->GetAsNumber();
			info.Option6 = lpMemScript->GetAsNumber();
			info.Duration = lpMemScript->GetAsNumber();
			info.MapNumber = lpMemScript->GetAsNumber();
			info.MonsterClass = lpMemScript->GetAsNumber();
			info.MonsterLevelMin = lpMemScript->GetAsNumber();
			info.MonsterLevelMax = lpMemScript->GetAsNumber();
			info.DropRate = lpMemScript->GetAsNumber();

			/*LogAdd(LOG_DEBUG, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
				info.Index,info.TimerRandom,info.Grade,info.Option0,info.Skill,info.Option2,info.Option3,
				info.Option4,info.Option5,info.Option6,info.Duration,info.MapNumber,info.MonsterClass,
				info.MonsterLevelMin,info.MonsterLevelMax,info.DropRate);*/

			this->m_ItemDropInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	this->isLoaded = true;

	delete lpMemScript;
}

int CItemDrop::DropItem(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if (this->isLoaded == false)
	{
		return 0;
	}

	CRandomManager RandomManager;

	for(std::vector<ITEM_DROP_INFO>::iterator it=this->m_ItemDropInfo.begin();it != this->m_ItemDropInfo.end();it++)
	{
		if (this->isLoaded == false)
		{
			return 0;
		}

		int DropRate;

		ITEM_INFO ItemInfo;

		if(gItemManager.GetInfo(it->Index,&ItemInfo) == 0)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != lpObj->Map)
		{
			continue;
		}

		if(it->MonsterClass != -1 && it->MonsterClass != lpObj->Class)
		{
			continue;
		}

		if(it->MonsterLevelMin != -1 && it->MonsterLevelMin > lpObj->Level)
		{
			continue;
		}

		if(it->MonsterLevelMax != -1 && it->MonsterLevelMax < lpObj->Level)
		{
			continue;
		}

		if (it->Index == GET_ITEM(12, 446))
		{
			int MaxValue = gItemManager.GetInventoryMaxValue(lpTarget);
			bool CanDrop = false;
			int ItemAmount = 0;

			for (int n = INVENTORY_WEAR_SIZE; n < MaxValue; n++)
			{
				if (lpTarget->Inventory[n].IsItem() != 0)
				{
					if (lpTarget->Inventory[n].m_Index == GET_ITEM(12, 407))
					{
						ItemAmount += lpTarget->Inventory[n].m_Durability;
						CanDrop = true;
					}

					if (lpTarget->Inventory[n].m_Index == GET_ITEM(12, 446))
					{
						ItemAmount += lpTarget->Inventory[n].m_Durability;
					}
				}
			}

			if (CanDrop == false || ItemAmount >= 255)
				continue;
		}

		if (it->Index == GET_ITEM(12, 447))
		{
			int MaxValue = gItemManager.GetInventoryMaxValue(lpTarget);
			bool CanDrop = false;
			int ItemAmount = 0;

			for (int n = INVENTORY_WEAR_SIZE; n < MaxValue; n++)
			{
				if (lpTarget->Inventory[n].IsItem() != 0)
				{
					if (lpTarget->Inventory[n].m_Index == GET_ITEM(12, 409))
					{
						ItemAmount += lpTarget->Inventory[n].m_Durability; 
						CanDrop = true;
					}

					if (lpTarget->Inventory[n].m_Index == GET_ITEM(12, 447))
					{
						ItemAmount += lpTarget->Inventory[n].m_Durability;
					}
				}
			}

			if (CanDrop == false || ItemAmount >= 255)
				continue;
		}

		DropRate = this->GetItemDropRate(lpObj, lpTarget, it->Index, it->Level, it->DropRate);

		if (DropRate > 0)
		{
			DropRate = DropRate * gServerInfo.m_ItemDropRate[lpObj->AccountLevel] / 100;
			DropRate = DropRate * (lpObj->ItemDropRate + lpObj->EffectOption.AddItemDropRate) / 100;

			/*if (it->Option4 > 0)
			{
				DropRate = (DropRate * gMapManager.GetMapPvPDamageRate(lpObj->Map)) / 100;
			}*/
		}

		DropRate = (DropRate > 1000000) ? 1000000 : DropRate;

		if(DropRate == -1 || (GetLargeRand() % 1000000) < DropRate)
		{			
			int rate = (1000000/((DropRate==-1)?1000000:DropRate));
			RandomManager.AddElement((int)(&(*it)),rate);
		}

		if (this->isLoaded == false)
		{
			return 0;
		}
	}

	if (this->isLoaded == false)
	{
		return 0;
	}

	ITEM_DROP_INFO* lpItemDropInfo;

	if(RandomManager.GetRandomElement((int*)&lpItemDropInfo) == 0)
	{
		return 0;
	}
	else
	{
		WORD ItemIndex = lpItemDropInfo->Index;
		BYTE ItemLevel = lpItemDropInfo->Level;
		BYTE ItemOption1 = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;
		BYTE ItemNewOption = 0;
		BYTE ItemSetOption = 0;
		BYTE ItemSocketOption[MAX_SOCKET_OPTION] = {0xFF,0xFF,0xFF,0xFF,0xFF};

		gItemOptionRate.GetItemOption0(lpItemDropInfo->Option0,&ItemLevel);
		gItemOptionRate.GetItemOption1(lpItemDropInfo->Option1,&ItemOption1);
		gItemOptionRate.GetItemOption2(lpItemDropInfo->Option2,&ItemOption2);
		gItemOptionRate.GetItemOption3(lpItemDropInfo->Option3,&ItemOption3);
		gItemOptionRate.GetItemOption4(lpItemDropInfo->Option4,&ItemNewOption);
		gItemOptionRate.GetItemOption5(lpItemDropInfo->Option5,&ItemSetOption);
		gItemOptionRate.GetItemOption6(lpItemDropInfo->Option6,&ItemSocketOption[0]);

		gItemOptionRate.MakeNewOption(ItemIndex,ItemNewOption,&ItemNewOption);
		gItemOptionRate.MakeSetOption(ItemIndex,ItemSetOption,&ItemSetOption);

		gItemOptionRate.MakeSocketOption(ItemIndex,ItemSocketOption[0],&ItemSocketOption[0]);

		gLog.Output(LOG_ITEM_DROP, "[ItemDrop.txt][%s][%s] [Map: %d] [Monster: %s] [Item: %s%s +%d] [Ancient: %d] [Socket: %d %d %d %d %d]", lpTarget->Account, lpTarget->Name, lpObj->Map, lpObj->MonsterName, (ItemNewOption > 0) ? "Excellent ":"" , gItemManager.GetName(ItemIndex), ItemLevel, ItemSetOption, ItemSocketOption[0], ItemSocketOption[1], ItemSocketOption[2], ItemSocketOption[3], ItemSocketOption[4]);
	
		GDCreateItemSend(lpTarget->Index,lpObj->Map,(BYTE)lpObj->X,(BYTE)lpObj->Y,ItemIndex,ItemLevel,0,ItemOption1,ItemOption2,ItemOption3,lpTarget->Index,((ItemNewOption==0)?lpItemDropInfo->Grade:ItemNewOption),ItemSetOption,0,0,ItemSocketOption,0xFF,((lpItemDropInfo->Duration>0)?((DWORD)time(0)+lpItemDropInfo->Duration):0));

		return 1;
	}
}

int CItemDrop::GetItemDropRate(LPOBJ lpObj,LPOBJ lpTarget,int ItemIndex,int ItemLevel,int DropRate) // OK
{
	/*if (ItemIndex == GET_ITEM(12, 25) || ItemIndex == GET_ITEM(14, 13) || ItemIndex == GET_ITEM(14, 14) || ItemIndex == GET_ITEM(14, 16) || ItemIndex == GET_ITEM(14, 22) || ItemIndex == GET_ITEM(14, 31))
	{
		if(gCrywolfSync.CheckApplyPenalty() != 0 && gCrywolfSync.GetOccupationState() == 1)
		{
			if((GetLargeRand()%100) >= gCrywolfSync.GetGemDropPenaltiyRate())
			{
				return 0;
			}
		}
	}*/

	return gBonusManager.GetBonusValue(lpTarget,BONUS_INDEX_CMN_ITEM_DROP_RATE,DropRate,ItemIndex,ItemLevel,lpObj->Class,lpObj->Level);
}
