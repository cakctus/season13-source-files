#pragma once

#include "User.h"

class CMemoryAllocatorInfo
{
public:
	CMemoryAllocatorInfo();
	void Clear();
	void Alloc();
public:
	int m_Index;
	int m_Active;
	DWORD m_ActiveTime;
	CSkill* m_SkillBackup;
	CSkill* m_Skill;
	VIEWPORT_STRUCT* m_VpPlayer;
	VIEWPORT_STRUCT* m_VpPlayer2;
	VIEWPORT_STRUCT* m_VpPlayerItem;
	HIT_DAMAGE_STRUCT* m_HitDamage;
	CEffect* m_Effect;
	PER_SOCKET_CONTEXT* m_PerSocketContext;
	CItem* m_Inventory1;
	CItem* m_Inventory2;
	BYTE* m_InventoryMap1;
	BYTE* m_InventoryMap2;
	CItem* m_Trade;
	BYTE* m_TradeMap;
	CItem* m_Warehouse;
	BYTE* m_WarehouseMap;
	CItem* m_ChaosBox;
	BYTE* m_ChaosBoxMap;
	CItem* m_EventInventory1;
	CItem* m_EventInventory2;
	BYTE* m_EventInventoryMap1;
	BYTE* m_EventInventoryMap2;
	CItem* m_MuunInventory;
	CItem *m_MuunInventory1;
	CItem *m_MuunInventory2;
	BYTE* m_MuunInventoryMap;
	CSkill* m_MasterSkill;
	QUEST_KILL_COUNT* m_QuestKillCount;
	QUEST_WORLD_LIST* m_QuestWorldList;
	GENS_SYSTEM_VICTIM_LIST* m_GensVictimList;
	DWORD* m_SkillDelay;
	DWORD* m_HackPacketDelay;
	DWORD* m_HackPacketCount;
	PENTAGRAM_JEWEL_INFO* m_PentagramJewelInfo_Inventory;
	PENTAGRAM_JEWEL_INFO* m_PentagramJewelInfo_Warehouse;
	CMuRummyInfo* m_MuRummyInfo;
	CBombHuntInfo *m_pCBombHuntInfo;
	struct NEW_QUEST_WORLD_LIST *m_NewQuestWorldList;
	CSkill* m_ForMasterSkill;
	LabyrinthInfo *m_LabyrinthInfo;
	_RESTORE_ITEMLIST * m_pRestoreInventory;
	_RESTORE_ITEMLIST * m_pRestoreInventory1;
};
