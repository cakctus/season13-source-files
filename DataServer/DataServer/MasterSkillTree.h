// MasterSkillTree.h: interface for the CMasterSkillTree class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "DataServerProtocol.h"

#define MAX_MASTER_SKILL_LIST 120

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_MASTER_SKILL_TREE_RECV
{
	PSBMSG_HEAD header; // C1:0D:00
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_MASTER_SKILL_TREE_SAVE_RECV
{
	PSWMSG_HEAD header; // C2:0D:30
	WORD index;
	char account[11];
	char name[11];
	DWORD MasterLevel;
	DWORD MasterPoint;
	QWORD MasterExperience;
	BYTE MasterSkill[MAX_MASTER_SKILL_LIST][3];
};

//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_MASTER_SKILL_TREE_SEND
{
	PSWMSG_HEAD header; // C2:0D:00
	WORD index;
	char account[11];
	char name[11];
	DWORD MasterLevel;
	DWORD MasterPoint;
	QWORD MasterExperience;
	BYTE MasterSkill[MAX_MASTER_SKILL_LIST][3];
};

//**********************************************//
//**********************************************//
//**********************************************//

class CMasterSkillTree
{
public:
	CMasterSkillTree();
	virtual ~CMasterSkillTree();
	void GDMasterSkillTreeRecv(SDHP_MASTER_SKILL_TREE_RECV* lpMsg,int index);
	void GDMasterSkillTreeSaveRecv(SDHP_MASTER_SKILL_TREE_SAVE_RECV* lpMsg);
};

extern CMasterSkillTree gMasterSkillTree;
