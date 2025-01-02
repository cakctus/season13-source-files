#include "stdafx.h"
#include "CustomJewel.h"
#include "CustomWing.h"
#include "ItemManager.h"
#include "MemScript.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "SetItemType.h"
#include "SocketItemType.h"
#include "Util.h"
#include "PentagramSystem.h"
#include "Notice.h"
#include "RheaGold.h"

CCustomJewel gCustomJewel;

CCustomJewel::CCustomJewel() // OK
{
	this->Init();
}

CCustomJewel::~CCustomJewel() // OK
{

}

void CCustomJewel::Init() // OK
{
	for(int n=0;n < MAX_CUSTOM_JEWEL;n++)
	{
		this->m_CustomJewelInfo[n].Index = -1;
	}
}

void CCustomJewel::Load(char* path) // OK
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

	this->Init();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}
		
			int section = lpMemScript->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					CUSTOM_JEWEL_INFO info;

					memset(&info,0,sizeof(info));

					info.Index = lpMemScript->GetNumber();
					info.ItemIndex = lpMemScript->GetAsNumber();
					info.MinItemLevel = lpMemScript->GetAsNumber();
					info.MaxItemLevel = lpMemScript->GetAsNumber();
					info.MaxItemSkill = lpMemScript->GetAsNumber();
					info.MaxItemLuck = lpMemScript->GetAsNumber();
					info.MinItemAdd = lpMemScript->GetAsNumber();
					info.MaxItemAdd = lpMemScript->GetAsNumber();
					info.MinItemExcellent = lpMemScript->GetAsNumber();
					info.MaxItemExcellent = lpMemScript->GetAsNumber();
					info.MaxItemSetOption = lpMemScript->GetAsNumber();
					info.MinItemSocketOption = lpMemScript->GetAsNumber();
					info.MaxItemSocketOption = lpMemScript->GetAsNumber();
					info.EnableSlotWeapon = lpMemScript->GetAsNumber();
					info.EnableSlotArmor = lpMemScript->GetAsNumber();
					info.EnableSlotWing = lpMemScript->GetAsNumber();
					info.EnableSlot4Wing = lpMemScript->GetAsNumber();
					info.EnableSlotAcessories = lpMemScript->GetAsNumber();
					info.EnableErrtel = lpMemScript->GetAsNumber();
					info.EnablePentagrams = lpMemScript->GetAsNumber();

					info.SuccessRate[0] = lpMemScript->GetAsNumber();
					info.SuccessRate[1] = lpMemScript->GetAsNumber();
					info.SuccessRate[2] = lpMemScript->GetAsNumber();
					info.SuccessRate[3] = lpMemScript->GetAsNumber();

					this->SetInfo(info);
				}
				else if(section == 1)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					CUSTOM_JEWEL_SUCCESS_INFO info;

					memset(&info,0,sizeof(info));

					info.Index = lpMemScript->GetNumber();
					info.Level = lpMemScript->GetAsNumber();
					info.Skill = lpMemScript->GetAsNumber();
					info.Luck = lpMemScript->GetAsNumber();
					info.AddOption = lpMemScript->GetAsNumber();
					info.Excellent = lpMemScript->GetAsNumber();
					info.Ancient = lpMemScript->GetAsNumber();
					info.SocketOption = lpMemScript->GetAsNumber();
					info.ErrtelLevel = lpMemScript->GetAsNumber();
					info.ErrtelRank = lpMemScript->GetAsNumber();
					info.Wing4thElement = lpMemScript->GetAsNumber();
					info.Wing4thOption = lpMemScript->GetAsNumber();
					info.AllFull = lpMemScript->GetAsNumber();

					this->SetSuccessInfo(info);
				}
				else if(section == 2)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					CUSTOM_JEWEL_FAILURE_INFO info;

					memset(&info,0,sizeof(info));

					info.Index = lpMemScript->GetNumber();
					info.Level = lpMemScript->GetAsNumber();
					info.Option1 = lpMemScript->GetAsNumber();
					info.Option2 = lpMemScript->GetAsNumber();
					info.Option3 = lpMemScript->GetAsNumber();
					info.NewOption = lpMemScript->GetAsNumber();
					info.SetOption = lpMemScript->GetAsNumber();
					info.SocketOption = lpMemScript->GetAsNumber();
					info.ErrtelLevel = lpMemScript->GetAsNumber();
					info.ErrtelRank = lpMemScript->GetAsNumber();
					info.Wing4thElement = lpMemScript->GetAsNumber();
					info.Wing4thOption = lpMemScript->GetAsNumber();
					info.AllFull = lpMemScript->GetAsNumber();

					this->SetFailureInfo(info);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CCustomJewel::SetInfo(CUSTOM_JEWEL_INFO info) // OK
{
	if(info.Index < 0 || info.Index >= MAX_CUSTOM_JEWEL)
	{
		return;
	}

	this->m_CustomJewelInfo[info.Index] = info;
}

