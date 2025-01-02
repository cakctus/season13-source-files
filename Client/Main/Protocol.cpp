#include "Stdafx.h"
#include "Protocol.h"
#include "PacketManager.h"
#include "Reconnect.h"
#include "Util.h"
#include "Console.h"
#include "CustomFunctions.h"
#include "CastleSiege.h"
#include "..\\..\\Util\\CCRC32.H"
#include <Interface.h>
#include "PrintPlayer.h"
#include "HackCheck.h"
#include "Offset.h"
#include "EventSchedule.h"
#include "ScreenShot.h"
#include "Serial.h"
#include "Oficina.h"

bool HardwareIdSent = false;

CrownSwitchData g_CrownSwitchData[3];

void InitProtocolCoreEx()
{
	SetCompleteHook(0xE9, 0x00BE443A, &SendPacket);
	SetCompleteHook(0xE9, 0x00C48E76, &ParsePacket);
}

BOOL ProtocolCoreEx(BYTE head, BYTE* lpMsg, int len, int Encrypt)
{
	int logProtocol = GetPrivateProfileInt("CONSOLE", "LogProtocol", 0, ".\\Config.ini");
	if (logProtocol == 1) {
		Log.ConsoleOutPut(1, c_Red, t_Default, "[RECVClient]: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x"
		, lpMsg[0], lpMsg[1], lpMsg[2], lpMsg[3], lpMsg[4], lpMsg[5], lpMsg[6]
		, lpMsg[7], lpMsg[8], lpMsg[9], lpMsg[10], lpMsg[11], lpMsg[12], lpMsg[13]
		, lpMsg[14], lpMsg[15], lpMsg[16], lpMsg[17], lpMsg[18], lpMsg[19]
			, lpMsg[20], lpMsg[21], lpMsg[22], lpMsg[23], lpMsg[24], lpMsg[25], lpMsg[26]
			, lpMsg[27], lpMsg[28], lpMsg[29]);
	}

	if (lpMsg[0] == 0xC1 && (head == 0xF3 || head == 0xF8 || head == 0x1C || head == 0xD6 || head == 0x15 || head == 0x10))
	{
		avoidByProtocol = GetTickCount64();
	}

	switch (head)
	{

	case 0x71:
	{
		PMSG_PING* ping = (PMSG_PING*)lpMsg;
		gInterface.ping = ping->ping;
		//Log.ConsoleOutPut(1, c_Red, t_Default, "[Ping]: %d", gInterface.ping);
	} break;

	case 0xB1:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
		case 0x00:
			GCMapServerMoveRecv((PMSG_MAP_SERVER_MOVE_RECV*)lpMsg);
			break;
		case 0x01:
			GCMapServerMoveAuthRecv((PMSG_MAP_SERVER_MOVE_AUTH_RECV*)lpMsg);
			break;
		}
	break;

	case 0xB2:
	{
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
			case 0x14:
			{
				CASTLE_SIEGE_SWITCH_STATE* lpData = (CASTLE_SIEGE_SWITCH_STATE*)lpMsg;
				g_CastleSiege.SetSwitchData(lpData->state, lpData->playerH * 512 + lpData->playerL, lpData->switchH * 512 + lpData->switchL);
				return 1;
			} break;

			case 0x15:
			{
				CROWN_ACCESS_STATE* lpData = (CROWN_ACCESS_STATE*)lpMsg;
				g_CastleSiege.SetCrownMasterData(lpData->state, lpData->accumulated_time);
				return 1;
			} break;

			case 0x16:
			{
				CROWN_STATE* lpData = (CROWN_STATE*)lpMsg;
				g_CastleSiege.SetCrownData(lpData->state);
				return 1;
			} break;

			case 0x18: //green message when click crown and register
			{
				return 1;
			} break;

			case 0x20:
			{
				GCSetCrownSwitchData(lpMsg);
				return 1;
			} break;
		}
	} 
	break;

	case 0xD8:
	{
		HandleElementalDamage(lpMsg);
	} break;

	case 0xDF:
	{
		HandleDamage(lpMsg);
	} break;

	case 0x26:
	{
		HandleHP(lpMsg);
	} break;

	case 0xF1:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
		case 0x00:
			GCConnectClientRecv((PMSG_CONNECT_CLIENT_RECV*)lpMsg);
			break;
		case 0x01:
			GCConnectAccountRecv((PMSG_CONNECT_ACCOUNT_RECV*)lpMsg);
			break;
		case 0x02:
			GCCloseClientRecv((PMSG_CLOSE_CLIENT_RECV*)lpMsg);
			break;
		}
	break;

	case 0xF3:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
		case 0x00:
			GCCharacterListRecv((PMSG_CHARACTER_LIST_RECV*)lpMsg);
			break;
		case 0x03:
			GCCharacterInfoRecv((PMSG_CHARACTER_INFO_RECV*)lpMsg);
			break;
		case 0x04:
			HandleRespawn(lpMsg);
			break;
		case 0x05:
			GCCharacterLevelUpRecv((PMSG_LEVEL_UP_RECV*)lpMsg);
			break;
		case 0x06:
			HandleLevelUpPoint(lpMsg);
			break;
		case 0x51:
			GCCharacterMasterLevelUpRecv((PMSG_MASTER_LEVEL_UP_RECV*)lpMsg);
			break;
		case 0xE0:
			GCNewCharacterInfoRecv((PMSG_NEW_CHARACTER_INFO_RECV*)lpMsg);
			return 1;
		case 0xE1:
			GCNewMessageRecv((PMSG_NEW_MESSAGE_RECV*)lpMsg);
			return 1;
		case 0xE2:
			GCMessagePopupRecv((PMSG_MESSAGE_POPUP_RECV*)lpMsg);
			return 1;
		case 0xE3:
			HandleBossRanking(lpMsg);
			break;
		case 0xE5:
		{
			PMSG_PIN_AUTH* pinMsg = (PMSG_PIN_AUTH*)lpMsg;
			//Log.ConsoleOutPut(1, c_Red, t_Default, "PIN: %s", pinMsg->pin);
			WritePrivateProfileString("PIN", UserAccount, pinMsg->pin, ".\\Config.ini");
		}
			break;
		case 0xE6:
			gEventSchedule.ProcessScheduleData((PMSG_SCHEDULE_EVENTS*)lpMsg);
			break;
		case 0xE7:
			gEventSchedule.ProcessScheduleData2((PMSG_SCHEDULE_INVASIONS*)lpMsg);
			break;
		case 0xE8:
			gSS.PrintScreen();
			break;
		case 0xE9:
			gOficina.ReceiveOficinaData((PMSG_OFICINA_OPEN*)lpMsg);
			break;
		case 0xEA:
			gOficina.CloseAndResetAll();
			break;
		}
	break;
	}

	return ProtocolCore(head,lpMsg,len,Encrypt);
}

