#pragma once

#include "ItemManager.h"
#include "Protocol.h"
#include "EffectManager.h"

enum eViewportState
{
	VIEWPORT_NONE = 0,
	VIEWPORT_SEND = 1,
	VIEWPORT_WAIT = 2,
	VIEWPORT_DESTROY = 3,
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_VIEWPORT_DESTROY_SEND
{
	PBMSG_HEAD header; // C1:14
	BYTE count;
};

struct PMSG_VIEWPORT_DESTROY_ITEM_SEND
{
	PWMSG_HEAD header; // C2:21
	BYTE count;
};

struct PMSG_VIEWPORT_DESTROY
{
	BYTE index[2];
};

struct PMSG_VIEWPORT_SEND
{
	PWMSG_HEAD header; // C2:[12:13:1F:20:45:65:68]
	BYTE count;
};

struct PMSG_VIEWPORT_PLAYER
{
	//header 0~4
	BYTE index[2];//0
	BYTE x;//2
	BYTE y;//3
	BYTE CharSet[19];//4
	char name[10];//23
	BYTE tx;//33
	BYTE ty;//34
	BYTE DirAndPkLevel;//35
	BYTE attribute;//36

	BYTE MuunItem1[2];//37
	BYTE MuunItem1Level;//39
	BYTE MuunItem2[2];//40
	BYTE MuunItem2Level;//42
	BYTE MuunItem3[2];//43
	BYTE MuunItem3Level;//45

	BYTE DisplayMuun;//46

	BYTE MuunRideItem[2];//47

	BYTE DarkSpirit;//49
	BYTE level[2];//50
	BYTE MaxHP[4];//52
	BYTE CurHP[4];//56
	WORD nServerCodeOfHomeWorld;//60

	BYTE count;//62
	WORD EffectList[MAX_EFFECT_LIST];//63
};

struct PMSG_VIEWPORT_MONSTER
{
	BYTE index[2];
	BYTE type[2];
	BYTE x;
	BYTE y;
	BYTE tx;
	BYTE ty;
	BYTE DirAndPkLevel;

	BYTE attribute;
	BYTE level[2];
	BYTE MaxHP[4];
	BYTE CurHP[4];

	WORD count;
	BYTE EffectList[MAX_EFFECT_LIST];
};

struct PMSG_VIEWPORT_SUMMON
{
	BYTE index[2];
	BYTE type[2];
	BYTE x;
	BYTE y;
	BYTE tx;
	BYTE ty;
	BYTE DirAndPkLevel;
	char name[10];
	BYTE attribute;
	BYTE level[2];//0x14
	BYTE MaxHP[4];
	BYTE CurHP[4];
	BYTE CharSet[19];
	BYTE SummonType;
	WORD count;
	BYTE EffectList[MAX_EFFECT_LIST];
};

struct PMSG_VIEWPORT_ITEM
{
	BYTE index[2];
	BYTE x;
	BYTE y;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_VIEWPORT_CHANGE
{
	BYTE index[2];
	BYTE x;
	BYTE y;
	BYTE skin[2];
	char name[10];
	BYTE tx;
	BYTE ty;
	BYTE DirAndPkLevel;
	BYTE CharSet[19];
	BYTE attribute;
	BYTE MuunItem1[2];
	BYTE MuunItem1Level;
	BYTE MuunItem2[2];
	BYTE MuunItem2Level;
	BYTE MuunItem3[2];
	BYTE MuunItem3Level;
	BYTE DisplayMuun;//46
	BYTE MuunRideItem[2];
	BYTE DarkSpirit;
	BYTE level[2];
	BYTE MaxHP[4];
	BYTE CurHP[4];
	WORD nServerCodeOfHomeWorld;
	BYTE count;
	WORD EffectList[MAX_EFFECT_LIST];
};

struct PMSG_VIEWPORT_GUILD
{
	DWORD number;
	BYTE status;
	BYTE type;
	BYTE relationship;
	BYTE index[2];
	BYTE owner;
};

struct PMSG_VIEWPORT_UNION
{
	BYTE index[2];
	DWORD number;
	BYTE relationship;
	char UnionName[8];
};

struct PMSG_VIEWPORT_CASTLE_SIEGE_WEAPON
{
	BYTE type;
	BYTE skin[2];
	BYTE index[2];
	BYTE x;
	BYTE y;
	BYTE CharSet[17];
	WORD count;
};

struct PMSG_VIEWPORT_STATE
{
	WORD effect;
};

//**********************************************//
//**********************************************//
//**********************************************//

class CViewport
{
public:
	CViewport();
	virtual ~CViewport();
	bool CheckViewportObjectPosition(int aIndex,int map,int x,int y,int view);
	bool CheckViewportObject1(int aIndex,int bIndex,int type);
	bool CheckViewportObject2(int aIndex,int bIndex,int type);
	bool CheckViewportObjectItem(int aIndex,int bIndex,int type);
	bool AddViewportObject1(int aIndex,int bIndex,int type);
	bool AddViewportObject2(int aIndex,int bIndex,int type);
	bool AddViewportObjectItem(int aIndex,int bIndex,int type);
	void AddViewportObjectAgro(int aIndex,int bIndex,int type);
	void DelViewportObjectAgro(int aIndex,int bIndex,int type);
	void DestroyViewportPlayer1(int aIndex);
	void DestroyViewportPlayer2(int aIndex);
	void DestroyViewportMonster1(int aIndex);
	void DestroyViewportMonster2(int aIndex);
	void DestroyViewportItem(int aIndex);
	void CreateViewportPlayer(int aIndex);
	void CreateViewportMonster(int aIndex);
	void CreateViewportItem(int aIndex);
	void GCViewportDestroySend(int aIndex);
	void GCViewportDestroyItemSend(int aIndex);
	void GCViewportPlayerSend(int aIndex);
	void GCViewportMonsterSend(int aIndex);
	void GCViewportSummonSend(int aIndex);
	void GCViewportItemSend(int aIndex);
	void GCViewportChangeSend(int aIndex);
	void GCViewportGuildSend(int aIndex);
	void GCViewportUnionSend(int aIndex);
	void GCViewportCastleSiegeWeaponSend(int aIndex,int tx,int ty);
	void GCViewportGensSystemSend(int aIndex);
	void GCViewportSimpleDestroySend(LPOBJ lpObj);
	void GCViewportSimplePlayerSend(LPOBJ lpObj);
	void GCViewportSimpleMonsterSend(LPOBJ lpObj);
	void GCViewportSimpleSummonSend(LPOBJ lpObj);
	void GCViewportSimpleChangeSend(LPOBJ lpObj);
	void GCViewportSimpleGuildSend(LPOBJ lpObj);
	void GCViewportSimpleUnionSend(LPOBJ lpObj);
	void GCViewportSimpleGensSystemSend(LPOBJ lpObj);

	void  gObjClearViewportOfMine(LPOBJ lpObj);
};

extern CViewport gViewport;