void CCustomJewel::SetSuccessInfo(CUSTOM_JEWEL_SUCCESS_INFO info) // OK
{
	if(info.Index < 0 || info.Index >= MAX_CUSTOM_JEWEL)
	{
		return;
	}

	this->m_CustomJewelInfo[info.Index].SuccessInfo = info;
}

void CCustomJewel::SetFailureInfo(CUSTOM_JEWEL_FAILURE_INFO info) // OK
{
	if(info.Index < 0 || info.Index >= MAX_CUSTOM_JEWEL)
	{
		return;
	}

	this->m_CustomJewelInfo[info.Index].FailureInfo = info;
}

CUSTOM_JEWEL_INFO* CCustomJewel::GetInfo(int index) // OK
{
	if(index < 0 || index >= MAX_CUSTOM_JEWEL)
	{
		return 0;
	}

	if(this->m_CustomJewelInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_CustomJewelInfo[index];
}

CUSTOM_JEWEL_INFO* CCustomJewel::GetInfoByItem(int ItemIndex) // OK
{
	for(int n=0;n < MAX_CUSTOM_JEWEL;n++)
	{
		CUSTOM_JEWEL_INFO* lpInfo = this->GetInfo(n);

		if(lpInfo == 0)
		{
			continue;
		}

		if(lpInfo->ItemIndex == ItemIndex)
		{
			return lpInfo;
		}
	}

	return 0;
}

CUSTOM_JEWEL_SUCCESS_INFO* CCustomJewel::GetSuccessInfo(int ItemIndex) // OK
{
	CUSTOM_JEWEL_INFO* lpInfo = this->GetInfoByItem(ItemIndex);

	if(lpInfo == 0)
	{
		return 0;
	}

	return &lpInfo->SuccessInfo;
}

CUSTOM_JEWEL_FAILURE_INFO* CCustomJewel::GetFailureInfo(int ItemIndex) // OK
{
	CUSTOM_JEWEL_INFO* lpInfo = this->GetInfoByItem(ItemIndex);

	if(lpInfo == 0)
	{
		return 0;
	}

	return &lpInfo->FailureInfo;
}

bool CCustomJewel::CheckCustomJewel(int index) // OK
{
	if(this->GetInfo(index) != 0)
	{
		return 1;
	}

	return 0;
}

bool CCustomJewel::CheckCustomJewelByItem(int ItemIndex) // OK
{
	if(this->GetInfoByItem(ItemIndex) != 0)
	{
		return 1;
	}

	return 0;
}

bool CCustomJewel::CheckCustomJewelApplyItem(int ItemIndex,CItem* lpItem) // OK
{
	CUSTOM_JEWEL_INFO* lpInfo = this->GetInfoByItem(ItemIndex);

	if(lpInfo == 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if(lpItem->IsLuckyItem() != 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if(lpItem->IsJewelOfHarmonyItem() != 0 && (lpInfo->MinItemSocketOption > 0 || lpInfo->MaxItemSocketOption > 0))
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	bool validUse = false;

	if (lpInfo->EnableSlotWeapon == 1 && (lpItem->m_Index >= GET_ITEM(0, 0) && lpItem->m_Index < GET_ITEM(7, 0)))
	{
		validUse = true;
	}

	if (lpInfo->EnableSlotArmor == 1 && (lpItem->m_Index >= GET_ITEM(7, 0) && lpItem->m_Index < GET_ITEM(12, 0)))
	{
		validUse = true;
	}

	if (lpInfo->EnableSlotWing == 1 && ((lpItem->m_Index >= GET_ITEM(12, 0) && lpItem->m_Index <= GET_ITEM(12, 6)) || (lpItem->m_Index >= GET_ITEM(12, 36) && lpItem->m_Index <= GET_ITEM(12, 43)) || lpItem->m_Index == GET_ITEM(12, 49) || lpItem->m_Index == GET_ITEM(12, 50) || (lpItem->m_Index >= GET_ITEM(12, 262) && lpItem->m_Index <= GET_ITEM(12, 270)) || lpItem->m_Index == GET_ITEM(13, 30) || gCustomWing.CheckCustomWingByItem(lpItem->m_Index) != 0))
	{
		validUse = true;
	}

	if (lpInfo->EnableSlot4Wing == 1 && (lpItem->m_Index >= GET_ITEM(12, 414) && lpItem->m_Index <= GET_ITEM(12, 421)))
	{
		validUse = true;
	}

	if (lpInfo->EnableSlotAcessories == 1 && 
		(lpItem->m_Index == GET_ITEM(13, 8) ||
			lpItem->m_Index == GET_ITEM(13, 9) ||
			lpItem->m_Index == GET_ITEM(13, 12) ||
			lpItem->m_Index == GET_ITEM(13, 13) ||
			(lpItem->m_Index >= GET_ITEM(13, 21) && lpItem->m_Index <= GET_ITEM(13, 28)) ||
			(lpItem->m_Index >= GET_ITEM(13, 171) && lpItem->m_Index <= GET_ITEM(13, 176)) 
		))
	{
		validUse = true;
	}

	if (lpInfo->EnableErrtel == 1 && lpItem->IsPentagramJewel())
	{
		validUse = true;
	}

	if (lpInfo->EnablePentagrams == 1 && lpItem->m_Index != GET_ITEM(12,215) && lpItem->IsPentagramItem())
	{
		validUse = true;
	}

	if (!validUse)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->MinItemLevel != -1 && lpItem->m_Level < lpInfo->MinItemLevel)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->MaxItemLevel != -1 && lpItem->m_Level > lpInfo->MaxItemLevel)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->MaxItemSkill != -1)
	{
		if (lpItem->m_Option1 > 0)
		{
			//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
			return 0;
		}

		if (gItemManager.GetItemSkill(lpItem->m_Index) <= 0)
		{
			//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
			return 0;
		}
	}

	if (lpInfo->MaxItemLuck != -1 && lpItem->m_Option2 > lpInfo->MaxItemLuck)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->MinItemAdd != -1 && lpItem->m_Option3 < lpInfo->MinItemAdd)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->MaxItemAdd != -1 && lpItem->m_Option3 > lpInfo->MaxItemAdd)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	/*if (lpInfo->SuccessInfo.SocketOption < 1 &&
		this->CheckCustomJewelApplyItemNewOption(lpInfo, lpItem) == 0 &&
		lpInfo->SuccessInfo.Luck < 1 && lpInfo->SuccessInfo.TimerRandom < 1)
	{
		LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}*/

	if (lpInfo->SuccessInfo.Excellent > 0 && this->CheckCustomJewelApplyItemNewOption(lpInfo, lpItem) == 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->SuccessInfo.Ancient > 0 && this->CheckCustomJewelApplyItemSetOption(lpInfo, lpItem) == 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (this->CheckCustomJewelApplyItemSocketOption(lpInfo, lpItem) == 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->SuccessInfo.ErrtelRank > 0 && this->CheckCustomJewelApplyErrtelRank(lpInfo, lpItem) == 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->SuccessInfo.ErrtelLevel > 0 && this->CheckCustomJewelApplyErrtelLevel(lpInfo, lpItem) == 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->SuccessInfo.Wing4thElement > 0 && this->CheckCustomJewelApply4thWingElement(lpInfo, lpItem) == 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	if (lpInfo->SuccessInfo.Wing4thOption > 0 && this->CheckCustomJewelApply4thWingOption(lpInfo, lpItem) == 0)
	{
		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);
		return 0;
	}

	return 1;
} 

