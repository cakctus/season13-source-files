#include "stdafx.h"
#include "MUFC.h"
#include "MemScript.h"
#include "ServerInfo.h"
#include "Notice.h"
#include "Util.h"
#include "EffectManager.h"
#include "Protocol.h"
#include "CommandManager.h"

CMUFC gMUFC;

CMUFC::CMUFC()
{
	gMUFC.Status = MUFC_IDLE;
}

CMUFC::~CMUFC()
{
	ZeroMemory(gMUFC.FighterName,sizeof(gMUFC.FighterName));
	gMUFC.FighterID[0] = 0;
	gMUFC.FighterID[1] = 0;
	gMUFC.FighterVictories[0] = 0;
	gMUFC.FighterVictories[1] = 0;
	gMUFC.Rounds = 1;
}

void CMUFC::FightStart(LPOBJ lpObj, int rounds)
{
	if(gMUFC.FighterName[0] != NULL && gMUFC.FighterName[1] != NULL)
	{
		if(strlen(gMUFC.FighterName[0]) > 3 && strlen(gMUFC.FighterName[1]) > 3)
		{
			LPOBJ Fighter1Obj = &gObj[gMUFC.FighterID[0]];
			LPOBJ Fighter2Obj = &gObj[gMUFC.FighterID[1]];

			PMSG_CLOSE_CLIENT_RECV pMsg;
			pMsg.header.set(0xF3, 0xE8, sizeof(pMsg));
			DataSend(Fighter1Obj->Index, (BYTE*)&pMsg, pMsg.header.size);
			DataSend(Fighter2Obj->Index, (BYTE*)&pMsg, pMsg.header.size);

			gCommandManager.CommandShowStats(lpObj, gMUFC.FighterName[0]);
			gCommandManager.CommandShowStats(lpObj, gMUFC.FighterName[1]);

			gMUFC.SendNotice("[MUFC]  INICIANDO LUTA  [MUFC]");
			gMUFC.SendNotice("[MUFC]  %s  vs  %s  [MUFC]", gMUFC.FighterName[0], gMUFC.FighterName[1]);
			
			gMUFC.Status = MUFC_PREPARED;
			gMUFC.StartCounter	= gServerInfo.MUFC_TimerToStart;

			int wtf = 0;

			gMUFC.Rounds = rounds;

			wtf = 1;

			gMUFC.FighterVictories[0] = 0;
			gMUFC.FighterVictories[1] = 0;

			wtf = 2;

			gMUFC.ReSetTimer = 0;

			return;
		}
	}

	GCMessagePopupSend(lpObj, "Falha ao iniciar o MUFC.\nCertifique-se de ter selecionado os dois lutadores.");
}

void CMUFC::ResetFight(LPOBJ lpObj)
{
	if(gMUFC.Status != MUFC_STARTED)
	{
		GCMessagePopupSend(lpObj, "Luta ainda iniciada nao pode reiniciar.");
		return;
	}
	
	if ( &gObj[gMUFC.FighterID[0]] == NULL || &gObj[gMUFC.FighterID[1]] == NULL )
	{
		GCMessagePopupSend(lpObj, "Personagem(ns) nao encontrado(s). Inicie a luta novamente");
		return;
	}

	gMUFC.Status = MUFC_PREPARED;

	LPOBJ Fighter1Obj = &gObj[gMUFC.FighterID[0]];
	LPOBJ Fighter2Obj = &gObj[gMUFC.FighterID[1]];

	gMUFC.PrepareFighter(1,Fighter1Obj,lpObj);
	gMUFC.PrepareFighter(2,Fighter2Obj,lpObj);
}

void CMUFC::FightRun()
{
	LPOBJ Fighter1Obj = &gObj[gMUFC.FighterID[0]];
	LPOBJ Fighter2Obj = &gObj[gMUFC.FighterID[1]];

	gMUFC.Status = MUFC_STARTED;

	gMUFC.SendNotice("[MUFC] !!!  L U T E M  !!! [MUFC]");

	gEffectManager.DelEffect(Fighter1Obj, EFFECT_ICE_ARROW);
	gEffectManager.DelEffect(Fighter2Obj, EFFECT_ICE_ARROW);

	gMUFC.FightTimer = 0;
	gMUFC.FightDamage = 0;
}

