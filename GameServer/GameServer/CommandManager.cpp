#include "stdafx.h"
#include "BonusManager.h"
#include "CashShop.h"
#include "CastleSiege.h"
#include "CommandManager.h"
#include "CustomAttack.h"
#include "CustomStore.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "GameMaster.h"
#include "Guild.h"
#include "GuildClass.h"
#include "ItemManager.h"
#include "Log.h"
#include "MapServerManager.h"
#include "MasterSkillTree.h"
#include "MasterSkillTree_4th.h"
#include "MemScript.h"
#include "Message.h"
#include "Monster.h"
#include "Move.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Protocol.h"
#include "Quest.h"
#include "QuestReward.h"
#include "RheaGold.h"
#include "ResetTable.h"
#include "ServerInfo.h"
#include "SXIDecryp.h"
#include "Util.h"
#include "EventInventory.h"
#include "ItemBagManager.h"
#include "FilaHit.h"
#include "SocketManager.h"
#include "MUFC.h"
#include "Attack.h"
#include "Wings4thOption.h"
#include "MasteryItemExtOptionSet.h"

CCommandManager gCommandManager;

CCommandManager::CCommandManager() // OK
{
}

CCommandManager::~CCommandManager() // OK
{
}

void CCommandManager::LoadCommands(char* path) // OK
{
	CMemScript* lpScript = new(std::nothrow) CMemScript;

	if (lpScript == NULL)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpScript->GetLastError());
		delete lpScript;
		return;
	}

	m_CommandInfo.clear();

	try
	{
		while (true)
		{
			if (lpScript->GetToken() == 2)
			{
				break;
			}

			int CommandIndex;
			char szCommandName[30];
			int CommandRights;

			CommandIndex = lpScript->GetNumber();
			if (CommandIndex < 1)
			{
				ErrorMessageBox("Error loading file Commands.txt");
				break;
			}
			memcpy(szCommandName, lpScript->GetAsString(), sizeof(szCommandName));
			CommandRights = lpScript->GetAsNumber();

			this->Add(szCommandName, CommandIndex, CommandRights);
		}
	}
	catch (...)
	{
		delete lpScript;
		ErrorMessageBox(0, path, "Error", 0);
		return;
	}

	delete lpScript;

	LogAdd(LOG_GREEN, "[Commands] %s is loaded", path);
}

void CCommandManager::MainProc() // OK
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) == 0)
		{
			continue;
		}

		if (gObj[n].AutoResetEnable != 0)
		{
			this->CommandResetAutoProc(&gObj[n]);
		}
	}

	//MUFC COMMAND
	if (gMUFC.ReSetTimer > 0)
	{
		gMUFC.ReSetTimer--;

		if (gMUFC.ReSetTimer == 0)
		{
			gMUFC.SetNewRound();
		}
	}

	if (gMUFC.Status == MUFC_PREPARED)
	{
		if (gMUFC.StartCounter > 0)
		{
			gMUFC.SendNotice("[MUFC]   Prepare-se! Iniciando luta em (%d)   [MUFC]", gMUFC.StartCounter);
			gMUFC.StartCounter--;
		}
		else if (gMUFC.StartCounter == 0)
		{
			gMUFC.FightRun();
		}
	}
	else if (gMUFC.Status == MUFC_STARTED)
	{
		gMUFC.FightTimer++;

		for (int n = 0; n < 32; n++)
		{
			if (gMUFC.FightDamageRate[n].Timer < 0)
				break;

			if (gMUFC.FightTimer == gMUFC.FightDamageRate[n].Timer)
			{
				gMUFC.FightDamage = gMUFC.FightDamageRate[n].DamageRate;
				gMUFC.SendNotice("[MUFC]  Dano aumentado em %d porcento  [MUFC]", gMUFC.FightDamage);
				break;
			}
		}
	}
}

void CCommandManager::Add(char* label, int code, int level) // OK
{
	m_CommandInfo.add(COMMAND_INFO(code, label, level));
}

long CCommandManager::GetNumber(char* arg, int pos, int defaultNumber) // OK
{
	int count = 0, p = 0;

	char buffer[60] = { 0 };

	int len = strlen(arg);

	len = ((len >= sizeof(buffer)) ? (sizeof(buffer) - 1) : len);

	for (int n = 0; n < len; n++)
	{
		if (arg[n] == 0x20)
		{
			count++;
		}
		else if (count == pos)
		{
			buffer[p] = arg[n];
			p++;
		}
	}

	if (p > 0)
	{
		return atoi(buffer);
	}


	return defaultNumber;
}

void CCommandManager::GetString(char* arg, char* out, int size, int pos) // OK
{
	int count = 0, p = 0;

	char buffer[60] = { 0 };

	int len = strlen(arg);

	len = ((len >= sizeof(buffer)) ? (sizeof(buffer) - 1) : len);

	for (int n = 0; n < len; n++)
	{
		if (arg[n] == 0x20)
		{
			count++;
		}
		else if (count == pos)
		{
			buffer[p] = arg[n];
			p++;
		}
	}

	memcpy(out, buffer, (size - 1));
}

long CCommandManager::GetCommandCode(char* label) // OK
{
	for (int n = 0; n < m_CommandInfo.size(); n++)
	{
		if (_stricmp(label, this->m_CommandInfo[n].label) == 0)
		{
			return this->m_CommandInfo[n].code;
		}
	}

	return -1;
}

long CCommandManager::GetCommandLevel(char* label) // OK
{
	for (int n = 0; n < m_CommandInfo.size(); n++)
	{
		if (_stricmp(label, this->m_CommandInfo[n].label) == 0)
		{
			return this->m_CommandInfo[n].level;
		}
	}

	return -1;
}

bool CCommandManager::ManagementCore(LPOBJ lpObj, char* message) // OK
{
	char command[32] = { 0 };

	memset(command, 0, sizeof(command));

	this->GetString(message, command, sizeof(command), 0);

	int code = this->GetCommandCode(command);
	int level = this->GetCommandLevel(command);

	char* argument = &message[strlen(command)];

	if (argument[0] == 0x20)
	{
		argument++;
	}

	if (level > 0 && gGameMaster.CheckGameMasterLevel(lpObj, level) == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return false;
	}

	switch (code)
	{
	case COMMAND_MOVE:
		this->CommandMove(lpObj, argument);
		break;
	case COMMAND_POST:
		this->CommandPost(lpObj, argument);
		break;
	case COMMAND_ADD_POINT_STRENGTH:
		this->CommandAddPoint(lpObj, argument, 0);
		break;
	case COMMAND_ADD_POINT_AGILITY:
		this->CommandAddPoint(lpObj, argument, 1);
		break;
	case COMMAND_ADD_POINT_VITALITY:
		this->CommandAddPoint(lpObj, argument, 2);
		break;
	case COMMAND_ADD_POINT_ENERGY:
		this->CommandAddPoint(lpObj, argument, 3);
		break;
	case COMMAND_ADD_POINT_LEADERSHIP:
		this->CommandAddPoint(lpObj, argument, 4);
		break;
	case COMMAND_PK_CLEAR:
		this->CommandPKClear(lpObj, argument);
		break;
	case COMMAND_CHANGE:
		this->CommandEvo(lpObj, argument);
		break;
	case COMMAND_WARE:
		this->CommandWare(lpObj, argument);
		break;
	case COMMAND_RESET:
		this->CommandReset(lpObj, argument);
		break;
	case COMMAND_GM_MOVE:
		this->CommandGMMove(lpObj, argument);
		break;
	case COMMAND_MOVE_GUILD:
		this->CommandMoveGuild(lpObj, argument);
		break;
	case COMMAND_GUILD_DISCONNECT:
		this->CommandGuildDisconnect(lpObj, argument);
		break;
	case COMMAND_TRACK:
		this->CommandTrack(lpObj, argument);
		break;
	case COMMAND_TRACE:
		this->CommandTrace(lpObj, argument);
		break;
	case COMMAND_DISCONNECT:
		this->CommandDisconnect(lpObj, argument);
		break;
	case COMMAND_FIREWORKS:
		this->CommandFireworks(lpObj, argument);
		break;
	case COMMAND_MAKE:
		this->CommandMake(lpObj, argument);
		break;
	case COMMAND_MAKE_NO_SERIAL:
		this->CommandMake(lpObj, argument, false);
		break;
	case COMMAND_SKIN:
		this->CommandSkin(lpObj, argument);
		break;
	case COMMAND_GM_MONEY:
		this->CommandSetMoney(lpObj, argument);
		break;
	case COMMAND_MASTER_RESET:
		this->CommandMasterReset(lpObj, argument);
		break;
	case COMMAND_GUILD_WAR:
		this->CommandGuildWar(lpObj, argument);
		break;
	case COMMAND_BATTLE_SOCCER:
		this->CommandBattleSoccer(lpObj, argument);
		break;
	case COMMAND_REQUEST:
		this->CommandRequest(lpObj, argument);
		break;
	case COMMAND_CUSTOM_STORE_OFFLINE:
		gCustomStore.CommandCustomStoreOffline(lpObj, argument);
		break;
	case COMMAND_HIDE:
		this->CommandHide(lpObj, argument);
		break;
	case COMMAND_CUSTOM_ATTACK:
		gCustomAttack.CommandCustomAttack(lpObj, argument);
		break;
	case COMMAND_CUSTOM_ATTACK_OFFLINE:
		gCustomAttack.CommandCustomAttackOffline(lpObj, argument);
		break;
	case COMMAND_CLEAR_INVENTORY:
		this->CommandClearInventory(lpObj, argument);
		break;
	case COMMAND_SPAWN:
		this->CommandSpawn(lpObj, argument);
		break;
	case COMMAND_REBUILD:
		this->CommandRebuild(lpObj, argument);
		break;
	case COMMAND_PVP_TEST:
		this->CommandPvpTest(lpObj, argument);
		break;
	case COMMAND_SET_BUFFS:
		this->CommandSetBuffs(lpObj, argument);
		break;
	case COMMAND_SET_TARGET:
		this->CommandSetTarget(lpObj, argument);
		break;
	case COMMAND_SHOW_STATS:
		this->CommandShowStats(lpObj, argument);
		break;
	case COMMAND_SET_STATS:
		this->CommandSetStats(lpObj, argument);
		break;
	case COMMAND_SET_AUTOPOTION:
		this->CommandSetAutoPotion(lpObj, argument);
		break;
	case COMMAND_WATCH:
		this->CommandWatch(lpObj, argument);
		break;
	case COMMAND_CHAT_BLOCK:
		this->CommandChatBlock(lpObj, argument);
		break;
	case COMMAND_GHOST:
		this->CommandGhost(lpObj, argument);
		break;
	case COMMAND_TRACE_PARTY:
		this->CommandBringParty(lpObj, argument);
		break;
	case COMMAND_AUTODT:
		this->CommandAutoDt(lpObj, argument);
		break;
	case COMMAND_SETPK:
		this->CommandSetPk(lpObj, argument);
		break;
	case COMMAND_MUFC:
		this->CommandMUFC(lpObj, argument);
		break;
	case COMMAND_SIEGE:
		this->CommandSiege(lpObj, argument);
		break;
	case COMMAND_FILL_POTION:
		this->CommandFillPotion(lpObj);
		break;
	case COMMAND_SEND_POPUP:
		this->CommandSendPopup(lpObj, argument);
		break;
	case COMMAND_PIN:
		this->CommandPin(lpObj, argument);
		break;
	case COMMAND_RUUD:
		this->CommandRuud(lpObj, argument);
		break;
	case COMMAND_ANTILAG:
		this->CommandAntilag(lpObj, argument);
		break;
	case COMMAND_SET_LEVEL:
		this->CommandSetLevel(lpObj, argument);
		break;
	case COMMAND_SET_MLEVEL:
		this->CommandSetMLevel(lpObj, argument);
		break;
	case COMMAND_SET_RESET:
		this->CommandSetReset(lpObj, argument);
		break;
	case COMMAND_REMOVE_WING_OPTION:
		this->CommandRemoveWingOption(lpObj, argument);
	case COMMAND_AUTO_PVP:
		this->CommandAutoPvP(lpObj, argument);
		break;
	case COMMAND_SAVEDT:
		this->CommandSaveBuild(lpObj, argument);
		break;
	case COMMAND_LOADDT:
		this->CommandLoadBuild(lpObj, argument);
		break;
	case COMMAND_SUMIU:
		this->CommandSumiu(lpObj, argument);
		break;
	case COMMAND_REBUILD_TREE:
		this->CommandRebuildSkillTree(lpObj, argument);
		break;
	default:
		return 0;
	}

	return 1;
}

void CCommandManager::CommandMove(LPOBJ lpObj, char* arg) // OK
{
	if (gEffectManager.CheckStunEffect(lpObj) != 0 || gEffectManager.CheckImmobilizeEffect(lpObj) != 0)
	{
		return;
	}

	MOVE_INFO MoveInfo;

	if (gMove.GetInfoByName(arg, &MoveInfo) != 0)
	{
		gMove.Move(lpObj, MoveInfo.Index);
		return;
	}
}