void GCMapServerMoveRecv(PMSG_MAP_SERVER_MOVE_RECV* lpMsg) // OK
{
	ReconnectOnMapServerMove(lpMsg->IpAddress, lpMsg->ServerPort);
}

void GCMapServerMoveAuthRecv(PMSG_MAP_SERVER_MOVE_AUTH_RECV* lpMsg) // OK
{
	ReconnectOnMapServerMoveAuth(lpMsg->result);
}

void GCConnectAccountRecv(PMSG_CONNECT_ACCOUNT_RECV* lpMsg) // OK
{
	ReconnectOnConnectAccount(lpMsg->result);
}

void GCCloseClientRecv(PMSG_CLOSE_CLIENT_RECV* lpMsg) // OK
{
	ReconnectOnCloseClient(lpMsg->result);
}

void GCCharacterListRecv(PMSG_CHARACTER_LIST_RECV* lpMsg) // OK
{
	ReconnectOnCharacterList();
}

void GCCharacterLevelUpRecv(PMSG_LEVEL_UP_RECV* lpMsg)
{
	LevelUpPoints = lpMsg->ViewPoint;

	char* msg = (char*)malloc(sizeof(char[64]));

	wsprintf(msg, "L:%d | M:%d | R:%d | G:%d", lpMsg->Level, lpMsg->MasterLevel, Resets, GResets);

	LevelText = msg;
}

void GCCharacterMasterLevelUpRecv(PMSG_MASTER_LEVEL_UP_RECV* lpMsg)
{
	char* msg = (char*)malloc(sizeof(char[64]));

	wsprintf(msg, "L:%d | M:%d | R:%d | G:%d", lpMsg->Level, lpMsg->MasterLevel, Resets, GResets);

	LevelText = msg;
}

