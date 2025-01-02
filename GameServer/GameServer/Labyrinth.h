#pragma once

#include "Protocol.h"
/* 4315 */
struct ST_LABYRINTH_MOVE_ZONE_LIST
{
	ST_LABYRINTH_MOVE_ZONE_LIST()
	{
		this->Clear();
	}
	void Clear()
	{
		this->btVisitedZone = 0;
		this->btZoneType = 0;
		memset(this->btGate, 0, sizeof(this->btGate));
	}
	BYTE btVisitedZone;
	BYTE btZoneType;
	BYTE btGate[4];
};



enum EN_GATE_POSITION
{
	POS_UP = 0x0,
	POS_DOWN = 0x1,
	POS_LEFT = 0x2,
	POS_RIGHT = 0x3,
};

/* 149 */
enum EN_GATE_STATE
{
	LAB_GATE_CLOSE = 0x0,
	LAB_GATE_OPENED = 0x1,
	LAB_GATE_OPTIONED = 0x2,
	LAB_GATE_REQ_USE_KEY = 0x3,
};

/* 150 */
enum EN_LAB_ZONE_TYPE
{
	LAB_ZONE_TYPE_NONE = 0x0,
	LAB_ZONE_TYPE_ENTERANCE = 0x1,
	LAB_ZONE_TYPE_NORMAL = 0x2,
	LAB_ZONE_TYPE_TRAP = 0x3,
	LAB_ZONE_TYPE_MAIN_MISSION = 0x4,
};
/* 152 */
enum EN_LAB_MISSION
{
	LAB_MISSION_NONE = 0xFFFFFFFF,
	LAB_MISSION_MONSTER_KILL = 0x1,
	LAB_MISSION_GATHERING_ZEN = 0x2,
	LAB_MISSION_DAMAGE = 0x3,
	LAB_MISSION_EXP = 0x4,
	LAB_MISSION_ASSAULTED_DAMAGE = 0x5,
	LAB_MISSION_MACRO_TIME = 0x6,
	LAB_MISSION_NOT_USE_HEALTH_POTION = 0x7,
	LAB_MISSION_NOT_USE_MANA_POTION = 0x8,
};

/* 153 */
enum EN_LAB_MISSION_STATE
{
	LAB_MISSION_STATE_NONE = 0x0,
	LAB_MISSION_ING = 0x1,
	LAB_MISSION_FAIL = 0x2,
	LAB_MISSION_SUCCESS = 0x3,
};





/* 156 */
enum EN_ENTIRE_LAB_CLEAR_STATE
{
	ENTIRE_LAB_CLEAR_NONE = 0x0,
	ENTIRE_LAB_CLEAR_SUCCESS = 0x1,
};

/* 157 */
enum _RUUD_REWARD_ITEMBAG_NAME
{
	RUUD_REWARD_ITEMBAG136 = 0x88,
	RUUD_REWARD_ITEMBAG137 = 0x89,
	RUUD_REWARD_ITEMBAG138 = 0x8A,
	RUUD_REWARD_ITEMBAG142 = 0x8E,
	RUUD_REWARD_ITEMBAG143 = 0x8F,
	RUUD_REWARD_ITEMBAG144 = 0x90,
	RUUD_REWARD_ITEMBAG149 = 0x95,
	RUUD_REWARD_ITEMBAG155 = 0x9B,
	RUUD_REWARD_ITEMBAG156 = 0x9C,
	RUUD_REWARD_ITEMBAG157 = 0x9D,
	RUUD_REWARD_ITEMBAG177 = 0xB1,
	RUUD_REWARD_ITEMBAG178 = 0xB2,
	RUUD_REWARD_ITEMBAG179 = 0xB3,
	RUUD_REWARD_ITEMBAG180 = 0xB4,
	RUUD_REWARD_ITEMBAG181 = 0xB5,
	RUUD_REWARD_ITEMBAG182 = 0xB6,
	RUUD_REWARD_ITEMBAG183 = 0xB7,
};

