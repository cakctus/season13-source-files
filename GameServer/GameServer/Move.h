// Move.h: interface for the CMove class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"
#include "User.h"

#define MAX_MOVE 100

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_TELEPORT_RECV
{
	PBMSG_HEAD header; // C1:1C
	WORD gate;
	BYTE x;
	BYTE y;
};

struct PMSG_TELEPORT_MOVE_RECV
{
	PBMSG_HEAD header; // C1:8E
	BYTE type;
	DWORD reserved;
	WORD number;
#if(GAMESERVER_UPDATE>=1000)//2017/02/28
	BYTE unk;
#endif
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_TELEPORT_SEND
{
	PBMSG_HEAD header; // C3:1C
	BYTE unk;
	WORD gate;
	#if(GAMESERVER_UPDATE>=1200)//2017/05/23
	WORD map;
#else
	BYTE map;
#endif
	BYTE x;
	BYTE y;
	BYTE dir;
};

//**********************************************//
//**********************************************//
//**********************************************//

struct MOVE_INFO
{
	int Index;
	char Name[32];
	int Money;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int AccountLevel;
	int Gate;
};

class CMove
{
public:
	CMove();
	virtual ~CMove();
	void Load(char* path);
	bool GetInfo(int index,MOVE_INFO* lpInfo);
	bool GetInfoByName(char* name,MOVE_INFO* lpInfo);
	void Move(LPOBJ lpObj,int index);
	void CGTeleportRecv(PMSG_TELEPORT_RECV* lpMsg,int aIndex);
	void CGTeleportMoveRecv(PMSG_TELEPORT_MOVE_RECV* lpMsg,int aIndex);
	void GCTeleportSend(int aIndex,int gate,WORD map,BYTE x,BYTE y,BYTE dir);
private:
	std::map<int,MOVE_INFO> m_MoveInfo;
};

extern CMove gMove;