void CCommandManager::CommandPost(LPOBJ lpObj, char* arg) // OK
{
	if (lpObj->ChatLimitTime > 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(269), lpObj->ChatLimitTime);
		return;
	}

	if (gServerInfo.m_CommandPostEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Level < gServerInfo.m_CommandPostLevel[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(65), gServerInfo.m_CommandPostLevel[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Reset < gServerInfo.m_CommandPostReset[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(66), gServerInfo.m_CommandPostReset[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandPostMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(67), gServerInfo.m_CommandPostMoney[lpObj->AccountLevel]);
		return;
	}

	DWORD tick = (GetTickCount() - lpObj->PostTime) / 1000;

	if (tick < ((DWORD)gServerInfo.m_CommandPostDelay[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(68), (gServerInfo.m_CommandPostDelay[lpObj->AccountLevel] - tick));
		return;
	}

	lpObj->PostTime = GetTickCount();

	lpObj->Money -= gServerInfo.m_CommandPostMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	if (gServerInfo.m_CommandPostGlobal[lpObj->AccountLevel] == 0)
	{
		PostMessage(lpObj->Name, gMessage.GetMessage(69), arg, gServerInfo.m_CommandPostType[lpObj->AccountLevel]);
	}
	else
	{
		GDGlobalPostSend(gMapServerManager.GetMapServerGroup(), gServerInfo.m_CommandPostType[lpObj->AccountLevel], lpObj->Name, arg);
	}

	gLog.Output(LOG_COMMAND, "[CommandPost][%s][%s] - (Message: %s)", lpObj->Account, lpObj->Name, arg);
}

void CCommandManager::CommandAddPoint(LPOBJ lpObj, char* arg, int type) // OK
{
	if (gServerInfo.m_CommandAddPointEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Class != CLASS_DL && type == 4)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(71));
		return;
	}

	int amount = this->GetNumber(arg, 0);

	if (amount <= 0 || lpObj->LevelUpPoint < amount)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(72));
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandAddPointMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(73), gServerInfo.m_CommandAddPointMoney[lpObj->AccountLevel]);
		return;
	}

	if (gObjectManager.CharacterLevelUpPointAdd(lpObj, type, amount) == 0)
	{
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandAddPointMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	GCNewCharacterInfoSend(lpObj);

	SXGetCharacterInfo(lpObj->Index);

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(74), amount, lpObj->LevelUpPoint);

	gLog.Output(LOG_COMMAND, "[CommandAddPoint][%s][%s] - (Type: %d, Amount: %d)", lpObj->Account, lpObj->Name, type, amount);
}

void CCommandManager::CommandPKClear(LPOBJ lpObj, char* arg) // OK
{
	/*int skill = this->GetNumber(arg, 0);
	int x = this->GetNumber(arg, 1);
	int y = this->GetNumber(arg, 2);
	int dir = this->GetNumber(arg, 3);

	gSkillManager.GCDurationSkillAttackSend(lpObj, skill, x, y, dir);
	return;*/

	if (gServerInfo.m_CommandPKClearEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->PKLevel <= 3)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(76));
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandPKClearMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(77), gServerInfo.m_CommandPKClearMoney[lpObj->AccountLevel]);
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandPKClearMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->PKLevel = 3;

	GCPKLevelSend(lpObj->Index, lpObj->PKLevel);

	GCMessagePopupSend(lpObj, gMessage.GetMessage(78));

	gLog.Output(LOG_COMMAND, "[CommandPKClear][%s][%s] - (PKLevel: %d)", lpObj->Account, lpObj->Name, lpObj->PKLevel);
}

void CCommandManager::CommandChange(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandChangeEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(82), gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel]);
		return;
	}

	int ChangeUp = lpObj->ChangeUp;

	if (lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		ChangeUp++;
	}

	if (ChangeUp >= gServerInfo.m_CommandChangeLimit[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(83));
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	for (int n = 0; n <= ((ChangeUp == 0) ? 3 : 6); n++)
	{
		if (gQuest.CheckQuestListState(lpObj, n, QUEST_FINISH) == 0)
		{
			if (n != 3 || lpObj->Class == CLASS_DK)
			{
				gQuest.AddQuestList(lpObj, n, QUEST_ACCEPT);
				gQuestReward.InsertQuestReward(lpObj, n);
				gQuest.AddQuestList(lpObj, n, QUEST_FINISH);
			}
		}
	}

	lpObj->SendQuestInfo = 0;

	gQuest.GCQuestInfoSend(lpObj->Index);

	GCMessagePopupSend(lpObj, gMessage.GetMessage(84));

	gLog.Output(LOG_COMMAND, "[CommandChange][%s][%s] - (ChangeUp: %d)", lpObj->Account, lpObj->Name, ChangeUp);
}

void CCommandManager::CommandWare(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandWareEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->LoadWarehouse != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(86));
		return;
	}

	int number = this->GetNumber(arg, 0);

	if (number < 0 || number >= gServerInfo.m_CommandWareNumber[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(87));
		return;
	}

	lpObj->WarehouseMoney = 0;

	memset(lpObj->WarehouseMap, 0xFF, WAREHOUSE_SIZE);

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		lpObj->Warehouse[n].Clear();
	}

	lpObj->Interface.use = 0;

	lpObj->Interface.type = INTERFACE_NONE;

	lpObj->Interface.state = 0;

	lpObj->LoadWarehouse = 0;

	lpObj->WarehouseNumber = number;

	GCMessagePopupSend(lpObj, gMessage.GetMessage(88), number);

	gLog.Output(LOG_COMMAND, "[CommandWare][%s][%s] - (Number: %d)", lpObj->Account, lpObj->Name, number);
}

void CCommandManager::CommandReset(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandResetEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	char mode[5] = { 0 };

	this->GetString(arg, mode, sizeof(mode), 0);

	if (strcmp(mode, "auto") == 0)
	{
		this->CommandResetAuto(lpObj, arg);
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(90));
		return;
	}

	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < gResetTable.GetResetLevel(lpObj))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(91), gResetTable.GetResetLevel(lpObj));
		return;
	}

	if (lpObj->Money < ((DWORD)gResetTable.GetResetMoney(lpObj)))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(92), gResetTable.GetResetMoney(lpObj));
		return;
	}

	if (lpObj->Reset >= gServerInfo.m_CommandResetLimit[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(93), gServerInfo.m_CommandResetLimit[lpObj->AccountLevel]);
		return;
	}

	if (gServerInfo.m_CommandResetCheckItem[lpObj->AccountLevel] != 0)
	{
		for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
		{
			if (lpObj->Inventory[n].IsItem() != 0)
			{
				GCMessagePopupSend(lpObj, gMessage.GetMessage(94));
				return;
			}
		}
	}

	if (((lpObj->CommandManagerTransaction[0] == 0) ? (lpObj->CommandManagerTransaction[0]++) : lpObj->CommandManagerTransaction[0]) != 0)
	{
		return;
	}

	SDHP_COMMAND_RESET_SEND pMsg;
	pMsg.header.set(0x0F, 0x00, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CCommandManager::CommandResetAuto(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandResetAutoEnable[lpObj->AccountLevel] == 0)
	{
		lpObj->AutoResetEnable = 0;
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->AutoResetEnable == 0)
	{
		lpObj->AutoResetEnable = 1;
		GCMessagePopupSend(lpObj, gMessage.GetMessage(116));
	}
	else
	{
		lpObj->AutoResetEnable = 0;
		GCMessagePopupSend(lpObj, gMessage.GetMessage(117));
	}
}

void CCommandManager::CommandGMMove(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	int map = this->GetNumber(arg, 1);

	int x = this->GetNumber(arg, 2);

	int y = this->GetNumber(arg, 3);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gObjTeleport(lpTarget->Index, map, x, y);

	gLog.Output(LOG_COMMAND, "[CommandGMMove][%s][%s] - (Name: %s, Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, name, map, x, y);
}

void CCommandManager::CommandGMPost(LPOBJ lpObj, int type, char* arg) // OK
{
	if (lpObj->Authority != 32)
	{
		return;
	}

	if (type == 0 || type == 1)
	{
		gNotice.SendMapServerGroupMsg(0, "[%s] %s", lpObj->Name, arg);
	}
	else if (type == 2)
	{
		gNotice.GCPopupSendToAll(arg);
	}
	else
	{
		GDGlobalPopupSend(gMapServerManager.GetMapServerGroup(), arg);
	}

	gLog.Output(LOG_COMMAND, "[CommandGMPost][%s][%s] - (Type: %d, Message: %s)", lpObj->Account, lpObj->Name, type, arg);
}

void CCommandManager::CommandSendPopup(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	GCMessagePopupSend(lpTarget, &arg[strlen(name)]);

	gLog.Output(LOG_COMMAND, "[CommandSendPopup][%s][%s] - (Message: %s)", lpObj->Account, lpObj->Name, &arg[strlen(name)]);
}

void CCommandManager::CommandTrack(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gObjTeleport(lpObj->Index, lpTarget->Map, lpTarget->X, lpTarget->Y);

	gLog.Output(LOG_COMMAND, "[CommandTrack][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
}

void CCommandManager::CommandTrace(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gObjTeleport(lpTarget->Index, lpObj->Map, lpObj->X, lpObj->Y);

	gLog.Output(LOG_COMMAND, "[CommandTrace][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
}

void CCommandManager::CommandDisconnect(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	if (lpTarget->AttackCustomOffline != 0)
	{
		LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[lpTarget->Index].Class);
		gObjDel(lpTarget->Index);
		lpTarget->AttackCustomOffline = 0;
		lpTarget->AttackCustomOfflineTime = 0;
		lpTarget->AttackCustomOfflineMoneyDelay = 0;
		lpTarget->isGhost = false;
	}
	else
	{
		gObjUserKill(lpTarget->Index);
	}

	gLog.Output(LOG_COMMAND, "[CommandDisconnect][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
}

void CCommandManager::CommandFireworks(LPOBJ lpObj, char* arg) // OK
{
	for (int n = 0; n < 15; n++)
	{
		GCFireworksSend(lpObj, (lpObj->X + (((GetLargeRand() % 5) * 2) - 4)), (lpObj->Y + (((GetLargeRand() % 5) * 2) - 4)));
	}

	gLog.Output(LOG_COMMAND, "[CommandFireworks][%s][%s] - (Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, lpObj->Map, lpObj->X, lpObj->Y);
}

void CCommandManager::CommandMake(LPOBJ lpObj, char* arg, bool serial) // OK
{
	int section = this->GetNumber(arg, 0);
	int type = this->GetNumber(arg, 1);
	int level = this->GetNumber(arg, 2, 0);
	int skill = this->GetNumber(arg, 3, 0);
	int luck = this->GetNumber(arg, 4, 0);
	int option = this->GetNumber(arg, 5, 0);
	int exc = this->GetNumber(arg, 6, 0);
	int set = this->GetNumber(arg, 7, 0);

	BYTE socket[5];
	BYTE socketArg = this->GetNumber(arg, 8, 255);
	memcpy(&socket[0], &socketArg, sizeof(socket[0]));
	socketArg = this->GetNumber(arg, 9, 255);
	memcpy(&socket[1], &socketArg, sizeof(socket[1]));
	socketArg = this->GetNumber(arg, 10, 255);
	memcpy(&socket[2], &socketArg, sizeof(socket[2]));
	socketArg = this->GetNumber(arg, 11, 255);
	memcpy(&socket[3], &socketArg, sizeof(socket[3]));
	socketArg = this->GetNumber(arg, 12, 255);
	memcpy(&socket[4], &socketArg, sizeof(socket[4]));

	int socketBonus = this->GetNumber(arg, 13, 255);

	if (serial)
	{
		GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, GET_ITEM(section, type), level, 0, skill, luck, option, -1, exc, set, 0, 0, socket, socketBonus, 0);
		gLog.Output(LOG_COMMAND, "[CommandMakeSerial][%s][%s] - (Section: %d, Type: %d, TimerRandom: %d, Skill: %d, Skill: %d, Option: %d, Exc: %d, Set: %d)", lpObj->Account, lpObj->Name, section, type, level, skill, luck, option, exc, set);
	}
	else
	{
		gLog.Output(LOG_COMMAND, "[CommandMakeNoSerial][%s][%s] - (Section: %d, Type: %d, TimerRandom: %d, Skill: %d, Skill: %d, Option: %d, Exc: %d, Set: %d)", lpObj->Account, lpObj->Name, section, type, level, skill, luck, option, exc, set);
		gMap[lpObj->Map].MonsterItemDrop(GET_ITEM(section, type), level, 0, lpObj->X, lpObj->Y, skill, luck, option, exc, set, lpObj->Index, 0, 0, 0, socket, socketBonus, 0);
	}

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(107));	
}

void CCommandManager::CommandSkin(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	lpTarget->Change = this->GetNumber(arg, 1);

	gObjViewportListProtocolCreate(lpTarget);

	gObjectManager.CharacterUpdateMapEffect(lpTarget);

	gLog.Output(LOG_COMMAND, "[CommandSkin][%s][%s] - (Name: %s, Change: %d)", lpObj->Account, lpObj->Name, name, lpTarget->Change);
}

void CCommandManager::CommandSetMoney(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };
	char type[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);
	this->GetString(arg, type, sizeof(name), 1);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int amount = this->GetNumber(arg, 2);

	if (amount <= 0)
	{
		return;
	}

	if (strcmp(type, gServerInfo.m_CommandCoinSyntaxZen) == 0)
	{
		if (gGameMaster.CheckGameMasterLevel(lpObj, gServerInfo.m_CommandCoinLevelZen) == 0)
		{
			GCMessagePopupSend(lpObj, "Acesso negado.");
			return;
		}

		if (gObjCheckMaxMoney(lpTarget->Index, amount) == 0)
		{
			lpTarget->Money = MAX_MONEY;
		}
		else
		{
			lpTarget->Money += amount;
		}

		GCMoneySend(lpTarget->Index, lpTarget->Money);

		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(32));
	}
	else if (strcmp(type, gServerInfo.m_CommandCoinSyntaxRuud) == 0)
	{
		if (gGameMaster.CheckGameMasterLevel(lpObj, gServerInfo.m_CommandCoinLevelRuud) == 0)
		{
			GCMessagePopupSend(lpObj, "Acesso negado.");
			return;
		}

		lpTarget->Ruud += amount;
		gRheaGold.GC_Notify_RuudUpdate(lpTarget->Index, lpTarget->Ruud, amount, 1);

		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(33));
	}
	else if (strcmp(type, gServerInfo.m_CommandCoinSyntaxGP) == 0)
	{
		if (gGameMaster.CheckGameMasterLevel(lpObj, gServerInfo.m_CommandCoinLevelGP) == 0)
		{
			GCMessagePopupSend(lpObj, "Acesso negado.");
			return;
		}

		gCashShop.GDCashShopAddPointSaveSend(lpTarget->Index, 0, 0, 0, amount, "CommandSetMoney");
		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(29));
	}
	else if (strcmp(type, gServerInfo.m_CommandCoinSyntaxWP) == 0)
	{
		if (gGameMaster.CheckGameMasterLevel(lpObj, gServerInfo.m_CommandCoinLevelWP) == 0)
		{
			GCMessagePopupSend(lpObj, "Acesso negado.");
			return;
		}

		gCashShop.GDCashShopAddPointSaveSend(lpTarget->Index, 0, 0, amount, 0, "CommandSetMoney");
		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(30));
	}
	else if (strcmp(type, gServerInfo.m_CommandCoinSyntaxWC) == 0)
	{
		if (gGameMaster.CheckGameMasterLevel(lpObj, gServerInfo.m_CommandCoinLevelWC) == 0)
		{
			GCMessagePopupSend(lpObj, "Acesso negado.");
			return;
		}

		gCashShop.GDCashShopAddPointSaveSend(lpTarget->Index, 0, amount, 0, 0, "CommandSetMoney");
		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(31));
	}

	gLog.Output(LOG_COMMAND, "[CommandCoin][%s][%s] - (Name: %s, Type: %s, Amount: %d)", lpObj->Account, lpObj->Name, name, type, amount);
}

