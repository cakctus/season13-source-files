#include "stdafx.h"
#include "MemScript.h"
#include "ObjectManager.h"
#include "MasterSkillTree_4th.h"
#include "GameMain.h"
#include "DSProtocol.h"
#include "Util.h"
#include "IpManager.h"
#include "CalFormula.h"
#include "ServerInfo.h"

MasterSkillTree_4th gMasterSkillTree_4th;

MasterSkillTree_4th::MasterSkillTree_4th()
{
	this->m_ForMasterSkillTreeInfo.clear();
}

MasterSkillTree_4th::~MasterSkillTree_4th()
{
}

void MasterSkillTree_4th::Load(char* path) // OK
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

	this->m_ForMasterSkillTreeInfo.clear();

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

					FOR_MASTER_SKILL_INFO info;
					info.Class = section;
					
					info.m_ForMasterSkillData.Line = lpMemScript->GetNumber();

					info.m_ForMasterSkillData.Group = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.UIgroup = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.unk = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.SkillID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.BuffIndex = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.AttributeType = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.FormulaDateID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot1 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot2 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot3 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot4 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot5 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.NeedPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.MaxPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.ActivateNextSlotPoint = lpMemScript->GetAsNumber();

					for (int i = 0; i < MAX_PARENT_SKILL; i++)
					{
						info.m_ForMasterSkillData.ParentSkill[i] = lpMemScript->GetAsNumber();

						info.m_ForMasterSkillData.ParentSkillNeedPoint[i] = lpMemScript->GetAsNumber();
					}

					this->m_ForMasterSkillTreeInfo.push_back(info);

				}
				else if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					FOR_MASTER_SKILL_INFO info;
					info.Class = section;

					info.m_ForMasterSkillData.Line = lpMemScript->GetNumber();

					info.m_ForMasterSkillData.Group = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.UIgroup = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.unk = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.SkillID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.BuffIndex = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.AttributeType = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.FormulaDateID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot1 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot2 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot3 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot4 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot5 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.NeedPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.MaxPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.ActivateNextSlotPoint = lpMemScript->GetAsNumber();

					for (int i = 0; i < MAX_PARENT_SKILL; i++)
					{
						info.m_ForMasterSkillData.ParentSkill[i] = lpMemScript->GetAsNumber();

						info.m_ForMasterSkillData.ParentSkillNeedPoint[i] = lpMemScript->GetAsNumber();
					}

					this->m_ForMasterSkillTreeInfo.push_back(info);
				}
				else if (section == 2)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					FOR_MASTER_SKILL_INFO info;
					info.Class = section;

					info.m_ForMasterSkillData.Line = lpMemScript->GetNumber();

					info.m_ForMasterSkillData.Group = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.UIgroup = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.unk = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.SkillID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.BuffIndex = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.AttributeType = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.FormulaDateID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot1 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot2 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot3 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot4 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot5 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.NeedPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.MaxPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.ActivateNextSlotPoint = lpMemScript->GetAsNumber();

					for (int i = 0; i < MAX_PARENT_SKILL; i++)
					{
						info.m_ForMasterSkillData.ParentSkill[i] = lpMemScript->GetAsNumber();

						info.m_ForMasterSkillData.ParentSkillNeedPoint[i] = lpMemScript->GetAsNumber();
					}

					this->m_ForMasterSkillTreeInfo.push_back(info);
				}
				else if (section == 3)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					FOR_MASTER_SKILL_INFO info;
					info.Class = section;

					info.m_ForMasterSkillData.Line = lpMemScript->GetNumber();

					info.m_ForMasterSkillData.Group = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.UIgroup = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.unk = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.SkillID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.BuffIndex = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.AttributeType = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.FormulaDateID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot1 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot2 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot3 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot4 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot5 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.NeedPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.MaxPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.ActivateNextSlotPoint = lpMemScript->GetAsNumber();

					for (int i = 0; i < MAX_PARENT_SKILL; i++)
					{
						info.m_ForMasterSkillData.ParentSkill[i] = lpMemScript->GetAsNumber();

						info.m_ForMasterSkillData.ParentSkillNeedPoint[i] = lpMemScript->GetAsNumber();
					}

					this->m_ForMasterSkillTreeInfo.push_back(info);
				}
				else if (section == 4)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					FOR_MASTER_SKILL_INFO info;
					info.Class = section;

					info.m_ForMasterSkillData.Line = lpMemScript->GetNumber();

					info.m_ForMasterSkillData.Group = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.UIgroup = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.unk = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.SkillID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.BuffIndex = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.AttributeType = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.FormulaDateID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot1 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot2 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot3 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot4 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot5 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.NeedPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.MaxPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.ActivateNextSlotPoint = lpMemScript->GetAsNumber();

					for (int i = 0; i < MAX_PARENT_SKILL; i++)
					{
						info.m_ForMasterSkillData.ParentSkill[i] = lpMemScript->GetAsNumber();

						info.m_ForMasterSkillData.ParentSkillNeedPoint[i] = lpMemScript->GetAsNumber();
					}

					this->m_ForMasterSkillTreeInfo.push_back(info);
				}
				else if (section == 5)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					FOR_MASTER_SKILL_INFO info;
					info.Class = section;

					info.m_ForMasterSkillData.Line = lpMemScript->GetNumber();

					info.m_ForMasterSkillData.Group = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.UIgroup = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.unk = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.SkillID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.BuffIndex = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.AttributeType = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.FormulaDateID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot1 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot2 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot3 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot4 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot5 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.NeedPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.MaxPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.ActivateNextSlotPoint = lpMemScript->GetAsNumber();

					for (int i = 0; i < MAX_PARENT_SKILL; i++)
					{
						info.m_ForMasterSkillData.ParentSkill[i] = lpMemScript->GetAsNumber();

						info.m_ForMasterSkillData.ParentSkillNeedPoint[i] = lpMemScript->GetAsNumber();
					}

					this->m_ForMasterSkillTreeInfo.push_back(info);
				}
				else if (section == 6)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					FOR_MASTER_SKILL_INFO info;
					info.Class = section;

					info.m_ForMasterSkillData.Line = lpMemScript->GetNumber();

					info.m_ForMasterSkillData.Group = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.UIgroup = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.unk = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.SkillID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.BuffIndex = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.AttributeType = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.FormulaDateID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot1 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot2 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot3 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot4 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot5 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.NeedPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.MaxPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.ActivateNextSlotPoint = lpMemScript->GetAsNumber();

					for (int i = 0; i < MAX_PARENT_SKILL; i++)
					{
						info.m_ForMasterSkillData.ParentSkill[i] = lpMemScript->GetAsNumber();

						info.m_ForMasterSkillData.ParentSkillNeedPoint[i] = lpMemScript->GetAsNumber();
					}

					this->m_ForMasterSkillTreeInfo.push_back(info);
				}
				else if (section == 7)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					FOR_MASTER_SKILL_INFO info;
					info.Class = section;

					info.m_ForMasterSkillData.Line = lpMemScript->GetNumber();

					info.m_ForMasterSkillData.Group = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.UIgroup = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.unk = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.SkillID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.BuffIndex = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.AttributeType = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.FormulaDateID = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot1 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot2 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot3 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot4 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.LinkSlot5 = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.NeedPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.MaxPoint = lpMemScript->GetAsNumber();

					info.m_ForMasterSkillData.ActivateNextSlotPoint = lpMemScript->GetAsNumber();

					for (int i = 0; i < MAX_PARENT_SKILL; i++)
					{
						info.m_ForMasterSkillData.ParentSkill[i] = lpMemScript->GetAsNumber();

						info.m_ForMasterSkillData.ParentSkillNeedPoint[i] = lpMemScript->GetAsNumber();
					}
					

					this->m_ForMasterSkillTreeInfo.push_back(info);
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

