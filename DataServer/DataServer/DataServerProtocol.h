#pragma once

#define INVENTORY_SIZE 237
#define MAX_EFFECT_LIST 32
#define MAX_SKILL_LIST 60
#define MAX_ACCOUNT_LEN	10
#define MAX_ITEM_INFO 16
#define MAX_CHARACTERS 8

#define SET_NUMBERHB(x) ((BYTE)((DWORD)(x)>>(DWORD)8))
#define SET_NUMBERLB(x) ((BYTE)((DWORD)(x)&0xFF))
#define SET_NUMBERHW(x) ((WORD)((DWORD)(x)>>(DWORD)16))
#define SET_NUMBERLW(x) ((WORD)((DWORD)(x)&0xFFFF))
#define SET_NUMBERHDW(x) ((DWORD)((QWORD)(x)>>(QWORD)32))
#define SET_NUMBERLDW(x) ((DWORD)((QWORD)(x)&0xFFFFFFFF))

#define MAKE_NUMBERW(x,y) ((WORD)(((BYTE)((y)&0xFF))|((BYTE)((x)&0xFF)<<8)))
#define MAKE_NUMBERDW(x,y) ((DWORD)(((WORD)((y)&0xFFFF))|((WORD)((x)&0xFFFF)<<16)))
#define MAKE_NUMBERQW(x,y) ((QWORD)(((DWORD)((y)&0xFFFFFFFF))|((DWORD)((x)&0xFFFFFFFF)<<32)))

//**********************************************//
//************ Packet Base *********************//
//**********************************************//

struct PBMSG_HEAD
{
	void set(BYTE head,BYTE size) // OK
	{
		this->type = 0xC1;
		this->size = size;
		this->head = head;
	}

	void setE(BYTE head,BYTE size) // OK
	{
		this->type = 0xC3;
		this->size = size;
		this->head = head;
	}

	BYTE type;
	BYTE size;
	BYTE head;
};

struct PSBMSG_HEAD
{
	void set(BYTE head,BYTE subh,BYTE size) // OK
	{
		this->type = 0xC1;
		this->size = size;
		this->head = head;
		this->subh = subh;
	}

	void setE(BYTE head,BYTE subh,BYTE size) // OK
	{
		this->type = 0xC3;
		this->size = size;
		this->head = head;
		this->subh = subh;
	}

	BYTE type;
	BYTE size;
	BYTE head;
	BYTE subh;
};

struct PWMSG_HEAD
{
	void set(BYTE head,WORD size) // OK
	{
		this->type = 0xC2;
		this->size[0] = SET_NUMBERHB(size);
		this->size[1] = SET_NUMBERLB(size);
		this->head = head;
	}

	void setE(BYTE head,WORD size) // OK
	{
		this->type = 0xC4;
		this->size[0] = SET_NUMBERHB(size);
		this->size[1] = SET_NUMBERLB(size);
		this->head = head;
	}

	BYTE type;
	BYTE size[2];
	BYTE head;
};

struct PSWMSG_HEAD
{
	void set(BYTE head,BYTE subh,WORD size) // OK
	{
		this->type = 0xC2;
		this->size[0] = SET_NUMBERHB(size);
		this->size[1] = SET_NUMBERLB(size);
		this->head = head;
		this->subh = subh;
	}

	void setE(BYTE head,BYTE subh,WORD size) // OK
	{
		this->type = 0xC4;
		this->size[0] = SET_NUMBERHB(size);
		this->size[1] = SET_NUMBERLB(size);
		this->head = head;
		this->subh = subh;
	}

	BYTE type;
	BYTE size[2];
	BYTE head;
	BYTE subh;
};

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_SERVER_INFO_RECV
{
	PBMSG_HEAD header; // C1:00
	BYTE type;
	WORD ServerPort;
	char ServerName[50];
	WORD ServerCode;
};

struct SDHP_CHARACTER_LIST_RECV
{
	PBMSG_HEAD header; // C1:01
	WORD index;
	char account[11];
};

struct SDHP_CHARACTER_CREATE_RECV
{
	PBMSG_HEAD header; // C1:02
	WORD index;
	char account[11];
	char name[11];
	BYTE Class;
};

struct SDHP_CHARACTER_DELETE_RECV
{
	PBMSG_HEAD header; // C1:03
	WORD index;
	char account[11];
	char name[11];
	BYTE guild;
	char GuildName[9];
};

