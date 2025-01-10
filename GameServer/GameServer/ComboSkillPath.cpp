#include "stdafx.h"
#include "User.h"
#include "SkillManager.h"
#include "DefaultClassInfo.h"
#include "ComboSkillPath.h"
#include "MemScript.h"
#include "Util.h"
#include "Attack.h"
ComboSkillPath g_ComboSkillPath;

ComboSkillPath::ComboSkillPath()
{

}


ComboSkillPath::~ComboSkillPath()
{
}
void ComboSkillPath::ReadSkillInfo(char* section, char* path) // OK
{
	this->m_ComboSkillPath = GetPrivateProfileInt(section, "ComboSkill", 3, path);
	this->m_CustomizeComboSwitch = GetPrivateProfileInt(section, "CustomizeComboSwitch", 0, path); 
	this->m_ComboSkillDelay = GetPrivateProfileInt(section, "ComboSkillDelay", 3000, path);
	
}
void ComboSkillPath::Load(char* path)
{
#if(GAMESERVER_UPDATE>=401)//2018/02/15
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

	this->m_ComboSkill.clear();

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

			COMBO_SKILL_TYPE info;

			info.Type = lpMemScript->GetNumber();
			
			info.combo.Class = lpMemScript->GetAsNumber();

			info.combo.SkillType = lpMemScript->GetAsNumber();

			info.combo.Skill = lpMemScript->GetAsNumber();

			this->m_ComboSkill.push_back(info);

		}
	
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
#endif
}
void ComboSkillPath::Init() // OK
{
	this->m_time = 0;
	this->m_skill[0] = 0xFFFF;
	this->m_skill[1] = 0xFFFF;
	this->m_index = -1;
}
bool ComboSkillPath::Combo(LPOBJ lpObj,int bIndex, CSkill* lpSkill)
{
	int type = -1;
	DWORD elapsed;
	switch (this->m_ComboSkillPath)
	{
	case 1:
		 elapsed = GetTickCount() - lpObj->ComboTime;

		if (elapsed < this->m_ComboSkillDelay)
		{
			this->Init();

			return 0;
		}

		if (this->CheckSkill(lpObj->Class, 1, lpSkill->m_skill) == 1)
		{
			gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 1);
			gSkillManager.GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);

			return 1;
		}

		break;
	case 2:
		type = this->GetSkillType(lpObj->Class, 2, lpSkill->m_skill,((this->m_skill[0]== 0xFFFF)?0:1));

		if (type == -1)
		{
			this->Init();
			return 0;
		}

		if (type == 0)
		{
			this->m_time = GetTickCount() + this->m_ComboSkillDelay;
			this->m_skill[0] = lpSkill->m_skill;
			this->m_index = 0;
			return 0;
		}

		if (type == 1 )
		{
			if (this->m_time < GetTickCount())
			{
				this->Init();
				return 0;
			}

			if (this->m_skill[0] == 0xFFFF)
			{
				this->Init();
				return 0;
			}

			if (this->m_index == 0 && this->m_skill[0] != lpSkill->m_skill)
			{
				gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 1);
				gSkillManager.GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
				this->Init();
				return 1;
			}
		}
		break;
	case 3:
		type = this->GetSkillType(lpObj->Class, 3, lpSkill->m_skill, ((this->m_skill[0] == 0xFFFF) ? 0 : 1));

		if (type == -1)
		{
			this->Init();
			return 0;
		}

		if (type == 0)
		{
			this->m_time = GetTickCount() + this->m_ComboSkillDelay;
			this->m_skill[0] = lpSkill->m_skill;
			this->m_index = 0;
			return 0;
		}

		if (type == 1|| type == 2)
		{
			if (this->m_time < GetTickCount())
			{
				this->Init();
				return 0;
			}

			if (this->m_skill[0] == 0xFFFF)
			{
				this->Init();
				return 0;
			}

			if (this->m_index == 0)
			{
				this->m_time = GetTickCount() + this->m_ComboSkillDelay;
				this->m_skill[1] = lpSkill->m_skill;
				this->m_index = 1;
				return 0;
			}

			if (this->m_index == 1 && this->m_skill[1] != lpSkill->m_skill)
			{
				gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 1);
				gSkillManager.GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
				this->Init();
				return 1;
			}
		}
		break;
	default:
		break;
	}
	this->Init();
	return 0;
}

bool ComboSkillPath::CheckSkill(int Class,int combo, int skill)
{
	for (std::vector<COMBO_SKILL_TYPE>::iterator it = this->m_ComboSkill.begin(); it != this->m_ComboSkill.end(); it++)
	{
		if (it->Type == combo &&it->combo.Skill == skill&&it->combo.Class ==Class)
		{
			return 1;
		}
	}
	return 0;
}
int ComboSkillPath::GetSkillType(int Class, int combo,WORD skill, int skilltype) // OK
{
	for (std::vector<COMBO_SKILL_TYPE>::iterator it = this->m_ComboSkill.begin(); it != this->m_ComboSkill.end(); it++)
	{
		if (it->Type == combo && it->combo.Skill == skill && it->combo.Class == Class && (it->combo.SkillType == skilltype + 1|| it->combo.SkillType == skilltype + 2))
		{

			return (it->combo.SkillType - 1);
	
		}
	}
	return -1;
}