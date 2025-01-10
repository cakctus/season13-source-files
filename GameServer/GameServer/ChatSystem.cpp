#include "StdAfx.h"
 
#include "ChatSystem.h"
#include "Configs.h"
#include "Logger.h"
#include "Addons.h"
#include "Util.h"
#include "Message.h"
#include "ESProtocol.h"
#include "DSProtocol.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Guild.h"
#include "Move.h"
#include "Monster.h"
#include "NewsSystem.h"
#include "ServerInfo.h"
#include "DropSystem.h"
cChat Chat;

cChat::cChat()
{
}

cChat::~cChat()
{
}

void cChat::AntiInject(char* message)
{
	for (UINT i = 0; i <= strlen(message); i++)
	{
		if (message[i] == '\'')
		{
			message[i] = ' ';
		}
	}
}

bool cChat::CheckMessageForCommand(const char* message, const char* command)
{
	return !memcmp(message, command, strlen(command));
}

bool cChat::WisperChat(LPOBJ lpObj, PMSG_CHAT_WHISPER_RECV* lpMsg)
{
	/*
		lpObj			- object struct of sender
		lpMsg->id		- nickname of receiver
		lpMsg->message	- msg
	*/
	//Log.ConsoleOutPut(0, c_Red, t_NULL, "WISPER [%d][%s]: %s", aIndex, lpMsg->id, lpMsg->message);

	AntiInject(lpMsg->message);

	bool bResult = false;


	char TempName[11] = {0};
	strncpy_s(TempName, lpMsg->name, 10);
	int Index = Addons.GetPlayerIndex(TempName);

	if (CheckMessageForCommand(lpMsg->message, "@") 
		|| CheckMessageForCommand(lpMsg->message, "@@") 
		|| CheckMessageForCommand(lpMsg->message, "~"))
	{
		PMSG_CHAT_RECV pMsg = { 0 };

		pMsg.header.set(0x00, sizeof(pMsg));
		memcpy(pMsg.name, lpMsg->name, sizeof(pMsg.name));
		memcpy(pMsg.message, lpMsg->message, sizeof(pMsg.message));


		CGChatRecv(&pMsg, lpObj->Index);
		return true;
	}
	
	char* messageWithoutCommand = strstr(lpMsg->message, " ") != nullptr
		                              ? strstr(lpMsg->message, " ") + 1
		                              : "";
	
	
	if (CheckMessageForCommand(lpMsg->message, "/gg"))
		bResult = GgCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpMsg->message, "!"))
		bResult = VosklCommand(lpObj, messageWithoutCommand);
			
	if (CheckMessageForCommand(lpMsg->message, "/trace"))
		bResult = TraceCommand(lpObj, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/disconnect"))
		bResult = DiskCommand(lpObj, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/track"))
		bResult = SummonCommand(lpObj, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/level"))
		bResult = LevelCommand(lpObj);
	
	if (CheckMessageForCommand(lpMsg->message, "/post"))
		bResult = PostCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpMsg->message, "/time"))
		bResult = TimeCommand(lpObj);
	
	if (CheckMessageForCommand(lpMsg->message, "/setchar"))
		bResult = SetCharCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/setpk"))
		bResult = SetPKCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/setzen"))
		bResult = SetZenCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/skin"))
		bResult = SkinCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/drop"))
		bResult = DropCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/setdrop"))
		bResult = SetDropCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/make"))
		bResult = MakeCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/item"))
		bResult = MakeCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/setmake"))
		bResult = SetMakeCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/gmove"))
		bResult = GmoveCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/online"))
		bResult = OnlineCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpMsg->message, "/status"))
		bResult = StatusCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/reload"))
		bResult = ReloadCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpMsg->message, "/pkclear"))
		bResult = PKClearCommand(lpObj, messageWithoutCommand, Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/str"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 0);
	
	if (CheckMessageForCommand(lpMsg->message, "/agi"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 1);
	
	if (CheckMessageForCommand(lpMsg->message, "/vit"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 2);
	
	if (CheckMessageForCommand(lpMsg->message, "/ene"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 3);
	
	if (CheckMessageForCommand(lpMsg->message, "/cmd"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 4); 
	
	if (CheckMessageForCommand(lpMsg->message, "/soccer"))
		bResult = CommandBattleSoccer(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpMsg->message, "/war"))
		bResult = CommandGuildWar(lpObj, messageWithoutCommand);

	if (CheckMessageForCommand(lpMsg->message, "/re"))
		bResult = CommandRequest(lpObj, lpMsg->message + strlen("/re"));

	if (CheckMessageForCommand(lpMsg->message, "/move"))
		bResult = CommandMove(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpMsg->message, "/hide"))
		bResult = CommandHide(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/fireworks"))
		bResult = CommandFireworks(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpMsg->message, "/clearinv"))
		bResult = CommandClearInventory(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpMsg->message, "/ware"))
		bResult = CommandWare(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpMsg->message, "/spawn"))
		bResult = CommandSpawn(lpObj, messageWithoutCommand); 
	
	if (CheckMessageForCommand(lpMsg->message, "@>"))
		bResult = GuildPost(lpObj, messageWithoutCommand);
			
	if (CheckMessageForCommand(lpMsg->message, "/popup"))
		bResult = MessagePopup(lpObj, messageWithoutCommand, Index);

	  

	if (lpObj->BanChat)
	{
		lpObj->Penalty |= 2;
		Chat.MessageLog(1, c_Blue, t_BAN, lpObj, "[BanChat] Your chat is banned!");
		return true;
	}
	else
	{
		lpObj->Penalty &= ~2;
	}

	if (Index != -1)
		PrivateLog(lpObj, TempName, lpMsg, true);
	else
		PrivateLog(lpObj, TempName, lpMsg, false);

	return bResult;
}