struct SDHP_CHARACTER_INFO_RECV
{
	PBMSG_HEAD header; // C1:04
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_CREATE_ITEM_RECV
{
	PBMSG_HEAD header; // C1:07
	WORD index;
	char account[11];
	BYTE X;
	BYTE Y;
	BYTE Map;
	WORD ItemIndex;
	BYTE Level;
	BYTE Dur;
	BYTE Option1;
	BYTE Option2;
	BYTE Option3;
	BYTE NewOption;
	WORD LootIndex;
	BYTE SetOption;
	BYTE JewelOfHarmonyOption;
	BYTE ItemOptionEx;
	BYTE SocketOption[5];
	BYTE SocketOptionBonus;
	DWORD Duration;
};

struct SDHP_OPTION_DATA_RECV
{
	PBMSG_HEAD header; // C1:08
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_PET_ITEM_INFO_RECV
{
	PWMSG_HEAD header; // C2:09
	WORD index;
	char account[11];
	BYTE type;
	BYTE count;
};

struct SDHP_PET_ITEM_INFO1
{
	BYTE slot;
	DWORD serial;
};

struct SDHP_CHARACTER_NAME_CHECK_RECV
{
	PBMSG_HEAD header; // C1:0A
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_CHARACTER_NAME_CHANGE_RECV
{
	PBMSG_HEAD header; // C1:0B
	WORD index;
	char account[11];
	char OldName[11];
	char NewName[11];
};

struct SDHP_CRYWOLF_SYNC_RECV
{
	PBMSG_HEAD header; // C1:1E
	WORD MapServerGroup;
	DWORD CrywolfState;
	DWORD OccupationState;
};

struct SDHP_CRYWOLF_INFO_RECV
{
	PBMSG_HEAD header; // C1:1F
	WORD MapServerGroup;
};

struct SDHP_GLOBAL_POST_RECV
{
	PBMSG_HEAD header; // C1:20
	WORD MapServerGroup;
	BYTE type;
	char name[11];
	char message[60];
};

struct SDHP_GLOBAL_NOTICE_RECV
{
	PBMSG_HEAD header; // C1:21
	WORD MapServerGroup;
	BYTE type;
	BYTE count;
	BYTE opacity;
	WORD delay;
	DWORD color;
	BYTE speed;
	char message[128];
};

struct SDHP_GLOBAL_POPUP_RECV
{
	PBMSG_HEAD header; // C1:20
	WORD MapServerGroup;
	char message[512];
};

struct SDHP_SNS_DATA_RECV
{
	PBMSG_HEAD header; // C1:24
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_CHARACTER_INFO_SAVE_RECV
{
	PWMSG_HEAD header; // C2:30
	WORD index;
	char account[11];
	char name[11];
	WORD Level;
	BYTE Class;
	DWORD LevelUpPoint;
	DWORD Experience;
	DWORD Money;
	DWORD Strength;
	DWORD Dexterity;
	DWORD Vitality;
	DWORD Energy;
	DWORD Leadership;
	DWORD Life;
	DWORD MaxLife;
	DWORD Mana;
	DWORD MaxMana;
	DWORD BP;
	DWORD MaxBP;
	DWORD Shield;
	DWORD MaxShield;
	BYTE Inventory[INVENTORY_SIZE][16];
	BYTE Skill[MAX_SKILL_LIST][3];
	BYTE Map;
	BYTE X;
	BYTE Y;
	BYTE Dir;
	DWORD PKCount;
	DWORD PKLevel;
	DWORD PKTime;
	BYTE Quest[50];
	WORD FruitAddPoint;
	WORD FruitSubPoint;
	BYTE Effect[MAX_EFFECT_LIST][14];
	BYTE ExtInventory;
	BYTE ExtWarehouse;
	DWORD Ruud;
	BYTE HuntingUserOpen;
	DWORD ChatLimitTime;
	DWORD AutoDt[5];
	BYTE AutoRr;
	BYTE AutoRe;
	BYTE IllusionTemple_Enter;
	BYTE IllusionTemple_Wins;
	BYTE ImperialGuardian_Enter;
	BYTE BloodCastle_Enter;
	BYTE DevilSquare_Enter;
	BYTE ChaosCastle_Enter;
};

struct SDHP_INVENTORY_ITEM_SAVE_RECV
{
	PWMSG_HEAD header; // C2:31
	WORD index;
	char account[11];
	char name[11];
	BYTE Inventory[INVENTORY_SIZE][16];
};

struct SDHP_OPTION_DATA_SAVE_RECV
{
	PBMSG_HEAD header; // C1:33
	WORD index;
	char account[11];
	char name[11];
	BYTE SkillKey[20];
	BYTE GameOption;
	BYTE QKey;
	BYTE WKey;
	BYTE EKey;
	BYTE ChatWindow;
	BYTE RKey;
	DWORD QWERLevel;
	BYTE ChangeSkin;
	WORD favWarpData[5];
	WORD favWarpId[5];
};

struct SDHP_PET_ITEM_INFO_SAVE_RECV
{
	PWMSG_HEAD header; // C2:34
	WORD index;
	char account[11];
	BYTE count;
};

struct SDHP_PET_ITEM_INFO_SAVE
{
	DWORD serial;
	BYTE level;
	DWORD experience;
};

struct SDHP_RESET_INFO_SAVE_RECV
{
	PBMSG_HEAD header; // C1:39
	WORD index;
	char account[11];
	char name[11];
	UINT Reset;
	UINT ResetDay;
	UINT ResetWek;
	UINT ResetMon;
};

struct SDHP_PK_INFO_SAVE_RECV
{
	PBMSG_HEAD header; // C1:39
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_ITEMTRACKING_SAVE_RECV
{
	PSBMSG_HEAD header;
	DWORD serial;
	BYTE action;
	BYTE subaction;
	BYTE itemOld[16];
	BYTE itemNew[16];
	char accountOld[11];
	char accountNew[11];
};

struct SDHP_PK_SAVE_RECV
{
	PSBMSG_HEAD header;
	char killer[11];
	char victim[11];
	int map;
	BYTE x;
	BYTE y;
	int helpers;
};

struct SDHP_BACKSPRING_SAVE_RECV
{
	PSBMSG_HEAD header;
	char author[11];
	char victim[11];
	BYTE situation;
};

struct SDHP_MASTER_RESET_INFO_SAVE_RECV
{
	PBMSG_HEAD header; // C1:3A
	WORD index;
	char account[11];
	char name[11];
	UINT Reset;
	UINT MasterReset;
	UINT MasterResetDay;
	UINT MasterResetWek;
	UINT MasterResetMon;
};

struct SDHP_RANKING_DUEL_SAVE_RECV
{
	PBMSG_HEAD header; // C1:3C
	WORD index;
	char account[11];
	char name[11];
	DWORD WinScore;
	DWORD LoseScore;
};

struct SDHP_RANKING_BLOOD_CASTLE_SAVE_RECV
{
	PBMSG_HEAD header; // C1:3D
	WORD index;
	char account[11];
	char name[11];
	DWORD score;
};

struct SDHP_RANKING_CHAOS_CASTLE_SAVE_RECV
{
	PBMSG_HEAD header; // C1:3E
	WORD index;
	char account[11];
	char name[11];
	DWORD score;
};

struct SDHP_RANKING_DEVIL_SQUARE_SAVE_RECV
{
	PBMSG_HEAD header; // C1:3F
	WORD index;
	char account[11];
	char name[11];
	DWORD score;
};

struct SDHP_RANKING_ILLUSION_TEMPLE_SAVE_RECV
{
	PBMSG_HEAD header; // C1:40
	WORD index;
	char account[11];
	char name[11];
	DWORD score;
};

struct SDHP_CREATION_CARD_SAVE_RECV
{
	PBMSG_HEAD header; // C1:42
	WORD index;
	char account[11];
	BYTE ExtClass;
};

struct SDHP_CRYWOLF_INFO_SAVE_RECV
{
	PBMSG_HEAD header; // C1:49
	WORD MapServerGroup;
	DWORD CrywolfState;
	DWORD OccupationState;
};

struct SDHP_SNS_DATA_SAVE_RECV
{
	PWMSG_HEAD header; // C1:4E
	WORD index;
	char account[11];
	char name[11];
	BYTE data[256];
};

struct SDHP_CUSTOM_MONSTER_REWARD_SAVE_RECV
{
	PBMSG_HEAD header; // C1:52
	WORD index;
	char account[11];
	char name[11];
	DWORD MonsterClass;
	DWORD MapNumber;
	DWORD RewardValue1;
	DWORD RewardValue2;
	DWORD RewardValue3;
	DWORD Damage;
};

struct SDHP_RANKING_CUSTOM_ARENA_SAVE_RECV
{
	PBMSG_HEAD header; // C1:55
	WORD index;
	char account[11];
	char name[11];
	DWORD number;
	DWORD score;
	DWORD rank;
};

struct SDHP_CONNECT_CHARACTER_RECV
{
	PBMSG_HEAD header; // C1:70
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_DISCONNECT_CHARACTER_RECV
{
	PBMSG_HEAD header; // C1:71
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_GLOBAL_WHISPER_RECV
{
	PBMSG_HEAD header; // C1:72
	WORD index;
	char account[11];
	char name[11];
	char TargetName[11];
	char message[60];
};
struct _SDHP_REQ_RESTORE_ITEM_LIST_SAVE
{
	PSWMSG_HEAD h;
	int userIndex;
	char AccountID[11];
	char Name[11];
	BYTE dbRestoreItemData[150];
};
struct __declspec(align(4)) _SDHP_REQ_RESTORE_ITEM_LIST
{
	PSWMSG_HEAD h;
	int userIndex;
	char AccountID[11];
	char Name[11];
};
struct _tagPMSG_REQ_BOMB_HUNT_SELECT_DS
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};
struct _tagPMSG_REQ_BOMB_HUNT_INSERT_DS
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
	WORD wScore;
	BYTE btGameState;
	BYTE szTileState[49];
};
struct _tagPMSG_REQ_BOMB_HUNT_DELETE_DS
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};
struct __declspec(align(2)) _tagPMSG_REQ_BOMB_HUNT_LOG_INSERT_DS
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD wScore;
	BYTE btClear;
};
/* 706 */
struct _ST_BLOCK_CHATTING_USER
{
	bool bIsUsed;
	BYTE btSlotNum;
	char szCharName[11];
};
/* 708 */
struct SDHP_REQ_ADD_BLOCK_CHAT_USER_INFO
{
	PSBMSG_HEAD h;
	int nUserIndex;
	char szCharName[11];
	_ST_BLOCK_CHATTING_USER szBlockChar;
};
/* 709 */
struct SDHP_REQ_DEL_BLOCK_CHAT_USER_INFO
{
	PSBMSG_HEAD h;
	int nUserIndex;
	char szCharName[11];
	_ST_BLOCK_CHATTING_USER szBlockChar;
};
/* 1127 */
struct  SDHP_REQ_BLOCK_CHAT_USER_INFO
{
	PBMSG_HEAD h;
	int iUserIndex;
	char szCharName[11];
};

struct HARDWARE_ID_RECV
{
	PSBMSG_HEAD h;
	char Account[11];
	char HarwareId[45];
};

struct SAVE_DT_RECV
{
	PSBMSG_HEAD h;
	int userIndex;
	char Name[11];
	char DtName[17];
	int Strength;
	int Dexterity;
	int Vitality;
	int Energy;
	int Leadership;
};

struct LOAD_DT_RECV
{
	PSBMSG_HEAD h;
	int userIndex;
	char Name[11];
	char DtName[17];
};

struct LOAD_DT_SEND
{
	PSBMSG_HEAD h;
	int userIndex;
	int result;
	int Strength;
	int Dexterity;
	int Vitality;
	int Energy;
	int Leadership;
};

//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_SERVER_INFO_SEND
{
	PBMSG_HEAD header; // C1:00
	BYTE result;
	DWORD ItemCount;
};

struct SDHP_CHARACTER_LIST_SEND
{
	PWMSG_HEAD header; // C2:01
	WORD index;
	char account[11];
	BYTE MoveCnt;
	BYTE ExtClass;
	WORD ExtWarehouse;
	BYTE count;
};

struct SDHP_CHARACTER_LIST
{
	BYTE slot;
	char name[11];
	WORD level;
	BYTE Class;
	BYTE CtlCode;
	BYTE Inventory[60];
	BYTE GuildStatus;
	BYTE PK_Level;
};

struct SDHP_CHARACTER_CREATE_SEND
{
	PBMSG_HEAD header; // C1:02
	WORD index;
	char account[11];
	char name[11];
	BYTE result;
	BYTE slot;
	BYTE Class;
	BYTE equipment[24];
	WORD level;
};

struct SDHP_CHARACTER_DELETE_SEND
{
	PBMSG_HEAD header; // C1:03
	WORD index;
	char account[11];
	BYTE result;
};

struct SDHP_CHARACTER_INFO_SEND
{
	PWMSG_HEAD header; // C2:04
	WORD index;
	char account[11];
	char name[11];
	BYTE result;
	BYTE Class;
	WORD Level;
	DWORD LevelUpPoint;
	DWORD Experience;
	DWORD Money;
	DWORD Strength;
	DWORD Dexterity;
	DWORD Vitality;
	DWORD Energy;
	DWORD Leadership;
	DWORD Life;
	DWORD MaxLife;
	DWORD Mana;
	DWORD MaxMana;
	DWORD BP;
	DWORD MaxBP;
	DWORD Shield;
	DWORD MaxShield;
	BYTE Inventory[INVENTORY_SIZE][16];
	BYTE Skill[MAX_SKILL_LIST][3];
	BYTE Map;
	BYTE X;
	BYTE Y;
	BYTE Dir;
	DWORD PKCount;
	DWORD PKLevel;
	DWORD PKTime;
	BYTE CtlCode;
	BYTE Quest[50];
	WORD FruitAddPoint;
	WORD FruitSubPoint;
	BYTE Effect[MAX_EFFECT_LIST][14];
	BYTE ExtInventory;
	BYTE ExtWarehouse;
	DWORD Reset;
	DWORD MasterReset;
	DWORD UseGuildMatching;
	DWORD UseGuildMatchingJoin;
	DWORD Ruud;
	DWORD ChatLimitTime;
	DWORD AutoDt[5];
	BYTE AutoRr;
	BYTE AutoRe;
	DWORD MasterLevel;
	BYTE IllusionTemple_Enter;
	BYTE IllusionTemple_Wins;
	BYTE ImperialGuardian_Enter;
	BYTE BloodCastle_Enter;
	BYTE DevilSquare_Enter;
	BYTE ChaosCastle_Enter;
};

struct SDHP_CREATE_ITEM_SEND
{
	PBMSG_HEAD header; // C1:07
	WORD index;
	char account[11];
	BYTE X;
	BYTE Y;
	BYTE Map;
	DWORD Serial;
	WORD ItemIndex;
	BYTE Level;
	BYTE Dur;
	BYTE Option1;
	BYTE Option2;
	BYTE Option3;
	BYTE NewOption;
	WORD LootIndex;
	BYTE SetOption;
	BYTE JewelOfHarmonyOption;
	BYTE ItemOptionEx;
	BYTE SocketOption[5];
	BYTE SocketOptionBonus;
	DWORD Duration;
};

struct SDHP_OPTION_DATA_SEND
{
	PBMSG_HEAD header; // C1:08
	WORD index;
	char account[11];
	char name[11];
	BYTE SkillKey[20];
	BYTE GameOption;
	BYTE QKey;
	BYTE WKey;
	BYTE EKey;
	BYTE ChatWindow;
	BYTE RKey;
	DWORD QWERLevel;
	BYTE ChangeSkin;
	WORD favMoveData[5];
	WORD favMoveId[5];
};

struct SDHP_PET_ITEM_INFO_SEND
{
	PWMSG_HEAD header; // C2:09
	WORD index;
	char account[11];
	BYTE type;
	BYTE count;
};

struct SDHP_PET_ITEM_INFO2
{
	BYTE slot;
	DWORD serial;
	BYTE level;
	DWORD experience;
};

struct SDHP_CHARACTER_NAME_CHECK_SEND
{
	PBMSG_HEAD header; // C1:0A
	WORD index;
	char account[11];
	char name[11];
	BYTE result;
};

struct SDHP_CHARACTER_NAME_CHANGE_SEND
{
	PBMSG_HEAD header; // C1:0B
	WORD index;
	char account[11];
	char OldName[11];
	char NewName[11];
	BYTE result;
};

struct SDHP_CRYWOLF_SYNC_SEND
{
	PBMSG_HEAD header; // C1:1E
	WORD MapServerGroup;
	DWORD CrywolfState;
	DWORD OccupationState;
};

struct SDHP_CRYWOLF_INFO_SEND
{
	PBMSG_HEAD header; // C1:1F
	WORD MapServerGroup;
	DWORD CrywolfState;
	DWORD OccupationState;
};

struct SDHP_GLOBAL_POST_SEND
{
	PBMSG_HEAD header; // C1:20
	WORD MapServerGroup;
	BYTE type;
	char name[11];
	char message[60];
};

struct SDHP_GLOBAL_NOTICE_SEND
{
	PBMSG_HEAD header; // C1:21
	WORD MapServerGroup;
	BYTE type;
	BYTE count;
	BYTE opacity;
	WORD delay;
	DWORD color;
	BYTE speed;
	char message[128];
};

struct SDHP_SNS_DATA_SEND
{
	PWMSG_HEAD header; // C1:24
	WORD index;
	char account[11];
	char name[11];
	BYTE result;
	BYTE data[256];
};

struct SDHP_GLOBAL_WHISPER_SEND
{
	PBMSG_HEAD header; // C1:72
	WORD index;
	char account[11];
	char name[11];
	BYTE result;
	char TargetName[11];
	char message[60];
};

struct SDHP_GLOBAL_WHISPER_ECHO_SEND
{
	PBMSG_HEAD header; // C1:73
	WORD index;
	char account[11];
	char name[11];
	char SourceName[11];
	char message[60];
};

struct _stGremoryCaseItem
{