void CMUFC::FighterKilled(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	gMUFC.SendNotice("%s bateu %d com %d hits", gMUFC.FighterName[0], gMUFC.FighterDamage[0], gMUFC.FighterHits[0]);
	gMUFC.SendNotice("%s bateu %d com %d hits", gMUFC.FighterName[1], gMUFC.FighterDamage[1], gMUFC.FighterHits[1]);
	gMUFC.SendNotice("[MUFC]   Vitoria de %s   [MUFC]",lpObj->Name);

	int fighterId = lpObj->MUFCFighter - 1;

	gMUFC.FighterVictories[fighterId]++;

	if (gMUFC.Rounds > 1)
	{
		gMUFC.SendNotice("[%s] %d x %d [%s]", gMUFC.FighterName[0], gMUFC.FighterVictories[0], gMUFC.FighterVictories[1], gMUFC.FighterName[1]);
	}		

	if(gMUFC.FighterVictories[fighterId] < gMUFC.Rounds)
	{
		gMUFC.Status = MUFC_MOVED;
		gMUFC.SendNotice("[MUFC] Proximo round em %d segundos. [MUFC]",gServerInfo.MUFC_TimerRound);
		gMUFC.ReSetTimer = gServerInfo.MUFC_TimerRound;
	}
	else
	{
		GCFireworksSend(lpObj, lpObj->X, lpObj->Y);

		lpTargetObj->MUFCFighter = 0;
		lpObj->MUFCFighter = 0;
		LogAdd(LOG_BLUE, "Zerou saporra");

		lpTargetObj->PKLevel = 3;
		lpObj->PKLevel = 3;
		GCPKLevelSend(lpTargetObj->Index, 3);
		GCPKLevelSend(lpObj->Index, 3);

		ZeroMemory(gMUFC.FighterName,sizeof(gMUFC.FighterName));

		gMUFC.FighterID[0] = 0;
		gMUFC.FighterID[1] = 0;

		gMUFC.Status = MUFC_FINISHED;
		gMUFC.FightTimer = 0;
		gMUFC.FightDamage = 0;

		gObjTeleport(lpTargetObj->Index, 6, 70, 42);
		gObjTeleport(lpObj->Index, 6, 70, 41);

		PMSG_CLOSE_CLIENT_RECV pMsg;
		pMsg.header.set(0xF3, 0xE8, sizeof(pMsg));
		DataSend(lpTargetObj->Index, (BYTE*)&pMsg, pMsg.header.size);
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
}

void CMUFC::PrepareFighter(int number, LPOBJ lpTargetObj, LPOBJ lpObj)
{
	int x = gServerInfo.MUFC_1_X;
	int y = gServerInfo.MUFC_1_Y;

	if(number == 2)
	{
		x = gServerInfo.MUFC_2_X;
		y = gServerInfo.MUFC_2_Y;
	}
	
	gObjTeleport(lpTargetObj->Index, gServerInfo.MUFC_Map, x, y);

	int wtf = 0;
	wtf = 9999;

	LogAdd(LOG_BLUE, "MUFC %d %s", number, lpTargetObj->Name);
	lpTargetObj->MUFCFighter = number;
	LogAdd(LOG_BLUE, "MUFC %d %s", lpTargetObj->MUFCFighter, lpTargetObj->Name);

	wtf = 1;

	lpTargetObj->PKLevel = 6;
	GCPKLevelSend(lpTargetObj->Index, 6);

	gEffectManager.ClearAllEffect(lpTargetObj);
	gEffectManager.AddEffect(lpTargetObj, 0, EFFECT_ICE_ARROW, 9999, 0, 0, 0, 0);

	lpTargetObj->PathCount = 0;
	lpTargetObj->PathStartEnd = 0;
	gObjSetPosition(lpTargetObj->Index, x, y);

	strcpy(gMUFC.FighterName[number-1],lpTargetObj->Name);
	gMUFC.FighterName[number-1][10] = '\0';

	gMUFC.FighterID[number-1] = lpTargetObj->Index;

	gMUFC.FighterDamage[number-1] = 0;
	gMUFC.FighterHits[number-1] = 0;
	
	//gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Lutador %s esta com %d em Vitalidade",lpTargetObj->Name,lpTargetObj->Vitality);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "[%s]: [F: %d] [A: %d] [V: %d] [E: %d] [C: %d]", lpTargetObj->Name, lpTargetObj->Strength, lpTargetObj->Dexterity, lpTargetObj->Vitality, lpTargetObj->Energy, lpTargetObj->Leadership);
	
	gMUFC.Status = MUFC_MOVED;
}

void CMUFC::SetNewRound()
{
	LPOBJ lpObj = &gObj[gMUFC.FighterID[0]];
	LPOBJ lpTargetObj = &gObj[gMUFC.FighterID[1]];
	
	gObjClearViewport(lpObj);
	gObjClearViewport(lpTargetObj);

	gObjTeleport(lpObj->Index, gServerInfo.MUFC_Map, gServerInfo.MUFC_1_X, gServerInfo.MUFC_1_Y);
	gObjTeleport(lpTargetObj->Index, gServerInfo.MUFC_Map, gServerInfo.MUFC_2_X, gServerInfo.MUFC_2_Y);

	gEffectManager.ClearAllEffect(lpTargetObj);
	gEffectManager.ClearAllEffect(lpObj);

	gEffectManager.AddEffect(lpObj, 0, EFFECT_ICE_ARROW, 9999, 0, 0, 0, 0);
	lpObj->PathCount = 0;
	lpObj->PathStartEnd = 0;
	gObjSetPosition(lpObj->Index, gServerInfo.MUFC_1_X, gServerInfo.MUFC_1_Y);

	gEffectManager.AddEffect(lpTargetObj, 0, EFFECT_ICE_ARROW, 9999, 0, 0, 0, 0);
	lpTargetObj->PathCount = 0;
	lpTargetObj->PathStartEnd = 0;
	gObjSetPosition(lpTargetObj->Index, gServerInfo.MUFC_2_X, gServerInfo.MUFC_2_Y);

	gObjViewportListProtocolCreate(lpObj);
	gObjViewportListProtocolCreate(lpTargetObj);		

	gMUFC.FighterDamage[0] = 0;
	gMUFC.FighterDamage[1] = 0;

	gMUFC.FighterHits[0] = 0;
	gMUFC.FighterHits[1] = 0;

	gMUFC.Status = MUFC_PREPARED;
	gMUFC.StartCounter	= gServerInfo.MUFC_TimerToStart + 5;
}

void CMUFC::Load(char* filename)
{
	for( int n = 0; n < 32; n++ )
	{
		gMUFC.FightDamageRate[n].Timer		= -1;
		gMUFC.FightDamageRate[n].DamageRate	= -1;
	}

	CMemScript* lpScript = new(std::nothrow) CMemScript;

	if(lpScript == NULL)
	{
		return;
	}

	if(lpScript->SetBuffer(filename) == 0)
	{
		delete lpScript;
		return;
	}

	int count = 0;

	try
	{
		while(true)
		{
			if( lpScript->GetToken() == 2 )
			{
				break;
			}

			gMUFC.FightDamageRate[count].Timer		= lpScript->GetNumber();
			gMUFC.FightDamageRate[count].DamageRate = lpScript->GetAsNumber();

			if(count >= 31)
				break;

			count++;
		}
	}
	catch(...)
	{
		delete lpScript;
		return;
	}

	LogAdd(LOG_BLACK, "[MUFC] MUFCDamageIncrease is loaded");

	delete lpScript;
}

void CMUFC::SendNotice(char* message, ...)
{
	char buff[256] = { 0 };

	va_list arg;
	va_start(arg, message);
	vsprintf_s(buff, message, arg);
	va_end(arg);

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
		if (gObjIsConnectedGP(n) != 0)
			if(gObj[n].Authority == 32 || gObj[n].MUFCFighter > 0)
				gNotice.GCNoticeSend(n, 0, 0, 0, 0, 0, 0, buff);
}

void CMUFC::CalcWeaponOption(LPOBJ lpObj, CItem* lpItem) // OK
{
	if (lpItem->m_Index != GET_ITEM(0, 51) && //Sword BK e MG
		lpItem->m_Index != GET_ITEM(0, 60) && //Claw RF
		lpItem->m_Index != GET_ITEM(2, 25) && //Scepter DL
		lpItem->m_Index != GET_ITEM(3, 26) && //Lance GL
		lpItem->m_Index != GET_ITEM(4, 30) && //Bow Elf
		lpItem->m_Index != GET_ITEM(5, 49) && //Staff DW
		lpItem->m_Index != GET_ITEM(5, 50)    //Stick SU
		)
	{
		return;
	}

	lpObj->DecBossHitDelay += gServerInfo.MASTERY_OPTION_BOSS_DECREASE_DELAY_2;
	lpObj->AddElementalDamageRate += gServerInfo.MASTERY_OPTION_ELEMENTAL_DAMAGE_INCREASE_WEAPON_2;
	lpObj->AddBossDamage += gServerInfo.MASTERY_OPTION_BOSS_DAMAGE_INCREASE_2;
	lpObj->AddPVPDamageRate += gServerInfo.MASTERY_OPTION_PVP_DAMAGE_INCREASE_2;
	lpObj->m_MPSkillOpt.DoubleDamageRate += gServerInfo.MASTERY_OPTION_DOUBLE_DAMAGE_RATE_2;
	lpObj->m_MPSkillOpt.TripleDamageRate += gServerInfo.MASTERY_OPTION_TRIPLE_DAMAGE_RATE_2;
	lpObj->TrueDamageRate += gServerInfo.MASTERY_OPTION_TRUE_DAMAGE_RATE_2;
	lpObj->UltraHit30perChance += gServerInfo.MASTERY_OPTION_30P_HP_ULTRAHIT_2;

	lpObj->UltraHit50perChance += gServerInfo.MASTERY_BONUS_ULTRAHIT_50_WEAPON_2;
	lpObj->ExcellentDamageOnly = true;
	lpObj->SubPlusDamageReflect += gServerInfo.MASTERY_BONUS_REFLECT_DECREASE_2;
	lpObj->CureHarmRate += gServerInfo.MASTERY_BONUS_CURE_HARM_2;
	lpObj->SternRate += gServerInfo.MASTERY_BONUS_STERN_RATE_2;
	lpObj->BleedingRate += gServerInfo.MASTERY_BONUS_BLEEDING_2;
	lpObj->BossDoubleDamageRate += gServerInfo.MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON_2;
	lpObj->FreezingDamageRate += gServerInfo.MASTERY_BONUS_FREEZING_DAMAGE;
}