void CCommandManager::CommandMasterReset(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandMasterResetEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(119));
		return;
	}

	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < gServerInfo.m_CommandMasterResetLevel[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(120), gServerInfo.m_CommandMasterResetLevel[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Reset < gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(121), gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(122), gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->MasterReset >= gServerInfo.m_CommandMasterResetLimit[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(123), gServerInfo.m_CommandMasterResetLimit[lpObj->AccountLevel]);
		return;
	}

	if (gServerInfo.m_CommandMasterResetCheckItem[lpObj->AccountLevel] != 0)
	{
		for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
		{
			if (lpObj->Inventory[n].IsItem() != 0)
			{
				GCMessagePopupSend(lpObj, gMessage.GetMessage(124));
				return;
			}
		}
	}

	if (((lpObj->CommandManagerTransaction[0] == 0) ? (lpObj->CommandManagerTransaction[0]++) : lpObj->CommandManagerTransaction[0]) != 0)
	{
		return;
	}

	SDHP_COMMAND_MASTER_RESET_SEND pMsg;

	pMsg.header.set(0x0F, 0x01, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CCommandManager::CommandGuildWar(LPOBJ lpObj, char* arg) // OK
{
	char GuildName[9] = { 0 };

	this->GetString(arg, GuildName, sizeof(GuildName), 0);

	if (strlen(GuildName) >= 1)
	{
		GCGuildWarRequestResult(GuildName, lpObj->Index, 0);
	}
}

void CCommandManager::CommandBattleSoccer(LPOBJ lpObj, char* arg) // OK
{
	char GuildName[9] = { 0 };

	this->GetString(arg, GuildName, sizeof(GuildName), 0);

	if (strlen(GuildName) >= 1)
	{
		GCGuildWarRequestResult(GuildName, lpObj->Index, 1);
	}
}

void CCommandManager::CommandRequest(LPOBJ lpObj, char* arg) // OK
{
	char mode[5] = { 0 };

	this->GetString(arg, mode, sizeof(mode), 0);

	if (strcmp(mode, "on") == 0)
	{
		lpObj->Option |= 1;
		lpObj->Option |= 2;
		lpObj->Option &= ~4;

		lpObj->AutoRe = false;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		GCMessagePopupSend(lpObj, gMessage.GetMessage(126));
	}
	else if (strcmp(mode, "off") == 0)
	{
		lpObj->Option &= ~1;
		lpObj->Option &= ~2;
		lpObj->Option &= ~4;

		lpObj->AutoRe = false;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		GCMessagePopupSend(lpObj, gMessage.GetMessage(127));
	}
	else if (strcmp(mode, "auto") == 0)
	{
		lpObj->Option |= 1;
		lpObj->Option |= 2;
		lpObj->Option |= 4;

		lpObj->AutoRe = true;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		this->GetString(arg, lpObj->AutoPartyPassword, sizeof(lpObj->AutoPartyPassword), 1);

		GCMessagePopupSend(lpObj, gMessage.GetMessage(128));
	}
}

void CCommandManager::CommandHide(LPOBJ lpObj, char* arg) // OK
{
	if (gEffectManager.CheckEffect(lpObj, EFFECT_INVISIBILITY) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, EFFECT_INVISIBILITY, 0, 0, 0, 0, 0);

		gObjViewportListProtocolDestroy(lpObj);

		gLog.Output(LOG_COMMAND, "[CommandHide][%s][%s] - (State: %d)", lpObj->Account, lpObj->Name, 1);
	}
	else
	{
		gEffectManager.DelEffect(lpObj, EFFECT_INVISIBILITY);

		gObjViewportListProtocolCreate(lpObj);

		gLog.Output(LOG_COMMAND, "[CommandHide][%s][%s] - (State: %d)", lpObj->Account, lpObj->Name, 0);
	}
}

void CCommandManager::CommandResetAutoProc(LPOBJ lpObj) // OK
{
	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		return;
	}

	/*if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->TimerRandom : (lpObj->TimerRandom + lpObj->MasterLevel)) < gResetTable.GetResetLevel(lpObj))
	{
		return;
	}*/

	if (lpObj->Level < gResetTable.GetResetLevel(lpObj))
	{
		return;
	}

	if (lpObj->Money < ((DWORD)gResetTable.GetResetMoney(lpObj)))
	{
		return;
	}

	if (lpObj->Reset >= gServerInfo.m_CommandResetLimit[lpObj->AccountLevel])
	{
		return;
	}

	if (lpObj->CommandManagerTransaction[0] == 0)
	{
		lpObj->CommandManagerTransaction[0]++;
		lpObj->CommandManagerTransactionTime[0] = GetTickCount();
	}
	else
	{
		if ((GetTickCount() - lpObj->CommandManagerTransactionTime[0]) < 30000)
		{
			return;
		}
	}

	SDHP_COMMAND_RESET_SEND pMsg;
	pMsg.header.set(0x0F, 0x00, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CCommandManager::DGCommandResetRecv(SDHP_COMMAND_RESET_RECV* lpMsg) // OK
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGCommandResetRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CommandManagerTransaction[0] = 0;

	if (lpMsg->ResetDay >= ((DWORD)gServerInfo.m_CommandResetLimitDay[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(129), gServerInfo.m_CommandResetLimitDay[lpObj->AccountLevel]);
		return;
	}

	if (lpMsg->ResetWek >= ((DWORD)gServerInfo.m_CommandResetLimitWek[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(130), gServerInfo.m_CommandResetLimitWek[lpObj->AccountLevel]);
		return;
	}

	if (lpMsg->ResetMon >= ((DWORD)gServerInfo.m_CommandResetLimitMon[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(131), gServerInfo.m_CommandResetLimitMon[lpObj->AccountLevel]);
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandResetMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->Level = ((gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel] == -1) ? (lpObj->Level - gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel]) : gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel]);

	lpObj->Experience = gLevelExperience[lpObj->Level - 1];

	lpObj->Reset += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->ResetDay += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpMsg->ResetWek += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpMsg->ResetMon += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	if (lpObj->Reset == 1 && lpObj->MasterReset == 0)
	{
		gNotice.SendMapServerGroupMsg(0, "Saudem o grande [%s] pelo seu primeiro reset!", lpObj->Name);
	}

	if (gServerInfo.m_CommandResetQuest[lpObj->AccountLevel] != 0)
	{
		lpObj->DBClass = lpObj->DBClass >> 4;
		lpObj->ChangeUp = CS_GET_CHANGEUP_SXII(lpObj->DBClass);

		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		BYTE Class = lpObj->DBClass;

		gQuest.GCQuestRewardSend(lpObj->Index, 201, Class);

		for (int i = 0; i < 200; i++)
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}

		gQuest.GCQuestInfoSend(lpObj->Index);
	}

	if (gServerInfo.m_CommandResetStartMasterLevel[lpObj->AccountLevel] > -1)
	{
		if (lpObj->MasterLevel > gServerInfo.m_CommandResetStartMasterLevel[lpObj->AccountLevel])
		{
			lpObj->MasterLevel = gServerInfo.m_CommandResetStartMasterLevel[lpObj->AccountLevel];
		}
	}

	if (gServerInfo.m_CommandResetSkill[lpObj->AccountLevel] != 0)
	{
		lpObj->MasterLevel = 0;
		lpObj->MasterPoint = 0;

		for (int n = 0; n < MAX_SKILL_LIST; n++)
		{
			lpObj->Skill[n].Clear();
		}

		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			lpObj->MasterSkill[n].Clear();
		}

		gMasterSkillTree.GCMasterSkillListSend(lpObj->Index);

		gSkillManager.GCSkillListSend(lpObj, 0);

		gObjectManager.CharacterCalcAttribute(lpObj->Index);

		gMasterSkillTree.GCMasterInfoSend(lpObj);
	}

	if (gServerInfo.m_CommandResetType == 1)
	{
		int point = gResetTable.GetResetPoint(lpObj);

		point = (point * gServerInfo.m_CommandResetPointRate[lpObj->Class]) / 100;

		point += (lpObj->Level - 1) * gServerInfo.m_LevelUpPoint[lpObj->Class];

		point += ((gQuest.CheckQuestListState(lpObj, 2, QUEST_FINISH) == 0) ? 0 : ((lpObj->Level > 220) ? ((lpObj->Level - 220) * gServerInfo.m_PlusStatPoint) : 0));

		point += gQuest.GetQuestRewardLevelUpPoint(lpObj);

		point += lpObj->FruitAddPoint;

		lpObj->LevelUpPoint = point;

		lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
		lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
		lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
		lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
		lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;
	}

	if (gServerInfo.m_CommandMasterResetType == 1)
	{
		int point = 0;

		point = gServerInfo.m_CommandMasterResetPoint[lpObj->AccountLevel] * lpObj->MasterReset;

		point = (point * gServerInfo.m_CommandMasterResetPointRate[lpObj->Class]) / 100;

		lpObj->LevelUpPoint += point;
	}

	//Auto stats add after reset
	for (int i = 0; i < 5; i++)
	{
		if (lpObj->Class != CLASS_DL && i == 4)
			break;

		if (lpObj->AutoDt[i] > 0)
		{
			gObjectManager.CharacterLevelUpPointAdd(lpObj, i, lpObj->AutoDt[i], false);
		}
	}

	int availablePoints = lpObj->LevelUpPoint;
	//Auto stats add
	for (int i = 0; i < 5; i++)
	{
		if (lpObj->Class != CLASS_DL && i == 4)
			break;

		int addPoints = GetRoundValue(availablePoints * lpObj->AutoDtFix[i] / 100);

		if (addPoints > 0)
		{
			gObjectManager.CharacterLevelUpPointAdd(lpObj, i, addPoints, false);
		}
	}

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	GCNewCharacterInfoSend(lpObj);

	//GDCharacterInfoSaveSend(lpObj->Index);

	GDResetInfoSaveSend(lpObj->Index, lpMsg->ResetDay, lpMsg->ResetWek, lpMsg->ResetMon);

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(95), lpObj->Reset);

	if (gServerInfo.m_CommandResetMove[lpObj->AccountLevel] != 0)
	{
		switch (lpObj->Class)
		{
		case CLASS_GL:
		case CLASS_DW:
		case CLASS_DK:
		case CLASS_MG:
		case CLASS_DL:
		case CLASS_RF:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_FE:
			gObjMoveGate(lpObj->Index, 27);
			break;
		case CLASS_SU:
			gObjMoveGate(lpObj->Index, 267);
			break;
		}
	}

	gLog.Output(LOG_COMMAND, "[CommandReset][%s][%s] - (Reset: %d)", lpObj->Account, lpObj->Name, lpObj->Reset);
}