	DWORD ItemGUID;
	time_t iReceiveDate;
	time_t iExpireDate;
	BYTE btRewardSource;
	BYTE btStorageType;
	DWORD dwAuthCode;
	BYTE ItemInfo[MAX_ITEM_INFO];

};
struct PMSG_DEFAULT2
{
	PBMSG_HEAD h;
	BYTE subcode;
};
struct _stReqDeleteItemFromGremoryCase
{
	PSBMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	DWORD dwItemGUID;
	DWORD dwAuthCode;
};
struct _stReqAddItemToGremoryCase
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	//_stGremoryCaseItem m_GremoryCaseItem;
	time_t iReceiveDate;
	time_t iExpireDate;
	BYTE btRewardSource;
	BYTE btStorageType;
	DWORD dwAuthCode;
	BYTE ItemInfo[MAX_ITEM_INFO];
};
struct _stAnsGremoryCaseItemList
{
	PSWMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	BYTE btResult;
	BYTE btCount;
};
struct _stReqCheckUseItemGremoryCase
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	DWORD dwItemGUID;
	DWORD dwAuthCode;
};
struct _stAnsCheckUseItemGremoryCase
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	BYTE btResult;
	DWORD dwItemGUID;
	DWORD dwAuthCode;
};
struct _stAnsAddItemToGremoryCase
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
	BYTE btResult;
	//_stGremoryCaseItem m_GremoryCaseItem;
	DWORD ItemGUID;
	time_t iReceiveDate;
	time_t iExpireDate;
	BYTE btRewardSource;
	BYTE btStorageType;
	DWORD dwAuthCode;
	BYTE ItemInfo[MAX_ITEM_INFO];
};
struct _stReqGremoryCaseItemList
{
	PSBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szName[MAX_ACCOUNT_LEN+1];
};