/* 158 */
enum EN_LAB_SYSTE_ERROR_STATE
{
	LAB_ERROR_NONE = 0x0,
	LAB_ERROR_INVALID_FILE_NAME = 0x1,
	LAB_ERROR_FAIL_OPEN_SCRIPT = 0x2,
	LAB_ERROR_ACCESS_OUT_OF_RANGE = 0x3,
	LAB_ERROR_NOT_FOUND_SUB_MISSION_INFO = 0x4,
	LAB_ERROR_NOT_FOUND_ENTERANCE_ZONE = 0x5,
	LAB_ERROR_NOT_USER = 0x6,
	LAB_ERROR_INVALID_USER = 0x7,
	LAB_ERROR_OUT_OF_RANGE_ZONE_PATH = 0x8,
	LAB_ERROR_MISMATCH_ZONE_TYPE_INDEX = 0x9,
};

/* 160 */
enum $821FED1B33AB463B3D19D85B87F8C44D
{
	EN_INSTANCE_STATE_NOT_USING = 0x0,
	EN_INSTANCE_STATE_RESERVED = 0x1,
	EN_INSTANCE_STATE_WAIT_OWNER_USER = 0x2,
	EN_INSTANCE_STATE_WAIT_USERS = 0x3,
	EN_INSTANCE_STATE_PLAYING = 0x4,
	EN_INSTANCE_STATE_END = 0x5,
	EN_INSTANCE_STATE_FREE_RESOURCE = 0x6,
};

/* 161 */
enum $EF57B522527C7D23C8371D5A68E15ABF
{
	EN_INSTANCE_ERROR_NONE = 0x0,
	EN_INSTANCE_ERROR_OUT_OF_RANGE_USER_INDEX = 0x1,
	EN_INSTANCE_ERROR_USER_NAME_IS_NULL = 0x2,
	EN_INSTANCE_ERROR_ALREADY_IN_MAP_USER = 0x3,
	EN_INSTANCE_ERROR_OUT_OF_RANGE_MONSTER_INDEX = 0x4,
	EN_INSTANCE_ERROR_ALREADY_IN_MAP_MONSTER_INDEX = 0x5,
	EN_INSTANCE_ERROR_INSTANCE_MAP_DATA_IS_NULL = 0x6,
	EN_INSTANCE_ERROR_INVALID_ATTR_SET_COORD_POINT = 0x7,
	EN_INSTANCE_ERROR_EXCEED_INSTANCE_OBJECT = 0x8,
	EN_INSTANCE_ERROR_EXCEED_INSTANCE_USER_COUNT = 0x9,
	EN_INSTANCE_ERROR_EXCEED_INSTANCE_MONSTER_COUNT = 0xA,
	EN_INSTANCE_ERROR_NOT_FOUND_INSTANCE_SYSTEM_INFO = 0xB,
	EN_INSTANCE_ERROR_LACK_OF_INSTANCE_OBJECT = 0xC,
};

/* 162 */
enum $994D7F7FFB7979082745B687108853F0
{
	EN_INSTANCE_REGIST_USER_FIND = 0x0,
	EN_INSTANCE_REGIST_USER_NOT_FIND = 0x1,
};

/* 163 */
enum INSTANCE_MAP_INDEX
{
	INST_MAP_INDEX_LABYRINTH = 500,
};
/* 381 */
enum EN_LAB_MISSION_CONDITION
{
	LAB_MISSION_COND_NONE = 0xFFFFFFFF,
	LAB_MISSION_COND_ON_KILLED = 0x0,
	LAB_MISSION_COND_USED = 0x1,
};
struct  _LAB_ZONE
{
	_LAB_ZONE()
	{
		this->Clear();
	}
	void Clear()
	{
		this->nRow = 0;
		this->nCol = 0;
		this->nLabIndex = 0;
		this->nZoneType = 0;
		this->nZoneIndex = 0;
		memset(this->btGate, 0, sizeof(this->btGate));
	}
	int nRow;
	int nCol;
	int nLabIndex;
	int nZoneType;
	int nZoneIndex;
	BYTE btGate[4];
};
/* 4221 */
struct  _LAB_ZONE_CONFIGURATION
{
	int nLabConfigNum;
	int nLabDimensionLevel;
	int nRow;
	int nCol;
	_LAB_ZONE stZone[100];
};

