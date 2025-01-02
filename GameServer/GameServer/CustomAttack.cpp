#include "stdafx.h"
#include "CustomAttack.h"
#include "ItemManager.h"
#include "Map.h"
#include "MasterSkillTree.h"
#include "Notice.h"
#include "SkillManager.h"
#include "SocketManager.h"
#include "Viewport.h"
#include "FilaHit.h"
#include "ServerInfo.h"
#include "ObjectManager.h"
#include "Util.h"
#include "MemoryAllocator.h"
#include "GameMain.h"
#include "JSProtocol.h"
#include "MapServerManager.h"
#include "ESProtocol.h"
#include "CashShop.h"
#include "PersonalShop.h"
#include "EventInventory.h"
#include "MuunSystem.h"
#include "CNixiesLakeBossZone.h"
#include "MapManager.h"
#include "NpcTalk.h"

CCustomAttack gCustomAttack;

CCustomAttack::CCustomAttack() // OK
{
	this->OfflineRestored = false;
}

CCustomAttack::~CCustomAttack() // OK
{

}

void CCustomAttack::ReadCustomAttackInfo(char* section, char* path) // OK
{
	this->m_CustomAttackSwitch = GetPrivateProfileInt(section, "CustomAttackSwitch", 0, path);

	this->m_CustomAttackEnable[0] = GetPrivateProfileInt(section, "CustomAttackEnable_AL0", 0, path);
	this->m_CustomAttackEnable[1] = GetPrivateProfileInt(section, "CustomAttackEnable_AL1", 0, path);
	this->m_CustomAttackEnable[2] = GetPrivateProfileInt(section, "CustomAttackEnable_AL2", 0, path);
	this->m_CustomAttackEnable[3] = GetPrivateProfileInt(section, "CustomAttackEnable_AL3", 0, path);

	this->m_CustomAttackRequireLevel[0] = GetPrivateProfileInt(section, "CustomAttackRequireLevel_AL0", 0, path);
	this->m_CustomAttackRequireLevel[1] = GetPrivateProfileInt(section, "CustomAttackRequireLevel_AL1", 0, path);
	this->m_CustomAttackRequireLevel[2] = GetPrivateProfileInt(section, "CustomAttackRequireLevel_AL2", 0, path);
	this->m_CustomAttackRequireLevel[3] = GetPrivateProfileInt(section, "CustomAttackRequireLevel_AL3", 0, path);

	this->m_CustomAttackRequireReset[0] = GetPrivateProfileInt(section, "CustomAttackRequireReset_AL0", 0, path);
	this->m_CustomAttackRequireReset[1] = GetPrivateProfileInt(section, "CustomAttackRequireReset_AL1", 0, path);
	this->m_CustomAttackRequireReset[2] = GetPrivateProfileInt(section, "CustomAttackRequireReset_AL2", 0, path);
	this->m_CustomAttackRequireReset[3] = GetPrivateProfileInt(section, "CustomAttackRequireReset_AL3", 0, path);

	GetPrivateProfileString(section, "CustomAttackCommandSyntax", "", this->m_CustomAttackCommandSyntax, sizeof(this->m_CustomAttackCommandSyntax), path);

	GetPrivateProfileString(section, "CustomAttackText1", "", this->m_CustomAttackText1, sizeof(this->m_CustomAttackText1), path);
	GetPrivateProfileString(section, "CustomAttackText2", "", this->m_CustomAttackText2, sizeof(this->m_CustomAttackText2), path);
	GetPrivateProfileString(section, "CustomAttackText3", "", this->m_CustomAttackText3, sizeof(this->m_CustomAttackText3), path);
	GetPrivateProfileString(section, "CustomAttackText4", "", this->m_CustomAttackText4, sizeof(this->m_CustomAttackText4), path);
	GetPrivateProfileString(section, "CustomAttackText5", "", this->m_CustomAttackText5, sizeof(this->m_CustomAttackText5), path);
	GetPrivateProfileString(section, "CustomAttackText6", "", this->m_CustomAttackText6, sizeof(this->m_CustomAttackText6), path);

	this->m_CustomAttackOfflineSwitch = GetPrivateProfileInt(section, "CustomAttackOfflineSwitch", 0, path);

	this->m_CustomAttackOfflineGPGain = GetPrivateProfileInt(section, "CustomAttackOfflineGPGain", 0, path);

	this->m_CustomAttackOfflineEnable[0] = GetPrivateProfileInt(section, "CustomAttackOfflineEnable_AL0", 0, path);
	this->m_CustomAttackOfflineEnable[1] = GetPrivateProfileInt(section, "CustomAttackOfflineEnable_AL1", 0, path);
	this->m_CustomAttackOfflineEnable[2] = GetPrivateProfileInt(section, "CustomAttackOfflineEnable_AL2", 0, path);
	this->m_CustomAttackOfflineEnable[3] = GetPrivateProfileInt(section, "CustomAttackOfflineEnable_AL3", 0, path);

	this->m_CustomAttackOfflineRequireLevel[0] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireLevel_AL0", 0, path);
	this->m_CustomAttackOfflineRequireLevel[1] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireLevel_AL1", 0, path);
	this->m_CustomAttackOfflineRequireLevel[2] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireLevel_AL2", 0, path);
	this->m_CustomAttackOfflineRequireLevel[3] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireLevel_AL3", 0, path);

	this->m_CustomAttackOfflineRequireReset[0] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireReset_AL0", 0, path);
	this->m_CustomAttackOfflineRequireReset[1] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireReset_AL1", 0, path);
	this->m_CustomAttackOfflineRequireReset[2] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireReset_AL2", 0, path);
	this->m_CustomAttackOfflineRequireReset[3] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireReset_AL3", 0, path);

	this->m_CustomAttackOfflineRequireMoney[0] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireMoney_AL0", 0, path);
	this->m_CustomAttackOfflineRequireMoney[1] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireMoney_AL1", 0, path);
	this->m_CustomAttackOfflineRequireMoney[2] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireMoney_AL2", 0, path);
	this->m_CustomAttackOfflineRequireMoney[3] = GetPrivateProfileInt(section, "CustomAttackOfflineRequireMoney_AL3", 0, path);

	GetPrivateProfileString(section, "CustomAttackOfflineCommandSyntax", "", this->m_CustomAttackOfflineCommandSyntax, sizeof(this->m_CustomAttackOfflineCommandSyntax), path);

	GetPrivateProfileString(section, "CustomAttackOfflineText1", "", this->m_CustomAttackOfflineText1, sizeof(this->m_CustomAttackOfflineText1), path);
	GetPrivateProfileString(section, "CustomAttackOfflineText2", "", this->m_CustomAttackOfflineText2, sizeof(this->m_CustomAttackOfflineText2), path);
	GetPrivateProfileString(section, "CustomAttackOfflineText3", "", this->m_CustomAttackOfflineText3, sizeof(this->m_CustomAttackOfflineText3), path);
	GetPrivateProfileString(section, "CustomAttackOfflineText4", "", this->m_CustomAttackOfflineText4, sizeof(this->m_CustomAttackOfflineText4), path);
	GetPrivateProfileString(section, "CustomAttackOfflineText5", "", this->m_CustomAttackOfflineText5, sizeof(this->m_CustomAttackOfflineText5), path);
	GetPrivateProfileString(section, "CustomAttackOfflineText6", "", this->m_CustomAttackOfflineText6, sizeof(this->m_CustomAttackOfflineText6), path);
}

