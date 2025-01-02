#include "User.h"

#ifndef ADDONS_H
#define ADDONS_H

class cAddons
{
public:
	void LoadAll();
	bool IsBadFileLine(char* FileLine, int& Flag);
	void LoginMsg(LPOBJ lpObj);
	int GetPlayerIndex(char* Name);
	char* GetMapName(int MapId);
	int GenExcOpt(int amount);
	int GetNumberByPercent(int Proc, int Min, int Max);
};
extern cAddons Addons;
#endif