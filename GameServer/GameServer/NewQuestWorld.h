#pragma once
#include "Protocol.h"
#define MAX_NEW_QUEST_WORLD 1000
#define MAX_NEW_QUEST_WORLD_LIST 4
#define MAX_ITEM_AWARD 3
enum eNewQuestWorld
{
	NEW_QUEST_WORLD_NORMAL = 0,
	NEW_QUEST_WORLD_ACCEPT = 1,
	NEW_QUEST_WORLD_SUCCESS = 2,
	NEW_QUEST_WORLD_FINISH = 3,
	NEW_QUEST_WORLD_CANCEL = 4,
};
struct NEW_QUEST_WORLD_LIST
{
	WORD QuestIndex;
	BYTE TagetNumber;
	BYTE QuestState;
};
//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//
struct PMSG_NEW_QUEST_WORLD_RECV
{
	#pragma pack(1)
	PSBMSG_HEAD header; // C1:F6:55
	NEW_QUEST_WORLD_LIST Quest;
	#pragma pack()
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//
struct PMSG_NEW_QUEST_WORLD_LOAD
{
	#pragma pack(1)
	PSBMSG_HEAD header; // C1:F6:20
	#pragma pack()
};
struct PMSG_NEW_QUEST_WORLD_INDEX
{
	PSBMSG_HEAD header; // C1:F6:50
	NEW_QUEST_WORLD_LIST Quest;
};
struct PMSG_NEW_QUEST_WORLD_SEND
{
	#pragma pack(1)
	PSBMSG_HEAD header; // C1:F6:55
	NEW_QUEST_WORLD_LIST Quest;
	#pragma pack()
};
struct PMSG_NEW_QUEST_COMPLETE_SEND
{
#pragma pack(1)
	PSBMSG_HEAD header; // C1:F6:51
	WORD UNK;
	BYTE state;
#pragma pack()
};
//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//
struct SDHP_NEW_QUEST_WORLD_RECV
{
	PSWMSG_HEAD header; // C1:10:00
	WORD index;
	char account[11];
	char name[11];
	//BYTE QuestWorldList[MAX_NEW_QUEST_WORLD_LIST];
	NEW_QUEST_WORLD_LIST Quest;
};
//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//
struct SDHP_NEW_QUEST_WORLD_SEND
{
	PSBMSG_HEAD header; // C1:10:40
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_NEW_QUEST_WORLD_SAVE_SEND
{
	PSWMSG_HEAD header; // C1:10:41
	WORD index;
	char account[11];
	char name[11];
	//BYTE QuestWorldList[MAX_NEW_QUEST_WORLD_LIST];
	NEW_QUEST_WORLD_LIST Quest;
};
//**********************************************//
//**********************************************//
//**********************************************//
struct ITEM_AWARD
{
	int RewardItemType;	//奖励项目类型
	int RewardItemIndex;	//奖励项目指数
	int RewardItemCount;	//奖励项目数量
};
enum QUEST_type
{
	COLLECT = 1,//收集
	KILL = 2,  //击杀
	LEVEL = 3, //等级
	TRANSFER = 4, //转职
	SEEK = 5, //寻找
};
struct ACCEPT_MISSION_INFO
{
	int TemplateType;//模板类型
	int QuestChapter;//任务章节
	int QuestCategory;//任务类别
	int QuestImportance;//任务重要性
	int QuestStartType;//任务任务开始类型
	int StartSubType;//开始子类型
	int LevelMin;//等级最小
	int QuestRepeatCycle;//任务重复周期
	int PrecedenceQuest;//优先任务
	int QuestStartItemType;//任务开始任务类型
	int QuestStartItemIndex;//任务开始物品索引
	int QuestCheckGens;//任务核对Gens
	int Class[MAX_CLASS];//
	int StartText;//
	int StartErrorText;//

};
struct TASK_PROGRESS_INFO
{
	int TemplateType;//模板类型
	int QuestType;	//任务类型
	int QMainType;//任务主类型
	int QSubType;//任务子类型
	int TagetNumber;//Taget号码
	int TargetMobMaxLv;//Target Mob Max Lv
	int DropRate;	//掉率
	int GateNumber;	//gate号码
	int MapNumber;	//地图编号
	int CoordinateX;	//坐标X
	int CoordinateY;	//坐标Y
	int RewardExp;	//奖励经验
	int RewardZen;	//奖励金币
	ITEM_AWARD ItemAward[3];
	int RewardType;//奖励类型
	int RewardSubType;	//奖励子类型
	int RewardNumber;	//奖励编号
	int ProgressText;	//
	int CompletetText;//完整文本

};
struct NEW_QUEST_INFO
{
	int Index;
	char Name[64];
	ACCEPT_MISSION_INFO Accept;
	TASK_PROGRESS_INFO TaskProgress;
};

class NewQuestWorld
{
public:
	NewQuestWorld(void);
	virtual ~NewQuestWorld(void);
	void Load(char* path);
	bool GetInfo(int index,NEW_QUEST_INFO* lpInfo);
	void GCNewQuestWorldLoad(LPOBJ lpObj);
	void GDNewQuestWorldSend(int aIndex);
	void DGNewQuestWorldRecv(SDHP_NEW_QUEST_WORLD_RECV* lpMsg);
	void GCNewQuestWorldInterfaceListSend(LPOBJ lpObj);
	void GDNewQuestWorldSaveSend(int aIndex);
	void CGNewQuestWorldGoNow(PMSG_NEW_QUEST_WORLD_RECV* lpMsg, int aIndex);
	void CGNewQuestWorldButtonMove(PMSG_NEW_QUEST_WORLD_RECV* lpMsg,int aIndex);
	void CGNewQuestWorldAward(PMSG_NEW_QUEST_WORLD_RECV* lpMsg,int aIndex);
	void GCUpdateNewQuestWorld(LPOBJ lpObj);
	//NEW_QUEST_INFO* NewQuestWorld::GetInfo(int index);

	BOOL GetTagetNumber(int Taget,int aIndex, int Type);
	void GCKillMonster(int aIndex, LPOBJ lpMonster);
	void NewQuestComplete(LPOBJ lpObj, int state);
	void CGNewQuestWorldGiveUpChapter(PMSG_NEW_QUEST_WORLD_RECV* lpMsg, int aIndex);
	void CGNewQuestWorldGiveUp(PMSG_NEW_QUEST_WORLD_RECV* lpMsg, int aIndex);
	int GetNewQuestItem(LPOBJ lpObj, int ItemIndex);
private:
	//NEW_QUEST_INFO m_NewQuestWorldInfo[MAX_NEW_QUEST_WORLD];
	std::map<int,NEW_QUEST_INFO> m_NewQuestWorldInfo;

};
extern NewQuestWorld gNewQuestWorld;
