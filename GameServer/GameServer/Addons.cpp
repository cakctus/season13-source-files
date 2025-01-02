#include "stdafx.h"
#include "Addons.h"
#include "Logger.h"
#include "Configs.h"
#include "NewsSystem.h"
#include "ChatSystem.h"
#include "GMSystem.h"
#include "ServerInfo.h"
#include "DropSystem.h"
cAddons Addons;

void cAddons::LoadAll()
{
	Log.ConsoleOutPut(0, c_Green, t_NULL, "Mu Online S13+ Emulator"); 
	Log.ConsoleOutPut(0, c_Green, t_NULL, "Compile Date: %s %s", __DATE__, __TIME__); 
	Log.ConsoleOutPut(0, c_Green, t_NULL, "Please report any bugs that you found!\n-----------------------------------------------------\n");

	Configs.LoadAll();
	News.LoadNotice();
	News.Load();
	GmSystem.Load();
	DropSystem.Load();


}
void cAddons::LoginMsg(LPOBJ lpObj)
{
	Chat.Message(0, lpObj, News.ConnectNotice); //fixed here


	if (News.ConnectInfo == 1)
	{
		Chat.Message(1, lpObj, "Total Online: %d/%d", gObjTotalUser, gServerInfo.m_ServerMaxUserNumber);

		SYSTEMTIME t;
		GetLocalTime(&t);
		Chat.Message(1, lpObj, "Server Time & Date: %02d:%02d:%02d %02d-%02d-%04d.", t.wHour, t.wMinute, t.wSecond, t.wDay, t.wMonth, t.wYear);
	}

	switch (GmSystem.IsAdmin(lpObj->Name))
	{
	case 1:
		Chat.MessageAllLog(Gold, c_Green, t_GM, lpObj, "[Admin] %s join the game!", lpObj->Name);
		break;
	case 2:
		Chat.MessageAllLog(Gold, c_Green, t_GM, lpObj, "[GM] %s join the game!", lpObj->Name);
		break;
	}
}

int cAddons::GetPlayerIndex(char* Name)
{
	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		LPOBJ lpObj = &gObj[i];
		if (lpObj->Connected < OBJECT_ONLINE) continue;
		if (!strcmp(lpObj->Name, Name)) return i;
	}
	return -1;
}


