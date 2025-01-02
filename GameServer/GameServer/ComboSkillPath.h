#pragma once

#define MAX_COMBO_SKILL 100
struct COMBO_SKILL_PATH
{

	int Class;
	int SkillType;
	int Skill;
};
struct COMBO_SKILL_TYPE
{
	int Type;
	COMBO_SKILL_PATH combo;
};

class ComboSkillPath
{
public:
	ComboSkillPath();
	virtual ~ComboSkillPath();
	void ReadSkillInfo(char* section, char* path); // OK
	void Load(char* path);
	void Init(); // OK
	int GetSkillType(int Class, int combo, WORD skill,int skilltype);
	bool CheckSkill(int Class,int combo, int skill);
	bool Combo(LPOBJ lpObj, int bIndex, CSkill* lpSkill);

public:
	std::vector<COMBO_SKILL_TYPE> m_ComboSkill;
	DWORD m_time;
	WORD m_skill[2];
	int m_index;
	BYTE m_ComboSkillPath;
	BYTE m_CustomizeComboSwitch;
	DWORD m_ComboSkillDelay;
};

extern ComboSkillPath g_ComboSkillPath;