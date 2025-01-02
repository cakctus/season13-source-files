#ifndef CHAT_H
#define CHAT_H

#include "Logger.h"
#include "User.h"
#include "GMSystem.h" 
#include "Protocol.h"

enum MessageType
{
	Gold,
	System,
	ChatMessage,
	WhisperMessage
};

class cChat
{
public:
	cChat();
	~cChat();
	bool WisperChat(LPOBJ lpObj, PMSG_CHAT_WHISPER_RECV* lpMsg);
	bool ChatDataSend(LPOBJ lpObj, PMSG_CHAT_RECV* lpChat);
	void MessageLog(int Type, sColor LogColor, sLogType LogType, LPOBJ lpObj, char* Msg, ...);
	void Message(int Type, LPOBJ lpObj, char* Msg, ...);		//Fixed
	void MessageAllLog(MessageType Type, sColor LogColor, sLogType LogType, LPOBJ lpObj, char* Msg, ...);
	void MessageAll(MessageType Type, LPOBJ lpObj, char* Msg, ...);		//Fixed

private:
	void AntiInject(char* message);
	bool CheckMessageForCommand(const char *message, const char *command);
	bool CheckCommand(LPOBJ lpObj, int Config,
		cGmSystem::Commands Command, int NeedZen,
		int NeedLvl, int Filled,
		int CheckPlayer, char* CommandName,
		char* CommandUsage, char* Msg);
	void TakeCommand(LPOBJ lpObj, int NeedZen, char* CommandName);
	void PrivateLog(LPOBJ lpObj, char* Name, PMSG_CHAT_WHISPER_RECV* lpMsg, bool Success);
	void MassLog(LPOBJ lpObj, char* Message);
	void MsgSrv(LPOBJ lpObj, char* Message, bool Whisper = false);
	bool GgCommand(LPOBJ lpObj, char* Msg);
	bool VosklCommand(LPOBJ lpObj, char* Msg); 
	bool PostCommand(LPOBJ lpObj, char* Msg);
	bool TimeCommand(LPOBJ lpObj);
	bool ReloadCommand(LPOBJ lpObj, char* Msg);
	bool OnlineCommand(LPOBJ lpObj, char* Msg);
	bool LevelCommand(LPOBJ lpObj); 
	bool AddCommands(LPOBJ lpObj, char* Msg, int Type); 
	bool GuildPost(LPOBJ lpObj, char* Msg); 
	bool MessagePopup(LPOBJ lpObj, char* Msg, int Index); 

	bool TraceCommand(LPOBJ lpObj, int Index);
	bool DiskCommand(LPOBJ lpObj, int Index);
	bool SummonCommand(LPOBJ lpObj, int Index); 

	bool SetCharCommand(LPOBJ lpObj, char* Msg, int Index);
	bool SetPKCommand(LPOBJ lpObj, char* Msg, int Index);
	bool SkinCommand(LPOBJ lpObj, char* Msg, int Index);
	bool DropCommand(LPOBJ lpObj, char* Msg, int Index);
	bool MakeCommand(LPOBJ lpObj, char* Msg, int Index);
	bool GmoveCommand(LPOBJ lpObj, char* Msg, int Index);
	bool StatusCommand(LPOBJ lpObj, char* Msg, int Index);
	bool PKClearCommand(LPOBJ lpObj, char* Msg, int Index);
	bool SetDropCommand(LPOBJ lpObj, char* Msg, int Index);
	bool SetMakeCommand(LPOBJ lpObj, char* Msg, int Index);
	bool SetZenCommand(LPOBJ lpObj, char* Msg, int Index); 

	bool CommandBattleSoccer(LPOBJ lpObj, char* Msg);
	bool CommandGuildWar(LPOBJ lpObj, char* Msg);
	bool CommandMove(LPOBJ lpObj, char* Msg);
	bool CommandHide(LPOBJ lpObj, char* Msg, int Index);
	bool CommandFireworks(LPOBJ lpObj, char* Msg);
	bool CommandClearInventory(LPOBJ lpObj, char* Msg, int Index);
	bool CommandWare(LPOBJ lpObj, char* Msg);
	bool CommandSpawn(LPOBJ lpObj, char* Msg);
	bool CommandRequest(LPOBJ lpObj, char* Msg);
};

extern cChat Chat;


#endif