void CCommandManager::CommandSpawn(LPOBJ lpObj, char* arg)
{
	auto monsterClass = GetNumber(arg, 0, 0);
	auto monsterCount = GetNumber(arg, 1, 1);
	auto monsterX = GetNumber(arg, 2, lpObj->X);
	auto monsterY = GetNumber(arg, 3, lpObj->Y);
	auto monsterMapNumber = GetNumber(arg, 4, lpObj->Map);

	for (int i = 0; i < monsterCount; i++)
	{
		int index = gObjAddMonster(lpObj->Map);

		if (OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		LPOBJ MonsterlpObj = &gObj[index];

		MonsterlpObj->X = monsterX;
		MonsterlpObj->Y = monsterY;
		MonsterlpObj->TX = monsterX;
		MonsterlpObj->TY = monsterY;
		MonsterlpObj->OldX = monsterX;
		MonsterlpObj->OldY = monsterY;
		MonsterlpObj->StartX = monsterX;
		MonsterlpObj->StartY = monsterY;
		MonsterlpObj->Dir = (lpObj->Dir + i) % 8;
		MonsterlpObj->Map = monsterMapNumber;

		if (gObjSetMonster(index, monsterClass, true) == 0)
		{
			LogAdd(LOG_RED, "gObjDel %s %s %d [Class: %d]", __FILE__, __FUNCTION__, __LINE__, gObj[index].Class);
			gObjDel(index);
			continue;
		}
	}

	gLog.Output(LOG_COMMAND, "[CommandSpawn][%s][%s] - (Monster: %d, Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, monsterClass, monsterMapNumber, monsterX, monsterY);
}

void CCommandManager::DGCommandMasterResetRecv(SDHP_COMMAND_MASTER_RESET_RECV* lpMsg) // OK
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGCommandMasterResetRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CommandManagerTransaction[0] = 0;

	if (lpMsg->MasterResetDay >= ((DWORD)gServerInfo.m_CommandMasterResetLimitDay[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(132), gServerInfo.m_CommandMasterResetLimitDay[lpObj->AccountLevel]);
		return;
	}

	if (lpMsg->MasterResetWek >= ((DWORD)gServerInfo.m_CommandMasterResetLimitWek[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(133), gServerInfo.m_CommandMasterResetLimitWek[lpObj->AccountLevel]);
		return;
	}

	if (lpMsg->MasterResetMon >= ((DWORD)gServerInfo.m_CommandMasterResetLimitMon[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(134), gServerInfo.m_CommandMasterResetLimitMon[lpObj->AccountLevel]);
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->Level = ((gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel] == -1) ? (lpObj->Level - gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel]) : gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel]);

	lpObj->Experience = gLevelExperience[lpObj->Level - 1];

	lpObj->Reset = ((gServerInfo.m_CommandMasterResetStartReset[lpObj->AccountLevel] == -1) ? (lpObj->Reset - gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel]) : gServerInfo.m_CommandMasterResetStartReset[lpObj->AccountLevel]);

	lpObj->MasterReset += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->MasterResetDay += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->MasterResetWek += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->MasterResetMon += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	if (gServerInfo.m_CommandMasterResetQuest[lpObj->AccountLevel] != 0)
	{
		lpObj->DBClass = lpObj->DBClass >> 4;

		lpObj->ChangeUp = CS_GET_CHANGEUP_SXII(lpObj->DBClass);	// 

		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		BYTE Class = lpObj->DBClass;

		gQuest.GCQuestRewardSend(lpObj->Index, 201, Class);

		for (int i = 0; i < 200; i++)
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}

		gQuest.GCQuestInfoSend(lpObj->Index);
	}

	if (gServerInfo.m_CommandMasterResetSkill[lpObj->AccountLevel] != 0)
	{
		lpObj->MasterLevel = 0;

		lpObj->MasterPoint = 0;

		for (int n = 0; n < MAX_SKILL_LIST; n++)
		{
			lpObj->Skill[n].Clear();
		}

		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			lpObj->MasterSkill[n].Clear();
		}

		gMasterSkillTree.GCMasterSkillListSend(lpObj->Index);

		gSkillManager.GCSkillListSend(lpObj, 0);

		gObjectManager.CharacterCalcAttribute(lpObj->Index);

		gMasterSkillTree.GCMasterInfoSend(lpObj);
	}

	if (gServerInfo.m_CommandMasterResetType == 1)
	{
		int point = 0;

		point = gServerInfo.m_CommandMasterResetPoint[lpObj->AccountLevel] * lpObj->MasterReset;

		point = (point * gServerInfo.m_CommandMasterResetPointRate[lpObj->Class]) / 100;

		point += (lpObj->Level - 1) * gServerInfo.m_LevelUpPoint[lpObj->Class];

		point += ((gQuest.CheckQuestListState(lpObj, 2, QUEST_FINISH) == 0) ? 0 : ((lpObj->Level > 220) ? ((lpObj->Level - 220) * gServerInfo.m_PlusStatPoint) : 0));

		point += gQuest.GetQuestRewardLevelUpPoint(lpObj);

		point += lpObj->FruitAddPoint;

		lpObj->LevelUpPoint = point;

		lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
		lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
		lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
		lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
		lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;
	}

	if (gServerInfo.m_CommandResetType == 1)
	{
		int point = gResetTable.GetResetPoint(lpObj);

		point = (point * gServerInfo.m_CommandResetPointRate[lpObj->Class]) / 100;

		lpObj->LevelUpPoint += point;
	}

	if (gServerInfo.m_CommandMasterResetRuudBonus[lpObj->AccountLevel] > 0)
	{
		lpObj->Ruud += gServerInfo.m_CommandMasterResetRuudBonus[lpObj->AccountLevel];
		gRheaGold.GC_Notify_RuudUpdate(lpObj->Index, lpObj->Ruud, gServerInfo.m_CommandMasterResetRuudBonus[lpObj->AccountLevel], 1);
		GCFireworksSend(lpObj, lpObj->X, lpObj->Y);
	}

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	GCNewCharacterInfoSend(lpObj);

	GDCharacterInfoSaveSend(lpObj->Index);

	GDMasterResetInfoSaveSend(lpObj->Index, lpMsg->MasterResetDay, lpMsg->MasterResetWek, lpMsg->MasterResetMon);

	GCMessagePopupSend(lpObj, gMessage.GetMessage(125), lpObj->MasterReset);

	if (gServerInfo.m_CommandMasterResetMove[lpObj->AccountLevel] != 0)
	{
		switch (lpObj->Class)
		{
		case CLASS_GL:
		case CLASS_DW:
		case CLASS_DK:
		case CLASS_MG:
		case CLASS_DL:
		case CLASS_RF:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_FE:
			gObjMoveGate(lpObj->Index, 27);
			break;
		case CLASS_SU:
			gObjMoveGate(lpObj->Index, 267);
			break;
		}
	}

	gLog.Output(LOG_COMMAND, "[CommandMasterReset][%s][%s] - (MasterReset: %d)", lpObj->Account, lpObj->Name, lpObj->MasterReset);
}

void CCommandManager::DGCommandLoadBuildRecv(LOAD_DT_RECV* lpMsg) // OK
{
	if (OBJECT_RANGE(lpMsg->userIndex) == 0)
	{
		LogAdd(LOG_RED, "[DGCommandLoadBuildRecv] Invalid Index [%d]", lpMsg->userIndex);
		CloseClient(lpMsg->userIndex);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->userIndex];

	if (lpMsg->result == 0)
	{
		GCMessagePopupSend(lpObj, "Essa build nao existe...\nPara cadastrar uma nova build, digite /savedt seguido do nome da build de pontos.");
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) == 0 && lpObj->Map != MAP_CASTLE_SIEGE)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(14));
		return;
	}

	if (lpObj->Map == MAP_CASTLE_SIEGE && ((lpObj->X < 30 || lpObj->X > 40) || (lpObj->Y < 30 || lpObj->Y > 45)))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(14));
		return;
	}

	int TotalPoints = 0;
	TotalPoints = lpObj->LevelUpPoint + lpObj->Strength + lpObj->Dexterity + lpObj->Energy + lpObj->Vitality + lpObj->Leadership;

	int TotalRebuild = 0;
	int Str, Dex, Vit, Eng, Com;

	Str = lpMsg->Strength;
	Dex = lpMsg->Dexterity;
	Vit = lpMsg->Vitality;
	Eng = lpMsg->Energy;
	Com = lpMsg->Leadership;

	TotalRebuild = Str + Dex + Vit + Eng + Com;
	
	if (TotalRebuild == 0)
	{
		GCMessagePopupSend(lpObj, "Essa build nao existe ou ta bugada...=/");
		return;
	}
	else
	{
		if (TotalPoints < TotalRebuild)
		{
			GCMessagePopupSend(lpObj, "Voce nao tem a quantidade de pontos especificada na build salva.");
			return;
		}

		if (Str < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength ||
			Dex < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity ||
			Vit < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality ||
			Eng < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy ||
			Com < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership)
		{
			GCMessagePopupSend(lpObj, "Voce nao pode usar menos pontos do que o padrao de seu personagem em nenhum atributo");
			return;
		}

		lpObj->Strength = Str;
		lpObj->Dexterity = Dex;
		lpObj->Vitality = Vit;
		lpObj->Energy = Eng;
		lpObj->Leadership = Com;
	}	

	lpObj->LevelUpPoint = TotalPoints - lpObj->Strength - lpObj->Dexterity - lpObj->Vitality - lpObj->Energy - lpObj->Leadership;

	gEffectManager.ClearAllEffect(lpObj);

	lpObj->MaxLife = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].MaxLife + ((lpObj->Level - 1) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].LevelLife) + ((lpObj->Vitality - gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].VitalityToLife);
	lpObj->MaxMana = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].MaxMana + ((lpObj->Level - 1) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].LevelMana) + ((lpObj->Energy - gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].EnergyToMana);

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	GCNewCharacterInfoSend(lpObj);
	SXGetCharacterInfo(lpObj->Index);
	GDCharacterInfoSaveSend(lpObj->Index);

	GCMessagePopupSend(lpObj, "Build alterada com sucesso.");

	gLog.Output(LOG_COMMAND, "[CommandLoadRebuild][%s][%s] - (TotalPoints: %d)", lpObj->Account, lpObj->Name, TotalPoints);
}

void CCommandManager::CommandClearInventory(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandClearInventoryEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	int MaxValue = gItemManager.GetInventoryMaxValue(lpObj);

	for (int i = INVENTORY_WEAR_SIZE; i < MaxValue; i++)
	{
		CItem* lpItem = &lpObj->Inventory[i];
		gPentagramSystem.DelAllPentagramJewelInfo(lpObj, lpItem, PENTAGRAM_JEWEL_TYPE_INVENTORY);
		gItemManager.InventoryDelItem(lpObj->Index, i);
		gItemManager.GCItemDeleteSend(lpObj->Index, i, 1);
	}

	GCMessagePopupSend(lpObj, gMessage.GetMessage(13));

	gLog.Output(LOG_COMMAND, "[CommandClearIventory] Used for: [%s] [%s]", lpObj->Account, lpObj->Name);
}