struct _tagPMSG_ANS_BOMB_HUNT_SELECT_DS
{
	PSBMSG_HEAD h;
	WORD aIndex;
	BYTE btResult;
	WORD wScore;
	BYTE btGameState;
	BYTE szTileState[49];
};
/* 707 */
struct SDHP_ANS_BLOCK_CHAT_USER_INFO
{
	PSWMSG_HEAD h;
	int nUserIndex;
	char szCharName[11];
	char btUserCnt;
	_ST_BLOCK_CHATTING_USER BlockCharList[20];
};
//**********************************************//
//**********************************************//
//**********************************************//

void DataServerProtocolCore(int index,BYTE head,BYTE* lpMsg,int size);
void GDServerInfoRecv(SDHP_SERVER_INFO_RECV* lpMsg,int index);
void GDCharacterListRecv(SDHP_CHARACTER_LIST_RECV* lpMsg,int index);
void GDCharacterCreateRecv(SDHP_CHARACTER_CREATE_RECV* lpMsg,int index);
void GDCharacterDeleteRecv(SDHP_CHARACTER_DELETE_RECV* lpMsg,int index);
void GDCharacterInfoRecv(SDHP_CHARACTER_INFO_RECV* lpMsg,int index);
void GDCreateItemRecv(SDHP_CREATE_ITEM_RECV* lpMsg,int index);
void GDOptionDataRecv(SDHP_OPTION_DATA_RECV* lpMsg,int index);
void GDFavoriteMoveRecv(SDHP_OPTION_DATA_RECV* lpMsg, int index);
void GDPetItemInfoRecv(SDHP_PET_ITEM_INFO_RECV* lpMsg,int index);
void GDCharacterNameCheckRecv(SDHP_CHARACTER_NAME_CHECK_RECV* lpMsg,int index);
void GDCharacterNameChangeRecv(SDHP_CHARACTER_NAME_CHANGE_RECV* lpMsg,int index);
void GDCrywolfSyncRecv(SDHP_CRYWOLF_SYNC_RECV* lpMsg,int index);
void GDCrywolfInfoRecv(SDHP_CRYWOLF_INFO_RECV* lpMsg,int index);
void GDGlobalPostRecv(SDHP_GLOBAL_POST_RECV* lpMsg,int index);
void GDGlobalNoticeRecv(SDHP_GLOBAL_NOTICE_RECV* lpMsg,int index);
void GDGlobalPopupRecv(SDHP_GLOBAL_POPUP_RECV* lpMsg, int index);
void GDSNSDataRecv(SDHP_SNS_DATA_RECV* lpMsg,int index);
void GDCharacterInfoSaveRecv(SDHP_CHARACTER_INFO_SAVE_RECV* lpMsg);
void GDInventoryItemSaveRecv(SDHP_INVENTORY_ITEM_SAVE_RECV* lpMsg);
void GDOptionDataSaveRecv(SDHP_OPTION_DATA_SAVE_RECV* lpMsg);
void GDPetItemInfoSaveRecv(SDHP_PET_ITEM_INFO_SAVE_RECV* lpMsg);
void GDResetInfoSaveRecv(SDHP_RESET_INFO_SAVE_RECV* lpMsg);
void GDPkInfoSaveRecv(SDHP_PK_INFO_SAVE_RECV* lpMsg);
void GDPKDetailsSaveRecv(SDHP_PK_SAVE_RECV* lpMsg);
void GDBackSpringDetailsSaveRecv(SDHP_BACKSPRING_SAVE_RECV* lpMsg);
void GDItemTrackingSaveRecv(SDHP_ITEMTRACKING_SAVE_RECV* lpMsg);
void GDMasterResetInfoSaveRecv(SDHP_MASTER_RESET_INFO_SAVE_RECV* lpMsg);
void GDRankingDuelSaveRecv(SDHP_RANKING_DUEL_SAVE_RECV* lpMsg);
void GDRankingBloodCastleSaveRecv(SDHP_RANKING_BLOOD_CASTLE_SAVE_RECV* lpMsg);
void GDRankingChaosCastleSaveRecv(SDHP_RANKING_CHAOS_CASTLE_SAVE_RECV* lpMsg);
void GDRankingDevilSquareSaveRecv(SDHP_RANKING_DEVIL_SQUARE_SAVE_RECV* lpMsg);
void GDRankingIllusionTempleSaveRecv(SDHP_RANKING_ILLUSION_TEMPLE_SAVE_RECV* lpMsg);
void GDCreationCardSaveRecv(SDHP_CREATION_CARD_SAVE_RECV* lpMsg);
void GDCrywolfInfoSaveRecv(SDHP_CRYWOLF_INFO_SAVE_RECV* lpMsg);
void GDSNSDataSaveRecv(SDHP_SNS_DATA_SAVE_RECV* lpMsg);
void GDCustomMonsterRewardSaveRecv(SDHP_CUSTOM_MONSTER_REWARD_SAVE_RECV* lpMsg);
void GDRankingCustomArenaSaveRecv(SDHP_RANKING_CUSTOM_ARENA_SAVE_RECV* lpMsg);
void GDConnectCharacterRecv(SDHP_CONNECT_CHARACTER_RECV* lpMsg,int index);
void GDDisconnectCharacterRecv(SDHP_DISCONNECT_CHARACTER_RECV* lpMsg,int index);
void GDGlobalWhisperRecv(SDHP_GLOBAL_WHISPER_RECV* lpMsg,int index);
void DGGlobalWhisperEchoSend(WORD ServerCode,WORD index,char* account,char* name,char* SourceName,char* message);

