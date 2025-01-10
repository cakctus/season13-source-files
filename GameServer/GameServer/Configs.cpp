#include "StdAfx.h"
#include "Configs.h"
#include "Logger.h"
#include "GMSystem.h" 
#include "NewsSystem.h"
cConfigs Configs;


void cConfigs::LoadAll()
{     
	LoadCommands(); 
}  

void cConfigs::LoadCommands()
{
	//		drop		//
	Commands.IsDrop = GetInt(0, 1, 1, "Drop", "DropEnabled", cfCOMANDS);
	
	//		drop		//
	Commands.IsMake = GetInt(0, 1, 1, "Make", "MakeEnabled", cfCOMANDS);

	//		Reload		//
	Commands.IsReload = GetInt(0, 1, 1, "Reload", "ReloadEnabled", cfCOMANDS);

	//		gmove		//
	Commands.IsGmove = GetInt(0, 1, 1, "Gmove", "GmoveEnabled", cfCOMANDS);

	//		setpk
	Commands.IsSetPK = GetInt(0, 1, 1, "SetPK", "SetPKEnabled", cfCOMANDS);

	//		gg			//																	 	
	Commands.IsGg = GetInt(0, 1, 1, "GPost", "GgEnabled", cfCOMANDS);

	//		popup			//																	 	
	Commands.IsMessagePopup = GetInt(0, 1, 1, "PopUp", "PopUpMsgEnabled", cfCOMANDS);

	//		time		//
	Commands.IsTime = GetInt(0, 1, 1, "Time", "TimeEnabled", cfCOMANDS);

	//		online		//
	Commands.IsOnline = GetInt(0, 1, 1, "OnlineCommand", "OnlineEnabled", cfCOMANDS);

	//		status		//
	Commands.IsStatus = GetInt(0, 1, 1, "Status", "StatusEnabled", cfCOMANDS);

	//		SetChar		//
	Commands.IsSetChar = GetInt(0, 1, 1, "SetChar", "SetCharEnabled", cfCOMANDS);

	//		SetZen		//
	Commands.IsSetZen = GetInt(0, 1, 1, "SetZen", "SetZenEnabled", cfCOMANDS);
	 
	//		skin		//
	Commands.IsSkin = GetInt(0, 1, 1, "Skin", "SkinEnabled", cfCOMANDS);

	//		Hide		//
	Commands.IsHide = GetInt(0, 1, 1, "Hide", "HideEnabled", cfCOMANDS);

	//		Fireworks		//
	Commands.IsFireworks = GetInt(0, 1, 1, "Fireworks", "FireworksEnabled", cfCOMANDS);

	//		ClearInv		//
	Commands.IsClearInv = GetInt(0, 1, 1, "ClearInv", "ClearInvEnabled", cfCOMANDS);

	//		Ware		//
	Commands.IsWare = GetInt(0, 1, 1, "Ware", "WareEnabled", cfCOMANDS);

	//		Spawn		//
	Commands.IsSpawn = GetInt(0, 1, 1, "Spawn", "SpawnEnabled", cfCOMANDS);

	//		post		//	 
	Commands.IsPost = GetInt(0, 1, 1, "Post", "PostEnabled", cfCOMANDS);
	Commands.PostLvl = GetInt(0, MAX_LEVEL_CMD, 1, "Post", "PostLevelReq", cfCOMANDS);
	Commands.PostPriceZen = GetInt(0, 2000000000, 10000, "Post", "PostPriceZen", cfCOMANDS);
	Commands.PostColor = GetInt(1, 3, 1, "Post", "PostColor", cfCOMANDS);
	Commands.PostDelay = GetInt(0, 60000, 60, "Post", "PostDelay", cfCOMANDS);

	//		add			//
	Commands.AddPointEnabled = GetInt(0, 1, 1, "AddCommand", "AddPointEnabled", cfCOMANDS);
	Commands.AddPointLevelReq = GetInt(0, MAX_LEVEL_CMD, 0, "AddCommand", "AddPointLevelReq", cfCOMANDS);
	Commands.AddPriceZen = GetInt(0, 2000000000, 10000, "AddCommand", "AddPriceZen", cfCOMANDS);
	Commands.MaxAddedPoints = GetInt(0, 32000, 0, "AddCommand", "MaxAddedPoints", cfCOMANDS);
	Commands.MaxStats = GetInt(0, 32000, 0, "AddCommand", "MaxStats", cfCOMANDS);

	//		pkclear		//
	Commands.IsPkClear = Configs.GetInt(0, 1, 1, "PkClear", "PkClearEnabled", cfCOMANDS); 
	Commands.PkClearForGm = Configs.GetInt(0, 1, 0, "PkClear", "PkClearOnlyForGm", cfCOMANDS);
	Commands.PkClearType = Configs.GetInt(0, 2, 1, "PkClear", "PKClearType", cfCOMANDS);
	Commands.PkClearPriceZen = Configs.GetInt(0, 2000000000, 100000, "PkClear", "PkClearPriceZen", cfCOMANDS);
	Commands.PkClearPriceZenForAll = Configs.GetInt(0, 2000000000, 1000000, "PkClear", "PkClearPriceZenForAll", cfCOMANDS);
	Commands.PkClearLevelReq = Configs.GetInt(0, MAX_LEVEL_CMD, 100, "PkClear", "PkClearLevelReq", cfCOMANDS);

}
 

