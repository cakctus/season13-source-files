#include "StdAfx.h"
#include "CheatGuard.h"
#include "user.h"
#include "GameMain.h"
#include "MemScript.h"
#include "ServerInfo.h"
#include "SpeedTable.h"
#include "Util.h"
#include "Log.h"
#include "ObjectManager.h"

CheatGuard gCheatGuard;

CheatGuard::CheatGuard()
{
}

CheatGuard::~CheatGuard()
{
}

void CheatGuard::HardIdRecv(int aIndex, HARDWARE_ID_RECV * pResult)
{
	HARDWARE_ID_SEND_DB pMsg;
	pMsg.header.set(0xD7, 10, sizeof(HARDWARE_ID_SEND_DB));
	strcpy(pMsg.Account,gObj[aIndex].Account);
	pMsg.Account[10] = '\0';
	pResult->HarwareId[44] = '\0';

	strcpy(pMsg.HardwareId,pResult->HarwareId);
	pMsg.HardwareId[44] = '\0';

	memcpy(&gObj[aIndex].HardwareId, &pMsg.HardwareId, 45);
	gObj[aIndex].HardwareId[44] = '\0';

	LogAdd(LOG_BLACK, "[HardwareInfo] %s :: %s", gObj[aIndex].Name, gObj[aIndex].HardwareId);

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

void CheatGuard::PingStack(int aIndex, int ping)
{
	if (ping >= 5 && ping < 1000)
	{
		int pingSum = 0;
		int pingCount = 1;

		for (int i = 0; i < 9; i++)
		{
			if (gObj[aIndex].PingLast[i + 1] > 0)
			{
				gObj[aIndex].PingLast[i] = gObj[aIndex].PingLast[i + 1];
				pingSum += gObj[aIndex].PingLast[i];
				pingCount++;
			}
		}

		gObj[aIndex].PingLast[9] = ping;
		pingSum += ping;

		gObj[aIndex].Ping = (int)(pingSum / pingCount);
	}
}

void CheatGuard::GCSendPing(int aIndex)
{
	char log[1024] = "vai bugar a pqp";
	char log2[1024] = "vai bugar a pqp";
	wsprintf(log, "%s", log2);

	PMSG_PING pMsg;
	pMsg.h.set(0x71, sizeof(pMsg));
	gObj[aIndex].PingSentTick = GetTickCount();
	pMsg.ping = gObj[aIndex].Ping;
	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}

void CheatGuard::GCPingSendRecv(PMSG_PING* aRecv, int aIndex)
{
	int LastPing = (GetTickCount() - gObj[aIndex].PingSentTick);
	this->PingStack(aIndex, LastPing);
}

void CheatGuard::SpeedInformationRecv(PMSG_HACKLOG * lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)

	{
		return;
	}

	//ataques comparar com tabela de pvp
	if (lpMsg->stucks >= 5/* || (lpMsg->durationSkillDelay > 0 && lpMsg->durationSkillDelay < 35) || (lpMsg->attackSkillDelay > 0 && lpMsg->attackSkillDelay < 35) || (lpMsg->attackDelay > 0 && lpMsg->attackDelay < 35)*/)
	{
		GCMessagePopupSend(&gObj[aIndex], "Estamos te desconectando do jogo por aceleração de movimentos.\nRegistros foram criados, e em caso de insistencia sua conta e/ou computador poderao sofrer bloqueio permanente.");
		gLog.Output(LOG_HACK, "[HackCheck][%s][%s] Speed moving client side [%d]", lpObj->Account, lpObj->Name, lpMsg->stucks);
		gLog.Output(LOG_HACK, "[HackCheck][%s][%s] Speed attack client side [%d][%d][%d]", lpObj->Account, lpObj->Name, lpMsg->durationSkillDelay, lpMsg->attackSkillDelay, lpMsg->attackDelay);
		gObjUserKill(aIndex);
	}	
	
	//LogAdd(LOG_RED, "[%s][%s] Stuck: %d | duration: %d | attackSkill: %d | attack: %d", lpObj->Account, lpObj->Name, lpMsg->stucks, lpMsg->durationSkillDelay, lpMsg->attackSkillDelay, lpMsg->attackDelay);
}