void GCCharacterInfoRecv(PMSG_CHARACTER_INFO_RECV* lpMsg) // OK
{
	ReconnectOnCharacterInfo();

	*(short*)(*(DWORD*)(MAIN_VIEWPORT_STRUCT)+0x3E) = 0;
	*(BYTE*)(*(DWORD*)(MAIN_VIEWPORT_STRUCT)+0x408) = 0;
	
	Resets = lpMsg->ViewReset;
	GResets = lpMsg->ViewMasterReset;

	ViewHP = lpMsg->ViewCurHP;
	ViewMaxHP = lpMsg->ViewMaxHP;
	LevelUpPoints = lpMsg->ViewPoint;

	//Log.ConsoleOutPut(1, c_Blue, t_Default, "GCCharacterInfoRecv Life %d/%d", ViewHP, ViewMaxHP);
	//Log.ConsoleOutPut(1, c_Blue, t_Default, "Account: %s", UserAccount);

	char pin[8];
	GetPrivateProfileString("PIN", UserAccount, "-", pin, sizeof(pin), ".\\Config.ini");

	if (pin[0] != '-')
	{
		Log.ConsoleOutPut(1, c_Blue, t_Default, "Account: %s | PIN: %s", UserAccount,pin);
		PMSG_PIN_AUTH pinSend;
		pinSend.h.set(0xF3, 0xE5, sizeof(pinSend));
		strcpy_s(pinSend.pin, 8, pin);
		pinSend.pin[8] = '\0';
		DataSend((LPBYTE)&pinSend, pinSend.h.size);
	}
	else
	{
		Log.ConsoleOutPut(1, c_Blue, t_Default, "PIN non ecziste para %s", UserAccount);
	}	

	char * msg = (char*)malloc(sizeof(char[64]));

	wsprintf(msg, "L:%d | M:%d | R:%d | G:%d", lpMsg->ViewLevel, lpMsg->ViewMasterLevel, lpMsg->ViewReset, lpMsg->ViewMasterReset);

	LevelText = msg;

	//if (HardwareIdSent == false)
	//{
	//	HardwareIdSent = true;

		HARDWARE_ID_SEND pResult;
		pResult.header.set(0x79, sizeof(pResult));
		ZeroMemory(pResult.HarwareId, sizeof(pResult.HarwareId));
		char* hwId = GetHardwareId();
		strcpy_s(pResult.HarwareId, 45, hwId);
		pResult.HarwareId[44] = '\0';

		DataSend((LPBYTE)&pResult, pResult.header.size);

		//Log.ConsoleOutPut(1, c_Blue, t_Default, "HarwareId %s", pResult.HarwareId);
		
		CCRC32 CRC32;
		DWORD PluginCRC32;

		if (CRC32.FileCRC("Data\\Player\\player.bmd", &PluginCRC32, 1024) == 0)
		{
			Log.ConsoleOutPut(1, c_Blue, t_Default, "CRC32.FileCRC");
			ExitProcess(0);
		}

		if (PluginCRC32 != 0x60211644)
		{
			Log.ConsoleOutPut(1, c_Blue, t_Default, "PluginCRC32 %08X", PluginCRC32);
			ExitProcess(0);
		}
	//}	
}