bool cChat::ChatDataSend(LPOBJ lpObj, PMSG_CHAT_RECV* lpChat)
{
	/*
		lpObj			- object struct of sender
		lpMsg->id		- nickname of sender
		lpMsg->message	- msg
	*/
	//Log.ConsoleOutPut(0, c_Red, t_NULL, "CHAT [%d][%s]: %s", lpObj->Index, test->chatid, test->message);

	AntiInject(lpChat->message);

	bool bResult = false;

	char* messageWithoutCommand = strstr(lpChat->message, " ") != nullptr
		                              ? strstr(lpChat->message, " ") + 1
		                              : "";
	
	if (CheckMessageForCommand(lpChat->message, "/gg"))
		bResult = GgCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "!"))
		bResult = VosklCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/level"))
		bResult = LevelCommand(lpObj);
	
	if (CheckMessageForCommand(lpChat->message, "/post"))
		bResult = PostCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/time"))
		bResult = TimeCommand(lpObj);
	
	if (CheckMessageForCommand(lpChat->message, "/setchar"))
		bResult = SetCharCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/setpk"))
		bResult = SetPKCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/setzen"))
		bResult = SetZenCommand(lpObj, messageWithoutCommand, lpObj->Index); 
	
	if (CheckMessageForCommand(lpChat->message, "/skin"))
		bResult = SkinCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/drop"))
		bResult = DropCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/make"))
		bResult = MakeCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/item"))
		bResult = MakeCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/setdrop"))
		bResult = SetDropCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/setmake"))
		bResult = SetMakeCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/gmove"))
		bResult = GmoveCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/online"))
		bResult = OnlineCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/status"))
		bResult = StatusCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/reload"))
		bResult = ReloadCommand(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/pkclear"))
		bResult = PKClearCommand(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/str"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 0);
	
	if (CheckMessageForCommand(lpChat->message, "/agi"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 1);
	
	if (CheckMessageForCommand(lpChat->message, "/vit"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 2);
	
	if (CheckMessageForCommand(lpChat->message, "/ene"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 3);
	
	if (CheckMessageForCommand(lpChat->message, "/cmd"))
		bResult = AddCommands(lpObj, messageWithoutCommand, 4);
	
	if (CheckMessageForCommand(lpChat->message, "/soccer"))
		bResult = CommandBattleSoccer(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/war"))
		bResult = CommandGuildWar(lpObj, messageWithoutCommand); 
	
	if (CheckMessageForCommand(lpChat->message, "/re"))
		bResult = CommandRequest(lpObj, lpChat->message + strlen("/re"));
	
	if (CheckMessageForCommand(lpChat->message, "/move"))
		bResult = CommandMove(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/hide"))
		bResult = CommandHide(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/fireworks"))
		bResult = CommandFireworks(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/clearinv"))
		bResult = CommandClearInventory(lpObj, messageWithoutCommand, lpObj->Index);
	
	if (CheckMessageForCommand(lpChat->message, "/ware"))
		bResult = CommandWare(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/spawn"))
		bResult = CommandSpawn(lpObj, messageWithoutCommand); 
	
	if (CheckMessageForCommand(lpChat->message, "@>"))
		bResult = GuildPost(lpObj, messageWithoutCommand);
	
	if (CheckMessageForCommand(lpChat->message, "/popup"))
		bResult = MessagePopup(lpObj, messageWithoutCommand, lpObj->Index);

	 
	if (lpObj->BanChat)
	{
		lpObj->Penalty |= 2;
		Chat.MessageLog(1, c_Blue, t_BAN, lpObj, "[BanChat] Your chat is banned!");
		return true;
	}
	else
	{
		lpObj->Penalty &= ~2;
	}

	MassLog(lpObj, lpChat->message);

	return bResult;
}

void cChat::MsgSrv(LPOBJ lpObj, char* Message, bool Whisper)
{
	if (Whisper)
		GCChatWhisperSendAll(lpObj->Name, Message);
	else
		GCChatMessageSendAll(lpObj->Name, Message);
}

void cChat::PrivateLog(LPOBJ lpObj, char* Name, PMSG_CHAT_WHISPER_RECV* lpMsg, bool Success)
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	char currdate[11] = { 0 };
	sprintf_s(currdate, "%02d:%02d:%02d", t.wHour, t.wMinute, t.wSecond);
	char Msg[512];
	char Suc;
	if (Success)
		Suc = '>';
	else
		Suc = 'X';
	sprintf_s(Msg, "[%s] [%s -%c %s]:\t%s\n", currdate, lpObj->Name, Suc, Name, lpMsg->message);
	Log.ConsoleOutPut(1, c_Green, t_COMMANDS, "[Private] [%s -%c %s]: %s", lpObj->Name, Suc, Name, lpMsg->message);

	char Date[55];
	sprintf_s(Date, "..\\ChatLogs\\%02d-%02d-%02d\\", t.wDay, t.wMonth, t.wYear);
	CreateDirectory("..\\ChatLogs\\", NULL);
	CreateDirectory(Date, NULL);

	char ChatLog[55];
	sprintf_s(ChatLog, "..\\ChatLogs\\%02d-%02d-%02d\\PrivateChatLogs.log", t.wDay, t.wMonth, t.wYear);
	Log.SaveFile(ChatLog, Msg);
}

void cChat::MassLog(LPOBJ lpObj, char* Message)
{
	char Type[20];
	char Msg2[512];
	if (!memcmp(Message, "@@", strlen("@@")))
	{
		strcpy_s(Type, "Guild Alliance");
		strcpy_s(Msg2, Message + strlen("@@"));
	}
	else if (!memcmp(Message, "@", strlen("@")))
	{
		strcpy_s(Type, "Guild");
		strcpy_s(Msg2, Message + strlen("@"));
	}
	else if (!memcmp(Message, "/post", strlen("/post")))
	{
		strcpy_s(Type, "Post");
		strcpy_s(Msg2, Message + strlen("/post"));
	}
	else if (!memcmp(Message, "~", strlen("~")))
	{
		strcpy_s(Type, "Party");
		strcpy_s(Msg2, Message + strlen("~"));
	}
	else if (!memcmp(Message, "/move", strlen("/move")))
	{
		strcpy_s(Type, "Move");
		strcpy_s(Msg2, Message + strlen("/move"));
	}
	else if (!memcmp(Message, "/warp", strlen("/warp")))
	{
		strcpy_s(Type, "Move");
		strcpy_s(Msg2, Message + strlen("/warp"));
	}
	else
	{
		strcpy_s(Type, "Chat");
		strcpy_s(Msg2, Message);
	}

	SYSTEMTIME t;
	GetLocalTime(&t);
	char currdate[11] = { 0 };
	sprintf_s(currdate, "%02d:%02d:%02d", t.wHour, t.wMinute, t.wSecond);
	char Msg[512];
	sprintf_s(Msg, "[%s] [%s:%s] [%s]\t%s\n", currdate, lpObj->Account, lpObj->Name, Type, Msg2);

	char Date[55];
	sprintf_s(Date, "..\\ChatLogs\\%02d-%02d-%02d\\", t.wDay, t.wMonth, t.wYear);
	CreateDirectory("..\\ChatLogs\\", NULL);
	CreateDirectory(Date, NULL);

	char ChatLog[55];
	sprintf_s(ChatLog, "..\\ChatLogs\\%02d-%02d-%02d\\ChatLogs.log", t.wDay, t.wMonth, t.wYear);
	Log.SaveFile(ChatLog, Msg);
}

void cChat::MessageLog(int Type, sColor LogColor, sLogType LogType, LPOBJ lpObj, char* Msg, ...)
{
	char message[255] = {0};

	va_list arg;
	va_start(arg, Msg);
	vsprintf_s(message, Msg, arg);
	va_end(arg);
	
	gNotice.GCNoticeSend(lpObj->Index, Type, 0, 0, 0, 0, 0, message);
	if (Type)
		Log.ConsoleOutPut(1, LogColor, LogType, "[%s:%s]: %s", lpObj->Account, lpObj->Name, message);
	else
		Log.ConsoleOutPut(1, LogColor, LogType, "[GoldMessage] %s", message);
}

void cChat::Message(int Type, LPOBJ lpObj, char* Msg, ...)
{
	char message[255] = {0};

	va_list arg;
	va_start(arg, Msg);
	vsprintf_s(message, Msg, arg);
	va_end(arg);
	
	gNotice.GCNoticeSend(lpObj->Index, Type, 0, 0, 0, 0, 0, message);
}


void cChat::MessageAllLog(MessageType Type, sColor LogColor, sLogType LogType, LPOBJ lpObj, char* Msg, ...)
{
	char message[255] = {0};

	va_list arg;
	va_start(arg, Msg);
	vsprintf_s(message, Msg, arg);
	va_end(arg);
	
	if (Type == ChatMessage || Type == WhisperMessage)
	{
		MsgSrv(lpObj, message, Type == WhisperMessage);
	}
	else
		for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
		{
			LPOBJ tObj = &gObj[i];
			if (tObj->Connected < OBJECT_ONLINE)	continue;
			gNotice.GCNoticeSend(i, Type, 0, 0, 0, 0, 0, message);
		}

	if (Type != Gold)
		Log.ConsoleOutPut(1, LogColor, LogType, "[%s:%s]: %s", lpObj->Account, lpObj->Name, message);
	else
		Log.ConsoleOutPut(1, LogColor, LogType, "[%s:%s][GoldMessage] %s", lpObj->Account, lpObj->Name, message);
}

void cChat::MessageAll(MessageType Type, LPOBJ lpObj, char* Msg, ...)
{
	char message[255] = {0};

	va_list arg;
	va_start(arg, Msg);
	vsprintf_s(message, Msg, arg);
	va_end(arg);
	
	if (Type == ChatMessage || Type == WhisperMessage)
	{
		MsgSrv(lpObj, message, Type == WhisperMessage);
	}
	else
		for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
		{ 
			LPOBJ tObj = &gObj[i];
			if (tObj->Connected < OBJECT_ONLINE)	continue;
			gNotice.GCNoticeSend(i, Type, 0, 0, 0, 0, 0, message);
		}
}

bool cChat::CheckCommand(LPOBJ lpObj, int lConfig, cGmSystem::Commands Command,
	int NeedZen, int NeedLvl, int Filled, int CheckPlayer,
	char* CommandName, char* CommandUsage, char* Msg)
{
	bool bResult = false;

	int wordsCount = GetWordsCount(Msg);

	if (Filled > 0 && (Filled > wordsCount || strlen(Msg) < 1))
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[%s] Usage: %s", CommandName, CommandUsage);
		return true;
	}

	if (lConfig == 0)
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[%s] Function temporarily disabled.", CommandName);
		return true;
	}

	if (!GmSystem.IsCommand(Command, lpObj->Name))
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[%s] You can't use this command.", CommandName);
		return true;
	}

	if (NeedLvl > 0 && lpObj->Level < NeedLvl)
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[%s] You haven't got enough level.", CommandName);
		bResult = true;
	}

	if (NeedZen > 0 && NeedZen > lpObj->Money)
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[%s] You haven't got enough money.", CommandName);
		bResult = true;
	} 

	if (CheckPlayer != 0)
	{
		if (CheckPlayer == -1)
		{
			MessageLog(1, c_Red, t_COMMANDS, lpObj, "[%s] Player offline or doesn't exist!", CommandName);
			bResult = true;
		}
	}

	return bResult;
}

void cChat::TakeCommand(LPOBJ lpObj, int NeedZen, char* CommandName)
{
	if (NeedZen > 0)
	{
		lpObj->Money -= NeedZen;
		GCMoneySend(lpObj->Index, lpObj->Money);
	} 
}

bool cChat::GgCommand(LPOBJ lpObj, char* Msg)
{
	if (CheckCommand(lpObj, Configs.Commands.IsGg, GmSystem.cGg, 0, 0, 1, 0, "GG", "/gg <msg>", Msg))
		return true;

	Log.CheckProcent(Msg);
	MessageAll(Gold, lpObj, "[%s]: %s", lpObj->Name, Msg);

	Log.ConsoleOutPut(0, c_Green, t_GM, "[GG] %s: %s", lpObj->Name, Msg);
	return true;
}

bool cChat::VosklCommand(LPOBJ lpObj, char* Msg)
{
	if (CheckCommand(lpObj, 1, GmSystem.cVoskl, 0, 0, 1, 0, "!", "! <msg>", Msg))
		return true;

	Log.CheckProcent(Msg);
	MessageAll(Gold, lpObj, Msg);

	Log.ConsoleOutPut(0, c_Green, t_GM, "[!] %s: %s", lpObj->Name, Msg);
	return true;
}

bool cChat::TraceCommand(LPOBJ lpObj, int Index)
{
	if (CheckCommand(lpObj, 1, GmSystem.cTrace, 0, 0, 0, Index, "Trace", "[Name] /trace", ""))
		return true;

	LPOBJ tObj = &gObj[Index];

	gObjTeleport(lpObj->Index, tObj->Map, (int)tObj->X, (int)tObj->Y);
	MessageLog(1, c_Green, t_GM, lpObj, "[Trace] You successfully traced to %s", tObj->Name);

	if (GmSystem.IsAdmin(lpObj->Name) == 1)
		MessageLog(1, c_Green, t_GM, tObj, "[Trace] [Admin] %s traced to you", lpObj->Name);
	else if (GmSystem.IsAdmin(lpObj->Name) == 2)
		MessageLog(1, c_Green, t_GM, tObj, "[Trace] [GM] %s traced to you", lpObj->Name);

	return true;
}

bool cChat::SummonCommand(LPOBJ lpObj, int Index)
{
	if (CheckCommand(lpObj, 1, GmSystem.cTrace, 0, 0, 0, Index, "Track", "[Name] /track", ""))
		return true;

	LPOBJ tObj = &gObj[Index];

	gObjTeleport(tObj->Index, lpObj->Map, (int)lpObj->X, (int)lpObj->Y);
	MessageLog(1, c_Green, t_GM, lpObj, "[Track] You successfully summoned %s", tObj->Name);

	if (GmSystem.IsAdmin(lpObj->Name) == 1)
		MessageLog(1, c_Green, t_GM, tObj, "[Track] You was summoned to [Admin] %s", lpObj->Name);
	else if (GmSystem.IsAdmin(lpObj->Name) == 2)
		MessageLog(1, c_Green, t_GM, tObj, "[Track] You was summoned to [GM] %s", lpObj->Name);

	return true;
}

bool cChat::DiskCommand(LPOBJ lpObj, int Index)
{
	if (CheckCommand(lpObj, 1, GmSystem.cDisconnect, 0, 0, 0, Index, "DC", "[Name] /disconnect", ""))
		return true;

	LPOBJ tObj = &gObj[Index];
	MessageLog(1, c_Green, t_GM, lpObj, "[Disconnect] %s was disconnected.", tObj->Name);

	if (GmSystem.IsAdmin(lpObj->Name) == 1)
		MessageLog(1, c_Green, t_GM, tObj, "[Disconnect] You was disconnected by [Admin] %s", lpObj->Name);
	else if (GmSystem.IsAdmin(lpObj->Name) == 2)
		MessageLog(1, c_Green, t_GM, tObj, "[Disconnect] You was disconnected by [GM] %s", lpObj->Name);

	CloseClient(Index); 
	return true;
}

bool cChat::PostCommand(LPOBJ lpObj, char* Msg)
{
	if (CheckCommand(lpObj, Configs.Commands.IsPost, GmSystem.NONE, Configs.Commands.PostPriceZen, Configs.Commands.PostLvl, 1, 0, "Post", "/post <msg>", Msg))
		return true;

	if (lpObj->BanChat)
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[BanChat] Your chat is banned!");
		return true;
	}  

	DWORD tick = (GetTickCount() - lpObj->PostTime) / 1000;

	if ((tick < ((DWORD)Configs.Commands.PostDelay)) && !GmSystem.IsAdmin(lpObj->Name))
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[ANTI-FLOOD] Wait %d sec until you can post!", (DWORD)Configs.Commands.PostDelay - tick);
		return true;
	}
	 
	TakeCommand(lpObj, Configs.Commands.PostPriceZen, "Post");
	switch (Configs.Commands.PostColor)
	{
	case 0:
	case 1:
		MessageAll(WhisperMessage, lpObj, "[POST] %s", Msg);
		break;
	case 2:
		MessageAll(ChatMessage, lpObj, "@[POST] %s", Msg);
		break;
	case 3:
		MessageAll(ChatMessage, lpObj, "~[POST] %s", Msg);
		break;
	}

	lpObj->PostTime = GetTickCount();

	Log.ConsoleOutPut(0, c_Green, t_POST, "[POST] %s: %s", lpObj->Name, Msg); 
	return true;
}

bool cChat::TimeCommand(LPOBJ lpObj)
{
	if (CheckCommand(lpObj, Configs.Commands.IsTime, GmSystem.NONE, 0, 0, 0, 0,  "Time", "/time", ""))
		return true;

	SYSTEMTIME t;
	GetLocalTime(&t);
	MessageLog(1, c_Blue, t_COMMANDS, lpObj, "Server Time & Date: %02d:%02d:%02d %02d-%02d-%02d.", t.wHour, t.wMinute, t.wSecond, t.wDay, t.wMonth, t.wYear);
	return true;
}


bool cChat::LevelCommand(LPOBJ lpObj)
{
	MessageLog(1, c_Red, t_GM, lpObj, "TimerRandom: %d, Master TimerRandom: %d", lpObj->Level, lpObj->MasterLevel);
	return true;
}

bool cChat::DropCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsDrop, GmSystem.cDrop, 0, 0, 3, Index, "Drop", "[Name] /drop <ItemCount> <ItemType> <ItemIndex> <ItemLvl> <ItemSkill> <ItemLuck> <ItemOpt> <ItemExc> <ItemAnc>", Msg))
		return true;

	int ItemCount = 0;
	int ItemType = 0;
	int ItemIndex = 0;
	int ItemLevel = 0;
	int ItemSkill = 0;
	int ItemLuck = 0;
	int ItemOpt = 0;
	int ItemExc = 0;
	int ItemAncient = 0;

	sscanf_s(Msg, "%d %d %d %d %d %d %d %d %d", &ItemCount, &ItemType, &ItemIndex, &ItemLevel, &ItemSkill, &ItemLuck, &ItemOpt, &ItemExc, &ItemAncient);
	int Item = ItemType * 512 + ItemIndex;
	if ((ItemCount < 0) || (ItemCount > 20))
		ItemCount = 1;
	if (ItemIndex < 0 || (ItemType < 0 || ItemType > 15) || (ItemLevel < 0 || ItemLevel > 15) || (ItemOpt < 0 || ItemOpt > 7) || (ItemLuck < 0 || ItemLuck > 1) || (ItemSkill < 0 || ItemSkill > 1) || (ItemExc < 0 || ItemExc > 63) || (ItemAncient < 0 || ItemAncient > 40))
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[Drop] Usage: /drop <ItemCount> <ItemType> <ItemIndex> <ItemLvl> <ItemSkill> <ItemLuck> <ItemOpt> <ItemExc> <ItemAnc>");
		return true;
	}
	else
	{
		LPOBJ tObj = &gObj[Index];

		for (int i = 0; i < ItemCount; i++)
			GDCreateItemSend(Index, tObj->Map, (int)tObj->X, (int)tObj->Y, Item, ItemLevel, 0, ItemSkill, ItemLuck, ItemOpt, Index, ItemExc, ItemAncient, 0, 0, 0, 0xFF, 0);
		 
		if (tObj == lpObj)
			MessageLog(1, c_Green, t_GM, lpObj, "[Drop] %d Item Created %d %d %d %d %d %d %d %d - Success", ItemCount, ItemType, ItemIndex, ItemLevel, ItemSkill, ItemLuck, ItemOpt, ItemExc, ItemAncient);
		else
		{
			MessageLog(1, c_Green, t_GM, lpObj, "[Drop] %d Item Created to %s %d %d %d %d %d %d %d %d - Success", ItemCount, tObj->Name, ItemType, ItemIndex, ItemLevel, ItemSkill, ItemLuck, ItemOpt, ItemExc, ItemAncient);
			Message(1, tObj, "[Drop] You lucked by %s with %d items! Take them faster!", lpObj->Name, ItemCount);
		}
	}

	return true;
}