void CCustomAttack::CommandCustomAttack(LPOBJ lpObj, char* arg) // OK
{
	if (this->m_CustomAttackSwitch == 0)
	{
		return;
	}

	if (this->m_CustomAttackEnable[lpObj->AccountLevel] == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText1);
		return;
	}

	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < this->m_CustomAttackRequireLevel[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText2, this->m_CustomAttackRequireLevel[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Reset < this->m_CustomAttackRequireReset[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText3, this->m_CustomAttackRequireReset[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->AttackCustom != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText4);
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText5);
		return;
	}

	if (CA_MAP_RANGE(lpObj->Map) != 0 || DS_MAP_RANGE(lpObj->Map) != 0 || BC_MAP_RANGE(lpObj->Map) != 0 || CC_MAP_RANGE(lpObj->Map) != 0 || IT_MAP_RANGE(lpObj->Map) != 0 || DA_MAP_RANGE(lpObj->Map) != 0 || DG_MAP_RANGE(lpObj->Map) != 0 || IG_MAP_RANGE(lpObj->Map) != 0 || (lpObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpObj->X, lpObj->Y)))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText5);
		return;
	}

	if (gMapManager.GetMapHelperEnable(lpObj->Map) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText5);
		return;
	}

	int SkillNumber = SKILL_NONE;

	if (this->GetAttackSkill(lpObj, &SkillNumber) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText6);
		return;
	}

	lpObj->AttackCustom = 1;
	lpObj->AttackCustomSkill = SkillNumber;
	lpObj->AttackCustomDelay = GetTickCount();
	lpObj->AttackCustomZoneX = lpObj->X;
	lpObj->AttackCustomZoneY = lpObj->Y;
	lpObj->AttackCustomZoneMap = lpObj->Map;
	lpObj->AttackCustomOffline = 0;
	lpObj->AttackCustomOfflineTime = 0;
	lpObj->AttackCustomOfflineMoneyDelay = 0;
}

