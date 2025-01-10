#pragma once
#include "DataServerProtocol.h"
#define MAX_NEW_QUEST_WORLD_LIST 4

struct NEW_QUEST_WORLD_LIST
{
	WORD QuestIndex;
	BYTE KillMonsterNum;
	BYTE QuestState;
};
//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//
struct SDHP_NEW_QUEST_WORLD_RECV
{
	PSBMSG_HEAD header; // C1:10:40
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_NEW_QUEST_WORLD_SAVE_RECV
{
	PSWMSG_HEAD header; // C1:10:30
	WORD index;
	char account[11];
	char name[11];
	//BYTE NewQuestWorldList[MAX_NEW_QUEST_WORLD_LIST];
	NEW_QUEST_WORLD_LIST Quest;
};
//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_NEW_QUEST_WORLD_SEND
{
	PSWMSG_HEAD header; // C1:10:00
	WORD index;
	char account[11];
	char name[11];
	//BYTE NewQuestWorldList[MAX_NEW_QUEST_WORLD_LIST];
	NEW_QUEST_WORLD_LIST Quest;
};
class NewQuestWorld
{
public:
	NewQuestWorld(void);
	virtual ~NewQuestWorld(void);
	void GDNewQuestWorldRecv(SDHP_NEW_QUEST_WORLD_RECV* lpMsg,int index);
	void GDNewQuestWorldSaveRecv(SDHP_NEW_QUEST_WORLD_SAVE_RECV* lpMsg);
};

extern NewQuestWorld gNewQuestWorld;