#include "stdafx.h"
#include "User.h"
#include "Protocol.h"
#include "Util.h"
#include "ForThTree.h"

#include "Logger.h"
#include "Party.h"
#include "Map.h"
#include "QuestReward.h"

ForThTree gFourthQuest;

ForThTree::ForThTree()
{
}


ForThTree::~ForThTree()
{
}

void ForThTree::MainProc()
{
	for (int i = stages.size() - 1; i >= 0; i--)
	{
		if(stages[i].CheckForLeftPlayers())
		{
			stages[i].ChangeStageByPlayers();
		}
		
		if (stages[i].PlayersCount() == 0)
		{
			RemoveStage(i);
		}
		else
		{
			stages[i].CheckForTimer();
		}
	}	
}

void ForThTree::CentTalk(LPOBJ lpNpc, LPOBJ lpObj)
{
	PMSG_NPC_CENT_TALK_SEND pMsg;
	pMsg.header.setE(0xF9, 0x01, sizeof(pMsg));
	pMsg.index = lpNpc->Class;
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}

void ForThTree::CGCentWarpToAreaOfAlegibility(int aIndex)
{
	if (!gObjIsConnected(aIndex)) return;
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_FOR_THTREE_ADMISSION_SEND pMsg;
	pMsg.header.set(0x3E, 0x20, sizeof(pMsg));

	if ((lpObj->Level + lpObj->MasterLevel) < 750)
	{
		pMsg.Type = 1;
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}
	gObjMoveGate(lpObj->Index, 537);
}

void ForThTree::CGCentClosePopupAndStartQuest(int aIndex)
{
	if (!gObjIsConnected(aIndex)) return;

	const int stageIndex = FindPlayerStageIndex(aIndex);
	if (stageIndex == -1) return;

	LPOBJ lpObj = &gObj[aIndex];
	auto stage = &stages[stageIndex];

	if(stage->IsStarted()) return;

	if(stage->CheckForPartyGotRewardsAndReadyForStageStart()) 
	{
		if(!stage->DoesPlayerHaveSameStage(lpObj))
		{
			GCMessagePopupSend(lpObj, "You should help your party members first, then complete on your quest stage");
		}
		
		stage->Start();
	}
	else
	{
		GCMessagePopupSend(lpObj, "Can't start quest till all players in party will accept it");
	}
}

void ForThTree::CGCentWarpToTestArea(int aIndex)
{
	if (!gObjIsConnected(aIndex)) return;

	LPOBJ lpObj = &gObj[aIndex];

	// Init monster count for player
	gQuestObjective.InitQuestObjectiveKillCount(lpObj, lpObj->QuestKillCountIndex);
	
	if (AddPlayerToStage(lpObj))
	{
		gObjMoveGate(lpObj->Index, 540);
		
		if(OBJECT_RANGE(lpObj->PartyNumber) != 0)
		{
			for (int n = 0; n < MAX_PARTY_USER; n++)
			{
				if (OBJECT_RANGE(gParty.m_PartyInfo[lpObj->PartyNumber].Index[n]) == 0)
				{
					continue;
				}
				LPOBJ PartylpObj = &gObj[gParty.m_PartyInfo[lpObj->PartyNumber].Index[n]];

				if (PartylpObj->Map != MAP_4_TH_QUEST && PartylpObj->Map == MAP_QUEST_ZONE_ENTRANCE)
				{
					CGCentWarpToTestArea(PartylpObj->Index);
				}
			}
		}
	}
	else
	{
		GCMessagePopupSend(lpObj, "Your party members already started the quest");
	}
}

void ForThTree::UpdateOnMonsterKilled(int monsterClass, LPOBJ lpObj)
{
	if (lpObj->Map != MAP_4_TH_QUEST) return;

	const int stageIndex = FindPlayerStageIndex(lpObj->Index);
	if (stageIndex == -1) return;

	stages[stageIndex].UpdateStageWithMonsterKilled(monsterClass);
}