bool cChat::MakeCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsMake, GmSystem.cMake, 0, 0, 3, Index, "Make", "[Name] /make <ItemCount> <ItemType> <ItemIndex> <ItemLvl> <ItemSkill> <ItemLuck> <ItemOpt> <ItemExc> <ItemAnc>", Msg))
		return true;

	int ItemCount = 0;
	int ItemType = 0;
	int ItemIndex = 0;
	int ItemLevel = 0;
	int ItemSkill = 0;
	int ItemLuck = 0;
	int ItemOpt = 0;
	int ItemExc = 0;
	int ItemAncient = 0;

	sscanf_s(Msg, "%d %d %d %d %d %d %d %d %d", &ItemCount, &ItemType, &ItemIndex, &ItemLevel, &ItemSkill, &ItemLuck, &ItemOpt, &ItemExc, &ItemAncient);
	int Item = ItemType * 512 + ItemIndex;
	if ((ItemCount < 0) || (ItemCount > 20))
		ItemCount = 1;
	if (ItemIndex < 0 || (ItemType < 0 || ItemType > 15) || (ItemLevel < 0 || ItemLevel > 15) || (ItemOpt < 0 || ItemOpt > 7) || (ItemLuck < 0 || ItemLuck > 1) || (ItemSkill < 0 || ItemSkill > 1) || (ItemExc < 0 || ItemExc > 63) || (ItemAncient < 0 || ItemAncient > 40))
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[Make] Usage: /drop <ItemCount> <ItemType> <ItemIndex> <ItemLvl> <ItemSkill> <ItemLuck> <ItemOpt> <ItemExc> <ItemAnc>");
		return true;
	}
	else
	{
		LPOBJ tObj = &gObj[Index];

		for (int i = 0; i < ItemCount; i++)
			GDCreateItemSend(Index, 0xEB, 0, 0, Item, ItemLevel, 0, ItemSkill, ItemLuck, ItemOpt, -1, ItemExc, ItemAncient, 0, 0, 0, 0xFF, 0);

		if (tObj == lpObj)
			MessageLog(1, c_Green, t_GM, lpObj, "[Make] %d Item Created %d %d %d %d %d %d %d %d - Success", ItemCount, ItemType, ItemIndex, ItemLevel, ItemSkill, ItemLuck, ItemOpt, ItemExc, ItemAncient);
		else
		{
			MessageLog(1, c_Green, t_GM, lpObj, "[Make] %d Item Created to %s %d %d %d %d %d %d %d %d - Success", ItemCount, tObj->Name, ItemType, ItemIndex, ItemLevel, ItemSkill, ItemLuck, ItemOpt, ItemExc, ItemAncient);
			Message(1, tObj, "[Make] You lucked by %s with %d items! Take them faster!", lpObj->Name, ItemCount);
		}
	}

	return true;
}