long cConfigs::GetInt(long Min, long Max, long Default, LPCSTR BlockName, LPCSTR ConfigName, LPCSTR FolderName)
{
	long lResult;

	WIN32_FIND_DATAA wfd;
	if (FindFirstFileA(FolderName, &wfd) == INVALID_HANDLE_VALUE)
	{
		Log.ConsoleOutPut(1, c_BoldGreen, t_Error, "[X] Error configs in %s!", FolderName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Can't find file!");
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Taked default value for %s(%ld)", ConfigName, Default);
		lResult = Default;
	}
	else
		lResult = GetPrivateProfileInt(BlockName, ConfigName, -100500, FolderName);

	if (lResult == -100500)
	{
		Log.ConsoleOutPut(1, c_BoldGreen, t_Error, "[X] Error configs in %s!", FolderName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Can't find '%s' in [%s] section!", ConfigName, BlockName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Taked default value for %s(%ld)", ConfigName, Default);
		lResult = Default;
	}

	if (lResult < Min || lResult > Max)
	{
		Log.ConsoleOutPut(1, c_BoldGreen, t_Error, "[X] Error configs in %s!", FolderName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "%s(%d) in [%s] is out of range!", ConfigName, lResult, BlockName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Min = %ld Max = %ld\tTaked default value(%ld)", Min, Max, Default);
		lResult = Default;
	}
	return lResult;
}

char cConfigs::GetChar(long Min, long Max, unsigned char Default, LPCSTR BlockName, LPCSTR ConfigName, LPCSTR FolderName)
{
	unsigned char cResult;

	WIN32_FIND_DATAA wfd;
	if (FindFirstFileA(FolderName, &wfd) == INVALID_HANDLE_VALUE)
	{
		Log.ConsoleOutPut(1, c_BoldGreen, t_Error, "[X] Error configs in %s!", FolderName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Can't find file!");
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] taked default value for %s(%ld)", ConfigName, Default);
		cResult = Default;
	}
	else
		cResult = GetPrivateProfileInt(BlockName, ConfigName, -100500, FolderName);

	if (cResult == -100500)
	{
		Log.ConsoleOutPut(1, c_BoldGreen, t_Error, "[X] Error configs in %s!", FolderName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Can't find '%s' in [%s] section!", ConfigName, BlockName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Taked default value for %s(%ld)", ConfigName, Default);
		cResult = Default;
	}

	if (cResult < Min || cResult > Max)
	{
		Log.ConsoleOutPut(1, c_BoldGreen, t_Error, "[X] Error configs in %s!", FolderName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] %s(%d) in [%s] is out of range!", ConfigName, cResult, BlockName);
		Log.ConsoleOutPut(1, c_BoldRed, t_Error, "[X] Min = %ld Max = %ld\tTaked default value(%ld)", Min, Max, Default);
		cResult = Default;
	}
	return cResult;
}

/*
void cConfigs::LoadFixes()
{

	// Eldoradi event conf
	EldaradoEventType = GetInt(0, 1, 0, "EldaradoEvent", "EldaradoEventType", IAJuliaEvents);

	// Other event conf
	CCAllowingPlayers = GetInt(0, 2, 0, "ChaosCastle", "ChaosCastleAllowingPlayers", IAJuliaEvents);
	BCAllowingPlayers = GetInt(0, 2, 0, "BloodCastle", "BloodCastleAllowingPlayers", IAJuliaEvents);
	BCEnterMasterType = GetInt(0, 1, 0, "BloodCastle", "BloodCastleMasterEnterType", IAJuliaEvents);
	DSAllowingPlayers = GetInt(0, 2, 0, "DevilSquare", "DevilSquareAllowingPlayers", IAJuliaEvents);

	UseChecksum = GetInt(0, 1, 0, "CheckSum", "UseCheckSum", IAJuliaCommon);
	PersonalIDFix = GetInt(0, 1, 1, "Bypasseres", "PersonalIDBypasser", IAJuliaCommon);
	GuildIDFix = GetInt(0, 1, 1, "Bypasseres", "GuildIDBypasser", IAJuliaCommon);
	DisableLogs = GetInt(0, 1, 0, "Logs", "DisableLogs", IAJuliaCommon);
	AllowExeAnc = GetInt(0, 1, 1, "SpecialItem", "AllowExeAnc", IAJuliaItems);
	AllowJohAnc = GetInt(0, 1, 1, "SpecialItem", "AllowJohAnc", IAJuliaItems);
	AllowExeSock = GetInt(0, 1, 1, "SpecialItem", "AllowExeSock", IAJuliaItems);
	MaxLifeOpt = GetInt(0, 1, 1, "SpecialItem", "MaxLifeOpt", IAJuliaItems);
	CSSkillsOnNOrmal = GetInt(0, 1, 1, "Skills", "CSSkillsOnNOrmal", IAJuliaSkills);
	Enable65kStats = GetInt(0, 1, 0, "AddCommand", "Enable65kStats", IAJuliaCmd);
	DumpFile = GetInt(0, 1, 0, "Dump", "EnableCrashDump", IAJuliaCommon);
	Disable380Opt = GetInt(0, 1, 0, "380Opt", "DisablePowers", IACalcCharacter);
	DisableHarmonyOpt = GetInt(0, 1, 0, "HarmonyOpt", "DisablePowers", IACalcCharacter);

	IsPartyGap = GetInt(0, 1, 1, "PartyGap", "IsPartyGap", IAJuliaCommon);
	PartyGapLvl = GetInt(0, 400, 120, "PartyGap", "PartyGapLvl", IAJuliaCommon);
	GuildRes = GetInt(0, 32767, 5, "GuildMaster", "GuildCreateReset", IAJuliaCommon);
	Unicode32 = GetInt(0, 1, 0, "CharacterGuild", "AllowUnicode32", IAJuliaCommon);
	GuildLevel = GetInt(0, 400, 320, "GameServerInfo", "GuildCreateLevel", "..\\Data\\CommonServer.cfg");

}
*/