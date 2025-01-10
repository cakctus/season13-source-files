#pragma once

#include "array.h"
#include "Logger.h"
#include "Map.h"
#include "Monster.h"
#include "Quest.h"

#define MAX_FOR_QUEST2_TIME (1*60*1000)

struct PMSG_NPC_CENT_TALK_SEND
{
	PSBMSG_HEAD header; // C1:F9:01
	WORD index;
	int unk;
};

struct PMSG_FOR_THTREE_ADMISSION_SEND
{
	PSBMSG_HEAD header; // C1:3E:20
	BYTE Type;
};

class FourthQuestStage
{
	enum StageIndex
	{
		NOT_READY,		// low level
		FIRST_TIME,		// not started
		STAGE_1,		// eligibility test - kill npc	[7]
		STAGE_2,		// new battle 1 - kill mobs		[8]
		STAGE_3,		// new battle 2 - kill npc, but stronger now	[9]
		FINISHED
	};

	StageIndex stage;
	array<int> playerIndexes;
	array<int> monsterIndexes;
	int npcIndex;
	bool started;
	DWORD timeToFinishQuest;
	
	static StageIndex CurrentStageOfPlayer(LPOBJ lpObj)
	{
		if (lpObj->Level + lpObj->MasterLevel < 800) return NOT_READY;
		if (lpObj->Quest[7].questState != QUEST_FINISH) return STAGE_1;
		if (lpObj->Quest[8].questState != QUEST_FINISH) return STAGE_2;
		if (lpObj->Quest[9].questState != QUEST_FINISH) return STAGE_3;
		if (lpObj->Quest[9].questState == QUEST_FINISH) return FINISHED;

		return FIRST_TIME;
	}

	static int CreateMonster(int MonsterClass, int phase)
	{
		int index = gObjAddMonster(MAP_4_TH_QUEST);

		if (OBJECT_RANGE(index) == 0)
		{
			return -1;
		}

		LPOBJ MonsterlpObj = &gObj[index];

		int px, py;
		bool RespawnAfterDie = false;
		if (MonsterClass == 766 || MonsterClass == 763)
		{
			px = 147;
			py = 30;
		}
		else
		{
			px = 143 + (GetLargeRand() % 7);
			py = 30 - (GetLargeRand() % 3);
			RespawnAfterDie = true;
		}

		MonsterlpObj->X = px;
		MonsterlpObj->Y = py;
		MonsterlpObj->TX = px;
		MonsterlpObj->TY = py;
		MonsterlpObj->OldX = px;
		MonsterlpObj->OldY = py;
		MonsterlpObj->StartX = px;
		MonsterlpObj->StartY = py;
		MonsterlpObj->Dir = 2;
		MonsterlpObj->Map = MAP_4_TH_QUEST;

		if (gObjSetMonster(index, MonsterClass, false, RespawnAfterDie) == 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[index].Class);
			gObjDel(index);
			return -1;
		}

		if (phase == 2)
		{
			//make the reinforcement
			MonsterlpObj->PhysiDamageMin *= 1.5;
			MonsterlpObj->PhysiDamageMax *= 1.5;
			MonsterlpObj->Defense *= 1.5;
			MonsterlpObj->MagicDefense *= 1.5;
			MonsterlpObj->m_MPSkillOpt.AttackSuccessRate *= 1.5;
			MonsterlpObj->m_MPSkillOpt.DefenseSuccessRate *= 1.5;
			MonsterlpObj->Life *= 1.5;
			MonsterlpObj->MaxLife *= 1.5;
		}
		
		if (MonsterClass != 766 && MonsterClass != 763)
		{
			MonsterlpObj->MaxRegenTime = 1000;
		}
		