bool cChat::SetCharCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsSetChar, GmSystem.cSetZen, 0, 0, 1, Index, "SetChar", "[Name] /setchar <Zen> <AddPnt> <lvl> <mastrerlvl> <Prof>", Msg))
		return true;

	DWORD Zen = 0;
	int AddPnt = 0, Lvl = 0, MasterLvl = 0, Prof = 0;

	sscanf_s(Msg, "%d %d %d %d %d", &Zen, &AddPnt, &Lvl, &MasterLvl, &Prof);

	if (Zen > 2000000000)
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetChar] Zen can't be less than 0 and more than 2000000000!");
		return true;
	}
	 
	int MaximumAddPnt  = 65000;

	if (AddPnt < 0 || AddPnt > MaximumAddPnt)
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetChar] AddPnt can't be less than 0 and more than %d!", MaximumAddPnt);

		return true;
	}

	if (Lvl < 0 || Lvl > 400)
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetChar] Lvl can't be less than 0 and more than 400!");
		return true;
	}
	if (MasterLvl < 0 || MasterLvl > 400)
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetChar] MasterLvl can't be less than 0 and more than 400!");
		return true;
	}

	if (Prof < 0 || Prof > 4)
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetChar] Prof can be in range of 0-4");
		return true;
	}

	LPOBJ tObj = &gObj[Index];

	if (Zen > 0)
	{
		tObj->Money = Zen;
		GCMoneySend(tObj->Index, Zen);
	}

	if (AddPnt > 0)
	{
		tObj->LevelUpPoint += AddPnt;
	}

	if (Lvl > 0)
	{
		tObj->Level = Lvl;
	}
	if (MasterLvl > 0)
	{
		tObj->MasterLevel = MasterLvl;
	}

	if (Prof > 0 && Prof < 5)
	{
		switch (Prof)
		{
			case 0:
			break;
			case 1: //default prof
			{
				if (tObj->DBClass >= 0 && tObj->DBClass <= 14) //DW
					tObj->DBClass = 0;

				else if (tObj->DBClass >= 16 && tObj->DBClass <= 30) //DK
					tObj->DBClass = 16;

				else if (tObj->DBClass >= 32 && tObj->DBClass <= 46) // ELF
					tObj->DBClass = 32;

				else if (tObj->DBClass >= 48 && tObj->DBClass <= 62) //MG
					tObj->DBClass = 48;

				else if (tObj->DBClass >= 64 && tObj->DBClass <= 78) //DL
					tObj->DBClass = 64;

				else if (tObj->DBClass >= 80 && tObj->DBClass <= 94) //SUM
					tObj->DBClass = 80;

				else if (tObj->DBClass >= 96 && tObj->DBClass <= 110) //RF
					tObj->DBClass = 96;

				else if (tObj->DBClass >= 112 && tObj->DBClass <= 126) //GL
					tObj->DBClass = 126;
			}
			break;
			case 2: //2nd prof
			{
				if (tObj->DBClass >= 0 && tObj->DBClass <= 14) //DW
					tObj->DBClass = 8;

				else if (tObj->DBClass >= 16 && tObj->DBClass <= 30) //DK
					tObj->DBClass = 24;

				else if (tObj->DBClass >= 32 && tObj->DBClass <= 46) // ELF
					tObj->DBClass = 40;

				else if (tObj->DBClass >= 48 && tObj->DBClass <= 62) //MG  no 2nd prof
					tObj->DBClass = 48;

				else if (tObj->DBClass >= 64 && tObj->DBClass <= 78) //DL  no 2nd prof
					tObj->DBClass = 64;

				else if (tObj->DBClass >= 80 && tObj->DBClass <= 94) //SUM
					tObj->DBClass = 88;

				else if (tObj->DBClass >= 96 && tObj->DBClass <= 110) //RF no 2nd prof
					tObj->DBClass = 96;

				else if (tObj->DBClass >= 112 && tObj->DBClass <= 126) //GL  no 2nd prof
					tObj->DBClass = 126;
			}
			break;
			case 3: //3rd prof
			{
				if (tObj->DBClass >= 0 && tObj->DBClass <= 14) //DW
					tObj->DBClass = 12;

				else if (tObj->DBClass >= 16 && tObj->DBClass <= 30) //DK
					tObj->DBClass = 28;

				else if (tObj->DBClass >= 32 && tObj->DBClass <= 46) // ELF
					tObj->DBClass = 44;

				else if (tObj->DBClass >= 48 && tObj->DBClass <= 62) //MG
					tObj->DBClass = 60;

				else if (tObj->DBClass >= 64 && tObj->DBClass <= 78) //DL
					tObj->DBClass = 76;

				else if (tObj->DBClass >= 80 && tObj->DBClass <= 94) //SUM
					tObj->DBClass = 92;

				else if (tObj->DBClass >= 96 && tObj->DBClass <= 110) //RF
					tObj->DBClass = 108;

				else if (tObj->DBClass >= 112 && tObj->DBClass <= 126) //GL
					tObj->DBClass = 124;
			}
			break;
			case 4: //4th prof
			{
				if (tObj->DBClass >= 0 && tObj->DBClass <= 14) //DW
					tObj->DBClass = 14;

				else if (tObj->DBClass >= 16 && tObj->DBClass <= 30) //DK
					tObj->DBClass = 30;

				else if (tObj->DBClass >= 32 && tObj->DBClass <= 46) // ELF
					tObj->DBClass = 46;

				else if (tObj->DBClass >= 48 && tObj->DBClass <= 62) //MG
					tObj->DBClass = 62;

				else if (tObj->DBClass >= 64 && tObj->DBClass <= 78) //DL
					tObj->DBClass = 78;

				else if (tObj->DBClass >= 80 && tObj->DBClass <= 94) //SUM
					tObj->DBClass = 94;

				else if (tObj->DBClass >= 96 && tObj->DBClass <= 110) //RF
					tObj->DBClass = 110;

				else if (tObj->DBClass >= 112 && tObj->DBClass <= 126) //GL
					tObj->DBClass = 126;
			}
			break;
		}
	} 
	if (lpObj == tObj)
	{
		if (AddPnt > 0 || Prof > 0 || Lvl > 0 || MasterLvl > 0)
			MessageLog(1, c_Red, t_GM, tObj, "[SetChar] Your character was edited, you must relogin!");
		else
			MessageLog(1, c_Red, t_GM, tObj, "[SetChar] Your character was edited!");
	}
	else
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetChar] You successfully changed %s character.", tObj->Name);
		if (AddPnt > 0 || Prof > 0 || Lvl > 0 || MasterLvl > 0)
			MessageLog(1, c_Red, t_GM, tObj, "[SetChar] Your character was edited by %s, you must relogin!", lpObj->Name);
		else
			MessageLog(1, c_Red, t_GM, tObj, "[SetChar] Your character was edited by %s!", lpObj->Name);
	}
	return true;
} 
bool cChat::OnlineCommand(LPOBJ lpObj, char* Msg)
{
	if (CheckCommand(lpObj, Configs.Commands.IsOnline, GmSystem.NONE, 0, 0, 0, 0, "Online", "/online", Msg))
		return true;

	MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[ONLINE]: %d Player(s)", gObjTotalUser);
	return true;
}

