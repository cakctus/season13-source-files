// IllusionTemple.h: interface for the CIllusionTemple class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MapItem.h"
#include "Protocol.h"
#include "User.h"

#define MAX_IT_LEVEL 4
#define MAX_IT_USER 10
#define MAX_IT_TEAM 2
#define MAX_IT_RANK 10

#define GET_IT_LEVEL(x) (((x)<MAP_ILLUSION_TEMPLE1)?-1:((x)>MAP_ILLUSION_TEMPLE6)?-1:((x)-MAP_ILLUSION_TEMPLE1))

#define IT_LEVEL_RANGE(x) (((x)<0)?0:((x)>=MAX_IT_LEVEL)?0:1)

enum eIllusionTempleState
{
	IT_STATE_BLANK = 0,
	IT_STATE_EMPTY = 1,
	IT_STATE_STAND = 2,
	IT_STATE_START = 3,
	IT_STATE_CLEAN = 4,
	IT_STATE_ROUNDBREAK = 5
};

enum __ITR_OCCUPIEDSTONE
{
	IT_OCCUPIED_WIN_TEAM_A = 0x0,
	IT_OCCUPIED_WIN_TEAM_B = 0x1,
	IT_OCCUPIED_GETTING_RELICS_A = 0xA,
	IT_OCCUPIED_GETTING_RELICS_B = 0xB,
	IT_OCCUPIED_OCCUPYING_A = 0x64,
	IT_OCCUPIED_OCCUPYING_B = 0x65,
	IT_OCCUPIED_INVALIDATION_ZONE = 0xFE,
	IT_OCCUPIED_NEUTRAL_ZONE = 0xFF,
};

enum eIllusionTempleTeam
{
	IT_TEAM_NONE = -1,
	IT_TEAM_Y = 0,
	IT_TEAM_B = 1,
};

enum _ITR__RELICS_ERROR
{
	_ITR_RELICS_ERROR_OK = 0x0,
	_ITR_RELICS_ERROR_TICKISNOZERO = 0x1,
	_ITR_RELICS_ERROR_NPCERROR1 = 0x2,
	_ITR_RELICS_ERROR_NPCERROR2 = 0x3,
	_ITR_RELICS_ERROR_NPCERROR3 = 0x4,
	_ITR_RELICS_ERROR_TEAMERROR = 0x5,
	_ITR_RELICS_ERROR_STATUSERROR = 0x6,
	_ITR_RELICS_ERROR_TICKISNOVALID = 0x7,
	_ITR_RELICS_ERROR_NPCERROR4 = 0x8,
	_ITR_RELICS_ERROR_TEAMERROR2 = 0x9,
	_ITR_RELICS_ERROR_STATUSERROR2 = 0xA,
	_ITR_RELICS_ERROR_STATUSERROR4 = 0xB,
	_ITR_RELICS_ERROR_RELICSITEMERROR = 0xC,
	_ITR_RELICS_ERROR_NPCERROR5 = 0xD,
	_ITR_RELICS_ERROR_NPCERROR6 = 0xE,
	_ITR_RELICS_ERROR_STATUSERROR3 = 0xF,
	_ITR_RELICS_ERROR_TICKISNOVALID1 = 0x10,
	_ITR_RELICS_ERROR_RELICSITEMERROR1 = 0x11,
	_ITR_RELICS_ERROR_TICKISNOVALID2 = 0x12,
	_ITR_RELICS_ERROR_NPCERROR7 = 0x13,
	_ITR_RELICS_ERROR_NPCERROR8 = 0x14,
};

enum _ITR__RELICS_ACT
{
	_ITR_RELICS_ACT_CHECK_GET_RELICS = 0x1,
	_ITR_RELICS_ACT_GET_RELICS = 0x2,
	_ITR_RELICS_ACT_CHECK_REGISTER_RELICS = 0x3,
	_ITR_RELICS_ACT_REGISTER_RELICS = 0x4,
};

