#pragma once
/* 155 */
enum EN_LAB_DIMENSION_LEVEL
{
	LAB_DIMENSION_LV_NONE = 0x0,
	LAB_DIMENSION_LV_1 = 0x1,
	LAB_DIMENSION_LV_2 = 0x2,
	LAB_DIMENSION_LV_3 = 0x3,
	LAB_DIMENSION_LV_4 = 0x4,
	LAB_DIMENSION_LV_5 = 0x5,
	LAB_DIMENSION_LV_6 = 0x6,
	LAB_DIMENSION_LV_7 = 0x7,
};
/* 154 */
enum EN_LAB_REWARD_STATE
{
	LAB_REWARD_STATE_NONE = 0x0,
	LAB_REWARD_STATE_SELECTED_REWARD = 0x1,
	LAB_REWARD_STATE_USER_CHECKED = 0x2,
	LAB_REWARD_STATE_COMPLETE_REWARD = 0x3,
};
/* 3094 */
struct  _LAB_MISSION
{
	_LAB_MISSION()
	{

	}
	void Clear()
	{
		this->nZoneNumber = 0;
		this->nIsMainMission = 0;
		this->nMainMissionOrdering = 0;
		this->nMissionType = 0;
		this->nMissionValue = 0;
		this->nScoreValue = 0;
		this->nMissionState = 0;
		this->nRewardItemType = 0;
		this->nRewardValue = 0;
		this->nRewardState = 0;
	}
	int nZoneNumber;
	int nIsMainMission;
	int nMainMissionOrdering;
	int nMissionType;
	unsigned int nMissionValue;
	unsigned int nScoreValue;
	int nMissionState;
	int nRewardItemType;
	int nRewardValue;
	int nRewardState;
};
/* 4313 */
struct ST_LABYRINTH_MAIN_MISSION
{
	BYTE btMainMissionOrderingNum;
	BYTE btMissionState;
	BYTE btMissionType;
	unsigned int nScoreValue;
	unsigned int nMissionValue;
};
class LabyrinthMission
{
public:
	LabyrinthMission();
	~LabyrinthMission();

	void InitMission(); // idb
	bool CheckClearMainMission(); // idb
	char CheckClearSubMission(int aIndex, int nZoneNumber, int *nErrorCode);
	_LAB_MISSION *GetSubMission(int nZoneNumber, int *nErrorCode); // idb
	_LAB_MISSION *GetMainMission(int nMainMissionOrdering, int *nErrorCode); // idb
	_LAB_MISSION *GetProgressQuestMission(int nZoneNumber, int nZoneType, int *nErrorCode); // idb
	void SendMainMissionInfo(int aIndex, int nZoneNumber); // idb
	void SelectRewardInfo(int aIndex, int nMainMissionOrdering); // idb
	unsigned int GetSubMissionCount();
	unsigned int GetMainMissionCount();
	int GetTotalRewardRuud(); // idb
	int InsertRewardItem(int aIndex, int nTotalRuud, int nJewelOfBless); // idb
	void LoggingLabyrinthMission(int aIndex); // idb
	void AddSubMission(_LAB_MISSION stLabMission); // idb
	void AddMainMission(_LAB_MISSION stLabMission); // idb
													//private:
	std::map<int, _LAB_MISSION> m_mapSubMission;
	std::vector<_LAB_MISSION > m_vecMainMission;
	//std::map<int, _LAB_MISSION, std::less<int>, std::allocator<std::pair<int const, _LAB_MISSION> > > m_mapSubMission;
	//std::vector<_LAB_MISSION, std::allocator<_LAB_MISSION> > m_vecMainMission;
	_LAB_MISSION NULL_MISSION;
};
extern LabyrinthMission g_LabyrinthMission;