bool cChat::SetPKCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsSetPK, GmSystem.cSetPK, 0, 0, 1, Index, "SetPK", "[Name] /setpk <pklvl>", Msg))
		return true;

	int SetLevel;
	sscanf_s(Msg, "%d", &SetLevel);

	if (SetLevel < 0 || SetLevel > 100)
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetPK] PK lvl can't be less than 0 and more than 100!");
		return true;
	}

	LPOBJ tObj = &gObj[Index];

	tObj->PKLevel = SetLevel;
	if (SetLevel >= 3)
		tObj->PKCount = SetLevel - 3;
	GCPKLevelSend(tObj->Index, SetLevel);

	if (lpObj == tObj)
		MessageLog(1, c_Red, t_GM, tObj, "[SetPK] Your pk was changed to %d.", SetLevel);
	else
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetPK] You successfully changed %s pk to %d.", tObj->Name, SetLevel);
		MessageLog(1, c_Red, t_GM, tObj, "[SetPK] Your pk was changed to %d by %s.", SetLevel, lpObj->Name);
	}
	return true;
}

bool cChat::ReloadCommand(LPOBJ lpObj, char* Msg)
{
	if (CheckCommand(lpObj, Configs.Commands.IsReload, GmSystem.cReload, 0, 0, 1, 0, "Reload", "/reload <number>", Msg))
		return true;

	int NumberReload;
	sscanf_s(Msg, "%d", &NumberReload);

	switch (NumberReload)
	{
	case 0:
	{
		Configs.LoadAll(); 
		News.LoadNotice();
		News.Load();
		GmSystem.Load(); 
		DropSystem.Load();
		MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[Reload] IA settings are reloaded.");
		break;
	}
	case 1:
	{
		gServerInfo.ReloadMonsterInfo();
		DropSystem.Load();
		MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[Reload] Monsters Reloaded.");
		break;
	}
	case 2:
	{
		gServerInfo.ReadShopInfo();
		MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[Reload] Shops Reloaded.");
		break;
	}
	case 3:
	{ 
		gServerInfo.ReadCommonInfo();  
		MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[Reload] EXP/Drop configs Reloaded.");
		break;
	} 
	default:
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[Reload] Usage: /reload <Number>.");
		break;
	}
	}
	return true;
}

bool cChat::StatusCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsStatus, GmSystem.cStatus, 0, 0, 0, Index, "Status", "[Name] /status", Msg))
		return true;

	LPOBJ tObj = &gObj[Index];

	MessageLog(0, c_Green, t_GM, lpObj, "IP Address: %s", tObj->IpAddr);
	MessageLog(0, c_Green, t_GM, lpObj, "Account: %s | Character: %s", tObj->Account, tObj->Name);
	MessageLog(0, c_Green, t_GM, lpObj, "TimerRandom: %d | Zen: %d", tObj->Level, tObj->Money);
	MessageLog(0, c_Green, t_GM, lpObj, "Map: %s (%d)(%d,%d)", Addons.GetMapName(tObj->Map), tObj->Map, tObj->X, tObj->Y); 

	MessageLog(1, c_Green, t_GM, tObj, "[Status][GM] %s get your status!", lpObj->Name);
	return true;
}

bool cChat::SkinCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsSkin, GmSystem.cSkin, 0, 0, 1, Index, "Skin", "[Name] /skin <num>", Msg))
		return true;

	int NumSkin = 0;

	sscanf_s(Msg, "%d", &NumSkin);

	LPOBJ tObj = &gObj[Index];

	tObj->Change = NumSkin;

	gObjViewportListProtocolCreate(tObj);  
	gObjectManager.CharacterUpdateMapEffect(tObj);


	if (tObj == lpObj)
		MessageLog(1, c_Red, t_GM, tObj, "[Skin] Your Skin was changed.");
	else
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[Skin] You successfully change %s Skin.", tObj->Name);
		MessageLog(1, c_Red, t_GM, tObj, "[Skin] Your Skin was changed by %s.", lpObj->Name);
	}

	return true;
}