bool CCustomJewel::CheckCustomJewelApplyItemNewOption(CUSTOM_JEWEL_INFO* lpInfo, CItem* lpItem) // OK
{
	if (gSocketItemType.CheckSocketItemType(lpItem->m_Index) != 0)
	{
		return 0;
	}

	if (lpInfo->MinItemExcellent != -1 && GetNewOptionCount(lpItem->m_NewOption) < lpInfo->MinItemExcellent)
	{
		return 0;
	}

	if (lpInfo->MaxItemExcellent != -1 && lpInfo->MaxItemExcellent > 10 && (lpItem->m_NewOption & (lpInfo->MaxItemExcellent - 10)) != 0)
	{
		return 0;
	}

	if (lpInfo->MaxItemExcellent != -1 && lpInfo->MaxItemExcellent < 10 && GetNewOptionCount(lpItem->m_NewOption) > lpInfo->MaxItemExcellent)
	{
		return 0;
	}

	return 1;
}

bool CCustomJewel::CheckCustomJewelApplyItemSetOption(CUSTOM_JEWEL_INFO* lpInfo, CItem* lpItem) // OK
{	
	if ((lpItem->m_Index >= GET_ITEM(7, 98) && lpItem->m_Index <= GET_ITEM(7, 115)) || //Blood Angel/Soul Helm
		(lpItem->m_Index >= GET_ITEM(7, 138) && lpItem->m_Index <= GET_ITEM(7, 145)) || //Dark Angel Helm
		(lpItem->m_Index >= GET_ITEM(7, 150) && lpItem->m_Index <= GET_ITEM(7, 163)) || //Holy Angel Helm
		(lpItem->m_Index >= GET_ITEM(8, 98) && lpItem->m_Index <= GET_ITEM(8, 115)) || //Blood Angel/Soul Armor
		(lpItem->m_Index >= GET_ITEM(8, 138) && lpItem->m_Index <= GET_ITEM(8, 145)) || //Dark Angel Armor
		(lpItem->m_Index >= GET_ITEM(8, 150) && lpItem->m_Index <= GET_ITEM(8, 163)) || //Holy Angel Armor
		(lpItem->m_Index >= GET_ITEM(9, 98) && lpItem->m_Index <= GET_ITEM(9, 115)) || //Blood Angel/Soul Pants
		(lpItem->m_Index >= GET_ITEM(9, 138) && lpItem->m_Index <= GET_ITEM(9, 145)) || //Dark Angel Pants
		(lpItem->m_Index >= GET_ITEM(9, 150) && lpItem->m_Index <= GET_ITEM(9, 163)) || //Holy Angel Pants
		(lpItem->m_Index >= GET_ITEM(10, 98) && lpItem->m_Index <= GET_ITEM(10, 115)) || //Blood Angel/Soul Gloves
		(lpItem->m_Index >= GET_ITEM(10, 138) && lpItem->m_Index <= GET_ITEM(10, 145)) || //Dark Angel Gloves
		(lpItem->m_Index >= GET_ITEM(10, 150) && lpItem->m_Index <= GET_ITEM(10, 163)) || //Holy Angel Gloves
		(lpItem->m_Index >= GET_ITEM(11, 98) && lpItem->m_Index <= GET_ITEM(11, 115)) || //Blood Angel/Soul Boots
		(lpItem->m_Index >= GET_ITEM(11, 138) && lpItem->m_Index <= GET_ITEM(11, 145)) || //Dark Angel Boots
		(lpItem->m_Index >= GET_ITEM(11, 150) && lpItem->m_Index <= GET_ITEM(11, 163)) //Holy Angel Boots
		)
	{
		return 0;
	}

	if (lpItem->IsSetItem() == 0)
	{
		if (gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 0) > 0 || gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 1) > 0 || gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 2) > 0)
		{
			return 1;
		}
	}
	else
	{
		BYTE SetIndex = lpItem->m_SetOption & 3;

		if (SetIndex == 1 && (gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 1) > 0 || gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 2) > 0))
		{
			return 1;
		}

		if (SetIndex == 2 && (gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 0) > 0 || gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 2) > 0))
		{
			return 1;
		}

		if (SetIndex == 3 && (gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 0) > 0 || gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 1) > 0))
		{
			return 1;
		}
	}	

	return 0;
}

