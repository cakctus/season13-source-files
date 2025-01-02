// Skill.h: interface for the CSkill class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CSkill
{
public:
	CSkill();
	virtual ~CSkill();
	void Clear();
	bool IsSkill();
	bool IsMasterSkill();
	bool IsForMasterSkill(); // OK
	bool Set(int aIndex, int index,int level);
	bool Set1(int aIndex, int index, int level,int group); // OK
public:
	BYTE m_level;
	WORD m_skill;
	WORD m_index;
	BYTE m_group;
	int m_DamageMin;
	int m_DamageMax;
};