bool cChat::GmoveCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsGmove, GmSystem.cGmove, 0, 0, 3, Index, "Gmove", "[Name] /gmove <map> <x> <y>", Msg))
		return true;

	char Map[250] = { 0 };
	int x = 0, y = 0;

	sscanf(Msg, "%s %d %d", &Map, &x, &y);
	LPOBJ tObj = &gObj[Index];

	int RMap;
	if (_strcmpi(Map, "0") == 0 || _strcmpi(Map, "LORENCIA") == 0) RMap = 0;
	else if (_strcmpi(Map, "1") == 0 || _strcmpi(Map, "DUNGEON") == 0) RMap = 1;
	else if (_strcmpi(Map, "2") == 0 || _strcmpi(Map, "DEVIAS") == 0) RMap = 2;
	else if (_strcmpi(Map, "3") == 0 || _strcmpi(Map, "NORIA") == 0) RMap = 3;
	else if (_strcmpi(Map, "4") == 0 || _strcmpi(Map, "LOST_TOWER") == 0) RMap = 4;
	else if (_strcmpi(Map, "5") == 0 || _strcmpi(Map, "RESERVED1") == 0) RMap = 5;
	else if (_strcmpi(Map, "6") == 0 || _strcmpi(Map, "ARENA") == 0) RMap = 6;
	else if (_strcmpi(Map, "7") == 0 || _strcmpi(Map, "ATLANS") == 0) RMap = 7;
	else if (_strcmpi(Map, "8") == 0 || _strcmpi(Map, "TARKAN") == 0) RMap = 8;
	else if (_strcmpi(Map, "9") == 0 || _strcmpi(Map, "DEVIL_SQUARE1") == 0) RMap = 9;
	else if (_strcmpi(Map, "10") == 0 || _strcmpi(Map, "ICARUS") == 0) RMap = 10;
	else if (_strcmpi(Map, "11") == 0 || _strcmpi(Map, "BLOOD_CASTLE1") == 0) RMap = 11;
	else if (_strcmpi(Map, "12") == 0 || _strcmpi(Map, "BLOOD_CASTLE2") == 0) RMap = 12;
	else if (_strcmpi(Map, "13") == 0 || _strcmpi(Map, "BLOOD_CASTLE3") == 0) RMap = 13;
	else if (_strcmpi(Map, "14") == 0 || _strcmpi(Map, "BLOOD_CASTLE4") == 0) RMap = 14;
	else if (_strcmpi(Map, "15") == 0 || _strcmpi(Map, "BLOOD_CASTLE5") == 0) RMap = 15;
	else if (_strcmpi(Map, "16") == 0 || _strcmpi(Map, "BLOOD_CASTLE6") == 0) RMap = 16;
	else if (_strcmpi(Map, "17") == 0 || _strcmpi(Map, "BLOOD_CASTLE7") == 0) RMap = 17;
	else if (_strcmpi(Map, "18") == 0 || _strcmpi(Map, "CHAOS_CASTLE1") == 0) RMap = 18;
	else if (_strcmpi(Map, "19") == 0 || _strcmpi(Map, "CHAOS_CASTLE2") == 0) RMap = 19;
	else if (_strcmpi(Map, "20") == 0 || _strcmpi(Map, "CHAOS_CASTLE3") == 0) RMap = 20;
	else if (_strcmpi(Map, "21") == 0 || _strcmpi(Map, "CHAOS_CASTLE4") == 0) RMap = 21;
	else if (_strcmpi(Map, "22") == 0 || _strcmpi(Map, "CHAOS_CASTLE5") == 0) RMap = 22;
	else if (_strcmpi(Map, "23") == 0 || _strcmpi(Map, "CHAOS_CASTLE6") == 0) RMap = 23;
	else if (_strcmpi(Map, "24") == 0 || _strcmpi(Map, "KALIMA1") == 0) RMap = 24;
	else if (_strcmpi(Map, "25") == 0 || _strcmpi(Map, "KALIMA2") == 0) RMap = 25;
	else if (_strcmpi(Map, "26") == 0 || _strcmpi(Map, "KALIMA3") == 0) RMap = 26;
	else if (_strcmpi(Map, "27") == 0 || _strcmpi(Map, "KALIMA4") == 0) RMap = 27;
	else if (_strcmpi(Map, "28") == 0 || _strcmpi(Map, "KALIMA5") == 0) RMap = 28;
	else if (_strcmpi(Map, "29") == 0 || _strcmpi(Map, "KALIMA6") == 0) RMap = 29;
	else if (_strcmpi(Map, "30") == 0 || _strcmpi(Map, "CASTLE_SIEGE") == 0) RMap = 30;
	else if (_strcmpi(Map, "31") == 0 || _strcmpi(Map, "LAND_OF_TRIALS") == 0) RMap = 31;
	else if (_strcmpi(Map, "32") == 0 || _strcmpi(Map, "DEVIL_SQUARE2") == 0) RMap = 32;
	else if (_strcmpi(Map, "33") == 0 || _strcmpi(Map, "AIDA") == 0) RMap = 33;
	else if (_strcmpi(Map, "34") == 0 || _strcmpi(Map, "CRYWOLF") == 0) RMap = 34;
	else if (_strcmpi(Map, "35") == 0 || _strcmpi(Map, "RESERVED2") == 0) RMap = 35;
	else if (_strcmpi(Map, "36") == 0 || _strcmpi(Map, "KALIMA7") == 0) RMap = 36;
	else if (_strcmpi(Map, "37") == 0 || _strcmpi(Map, "KANTURU1") == 0) RMap = 37;
	else if (_strcmpi(Map, "38") == 0 || _strcmpi(Map, "KANTURU2") == 0) RMap = 38;
	else if (_strcmpi(Map, "39") == 0 || _strcmpi(Map, "KANTURU3") == 0) RMap = 39;
	else if (_strcmpi(Map, "40") == 0 || _strcmpi(Map, "SILENT") == 0) RMap = 40;
	else if (_strcmpi(Map, "41") == 0 || _strcmpi(Map, "BARRACKS") == 0) RMap = 41;
	else if (_strcmpi(Map, "42") == 0 || _strcmpi(Map, "REFUGE") == 0) RMap = 42;
	else if (_strcmpi(Map, "45") == 0 || _strcmpi(Map, "ILLUSION_TEMPLE1") == 0) RMap = 45;
	else if (_strcmpi(Map, "46") == 0 || _strcmpi(Map, "ILLUSION_TEMPLE2") == 0) RMap = 46;
	else if (_strcmpi(Map, "47") == 0 || _strcmpi(Map, "ILLUSION_TEMPLE3") == 0) RMap = 47;
	else if (_strcmpi(Map, "48") == 0 || _strcmpi(Map, "ILLUSION_TEMPLE4") == 0) RMap = 48;
	else if (_strcmpi(Map, "49") == 0 || _strcmpi(Map, "ILLUSION_TEMPLE5") == 0) RMap = 49;
	else if (_strcmpi(Map, "50") == 0 || _strcmpi(Map, "ILLUSION_TEMPLE6") == 0) RMap = 50;
	else if (_strcmpi(Map, "51") == 0 || _strcmpi(Map, "ELBELAND") == 0) RMap = 51;
	else if (_strcmpi(Map, "52") == 0 || _strcmpi(Map, "BLOOD_CASTLE8") == 0) RMap = 52;
	else if (_strcmpi(Map, "53") == 0 || _strcmpi(Map, "CHAOS_CASTLE7") == 0) RMap = 53;
	else if (_strcmpi(Map, "56") == 0 || _strcmpi(Map, "SWAMP_OF_CALMNESS") == 0) RMap = 56;
	else if (_strcmpi(Map, "57") == 0 || _strcmpi(Map, "RAKLION1") == 0) RMap = 57;
	else if (_strcmpi(Map, "58") == 0 || _strcmpi(Map, "RAKLION2") == 0) RMap = 58;
	else if (_strcmpi(Map, "62") == 0 || _strcmpi(Map, "SANTA_TOWN") == 0) RMap = 62;
	else if (_strcmpi(Map, "63") == 0 || _strcmpi(Map, "VULCANUS") == 0) RMap = 63;
	else if (_strcmpi(Map, "64") == 0 || _strcmpi(Map, "DUEL_ARENA") == 0) RMap = 64;
	else if (_strcmpi(Map, "65") == 0 || _strcmpi(Map, "DOUBLE_GOER1") == 0) RMap = 65;
	else if (_strcmpi(Map, "66") == 0 || _strcmpi(Map, "DOUBLE_GOER2") == 0) RMap = 66;
	else if (_strcmpi(Map, "67") == 0 || _strcmpi(Map, "DOUBLE_GOER3") == 0) RMap = 67;
	else if (_strcmpi(Map, "68") == 0 || _strcmpi(Map, "DOUBLE_GOER4") == 0) RMap = 68;
	else if (_strcmpi(Map, "69") == 0 || _strcmpi(Map, "IMPERIAL_GUARDIAN1") == 0) RMap = 69;
	else if (_strcmpi(Map, "70") == 0 || _strcmpi(Map, "IMPERIAL_GUARDIAN2") == 0) RMap = 70;
	else if (_strcmpi(Map, "71") == 0 || _strcmpi(Map, "IMPERIAL_GUARDIAN3") == 0) RMap = 71;
	else if (_strcmpi(Map, "72") == 0 || _strcmpi(Map, "IMPERIAL_GUARDIAN4") == 0) RMap = 72;
	else if (_strcmpi(Map, "79") == 0 || _strcmpi(Map, "LOREN_MARKET") == 0) RMap = 79;
	else if (_strcmpi(Map, "80") == 0 || _strcmpi(Map, "KARUTAN1") == 0) RMap = 80;
	else if (_strcmpi(Map, "81") == 0 || _strcmpi(Map, "KARUTAN2") == 0) RMap = 81;
	else if (_strcmpi(Map, "82") == 0 || _strcmpi(Map, "DOUBLE_RENEWAL") == 0) RMap = 82;
	else if (_strcmpi(Map, "91") == 0 || _strcmpi(Map, "ACHERON1") == 0) RMap = 91;
	else if (_strcmpi(Map, "92") == 0 || _strcmpi(Map, "ACHERON2") == 0) RMap = 92;
	else if (_strcmpi(Map, "95") == 0 || _strcmpi(Map, "DEVENTER1") == 0) RMap = 95;
	else if (_strcmpi(Map, "96") == 0 || _strcmpi(Map, "DEVENTER2") == 0) RMap = 96;
	else if (_strcmpi(Map, "97") == 0 || _strcmpi(Map, "CHAOS_CASTLE_FINAL") == 0) RMap = 97;
	else if (_strcmpi(Map, "98") == 0 || _strcmpi(Map, "ILLUSION_TEMPLE_FINAL1") == 0) RMap = 98;
	else if (_strcmpi(Map, "99") == 0 || _strcmpi(Map, "ILLUSION_TEMPLE_FINAL2") == 0) RMap = 99;
	else if (_strcmpi(Map, "100") == 0 || _strcmpi(Map, "URQUE_HILL") == 0) RMap = 100;
	else if (_strcmpi(Map, "110") == 0 || _strcmpi(Map, "DURBAN_TYLER") == 0) RMap = 110;
	else if (_strcmpi(Map, "112") == 0 || _strcmpi(Map, "FEREA") == 0) RMap = 112;
	else if (_strcmpi(Map, "113") == 0 || _strcmpi(Map, "KNICKS") == 0) RMap = 113;
	else if (_strcmpi(Map, "114") == 0 || _strcmpi(Map, "QUEST_ZONE_ENTRANCE") == 0) RMap = 114;
	else if (_strcmpi(Map, "115") == 0 || _strcmpi(Map, "Labyrinth") == 0) RMap = 115;
	else if (_strcmpi(Map, "116") == 0 || _strcmpi(Map, "DEEP_DUNGEON1") == 0) RMap = 116;
	else if (_strcmpi(Map, "117") == 0 || _strcmpi(Map, "DEEP_DUNGEON2") == 0) RMap = 117;
	else if (_strcmpi(Map, "118") == 0 || _strcmpi(Map, "DEEP_DUNGEON3") == 0) RMap = 118;
	else if (_strcmpi(Map, "119") == 0 || _strcmpi(Map, "DEEP_DUNGEON4") == 0) RMap = 119;
	else if (_strcmpi(Map, "120") == 0 || _strcmpi(Map, "DEEP_DUNGEON5") == 0) RMap = 120;
	else if (_strcmpi(Map, "121") == 0 || _strcmpi(Map, "4_TH_QUEST") == 0) RMap = 121;
	else if (_strcmpi(Map, "122") == 0 || _strcmpi(Map, "SWAMP_OF_DARKNESS") == 0) RMap = 122;
	else
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[GMove] Such map doesn't exist!");
		return true;
	}
	if (x > 255 || y > 255 || x < 0 || y < 0)
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[GMove] Such coords doesn't exist!");
		return true;
	}
	gObjTeleport(tObj->Index, RMap, x, y);
	if (tObj == lpObj)
		MessageLog(1, c_Green, t_GM, lpObj, "[GMove] You successfully moved to %s %d %d!", Map, x, y);
	else
	{
		MessageLog(1, c_Green, t_GM, lpObj, "[GMove] You successfully move %s to %s %d %d!", tObj->Name, Map, x, y);
		MessageLog(1, c_Green, t_GM, tObj, "[GMove] You successfully moved to %s %d %d by %s!", Map, x, y, lpObj->Name);
	}
	return true;
}

