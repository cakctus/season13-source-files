#include "stdafx.h"
#include "Skill.h"
#include "SkillManager.h"
#include "MasterSkillTree.h"
#include "MasterSkillTree_4th.h"

CSkill::CSkill() // OK
{
	this->Clear();
}

CSkill::~CSkill() // OK
{

}

void CSkill::Clear() // OK
{
	this->m_level = 0;
	this->m_skill = 0xFFFF;
	this->m_index = 0xFFFF;
	this->m_DamageMin = 0;
	this->m_DamageMax = 0;
}

bool CSkill::IsSkill() // OK
{
	if(this->m_index == 0xFFFF)
	{
		return 0;
	}

	return 1;
}

bool CSkill::IsMasterSkill() // OK
{
	if (this->m_index == 0xFFFF)
	{
		return 0;
	}

	if (this->m_index > 739 || (this->m_index >= 300 && this->m_index < 723))
	{
		return 1;
	}

	return 0;
}

bool CSkill::IsForMasterSkill() // OK
{
	if (this->m_index == 0xFFFF)
	{
		return 0;
	}
	if (this->m_index >= 1001 && this->m_index < 1150)
	{
		return 1;
	}

	return 0;
}

bool CSkill::Set(int aIndex, int index, int level) // OK
{
	if (this->IsSkill() != 0 && (index < 300 || (index >= 723 && index <= 740)))
	{
		return 0;
	}

	this->m_level = level;

	this->m_skill = (((this->m_skill = gMasterSkillTree.GetMasterSkillRelated(index)) == 0) ? index : this->m_skill);

	if (index >= 1001 && index <= 1150)
	{
		//int iskill = gMasterSkillTree_4th.GetForMasterSkillParentSkill(gObj[aIndex].Class, index);

		////this->m_skill = ((this->m_skill = gMasterSkillTree.GetMasterSkillRelated(iskill)) == 0) ? index : this->m_skill;

		//CSkill *mskill;
		//mskill = gSkillManager.GetSkill(&gObj[aIndex], iskill);
		//if (mskill != NULL)
		//{
		//	this->m_skill = mskill->m_skill;
		//}
		if (index == 1069)
		{
			this->m_skill = 42;
		}
		if (index == 1071)
		{
			this->m_skill = 43;
		}
		if (index == 1072)
		{
			this->m_skill = 723;
		}
		if (index==1075)
		{
			this->m_skill = 724;
		}
		if (index==1078|| index == 1088)
		{
			this->m_skill = 9;
		}
		if (index==1076)
		{
			this->m_skill = 725;
		}
		if (index == 1081)
		{
			this->m_skill = 24;
		}
		if (index == 1083)
		{
			this->m_skill = 235;
		}
		if (index == 1085)
		{
			this->m_skill = 727;
		}
		if (index == 1094)
		{
			this->m_skill = 732;
		}
		if (index == 1095)
		{
			this->m_skill = 731;
		}
		if (index == 1092)
		{
			this->m_skill = 55;
		}
		if (index == 1087)
		{
			this->m_skill = 237;
		}
		if (index == 1098)
		{
			this->m_skill = 238;
		}
		if (index == 1099)
		{
			this->m_skill = 737;
		}
		if (index == 1096)
		{
			this->m_skill = 61;
		}
		if (index == 1103)
		{
			this->m_skill = 230;
		}
		if (index == 1105)
		{
			this->m_skill = 730;
		}
		if (index == 1102)
		{
			this->m_skill = 729;
		}
		if (index == 1112)
		{
			this->m_skill = 262;
		}
		if (index == 1113)
		{
			this->m_skill = 263;
		}
		if (index == 1111)
		{
			this->m_skill = 264;
		}
		if (index == 1118)
		{
			this->m_skill = 279;
		}

		if (index == 1117)
		{
			this->m_skill = 274;
		}
		if (index == 1119)
		{
			this->m_skill = 277;
		}
	}

	this->m_index = index;

	this->m_DamageMin = gSkillManager.GetSkillDamage(index);

	this->m_DamageMax = this->m_DamageMin+(this->m_DamageMin/2);

	return 1;
}

bool CSkill::Set1(int aIndex, int index, int level,int group) // OK
{
	if (this->IsSkill() != 0 && (index < 300 || (index >= 723 && index <= 740)))
	{
		return 0;
	}

	CSkill *mskill;

	int iskill = gMasterSkillTree_4th.GetForMasterSkillParentSkill(gObj[aIndex].Class, index);

	if (iskill == -1)
	{
		this->m_skill = index;
	}
	else
	{
		if (gSkillManager.GetSkill(&gObj[aIndex], index) == 0)
		{
			mskill = gSkillManager.GetSkill(&gObj[aIndex], iskill);

			this->m_skill = mskill->m_skill;
		}
		else
		{
			mskill = gSkillManager.GetSkill(&gObj[aIndex], index);
			this->m_skill = mskill->m_skill;
		}
	}	

	this->m_group = group;

	this->m_level = level;

	this->m_index = index;

	this->m_DamageMin = gSkillManager.GetSkillDamage(index);

	this->m_DamageMax = this->m_DamageMin + (this->m_DamageMin / 2);

	return 1;
}