#pragma once
#include "DataServerProtocol.h"
#define MAX_FOR_MASTER_SKILL_LIST 150
//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_FOR_MASTER_SKILL_TREE_RECV
{
	PSBMSG_HEAD header; // C1:7E:00
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_FOR_MASTER_SKILL_TREE_SAVE_RECV
{
	PSWMSG_HEAD header; // C2:7E:01
	WORD index;
	char account[11];
	char name[11];
	DWORD ForMasterPoint;
	BYTE ForMasterSkill[MAX_FOR_MASTER_SKILL_LIST][4];
};
//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_FOR_MASTER_SKILL_TREE_SEND
{
	PSWMSG_HEAD header; // C2:7E:00
	WORD index;
	char account[11];
	char name[11];
	DWORD ForMasterPoint;
	BYTE ForMasterSkill[MAX_FOR_MASTER_SKILL_LIST][4];
};

class MasterSkillTree_4th
{
public:
	MasterSkillTree_4th();
	~MasterSkillTree_4th();
	void GDMasterSkillTreeRecv(SDHP_FOR_MASTER_SKILL_TREE_RECV* lpMsg, int index); // OK
	void GDMasterSkillTreeSaveRecv(SDHP_FOR_MASTER_SKILL_TREE_SAVE_RECV* lpMsg); // OK
};
extern MasterSkillTree_4th gMasterSkillTree_4th;