void CCustomAttack::CommandCustomAttackOffline(LPOBJ lpObj, char* arg) // OK
{
	if (this->m_CustomAttackOfflineSwitch == 0)
	{
		return;
	}

	if (this->m_CustomAttackOfflineEnable[lpObj->AccountLevel] == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackOfflineText1);
		return;
	}

	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < this->m_CustomAttackOfflineRequireLevel[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackOfflineText2, this->m_CustomAttackOfflineRequireLevel[lpObj->AccountLevel]);
		return;
	}

	if (((int)lpObj->Reset) < this->m_CustomAttackOfflineRequireReset[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackOfflineText3, this->m_CustomAttackOfflineRequireReset[lpObj->AccountLevel]);
		return;
	}

	if (((int)lpObj->Money) < this->m_CustomAttackOfflineRequireMoney[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackOfflineText4, this->m_CustomAttackOfflineRequireMoney[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->AttackCustom == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackOfflineText5);
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackOfflineText6);
		return;
	}

	if ((lpObj->Map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpObj->X, lpObj->Y)) || CA_MAP_RANGE(lpObj->Map) != 0 || DS_MAP_RANGE(lpObj->Map) != 0 || BC_MAP_RANGE(lpObj->Map) != 0 || CC_MAP_RANGE(lpObj->Map) != 0 || IT_MAP_RANGE(lpObj->Map) != 0 || DA_MAP_RANGE(lpObj->Map) != 0 || DG_MAP_RANGE(lpObj->Map) != 0 || IG_MAP_RANGE(lpObj->Map) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackOfflineText6);
		return;
	}

	//TODO send packet to stop reconnect before closing socket

	lpObj->AutoPotionBot = false;

	lpObj->Socket = INVALID_SOCKET;
	lpObj->AttackCustomOffline = 1;
	lpObj->AttackCustomOfflineTime = 0;
	lpObj->AttackCustomOfflineMoneyDelay = GetTickCount();
	lpObj->Money -= this->m_CustomAttackOfflineRequireMoney[lpObj->AccountLevel];

	gCustomAttack.GDUpdateData(lpObj->Index);

	closesocket(lpObj->PerSocketContext->Socket);
}

bool CCustomAttack::GetAttackSkill(LPOBJ lpObj, int* SkillNumber) // OK
{
	CSkill* lpSkill = 0;

	lpSkill = gSkillManager.GetSkill(lpObj, lpObj->MuHelperSkill);

	if (lpSkill == 0)
	{
		switch (lpObj->Class)
		{
		case CLASS_DW:
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_ICE_STORM) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_EVIL_SPIRIT) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_INFERNO) : lpSkill);
			break;
		case CLASS_DK:
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_POWER_SLASH) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_TWISTING_SLASH) : lpSkill);
			break;
		case CLASS_FE:
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_FIVE_SHOT) : lpSkill);
			break;
		case CLASS_MG:
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_FLAME_STRIKE) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_LIGHTNING_STORM) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_POWER_SLASH) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_EVIL_SPIRIT) : lpSkill);
			break;
		case CLASS_DL:
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_WIND_SOUL) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_BIRDS) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_FIRE_BURST) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_FORCE) : lpSkill);
			break;
		case CLASS_SU:
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_RED_STORM) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_CHAIN_LIGHTNING) : lpSkill);
			break;
		case CLASS_RF:
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_DARK_SIDE) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_DRAGON_LORE) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_PHOENIX_SHOT) : lpSkill);
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_KILLING_BLOW) : lpSkill);
		case CLASS_GL:
			lpSkill = ((lpSkill == 0) ? gSkillManager.GetSkill(lpObj, SKILL_SHINING_PEAK) : lpSkill);
			break;
		}
	}

	if (lpSkill == 0)
	{
		return 0;
	}
	else
	{
		(*SkillNumber) = lpSkill->m_index;
		return 1;
	}
}

bool CCustomAttack::GetTargetMonster(LPOBJ lpObj, int SkillNumber, int* MonsterIndex) // OK
{
	int NearestDistance = 100;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE || OBJECT_RANGE(lpObj->VpPlayer2[n].index) == 0 || lpObj->VpPlayer2[n].type != OBJECT_MONSTER)
		{
			continue;
		}

		if (gSkillManager.CheckSkillTarget(lpObj, lpObj->VpPlayer2[n].index, -1, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (gObjCalcDistance(lpObj, &gObj[lpObj->VpPlayer2[n].index]) >= NearestDistance)
		{
			continue;
		}

		if (gSkillManager.CheckSkillRange(SkillNumber, lpObj->X, lpObj->Y, gObj[lpObj->VpPlayer2[n].index].X, gObj[lpObj->VpPlayer2[n].index].Y) != 0)
		{
			(*MonsterIndex) = lpObj->VpPlayer2[n].index;
			NearestDistance = gObjCalcDistance(lpObj, &gObj[lpObj->VpPlayer2[n].index]);
			continue;
		}

		if (gSkillManager.CheckSkillRadio(SkillNumber, lpObj->X, lpObj->Y, gObj[lpObj->VpPlayer2[n].index].X, gObj[lpObj->VpPlayer2[n].index].Y) != 0)
		{
			(*MonsterIndex) = lpObj->VpPlayer2[n].index;
			NearestDistance = gObjCalcDistance(lpObj, &gObj[lpObj->VpPlayer2[n].index]);
			continue;
		}
	}

	return ((NearestDistance == 100) ? 0 : 1);
}

bool CCustomAttack::CheckRequireMoney(LPOBJ lpObj) // OK
{
	if (lpObj->AttackCustomOffline == 0)
	{
		return 1;
	}

	if ((GetTickCount() - ((DWORD)lpObj->AttackCustomOfflineMoneyDelay)) < 60000)
	{
		return 1;
	}

	if (((int)lpObj->Money) < this->m_CustomAttackOfflineRequireMoney[lpObj->AccountLevel])
	{
		return 0;
	}

	lpObj->Money -= this->m_CustomAttackOfflineRequireMoney[lpObj->AccountLevel];

	lpObj->AttackCustomOfflineMoneyDelay = GetTickCount();

	return 1;
}