void GCNewCharacterInfoRecv(PMSG_NEW_CHARACTER_INFO_RECV* lpMsg) // OK
{
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+0) = lpMsg->Level;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+474) = lpMsg->LevelUpPoint;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+398) = lpMsg->Strength;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+400) = lpMsg->Dexterity;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+402) = lpMsg->Vitality;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+404) = lpMsg->Energy;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+406) = lpMsg->Leadership;
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+108) = lpMsg->Life;
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+116) = lpMsg->MaxLife;
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+112) = lpMsg->Mana;
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+120) = lpMsg->MaxMana;
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+132) = lpMsg->BP;
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+136) = lpMsg->MaxBP;
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+124) = lpMsg->Shield;
	*(DWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+128) = lpMsg->MaxShield;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+434) = lpMsg->FruitAddPoint;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+436) = lpMsg->MaxFruitAddPoint;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+438) = lpMsg->FruitSubPoint;
	*(WORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+440) = lpMsg->MaxFruitSubPoint;
	*(QWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+8) = (QWORD)lpMsg->Experience;
	*(QWORD*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+16) = (QWORD)lpMsg->NextExperience;

	ViewHP = lpMsg->ViewCurHP;
	ViewMaxHP = lpMsg->ViewMaxHP;
	LevelUpPoints = lpMsg->ViewPoint;

	//Log.ConsoleOutPut(1, c_Blue, t_Default, "GCNewCharacterInfoRecv Life %d/%d", ViewHP, ViewMaxHP);

	Resets = lpMsg->ViewReset;
	GResets = lpMsg->ViewMasterReset;

	char* msg = (char*)malloc(sizeof(char[64]));

	wsprintf(msg, "L:%d | M:%d | R:%d | G:%d", lpMsg->ViewLevel, lpMsg->ViewMasterLevel, lpMsg->ViewReset, lpMsg->ViewMasterReset);

	LevelText = msg;
}

void DataSend(BYTE* lpMsg, int size)
{
	BYTE EncBuff[2048];

	if (gPacketManager.AddData(lpMsg, size) != 0 && gPacketManager.ExtractPacket(EncBuff) != 0)
	{
		BYTE send[2048];

		memcpy(send, EncBuff, size);

		if (EncBuff[0] == 0xC3 || EncBuff[0] == 0xC4)
		{
			if (EncBuff[0] == 0xC3)
			{
				BYTE save = EncBuff[1];

				EncBuff[1] = (*(BYTE*)(MAIN_PACKET_SERIAL))++;

				size = gPacketManager.Encrypt(&send[2], &EncBuff[1], (size - 1)) + 2;

				EncBuff[1] = save;

				send[0] = 0xC3;
				send[1] = LOBYTE(size);
			}
			else
			{
				BYTE save = EncBuff[2];

				EncBuff[2] = (*(BYTE*)(MAIN_PACKET_SERIAL))++;

				size = gPacketManager.Encrypt(&send[3], &EncBuff[2], (size - 2)) + 3;

				EncBuff[2] = save;

				send[0] = 0xC4;
				send[1] = HIBYTE(size);
				send[2] = LOBYTE(size);
			}
		}

		((void(__thiscall*)(void*, BYTE*, DWORD))0x00BE4865)((void*)0x0A1EA680, send, size);
	}
}

//WORD skillsToCheck[10] = { 41, 724, 235, 737, 215, 263, 277 };

void SendPacket(BYTE* lpMsg, DWORD size, int enc, int unk1)
{
	//Log.ConsoleOutPut(1, c_Blue, t_Default, "2: %02X | 3:%02X | 4:%02X | 5:%02X | 6:%02X | 7:%02X | 8:%02X | 9:%02X | 10:%02X", lpMsg[2], lpMsg[3], lpMsg[4], lpMsg[5], lpMsg[6], lpMsg[7], lpMsg[8], lpMsg[9], lpMsg[10]);
	
	if (lpMsg[2] == 0x19)
	{
		attackSkillCount++;
	}
	else if (lpMsg[2] == 0x1E)
	{
		durationSkillCount++;
	}
	else if (lpMsg[2] == 0xDF)
	{
		attackCount++;
	}	

	if (enc)
	{
		BYTE *send = (BYTE*)alloca(6148);
	
		memcpy(send, lpMsg, size);
	
		if (lpMsg[0] == 0xC1)
		{
			BYTE save = lpMsg[1];
	
			lpMsg[1] = (*(BYTE*)(MAIN_PACKET_SERIAL))++;
	
			size = gPacketManager.Encrypt(&send[2], &lpMsg[1], (size - 1)) + 2;
	
			lpMsg[1] = save;
	
			send[0] = 0xC3;
			send[1] = LOBYTE(size);
		}
		else if (lpMsg[0] == 0xC2)
		{
			BYTE save = lpMsg[2];
	
			lpMsg[2] = (*(BYTE*)(MAIN_PACKET_SERIAL))++;
	
			size = gPacketManager.Encrypt(&send[3], &lpMsg[2], (size - 2)) + 3;
	
			lpMsg[2] = save;
	
			send[0] = 0xC4;
			send[1] = HIBYTE(size);
			send[2] = LOBYTE(size);
		}
	
		((void(__thiscall*)(void*, BYTE*, DWORD))0x00BE4865)((void*)0x0A1EA680, send, size);
	}
	else
	{
		((void(__thiscall*)(void*, BYTE*, DWORD))0x00BE4865)((void*)0x0A1EA680, lpMsg, size);
	}
}

