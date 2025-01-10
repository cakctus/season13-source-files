// MiniMap.h: interface for the CMiniMap class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//
struct PMSG_MINI_MAP_INFO_RECV
{
	PSBMSG_HEAD header; // C1:E7:03
	BYTE flag;
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//
struct PMSG_MINI_MAP_PARTY_INFO_SEND
{
	PSBMSG_HEAD header; // C1:E7:01
	BYTE count;
};

#pragma pack(1)
struct PMSG_MINI_MAP_PARTY_INFO
{
	char name[11];
	WORD map;
	BYTE x;
	BYTE y;
};
#pragma pack()

struct PMSG_MINI_MAP_INFO_SEND
{
	PSBMSG_HEAD header; // C1:E7:03
	BYTE index;
	BYTE group;
	BYTE type;
	BYTE flag;
	BYTE x;
	BYTE y;
	char text[31];
};

typedef struct _PMSG_ANS_ITL_NPC_POSITION
{
	PSBMSG_HEAD h;
	BYTE btIdentNo;
	BYTE btIsNpc;
	BYTE btTag;
	BYTE btType;
	BYTE btPosX;
	BYTE btPosY;
};

//**********************************************//
//**********************************************//
//**********************************************//

struct MINI_MAP_INFO
{
	int Index;
	int Group;
	int Type;
	int X;
	int Y;
	char Text[32];
};

class CMiniMap
{
public:
	CMiniMap();
	virtual ~CMiniMap();
	void Load(char* path);
	void CGMiniMapStartPartyInfoRecv(int aIndex);
	void CGMiniMapClosePartyInfoRecv(int aIndex);
	void CGMiniMapInfoRecv(PMSG_MINI_MAP_INFO_RECV* lpMsg,int aIndex);
	void GCMiniMapPartyInfoSend(int aIndex);
	void GCMiniMapInfoSend(int aIndex);
	void SendITLNpcCoordinate(int nDestUserIndex, BYTE btMapIndex);
	void FindIllusionTempleNpc(BYTE byMapNumber, WORD wNpcType, BYTE& byMapTagIndex);
private:
	std::vector<MINI_MAP_INFO> m_MiniMapInfo;
};

extern CMiniMap gMiniMap;