char* cAddons::GetMapName(int MapId) // TODO: Need to update map list
{
	switch (MapId)
	{
	case -1: return "All Maps";
	case 0: return "LORENCIA"; // 0
	case 1: return "DUNGEON"; // 1,
	case 2: return "DEVIAS"; // 2,
	case 3: return "NORIA"; // 3,
	case 4: return "LOST_TOWER"; // 4,
	case 5: return "RESERVED1"; // 5,
	case 6: return "ARENA"; // 6,
	case 7: return "ATLANS"; // 7,
	case 8: return "TARKAN"; // 8,
	case 9: return "DEVIL_SQUARE1"; // 9,
	case 10: return "ICARUS"; // 10,
	case 11: return "BLOOD_CASTLE1"; // 11,
	case 12: return "BLOOD_CASTLE2"; // 12,
	case 13: return "BLOOD_CASTLE3"; // 13,
	case 14: return "BLOOD_CASTLE4"; // 14,
	case 15: return "BLOOD_CASTLE5"; // 15,
	case 16: return "BLOOD_CASTLE6"; // 16,
	case 17: return "BLOOD_CASTLE7"; // 17,
	case 18: return "CHAOS_CASTLE1"; // 18,
	case 19: return "CHAOS_CASTLE2"; // 19,
	case 20: return "CHAOS_CASTLE3"; // 20,
	case 21: return "CHAOS_CASTLE4"; // 21,
	case 22: return "CHAOS_CASTLE5"; // 22,
	case 23: return "CHAOS_CASTLE6"; // 23,
	case 24: return "KALIMA1"; // 24,
	case 25: return "KALIMA2"; // 25,
	case 26: return "KALIMA3"; // 26,
	case 27: return "KALIMA4"; // 27,
	case 28: return "KALIMA5"; // 28,
	case 29: return "KALIMA6"; // 29,
	case 30: return "CASTLE_SIEGE"; // 30,
	case 31: return "LAND_OF_TRIALS"; // 31,
	case 32: return "DEVIL_SQUARE2"; // 32,
	case 33: return "AIDA"; // 33,
	case 34: return "CRYWOLF"; // 34,
	case 35: return "RESERVED2"; // 35,
	case 36: return "KALIMA7"; // 36,
	case 37: return "KANTURU1"; // 37,
	case 38: return "KANTURU2"; // 38,
	case 39: return "KANTURU3"; // 39,
	case 40: return "SILENT"; // 40,
	case 41: return "BARRACKS"; // 41,
	case 42: return "REFUGE"; // 42,
	case 45: return "ILLUSION_TEMPLE1"; // 45,
	case 46: return "ILLUSION_TEMPLE2"; // 46,
	case 47: return "ILLUSION_TEMPLE3"; // 47,
	case 48: return "ILLUSION_TEMPLE4"; // 48,
	case 49: return "ILLUSION_TEMPLE5"; // 49,
	case 50: return "ILLUSION_TEMPLE6"; // 50,
	case 51: return "ELBELAND"; // 51,
	case 52: return "BLOOD_CASTLE8"; // 52,
	case 53: return "CHAOS_CASTLE7"; // 53,
	case 56: return "SWAMP_OF_CALMNESS"; // 56,
	case 57: return "RAKLION1"; // 57,
	case 58: return "RAKLION2"; // 58,
	case 62: return "SANTA_TOWN"; // 62,
	case 63: return "VULCANUS"; // 63,
	case 64: return "DUEL_ARENA"; // 64,
	case 65: return "DOUBLE_GOER1"; // 65,
	case 66: return "DOUBLE_GOER2"; // 66,
	case 67: return "DOUBLE_GOER3"; // 67,
	case 68: return "DOUBLE_GOER4"; // 68,
	case 69: return "IMPERIAL_GUARDIAN1"; // 69,
	case 70: return "IMPERIAL_GUARDIAN2"; // 70,
	case 71: return "IMPERIAL_GUARDIAN3"; // 71,
	case 72: return "IMPERIAL_GUARDIAN4"; // 72,
	case 79: return "LOREN_MARKET"; // 79,
	case 80: return "KARUTAN1"; // 80,
	case 81: return "KARUTAN2"; // 81, 
	case 82: return "DOUBLE_RENEWAL"; // 82, 
	case 91: return "ACHERON1"; // 91,
	case 92: return "ACHERON2"; // 92,
	case 95: return "DEVENTER1"; // 95,
	case 96: return "DEVENTER2"; // 96,
	case 97: return "CHAOS_CASTLE_FINAL"; // 97,
	case 98: return "ILLUSION_TEMPLE_FINAL1"; // 98,
	case 99: return "ILLUSION_TEMPLE_FINAL2"; // 99,
	case 100: return "URQUE_HILL"; // 100,
	case 110: return "DURBAN_TYLER"; // 110,	
	case 112: return "FEREA"; // 0; =112,
	case 113: return "KNICKS"; // 113,
	case 114: return "QUEST_ZONE_ENTRANCE"; // 114,
	case 115: return "Labyrinth"; // 115,
	case 116: return "DEEP_DUNGEON1"; // 116,
	case 117: return "DEEP_DUNGEON2"; // 117,
	case 118: return "DEEP_DUNGEON3"; // 118,
	case 119: return "DEEP_DUNGEON4"; // 119,
	case 120: return "DEEP_DUNGEON5"; // 120,
	case 121: return "4_TH_QUEST"; // 121,
	case 122: return "SWAMP_OF_DARKNESS"; //122,
	} 

	return "Unknown";
}


bool cAddons::IsBadFileLine(char* FileLine, int& Flag)
{
	if (Flag == 0)
	{
		for (unsigned int i = 0; i < strlen(FileLine); i++)
		{
			char checkingChar = FileLine[i];
			if (checkingChar == '/' || checkingChar == '\n' || checkingChar == '\0')
			{
				return true;
			}

			if (isdigit(checkingChar))
			{
				Flag = Flag * 10 + (checkingChar - 48);
			}
			else
			{
				return true;
			}
		}
		return true;
	}

	if (!strncmp(FileLine, "end", 3))
	{
		Flag = 0;
		return true;
	}

	if (FileLine[0] == '/' || FileLine[0] == '\n')
		return true;

	for (UINT i = 0; i < strlen(FileLine); i++)
	{
		if (isalnum(FileLine[i]))
			return false;
	}
	return true;
}

int cAddons::GenExcOpt(int amount)
{
	// User input errors
	if (amount > 6) amount = 6;
	if (amount < 1) amount = 1;

	int opt_db[6] = { 1, 2, 4, 8, 16, 32 };
	int exc = 0;

	std::random_shuffle(opt_db, opt_db + 6);

	for (int n = 0; n < amount; n++)
		exc += opt_db[n];

	return exc;
}

int cAddons::GetNumberByPercent(int Proc, int Min, int Max)
{
	int Random = rand() % 100;

	if (Proc == 0 || Max == Min)
		return Min;

	if (Random <= Proc)
		return Max;

	if (Proc > 50)
		return GetNumberByPercent(Proc / 2, Min, Max - 1);
	else if (Proc < 30)
		return GetNumberByPercent(Proc * 3 / 2, Min, Max - 1);
	else
		return GetNumberByPercent(Proc, Min, Max - 1);
}
