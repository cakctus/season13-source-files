#include "StdAfx.h"
#include "DropSystem.h"

#include "Logger.h"
#include "Addons.h"
#include "Configs.h"
#include "DSProtocol.h"

cDropSystem DropSystem;

void cDropSystem::ResetConfig()
{
	for (int j = 0; j < 1000; j++)
	{
		AllMobItemsDrop[j].MMap = 0;
		AllMobItemsDrop[j].MMinLvl = 0;
		AllMobItemsDrop[j].MMaxLvl = 0;
		AllMobItemsDrop[j].IDropRate = 0;
		AllMobItemsDrop[j].IGroup = 0;
		AllMobItemsDrop[j].IIndex = 0;
		AllMobItemsDrop[j].IMinLvl = 0;
		AllMobItemsDrop[j].IMaxLvl = 0;
		AllMobItemsDrop[j].ILvlRate = 0;
		AllMobItemsDrop[j].IMinOpt = 0;
		AllMobItemsDrop[j].IMaxOpt = 0;
		AllMobItemsDrop[j].IOptRate = 0;
		AllMobItemsDrop[j].ISkill = 0;
		AllMobItemsDrop[j].ISkillRate = 0;
		AllMobItemsDrop[j].ILuck = 0;
		AllMobItemsDrop[j].ILuckRate = 0;
		AllMobItemsDrop[j].IMinExc = 0;
		AllMobItemsDrop[j].IMaxExc = 0;
		AllMobItemsDrop[j].IExcRate = 0;
		AllMobItemsDrop[j].IAnc = 0;
		AllMobItemsDrop[j].IAncRate = 0;
	}

	AllMobArrayMaxItem = 0;

	for (int i = 0; i < MAX_MONSTER_ID; i++)
	{
		for (int j = 0; j < MAX_ITEM_FOR_MONSTER; j++)
		{
			ItemsDrop[i][j].MMap = 0;
			ItemsDrop[i][j].MMinLvl = 0;
			ItemsDrop[i][j].MMaxLvl = 0;
			ItemsDrop[i][j].IDropRate = 0;
			ItemsDrop[i][j].IGroup = 0;
			ItemsDrop[i][j].IIndex = 0;
			ItemsDrop[i][j].IMinLvl = 0;
			ItemsDrop[i][j].IMaxLvl = 0;
			ItemsDrop[i][j].ILvlRate = 0;
			ItemsDrop[i][j].IMinOpt = 0;
			ItemsDrop[i][j].IMaxOpt = 0;
			ItemsDrop[i][j].IOptRate = 0;
			ItemsDrop[i][j].ISkill = 0;
			ItemsDrop[i][j].ISkillRate = 0;
			ItemsDrop[i][j].ILuck = 0;
			ItemsDrop[i][j].ILuckRate = 0;
			ItemsDrop[i][j].IMinExc = 0;
			ItemsDrop[i][j].IMaxExc = 0;
			ItemsDrop[i][j].IExcRate = 0;
			ItemsDrop[i][j].IAnc = 0;
			ItemsDrop[i][j].IAncRate = 0;
		}
		ArrayMaxItem[i] = 0;
	}
}