bool CCustomJewel::CheckCustomJewelApplyItemSocketOption(CUSTOM_JEWEL_INFO* lpInfo, CItem* lpItem) // OK
{
	if (lpInfo->MaxItemSocketOption != -1 &&
		((lpInfo->MaxItemSocketOption == 0 && gSocketItemType.CheckSocketItemType(lpItem->m_Index) != 0) ||
			(lpInfo->MaxItemSocketOption != 0 && ((gSocketItemType.CheckSocketItemType(lpItem->m_Index) == 0 && lpItem->IsPentagramItem() == 0) || GetSocketOptionCount(lpItem->m_SocketOption) > lpInfo->MaxItemSocketOption))))
	{
		return 0;
	}

	return 1;
}

bool CCustomJewel::CheckCustomJewelApplyErrtelRank(CUSTOM_JEWEL_INFO* lpInfo, CItem* lpItem) // OK
{
	/*for (int l = 0; l < 5; l++)
	{
		if (lpItem->m_SocketOption[l] == 0xFF)
		{
			return 1;
		}
	}*/

	if (((lpItem->m_SocketOptionBonus / 16)) >= MAX_PENTAGRAM_JEWEL_RANK)
	{
		//LogAdd(LOG_DEBUG, "lpItem->m_SocketOptionBonus: %d", lpItem->m_SocketOptionBonus);
		return 0;
	}

	return 1;
}