void CCustomAttack::OnAttackClose(LPOBJ lpObj, int type) // OK
{
	if (lpObj->AttackCustom != 0)
	{
		lpObj->AttackCustom = 0;
		lpObj->AttackCustomSkill = 0;
		lpObj->AttackCustomDelay = 0;
		lpObj->AttackCustomZoneX = 0;
		lpObj->AttackCustomZoneY = 0;
		lpObj->AttackCustomZoneMap = 0;
		GCMessagePopupSend(lpObj, "Ataque automatico desligado.");
	}

	if (lpObj->AttackCustomOffline == 1)
	{
		lpObj->AttackCustomOffline = 2;
		lpObj->AttackCustomOfflineTime = 5;
		LogAdd(LOG_RED, "[%s][%s] Stopping Attack offline [Type: %d]", lpObj->Account, lpObj->Name, type);
	}
}

void CCustomAttack::OnAttackSecondProc(LPOBJ lpObj) // OK
{
	if (lpObj->isGhost)
	{
		lpObj->CheckSumTime = GetTickCount();
		lpObj->ConnectTickCount = GetTickCount();
	}

	if (lpObj->AttackCustomOffline != 0)
	{
		if (lpObj->AttackCustomOffline == 2)
		{
			if ((--lpObj->AttackCustomOfflineTime) == 0)
			{
				LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[lpObj->Index].Class);
				gObjDel(lpObj->Index);
				lpObj->AttackCustomOffline = 0;
				lpObj->AttackCustomOfflineTime = 0;
				lpObj->AttackCustomOfflineMoneyDelay = 0;
			}
		}

		lpObj->CheckSumTime = GetTickCount();
		lpObj->ConnectTickCount = GetTickCount();
		//lpObj->PcPointPointTime = ((this->m_CustomAttackOfflineGPGain == 0) ? GetTickCount() : lpObj->PcPointPointTime);
		//lpObj->CashShopGoblinPointTime = ((this->m_CustomAttackOfflineGPGain == 0) ? GetTickCount() : lpObj->CashShopGoblinPointTime);
	}
}

void CCustomAttack::OnAttackAlreadyConnected(LPOBJ lpObj) // OK
{
	if (lpObj->AttackCustomOffline != 0 || lpObj->isGhost)
	{
		lpObj->AttackCustomOffline = 0;
		lpObj->AttackCustomOfflineTime = 0;
		lpObj->AttackCustomOfflineMoneyDelay = 0;
		lpObj->isGhost = false;
		gCustomAttack.GDUpdateData(lpObj->Index);
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[lpObj->Index].Class);
		gObjDel(lpObj->Index);
	}
}

void CCustomAttack::OnAttackMonsterAndMsgProc(LPOBJ lpObj) // OK
{
	CSkill* lpSkill;

	if (lpObj->AttackCustom == 0)
	{
		return;
	}

	/*if (this->CheckRequireMoney(lpObj) == 0)
	{
		this->OnAttackClose(lpObj);
		return;
	}*/

	/*if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
	{
		this->OnAttackClose(lpObj);
		return;
	}*/

	if ((lpSkill = gSkillManager.GetSkill(lpObj, lpObj->AttackCustomSkill)) == 0)
	{
		this->OnAttackClose(lpObj, 1);
		return;
	}

	if (gViewport.CheckViewportObjectPosition(lpObj->Index, lpObj->AttackCustomZoneMap, lpObj->AttackCustomZoneX, lpObj->AttackCustomZoneY, 5) == 0)
	{
		this->OnAttackClose(lpObj, 2);
		return;
	}

	/*if (((lpObj->Life * 100) / (lpObj->MaxLife + lpObj->AddLife)) < 50)
	{
		PMSG_ITEM_USE_RECV pMsg;

		pMsg.header.set(0x26, sizeof(pMsg));
		pMsg.SourceSlot = 0xFF;
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 3), -1) : pMsg.SourceSlot);
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 2), -1) : pMsg.SourceSlot);
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 1), -1) : pMsg.SourceSlot);
		pMsg.TargetSlot = 0xFF;
		pMsg.type = 0;

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0) { gItemManager.CGItemUseRecv(&pMsg, lpObj->Index); }
	}*/

	/*if (((lpObj->Mana * 100) / (lpObj->MaxMana + lpObj->AddMana)) < 50)
	{
		PMSG_ITEM_USE_RECV pMsg;

		pMsg.header.set(0x26, sizeof(pMsg));
		pMsg.SourceSlot = 0xFF;
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 6), -1) : pMsg.SourceSlot);
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 4), -1) : pMsg.SourceSlot);
		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 3), -1) : pMsg.SourceSlot);
		pMsg.TargetSlot = 0xFF;
		pMsg.type = 0;

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0) { gItemManager.CGItemUseRecv(&pMsg, lpObj->Index); }
	}*/

	if ((GetTickCount() - ((DWORD)lpObj->AttackCustomDelay)) >= 80)
	{
		int MonsterIndex = -1;

		lpObj->AttackCustomDelay = GetTickCount();

		if (this->GetTargetMonster(lpObj, lpSkill->m_index, &MonsterIndex) != 0)
		{
			switch (gSkillManager.GetBaseSkill(lpSkill->m_skill))
			{
			case SKILL_EVIL_SPIRIT:
			case SKILL_INFERNO:
				this->SendMultiSkillAttack(lpObj, MonsterIndex, lpSkill->m_index);
				break;
			case SKILL_FORCE:
			case SKILL_KILLING_BLOW:
			case SKILL_PHOENIX_SHOT:
			case SKILL_SHINING_PEAK:
			case SKILL_FIRE_BURST:
			case SKILL_CHAIN_LIGHTNING:
				this->SendSkillAttack(lpObj, MonsterIndex, lpSkill->m_index);
				break;
			case SKILL_RED_STORM:
			case SKILL_FIVE_SHOT:
			case SKILL_FLAME_STRIKE:
			case SKILL_LIGHTNING_STORM:
			case SKILL_BIRDS:
			case SKILL_WIND_SOUL:
			case SKILL_DRAGON_LORE:
			case SKILL_TWISTING_SLASH:
			case SKILL_POWER_SLASH:
			case SKILL_ICE_STORM:
			case SKILL_DARK_SIDE:
			case SKILL_SPIRIT_HOOK:
			case SKILL_BRECHE:
				this->SendDurationSkillAttack(lpObj, MonsterIndex, lpSkill->m_index);
				break;
			}
		}
	}
}