void MasterSkillTree_4th::GCSkillTreeEnchancePointSend(LPOBJ lpObj) // OK
{
	PMSG_SKILL_ENHANCE_TREE_POINT_SEND pMsg;
	pMsg.header.set(0x7E, 0x03, sizeof(pMsg));
	pMsg.SkillEnhanceTreePoint = lpObj->SkillEnhanceTreePoints;
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void MasterSkillTree_4th::GDForMasterSkillTreeSend(int aIndex) // OK
{
	if (gObjIsAccountValid(aIndex, gObj[aIndex].Account) == 0)
	{
		return;
	}

	if (gObj[aIndex].ForLoadMasterLevel != 0)
	{
		return;
	}

	SDHP_FOR_MASTER_SKILL_TREE_SEND pMsg;
	pMsg.header.set(0x7E, 0x00, sizeof(pMsg));
	pMsg.index = aIndex;
	memcpy(pMsg.account, gObj[aIndex].Account, sizeof(pMsg.account));
	memcpy(pMsg.name, gObj[aIndex].Name, sizeof(pMsg.name));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void MasterSkillTree_4th::GDForMasterSkillTreeSaveSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckForMaster(lpObj) == 0)
	{
		return;
	}

	if (lpObj->ForLoadMasterLevel == 0)
	{
		return;
	}

	SDHP_FOR_MASTER_SKILL_TREE_SAVE_SEND pMsg;

	pMsg.header.set(0x7E, 0x01, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	pMsg.SkillEnhanceTreePoints = lpObj->SkillEnhanceTreePoints;

	for (int n = 0; n < MAX_FOR_MASTER_SKILL_LIST; n++) { 
		gSkillManager.ForSkillByteConvert(pMsg.ForMasterSkill[n], &lpObj->ForMasterSkill[n]); 
	}

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

void MasterSkillTree_4th::DGForMasterSkillTreeRecv(SDHP_FOR_MASTER_SKILL_TREE_RECV* lpMsg) // OK
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGForMasterSkillTreeRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if (this->CheckForMaster(lpObj) == 0)
	{
		return;
	}

	if (lpObj->ForLoadMasterLevel != 0)
	{
		return;
	}

	lpObj->ForLoadMasterLevel = 1;

	lpObj->SkillEnhanceTreePoints = lpMsg->SkillEnhanceTreePoints;

	this->GCSkillTreeEnchancePointSend(lpObj);

	for (int n = 0; n < MAX_FOR_MASTER_SKILL_LIST; n++)
	{
		CSkill skill;

		lpObj->ForMasterSkill[n].Clear();

		if (gSkillManager.ConvertForSkillByte(&skill, lpMsg->ForMasterSkill[n]) != 0) 
		{ 
			gSkillManager.AddForMasterSkill(lpObj, skill.m_index, skill.m_level,skill.m_group);
		}
	}

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	this->GCForMasterInfoSend(lpObj);

	MyStruct2 pMsg;
	BYTE send[1024] = { 0 };
	pMsg.header.set(0x7E, 0x06, sizeof(pMsg));
	int size = sizeof(pMsg);
	pMsg.count = 0;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(lpObj->Index, send, size);
}

void MasterSkillTree_4th::GCForMasterInfoSend(LPOBJ lpObj) // OK
{
	PMSG_FOR_MASTER_INFO_SEND pMsg;

	BYTE send[1024];

	ZeroMemory(send, 1024);

	pMsg.header.set(0x7E, 0x02, sizeof(pMsg));

	pMsg.SkillEnhanceTreePoints = lpObj->SkillEnhanceTreePoints;

	pMsg.count = 0;

	int size = sizeof(pMsg);

	FOR_MASTER_SKILL_LIST_INFO info;

	for (int i = 0; i < MAX_FOR_MASTER_SKILL_LIST; i++)
	{
		if (lpObj->ForMasterSkill[i].IsForMasterSkill() != 0)
		{
			FOR_MASTER_SKILL_INFO ForMasterSkillInfo;

			if (this->GetInfo(lpObj->Class, lpObj->ForMasterSkill[i].m_index, lpObj->ForMasterSkill[i].m_group, &ForMasterSkillInfo) != 0)
			{
				info.Group = ForMasterSkillInfo.m_ForMasterSkillData.Group;
				info.Line = ForMasterSkillInfo.m_ForMasterSkillData.Line;
				info.level = lpObj->ForMasterSkill[i].m_level;
				info.Value = gCalFormula.CalculationFormula(ForMasterSkillInfo.m_ForMasterSkillData.FormulaDateID, lpObj->ForMasterSkill[i].m_level);
				info.NextValue = gCalFormula.CalculationFormula(ForMasterSkillInfo.m_ForMasterSkillData.FormulaDateID, (((lpObj->ForMasterSkill[i].m_level + 1) >= ForMasterSkillInfo.m_ForMasterSkillData.MaxPoint) ? lpObj->ForMasterSkill[i].m_level : (lpObj->ForMasterSkill[i].m_level + 1)));
				memcpy(&send[size], &info, sizeof(info));
				size += sizeof(info);
				pMsg.count++ ;
			}
		}
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(lpObj->Index, send, size);
}

int MasterSkillTree_4th::GetForMasterSkillParentSkill(int Class, int index)
{
	for (std::vector<FOR_MASTER_SKILL_INFO>::iterator it = this->m_ForMasterSkillTreeInfo.begin(); it != this->m_ForMasterSkillTreeInfo.end(); it++)
	{
		if (it->Class == Class && it->m_ForMasterSkillData.SkillID == index)
		{
			return it->m_ForMasterSkillData.ParentSkill[0];
		}
		if (it == this->m_ForMasterSkillTreeInfo.end())
		{
			return 0;
		}
	}
	return 0;
}

void MasterSkillTree_4th::CGMasterSkillRecv(PMSG_FOR_MASTER_SKILL_RECV* lpMsg, int aIndex) // OK
{
	return; //disabled until full implementation and test of skills

	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	FOR_MASTER_SKILL_INFO ForMasterSkillTreeInfo;

	if (this->GetInfo(lpObj->Class,lpMsg->ForMasterSkill,lpMsg->Group, &ForMasterSkillTreeInfo) == 0)
	{
		return;
	}

	if (lpObj->SkillEnhanceTreePoints < ForMasterSkillTreeInfo.m_ForMasterSkillData.NeedPoint)
	{
		return;
	}

	CSkill* lpForMasterSkill = gSkillManager.GetForMasterSkill(lpObj, ForMasterSkillTreeInfo.m_ForMasterSkillData.SkillID,lpMsg->Group);

	if (lpForMasterSkill == 0)
	{
		if (this->CheckParentSkill(lpObj, &ForMasterSkillTreeInfo)==0)
		{
			return ;
		}
		if (this->CheckForMasterSkillGroup(lpObj, lpMsg->Group, lpMsg->Line, &ForMasterSkillTreeInfo) == 0)
		{
			return;
		}
		if (this->ReplaceSkill(lpObj, ForMasterSkillTreeInfo.m_ForMasterSkillData.NeedPoint, &ForMasterSkillTreeInfo) == 0)
		{
			return;
		}

		gSkillManager.AddForMasterSkill(lpObj, ForMasterSkillTreeInfo.m_ForMasterSkillData.SkillID, ForMasterSkillTreeInfo.m_ForMasterSkillData.NeedPoint, ForMasterSkillTreeInfo.m_ForMasterSkillData.Group);
	}
	else
	{
		if ((lpForMasterSkill->m_level+1) > ForMasterSkillTreeInfo.m_ForMasterSkillData.MaxPoint)
		{
			return;
		}
		if (this->ReplaceSkill(lpObj, lpForMasterSkill->m_level + 1, &ForMasterSkillTreeInfo) == 0)
		{
			return;
		}


		lpForMasterSkill->Set1(lpObj->Index, ForMasterSkillTreeInfo.m_ForMasterSkillData.SkillID, (lpForMasterSkill->m_level + 1),ForMasterSkillTreeInfo.m_ForMasterSkillData.Group);
	}

	gSkillManager.GCSkillAddSend(lpObj->Index, gSkillManager.GetSkillSlot(lpObj, ForMasterSkillTreeInfo.m_ForMasterSkillData.SkillID), ForMasterSkillTreeInfo.m_ForMasterSkillData.SkillID, 0, 0);

	//gSkillManager.GCSkillListSend(lpObj, 0);

	lpObj->SkillEnhanceTreePoints -= ForMasterSkillTreeInfo.m_ForMasterSkillData.NeedPoint;

	this->GCAddForMasterSkill(lpObj, &ForMasterSkillTreeInfo);

	gObjectManager.CharacterCalcAttribute(aIndex);

	//this->GCMasterSkillSend(aIndex, 0, 0, lpObj->MasterPoint, lpMsg->MasterSkill, lpMsg->MasterEmpty);
	//this->GCMasterInfoSend(lpObj);
}

bool MasterSkillTree_4th::GetInfo(int Class,int index,int group, FOR_MASTER_SKILL_INFO* lpInfo) // OK
{
	for (std::vector<FOR_MASTER_SKILL_INFO>::iterator it = this->m_ForMasterSkillTreeInfo.begin(); it != this->m_ForMasterSkillTreeInfo.end(); it++)
	{
		if (it->Class == Class && it->m_ForMasterSkillData.SkillID == index && it->m_ForMasterSkillData.Group == group)
		{
			(*lpInfo).Class = it->Class ;
			(*lpInfo).m_ForMasterSkillData = it->m_ForMasterSkillData;
			return 1;
}
		if (it == this->m_ForMasterSkillTreeInfo.end())
		{
			return 0;
		}
	}
	return 0;
}

bool MasterSkillTree_4th::CheckParentSkill(LPOBJ lpObj, FOR_MASTER_SKILL_INFO*lpIofo)
{
	for (int i = 0; i < MAX_PARENT_SKILL; i++)
	{
		if (lpIofo->m_ForMasterSkillData.ParentSkill[i] != -1)
		{
			if (lpIofo->m_ForMasterSkillData.ParentSkillNeedPoint[i] == -1)
			{
				CSkill* lpSkill = gSkillManager.GetSkill(lpObj, lpIofo->m_ForMasterSkillData.ParentSkill[i]);
				if (lpSkill == 0)
					return 0;
			}
			else
			{
				CSkill* lpMasterSkill;
				if (lpIofo->m_ForMasterSkillData.ParentSkill[i] >= 300 && lpIofo->m_ForMasterSkillData.ParentSkill[i] <= 719)
				{
					lpMasterSkill = gSkillManager.GetMasterSkill(lpObj, lpIofo->m_ForMasterSkillData.ParentSkill[i]);
					if (lpMasterSkill == 0)
						return 0;
					if ((lpMasterSkill->m_level+1) < lpIofo->m_ForMasterSkillData.ParentSkillNeedPoint[i])
						return 0;
				}
				else
				{
					lpMasterSkill = gSkillManager.GetForMasterSkill(lpObj, lpIofo->m_ForMasterSkillData.ParentSkill[i],lpIofo->m_ForMasterSkillData.Group);
					if (lpMasterSkill == 0)
						return 0;
					if (lpMasterSkill->m_level < lpIofo->m_ForMasterSkillData.ParentSkillNeedPoint[i])
						return 0;
				}
			}			
		}
	}
	return 1;
}

bool MasterSkillTree_4th::GetParentSkill(LPOBJ lpObj, int Skill, FOR_MASTER_SKILL_INFO*lpIofo)
{
	for (std::vector<FOR_MASTER_SKILL_INFO>::iterator it = this->m_ForMasterSkillTreeInfo.begin(); it != this->m_ForMasterSkillTreeInfo.end(); it++)
	{
		if (it->Class == lpObj->Class)
		{
			for (int i = 0; i < MAX_PARENT_SKILL; i++)
			{
				if (it->m_ForMasterSkillData.ParentSkill[i] == Skill)
				{
					(*lpIofo).Class = it->Class;
					(*lpIofo).m_ForMasterSkillData = it->m_ForMasterSkillData;
				}
			}

			return 1;
		}
		if (it == this->m_ForMasterSkillTreeInfo.end())
		{
			return 0;
		}
	}
	return 0;
}

bool MasterSkillTree_4th::CheckForMasterSkillGroup(LPOBJ lpObj, int Group,int Line,FOR_MASTER_SKILL_INFO*lpIofo)
{
	if (lpIofo->m_ForMasterSkillData.Group == Group&&lpIofo->m_ForMasterSkillData.Line == Line)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool MasterSkillTree_4th::ReplaceSkill(LPOBJ lpObj,int level, FOR_MASTER_SKILL_INFO*lpIofo) // OK
{
	CSkill* lpSkill;
	for (int i = 0; i < MAX_PARENT_SKILL; i++)
	{
		lpSkill = gSkillManager.GetSkill(lpObj, lpIofo->m_ForMasterSkillData.ParentSkill[i]);

		if (lpSkill==NULL)
		{
			lpSkill = gSkillManager.GetSkill(lpObj, lpIofo->m_ForMasterSkillData.SkillID);
		}

		if (lpIofo->m_ForMasterSkillData.ParentSkill[i] != -1 && lpIofo->m_ForMasterSkillData.ParentSkillNeedPoint[i] == -1)
		{
			if (lpSkill == 0)
			{
				return ((lpIofo->m_ForMasterSkillData.ParentSkill[i] > 0) ? 0:1);
			}
			else
			{
				lpSkill->Set1(lpObj->Index, lpIofo->m_ForMasterSkillData.SkillID, level,lpIofo->m_ForMasterSkillData.Group);
			}
		}
		if (lpIofo->m_ForMasterSkillData.ParentSkill[i] != -1 && lpIofo->m_ForMasterSkillData.ParentSkillNeedPoint[i] != -1)
		{
			CSkill* lpMasterSkill;
			if (lpIofo->m_ForMasterSkillData.ParentSkill[i] >= 300 && lpIofo->m_ForMasterSkillData.ParentSkill[i] <= 719)
			{
				
				if (lpSkill == 0)
					return 0;
				if (lpSkill->m_index == lpIofo->m_ForMasterSkillData.ParentSkill[i] && ((lpSkill->m_level + 2) < lpIofo->m_ForMasterSkillData.ParentSkillNeedPoint[i]))
				{
					return 0;
				}
				else
				{
					lpSkill->Set1(lpObj->Index, lpIofo->m_ForMasterSkillData.SkillID, level, lpIofo->m_ForMasterSkillData.Group);
				}
					
			}
			else
			{
				lpMasterSkill = gSkillManager.GetForMasterSkill(lpObj, lpIofo->m_ForMasterSkillData.ParentSkill[i],lpIofo->m_ForMasterSkillData.Group);
				if (lpMasterSkill == 0)
					return 0;
				if (lpMasterSkill->m_level < lpIofo->m_ForMasterSkillData.ParentSkillNeedPoint[i])
					return 0;
			}
		}

		if (lpIofo->m_ForMasterSkillData.ParentSkill[i] == -1 && lpIofo->m_ForMasterSkillData.ParentSkillNeedPoint[i] == -1)
		{

		}
	}
	return 1;
}

void MasterSkillTree_4th::GCAddForMasterSkill(LPOBJ lpObj, FOR_MASTER_SKILL_INFO*lpIofo)
{
	PMSG_ADD_FOR_MASTERSKILL_SEND pMsg;
	pMsg.header.set(0x7E, 0x01, sizeof(pMsg));
	pMsg.Type = 1;
	pMsg.SkillEnhanceTreePoints = lpObj->SkillEnhanceTreePoints;
	pMsg.Group = lpIofo->m_ForMasterSkillData.Group;

	CSkill* lpForMasterSkill = gSkillManager.GetForMasterSkill(lpObj, lpIofo->m_ForMasterSkillData.SkillID, lpIofo->m_ForMasterSkillData.Group);

	pMsg.Level = lpForMasterSkill->m_level;
	pMsg.Line = lpIofo->m_ForMasterSkillData.Line;
	pMsg.Value = gCalFormula.CalculationFormula(lpIofo->m_ForMasterSkillData.FormulaDateID, lpForMasterSkill->m_level);
	pMsg.NextValue = gCalFormula.CalculationFormula(lpIofo->m_ForMasterSkillData.FormulaDateID, (((lpForMasterSkill->m_level + 1) >= lpIofo->m_ForMasterSkillData.MaxPoint) ? lpForMasterSkill->m_level : (lpForMasterSkill->m_level + 1)));

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

bool MasterSkillTree_4th::CheckForMasterSkill(LPOBJ lpObj, int index, int group)
{
	for (int n = 0; n < MAX_FOR_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->ForMasterSkill[n].IsForMasterSkill() != 0)
		{
			if (lpObj->ForMasterSkill[n].m_skill ==index && lpObj->ForMasterSkill[n].m_group ==group)
			{
				return TRUE;
			}

		}
	}
	return FALSE;
}

float MasterSkillTree_4th::GetForMasterSkillValue(LPOBJ lpObj, int index,int group) // OK
{
	FOR_MASTER_SKILL_INFO ForMasterSkillTreeInfo;

	if (this->GetInfo(lpObj->Class, index,group, &ForMasterSkillTreeInfo) == 0)
	{
		return 0;
	}

	CSkill* lpForMasterSkill = ((lpObj->Type == OBJECT_USER) ? gSkillManager.GetForMasterSkill(lpObj, index, ForMasterSkillTreeInfo.m_ForMasterSkillData.Group) : 0);

	if (lpForMasterSkill == 0)
	{
		return 0;
	}
	else
	{

		return gCalFormula.CalculationFormula(ForMasterSkillTreeInfo.m_ForMasterSkillData.FormulaDateID, lpForMasterSkill->m_level);
	}
	return 0;
}

int MasterSkillTree_4th::ForMasterSkillAddDamage(LPOBJ lpObj,int Damage, int skill)
{
	switch (skill)
	{
	case SKILL_EVIL_SPIRIT:
		if (lpObj->Class ==CLASS_DW)
		{
			Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_EVIL_SPIRIT_ENHANCED_DW, 1);
			Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		}
		if (lpObj->Class == CLASS_MG)
		{
			Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_EVIL_SPIRIT_ENHANCED_MG, 5);
			Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 5);
		}
		break;
	case SKILL_DW_METEOR_STRIKE:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_METEOR_STRIKE_ENHANCED,0);
		Damage+= (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE,0);
		break;
	case SKILL_METEOR_STORM:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_METEOR_STORM_ENHANCED, 2);
		break;
	case SKILL_DEATH_STAB:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_DEATH_STAB_ENHANCED, 0);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 0);
	case SKILL_FIRE_BLOW:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_FIRE_BLOW_ENHANCED, 1);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		break;
	case SKILL_RAGEFUL_BLOW:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_RAGEFUL_BLOW_ENHANCED, 2);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 2);
		break;
	case SKILL_TRIPLE_SHOT:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_TRIPLE_SHOT_ENHANCED, 0);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 0);
		break;
	case SKILL_FIVE_SHOT:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_MULTI_SHOT_ENHANCED, 1);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		break;
	case SKILL_FOCUS_SHOT:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_FOCUS_SHOT_ENHANCED, 2);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 2);
		break;
	case SKILL_FIRE_BLOOD:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_FIRE_BLOOD_ENHANCED, 0);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 0);
		break;
	case SKILL_ICE_BLOOD:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_ICE_BLOOD_ENHANCED, 1);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		break;
	case SKILL_FIRE_SLASH:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_FIRE_SLASH_ENHANCED, 2);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 2);
		break;
	case SKILL_DARK_BLAST:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_DARK_BLAST_ENHANCED, 3);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 3);
		break;
	case SKILL_LIGHTNING_STORM:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_GIGANTIC_STORM_ENHANCED, 4);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 4);
		break;
	case SKILL_BIRDS:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_CHAOTIC_DISEIER_ENHANCED, 0);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 0);
		break;
	case SKILL_WIND_SOUL:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WIND_SOUL_ENHANCED, 1);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		break;
	case SKILL_FIRE_BURST:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_FIRE_BURST_ENHANCED, 2);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 2);
		break;
	case SKILL_RED_STORM:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_LIGHTNING_SHOCK_ENHANCED, 0);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 0);
		break;
	case SKILL_AQUA_BEAST:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_AQUA_BEAST_ENHANCED, 1);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		break;
	case SKILL_FIRE_BEAST:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_FIRE_BEAST_ENHANCED, 1);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		break;
	case SKILL_CHAIN_DRIVER:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_CHAIN_DRIVE_ENHANCED, 0);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 0);
		break;
	case SKILL_DARK_SIDE:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_DARK_SIDE_ENHANCED, 1);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		break;
	case SKILL_DRAGON_LORE:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_DRAGON_ROAR_ENHANCED, 2);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 2);
		break;
	case SKILL_BRECHE:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_BRECHE_ENHANCED, 0);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 0);
		break;
	case SKILL_MAGIC_PIN:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_MAGIC_PIN_ENHANCED, 1);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 1);
		break;
	case SKILL_SHINING_PEAK:
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_SHINING_PEAK_ENHANCED, 2);
		Damage += (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SKILL_DAMAGE, 2);
		break;
	default:
		break;
	}
	return Damage;
}