		return index;
	}

	void StartCountdown()
	{
		for (int i=0; i<playerIndexes.size(); i++)
		{
			if (gObjIsConnected(playerIndexes[i]))
			{
				Countdown(playerIndexes[i], 6, 0, MAX_FOR_QUEST2_TIME);
				timeToFinishQuest = GetTickCount() + MAX_FOR_QUEST2_TIME;
			}
		}
	}
	
	void StopCountdown()
	{
		for (int i=0; i<playerIndexes.size(); i++)
		{
			if (gObjIsConnected(playerIndexes[i]))
			{
				Countdown(playerIndexes[i], 6, 0, -1);
			}
		}
	}

	bool DoesAllPlayersCompletedSecondStage()
	{
		for (int i=0; i<playerIndexes.size(); i++)
		{
			if (gObjIsConnected(playerIndexes[i]))
			{
				LPOBJ lpObj = &gObj[playerIndexes[i]];

				if(lpObj->Map != MAP_4_TH_QUEST) continue;
				
				if(/*lpObj->QuestKillCountIndex == 8 && */gQuestObjective.CheckQuestObjective(lpObj, 8) == false)
				{
					return false;
				}
			}
		}
		return true;
	}

	void StartNeededStage()
	{
		if(stage == FINISHED) return;
		
		started = true;
		RemoveNpc();

		switch (stage)
		{
		case FIRST_TIME:
		case STAGE_1:
			stage = STAGE_1;
			monsterIndexes.add(CreateMonster(763, 1));
			break;
		case STAGE_2:
			monsterIndexes.add(CreateMonster(767, 0));
			monsterIndexes.add(CreateMonster(767, 0));
			monsterIndexes.add(CreateMonster(771, 0));
			monsterIndexes.add(CreateMonster(771, 0));
			monsterIndexes.add(CreateMonster(770, 0));
			monsterIndexes.add(CreateMonster(770, 0));
			StartCountdown();
			break;
		case STAGE_3: 
			monsterIndexes.add(CreateMonster(763, 2));
			monsterIndexes.add(CreateMonster(767, 2));
			monsterIndexes.add(CreateMonster(767, 2));
			monsterIndexes.add(CreateMonster(767, 2));
			monsterIndexes.add(CreateMonster(771, 2));
			monsterIndexes.add(CreateMonster(771, 2));
			monsterIndexes.add(CreateMonster(771, 2));
			monsterIndexes.add(CreateMonster(770, 2));
			monsterIndexes.add(CreateMonster(770, 2));
			monsterIndexes.add(CreateMonster(770, 2));
			break;
		}
	}

	void CompleteStage()
	{
		started = false;
		RemoveMonsters();
		SpawnNpc();
		
		switch (stage)
		{
		case STAGE_1:
			stage = STAGE_2;
			break;
		case STAGE_2:
			stage = STAGE_3;
			break;
		case STAGE_3:
			stage = FINISHED;
			break;
		}
	}

	void FailSecondStage()
	{
		started = false;
		RemoveMonsters();
		SpawnNpc();
		StopCountdown();
		
		for (int i=0; i<playerIndexes.size(); i++)
		{
			if (gObjIsConnected(playerIndexes[i]))
			{
				LPOBJ lpObj = &gObj[playerIndexes[i]];

				if(lpObj->Map != MAP_4_TH_QUEST) continue;
				
				EraseMonsterCount(lpObj);

				GCMessagePopupSend(lpObj, "You did not have time to kill all the monsters.\nBut not afraid, you can try once again!");
			}
		}
	}

	void SpawnNpc()
	{
		npcIndex = CreateMonster(766, 0);
	}

	void RemoveNpc()
	{
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[npcIndex].Class);
		gObjDel(npcIndex);
		npcIndex = -1;
	}
	
	void RemoveMonsters()
	{
		for (int i = 0; i < monsterIndexes.size(); i++)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[monsterIndexes[i]].Class);
			gObjDel(monsterIndexes[i]);
		}
		monsterIndexes.clear();
	}
	