void CCustomAttack::SendSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber) // OK
{
	PMSG_SKILL_ATTACK_RECV pMsg;

	pMsg.header.set(0x19, sizeof(pMsg));

	pMsg.skillH = SET_NUMBERHB(SkillNumber);
	pMsg.skillL = SET_NUMBERLB(SkillNumber);

	pMsg.indexH = SET_NUMBERHB(aIndex);
	pMsg.indexL = SET_NUMBERLB(aIndex);

	pMsg.dis = 0;

	//gSkillManager.CGSkillAttackRecv(&pMsg, lpObj->Index);
	g_FilaHit.Add(1, lpObj->Index, &pMsg.header.type);
}

void CCustomAttack::SendMultiSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber) // OK
{
	this->SendDurationSkillAttack(lpObj, aIndex, SkillNumber);

	/*
	BYTE send[256];

	PMSG_MULTI_SKILL_ATTACK_RECV pMsg;

	pMsg.header.set(PROTOCOL_CODE4, sizeof(pMsg));

	int size = sizeof(pMsg);

	pMsg.skillH = SET_NUMBERHB(SkillNumber);
	pMsg.skillL = SET_NUMBERLB(SkillNumber);

	pMsg.x = (BYTE)lpObj->X;
	pMsg.y = (BYTE)lpObj->Y;

	pMsg.serial = 0;

	pMsg.count = 0;

	PMSG_MULTI_SKILL_ATTACK info;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE || OBJECT_RANGE(lpObj->VpPlayer2[n].index) == 0 || lpObj->VpPlayer2[n].type != OBJECT_MONSTER)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (gSkillManager.CheckSkillTarget(lpObj, index, aIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (gSkillManager.CheckSkillRadio(SkillNumber, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		info.indexH = SET_NUMBERHB(index);
		info.indexL = SET_NUMBERLB(index);

		info.MagicKey = 0;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		if (CHECK_SKILL_ATTACK_COUNT(pMsg.count) == 0)
		{
			break;
		}
	}

	pMsg.header.size = size;

	memcpy(send, &pMsg, sizeof(pMsg));

	gSkillManager.CGMultiSkillAttackRecv((PMSG_MULTI_SKILL_ATTACK_RECV*)send, lpObj->Index, 0);*/

}

void CCustomAttack::SendDurationSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber) // OK
{
	PMSG_DURATION_SKILL_ATTACK_RECV pMsg;

	pMsg.header.set(0x1E, sizeof(pMsg));

	pMsg.skillH = SET_NUMBERHB(SkillNumber);
	pMsg.skillL = SET_NUMBERLB(SkillNumber);

	pMsg.x = (BYTE)gObj[aIndex].X;
	pMsg.y = (BYTE)gObj[aIndex].Y;

	pMsg.dir = (gSkillManager.GetSkillAngle(gObj[aIndex].X, gObj[aIndex].Y, lpObj->X, lpObj->Y) * 255) / 360;

	pMsg.dis = 0;

	pMsg.angle = (gSkillManager.GetSkillAngle(lpObj->X, lpObj->Y, gObj[aIndex].X, gObj[aIndex].Y) * 255) / 360;

	pMsg.indexH = SET_NUMBERHB(aIndex);
	pMsg.indexL = SET_NUMBERLB(aIndex);

	pMsg.MagicKey = 0;

	//gSkillManager.CGDurationSkillAttackRecv(&pMsg, lpObj->Index);
	g_FilaHit.Add(2, lpObj->Index, &pMsg.header.type);
}