void GDReqGremoryCaseAddItem(int aIndex, _stReqAddItemToGremoryCase * aRecv);
void GDReqGremoryCaseItemList(int aIndex, _stReqGremoryCaseItemList * aRecv);
void GDReqGremoryCaseDeleteItem(int aIndex, _stReqDeleteItemFromGremoryCase * aRecv);
void GDReqCheckUseItemGremoryCase(int aIndex, _stReqCheckUseItemGremoryCase * aRecv);

void  GDReqLoadRestoreItemList(_SDHP_REQ_RESTORE_ITEM_LIST *lpRecv, int aIndex); // idb
void  GDReqSaveRestoreItemList(_SDHP_REQ_RESTORE_ITEM_LIST_SAVE *lpRecv);

void  GDReqBombHuntSelect(_tagPMSG_REQ_BOMB_HUNT_SELECT_DS *lpRecv, int aIndex);
void  GDReqBombHuntInsert(_tagPMSG_REQ_BOMB_HUNT_INSERT_DS *lpRecv);
void  GDReqBombHuntDelete(_tagPMSG_REQ_BOMB_HUNT_DELETE_DS *lpRecv);
void  GDReqBombLogInsert(_tagPMSG_REQ_BOMB_HUNT_LOG_INSERT_DS *lpRecv);