void cDropSystem::Load()
{
	ResetConfig();
	FILE* file;
	file = fopen(cfDROP, "r");

	if (file == NULL)
	{
		Log.ConsoleOutPut(1, c_Red, t_Error, "[X] [Drop System]\tCan`t Find %s", cfDROP);
		return;
	}

	char Buff[256];
	int Flag = 0;

	while (!feof(file))
	{
		fgets(Buff, 256, file);

		if (Addons.IsBadFileLine(Buff, Flag))
			continue;

		if (Flag == 1)
		{
			int n[22];
			sscanf(Buff, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &n[0], &n[1], &n[2], &n[3], &n[4], &n[5], &n[6], &n[7], &n[8], &n[9], &n[10], &n[11], &n[12], &n[13], &n[14], &n[15], &n[16], &n[17], &n[18], &n[19], &n[20], &n[21]);

			if (n[0] == -1)
			{
				int j = AllMobArrayMaxItem;
				if (j >= 1000)
					continue;
				AllMobItemsDrop[j].MMap = n[1];
				AllMobItemsDrop[j].MMinLvl = n[2];
				AllMobItemsDrop[j].MMaxLvl = n[3];
				AllMobItemsDrop[j].IDropRate = n[4];
				AllMobItemsDrop[j].IGroup = n[5];
				AllMobItemsDrop[j].IIndex = n[6];
				AllMobItemsDrop[j].IMinLvl = n[7];
				AllMobItemsDrop[j].IMaxLvl = n[8];
				AllMobItemsDrop[j].ILvlRate = n[9];
				AllMobItemsDrop[j].IMinOpt = n[10];
				AllMobItemsDrop[j].IMaxOpt = n[11];
				AllMobItemsDrop[j].IOptRate = n[12];
				AllMobItemsDrop[j].ISkill = n[13];
				AllMobItemsDrop[j].ISkillRate = n[14];
				AllMobItemsDrop[j].ILuck = n[15];
				AllMobItemsDrop[j].ILuckRate = n[16];
				AllMobItemsDrop[j].IMinExc = n[17];
				AllMobItemsDrop[j].IMaxExc = n[18];
				AllMobItemsDrop[j].IExcRate = n[19];
				AllMobItemsDrop[j].IAnc = n[20];
				AllMobItemsDrop[j].IAncRate = n[21];

				AllMobArrayMaxItem = ++j;
			}
			else
			{
				int MobId = n[0];
				int j = ArrayMaxItem[MobId];
				if (j >= MAX_ITEM_FOR_MONSTER)
					continue;
				ItemsDrop[MobId][j].MMap = n[1];
				ItemsDrop[MobId][j].MMinLvl = n[2];
				ItemsDrop[MobId][j].MMaxLvl = n[3];
				ItemsDrop[MobId][j].IDropRate = n[4];
				ItemsDrop[MobId][j].IGroup = n[5];
				ItemsDrop[MobId][j].IIndex = n[6];
				ItemsDrop[MobId][j].IMinLvl = n[7];
				ItemsDrop[MobId][j].IMaxLvl = n[8];
				ItemsDrop[MobId][j].ILvlRate = n[9];
				ItemsDrop[MobId][j].IMinOpt = n[10];
				ItemsDrop[MobId][j].IMaxOpt = n[11];
				ItemsDrop[MobId][j].IOptRate = n[12];
				ItemsDrop[MobId][j].ISkill = n[13];
				ItemsDrop[MobId][j].ISkillRate = n[14];
				ItemsDrop[MobId][j].ILuck = n[15];
				ItemsDrop[MobId][j].ILuckRate = n[16];
				ItemsDrop[MobId][j].IMinExc = n[17];
				ItemsDrop[MobId][j].IMaxExc = n[18];
				ItemsDrop[MobId][j].IExcRate = n[19];
				ItemsDrop[MobId][j].IAnc = n[20];
				ItemsDrop[MobId][j].IAncRate = n[21];

				ArrayMaxItem[MobId] = ++j;
			}
		}
	}

	fclose(file);
	Log.ConsoleOutPut(1, c_Yellow, t_DROP, "[V] [Drop System]\tDrop System Items Loaded");
}

bool cDropSystem::DropItem(LPOBJ mObj, LPOBJ pObj)
{
	if (ArrayMaxItem[mObj->Class] == 0)
		return DropItem2(mObj, pObj, AllMobItemsDrop, AllMobArrayMaxItem);
	else
		return DropItem2(mObj, pObj, ItemsDrop[mObj->Class], ArrayMaxItem[mObj->Class]);
}