void CCustomAttack::FilaDurationSkillAttack(LPOBJ lpObj, int aIndex, int SkillNumber) // OK
{
	PMSG_DURATION_SKILL_ATTACK_RECV pDuration;

	pDuration.header.setE(0x1E, sizeof(PMSG_DURATION_SKILL_ATTACK_RECV));

	pDuration.skillH = SET_NUMBERHB(SkillNumber);
	pDuration.skillL = SET_NUMBERLB(SkillNumber);

	pDuration.x = (BYTE)gObj[aIndex].X;
	pDuration.y = (BYTE)gObj[aIndex].Y;

	pDuration.dir = (gSkillManager.GetSkillAngle(gObj[aIndex].X, gObj[aIndex].Y, lpObj->X, lpObj->Y) * 255) / 360;
	pDuration.angle = (gSkillManager.GetSkillAngle(gObj[aIndex].X, gObj[aIndex].Y, lpObj->X, lpObj->Y) * 255) / 360;

	pDuration.indexH = SET_NUMBERHB(aIndex);
	pDuration.indexL = SET_NUMBERLB(aIndex);

	pDuration.MagicKey = 0;

	g_FilaHit.Add(2, lpObj->Index, (LPBYTE)&pDuration);
}

void CCustomAttack::FilaPvPTest() // OK
{
	/*if (!IsPvPTestOn)
	{
		return;
	}*/

	LPOBJ lpObj;

	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		if (gObj[i].Type == OBJECT_USER && gObj[i].Connected == OBJECT_ONLINE)
		{
			lpObj = &gObj[i];

			if (lpObj->AutoPVPTesting == false && !IsPvPTestOn)
			{
				continue;
			}

			if (!IsPvPTestOn && gViewport.CheckViewportObjectPosition(lpObj->Index, lpObj->AttackCustomZoneMap, lpObj->AttackCustomZoneX, lpObj->AttackCustomZoneY, 3) == 0)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Teste finalizado");
				lpObj->AutoPVPTesting = false;
				continue;
			}

			if (lpObj->TargetPvPIndex > -1)
			{
				switch (lpObj->Class)
				{
				case CLASS_DK:
				{
					if (lpObj->TargetPvPIndex == 99999)
					{
						if (gSkillManager.GetSkill(lpObj, SKILL_DEFENSE) != 0)
						{
							gSkillManager.SkillDefense(lpObj->Index, lpObj->Index, gSkillManager.GetSkill(lpObj, SKILL_DEFENSE));
						}
					}
					else
					{
						this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_FIRE_BLOW);
					}
				}
				break;

				case CLASS_MG:
					this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_FIRE_SLASH);
					//this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_TWISTING_SLASH);
				break;

				case CLASS_RF:
					this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_SPIRIT_HOOK);
					break;

				case CLASS_SU:
					this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_CHAIN_LIGHTNING);
					break;

				case CLASS_DW:
					this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_SOUL_SEEKER);
					break;

				case CLASS_FE:
					this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_TRIPLE_SHOT);
					break;

				case CLASS_GL:
					this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_SHINING_PEAK);
					break;

				case CLASS_DL:
					this->FilaDurationSkillAttack(lpObj, lpObj->TargetPvPIndex, SKILL_WIND_SOUL);
					break;
				}
			}
		}
	}
}

void CCustomAttack::UseAllPotionAuto()
{
	LPOBJ lpObj;

	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		if (gObj[i].Type == OBJECT_USER && gObj[i].Connected == OBJECT_ONLINE)
		{
			lpObj = &gObj[i];

			CItem* citem = &gObj[lpObj->Index].Inventory[lpObj->AutoPotionPos];

			if (gServerInfo.m_PotionMode == 2)
			{
				if ((GetTickCount() - lpObj->AutoPotionTime) > (int)(lpObj->Ping * 1.5))
				{
					lpObj->AutoPotion = false;
					continue;
				}

				if (gObjectManager.CharacterUsePotion(lpObj, citem))
				{
					gItemManager.DecreaseItemDur(lpObj, lpObj->AutoPotionPos, 1);
				}

				continue;
			}
			else if (gServerInfo.m_PotionMode == 1)
			{
				if ((GetTickCount() - lpObj->AutoPotionTime) > gServerInfo.m_PotionModeDelay)
				{
					lpObj->AutoPotion = false;
					continue;
				}

				if (gObjectManager.CharacterUsePotion(lpObj, citem))
				{
					gItemManager.DecreaseItemDur(lpObj, lpObj->AutoPotionPos, 1);
				}
			}
		}
	}
}

short CCustomAttack::AddIndexPlayer()
{
	if (gGameServerLogOut != 0)
	{
		return -1;
	}

	if (gGameServerDisconnect != 0)
	{
		return -1;
	}

	if (gObjTotalUser >= gServerInfo.m_ServerMaxUserNumber)
	{
		return -1;
	}

	int index = -1;
	int count = gObjCount;

	if (gMemoryAllocator.GetMemoryAllocatorFree(&index, OBJECT_START_USER, MAX_OBJECT, 10000) != 0)
	{
		return index;
	}

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObj[count].Connected == OBJECT_OFFLINE)
		{
			return count;
		}
		else
		{
			count = (((++count) >= MAX_OBJECT) ? OBJECT_START_USER : count);
		}
	}

	return -1;
}

void CCustomAttack::GDReqAllPlayer()
{
	if (gCustomAttack.OfflineRestored)
	{
		return;
	}

	LogAdd(LOG_BLUE, "Requesting offline players");
	PSBMSG_HEAD pMsg;
	pMsg.set(0xD7, 20, sizeof(pMsg));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.size);
}