void CCommandManager::CommandEvo(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandChangeEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(82), gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(81));
		return;
	}

	int profession = 0;

	if (lpObj->Authority == 32)
	{
		profession = GetNumber(arg, 0, 0);
	}

	if (profession == 0) // no input
	{
		switch (lpObj->DBClass)
		{
		case DB_CLASS_DW:	case DB_CLASS_DK:	case DB_CLASS_FE:	case DB_CLASS_SU:
			profession = 2;
			break;
		case DB_CLASS_SM:	case DB_CLASS_BK:	case DB_CLASS_ME:	case DB_CLASS_MG:
		case DB_CLASS_DL:	case DB_CLASS_BS:	case DB_CLASS_RF:	case DB_CLASS_FK:
			profession = 3;
			break;
		case DB_CLASS_GM:	case DB_CLASS_BM:	case DB_CLASS_HE:	case DB_CLASS_DM:
		case DB_CLASS_LE:	case DB_CLASS_DS:	case DB_CLASS_FM:	case DB_CLASS_PK:
			profession = 4;
			break;
		default:
			return;
			break;
		}
	}

	if (lpObj->DBClass == DB_CLASS_MG || lpObj->DBClass == DB_CLASS_DL && profession == 2)
	{
		profession = 3;
	}

	bool changed = false;

	switch (profession)
	{
	case 1:
	{
		lpObj->DBClass &= ~14;
		lpObj->ChangeUp = 0;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		for (int i = 0; i <= 9; i++)
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}

		lpObj->SendQuestInfo = 0;

		gQuest.GCQuestRewardSend(lpObj->Index, 200, lpObj->DBClass);
		gQuest.GCQuestInfoSend(lpObj->Index);
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		GCMessagePopupSend(lpObj, gMessage.GetMessage(80));
		changed = true;
	}
	break;
	case 2:
	{
		lpObj->ChangeUp = 1;
		lpObj->DBClass |= CS_SET_CHANGEUP_SXII;

		//lpObj->LevelUpPoint += 220;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		BYTE btClass = lpObj->Class << 4;
		btClass |= 0x08;

		for (int i = 0; i < 4; i++) // complete 4 first quest states = first prof
		{
			lpObj->Quest[i].questState = QUEST_FINISH;
		}
		for (int i = 4; i <= 9; i++) // uncomplete 5-9 quests 
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}
		lpObj->SendQuestInfo = 0;

		gQuest.GCQuestRewardSend(lpObj->Index, 201, btClass);
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		changed = true;
	}
	break;
	case 3:
	{
		lpObj->ChangeUp = 2;
		//lpObj->LevelUpPoint += 250;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		BYTE btClass = (lpObj->Class << 4);
		btClass |= 0x08;
		btClass |= 0x04;

		lpObj->DBClass |= CS_SET_CHANGEUP_SXII;
		lpObj->DBClass |= CS_SET_CHANGEUP2_SXII;

		for (int i = 0; i < 7; i++) // complete 7 first quest states = first prof
		{
			lpObj->Quest[i].questState = QUEST_FINISH;
		}
		for (int i = 7; i <= 9; i++) // uncomplete other quests 
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}
		lpObj->SendQuestInfo = 0;

		GCLevelUpSend(lpObj);
		gQuest.GCQuestRewardSend(lpObj->Index, 204, btClass);
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		changed = true;
	}
	break;
	case 4:
	{
		lpObj->ChangeUp = 3;
		//lpObj->LevelUpPoint += 250;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		BYTE btClass = (lpObj->DBClass);
		btClass |= 0x08;
		btClass |= 0x04;
		btClass |= 0x02;

		lpObj->DBClass |= CS_SET_CHANGEUP_SXII;
		lpObj->DBClass |= CS_SET_CHANGEUP2_SXII;
		lpObj->DBClass |= CS_SET_CHANGEUP4_SXII;

		for (int i = 0; i <= 9; i++) // complete all prof quests
		{
			lpObj->Quest[i].questState = QUEST_FINISH;
		}
		lpObj->SendQuestInfo = 0;

		gQuest.GCQuestRewardSend(lpObj->Index, 206, btClass);
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		GCLevelUpSend(lpObj);
		changed = true;
	}
	break;
	}

	if (changed)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(84));
	}
	else
	{
		GCMessagePopupSend(lpObj, "%s\n%s", gMessage.GetMessage(85), gMessage.GetMessage(86));
	}

	gLog.Output(LOG_COMMAND, "[CommandEvo][%s][%s] - (Profession: %d)", lpObj->Account, lpObj->Name, profession);
}

void CCommandManager::CommandRebuild(LPOBJ lpObj, char* arg)
{
	if (gServerInfo.m_CommandRebuildEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) == 0 && lpObj->Map != MAP_CASTLE_SIEGE)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(14));
		return;
	}

	if (lpObj->Map == MAP_CASTLE_SIEGE && ((lpObj->X < 30 || lpObj->X > 40) || (lpObj->Y < 30 || lpObj->Y > 45)))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(14));
		return;
	}

	int TotalPoints = 0;
	TotalPoints = lpObj->LevelUpPoint + lpObj->Strength + lpObj->Dexterity + lpObj->Energy + lpObj->Vitality + lpObj->Leadership;

	int TotalRebuild = 0;
	int Str, Dex, Vit, Eng, Com;

	char attr[2] = { 0 };
	this->GetString(arg, attr, sizeof(attr), 0);

	bool attributeOnly = false;

	if (strcmp(attr, "f") == 0)
	{
		lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
		attributeOnly = true;
		GCMessagePopupSend(lpObj, "Pontos em Forca resetados.");
	}
	else if (strcmp(attr, "a") == 0)
	{
		lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
		attributeOnly = true;
		GCMessagePopupSend(lpObj, "Pontos em Agilidade resetados.");
	}
	else if (strcmp(attr, "v") == 0)
	{
		lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
		attributeOnly = true;
		GCMessagePopupSend(lpObj, "Pontos em Vitalidade resetados.");
	}
	else if (strcmp(attr, "e") == 0)
	{
		lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
		attributeOnly = true;
		GCMessagePopupSend(lpObj, "Pontos em Energia resetados.");
	}
	else if (strcmp(attr, "c") == 0)
	{
		lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;
		attributeOnly = true;
		GCMessagePopupSend(lpObj, "Pontos em Comando resetados.");
	}
	else
	{
		Str = this->GetNumber(arg, 0, 0);
		Dex = this->GetNumber(arg, 1, 0);
		Vit = this->GetNumber(arg, 2, 0);
		Eng = this->GetNumber(arg, 3, 0);
		Com = this->GetNumber(arg, 4, 0);
		
		TotalRebuild = Str + Dex + Vit + Eng + Com;
	}

	if (!attributeOnly)
	{
		if (TotalRebuild == 0)
		{
			lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
			lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
			lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
			lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
			lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;
		}
		else
		{
			if (TotalPoints < TotalRebuild)
			{
				GCMessagePopupSend(lpObj, "Voce nao tem a quantidade de pontos especificada.");
				return;
			}

			if (Str < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength ||
				Dex < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity ||
				Vit < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality ||
				Eng < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy ||
				Com < gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership)
			{
				GCMessagePopupSend(lpObj, "Voce nao pode usar menos pontos do que o padrao de seu personagem em nenhum atributo");
				return;
			}

			lpObj->Strength = Str;
			lpObj->Dexterity = Dex;
			lpObj->Vitality = Vit;
			lpObj->Energy = Eng;
			lpObj->Leadership = Com;
		}
	}

	lpObj->LevelUpPoint = TotalPoints - lpObj->Strength - lpObj->Dexterity - lpObj->Vitality - lpObj->Energy - lpObj->Leadership;

	gEffectManager.ClearAllEffect(lpObj);

	lpObj->MaxLife = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].MaxLife + ((lpObj->Level - 1) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].LevelLife) + ((lpObj->Vitality - gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].VitalityToLife);
	lpObj->MaxMana = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].MaxMana + ((lpObj->Level - 1) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].LevelMana) + ((lpObj->Energy - gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].EnergyToMana);

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	GCNewCharacterInfoSend(lpObj);
	SXGetCharacterInfo(lpObj->Index);
	GDCharacterInfoSaveSend(lpObj->Index);
	
	if (!attributeOnly && TotalRebuild == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(15));
	}

	gLog.Output(LOG_COMMAND, "[CommandRebuild][%s][%s] - (TotalPoints: %d)", lpObj->Account, lpObj->Name, TotalPoints);
}

void CCommandManager::CommandPvpTest(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	if (strlen(name) > 3)
	{
		LPOBJ lpTarget = gObjFind(name);

		if (lpTarget == 0)
		{
			GCMessagePopupSend(lpObj, "Defensive Player not found (%s).", name);
			return;
		}
		else
		{
			lpTarget->TargetPvPIndex = 99999;
		}
	}
	else
	{
		if (strcmp(name, "dpt") == 0)
		{
			PvP_DPT_Timer = GetTickCount();
			PvP_DPT_Delay = (this->GetNumber(arg, 1, 5) * 1000);
			PvP_DPT_Damage = 0;
		}
	}

	if (!IsPvPTestOn)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
		{
			if (gObjIsConnectedGP(n) != 0)
			{
				gObj[n].LowerHPPvPTest = 2000000000;
			}
		}
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Teste iniciado");
	}
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Teste finalizado");
		g_FilaHit.LimparFila();
	}

	PvPTimer = GetTickCount();
	IsPvPTestOn = !IsPvPTestOn;
}

void CCommandManager::CommandAutoPvP(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);
	
	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, "Jogador nao encontrado (%s).", name);
		return;
	}

	if (lpTarget->AutoPVPTesting)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Teste finalizado");
		lpTarget->AutoPVPTesting = false;
		return;
	}

	if (lpObj->MUFCFighter > 0 || lpTarget->MUFCFighter > 0)
	{
		GCMessagePopupSend(lpObj, "Deixe de putaria, infiliz!");
		return;
	}

	if (strcmp(lpObj->HardwareId, lpTarget->HardwareId) != 0)
	{
		GCMessagePopupSend(lpObj, "Voce so pode setar um algo estando na mesma maquina que ele");
		return;
	}

	if (lpTarget->Map != MAP_ARENA && lpTarget->Map != MAP_CASTLE_SIEGE)
	{
		GCMessagePopupSend(lpObj, "Comando disponivel apenas em Arena e Loren");
		return;
	}

	lpTarget->TargetPvPIndex = lpObj->Index;

	lpTarget->AttackCustomZoneX = lpTarget->X;
	lpTarget->AttackCustomZoneY = lpTarget->Y;
	lpTarget->AttackCustomZoneMap = lpTarget->Map;
		
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Teste iniciado");

	gLog.Output(LOG_COMMAND, "[CommandAutoPVP][%s][%s] - (Target: %s)", lpObj->Account, lpObj->Name, lpTarget->Name);

	lpTarget->AutoPVPTesting = true;
}

void CCommandManager::CommandSetBuffs(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int Type = this->GetNumber(arg, 1, 0);

	if (Type == 0) //self class buff
	{
		switch (lpTarget->Class)
		{
		case CLASS_DW:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_MANA_SHIELD) != 0)
				gSkillManager.SkillManaShield(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_MANA_SHIELD));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Soul Barrier.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_MAGIC_CIRCLE) != 0)
				gSkillManager.SkillMagicCircle(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_MAGIC_CIRCLE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Expansion of Wizardry.", lpTarget->Name);
		}
		break;
		case CLASS_DK:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_LIFE) != 0)
				gSkillManager.SkillGreaterLife(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_LIFE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Greater Fortitude.", lpTarget->Name);
		}
		break;
		case CLASS_DL:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_CRITICAL_DAMAGE) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_CRITICAL_DAMAGE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Critical Damage.", lpTarget->Name);
		}
		break;
		case CLASS_SU:
		{
			/*if (gSkillManager.GetSkill(lpTarget, SKILL_SWORD_POWER) != 0)
				gSkillManager.SkillSwordPower(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_SWORD_POWER));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Berseker.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_DAMAGE_REFLECT) != 0)
				gSkillManager.SkillDamageReflect(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_DAMAGE_REFLECT));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Damage Reflection.", lpTarget->Name);*/
		}
		break;
		case CLASS_FE:
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_GREATER_DEFENSE, 9999999, 0, 0, 0, 0);		
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_GREATER_DAMAGE, 9999999, 0, 0, 0, 0);
		}
		break;
		case CLASS_RF:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_FITNESS) != 0)
				gSkillManager.SkillFitness(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_FITNESS));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Fitness.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_DEFENSE_SUCCESS_RATE) != 0)
				gSkillManager.SkillGreaterDefenseSuccessRate(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_DEFENSE_SUCCESS_RATE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Increase Block.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_IGNORE_DEFENSE_RATE) != 0)
				gSkillManager.SkillGreaterIgnoreDefenseRate(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_IGNORE_DEFENSE_RATE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Ignore Defense.", lpTarget->Name);
		}
		break;
		case CLASS_GL:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_WRATH) != 0)
				gSkillManager.SkillWrath(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_WRATH));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Wrath.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_OBSIDIAN) != 0)
				gSkillManager.SkillObsidian(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_OBSIDIAN));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Obsidian.", lpTarget->Name);
		}
		break;
		}
	}
	else if (Type == 1) //put defense
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_MANA_SHIELD), 999999999, gServerInfo.m_ManaShieldMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_LIFE), 999999999, gServerInfo.m_GreaterLifeMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_DEFENSE), 999999999, gServerInfo.m_GreaterDefenseMax, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_DAMAGE_REFLECT), 999999999, gServerInfo.m_ReflectDamageMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_FITNESS), 999999999, gServerInfo.m_FitnessMax, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_DEFENSE_SUCCESS_RATE), 999999999, gServerInfo.m_GreaterDefenseSuccessMax, 0, 0, 0);
	}
	else if (Type == 2) //put attack
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_CRITICAL_DAMAGE), 999999999, gServerInfo.m_GreaterCriticalDamageMax, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_DAMAGE), 999999999, gServerInfo.m_GreaterDamageMax, 0, 0, 0);

		if (lpTarget->Class == CLASS_DW)
		{
			int v1 = (((lpTarget->Energy + lpTarget->AddEnergy) / gServerInfo.m_DWMagicDamageMinConstA) * gServerInfo.m_MagicCircleConstA) / 100.0f;
			int v2 = (((lpTarget->Energy + lpTarget->AddEnergy) / gServerInfo.m_DWMagicDamageMaxConstA) * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MAGIC_CIRCLE_IMPROVED)) / 100.0f;
			int v3 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MAGIC_CIRCLE_ENHANCED);
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_MAGIC_CIRCLE), 999999999, gServerInfo.m_MagicCircleConstA, v1, v2, v3);
		}

		if (lpTarget->Class == CLASS_FE)
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_INFINITY_ARROW), 999999999, 0, 0, 0, 0);

		if (lpTarget->Class == CLASS_SU)
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_SWORD_POWER), 999999999, gServerInfo.m_SwordPowerMaxRate, gServerInfo.m_SwordPowerMaxRate, 0, 0);

		if (lpTarget->Class == CLASS_RF)
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_IGNORE_DEFENSE_RATE), 999999999, gServerInfo.m_GreaterIgnoreDefenseMax, 0, 0, 0);

		if (lpTarget->Class == CLASS_GL)
		{
			int value = lpTarget->Defense * gServerInfo.m_WrathCutDefConst / 100;
			int value1 = lpTarget->PhysiDamageMaxRight * gServerInfo.m_WrathConst / 100;
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_WRATH), 999999999, value, value1, 0, 0);
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_OBSIDIAN), 999999999, gServerInfo.m_ObsidianMax, 0, 0, 0);
		}
	}
	else if (Type == 3) //buffs comuns de pt
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_MANA_SHIELD), 999999999, gServerInfo.m_ManaShieldMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_LIFE), 999999999, gServerInfo.m_GreaterLifeMaxRate, 0, 0, 0);
	}
	else if (Type == 4) //debuffs
	{
		//Summoner
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_LESSER_DAMAGE), 999999999, gServerInfo.m_LesserDamageMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_LESSER_DEFENSE), 999999999, gServerInfo.m_LesserDefenseMaxRate, 0, 0, 0);
	}
	else if (Type == 5) //clear all
	{
		gEffectManager.ClearAllEffect(lpTarget);
	}
	else if (Type == 6) //specific buff
	{
		int buff = this->GetNumber(arg, 2, 0);
		int duration = this->GetNumber(arg, 3, 0);
		gEffectManager.AddEffect(lpTarget, 1, buff, (int)(time(0) + duration), 0, 0, 0, 0);
	}

	gObjectManager.CharacterCalcAttribute(lpTarget->Index);
}

