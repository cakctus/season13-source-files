#ifndef DROPSYSTEM
#define DROPSYSTEM

#define MAX_MONSTER_ID			1000
#define MAX_ITEM_FOR_MONSTER	40
#define ITEMGET(x,y) ((x)*512+(y))
#include "User.h"

class cDropSystem
{
public:
	void ResetConfig();
	void Load();
	bool DropItem(LPOBJ mObj,LPOBJ pObj);

private:
	struct sItemsDrop
	{
		short MMap;			// #1
		short MMinLvl;		// #2
		short MMaxLvl;		// #3

		short IDropRate;	// #4
		short IGroup;		// #5
		short IIndex;		// #6

		short IMinLvl;		// #7
		short IMaxLvl;		// #8
		short ILvlRate;		// #9

		short IMinOpt;		// #10
		short IMaxOpt;		// #11
		short IOptRate;		// #12

		short ISkill;		// #13
		short ISkillRate;	// #14
		short ILuck;		// #15
		short ILuckRate;	// #16

		short IMinExc;		// #17
		short IMaxExc;		// #18
		short IExcRate;		// #19

		short IAnc;			// #20
		short IAncRate;		// #21
	};
	sItemsDrop ItemsDrop[MAX_MONSTER_ID][MAX_ITEM_FOR_MONSTER];
	sItemsDrop AllMobItemsDrop[1000];

	int LastRandomValue;
	int LastRandomItem;

	unsigned char ArrayMaxItem[MAX_MONSTER_ID];
	unsigned int AllMobArrayMaxItem;

	bool DropItem2(LPOBJ mObj,LPOBJ pObj, sItemsDrop ItemDrop[], unsigned int MaxItem);
};

extern cDropSystem DropSystem;

#endif