void CCustomAttack::DGOfflineAccountRecv(PMSG_OFFMODE_START_RECV* Data)
{
	gCustomAttack.OfflineRestored = true;

	short aIndex = this->AddIndexPlayer();

	if (aIndex == -1)
	{
		return;
	}

	if (gObj[aIndex].Connected == OBJECT_PLAYING)
	{
		return;
	}

	if (gObjAdd(INVALID_SOCKET, "127.0.0.1", aIndex) == -1)
	{
		return;
	}

	if (!OBJECT_RANGE(aIndex))
	{
		return;
	}

	char id[11] = { 0 };
	memcpy(id, Data->AccountID, sizeof(Data->AccountID));
	memcpy(gObj[aIndex].Account, id, sizeof(Data->AccountID));
	memcpy(gObj[aIndex].Name, Data->Name, sizeof(Data->Name));

	char psw[21] = { 0 };
	memcpy(psw, Data->Password, sizeof(Data->Password));
	memcpy(gObj[aIndex].Password, psw, sizeof(psw));
	gObj[aIndex].Password[20] = '\0';

	for (int i = MAX_OBJECT_MONSTER; i < MAX_OBJECT; i++)
	{
		LPOBJ sObj = &gObj[i];

		if (sObj->Connected != OBJECT_PLAYING)
		{
			continue;
		}

		if (id[0] == sObj->Account[0] && psw[0] == sObj->Password[0])
		{
			if (!strncmp(id, sObj->Account, 10) && !strncmp(psw, sObj->Password, 20))
			{
				LogAdd(LOG_RED, "[Offline] Delete object [%d][%s][%s][%s][%s]", aIndex, id, sObj->Account, psw, sObj->Password);
				gObjDel(aIndex);
				return;
			}
		}
	}

	LPOBJ lpObj = &gObj[aIndex];

	GJDisconnectAccountSend(0, Data->AccountID, "127.0.0.1");

	if (gObj[aIndex].Connected != OBJECT_CONNECTED)
	{
		LogAdd(LOG_RED, "[%s] Offline Account Received not connected", Data->AccountID);
		CloseClient(aIndex);
		return;
	}

	if (gObj[aIndex].LoginMessageSend != 0)
	{
		if (gObj[aIndex].LoginMessageSend == 1)
		{
			LogAdd(LOG_BLUE, "[%d][%s] Offline connection ", aIndex, id);
		}
		else
		{
			LogAdd(LOG_RED, "Offline connection error : %d %s %d", gObj[aIndex].LoginMessageSend, __FILE__, __LINE__);
		}
		return;
	}

	if (lpObj->Connected >= OBJECT_LOGGED)
	{
		return;
	}

	//LogAdd(LOG_BLUE, "Offline Auth user [%s][%s]", Data->AccountID, Data->Password);

	lpObj->ConnectTickCount = GetTickCount();
	lpObj->ClientTickCount = Data->TickCount;
	lpObj->ServerTickCount = GetTickCount();
	lpObj->MapServerMoveRequest = 0;
	lpObj->LastServerCode = -1;
	lpObj->DestMap = -1;
	lpObj->DestX = 0;
	lpObj->DestY = 0;
	lpObj->LoginMessageSend = 1;
	lpObj->LoginMessageCount = 1;

	lpObj->OfflineMode = true;

	GJConnectAccountSend(aIndex, Data->AccountID, Data->Password, lpObj->IpAddr);
}

void CCustomAttack::GDReqOffCharacter(int aIndex)
{
	if (!OBJECT_RANGE(aIndex))
	{
		return;
	}

	LPOBJ lpUser = &gObj[aIndex];

	lpUser->OfflineMode = false;

	PMSG_OFFMODE_CHAR pMsg = { 0 };
	pMsg.h.set(0xD7, 21, sizeof(pMsg));
	memcpy(pMsg.AccountID, lpUser->Account, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, lpUser->Name, sizeof(pMsg.Name));
	pMsg.Number = aIndex;
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
}