bool CCustomJewel::CheckCustomJewelApplyErrtelLevel(CUSTOM_JEWEL_INFO* lpInfo, CItem* lpItem) // OK
{
	for (int l = 0; l < 5; l++)
	{
		if (lpItem->m_SocketOption[l] != 0xFF)
		{
			if ((lpItem->m_SocketOption[l] / 16) < 10)
			{
				return 1;
			}
		}
	}

	return 0;
}

bool CCustomJewel::CheckCustomJewelApply4thWingElement(CUSTOM_JEWEL_INFO* lpInfo, CItem* lpItem) // OK
{
	if (lpItem->m_Index < GET_ITEM(12, 414) || lpItem->m_Index > GET_ITEM(12, 421))
	{
		return 0;
	}

	/*if (lpItem->m_SocketOptionBonus < 15)
	{
		return 1;
	}
		
	if (lpItem->m_SocketOption[4] == 0xFF)
	{
		return 1;
	}

	if (lpItem->m_SocketOption[4] % 16 < 15)
	{
		return 1;
	}*/

	return 1;
}

bool CCustomJewel::CheckCustomJewelApply4thWingOption(CUSTOM_JEWEL_INFO* lpInfo, CItem* lpItem) // OK
{
	if (lpItem->m_Index < GET_ITEM(12, 414) || lpItem->m_Index > GET_ITEM(12, 421))
	{
		return 0;
	}

	for (int l = 0; l < 4; l++)
	{
		if (lpItem->m_SocketOption[l] != 0xFF)
		{
			if ((lpItem->m_SocketOption[l] % 16) < 9)
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}
	}

	return 0;
}

int CCustomJewel::GetCustomJewelSuccessRate(int ItemIndex,int AccountLevel) // OK
{
	CUSTOM_JEWEL_INFO* lpInfo = this->GetInfoByItem(ItemIndex);

	if(lpInfo == 0)
	{
		return 0;
	}

	return lpInfo->SuccessRate[AccountLevel];
}

int CCustomJewel::GetCustomJewelSalePrice(int ItemIndex) // OK
{
	CUSTOM_JEWEL_INFO* lpInfo = this->GetInfoByItem(ItemIndex);

	if(lpInfo == 0)
	{
		return 0;
	}

	return 0;// lpInfo->SalePrice;
}

int CCustomJewel::GetCustomJewelNewOption(CItem* lpItem,int value) // OK
{
	if(value >= 10)
	{
		return (lpItem->m_NewOption=(lpItem->m_NewOption|(value-10)));
	}

	int count = 0;

	int MaxNewOption = MAX_EXC_OPTION;

	if(lpItem->m_Index == GET_ITEM(12,269) || (lpItem->m_Index >= GET_ITEM(12,3) && lpItem->m_Index <= GET_ITEM(12,6)) || lpItem->m_Index == GET_ITEM(12,42) || lpItem->m_Index == GET_ITEM(12,49) || lpItem->m_Index == GET_ITEM(13,30)) // 2nd Wings
	{
		MaxNewOption = 4;// ((lpItem->m_Index == GET_ITEM(13, 30)) ? 3 : 4);
	}

	if(lpItem->m_Index == GET_ITEM(12,270)||
		(lpItem->m_Index >= GET_ITEM(12,36) && lpItem->m_Index <= GET_ITEM(12,40)) || lpItem->m_Index == GET_ITEM(12,43) || lpItem->m_Index == GET_ITEM(12,50)) // 3rd Wings
	{
		MaxNewOption = 4;
	}

	if(lpItem->m_Index >= GET_ITEM(12,262) && lpItem->m_Index <= GET_ITEM(12,265)) // Monster Wings
	{
		MaxNewOption = 2;
	}

	if(lpItem->m_Index == GET_ITEM(12,266)) // Wings of Conqueror
	{
		MaxNewOption = 3;
	}

	if(lpItem->m_Index == GET_ITEM(12,267)) // Wings of Angel and Devil
	{
		MaxNewOption = 3;
	}

	if(lpItem->m_Index == GET_ITEM(13,3)) // Dinorant
	{
		MaxNewOption = 3;
	}

	if(lpItem->m_Index == GET_ITEM(13,37)) // Fenrir
	{
		MaxNewOption = 3;
	}

	while(true)
	{
		if(count >= value || GetNewOptionCount(lpItem->m_NewOption) >= MaxNewOption)
		{
			break;
		}

		int option = 1 << (GetLargeRand()%MaxNewOption);

		if((lpItem->m_NewOption & option) == 0)
		{
			lpItem->m_NewOption |= option;
			count++;
		}
	}

	return count;
}