enum __ITR_RELICS_ACT
{
	_RELICS_GET_ITR = 0x0,
	_RELICS_THROW_ITR = 0x1,
};

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_ILLUSION_TEMPLE_ENTER_RECV
{
	PSBMSG_HEAD header; // C1:BF:70
};

struct PMSG_ILLUSION_TEMPLE_ACCEPT_RECV
{
	PSBMSG_HEAD header; // C1:BF:71
	BYTE enterOK;
};

struct PMSG_ILLUSION_TEMPLE_RELATE_RELICS
{
	PSBMSG_HEAD h; // C1:BF:6A
	BYTE NumberH;
	BYTE NumberL;
	WORD NpcType;
	BYTE byPrePacket;
	BYTE byAct;
};

struct PMSG_ILLUSION_TEMPLE_ERROR
{
	PSBMSG_HEAD h; // C1:BF:6A
	BYTE byError;
	BYTE byNumberH;
	BYTE byNumberL;
	WORD wNpcType;
	BYTE byAct;
};

struct PMSG_ILLUSION_TEMPLE_SKILL_RECV
{
	PSBMSG_HEAD header; // C1:BF:02
	BYTE skill[2];
	BYTE index[2];
	BYTE dir;
};

struct PMSG_ILLUSION_TEMPLE_STONE_INFO
{
	PSBMSG_HEAD h; // C1:BF:6C
	BYTE byMapTagIndex;
	WORD wOccupiedStoneType;
	BYTE byOccupiedStoneState;
	int nNpcIndex;
};