bool ForThTree::ShouldViewMonster(LPOBJ lpObj, LPOBJ mObj)
{
	if (lpObj->Map != MAP_4_TH_QUEST || mObj->Map != MAP_4_TH_QUEST) return true;

	const int stageIndex = FindPlayerStageIndex(lpObj->Index);

	if (stageIndex == -1) return true;

	if (stages[stageIndex].SearchNpcOrMonster(mObj->Index)) return true;

	// Show Summons
	if (gObjIsConnected(mObj->SummonIndex))
	{
		if (ShouldViewPlayer(lpObj, &gObj[mObj->SummonIndex])) return true;
	}	
	
	return false;
}

bool ForThTree::ShouldViewPlayer(LPOBJ lpObj, LPOBJ tObj)
{
	if (lpObj->Map != MAP_4_TH_QUEST || tObj->Map != MAP_4_TH_QUEST)
		return true;

	const int firstPlayerStage = FindPlayerStageIndex(lpObj->Index);

	if (firstPlayerStage == -1)
		return true;

	const int secondPlayerStage = FindPlayerStageIndex(tObj->Index);

	if (secondPlayerStage == -1)
		return true;

	if (firstPlayerStage != -1 && firstPlayerStage == secondPlayerStage)
		return true;

	return false;
}

bool ForThTree::ShouldSearchTarget(LPOBJ mObj, LPOBJ tObj, BYTE objtype)
{
	if (mObj->Map != MAP_4_TH_QUEST || tObj->Map != MAP_4_TH_QUEST) return true;

	// Target of monster - Monster->Players
	if (objtype == OBJECT_USER && ShouldViewMonster(tObj, mObj)) return true;

	// Target of summon - Summon->Monsters
	if (objtype == OBJECT_MONSTER && gObjIsConnected(mObj->SummonIndex) && ShouldViewMonster(&gObj[mObj->SummonIndex], mObj)) return true;

	return false;
}

void ForThTree::CharacterDied(LPOBJ lpObj)
{
	if (lpObj->Map != MAP_4_TH_QUEST) return;
	
	RemovePlayerFromStage(lpObj);

	gObjMoveGate(lpObj->Index, 537);
}

int ForThTree::FindPlayerStageIndex(int index)
{
	for (int i = 0; i < stages.size(); i++)
	{
		if (stages[i].SearchPlayer(index)) return i;
	}

	return -1;
}

bool ForThTree::AddPlayerToStage(LPOBJ lpObj)
{
	int stageIndex = -1;
	
	if(OBJECT_RANGE(lpObj->PartyNumber) != 0)
	{
		for (int i=0; i<MAX_PARTY_USER; i++)
		{
			if (OBJECT_RANGE(gParty.m_PartyInfo[lpObj->PartyNumber].Index[i]) == 0)
			{
				continue;
			}
			
			const int partyPlayerIndex = gParty.m_PartyInfo[lpObj->PartyNumber].Index[i];
			stageIndex = FindPlayerStageIndex(partyPlayerIndex);
			if (stageIndex != -1) break;
		}
	}

	if(stageIndex == -1)
	{
		stageIndex = AddStage();
		stages[stageIndex].StartWelcomeStage();
	}
	else
	{
		if(stages[stageIndex].IsStarted())
		{
			return false;
		}
	}

	stages[stageIndex].AddPlayer(lpObj);
	stages[stageIndex].ChangeStageByPlayers();
	return true;
}

int ForThTree::AddStage()
{
	const FourthQuestStage stage;
	return stages.add(stage);
}

void ForThTree::RemovePlayerFromStage(LPOBJ lpObj)
{
	FourthQuestStage::EraseMonsterCount(lpObj);
	
	const int playerStageIndex = FindPlayerStageIndex(lpObj->Index);
	if (playerStageIndex == -1) return;

	stages[playerStageIndex].RemovePlayer(lpObj->Index);
	stages[playerStageIndex].ChangeStageByPlayers();
	if(stages[playerStageIndex].PlayersCount() == 0)
	{
		RemoveStage(playerStageIndex);
	}
}

void ForThTree::RemoveStage(int stageIndex)
{
	stages[stageIndex].RemoveStage();
	stages.remove_index(stageIndex);
}