void  GDReqAddBlockChattingUser(SDHP_REQ_ADD_BLOCK_CHAT_USER_INFO *lpRecv);
void  GDReqDelBlockChattingUser(SDHP_REQ_DEL_BLOCK_CHAT_USER_INFO *lpRecv);
void  GDReqLoadBlockChattingUser(SDHP_REQ_BLOCK_CHAT_USER_INFO *lpRecv, int aIndex);
void SendHardwareId(char* Account, char* HardwareId);
void SendSaveDt(char* Name, char* DtName, int Strength, int Dexterity, int Vitality, int Energy, int Leadership);
void SendLoadDt(char* Name, char* DtName, int userIndex, int serverIndex);
void DS_GDSaveSiegeCharInfo(BYTE* lpRecv, int aIndex);
void DS_GDLoadSiegeCharInfo(BYTE* lpRecv, int aIndex);
//**************************************************************************//
// RAW FUNCTIONS ***********************************************************//
//**************************************************************************//

void DS_GDReqCastleTotalInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqOwnerGuildMaster(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleNpcBuy(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleNpcRepair(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleNpcUpgrade(BYTE *lpRecv, int aIndex);
void DS_GDReqTaxInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqTaxRateChange(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleMoneyChange(BYTE *lpRecv, int aIndex);
void DS_GDReqSiegeDateChange(BYTE *lpRecv, int aIndex);
void DS_GDReqGuildMarkRegInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqSiegeEndedChange(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleOwnerChange(BYTE *lpRecv, int aIndex);
void DS_GDReqRegAttackGuild(BYTE *lpRecv, int aIndex);
void DS_GDReqRestartCastleState(BYTE *lpRecv, int aIndex);
void DS_GDReqMapSvrMsgMultiCast(BYTE *lpRecv, int aIndex);
void DS_GDReqRegGuildMark(BYTE *lpRecv, int aIndex);
void DS_GDReqGuildMarkReset(BYTE *lpRecv, int aIndex);
void DS_GDReqGuildSetGiveUp(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleNpcRemove(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleStateSync(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleTributeMoney(BYTE *lpRecv, int aIndex);
void DS_GDReqResetCastleTaxInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqResetSiegeGuildInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqResetRegSiegeInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleInitData(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleNpcInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqAllGuildMarkRegInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqFirstCreateNPC(BYTE *lpRecv, int aIndex);
void DS_GDReqCalcRegGuildList(BYTE *lpRecv, int aIndex);
void DS_GDReqCsGuildUnionInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqCsSaveTotalGuildInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqCsLoadTotalGuildInfo(BYTE *lpRecv, int aIndex);
void DS_GDReqCastleNpcUpdate(BYTE *lpRecv, int aIndex);

void PHeadSubSetB(LPBYTE lpBuf, BYTE head, BYTE sub, int size);