void CCustomAttack::DGAnsOffCharacter(SDHP_CHARACTER_INFO_RECV* lpMsg)
{
	short aIndex = lpMsg->index;

	if (!OBJECT_RANGE(aIndex))
	{
		return;
	}

	char szAccountID[11] = { 0 };
	char szName[11] = { 0 };

	memcpy(szAccountID, lpMsg->account, sizeof(lpMsg->account));
	memcpy(szName, lpMsg->name, 11);

	if (gObjIsAccountValid(aIndex, szAccountID) == FALSE)
	{
		LogAdd(LOG_RED, "[Offline] Acc not connected [%s]", szAccountID);
		CloseClient(aIndex);
		return;
	}

	if (lpMsg->result == false)
	{
		LogAdd(LOG_RED, "[Offline] Result false [%s][%s]", gObj[aIndex].Account, szName);
		CloseClient(aIndex);
		return;
	}

	if ((lpMsg->CtlCode & 1) != 0)
	{
		LogAdd(LOG_RED, "[Offline] Blocked Character [%s][%s]", gObj[aIndex].Account, szName);
		CloseClient(aIndex);
		return;
	}

	/*for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		if (gObj[i].Connected == OBJECT_PLAYING)
		{
			if (gObj[i].Type == OBJECT_USER)
			{
				if (!strncmp(szName, gObj[i].Name, 11) || !strncmp(szAccountID, gObj[i].Account, 11))
				{
					LogAdd(LOG_BLACK,"Attempted Character-Copy by double logging [%s][%s]", gObj[aIndex].Account, szName);
					CloseClient(aIndex);
					return;
				}
			}
		}
	}*/

	gObj[aIndex].Connected = OBJECT_LOGGED;

	if (gObjectManager.CharacterInfoSet((BYTE*)lpMsg, aIndex) == 0)
	{
		LogAdd(LOG_RED, "[Offline] CharacterInfoSet error [%s][%s]", gObj[aIndex].Account, szName);
		CloseClient(aIndex);
		return;
	}

	gObj[aIndex].CheckSumTime = 0;

	gObj[aIndex].Shield = gObj[aIndex].MaxShield + gObj[aIndex].m_MPSkillOpt.AddShield;

	LPOBJ lpObj = &gObj[aIndex];

	if (gObj[aIndex].Index != aIndex)
	{
		LogAdd(LOG_RED, "[Offline] Wrong index [%s][%s][%d]", gObj[aIndex].Account, szName, aIndex);
		CloseClient(aIndex);
		return;
	}

	if (lpObj->MapServerMoveRequest == 0)
	{
		if ((lpObj->NextServerCode = gMapServerManager.CheckMapServerMove(lpObj->Index, lpObj->Map, lpObj->LastServerCode)) != gServerInfo.m_ServerCode)
		{
			if (lpObj->NextServerCode == -1)
			{
				CloseClient(lpObj->Index);
				return;
			}
			else
			{
				GJMapServerMoveSend(lpObj->Index, lpObj->NextServerCode, lpObj->Map, (BYTE)lpObj->X, (BYTE)lpObj->Y);
				return;
			}
		}
	}

	lpObj->MapServerMoveRequest = 0;

	GDConnectCharacterSend(lpObj->Index);
	GDOptionDataSend(lpObj->Index);
	DGGuildMemberInfoRequest(lpObj->Index);
	GDPetItemInfoSend(lpObj->Index, 0);
	gHelper.GDHelperDataSend(lpObj->Index);
	gCashShop.GDCashShopPeriodicItemSend(lpObj->Index);
	gPentagramSystem.GDPentagramJewelInfoSend(lpObj->Index, PENTAGRAM_JEWEL_TYPE_INVENTORY);
	gPentagramSystem.GDPentagramJewelInfoSend(lpObj->Index, PENTAGRAM_JEWEL_TYPE_WAREHOUSE);
	gPersonalShop.GDPShopItemValueSend(lpObj->Index);
	gEventInventory.GDEventInventorySend(lpObj->Index);
	gMuunSystem.GDMuunInventorySend(lpObj->Index);

	LogAdd(LOG_BLACK, "[ObjectManager][%d] AddCharacterInfo (%s)", lpObj->Index, lpObj->Name);
}

void CCustomAttack::DGPSelectData(PMSG_OFFMODE_DATA* lpMsg)
{
	short aIndex = lpMsg->Number;

	if (!OBJECT_RANGE(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	//activate attack
	if (lpMsg->Attack)
	{
		int SkillNumber = SKILL_NONE;

		if (this->GetAttackSkill(lpObj, &SkillNumber) == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, this->m_CustomAttackText6);
			return;
		}

		lpObj->AttackCustom = 1;
		lpObj->AttackCustomSkill = SkillNumber;
		lpObj->AttackCustomDelay = GetTickCount();
		lpObj->AttackCustomZoneX = lpObj->X;
		lpObj->AttackCustomZoneY = lpObj->Y;
		lpObj->AttackCustomZoneMap = lpObj->Map;

		lpObj->AttackCustomOffline = 1;
		lpObj->AttackCustomOfflineTime = 0;
		lpObj->AttackCustomOfflineMoneyDelay = GetTickCount();
	}

	if (lpMsg->Ghost)
	{
		lpObj->isGhost = true;
	}

	if (lpMsg->PShopOpen)
	{
		memcpy(lpObj->PShopText, lpMsg->PShopText, sizeof(lpMsg->PShopText));
		lpObj->PShopOpen = true;
		lpObj->PShopCustomOffline = true;
		lpObj->PShopCustomOfflineTime = 0;
	}
}

void CCustomAttack::GDUpdateData(int aIndex)
{
	if (!OBJECT_RANGE(aIndex))
	{
		return;
	}

	LPOBJ lpUser = &gObj[aIndex];

	PMSG_OFFMODE_DATA_SAVE pMsg = { 0 };
	pMsg.h.set(0xD7, 22, sizeof(pMsg));
	memcpy(pMsg.Name, lpUser->Name, sizeof(pMsg.Name));
	pMsg.Status = (lpUser->AttackCustomOffline || lpUser->PShopCustomOffline || lpUser->isGhost);
	memcpy(pMsg.PShopText, lpUser->PShopText, sizeof(pMsg.PShopText));
	pMsg.PShopOpen = lpUser->PShopOpen;
	pMsg.Attack = lpUser->AttackCustomOffline;
	pMsg.Ghost = lpUser->isGhost;

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
}