void ParsePacket(void* PackStream, int ChatServer, int ChatRoom)
{
	static int head, size, enc, DecSize;

	BYTE* recv = (BYTE*)alloca(11416);

	BYTE* lpMsg;

	while ((lpMsg = pParsePacket(PackStream)) != 0)
	{
		head = -1;
		size = enc = DecSize = 0;

		switch (lpMsg[0])
		{
		case 0xC1:
			head = lpMsg[2];
			size = lpMsg[1];
			enc = 0;
			break;
		case 0xC2:
			head = lpMsg[3];
			size = MAKE_NUMBERW(lpMsg[1], lpMsg[2]);
			enc = 0;
			break;
		case 0xC3:
			enc = 1;
			size = lpMsg[1];
			DecSize = gPacketManager.Decrypt(&recv[1], &lpMsg[2], size - 2);
			recv[0] = 0xC1;
			recv[1] = SET_NUMBERLB(DecSize + 2);
			size = DecSize + 2;
			lpMsg = recv;
			head = lpMsg[2];
			break;
		case 0xC4:
			enc = 1;
			size = MAKE_NUMBERW(lpMsg[1], lpMsg[2]);
			DecSize = gPacketManager.Decrypt(&recv[2], &lpMsg[3], size - 3);
			recv[0] = 0xC2;
			recv[1] = SET_NUMBERHB(DecSize + 3);
			recv[2] = SET_NUMBERLB(DecSize + 3);
			size = DecSize + 3;
			lpMsg = recv;
			head = lpMsg[3];
			break;
		}

		if (ChatServer == 1)
		{
			pChatProtocolCore(ChatRoom, head, lpMsg, size, enc);
		}
		else
		{
			ProtocolCoreEx(head, lpMsg, size, enc);
		}
	}
}

void GCNewMessageRecv(PMSG_NEW_MESSAGE_RECV* lpMsg) // OK
{
	WzMessageSend(lpMsg->type, lpMsg->message, "");
}

void GCMessagePopupRecv(PMSG_MESSAGE_POPUP_RECV* lpMsg)
{
	CustomFunctions::CallClientMessagePopup(lpMsg->message);
}

char * GetHardwareId()
{
	Log.ConsoleOutPut(1, c_Green, t_Default, "Serial %s", CSerial::GetSerial());
	HardwareId = CSerial::GetSerial();
	return HardwareId;
}

void GCSetCrownSwitchData(BYTE* Packet)
{
	CASTLE_SIEGE_SWITCH_INFO* lpMsg = (CASTLE_SIEGE_SWITCH_INFO*)Packet;

	if (lpMsg->id >= 2)
	{
		return;
	}

	g_CrownSwitchData[2].state = lpMsg->crownState;

	g_CrownSwitchData[lpMsg->id].index = lpMsg->switch_index;
	g_CrownSwitchData[lpMsg->id].state = lpMsg->state;
	g_CrownSwitchData[lpMsg->id].join_side = lpMsg->join_side;

	if (!g_CrownSwitchData[lpMsg->id].state)
	{
		g_CrownSwitchData[lpMsg->id].Reset();
	}
	else
	{
		memcpy(g_CrownSwitchData[lpMsg->id].name, lpMsg->user, 11);
		g_CrownSwitchData[lpMsg->id].name[10] = '\0';
		memcpy(g_CrownSwitchData[lpMsg->id].guild, lpMsg->guild, 9);
		g_CrownSwitchData[lpMsg->id].guild[8] = '\0';
	}

	if (!g_CrownSwitchData[2].state)
	{
		g_CrownSwitchData[2].Reset();
	}
	else
	{
		memcpy(g_CrownSwitchData[2].name, lpMsg->crownUser, 11);
		g_CrownSwitchData[2].name[10] = '\0';
		memcpy(g_CrownSwitchData[2].guild, lpMsg->crownGuild, 9);
		g_CrownSwitchData[2].guild[8] = '\0';
		g_CrownSwitchData[2].accumulated_time = lpMsg->accumulated_time;
	}
}