void CCommandManager::CommandSetTarget(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpAttacker = gObjFind(name);

	if (lpAttacker == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	this->GetString(arg, name, sizeof(name), 1);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	lpAttacker->TargetPvPIndex = lpTarget->Index;
	lpAttacker->LowerHPPvPTest = 2000000000;
	lpTarget->LowerHPPvPTest = 2000000000;

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Setado alvo PvP: %s (%d) -> %s (%d)", lpAttacker->Name, lpAttacker->Index, lpTarget->Name, lpTarget->Index);
}

void CCommandManager::CommandShowStats(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "[%s]: [F: %d] [A: %d] [V: %d] [E: %d] [C: %d]", lpTarget->Name, lpTarget->Strength, lpTarget->Dexterity, lpTarget->Vitality, lpTarget->Energy, lpTarget->Leadership);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "[%s]: [Def: %d] [PhAtck: %d + %d] [MgAtck: %d]", lpTarget->Name, lpTarget->Defense, lpTarget->PhysiDamageMaxRight, lpTarget->PhysiDamageMaxLeft, lpTarget->MagicDamageMax);
	WORD effect = 0;
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "[%s]: [Final Def: %d]", lpTarget->Name, gAttack.GetTargetDefense(lpObj, lpTarget, &effect));

	PMSG_CLOSE_CLIENT_RECV pMsg;
	pMsg.header.set(0xF3, 0xE8, sizeof(pMsg));
	DataSend(lpTarget->Index, (BYTE*)&pMsg, pMsg.header.size);

	gLog.Output(CHARACTER_STATS, "-- [ Stats: > %s < ] ---------", lpTarget->Name);
	gLog.Output(CHARACTER_STATS, "[F: %d] [A: %d] [V: %d] [E: %d] [C: %d]", lpTarget->Strength, lpTarget->Dexterity, lpTarget->Vitality, lpTarget->Energy, lpTarget->Leadership);
	gLog.Output(CHARACTER_STATS, "[Def: %d] [PhAtck: %d + %d] [MgAtck: %d]", lpTarget->Defense, lpTarget->PhysiDamageMaxRight, lpTarget->PhysiDamageMaxLeft, lpTarget->MagicDamageMax);
	gLog.Output(CHARACTER_STATS, "[Final Def: %d]", gAttack.GetTargetDefense(lpObj, lpTarget, &effect));
	
	CItem* lpItem = &lpTarget->Inventory[7];
	if (lpItem->m_Index >= GET_ITEM(12, 414) && lpItem->m_Index <= GET_ITEM(12, 421))
	{
		gLog.Output(CHARACTER_STATS, "[Asa/Capa]: ");
		int Option, Level;
		for (int i = 0; i < 4; i++)
		{
			if (lpItem->m_SocketOption[i] != 254 && lpItem->m_SocketOption[i] != 255)
			{
				Option = (lpItem->m_SocketOption[i] / 16);
				Level = (lpItem->m_SocketOption[i] % 16);

				if (Level < 0 || Level > 9)
				{
					continue;
				}

				gLog.Output(CHARACTER_STATS, " Option %d: %d :: Level: %d ", (i+1), Option, Level );
			}
		}

		if (lpItem->m_SocketOptionBonus >= 0 && lpItem->m_SocketOptionBonus <= 15)
		{
			gLog.Output(CHARACTER_STATS, " Elemental Def: %d", lpItem->m_SocketOptionBonus);
		}

		if (lpItem->m_SocketOption[4] != 254 && lpItem->m_SocketOption[4] != 255)
		{
			Option = (lpItem->m_SocketOption[4] / 16);
			Level = (lpItem->m_SocketOption[4] % 16);

			if (Level >= 0 && Level <= 15)
			{
				gLog.Output(CHARACTER_STATS, " Elemental Option: %d :: Level: %d ", Option, Level);				
			}
		}
	}
	
	if (gMasteryItemExtOptionSet.IsMasteryWeapon(lpTarget->Inventory[0].m_Index))
	{
		CItem* lpItem = &lpTarget->Inventory[0];
		gLog.Output(CHARACTER_STATS, "[Arma Ruud 1]: ");
		for (int n = 0; n < 3; n++)
		{
			if (lpItem->m_SocketOption[n] != 255)
			{
				gLog.Output(CHARACTER_STATS, " Option %d", lpItem->m_SocketOption[n]);
			}
		}

		if (lpItem->m_SocketOption[4] != 0xFF)
		{
			gLog.Output(CHARACTER_STATS, " Bonus %d", lpItem->m_SocketOption[4]);
		}
	}

	if (gMasteryItemExtOptionSet.IsMasteryWeapon(lpTarget->Inventory[1].m_Index))
	{
		CItem* lpItem = &lpTarget->Inventory[1];
		gLog.Output(CHARACTER_STATS, "[Arma Ruud 2]: ");
		for (int n = 0; n < 3; n++)
		{
			if (lpItem->m_SocketOption[n] != 255)
			{
				gLog.Output(CHARACTER_STATS, " Option %d", lpItem->m_SocketOption[n]);
			}
		}

		if (lpItem->m_SocketOption[4] != 0xFF)
		{
			gLog.Output(CHARACTER_STATS, " Bonus %d", lpItem->m_SocketOption[4]);
		}
	}

	if (gMasteryItemExtOptionSet.IsMasteryShield(lpTarget->Inventory[1].m_Index))
	{
		CItem* lpItem = &lpTarget->Inventory[1];
		gLog.Output(CHARACTER_STATS, "[Shield Ruud]: ");
		for (int n = 0; n < 3; n++)
		{
			if (lpItem->m_SocketOption[n] != 255)
			{
				gLog.Output(CHARACTER_STATS, " Option %d", lpItem->m_SocketOption[n]);
			}
		}

		if (lpItem->m_SocketOption[4] != 0xFF)
		{
			gLog.Output(CHARACTER_STATS, " Bonus %d", lpItem->m_SocketOption[4]);
		}
	}

	for (int i = 2; i <= 6; i++)
	{
		if (gMasteryItemExtOptionSet.IsMasterySet(lpTarget->Inventory[i].m_Index))
		{
			CItem* lpItem = &lpTarget->Inventory[i];
			gLog.Output(CHARACTER_STATS, "[Set Ruud | Item %d]: ", i);
			for (int n = 0; n < 3; n++)
			{
				if (lpItem->m_SocketOption[n] != 255)
				{
					gLog.Output(CHARACTER_STATS, " Option %d", lpItem->m_SocketOption[n]);
				}
			}

			if (lpItem->m_SocketOption[4] != 0xFF)
			{
				gLog.Output(CHARACTER_STATS, " Bonus %d", lpItem->m_SocketOption[4]);
			}
		}
	}

	if (gMasteryItemExtOptionSet.IsMasteryPendant(lpTarget->Inventory[9].m_Index))
	{
		CItem* lpItem = &lpTarget->Inventory[9];
		gLog.Output(CHARACTER_STATS, "[Pendant]: ");
		for (int n = 0; n < 3; n++)
		{
			if (lpItem->m_SocketOption[n] != 255)
			{
				gLog.Output(CHARACTER_STATS, " Option %d", lpItem->m_SocketOption[n]);
			}
		}

		if (lpItem->m_SocketOption[4] != 0xFF)
		{
			gLog.Output(CHARACTER_STATS, " Bonus %d", lpItem->m_SocketOption[4]);
		}
	}

	for (int i = 10; i <= 11; i++)
	{
		if (gMasteryItemExtOptionSet.IsMasteryRing(lpTarget->Inventory[i].m_Index))
		{
			CItem* lpItem = &lpTarget->Inventory[i];
			gLog.Output(CHARACTER_STATS, "[Ring | Item %d]: ", i);
			for (int n = 0; n < 3; n++)
			{
				if (lpItem->m_SocketOption[n] != 255)
				{
					gLog.Output(CHARACTER_STATS, " Option %d", lpItem->m_SocketOption[n]);
				}
			}

			if (lpItem->m_SocketOption[4] != 0xFF)
			{
				gLog.Output(CHARACTER_STATS, " Bonus %d", lpItem->m_SocketOption[4]);
			}
		}
	}

	gLog.Output(CHARACTER_STATS, "---------------------------------------------");

	gLog.Output(LOG_COMMAND, "[CommandShowStats][%s][%s] - (From char: %s)", lpObj->Account, lpObj->Name, lpTarget->Name);
}

void CCommandManager::CommandSetStats(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int str = this->GetNumber(arg, 1);
	int dex = this->GetNumber(arg, 2);
	int vit = this->GetNumber(arg, 3);
	int eng = this->GetNumber(arg, 4);
	int com = this->GetNumber(arg, 5, 0);

	lpTarget->Strength = str;
	lpTarget->Dexterity = dex;
	lpTarget->Vitality = vit;
	lpTarget->Energy = eng;
	lpTarget->Leadership = com;

	gEffectManager.ClearAllEffect(lpTarget);

	gObjectManager.CharacterCalcAttribute(lpTarget->Index);

	GCNewCharacterInfoSend(lpObj);

	SXGetCharacterInfo(lpObj->Index);

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Stats of %s changed to %d %d %d %d %d", lpTarget->Name, str, dex, vit, eng, com);
	gNotice.GCNoticeSend(lpTarget->Index, 1, 0, 0, 0, 0, 0, "%s changed your stats to %d %d %d %d %d", lpObj->Name, str, dex, vit, eng, com);

	gLog.Output(LOG_COMMAND, "[CommandSetStats][%s][%s] - (Char %s changed to %d %d %d %d %d)", lpObj->Account, lpObj->Name, lpTarget->Name, str, dex, vit, eng, com);
}

void CCommandManager::CommandSetAutoPotion(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	if (!lpTarget->AutoPotionBot)
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Auto potion started to %s", lpTarget->Name);
	else
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Auto potion stopped to %s", lpTarget->Name);

	lpTarget->AutoPotionBot = !lpTarget->AutoPotionBot;
}

void CCommandManager::CommandWatch(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	if (strcmp(name, "#all") == 0)
	{
		WatchTargetIndex = (WatchTargetIndex == 99999) ? -1 : 99999;
		return;
	}

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	LogAdd(LOG_BLUE, "[GM][%s] is watching user %s", lpObj->Name, lpTarget->Name);

	if (WatchTargetIndex == lpTarget->Index)
	{
		WatchTargetIndex = -1;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "%s is not being watched anymore", lpTarget->Name);
	}
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "%s is being watched", lpTarget->Name);
		WatchTargetIndex = lpTarget->Index;
	}
}

void CCommandManager::CommandChatBlock(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int time = this->GetNumber(arg, 1, 1);

	lpTarget->ChatLimitTime = time;

	GCMessagePopupSend(lpObj, gMessage.GetMessage(27), lpTarget->Name, time);

	gLog.Output(LOG_COMMAND, "[CommandChatBlock][%s][%s] - (Char: %s)", lpObj->Account, lpObj->Name, lpTarget->Name);
}