struct PMSG_ILLUSION_TEMPLE_RELICS_GET_USER
{
	PSBMSG_HEAD h; // C1:BF:61
	WORD wUserIndex;
	char Name[10];
	BYTE byGet;
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_ILLUSION_TEMPLE_ENTER_SEND
{
	PSBMSG_HEAD header; // C1:BF:00
	BYTE result;
	char Name[11];
};

struct PMSG_ILLUSION_TEMPLE_ACCEPT_SEND
{
	PSBMSG_HEAD header; // C1:BF:00
	BYTE result;
	char Name[11];
};

struct PMSG_ILLUSION_TEMPLE_BATTLE_INFO_SEND
{
	PSBMSG_HEAD header; // C1:BF:01
	WORD time;
	WORD EventItemOwner;
	BYTE EventItemOwnerX;
	BYTE EventItemOwnerY;
	BYTE score[2];
	BYTE team;
	BYTE count;
};

struct PMSG_ILLUSION_TEMPLE_BATTLE_INFO
{
	WORD index;
	BYTE map;
	BYTE x;
	BYTE y;
};

struct PMSG_ILLUSION_TEMPLE_ADD_SKILL_SEND
{
	PSBMSG_HEAD header; // C1:BF:02
	BYTE result;
	BYTE skill[2];
	WORD index;
	WORD target;
};

struct PMSG_ILLUSION_TEMPLE_USER_COUNT_SEND
{
	PSBMSG_HEAD header; // C1:BF:03
	BYTE count[MAX_IT_LEVEL];
};

struct PMSG_ITL_PLAY_STATE
{
	PSBMSG_HEAD h; // C1:BF:62 GS->C
	WORD wRemainSec;
	BYTE btAlliedPoint;
	BYTE btIllusionPoint;
	BYTE btMyTeam;
};

struct PMSG_ILLUSION_TEMPLE_REWARD_SCORE_SEND
{
	PSBMSG_HEAD header; // C1:BF:04
	BYTE btAlliedPoint;
	BYTE btIllusionPoint;
	BYTE btUserCount;
	BYTE byWinTeamIndex;
};

struct PMSG_ILLUSION_TEMPLE_REWARD_SCORE
{
	char name[10];
	BYTE map;
	BYTE team;
	BYTE Class;
	BYTE btUserKillCount;
};

struct PMSG_ILLUSION_TEMPLE_KILL_POINT_SEND
{
	PSBMSG_HEAD header; // C1:BF:06
	BYTE KillPoint;
};

struct PMSG_ILLUSION_TEMPLE_DEL_SKILL_SEND
{
	PSBMSG_HEAD header; // C1:BF:07
	BYTE skill[2];
	WORD index;
};

struct PMSG_ILLUSION_TEMPLE_EVENT_ITEM_OWNER_SEND
{
	PSBMSG_HEAD header; // C1:BF:08
	WORD index;
	char name[10];
};

struct PMSG_ILLUSION_TEMPLE_CHANGE_SCENARIO_SEND
{
	PSBMSG_HEAD header; // C1:BF:09
	BYTE level;
	BYTE state;
};

//**********************************************//
//**********************************************//
//**********************************************//

struct ILLUSION_TEMPLE_START_TIME
{
	int Year;
	int Month;
	int Day;
	int DayOfWeek;
	int Hour;
	int Minute;
	int Second;
};

struct ILLUSION_TEMPLE_USER
{
	int Index;
	int Party;
	bool EnterOK;
	int Team;
	int Point;
	int Score;
	int RewardExperience;
	int OccupiedCount;
};

struct ILLUSION_TEMPLE_OBJECT_DATA
{
	int Index;
	WORD Type;
	short PosNum;
	BYTE byStatus;
	//BYTE byMapTagIndex;
};

struct ILLUSION_TEMPLE_LEVEL
{
	//int TimerRandom;
	int State;
	int Map;
	int RemainTime;
	int TargetTime;
	int RoundBreakCount;
	int CurrentRound;
	int TickCount;
	int EnterEnabled;
	int MinutesLeft;
	int TimeCount;
	int Score[MAX_IT_TEAM];
	int PartyNumber[MAX_IT_TEAM];
	int WinnerTeam;
	int TeamRewardExperience[MAX_IT_TEAM];
	ILLUSION_TEMPLE_USER User[MAX_IT_USER];
	int StoneStatueIndex;
	ILLUSION_TEMPLE_OBJECT_DATA StoneStatue;
	ILLUSION_TEMPLE_OBJECT_DATA OccupiedStone[10];
};

class CIllusionTemple
{
public:
	CIllusionTemple();
	virtual ~CIllusionTemple();
	void Init();
	void Load(char* path);
	void MainProc();
	void ProcState_NONE(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void ProcState_EMPTY(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void ProcState_STAND(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void ProcState_START(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void ProcState_CLEAN(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void ProcState_ROUNDBREAK();
	void SetState(ILLUSION_TEMPLE_LEVEL* lpLevel,int state);
	void SetState_BLANK(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void SetState_EMPTY(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void SetState_STAND(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void SetState_START(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void SetState_CLEAN(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void SetState_ROUNDBREAK(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void CheckSync(ILLUSION_TEMPLE_LEVEL* lpLevel);
	int GetState();
	int GetRemainTime(int level);
	int GetEnterEnabled(int level);
	int GetEnteredUserCount(int level);
	bool DelUser(ILLUSION_TEMPLE_LEVEL* lpLevel,int aIndex);
	ILLUSION_TEMPLE_USER* GetUser(ILLUSION_TEMPLE_LEVEL* lpLevel,int aIndex);
	void CleanUser(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void ClearUser(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void CheckUser(ILLUSION_TEMPLE_LEVEL* lpLevel);
	int GetUserCount(ILLUSION_TEMPLE_LEVEL* lpLevel,int team);
	int GetUserJoinTeam(LPOBJ lpObj);
	bool GetUserRespawnLocation(LPOBJ lpObj,int* gate,int* map,int* x,int* y,int* dir,int* level);
	void SearchUserDropEventItem(int aIndex);
	void SearchUserDeleteEventItem(int aIndex);
	void ClearMonster(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void SetStoneStatue(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void NpcStoneStatue(PMSG_ILLUSION_TEMPLE_RELATE_RELICS* lpMsg, int aIndex);
	void CheckGetRelics(LPOBJ lpNpc, LPOBJ lpObj, WORD wNpcType);
	void CheckRegisterRelics(LPOBJ lpNpc, LPOBJ lpObj);
	void RegisterRelics(LPOBJ lpNpc, LPOBJ lpObj);
	void SendRelicsUserInfo(LPOBJ lpObj, BYTE byGet);
	void SendPlayState(BYTE btTeam);
	void GetOccupiedStoneCount(BYTE& byAllied, BYTE& byIllusion);
	void GetKillCount(BYTE& byAllied, BYTE& byIllusion);
	void RegenCursedStoneStatus(BYTE byRound);
	void RegenOccupiedStoneStatus();
	void CheckUserStoneState(LPOBJ lpObj);
	int HaveTicket(int aIndex);
	void DeleteTicket();
	void NpcMirageTheMummy(LPOBJ lpNpc,LPOBJ lpObj);
	void DropEventItem(int map,int number,int aIndex);
	void UserDieProc(LPOBJ lpObj,LPOBJ lpTarget);
	bool CheckPlayerTarget(LPOBJ lpObj,LPOBJ lpTarget);
	void NoticeSendToAll(int type,char* message,...);
	void DataSendToAll(BYTE* lpMsg,int size);
	void CGIllusionTempleEnterRecv(PMSG_ILLUSION_TEMPLE_ENTER_RECV* lpMsg,int aIndex);
	BYTE EnterCheckUser(int iIndex, int& TroubleUserIndex);
	void CGIllusionTempleAcceptRecv(PMSG_ILLUSION_TEMPLE_ACCEPT_RECV* lpMsg, int aIndex);
	void CGIllusionTempleSkillRecv(PMSG_ILLUSION_TEMPLE_SKILL_RECV* lpMsg,int aIndex);
	void CGIllusionTempleButtonClickRecv(int aIndex);
	void GCIllusionTempleAddSkillSend(int aIndex,int result,int skill,int bIndex);
	void GCIllusionTempleRewardScoreSend(ILLUSION_TEMPLE_LEVEL* lpLevel);
	void GCIllusionTempleKillPointSend(int aIndex,int point);
	void GCIllusionTempleDelSkillSend(int aIndex,int skill);
	void GCIllusionTempleChangeScenarioSend(int state,int aIndex);
	BYTE Find_EmptySlot(int PartyIndex, BYTE& byEmptySlot);
	BYTE FindEmptyPartySlot(int partyindex, BYTE& byEmptySlot);
	BYTE SetPartyTeam(BYTE bySlotOrder, int nPartyIdx, int nUserindex);
	BYTE DeletePartyTeam(int nPartyIndex);
	BYTE FindExistingPartyInfo(int partyinfo, BYTE& bySlotNum);
	BYTE FindSameParty(int partyindex, int& nUserindex);
	BYTE FindPartySlot(int partyindex, BYTE& bySlotNum);
	BOOL EnterIllusionTemple(int aIndex, BYTE bySlotNum);
	BOOL EnterUserIllusionTemple(int aIndex, int nPartyIdx);
	int SearchRelicsItem(int iIndex);
	void SetOccupiedStoneStatue(ILLUSION_TEMPLE_LEVEL* lpLevel);
	int GetOccupiedStoneStatue(ILLUSION_TEMPLE_LEVEL* lpLevel, int aIndex);
	void GetRelics(LPOBJ lpNpc, LPOBJ lpObj, WORD wNpcType);
	void ResetUserFlag_DoingRelicsThing(LPOBJ lpObj);
	void SendRelicsError(LPOBJ lpNpc, int index, BYTE byError, BYTE byAct);
	void GCSendStoneInfo(WORD wNpcType, BYTE byState, int NpcIndex);
private:
	ILLUSION_TEMPLE_LEVEL m_IllusionTempleLevel;
	int m_WarningTime;
	int m_NotifyTime;
	int m_EventTime;
	int m_CloseTime;
	std::vector<ILLUSION_TEMPLE_START_TIME> m_IllusionTempleStartTime;
	int m_IllusionTempleExperienceTable[2];
	CRITICAL_SECTION m_critical;
};

extern CIllusionTemple gIllusionTemple;