void HandleElementalDamage(BYTE* Packet)
{
	PMSG_ELEMENTAL_DAMAGE_RECV* lpMsg = (PMSG_ELEMENTAL_DAMAGE_RECV*)Packet;

	int aIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]) & 0x7FFF;

	if (ViewIndex == aIndex)
	{
		ViewHP = lpMsg->ViewCurHP;
		ViewSD = lpMsg->ViewCurSD;
		//Log.ConsoleOutPut(1, c_Blue, t_Default, "HandleElementalDamage Life %d/%d", ViewHP, ViewMaxHP);
	}
}

void HandleDamage(BYTE* Packet)
{
	PMSG_DAMAGE_RECV* lpMsg = (PMSG_DAMAGE_RECV*)Packet;

	int aIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]) & 0x7FFF;

	if (ViewIndex == aIndex)
	{
		ViewHP = lpMsg->ViewCurHP;
		ViewSD = lpMsg->ViewCurSD;
		//Log.ConsoleOutPut(1, c_Blue, t_Default, "HandleDamage Life %d/%d", ViewHP, ViewMaxHP);
	}
}

void HandleHP(BYTE* Packet)
{
	PMSG_LIFE_RECV* lpMsg = (PMSG_LIFE_RECV*)Packet;

	if (lpMsg->type == 0xFE)
	{
		ViewMaxHP = lpMsg->ViewHP;
		ViewMaxSD = lpMsg->ViewSD;
		//Log.ConsoleOutPut(1, c_Green, t_Default, "HandleHP MAX Life %d", ViewMaxHP);
	}
	else if (lpMsg->type == 0xFF)
	{
		ViewHP = lpMsg->ViewHP;
		ViewSD = lpMsg->ViewSD;
		//Log.ConsoleOutPut(1, c_Blue, t_Default, "HandleHP Life %d", ViewHP);
	}
}

void HandleRespawn(BYTE* Packet)
{
	PMSG_CHARACTER_REGEN_RECV* lpMsg = (PMSG_CHARACTER_REGEN_RECV*)Packet;

	ViewHP = lpMsg->ViewCurHP;
	ViewMP = lpMsg->ViewCurMP;
	ViewSD = lpMsg->ViewCurSD;
	ViewAG = lpMsg->ViewCurBP;

	//Log.ConsoleOutPut(1, c_Blue, t_Default, "HandleSpawn %d/%d", ViewHP, ViewMaxHP);
}

void HandleLevelUpPoint(BYTE* Packet)
{
	PMSG_LEVEL_UP_POINT_RECV* lpMsg = (PMSG_LEVEL_UP_POINT_RECV*)Packet;
	LevelUpPoints = lpMsg->ViewPoint;
}

void HandleBossRanking(BYTE* Packet)
{
	gInterface.BossTimer = GetTickCount64();

	gInterface.sRank = -1;
	
	PMSG_TARGET_BOSS_DATA* lpMsg = (PMSG_TARGET_BOSS_DATA*)Packet;

	gInterface.Count = lpMsg->Count;

	gInterface.Rank[0] = 0;
	strcpy(gInterface.Rank, lpMsg->Rank);
	gInterface.RankColor = lpMsg->RankColor;

	//Log.ConsoleOutPut(1, c_Blue, t_Default, "Boss %s", gInterface.Rank);
	//Log.ConsoleOutPut(1, c_Blue, t_Default, "Count %d", gInterface.Count);

	gInterface.Level = lpMsg->Level;

	gInterface.Life = lpMsg->Life;
	gInterface.MaxLife = lpMsg->MaxLife;

	gInterface.sRank = lpMsg->sRank;
	gInterface.sDamage = lpMsg->sDamage;

	for(int i = 0 ; i < gInterface.Count ; i++)
	{
		gInterface.Damage[i] = lpMsg->Damage[i];
		strcpy(gInterface.Name[i], lpMsg->Name[i]);
		gInterface.Name[i][10] = '\0';
	}
}

void GCConnectClientRecv(PMSG_CONNECT_CLIENT_RECV* lpMsg)
{
	ViewIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);
	//Log.ConsoleOutPut(1, c_Blue, t_Default, "index %d", ViewIndex);
}