void CCommandManager::CommandGhost(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	if (strlen(name) < 4 || strcmp(name, lpObj->Name) == 0)
	{
		lpObj->Socket = INVALID_SOCKET;
		lpObj->isGhost = true;
		gCustomAttack.GDUpdateData(lpObj->Index);
		closesocket(lpObj->PerSocketContext->Socket);
		return;
	}

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	lpTarget->Socket = INVALID_SOCKET;
	lpTarget->isGhost = true;
	gCustomAttack.GDUpdateData(lpTarget->Index);
	closesocket(lpTarget->PerSocketContext->Socket);
}

void CCommandManager::CommandBringParty(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	if (lpTarget->PartyNumber < 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(28), name);
		return;
	}

	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		if (gObj[i].Connected == OBJECT_ONLINE && gObj[i].Type == OBJECT_USER && gObj[i].PartyNumber == lpTarget->PartyNumber)
		{
			int x_inc = (GetLargeRand() % 3);
			int y_inc = (GetLargeRand() % 3);

			if (GetLargeRand() % 2 == 0) { x_inc *= -1; }
			if (GetLargeRand() % 2 == 0) { y_inc *= -1; }

			LogAdd(LOG_BLUE, "[GM][%s] Brought user %s to him by Party Move [%d %d]", lpObj->Name, lpTarget->Name, x_inc, y_inc);

			gObjTeleport(gObj[i].Index, lpObj->Map, lpObj->X + x_inc, lpObj->Y + y_inc);
		}
	}

	gLog.Output(LOG_COMMAND, "[CommandBringParty][%s][%s] - (Char: %s)", lpObj->Account, lpObj->Name, lpTarget->Name);
}

void CCommandManager::CommandSaveBuild(LPOBJ lpObj, char* arg)
{
	char dt_name[18] = { 0 };

	this->GetString(arg, dt_name, sizeof(dt_name), 0);

	if (strlen(dt_name) < 2 || strlen(dt_name) > 16)
	{
		GCMessagePopupSend(lpObj, "Escolha um nome com pelo menos 3 e no maximo 16 caracteres para salvar sua build.\nExemplos:\n/savedt def\n/savedt atk\n/savedt boss");
		return;
	}

	dt_name[16] = '\0';

	SAVE_DT_SEND pMsg = { 0 };
	pMsg.h.set(0xD7, 23, sizeof(pMsg));
	memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
	memcpy(pMsg.DtName, dt_name, sizeof(pMsg.DtName));
	pMsg.userIndex = lpObj->Index;
	pMsg.Strength = lpObj->Strength;
	pMsg.Dexterity = lpObj->Dexterity;
	pMsg.Vitality = lpObj->Vitality;
	pMsg.Energy = lpObj->Energy;
	pMsg.Leadership = lpObj->Leadership;

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);

	GCMessagePopupSend(lpObj, "Build atual salva com o nome '%s'\nPara utilizar, digite /loaddt %s.", dt_name, dt_name);
}

void CCommandManager::CommandLoadBuild(LPOBJ lpObj, char* arg)
{
	char dt_name[18] = { 0 };

	this->GetString(arg, dt_name, sizeof(dt_name), 0);

	if (strlen(dt_name) < 2 || strlen(dt_name) > 16)
	{
		GCMessagePopupSend(lpObj, "Escolha um nome com pelo menos 3 e no maximo 16 caracteres para carregar uma build salva.");
		return;
	}

	dt_name[16] = '\0';

	LOAD_DT_SEND pMsg = { 0 };
	pMsg.h.set(0xD7, 24, sizeof(pMsg));
	pMsg.userIndex = lpObj->Index;
	memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
	memcpy(pMsg.DtName, dt_name, sizeof(pMsg.DtName));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.h.size);
}

void CCommandManager::CommandAutoDt(LPOBJ lpObj, char* arg)
{
	if (gServerInfo.m_CommandAddPointAutoEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	int str = this->GetNumber(arg, 0, 0);
	int agi = this->GetNumber(arg, 1, 0);
	int vit = this->GetNumber(arg, 2, 0);
	int ene = this->GetNumber(arg, 3, 0);

	if (str == 0 && agi == 0 && vit == 0 && ene == 0)
	{
		lpObj->AutoDt[0] = lpObj->AutoDt[1] = lpObj->AutoDt[2] = lpObj->AutoDt[3] = lpObj->AutoDt[4] = 0;
		lpObj->AutoDtFix[0] = lpObj->AutoDtFix[1] = lpObj->AutoDtFix[2] = lpObj->AutoDtFix[3] = lpObj->AutoDtFix[4] = 0;
		GCMessagePopupSend(lpObj, "Auto distribuicao de pontos desativada.");
		return;
	}

	if (str > 0 || agi > 0 || vit > 0 || ene > 0)
	{
		lpObj->AutoDt[0] = str;
		lpObj->AutoDt[1] = agi;
		lpObj->AutoDt[2] = vit;
		lpObj->AutoDt[3] = ene;
		lpObj->AutoDt[4] = 0;

		if (lpObj->Class == CLASS_DL)
		{
			int com = this->GetNumber(arg, 4, 0);
			lpObj->AutoDt[4] = com;
		}

		float TotalPoints = lpObj->AutoDt[0] + lpObj->AutoDt[1] + lpObj->AutoDt[2] + lpObj->AutoDt[3] + lpObj->AutoDt[4];

		for (int i = 0; i < 5; i++)
		{
			lpObj->AutoDtFix[i] = GetRoundValue(lpObj->AutoDt[i] / TotalPoints * 100.0);
		}
	}

	if (lpObj->Class == CLASS_DL)
	{
		GCMessagePopupSend(lpObj, "%s\n%s: %d (%d%%)\n%s: %d (%d%%)\n%s: %d (%d%%)\n%s: %d (%d%%)\n%s: %d (%d%%)",
			gMessage.GetMessage(26),
			gMessage.GetMessage(21), lpObj->AutoDt[0], lpObj->AutoDtFix[0],
			gMessage.GetMessage(22), lpObj->AutoDt[1], lpObj->AutoDtFix[1],
			gMessage.GetMessage(23), lpObj->AutoDt[2], lpObj->AutoDtFix[2],
			gMessage.GetMessage(24), lpObj->AutoDt[3], lpObj->AutoDtFix[3],
			gMessage.GetMessage(25), lpObj->AutoDt[4], lpObj->AutoDtFix[4]);
	}
	else
	{
		GCMessagePopupSend(lpObj, "%s\n%s: %d (%d%%)\n%s: %d (%d%%)\n%s: %d (%d%%)\n%s: %d (%d%%)",
			gMessage.GetMessage(26),
			gMessage.GetMessage(21), lpObj->AutoDt[0], lpObj->AutoDtFix[0],
			gMessage.GetMessage(22), lpObj->AutoDt[1], lpObj->AutoDtFix[1],
			gMessage.GetMessage(23), lpObj->AutoDt[2], lpObj->AutoDtFix[2],
			gMessage.GetMessage(24), lpObj->AutoDt[3], lpObj->AutoDtFix[3] );
	}
}

void CCommandManager::CommandSetPk(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int pkLevel = (lpTarget->PKLevel >= 5) ? 3 : 6;

	lpTarget->PKLevel = pkLevel;
	GCPKLevelSend(lpTarget->Index, pkLevel);

	gLog.Output(LOG_COMMAND, "[CommandSetPK][%s][%s] - (Char: %s , PK: %d)", lpObj->Account, lpObj->Name, lpTarget->Name, pkLevel);
}

void CCommandManager::CommandMUFC(LPOBJ lpObj, char* arg)
{
	char name1[11] = { 0 };
	char name2[11] = { 0 };

	this->GetString(arg, name1, sizeof(name1), 0);
	this->GetString(arg, name2, sizeof(name2), 1);	

	if (strlen(name1) > 3 && strlen(name2) > 3)
	{
		if (strcmp(name1, name2) == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Nomes iguais, ta maluco?");
			return;
		}

		LPOBJ lpObj1 = gObjFind(name1);		

		if (lpObj1 == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Lutador 1 nao encontrado");
			return;
		}

		LPOBJ lpObj2 = gObjFind(name2);

		if (lpObj2 == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Lutador 2 nao encontrado");
			return;
		}

		gMUFC.PrepareFighter(1, lpObj1, lpObj);
		gMUFC.PrepareFighter(2, lpObj2, lpObj);
	}
	else
	{
		int rounds = 0;
		rounds = this->GetNumber(arg, 0);

		if (rounds < 1 || rounds > 5)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Quantidade de rounds invalida");
			return;
		}

		gMUFC.FightStart(lpObj, rounds);
	}
}

void CCommandManager::CommandSiege(LPOBJ lpObj, char* arg)
{
#if(GAMESERVER_TYPE==1)
	char cmd[16] = { 0 };
	this->GetString(arg, cmd, sizeof(cmd), 0);

	if (strcmp(cmd, "start") == 0)
	{
		gCastleSiege.SetState(CASTLESIEGE_STATE_STARTSIEGE, TRUE);
	}
	else if (strcmp(cmd, "extend") == 0)
	{
		int ext = this->GetNumber(arg, 1, 0);
		gCastleSiege.m_ExtensionCount++;
		gCastleSiege.m_iCS_REMAIN_MSEC += ext * 60 * 1000;
		gCastleSiege.m_iCS_TARGET_MSEC += ext * 60;

		GCAnsCsLeftTimeAlarm((gCastleSiege.m_iCS_REMAIN_MSEC / 1000) / 3600, ((gCastleSiege.m_iCS_REMAIN_MSEC / 1000) % 3600) / 60);
		gCastleSiege.m_dwCS_LEFTTIME_TICK_COUNT = GetTickCount();
	}
#endif
}

void CCommandManager::CommandFillPotion(LPOBJ lpObj)
{
	if (gServerInfo.m_CommandFillPotionEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}	

	if (lpObj->Money < gServerInfo.m_CommandFillPotionMoney[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, "%s %s", gMessage.GetMessage(35), gMessage.GetMessage(32));
		return;
	}

	CItem item;
	BYTE btItemPos;
	int PotionCount = 0;
	int totalCost = 0;
	int item_type = GET_ITEM(14, 3);
	item.m_Level = 0;
	item.m_Durability = 255;

	item.Convert(item_type, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF);

	for (int i = INVENTORY_WEAR_SIZE; i < INVENTORY_MAIN_SIZE; i++)
	{
		if (gServerInfo.m_CommandFillPotionMaxAmount[lpObj->AccountLevel] > 0 && PotionCount > gServerInfo.m_CommandFillPotionMaxAmount[lpObj->AccountLevel])
		{
			break;
		}

		//if (gItemManager.InventoryInsertItemStack(lpObj, &item) == 0)
		//{
			btItemPos = gItemManager.InventoryInsertItem(lpObj->Index, item);

			if (btItemPos == 0xFF)
				continue;

			PotionCount++;
		//}
	}

	gItemManager.GCItemListSend(lpObj->Index);

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(36));

	gLog.Output(LOG_COMMAND, "[CommandFillPotion][%s][%s] - (%d)", lpObj->Account, lpObj->Name, PotionCount);
}


void CCommandManager::CommandAntilag(LPOBJ lpObj, char* arg)
{
	char name[5] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	if (strcmp(name, "full") == 0)
	{
		lpObj->Antilag[0] = true;
		lpObj->Antilag[1] = true;
		lpObj->Antilag[2] = true;
		lpObj->Antilag[3] = true;
		GCMessagePopupSend(lpObj, "Sistema antilag ATIVADO no modo FULL!\nPode ser necessario renovar a tela para ver os efeitos.");
		gLog.Output(LOG_COMMAND, "[CommandAntilag][%s][%s] - (TimerRandom: full)", lpObj->Account, lpObj->Name);
		return;
	}

	int level = GetNumber(arg, 0, 0);

	if (level < 0 || level > 4)
	{
		GCMessagePopupSend(lpObj, "Numero invalido para o sistema de antilag.\nDigite um numero de 0 a 4.");
	}

	if (level == 0)
	{
		GCMessagePopupSend(lpObj, "Sistema antilag DESATIVADO.\nPode ser necessario renovar a tela para ver os efeitos.");
		lpObj->Antilag[0] = false;
		lpObj->Antilag[1] = false;
		lpObj->Antilag[2] = false;
		lpObj->Antilag[4] = false;
		return;
	}
	else
	{
		if (lpObj->Antilag[level - 1])
		{
			GCMessagePopupSend(lpObj, "Sistema antilag tipo %d DESATIVADO!\nPode ser necessario renovar a tela para ver os efeitos.", level);
			lpObj->Antilag[level - 1] = false;
		}
		else
		{
			GCMessagePopupSend(lpObj, "Sistema antilag tipo %d ATIVADO!\nPode ser necessario renovar a tela para ver os efeitos.", level);
			lpObj->Antilag[level - 1] = true;
		}		
	}

	gLog.Output(LOG_COMMAND, "[CommandAntilag][%s][%s] - (TimerRandom: %d)", lpObj->Account, lpObj->Name, level);
}