/* 4191 */
struct  _LAB_MAP_INFO
{
	int nLabIndex;
	unsigned __int16 wLabMapNumber;
	BYTE sX;
	BYTE sY;
	BYTE eX;
	BYTE eY;
};

/* 4197 */
struct  _LAB_ZONE_MISSION_TYPE_TRAIT
{
	int nMissionType;
	int nMissionValue;
};
/* 4198 */
struct  _LAB_ZONE_TRAIT
{
	int nZoneType;
	int nZoneIndex;
	_LAB_ZONE_MISSION_TYPE_TRAIT stMissionTrait[6];
	std::map<int, int, std::less<int>, std::allocator<std::pair<int const, int> > > mapMonsterGen;
};
/* 4212 */
struct  _LAB_REBALANCING_CONST_VALUE
{
	float fHP;
	float fDamageMin;
	float fDamageMax;
	float fDefence;
	float fExpLevel;
	float fJewelOfBless;
};
//**********************************************//
//**********************************************//
//**********************************************//
/* 4316 */
struct PMSG_LABYRINTH_MOVE_ZONE_LIST_COUNT
{
	PSWMSG_HEAD h;
	BYTE btVisitedZoneCount;
};
/* 4317 */
struct PMSG_LABYRINTH_MISSION_INFO
{
	PSBMSG_HEAD h;
	BYTE btMissionState;
	BYTE btMissionType;
	unsigned int nScoreValue;
	unsigned int nMissionValue;
};
/* 4318 */
struct PMSG_MAIN_MISSION_UPDATE
{
	PSBMSG_HEAD h;
	BYTE btMainMissionOrderingNum;
	BYTE btMissionState;
	BYTE btMissionType;
	unsigned int nScoreValue;
	unsigned int nMissionValue;
};
/* 4319 */
struct PMSG_NOTIFY_LABYRINTH_SUB_MISSION_FAILED
{
	PSBMSG_HEAD h;
};
/* 4312 */
struct PMSG_LABYRINTH_MAIN_MISSION_COUNT
{
	PSBMSG_HEAD h;
	BYTE btMainMissionCount;
};
/* 4314 */
struct PMSG_NOTIFY_LABYRINTH_RESULT
{
	PSBMSG_HEAD h;
	BYTE btLabyrinthLevel;
	BYTE btFailedState;
	unsigned __int64 nEntireExp;
	unsigned __int64 nEntireMonsterKillCnt;
	unsigned int nTotalRewardRuud;
	unsigned int nTotalJewelOfBless;
};
struct PMSG_REQ_MOVE_LABYRINTH_ZONE
{
	PSBMSG_HEAD h;
	BYTE btGatePosition;
};
/* 4224 */
struct PMSG_REQ_LABYRINTH_DIMENSION_LEVEL_UP
{
	PSBMSG_HEAD h;
};
/* 4301 */
struct SDHP_REQ_LABYRINTH_INFO
{
	PSWMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	int nUserIndex;
};
/* 4302 */
struct SDHP_ANS_LABYRINTH_INFO
{
	PSWMSG_HEAD h;
	int nUserIndex;
	BYTE btResult;
	BYTE btDimensionLevel;
	unsigned __int16 wConfigNum;
	BYTE btCurrentZone;
	BYTE btVisitedCnt;
	BYTE btVisitedList[200];
	__int64 nEntireExp;
	__int64 nEntireMonKillCnt;
	int nClearCnt;
	BYTE btClearState;
	BYTE btMissionCount;
};
/* 4303 */
struct  MISSION_INFO
{
	BYTE btZoneNumber;
	BYTE btMissionType;
	int nMissionValue;
	int nAcquisionValue;
	BYTE btMissionState;
	BYTE btIsMainMission;
	BYTE btMainMissionOrder;
	int nRewardItemType;
	int nRewardItemIndex;
	int nRewardValue;
	BYTE btRewardCheckState;
};
/* 4304 */
struct SDHP_REQ_LABYRINTH_INFO_SAVE
{
	PSWMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	int nUserIndex;
	BYTE btDimensionLevel;
	unsigned __int16 wConfigNum;
	char btCurrentZone;
	BYTE btVisitedCnt;
	BYTE btVisitedList[200];
	__int64 nEntireExp;
	__int64 nEntireMonKillCnt;
	int nClearCnt;
	BYTE btClearState;
	BYTE btMissionCount;
};
/* 4305 */
struct SDHP_REQ_LABYRINTH_INFO_UPDATE
{
	PSWMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	int nUserIndex;
	BYTE btDimensionLevel;
	unsigned __int16 wConfigNum;
	BYTE btCurrentZone;
	BYTE btVisitedCnt;
	BYTE btVisitedList[200];
	__int64 nEntireExp;
	__int64 nEntireMonKillCnt;
};
/* 4306 */
struct  SDHP_REQ_LABYRINTH_MISSION_UPDATE
{
	PSWMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	int nUserIndex;
	BYTE btZoneNumber;
	BYTE btMissionType;
	int nMissionValue;
	int nAcquisionValue;
	BYTE btMissionState;
	BYTE btIsMainMission;
	BYTE btMainMissionOrder;
	int nRewardItemType;
	int nRewardItemIndex;
	int nRewardValue;
	BYTE btRewardCheckState;
};
/* 4307 */
struct SDHP_REQ_LABYRINTH_MISSION_DELETE
{
	PSWMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	int nUserIndex;
};
/* 4308 */
struct  SDHP_REQ_LABYRINTH_END_UPDATE
{
	PSWMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	int nUserIndex;
	int nClearCnt;
	BYTE btClearState;
};
/* 4309 */
struct  SDHP_REQ_LABYRINTH_REWARD_COMPLETE
{
	PSWMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	int nUserIndex;
	BYTE btIsMainMission;
	BYTE btRewardCheckState;
};
/* 4310 */
struct SDHP_REQ_LABYRINTH_CLEAR_LOG_SET_SAVE
{
	PSWMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	int nUserIndex;
	int nDimensionLevel;
};
/* 4311 */
struct PMSG_ANS_MOVE_LABYRINTH_ZONE
{
	PSBMSG_HEAD h;
	BYTE btResult;
	BYTE btMapNumber;
	BYTE posX;
	BYTE posY;
	BYTE btMaxColumn;
	BYTE btMaxRow;
	BYTE btColumn;
	BYTE btRow;
	BYTE btMapIndex;
	BYTE btZoneType;
	BYTE btZoneIndex;
	BYTE btGate[4];
};


