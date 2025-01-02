// SummonScroll.h: interface for the CSummonScroll class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_SUMMON_SCROLL_MONSTER_GROUP 20

struct SUMMON_SCROLL_INFO
{
	int Index;
	int ItemIndex;
};

struct SUMMON_SCROLL_MONSTER_INFO
{
	int Index;
	int Group;
	int MonsterClass;
	int CreateRate;
};

struct SUMON_INFO
{
	int Index;
	int MonsterClass;
};
struct SUMON_MONSTER_INFO
{
	int Index;
	int Number;
	int MonsterClass;
	int CreaterRate;
};

class CSummonScroll
{
public:
	CSummonScroll();
	virtual ~CSummonScroll();
	void Load(char* path);
	bool CheckSummonScroll(int ItemIndex);
	bool GetSummonScrollInfo(int ItemIndex,SUMMON_SCROLL_INFO* lpInfo);
	bool CreateSummonScrollMonster(int aIndex,int ItemIndex,int map,int x,int y);
	bool CheckSummon(int MonsterClass);
	bool GetSummonInfo(int MonsterClass,SUMON_INFO* lpInfo);
	bool CreateSummonMonster(int aIndex,int MonsterClass,int map,int x,int y);
private:
	std::vector<SUMMON_SCROLL_INFO> m_SummonScrollInfo;
	std::vector<SUMMON_SCROLL_MONSTER_INFO> m_SummonScrollMonsterInfo;
	std::vector<SUMON_INFO> m_SummonInfo;
	std::vector<SUMON_MONSTER_INFO> m_SummonMonsterInfo;
};

extern CSummonScroll gSummonScroll;
