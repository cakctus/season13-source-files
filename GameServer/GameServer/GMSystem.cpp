#include "StdAfx.h"
#include "GmSystem.h"
#include "Logger.h"	    
#include "Configs.h"
#include "User.h"
#include "Addons.h"
cGmSystem GmSystem;


void cGmSystem::ResetConfig()
{
	Config.IsGMSystem = 0;
	Config.IsGMInDB = 0;

	GMCount = 0;
	for (int i = 0; i < 255; i++)
	{
		GM[i].IsAdmin = 0;
		memset(GM[i].Name, 0, sizeof(GM[i].Name));
		GM[i].Drop = 0;
		GM[i].Gg = 0;
		GM[i].SetZen = 0;
		GM[i].Status = 0;
		GM[i].Gmove = 0;
		GM[i].SetPK = 0;
		GM[i].PkClear = 0;
		GM[i].BanPlayer = 0;
		GM[i].Reload = 0;
		GM[i].Skin = 0;
		GM[i].BanPost = 0;
		GM[i].Voskl = 0;
		GM[i].Disconnect = 0;
		GM[i].Trace = 0;
		GM[i].DisableChat = 0; 
		GM[i].Hide = 0;
		GM[i].Fireworks = 0;
		GM[i].ClearInv = 0;
		GM[i].Spawn = 0;
		GM[i].MessagePopup = 0;
		GM[i].Make = 0;
	}
}

void cGmSystem::Load()
{
	ResetConfig();
	Config.IsGMSystem = Configs.GetInt(0, 1, 0, "GMSystem", "IsGMSystem", cfGM);
	if (!Config.IsGMSystem)return;
	Config.IsGMInDB = Configs.GetInt(0, 1, 0, "GMSystem", "IsGMInDB", cfGM);

	FILE* file;

	file = fopen(cfGM, "r");

	if (file == NULL)
	{
		Config.IsGMSystem = 0;
		Log.ConsoleOutPut(1, c_Red, t_Error, "[X] [GMSystem]\tCan`t Find %s", cfGM);
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
			int n[21];
			char GetGMName[10];
			int isAdmin;

			sscanf(Buff, "%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &isAdmin, &GetGMName, &n[0], &n[1], &n[2], &n[3], &n[4], &n[5], &n[6], &n[7], &n[8], &n[9], &n[10], &n[11], &n[12], &n[13], &n[14], &n[15], &n[16], &n[17], &n[18], &n[19], &n[20]);
			GM[GMCount].IsAdmin = isAdmin;
			strncpy_s(GM[GMCount].Name, GetGMName, 10);
			GM[GMCount].Drop = n[0];
			GM[GMCount].Gg = n[1];
			GM[GMCount].SetZen = n[2];
			GM[GMCount].Status = n[3];
			GM[GMCount].Gmove = n[4];
			GM[GMCount].SetPK = n[5];
			GM[GMCount].PkClear = n[6];
			GM[GMCount].BanPlayer = n[7];
			GM[GMCount].Reload = n[8];
			GM[GMCount].Skin = n[9];
			GM[GMCount].BanPost = n[10];
			GM[GMCount].Voskl = n[11];
			GM[GMCount].Disconnect = n[12];
			GM[GMCount].Trace = n[13];
			GM[GMCount].DisableChat = n[14];
			GM[GMCount].Hide = n[15];
			GM[GMCount].Fireworks = n[16];
			GM[GMCount].ClearInv = n[17];
			GM[GMCount].Spawn = n[18];
			GM[GMCount].MessagePopup = n[19];
			GM[GMCount].Make = n[20];
			GMCount++;
		}
	}

	fclose(file);
	Log.ConsoleOutPut(1, c_Magenta, t_GM, "[V] [GMSystem]\tLoad successfully. Total GMs: %d", GMCount);
}


int cGmSystem::IsCommand(Commands Cmd, char *Name)
{
	if (Cmd == NONE) return 1;

	if (IsGMBD(Name))
	{
		if (Config.IsGMSystem)
		{
			for (int x = 0; x < GMCount; x++)
				if (!strncmp(GM[x].Name, Name, 10))
				{
					switch (Cmd)
					{
					case cDrop:
						return GM[x].Drop;
					case cGg:
						return GM[x].Gg;
					case cSetZen:
						return GM[x].SetZen;
					case cStatus:
						return GM[x].Status;
					case cGmove:
						return GM[x].Gmove;
					case cSetPK:
						return GM[x].SetPK;
					case cPkClear:
						return GM[x].PkClear;
					case cBanPlayer:
						return GM[x].BanPlayer;
					case cReload:
						return GM[x].Reload;
					case cSkin:
						return GM[x].Skin;
					case cBanPost:
						return GM[x].BanPost;
					case cVoskl:
						return GM[x].Voskl;
					case cDisconnect:
						return GM[x].Disconnect;
					case cTrace:
						return GM[x].Trace;
					case cDisableChat:
						return GM[x].DisableChat; 
					case cHide:
						return GM[x].Hide;
					case cFireworks:
						return GM[x].Fireworks;
					case cClearInv:
						return GM[x].ClearInv;
					case cSpawn:
						return GM[x].Spawn;
					case cMessagePopup:
						return GM[x].MessagePopup;
					case cMake:
						return GM[x].Make;
					}
				}
			return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}

int cGmSystem::IsAdmin(char *Name)
{
	if (IsGMBD(Name))
	{
		if (Config.IsGMSystem)
		{
			for (int x = 0; x < GMCount; x++)
				if (!strncmp(GM[x].Name, Name, 10))
				{
					switch (GM[x].IsAdmin)
					{
					case 0:
						return 0;
					case 1:
						return 1;
					case 2:
						return 2;
					}
				}
			return 0;
		}
		else
			return 2;
	}
	else return 0;
}

bool cGmSystem::IsGMBD(char *Name)
{
	if (Config.IsGMInDB == 0)
		return true;
	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		LPOBJ lpObj = &gObj[i];
		if (lpObj->Connected < OBJECT_ONLINE) continue;
		if (!strncmp(lpObj->Name, Name, 10) && (lpObj->Authority == 32 || lpObj->Authority == 8))
			return true;
	}
	return false;
}

bool cGmSystem::IsGMBD(int aIndex)
{ 
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Connected < OBJECT_ONLINE)
		return false;
	if ((lpObj->Authority == 32 || lpObj->Authority == 8))
		return true;

	return false;
}