bool cChat::PKClearCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsPkClear, GmSystem.NONE, 0, 0, 0, Index, "PKClear", "/pkclear", Msg))
		return true;

	if (GmSystem.IsCommand(GmSystem.cPkClear, lpObj->Name))
	{
		LPOBJ tObj = &gObj[Index];

		if (tObj->PKLevel <= 3)
		{ 
			MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[PkClear] %s is not pk!", tObj->Name);
			return true;
		}

		tObj->PKLevel = 3;
		tObj->PKCount = 0;
		GCPKLevelSend(tObj->Index, 3);

		if (tObj == lpObj)
		{
			MessageLog(1, c_Red, t_GM, tObj, "[PkClear] Your pk was cleared.");
		}
		else
		{
			MessageLog(1, c_Red, t_GM, lpObj, "[PkClear] You successfully clear %s pk.", tObj->Name);
			MessageLog(1, c_Red, t_GM, tObj, "[PkClear] Your pk was cleared by %s.", lpObj->Name);
		}
	}
	else if (!Configs.Commands.PkClearForGm)
	{
		int PriceZen; 
		switch (Configs.Commands.PkClearType)
		{
		case 1:
			PriceZen = (Configs.Commands.PkClearPriceZen * lpObj->PKCount);
			break;
		case 2:
			PriceZen = Configs.Commands.PkClearPriceZenForAll;
			break;
		case 0:
			PriceZen = 0; 
			break;
		}

		if (lpObj->PKLevel <= 3)
		{
			MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[PkClear] Your are not pk!");
			return true;
		}
		if (CheckCommand(lpObj, Configs.Commands.IsPkClear, GmSystem.NONE, PriceZen, Configs.Commands.PkClearLevelReq, 0, 0, "PKClear", "/pkclear", Msg))
			return true;

		TakeCommand(lpObj, PriceZen, "PKClear");
		lpObj->PKLevel = 3;
		lpObj->PKCount = 0;
		GCPKLevelSend(lpObj->Index, 3);
		MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[PkClear] Your Pk successfully cleared!");
	}
	return true;
}

#pragma warning(disable: 4018 4244)
bool cChat::AddCommands(LPOBJ lpObj, char* Msg, int Type)
{
	switch (Type)
	{
	case 0:
		if (CheckCommand(lpObj, Configs.Commands.AddPointEnabled, GmSystem.NONE, Configs.Commands.AddPriceZen, Configs.Commands.AddPointLevelReq, 1, 0, "AddStats", "/addstr <num>", Msg))
			return true;
		break;
	case 1:
		if (CheckCommand(lpObj, Configs.Commands.AddPointEnabled, GmSystem.NONE, Configs.Commands.AddPriceZen, Configs.Commands.AddPointLevelReq, 1, 0, "AddStats", "/addagi <num>", Msg))
			return true;
		break;
	case 2:
		if (CheckCommand(lpObj, Configs.Commands.AddPointEnabled, GmSystem.NONE, Configs.Commands.AddPriceZen, Configs.Commands.AddPointLevelReq, 1, 0, "AddStats", "/addvit <num>", Msg))
			return true;
		break;
	case 3:
		if (CheckCommand(lpObj, Configs.Commands.AddPointEnabled, GmSystem.NONE, Configs.Commands.AddPriceZen, Configs.Commands.AddPointLevelReq, 1, 0, "AddStats", "/addene <num>", Msg))
			return true;
		break;
	case 4:
		if (CheckCommand(lpObj, Configs.Commands.AddPointEnabled, GmSystem.NONE, Configs.Commands.AddPriceZen, Configs.Commands.AddPointLevelReq, 1, 0, "AddStats", "/addcmd <num>", Msg))
			return true;
		if (lpObj->Class != CLASS_DL)
		{
			MessageLog(1, c_Red, t_COMMANDS, lpObj, "[AddStats] You are not Dark Lord!!!");
			return true;
		}
		break;
	}

	DWORD Points; 
	sscanf_s(Msg, "%d", &Points);
	 

	if ((Configs.Commands.MaxAddedPoints > 0) && (Points > Configs.Commands.MaxAddedPoints))
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[AddStats] You can't add more than %d points!!!", Configs.Commands.MaxAddedPoints);
		return true;
	} 

	unsigned int Stats = 0;
	bool bResult = false;
	int MaxStats = Configs.Commands.MaxStats;

	switch (Type)
	{
	case 0x00:
		Stats = lpObj->Strength; 
		break;
	case 0x01:
		Stats = lpObj->Dexterity; 
		break;
	case 0x02:
		Stats = lpObj->Vitality; 
		break;
	case 0x03:
		Stats = lpObj->Energy; 
		break;
	case 0x04:
		Stats = lpObj->Leadership; 
		break;
	} 
	 
	Stats += Points;


	if (lpObj->LevelUpPoint < Points)
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[AddStats] You don't have enough points to add. Need %d more.", Points - lpObj->LevelUpPoint);
		return true;
	}  

	if (((MaxStats > 0) && (Stats > MaxStats || Stats < 0)) || ((MaxStats < 0) && (Stats < 0) && (Stats > MaxStats)))
	{
		MessageLog(1, c_Red, t_COMMANDS, lpObj, "[AddStats] Each points limit: %d !!!", Configs.Commands.MaxStats);
		return true;
	}

	TakeCommand(lpObj, Configs.Commands.AddPriceZen,  "AddStats");

	if (gObjectManager.CharacterLevelUpPointAdd(lpObj, Type, Points))
	{
		GCLevelUpSend(lpObj, false);
		Chat.MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[AddStats] Your stats successfully added!");

		return true;
	} 

	return true;
}

bool cChat::SetDropCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsDrop, GmSystem.cDrop, 0, 0, 1, Index, "SetDrop", "[Name] /setdrop <ItemIndex> <ItemLvl> <ItemLuck> <ItemOpt> <ItemExc> <ItemAnc>", Msg))
		return true;

	int ItemIndex = 0;
	int ItemLevel = 0;
	int ItemLuck = 0;
	int ItemOpt = 0;
	int ItemExc = 0;
	int ItemAncient = 0;
	int ItemMin = 7;


	sscanf_s(Msg, "%d %d %d %d %d %d", &ItemIndex, &ItemLevel, &ItemLuck, &ItemOpt, &ItemExc, &ItemAncient);

	if (ItemIndex < 0 || (ItemLevel < 0 || ItemLevel > 15) || (ItemOpt < 0 || ItemOpt > 7) || (ItemLuck < 0 || ItemLuck > 1) || (ItemExc < 0 || ItemExc > 63) || (ItemAncient < 0 || ItemAncient > 40))
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetDrop] Usage: /setdrop <ItemIndex> <ItemLvl> <ItemLuck> <ItemOpt> <ItemExc> <ItemAnc>");
		return true;
	}
	 
	LPOBJ gUbj = &gObj[Index];
	for (int i = ItemMin; i < 12; i++)
	{
		int Item = i * 512 + ItemIndex;
		int Rand1 = rand() % 3;
		int Rand2 = rand() % 3;
		GDCreateItemSend(gUbj->Index, gUbj->Map, (int)gUbj->X + Rand1, (int)gUbj->Y + Rand2, Item, ItemLevel, 0, 0, ItemLuck, ItemOpt, gUbj->Index, ItemExc, ItemAncient, 0, 0, 0, 0xFF, 0);
	}
	if (lpObj == gUbj)
		MessageLog(1, c_Red, t_GM, lpObj, "[SetDrop] Item Created %d %d %d %d %d %d - Success", ItemIndex, ItemLevel, ItemLuck, ItemOpt, ItemExc, ItemAncient);
	else
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetDrop] Items Created to %s %d %d %d %d %d %d - Success", gUbj->Name, ItemIndex, ItemLevel, ItemLuck, ItemOpt, ItemExc, ItemAncient);
		Message(1, gUbj, "[SetDrop] You lucked by %s with items! Take them faster!", lpObj->Name);
	}

	return true;
}

bool cChat::SetMakeCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsMake, GmSystem.cMake, 0, 0, 1, Index, "SetMake", "[Name] /setmake <ItemIndex> <ItemLvl> <ItemLuck> <ItemOpt> <ItemExc> <ItemAnc>", Msg))
		return true;

	int ItemIndex = 0;
	int ItemLevel = 0;
	int ItemLuck = 0;
	int ItemOpt = 0;
	int ItemExc = 0;
	int ItemAncient = 0;
	int ItemMin = 7;


	sscanf_s(Msg, "%d %d %d %d %d %d", &ItemIndex, &ItemLevel, &ItemLuck, &ItemOpt, &ItemExc, &ItemAncient);

	if (ItemIndex < 0 || (ItemLevel < 0 || ItemLevel > 15) || (ItemOpt < 0 || ItemOpt > 7) || (ItemLuck < 0 || ItemLuck > 1) || (ItemExc < 0 || ItemExc > 63) || (ItemAncient < 0 || ItemAncient > 40))
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetMake] Usage: /setmake <ItemIndex> <ItemLvl> <ItemLuck> <ItemOpt> <ItemExc> <ItemAnc>");
		return true;
	}
	 
	LPOBJ gUbj = &gObj[Index];
	for (int i = ItemMin; i < 12; i++)
	{
		int Item = i * 512 + ItemIndex;
		GDCreateItemSend(gUbj->Index, 0xEB, 0, 0, Item, ItemLevel, 0, 0, ItemLuck, ItemOpt, gUbj->Index, ItemExc, ItemAncient, 0, 0, 0, 0xFF, 0);
		
	}
	if (lpObj == gUbj)
		MessageLog(1, c_Red, t_GM, lpObj, "[SetMake] Item Created %d %d %d %d %d %d - Success", ItemIndex, ItemLevel, ItemLuck, ItemOpt, ItemExc, ItemAncient);
	else
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetMake] Items Created to %s %d %d %d %d %d %d - Success", gUbj->Name, ItemIndex, ItemLevel, ItemLuck, ItemOpt, ItemExc, ItemAncient);
		Message(1, gUbj, "[SetMake] You lucked by %s with items!", lpObj->Name);
	}

	return true;
}