public:
	FourthQuestStage()
	{
		stage = FIRST_TIME;
		playerIndexes.clear();
		monsterIndexes.clear();
		npcIndex = -1;
		started = false;
	}

	void StartWelcomeStage()
	{
		started = false;
		SpawnNpc();
	}

	void Start()
	{
		StartNeededStage();
	}

	void UpdateStageWithMonsterKilled(int monsterClass)
	{
		switch (stage)
		{
		case STAGE_1:
		case STAGE_3: 
			if(monsterClass == 763)
			{
				CompleteStage();
			}			
			break;
		case STAGE_2:
			if(DoesAllPlayersCompletedSecondStage())
			{
				StopCountdown();
				CompleteStage();
			}
			break;
		default: ;
		}
	}

	bool SearchNpcOrMonster(const int index)
	{
		if (npcIndex == index) return true;

		for (auto i = 0; i < monsterIndexes.size(); i++)
		{
			if (index == monsterIndexes[i]) return true;
		}

		return false;
	}

	void RemoveStage()
	{
		RemoveNpc();
		RemoveMonsters();
	}

	bool SearchPlayer(const int index)
	{
		for (auto i = 0; i < playerIndexes.size(); i++)
		{
			if (playerIndexes[i] == index) return true;
		}

		return false;
	}

	void AddPlayer(LPOBJ lpObj) { playerIndexes.add(lpObj->Index); }
	
	bool CheckForPartyGotRewardsAndReadyForStageStart()
	{
		int questNumber = stage == STAGE_1 || stage == FIRST_TIME ? 7 : stage == STAGE_2 ? 8 : 9;
		
		for (int i=0; i<playerIndexes.size(); i++)
		{
			if (gObjIsConnected(playerIndexes[i]))
			{
				LPOBJ lpObj = &gObj[playerIndexes[i]];

				if(lpObj->Map != MAP_4_TH_QUEST) continue;

				if(CurrentStageOfPlayer(lpObj) == NOT_READY) continue;			// Helper not in count
				
				if(lpObj->Quest[questNumber].questState != QUEST_ACCEPT && lpObj->Quest[questNumber].questState != QUEST_FINISH)
				{
					return false;
				}
			}
		}
		return true;
	}

	void ChangeStageByPlayers()
	{
		if (started) return;

		StageIndex minimumStage = STAGE_3;
		bool allNotReady = true;

		for (auto i = 0; i < playerIndexes.size(); i++)
		{
			auto index = playerIndexes[i];
			if (!gObjIsConnected(index)) continue;

			auto playerStage = CurrentStageOfPlayer(&gObj[index]);
			if (playerStage != NOT_READY && playerStage <= minimumStage)
			{
				minimumStage = playerStage;
				allNotReady = false;
			}
		}

		if (allNotReady)
		{
			stage = NOT_READY;
		}
		else
		{
			stage = minimumStage;
		}
	}

	void CheckForTimer()
	{
		if(started && stage == STAGE_2)
		{
			if(GetTickCount() > timeToFinishQuest)
			{
				FailSecondStage();
			}
		}
	}

	bool CheckForLeftPlayers()
	{
		bool changed = false;
		for (auto i = playerIndexes.size() - 1; i >= 0; i--)
		{
			const auto index = playerIndexes[i];
			if (!gObjIsConnected(index)) 
			{
				playerIndexes.remove_index(i);
				changed = true;
			}
			else if (gObj[index].Map != MAP_4_TH_QUEST)
			{
				playerIndexes.remove_index(i);
				EraseMonsterCount(&gObj[index]);
				changed = true;
			}
		}
		return changed;
	}

	void RemovePlayer(int index)
	{
		for (auto i = playerIndexes.size() - 1; i >= 0; i--)
		{
			if(playerIndexes[i] != index) continue;
			
			if (gObjIsConnected(index))
			{
				EraseMonsterCount(&gObj[index]);
			}				
			playerIndexes.remove_index(i);
			return;
		}
	}

	static void EraseMonsterCount(LPOBJ lpObj)
	{
		if (lpObj->QuestKillCountIndex == 7 || lpObj->QuestKillCountIndex == 8 || lpObj->QuestKillCountIndex == 9)
		{
			for (int i = 0; i < MAX_QUEST_KILL_COUNT; i++)
			{
				lpObj->QuestKillCount[i].Clear();
			}
		}
		gQuest.GCQuestKillCountSend(lpObj->Index, lpObj->QuestKillCountIndex);
	}

	bool DoesPlayerHaveSameStage(LPOBJ lpObj) const { return CurrentStageOfPlayer(lpObj) == stage; }

	bool IsStarted() const { return started; }

	int PlayersCount() const { return playerIndexes.size(); }
};

class ForThTree
{
public:
	ForThTree();
	virtual ~ForThTree();
	void MainProc();
	void CentTalk(LPOBJ lpNpc, LPOBJ lpObj);
	void CGCentWarpToAreaOfAlegibility(int aIndex);
	void CGCentClosePopupAndStartQuest(int aIndex);
	void CGCentWarpToTestArea(int aIndex);
	
	void UpdateOnMonsterKilled(int monsterClass, LPOBJ lpObj);

	bool ShouldViewMonster(LPOBJ lpObj, LPOBJ mObj);
	bool ShouldViewPlayer(LPOBJ lpObj, LPOBJ tObj);
	bool ShouldSearchTarget(LPOBJ mObj, LPOBJ tObj, BYTE objtype);
	void CharacterDied(LPOBJ lpObj);
private:
	int FindPlayerStageIndex(int Index);

	bool AddPlayerToStage(LPOBJ lpObj);
	int AddStage();
	void RemovePlayerFromStage(LPOBJ lpObj);
	void RemoveStage(int stageIndex);

	array<FourthQuestStage> stages;
};

extern ForThTree gFourthQuest;