/* 4320 */
struct PMSG_ANS_ENTER_STATE_LABYRINTH
{
	PSBMSG_HEAD h;
	bool bEnterPossible;
	BYTE btResultType;
	unsigned int dwLeftTime;
};
/* 4321 */
struct PMSG_NOTIFY_LABYRINTH_DIMENSION_ENTER
{
	PSBMSG_HEAD h;
	BYTE btLabyrinthLevel;
	int bIsFinal;
};




class Labyrinth
{
public:
	Labyrinth();
	~Labyrinth();

	void LoadScript(  char *szFileName); // idb
	_LAB_ZONE *GetLabyrinthZone( int nLabConfigNum, int nZoneNumber, int *nErrorCode); // idb
	double GetJewelOfBlessValue( int nDimensionLevel);
	int LeaveLabyrinth( int aIndex, int nLabConfigNum); // idb
	int ReBalanceMonster( LPOBJ lpObj, LPOBJ lpMonsterObj); // idb
	int ChangeMissionTypeMaxLevel( int aIndex, int nZoneType, int nZoneIndex, int *nMissionType, int *nMissionValue); // idb
	void CGReqCheckLabyrinthEntering( int aIndex); // idb
	void CGReqEnterLabyrinth( int aIndex); // idb
	void CGReqMoveLabyrinthZone( int aIndex, PMSG_REQ_MOVE_LABYRINTH_ZONE *pMsg); // idb
	void CGReqLevelUpLabyrinth( int aIndex, PMSG_REQ_LABYRINTH_DIMENSION_LEVEL_UP *pMsg); // idb
	void GDReqLabyrinthInfo( int aIndex); // idb
	void DGAnsLabyrinthInfo( BYTE *aRecv); // idb
	void GDReqLabyrinthInfoSave( int aIndex); // idb
	void GDReqLabyrinthInfoUpdate( int aIndex); // idb
	void GDReqLabyrinthMissionInfoUpdate( int aIndex, _LAB_MISSION *stMission); // idb
	void GDReqGJSetStateUpdate( LPOBJ lpObj); // idb
	void GDReqLabyrinthMissionDelete( int aIndex); // idb
	void GDReqLabyrinthEndUpdate( int aIndex); // idb
	void GDReqLabyrinthRewardCompleteUpdate( int aIndex, char btRewardState); // idb
	void GDReqLabyrinthClearLogSave( int aIndex, int nDimensionLevel); // idb
	int AddLabyrinthMissionDataSet( int aIndex, int nLabConfigNum); // idb
	int GetLabMissionTypeValue( int nZoneType, int nZoneIndex, int *nMissionType, int *nMissionValue); // idb
	int FindEntranceZone( int nLabConfigNum, int *nZoneNumber); // idb
	int FindMainMissionZone( int nLabConfigNum, int *nZoneNumber); // idb
	int MoveLabyrinthZone( int aIndex, int nLabConfigNum, int nMoveZoneNumber); // idb
	int Coordi2ZoneNum(int nMaxRow, int nMaxCol, int nRow, int nCol);
	void ZoneNum2Coordi(int nMaxRow, int nMaxCol, int nZoneNum, int *nRow, int *nCol);
	BOOL CheckZoneBoundary( int nMaxRow, int nMaxCol, int nZoneNum, int nGatePosition);
	void LoggingLabyrinthInfo( int aIndex); // idb
	void LoggingLabyrinthMissionInfo(int aIndex);
	void GM_LabyrinthMissionClear( LPOBJ lpObj); // idb
	void GM_LabyrintSelectConfiguration( LPOBJ lpObj, int nConfigureNumber); // idb
	void SendRequestEnterResult( int aIndex, bool bEnterFlag, char btResult); // idb
	void SendNotifyLabyrinthDimensionEnter( int aIndex); // idb
	int  CheckUser(int aIndex, char *szCallerName, char *szFile, int nLine);
	bool  IsLabyrinthMapNumber(unsigned __int16 wMapNumber);
	bool IsLabyrinthPlayFatiqueLimit(int aIndex);
private:
public:
	//LabyrinthVtbl * vfptr;
	std::vector<_LAB_MAP_INFO > m_vecMapInfo;
	std::vector<_LAB_ZONE_TRAIT> m_vecZoneTrait;
	std::map<int, _LAB_REBALANCING_CONST_VALUE> m_mapRebalanceValue;
	_LAB_ZONE NULL_LAB_ZONE;
	std::vector<_LAB_ZONE_CONFIGURATION> m_vecZoneConfig;
};


extern Labyrinth g_Labyrinth;