bool cChat::SetZenCommand(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsSetZen, GmSystem.cSetZen, 0, 0, 1, Index, "SetZen", "[Name] /setzen <Zen>", Msg))
		return true;

	DWORD Zen = 0;

	sscanf_s(Msg, "%d", &Zen);

	if (Zen <= 0 || Zen > 2000000000)
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetZen] Zen can't be less than 1 and more than 2000000000!");
		return true;
	}

	LPOBJ tObj = &gObj[Index];

	tObj->Money = Zen;
	GCMoneySend(tObj->Index, Zen);
	if (lpObj == tObj)
	{
		MessageLog(1, c_Red, t_GM, tObj, "[SetZen] Your zen was edited!");
	}
	else
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[SetZen] You successfully changed %s zen.", tObj->Name);
		MessageLog(1, c_Red, t_GM, tObj, "[SetZen] Your zen was edited by %s!", lpObj->Name);
	}
	return true;
} 

bool cChat::CommandClearInventory(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsClearInv, GmSystem.cClearInv, 0, 0, 0, Index, "ClearInv", "[Name] /clearinv", Msg))
		return true;

	LPOBJ tObj = &gObj[Index];

	int MaxValue = gItemManager.GetInventoryMaxValue(lpObj);
	for (int i = INVENTORY_WEAR_SIZE; i < MaxValue; i++)
	{
		gItemManager.InventoryDelItem(lpObj->Index, i);
		gItemManager.GCItemDeleteSend(lpObj->Index, i, 1);
	}

	if (lpObj == tObj)
	{
		MessageLog(1, c_Red, t_GM, tObj, "[ClearInv] Your invertory was cleaned!");
	}
	else
	{
		MessageLog(1, c_Red, t_GM, lpObj, "[ClearInv] You successfully cleaned %s invertory.", tObj->Name);
		MessageLog(1, c_Red, t_GM, tObj, "[ClearInv] Your invertory was cleaned by %s!", lpObj->Name);
	}
	return true;
}



bool cChat::CommandSpawn(LPOBJ lpObj, char* Msg)
{
	if (CheckCommand(lpObj, Configs.Commands.IsSpawn, GmSystem.cSpawn, 0, 0, 1, 0, "Spawn", "/spawn <mobid> <count> <map> <x> <y>", Msg))
		return true;
	 
	int monsterClass = 0;
	int monsterCount = 1;
	int monsterMapNumber = lpObj->Map;
	int monsterX = (int)lpObj->X;
	int monsterY = (int)lpObj->Y; 

	sscanf_s(Msg, "%d %d %d %d %d", &monsterClass, &monsterCount, &monsterMapNumber, &monsterX, &monsterY);


	for (int i = 0; i < monsterCount; i++)
	{
		int index = gObjAddMonster(lpObj->Map);

		if (OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		LPOBJ MonsterlpObj = &gObj[index];

		MonsterlpObj->X = monsterX;
		MonsterlpObj->Y = monsterY;
		MonsterlpObj->TX = monsterX;
		MonsterlpObj->TY = monsterY;
		MonsterlpObj->OldX = monsterX;
		MonsterlpObj->OldY = monsterY;
		MonsterlpObj->StartX = monsterX;
		MonsterlpObj->StartY = monsterY;
		MonsterlpObj->Dir = (lpObj->Dir + i) % 8;
		MonsterlpObj->Map = monsterMapNumber;

		if (gObjSetMonster(index, monsterClass, true) == 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[index].Class);
			gObjDel(index);
			continue;
		}
	}
	MessageLog(1, c_Red, t_GM, lpObj, "[Spawn] Monster Spawned: %d %d %d %d %d", monsterClass, monsterCount, monsterMapNumber, monsterX, monsterY);
	return true;
}

bool cChat::CommandWare(LPOBJ lpObj, char* Msg)
{
	if (CheckCommand(lpObj, Configs.Commands.IsWare, GmSystem.NONE, 0, 0, 1, 0, "Ware", "/ware <num>", Msg))
		return true;

	int number;
	sscanf_s(Msg, "%d", &number); 

	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->LoadWarehouse != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(86));
		return true;
	} 

	lpObj->WarehouseMoney = 0;

	memset(lpObj->WarehouseMap, 0xFF, WAREHOUSE_SIZE);

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		lpObj->Warehouse[n].Clear();
	}

	lpObj->Interface.use = 0;

	lpObj->Interface.type = INTERFACE_NONE;

	lpObj->Interface.state = 0;

	lpObj->LoadWarehouse = 0;

	lpObj->WarehouseNumber = number;

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(88), number);
	 
	Chat.MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[Ware] You changed your vault to %d!", number);
	return true;
}

bool cChat::CommandFireworks(LPOBJ lpObj, char* Msg)
{
	if (CheckCommand(lpObj, Configs.Commands.IsFireworks, GmSystem.cFireworks, 0, 0, 0, 0, "Fireworks", "", Msg))
		return true;

	for (int n = 0; n < 15; n++)
	{
		GCFireworksSend(lpObj, (lpObj->X + (((GetLargeRand() % 5) * 2) - 4)), (lpObj->Y + (((GetLargeRand() % 5) * 2) - 4)));
	}


	Chat.MessageLog(1, c_Blue, t_COMMANDS, lpObj, "[Fireworks] You made beutiful fireworks, goverment would proud of you!");
	return true;
}

bool cChat::CommandHide(LPOBJ lpObj, char* Msg, int Index)
{

	if (CheckCommand(lpObj, Configs.Commands.IsHide, GmSystem.cHide, 0, 0, 0, Index, "Hide", "[Name] /hide", Msg))
		return true;
	  

	LPOBJ tObj = &gObj[Index];

	if (gEffectManager.CheckEffect(tObj, EFFECT_INVISIBILITY) == 0)
	{
		gEffectManager.AddEffect(tObj, 0, EFFECT_INVISIBILITY, 0, 0, 0, 0, 0);

		gObjViewportListProtocolDestroy(tObj);

		Chat.MessageLog(1, c_Blue, t_COMMANDS, tObj, "[Hide] You are invisible now!");
	}
	else
	{
		gEffectManager.DelEffect(tObj, EFFECT_INVISIBILITY);

		gObjViewportListProtocolCreate(tObj);

		Chat.MessageLog(1, c_Blue, t_COMMANDS, tObj, "[Hide] You are visible now!");
	}

	return true;
}

//WZ Standard Funcs
bool cChat::GuildPost(LPOBJ lpObj, char* Msg)
{
	if (lpObj->GuildStatus == 128 || lpObj->GuildStatus == 64)
		GDGuildNoticeSave(lpObj->GuildName, Msg);
	else
		MessageLog(1, c_Red, t_Default, lpObj, "You aren't guild master or assistant");
	return true;
}

bool cChat::MessagePopup(LPOBJ lpObj, char* Msg, int Index)
{
	if (CheckCommand(lpObj, Configs.Commands.IsMessagePopup, GmSystem.cMessagePopup, 0, 0, 1, Index, "IsMessagePopup", "[Name] /popup <msg>", Msg))
		return true;

	LPOBJ tObj = &gObj[Index];

	GCMessagePopupSend(tObj, Msg);
	return true;
}


bool cChat::CommandMove(LPOBJ lpObj, char* Msg)
{
	MOVE_INFO MoveInfo;

	if (gMove.GetInfoByName(Msg, &MoveInfo) != 0)
	{
		gMove.Move(lpObj, MoveInfo.Index); 
	}
	return true;
}

bool cChat::CommandGuildWar(LPOBJ lpObj, char* Msg)
{
	char GuildName[9] = { 0 }; 
	sscanf(Msg, "%s", &GuildName); 

	if (strlen(GuildName) >= 1)
	{
		GCGuildWarRequestResult(GuildName, lpObj->Index, 0);
	}
	return true;
}

bool cChat::CommandBattleSoccer(LPOBJ lpObj, char* Msg)
{
	char GuildName[9] = { 0 }; 
	sscanf(Msg, "%s", &GuildName);

	if (strlen(GuildName) >= 1)
	{
		GCGuildWarRequestResult(GuildName, lpObj->Index, 1);
	}
	return true;
}

bool cChat::CommandRequest(LPOBJ lpObj, char* Msg)
{
	char mode[5] = { 0 };
	char PassParty[11] = { 0 };
	sscanf(Msg, "%s %s", &mode, &PassParty);

	if (strcmp(mode, "on") == 0)
	{
		lpObj->Option |= 1;
		lpObj->Option |= 2;
		lpObj->Option &= ~4;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(126));
	}
	else if (strcmp(mode, "off") == 0)
	{
		lpObj->Option &= ~1;
		lpObj->Option &= ~2;
		lpObj->Option &= ~4;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(127));
	}
	else if (strcmp(mode, "auto") == 0)
	{
		lpObj->Option |= 1;
		lpObj->Option |= 2;
		lpObj->Option |= 4;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		strcpy_s(lpObj->AutoPartyPassword, PassParty);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(128));
	}
	return true;
}