void MasterSkillTree_4th::CalcMasterSkillTreeOption(LPOBJ lpObj, bool flag) // OK
{
	for (int n = 0; n < MAX_FOR_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->ForMasterSkill[n].IsForMasterSkill() != 0)
		{
			FOR_MASTER_SKILL_INFO ForMasterSkillTreeInfo;

			if (this->GetInfo(lpObj->Class,lpObj->ForMasterSkill[n].m_index, lpObj->ForMasterSkill[n].m_group, &ForMasterSkillTreeInfo) != 0)
			{
				this->InsertOption(lpObj, ForMasterSkillTreeInfo.m_ForMasterSkillData.SkillID, gCalFormula.CalculationFormula(ForMasterSkillTreeInfo.m_ForMasterSkillData.FormulaDateID, (lpObj->ForMasterSkill[n].m_level>= ForMasterSkillTreeInfo.m_ForMasterSkillData.MaxPoint)? ForMasterSkillTreeInfo.m_ForMasterSkillData.MaxPoint: lpObj->ForMasterSkill[n].m_level), flag, lpObj->ForMasterSkill[n].m_group);
			}
		}
	}
}

void MasterSkillTree_4th::InsertOption(LPOBJ lpObj, int index, float value, bool flag,int group) // OK
{
	switch (index)
	{
	case FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE2:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE3:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE4:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE5:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASE_THE_CHANCE_OF_ADDITIONAL_DAMAGE:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_ATTACK_SPEED:
		lpObj->PhysiSpeed+= (int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_ATTACK_SPEED, 1);
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_RANGE:
		break;
	case FOR_MASTER_SKILL_ADD_ADD_SPLASH_DAMAGE:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_DISTANCE:
		break;
	case FOR_MASTER_SKILL_ADD_TARGET_INCREASE:
		break;
	case FOR_MASTER_SKILL_ADD_BUFF_SYNERGY:
		break;
	case FOR_MASTER_SKILL_ADD_BUFF_SYNERGY1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASE_SKILL_DURATION:
		break;
	case FOR_MASTER_SKILL_ADD_IRON_DEFENSE:
		break;
	case FOR_MASTER_SKILL_ADD_REINFORCED_IRON_DEFENSE:
		break;
	case FOR_MASTER_SKILL_ADD_REDUCE_REUSE_TIME:
		break;
	case FOR_MASTER_SKILL_ADD_ELIMINATE_REUSE_TIME:
		break;
	case FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE:
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_DEATH_STAB_ENHANCED, 0) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_TRIPLE_SHOT_ENHANCED, 0) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_FIRE_BLOOD_ENHANCED, 0) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_CHAOTIC_DISEIER_ENHANCED, 0) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_CHAIN_DRIVE_ENHANCED, 0) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_BRECHE_ENHANCED, 0))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 0);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 0);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 0);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 0);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_FIRE_BLOW_ENHANCED, 1) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_MULTI_SHOT_ENHANCED, 1) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_ICE_BLOOD_ENHANCED, 1) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_WIND_SOUL_ENHANCED, 1) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_DARK_SIDE_ENHANCED, 1) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_MAGIC_PIN_ENHANCED, 1))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 1);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 1);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 1);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 1);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_RAGEFUL_BLOW_ENHANCED, 2) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_FOCUS_SHOT_ENHANCED, 2) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_FIRE_SLASH_ENHANCED, 2) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_FIRE_BURST_ENHANCED, 2) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_DRAGON_ROAR_ENHANCED, 2) ||
			this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_SHINING_PEAK_ENHANCED, 2))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 2);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 2);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 2);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_WEAPON_ATTACK_POWER_INCREASE, 2);

			}
		}

		break;
	case FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER:
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_EVIL_SPIRIT_ENHANCED_DW, 1))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 1);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 1);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 1);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 1);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_METEOR_STRIKE_ENHANCED, 0))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 0);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 0);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 0);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 0);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_METEOR_STORM_ENHANCED, 2))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 2);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 2);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 2);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 2);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_DARK_BLAST_ENHANCED, 3))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 3);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 3);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 3);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 3);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_GIGANTIC_STORM_ENHANCED, 4))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 4);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 4);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 4);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 4);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_EVIL_SPIRIT_ENHANCED_MG, 5))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 5);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 5);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 5);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 5);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_LIGHTNING_SHOCK_ENHANCED, 0))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 0);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 0);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 0);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 0);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_AQUA_BEAST_ENHANCED, 1))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 1);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 1);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 1);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 1);

			}
		}
		if (this->CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_FIRE_BEAST_ENHANCED, 2))
		{
			if (lpObj->Inventory[0].IsItem())
			{
				lpObj->PhysiDamageMinRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 2);
				lpObj->PhysiDamageMaxRight += lpObj->PhysiDamageMinRight*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 2);

			}
			if (lpObj->Inventory[1].IsItem())
			{
				lpObj->PhysiDamageMinLeft += lpObj->PhysiDamageMinLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 2);
				lpObj->PhysiDamageMaxLeft += lpObj->PhysiDamageMaxLeft*(int)this->GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASED_WEAPON_POWER, 2);

			}
		}
		break;
	case FOR_MASTER_SKILL_ADD_ADD_PENETRATION_EFFECT:
		break;
	case FOR_MASTER_SKILL_ADD_ADD_ARROW_PROJECTILE:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_EFFECT1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION1 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_POISONING_EFFECT1 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_FREEZING_EFFECT1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_BLEEDING_EFFECT1:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_SHOCKING_EFFECT1:
		break;
	case FOR_MASTER_SKILL_ADD_POISONING_UPGRADE_PROBABILITY_ADDICTION1:
		break;
	case FOR_MASTER_SKILL_ADD_FREEZING_UPGRADE_PROBABILITY_FREEZE1 :
		break;
	case FOR_MASTER_SKILL_ADD_BLEEDING_UPGRADE_PROBABILITY_EXCESSIVE_BLEEDING1 :
		break;
	case FOR_MASTER_SKILL_ADD_SHOCKING_UPGRADE_PROBABILITY_STUN1 :
		break;
	case FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_EFFECT1:
		break;
	case FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION1 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING2:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_EFFECT2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT2:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_POISONING_EFFECT2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_FREEZING_EFFECT2 :
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_BLEEDING_EFFECT2:
		break;
	case FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_SHOCKING_EFFECT2 :
		break;
	case FOR_MASTER_SKILL_ADD_POISONING_UPGRADE_PROBABILITY_ADDICTION2 :
		break;
	case FOR_MASTER_SKILL_ADD_FREEZING_UPGRADE_PROBABILITY_FREEZE2:
		break;
	case FOR_MASTER_SKILL_ADD_BLEEDING_UPGRADE_PROBABILITY_EXCESSIVE_BLEEDING2:
		break;
	case FOR_MASTER_SKILL_ADD_SHOCKING_UPGRADE_PROBABILITY_STUN2:
		break;
	case FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_EFFECT2:
		break;
	case FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION2 :
		break;
	case FOR_MASTER_SKILL_ADD_RAGEFUL_BLOW_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_DEATH_STAB_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_FIRE_BLOW_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_METEOR_STRIKE_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_METEOR_STORM_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_EVIL_SPIRIT_ENHANCED_DW :
		break;
	case FOR_MASTER_SKILL_ADD_TRIPLE_SHOT_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_MULTI_SHOT_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_FOCUS_SHOT_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_GIGANTIC_STORM_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_EVIL_SPIRIT_ENHANCED_MG :
		break;
	case FOR_MASTER_SKILL_ADD_DARK_BLAST_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_FIRE_SLASH_ENHANCED:
		break;
	case FOR_MASTER_SKILL_ADD_FIRE_BLOOD_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_ICE_BLOOD_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_FIRE_BURST_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_CHAOTIC_DISEIER_ENHANCED:
		break;
	case FOR_MASTER_SKILL_ADD_WIND_SOUL_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_FIRE_BEAST_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_LIGHTNING_SHOCK_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_AQUA_BEAST_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_DRAGON_ROAR_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_CHAIN_DRIVE_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_DARK_SIDE_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_MAGIC_PIN_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_BRECHE_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_SHINING_PEAK_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_POISONING_STORM :
		break;
	case FOR_MASTER_SKILL_ADD_FREEZING_SLAUGHTER :
		break;
	case FOR_MASTER_SKILL_ADD_BLEEDING_HEART :
		break;
	case FOR_MASTER_SKILL_ADD_SHOCKING_BOOM :
		if ((value = gSkillManager.AddSkill(lpObj, SKILL_SHOCKING_BOOM, 0)) >= 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, value, SKILL_SHOCKING_BOOM, 0, 0);
		}
		break;
	case FOR_MASTER_SKILL_ADD_POISONING_STORM_ENHANCED_DW :
		break;
	case FOR_MASTER_SKILL_ADD_FREEZING_SLAUGHTER_ENHANCED:
		break;
	case FOR_MASTER_SKILL_ADD_BLEEDING_HEART_ENHANCED:
		break;
	case FOR_MASTER_SKILL_ADD_SHOCKING_BOOM_ENHANCED:
		break;
	case FOR_MASTER_SKILL_ADD_POISONING_STORM_ENHANCED :
		break;
	case FOR_MASTER_SKILL_ADD_FREEZING_SLAUGHTER_ENHANCED1 :
		break;
	case FOR_MASTER_SKILL_ADD_BLEEDING_HEART_ENHANCED1 :
		break;
	case FOR_MASTER_SKILL_ADD_SHOCKING_BOOM_ENHANCED1:
		break;
	default:
		break;
	}
}

BOOL MasterSkillTree_4th::CheckForMaster(LPOBJ lpObj)
{
	if (gServerInfo.m_ForMasterSkillTree == 0)
	{
		return 0;
	}

	if (lpObj->Level >= MAX_CHARACTER_LEVEL && lpObj->MasterLevel >= gServerInfo.m_MasterSkillTreeMaxLevel && lpObj->ChangeUp >= 3)
	{
		return 1;
	}
	
	return 0;
}