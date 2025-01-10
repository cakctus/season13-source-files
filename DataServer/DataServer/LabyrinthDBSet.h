#pragma once
/* 6304 */
struct SDHP_REQ_LABYRINTH_INFO_UPDATE
{
	PSWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
	BYTE btDimensionLevel;
	unsigned __int16 wConfigNum;
	BYTE btCurrentZone;
	BYTE btVisitedCnt;
	BYTE btVisitedList[200];
	__int64 nEntireExp;
	__int64 nEntireMonKillCnt;
};

/* 6305 */
struct  SDHP_REQ_LABYRINTH_MISSION_UPDATE
{
	PSWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
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

/* 6306 */
struct SDHP_REQ_LABYRINTH_INFO
{
	PSWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
};

/* 6307 */
struct SDHP_REQ_LABYRINTH_MISSION_DELETE
{
	PSWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
};

/* 6308 */
struct  SDHP_REQ_LABYRINTH_END_UPDATE
{
	PSWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
	int nClearCnt;
	BYTE btClearState;
};

/* 6309 */
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
//struct  SDHP_ANS_LABYRINTH_INFO
//{
//	PSWMSG_HEAD h;
//	int nUserIndex;
//	BYTE btResult;
//	BYTE btDimensionLevel;
//	unsigned __int16 wConfigNum;
//	BYTE btCurrentZone;
//	int nLeftTime;
//	char btVisitedCnt;
//	BYTE btVisitedList[200];
//	BYTE btClearState;
//	BYTE btMissionCount;
//};

/* 6310 */
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

/* 6311 */
struct  SDHP_REQ_LABYRINTH_INFO_SAVE
{
	PSWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
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

/* 6312 */
struct  SDHP_REQ_LABYRINTH_REWARD_COMPLETE
{
	PSWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
	BYTE btIsMainMission;
	BYTE btRewardCheckState;
};

/* 6313 */
struct SDHP_REQ_LABYRINTH_CLEAR_LOG_SET_SAVE
{
	PSWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
	int nDimensionLevel;
};
class LabyrinthDBSet
{
public:
	LabyrinthDBSet();
	~LabyrinthDBSet();
	void  GDReqLabyrinthInfo(SDHP_REQ_LABYRINTH_INFO *lpRecv, int aIndex); // idb
	void  GDReqLabyrinthInfoSave(BYTE *lpRecv);
	void  GDReqLabyrinthInfoUpdate(SDHP_REQ_LABYRINTH_INFO_UPDATE *lpRecv);
	void  GDReqLabyrinthMissionUpdate(SDHP_REQ_LABYRINTH_MISSION_UPDATE *lpRecv);
	void  GDReqLabyrinthMissionDelete(SDHP_REQ_LABYRINTH_MISSION_DELETE *lpRecv);
	void  GDReqLabyrinthEndUpdate(SDHP_REQ_LABYRINTH_END_UPDATE *lpRecv);
	void  GDReqLabyrinthRewardComplete(SDHP_REQ_LABYRINTH_REWARD_COMPLETE *lpRecv);
	void  GDReqLabyrinthSaveClearLog(SDHP_REQ_LABYRINTH_CLEAR_LOG_SET_SAVE *lpRecv);

	int  Connect(); // idb
	int  LoadLabyrinthInfo(char *szAccountID, char *szName, int nUserIndex, BYTE *Buffer, int *nBufferLen); // idb

	int  SaveLabyrinthInfo(BYTE *aRecv); // idb
	unsigned __int64 UpdateLabyrinthInfo( SDHP_REQ_LABYRINTH_INFO_UPDATE *aRecv);
	int  UpdateLabyrinthMission(SDHP_REQ_LABYRINTH_MISSION_UPDATE *aRecv); // idb
	int  DeleteLabyrinthMission(char *szAccountID, char *szName); // idb
	int  EndUpdateLabyrinthInfo(char *szAccountID, char *szName, int nClearCnt, BYTE btClearState); // idb
	int  UpdateLabyrinthRewardState(char *szAccountID, char *szName, BYTE btIsMainMission, BYTE btRewardCheckState); // idb
	int  SaveLabyrinthClearLog(char *szAccountID, char *szName, int nDimensionLevel); // idb
};

extern LabyrinthDBSet g_LabyrinthDBSet;