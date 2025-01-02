//S11 SocketChaos
#pragma once
#include "Protocol.h"
#include "User.h"
struct ITEM_UP_INFO
{
	int ItemUpBefore;
	int ItemUpAfter;
	int SaveLevel;
	int SaveOption;
};
struct SEED_SPHERE_UP
{
	int level;
	int number;
	int money;
	int rate;
};
enum SXSocketMixNumber
{
	SOCKET_MIX_ITEM_UP = 1,
	SOCKET_MIX_ITEM_CREATE_SEED = 2,
	SOCKET_MIX_SOCKET_ITEM_MOUNT_SEED_SPHERE = 3,
	SOCKET_MIX_ITEM_EXTRACT_SEED_SPHERE = 4,
	SOCKET_MIX_SOCKET_ITEM_CREATE_SEED_SPHERE = 5,
	SOCKET_MIX_SEED_SPHERE_LEVEL_UP = 6,
	SOCKET_MIX_SOCKET_DEL = 7,
};
//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//
struct PMSG_SOCKET_MIX_RECV
{
	PBMSG_HEAD header;
	BYTE info;
};
struct PMSG_UPGRADE_SLOT
{
	PBMSG_HEAD header; // C1:78:01
	BYTE Info;
	BYTE ItemIndex;
	BYTE Paper;
	BYTE unk1;
	BYTE unk2;
	BYTE unk3;
	BYTE unk4;
	BYTE unk5;
	BYTE unk6;
	BYTE unk7;
	BYTE unk8;
	BYTE Bless[5];
	BYTE unk9;
	BYTE unk10;
	BYTE unk11;
	BYTE unk12;
	BYTE Soul[5];
	BYTE unk13;
	BYTE unk14;
	BYTE unk15;
	BYTE unk16;
	BYTE Chaos;
	BYTE unk17[8];
};
struct PMSG_EXTRACT_RECV
{
	PBMSG_HEAD header; // C1:78:02
	BYTE Info;
	BYTE SetItem;
	BYTE ExcItem;
	BYTE Harmony;
	BYTE unk[8];
	BYTE Creation;
	BYTE unk1[8];
	BYTE Chaos;
};
struct PMSG_SEED_SPHERE_MIX_RECV
{
	PBMSG_HEAD header; // C1:78:05
	BYTE Info;
	BYTE Seed;
	BYTE Sphere;
	BYTE Creation;
	BYTE Chaos;
};

struct PMSG_MOUNT_SEND_SPHERE_MIX_RECV
{
	PBMSG_HEAD header; // C1:78:03
	BYTE Info;
	BYTE Item;
	BYTE SocketNum;
	BYTE SeedSphere;
	BYTE Creation;
	BYTE unk[8];
	BYTE Chaos;
};

struct PMSG_EXTRACT_SEED_SPHERE_MIX_RECV
{
	PBMSG_HEAD header; // C1:78:04
	BYTE Info;
	BYTE Item;
	BYTE SocketNum;
	BYTE Guardian;
	BYTE unk[8];
	BYTE Harmony[5];
	BYTE unk1[4];
	BYTE Chaos[5];

	/*
	PBMSG_HEAD2 h; // C1:78:04 C->GS
	BYTE btSourceItemPos;
	BYTE btSocketSlotIndex;
	BYTE btJewelItem1Pos[9];
	BYTE btJewelItem2Pos[9];
	BYTE btJewelItem3Pos[9];
	BYTE btJewelItem4Pos[9];
	*/
};

struct PMSG_SEED_SPHERE_LEVLE_PRCV
{
	PBMSG_HEAD header; // C1:78:04
	BYTE Info;
	BYTE SeedSphere1;
	BYTE SeedSphere2;
	BYTE Rune;
};
struct PMSG_SOCKET_DEL_RECV
{
	
	PBMSG_HEAD header; // C1:78:04
	BYTE Info;
	BYTE Item;
	BYTE Creation;
	BYTE Chaos;

};
//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//
struct PMSG_ANS_MIX_SOCKETITEM_INVENTORY
{
	PSBMSG_HEAD h; // C1:78:01 GS->C
	BYTE btResult;
	BYTE btInsertKind;
	BYTE btResultItemPos;
};

class CSocketChaos
{
public:
	CSocketChaos();
	virtual ~CSocketChaos();
	void Load(char* path);
	void CGSocketChaosMix(PMSG_SOCKET_MIX_RECV *lpMsg,int aIndex);
	void UpgradeTheSlot(PMSG_UPGRADE_SLOT *lpMsg, int aIndex);
	void ExtractMix(PMSG_EXTRACT_RECV*lpMsg,int aIndex);
	void SocketItemCreateSeedSphereMix(PMSG_SEED_SPHERE_MIX_RECV *lpMsg,int aIndex);
	void SocketItemMountSeedSphereMix(PMSG_MOUNT_SEND_SPHERE_MIX_RECV *lpMsg,int aIndex);
	void SocketItemExtractSeedSphereMix(PMSG_EXTRACT_SEED_SPHERE_MIX_RECV *lpMsg,int aIndex);
	void SocketSeedSphereLevelUpMix(PMSG_SEED_SPHERE_LEVLE_PRCV *lpMsg,int aIndex);
	bool GetItemUpType(int index,ITEM_UP_INFO* lpInfo);
	bool GetSeedSphereUp(int index,SEED_SPHERE_UP* lpInfo);
#if(GAMESERVER_UPDATE>=1200)//2017/11/11
	void SocketItemDelSocket(PMSG_SOCKET_DEL_RECV *lpMsg,int aIndex);
	void GCAnsSocketItemMixInventory(int nIndex, BYTE btResult);
#endif
private:
		std::map<int,ITEM_UP_INFO> m_ItemUpInfo;
		std::map<int,SEED_SPHERE_UP> m_SeedSphereUp;
};
extern CSocketChaos gSocketChaos;