bool cDropSystem::DropItem2(LPOBJ mObj, LPOBJ pObj, sItemsDrop ItemDrop[], unsigned int MaxItem)
{
	int MapArrayItem[1000];
	int CountArrayItem = 0;
	int LvlArrayItem[1000];
	int CountLvlArrayItem = 0;

	for (unsigned int i = 0; i < MaxItem; i++)
	{
		if (ItemDrop[i].MMap == mObj->Map || ItemDrop[i].MMap == -1)
		{
			MapArrayItem[CountArrayItem] = i;
			CountArrayItem++;
		}
	}

	if (CountArrayItem == 0) return false;

	for (int j = 0; j < CountArrayItem; j++)
	{
		if ((ItemDrop[MapArrayItem[j]].MMinLvl <= mObj->Level &&
			ItemDrop[MapArrayItem[j]].MMaxLvl >= mObj->Level) ||
			ItemDrop[MapArrayItem[j]].MMaxLvl == 0)
		{
			LvlArrayItem[CountLvlArrayItem] = MapArrayItem[j];
			CountLvlArrayItem++;
		}
	}

	if (CountLvlArrayItem == 0) return false;

	int RandomValue = rand() % 10000 + 1;
	if (LastRandomValue == RandomValue)
		return false;
	else
		LastRandomValue = RandomValue;

	int d = 0;
	int RandomItem = -1;
	short d_start = 0;
	short d_end = 0;

	for (int f = 0; f < CountLvlArrayItem; f++)
	{
		if (d_start > 10000) d_start = 10000;
		d_end = d_start + ItemDrop[LvlArrayItem[f]].IDropRate;
		if (d_end > 10000) d_end = 10000;

		if (RandomValue >= d_start && RandomValue <= d_end)
		{
			RandomItem = LvlArrayItem[f];
			break;
		}
		else {
			d_start += ItemDrop[LvlArrayItem[f]].IDropRate;
		}
	}

	if (RandomItem == -1) return false;

	// make opt
	int Level = 0, Skill = 0, Luck = 0, Opt = 0, Exc = 0, ExcNum = 0, Group = 0, Index = 0, Anc = 0;

	Index = ItemDrop[RandomItem].IIndex;
	Group = ItemDrop[RandomItem].IGroup;
	if (Group == -1)
	{
		if (Index == 15 || Index == 20 || Index == 23 || Index == 32 || Index == 37 || Index == 47 || Index == 48)
			Group = rand() % 4 + 8;
		else
			Group = rand() % 5 + 7;
	}
	Level = Addons.GetNumberByPercent(ItemDrop[RandomItem].ILvlRate,
		ItemDrop[RandomItem].IMinLvl,
		ItemDrop[RandomItem].IMaxLvl);
	Opt = Addons.GetNumberByPercent(ItemDrop[RandomItem].IOptRate,
		ItemDrop[RandomItem].IMinOpt,
		ItemDrop[RandomItem].IMaxOpt);
	Skill = Addons.GetNumberByPercent(ItemDrop[RandomItem].ISkillRate, 0,
		ItemDrop[RandomItem].ISkill);
	Luck = Addons.GetNumberByPercent(ItemDrop[RandomItem].ILuckRate, 0,
		ItemDrop[RandomItem].ILuck);

	if (ItemDrop[RandomItem].IExcRate > 0)
	{
		ExcNum = Addons.GetNumberByPercent(ItemDrop[RandomItem].IExcRate,
			ItemDrop[RandomItem].IMinExc,
			ItemDrop[RandomItem].IMaxExc);
		Exc = Addons.GenExcOpt(ExcNum);
	}

	if (ItemDrop[RandomItem].IAnc > 0)
		if (ItemDrop[RandomItem].IAncRate > rand() % 100)
			Anc = ItemDrop[RandomItem].IAnc;

	int Item = ITEMGET(Group, Index);
	 
	GDCreateItemSend(pObj->Index, mObj->Map, (BYTE)mObj->X, (BYTE)mObj->Y, Item, Level, 0, Skill, Luck, Opt, pObj->Index, Exc, Anc, 0, 0, 0, 0xFF, 0);
	
	Log.ConsoleOutPut(1, c_Green, t_DROP, "[Drop System] [R=%d] [%s]\t%d %d %d %d %d %d %d %d", RandomValue, pObj->Name, Group, Index, Level, Luck, Skill, Opt, Exc, Anc);

	return true;
}