void CCommandManager::CommandMoveGuild(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	GUILD_INFO_STRUCT* lpGuildInfo = gGuildClass.SearchGuild(name);

	if (lpGuildInfo == NULL)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int M = GetNumber(arg, 1, lpObj->Map);
	int X = GetNumber(arg, 2, lpObj->X);
	int Y = GetNumber(arg, 3, lpObj->Y);

	for (int i = 0; i < MAX_GUILD_USER; i++)
	{
		if (lpGuildInfo->Index[i] >= 0)
		{
			gObjTeleport(lpGuildInfo->Index[i], M, X++, Y);
		}
	}

	gLog.Output(LOG_COMMAND, "[CommandMoveGuild][%s][%s] - (Guild: %s)", lpObj->Account, lpObj->Name, name);
}

void CCommandManager::CommandGuildDisconnect(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	GUILD_INFO_STRUCT* lpGuildInfo = gGuildClass.SearchGuild(name);

	if (lpGuildInfo == NULL)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	for (int i = 0; i < MAX_GUILD_USER; i++)
	{
		if (lpGuildInfo->Index[i] >= 0)
		{
			gObjUserKill(lpGuildInfo->Index[i]);
		}
	}

	gLog.Output(LOG_COMMAND, "[CommandGuildDisconnect][%s][%s] - (Guild: %s)", lpObj->Account, lpObj->Name, name);
}

void CCommandManager::CommandPin(LPOBJ lpObj, char* arg)
{
	char pin[11] = { 0 };

	this->GetString(arg, pin, sizeof(pin), 0);

	if (gObjCheckPersonalCode(lpObj->Index, pin))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(38));
		PMSG_PIN_AUTH pinSend;
		pinSend.h.set(0xF3, 0xE5, sizeof(pinSend));
		ZeroMemory(pinSend.pin, 8);
		memcpy(&pinSend.pin, pin, 7);
		pinSend.pin[7] = '\0';
		lpObj->PersonalCodeChecked = true;
		DataSend(lpObj->Index, (LPBYTE)&pinSend, pinSend.h.size);
	}
	else
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(39));
	}

	gLog.Output(LOG_COMMAND, "[CommandPin][%s][%s] - (Pin: %d)", lpObj->Account, lpObj->Name, pin);	
}

void CCommandManager::CommandRuud(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(90));
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(14));
		return;
	}

	int ruud = GetNumber(arg, 0, 0);

	if (lpObj->Ruud < ruud)
	{
		GCMessagePopupSend(lpObj, "Voce nao tem essa quantidade de Ruud no momento.");
		return;
	}

	if (lpObj->PersonalCodeChecked == false)
	{
		GCMessagePopupSend(lpObj, "Voce nao pode criar baus de Ruud sem antes autenticar o PIN. Utilize o comando /pin seguido de seu PIN para faze-lo agora mesmo.");
		return;
	}
	
	int item_type = -1;
	bool inventory = false;

	switch (ruud)
	{	
	case 1000:
		item_type = GET_ITEM(14, 352);
		break;
	case 10000:
		item_type = GET_ITEM(14, 336);
		break;
	case 50000:
		item_type = GET_ITEM(14, 289);
		break;
	case 100000:
		item_type = GET_ITEM(14, 288);
		break;
	case 200000:
		item_type = GET_ITEM(14, 287);
		break;
	case 500000:
		item_type = GET_ITEM(14, 356);
		break;
	default:
		GCMessagePopupSend(lpObj, "Valor inválido. Tente 1000, 10000, 100000, 200000 ou 500000");
		break;
	}

	if (item_type > -1)
	{
		CItem item;
		BYTE btItemPos = 0xFF;		
		item.Convert(item_type, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF);

		for (int i = INVENTORY_WEAR_SIZE; i < gItemManager.GetInventoryMaxValue(lpObj); i++)
		{
			btItemPos = gItemManager.InventoryInsertItem(lpObj->Index, item);

			if (btItemPos == 0xFF)
				continue;

			gItemManager.GCItemListSend(lpObj->Index);
			lpObj->Ruud -= ruud;
			gRheaGold.GC_Notify_RuudUpdate(lpObj->Index, lpObj->Ruud, ruud, 0);

			gLog.Output(LOG_COMMAND, "[CommandRuud][%s][%s] - (Ruud: %d)", lpObj->Account, lpObj->Name, ruud);

			return;
		}
	}	
}

void CCommandManager::CommandSetLevel(LPOBJ lpObj, char* arg)
{
	/*int dmg = GetNumber(arg, 0, 0);
	GCDamageSend(lpObj->Index, lpObj->Index, 0, dmg, dmg, 0);
	
	return;*/

	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);
	int level = GetNumber(arg, 1, 400);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	

	lpTarget->Level = level;

	GCLevelUpSend(lpTarget);

	gLog.Output(LOG_COMMAND, "[CommandSetLevel][%s][%s] - (Char: %s | TimerRandom: %d)", lpObj->Account, lpObj->Name, name, level);
}

void CCommandManager::CommandSetMLevel(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);
	int level = GetNumber(arg, 1, gServerInfo.m_MasterSkillTreeMaxLevel);
	int pontos = GetNumber(arg, 2, level);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	lpTarget->MasterLevel = level;
	lpTarget->MasterPoint = pontos;

	gMasterSkillTree.GCMasterLevelUpSend(lpTarget);
	gMasterSkillTree.GCMasterInfoSend(lpTarget);

	gLog.Output(LOG_COMMAND, "[CommandSetMasterLevel][%s][%s] - (Char: %s | MLevel: %d | Pontos: %d)", lpObj->Account, lpObj->Name, name, level, pontos);
}

void CCommandManager::CommandSetReset(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);
	int reset = GetNumber(arg, 1, 400);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	lpTarget->Reset = reset;
	gObjectManager.CharacterCalcAttribute(lpTarget->Index);
	GCNewCharacterInfoSend(lpTarget);

	gLog.Output(LOG_COMMAND, "[CommandSetResets][%s][%s] - (Char: %s | Resets: %d)", lpObj->Account, lpObj->Name, name, reset);
}

void CCommandManager::CommandRemoveWingOption(LPOBJ lpObj, char* arg)
{
	int option = GetNumber(arg, 0);

	if (option < 1 || option > 4)
	{
		GCMessagePopupSend(lpObj, "Argumento inválido para este comando.\nDigite o comando seguido do numero do option (1 a 4) o qual deseja remover");
		return;
	}

	if (lpObj->Inventory[INVENTORY_WEAR_SIZE].IsItem() == 0)
	{
		GCMessagePopupSend(lpObj, "Coloque sua asa ou capa level 4 no primeiro espaço do inventario para utilizar este comando.");
		return;
	}

	CItem olditem = lpObj->Inventory[INVENTORY_WEAR_SIZE];
	CItem* lpItem = &lpObj->Inventory[INVENTORY_WEAR_SIZE];

	if (lpItem->m_Index < GET_ITEM(12, 414) || lpItem->m_Index > GET_ITEM(12, 421))
	{
		GCMessagePopupSend(lpObj, "Coloque sua uma asa ou capa level 4 no primeiro espaço do inventario para utilizar este comando.");
		return;
	}

	if (lpItem->m_SocketOption[option-1] == 0xFF || lpItem->m_SocketOption[option-1] == 0xFE)
	{
		GCMessagePopupSend(lpObj, "Nao existe option na posicao informada.");
		return;
	}

	if (lpObj->Ruud < gServerInfo.m_CommandRemoveWingOptionRuud)
	{
		GCMessagePopupSend(lpObj, "Você não a quantidade de Ruud necessaria no momento (%d).", gServerInfo.m_CommandRemoveWingOptionRuud);
		return;
	}

	lpObj->Ruud -= gServerInfo.m_CommandRemoveWingOptionRuud;
	gRheaGold.GC_Notify_RuudUpdate(lpObj->Index, lpObj->Ruud, gServerInfo.m_CommandRemoveWingOptionRuud, 0);

	gLog.Output(LOG_COMMAND, "[CommandRemoveWingOption][%s][%s] - (Ruud: %d, Option: %d, Old: %d)", lpObj->Account, lpObj->Name, gServerInfo.m_CommandRemoveWingOptionRuud, option, lpItem->m_SocketOption[option - 1]);

	lpItem->m_SocketOption[option - 1] = 0xFF;

	gItemManager.GCItemModifySend(lpObj->Index, INVENTORY_WEAR_SIZE);

	CItem newitem = lpObj->Inventory[INVENTORY_WEAR_SIZE];

	GDItemTrackingSaveSend(olditem, newitem, lpObj->Account, lpObj->Account, 4, 4);

	GCMessagePopupSend(lpObj, "Option %d removido com sucesso.",option);
}

void CCommandManager::CommandSumiu(LPOBJ lpObj, char* arg)
{
	int classe = GetNumber(arg, 0);
	bool found = false;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer[n].state != 0 && lpObj->VpPlayer[n].type == OBJECT_MONSTER)
		{
			LogAdd(LOG_DEBUG, "VP de %s: %d (Live: %d)", lpObj->Name, gObj[lpObj->VpPlayer[n].index].Class, gObj[lpObj->VpPlayer[n].index].Live);
		}
	}

	for (int n = OBJECT_START_MONSTER; n < MAX_OBJECT_MONSTER; n++)
	{
		if (gObj[n].Connected != OBJECT_ONLINE || n == lpObj->Index)
		{
			continue;
		}

		if (gObj[n].State != OBJECT_CREATE && gObj[n].State != OBJECT_PLAYING)
		{
			continue;
		}

		if (gObj[n].Map == lpObj->Map && gObj[n].Class == classe)
		{
			found = true;
			GCMessagePopupSend(lpObj, "Localizado em (%d,%d)\nLive: %d", gObj[n].X, gObj[n].Y, gObj[n].Live);
			LogAdd(LOG_DEBUG, "Classe %d no Mapa %d por %s (Live: %d)", classe, lpObj->Map, lpObj->Name, gObj[n].Live);
		}		
	}

	if (!found)
	{
		GCMessagePopupSend(lpObj, "Monstro nao encontrado =(");
	}
}

void CCommandManager::CommandRebuildSkillTree(LPOBJ lpObj, char* arg)
{
	if (lpObj->AccountLevel < 1)
	{
		GCMessagePopupSend(lpObj, "Comando disponivel apenas para contas VIP");
		return;
	}

	int type = GetNumber(arg, 0, 9);

	MASTER_SKILL_TREE_INFO MasterSkillTreeInfo;

	if (type == 1)
	{
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			if (gMasterSkillTree.GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				if (MasterSkillTreeInfo.Group == 1)
				{
					for (int i = 0; i < MAX_SKILL_LIST; i++)
					{
						if (lpObj->Skill[i].m_skill == lpObj->MasterSkill[n].m_skill)
						{
							lpObj->Skill[i].m_index = lpObj->Skill[i].m_skill;
						}
					}
					lpObj->MasterPoint += lpObj->MasterSkill[n].m_level + 1;
					lpObj->MasterSkill[n].Clear();
				}
			}
		}
	}
	else if (type == 2)
	{
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			if (gMasterSkillTree.GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				if (MasterSkillTreeInfo.Group == 2)
				{
					for (int i = 0; i < MAX_SKILL_LIST; i++)
					{
						if (lpObj->Skill[i].m_skill == lpObj->MasterSkill[n].m_skill)
						{
							lpObj->Skill[i].m_index = lpObj->Skill[i].m_skill;
						}
					}
					lpObj->MasterPoint += lpObj->MasterSkill[n].m_level + 1;
					lpObj->MasterSkill[n].Clear();
				}
			}
		}
	}
	else if (type == 3)
	{
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			if (gMasterSkillTree.GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				if (MasterSkillTreeInfo.Group == 3)
				{
					for (int i = 0; i < MAX_SKILL_LIST; i++)
					{
						if (lpObj->Skill[i].m_skill == lpObj->MasterSkill[n].m_skill)
						{
							lpObj->Skill[i].m_index = lpObj->Skill[i].m_skill;
						}
					}
					lpObj->MasterPoint += lpObj->MasterSkill[n].m_level + 1;
					lpObj->MasterSkill[n].Clear();
				}
			}
		}
	}
	else if (type == 0)
	{
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			if (gMasterSkillTree.GetInfo(lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo) != 0)
			{
				for (int i = 0; i < MAX_SKILL_LIST; i++)
				{
					if (lpObj->Skill[i].m_skill == lpObj->MasterSkill[n].m_skill)
					{
						lpObj->Skill[i].m_index = lpObj->Skill[i].m_skill;
					}
				}
				lpObj->MasterPoint += lpObj->MasterSkill[n].m_level + 1;
				lpObj->MasterSkill[n].Clear();
			}
		}
	}
	else
	{
		GCMessagePopupSend(lpObj, "Voce digitou uma opcao invalida.\nDigite o comando seguido de 1, 2, 3 ou 0, sendo:\n1 para resetar a parte esquerda da arvore;\n2 para resetar a parte do meio;\n3 para resetar a parte da direita; ou\n0 para resetar a arvore completa.");
		return;
	}

	GCMessagePopupSend(lpObj, "Arvore de skills resetada.\nSeu personagem precisa se recarregado. Aguarde...");

	GDCharacterInfoSaveSend(lpObj->Index);
	//GCCloseClientSend(lpObj->Index, 1);
	gObjectManager.CharacterGameCloseSet(lpObj->Index, 1);
}