int CCustomJewel::GetCustomJewelSetOption(CItem* lpItem,int value) // OK
{
	BYTE SetIndex = lpItem->m_SetOption & 3;
	BYTE SetValue = lpItem->m_SetOption & 12;

	if (SetValue == 0)
	{
		SetValue = 4;
	}
	else if (SetValue == 4 && (GetLargeRand() % 100) < 50)
	{
		SetValue = 8;
	}

	if(SetIndex == 0)
	{
		for(int n=0;n < MAX_SET_ITEM_OPTION_INDEX;n++)
		{
			if(gSetItemType.GetSetItemOptionIndex(lpItem->m_Index,n) > 0)
			{
				SetIndex = n + 1;
				break;
			}
		}
	}
	else if (SetIndex == 1)
	{
		if (gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 1) > 0)
		{
			SetIndex = 2;
		}
		else
		{
			SetIndex = 16;
		}
	}
	else if (SetIndex == 2)
	{
		if (gSetItemType.GetSetItemOptionIndex(lpItem->m_Index, 2) > 0)
		{
			SetIndex = 16;
		}
		else
		{
			SetIndex = 1;
		}
	}

	return ((lpItem->m_SetOption)=(SetIndex+SetValue));
}

int CCustomJewel::GetCustomJewelSocketOption(CItem* lpItem,int value) // OK
{
	int count = 0;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		if(count < value && lpItem->m_SocketOption[n] == 0xFF)
		{
			lpItem->m_SocketOption[n] = 0xFE;
			count++;
		}
	}

	return count;
}

int CCustomJewel::UseJewelOfSphereUpgrade(LPOBJ lpObj, int SourceSlot, int TargetSlot)
{
	if (INVENTORY_FULL_RANGE(SourceSlot) == 0)
	{
		return 0;
	}

	if (INVENTORY_FULL_RANGE(TargetSlot) == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[SourceSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[TargetSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[TargetSlot].m_IsPeriodicItem != 0)
	{
		return 0;
	}

	if (gItemManager.InventoryAddItemStack(lpObj, SourceSlot, TargetSlot) != 0)
	{
		return 0;
	}

	if (
		(lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 100) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 129)) ||
		(lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 311) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 334))
		)
	{
		int ruud = 0;

		if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 100) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 105))
		{
			ruud = 4000;
		}
		else if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 106) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 111))
		{
			ruud = 9000;
		}
		else if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 112) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 117))
		{
			ruud = 14000;
		}
		else if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 118) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 123))
		{
			ruud = 19000;
		}
		else if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 124) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 129))
		{
			ruud = 25000;
		}
		else if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 311) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 316))
		{
			ruud = 32000;
		}
		else if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 317) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 322))
		{
			ruud = 40000;
		}
		else if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 323) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 328))
		{
			ruud = 49000;
		}
		else if (lpObj->Inventory[TargetSlot].m_Index >= GET_ITEM(12, 329) && lpObj->Inventory[TargetSlot].m_Index <= GET_ITEM(12, 334))
		{
			ruud = 59000;
		}

		if (lpObj->Ruud < ruud)
		{
			GCMessagePopupSend(lpObj, "Voce nao tem o Ruud necessario.\nSao necessarios %d Ruud para elevar esta Sphere ao proximo nivel.", ruud);
			return 0;
		}

		CItem* lpItem = &lpObj->Inventory[TargetSlot];

		lpItem->m_Index += (lpItem->m_Index >= GET_ITEM(12, 124) && lpItem->m_Index <= GET_ITEM(12, 129)) ? 187 : 6;

		lpObj->Ruud -= ruud;
		gRheaGold.GC_Notify_RuudUpdate(lpObj->Index, lpObj->Ruud, ruud, 0);

		return 1;
	}
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Esta joia nao se aplica a este item.");
		return 0;
	}
}