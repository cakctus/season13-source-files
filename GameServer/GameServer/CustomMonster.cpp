#include "stdafx.h"
#include "CustomMonster.h"
#include "DSProtocol.h"
#include "MemScript.h"
#include "Message.h"
#include "Monster.h"
#include "Notice.h"
#include "User.h"
#include "Util.h"
#include "Viewport.h"
#include "ServerInfo.h"
#include "ObjectManager.h"
#include "RheaGold.h"
#include "NpcTalk.h"

CCustomMonster gCustomMonster;

CCustomMonster::CCustomMonster() // OK
{
	this->m_CustomMonsterInfo.clear();
}

CCustomMonster::~CCustomMonster() // OK
{

}

void CCustomMonster::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_CustomMonsterInfo.clear();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			CUSTOM_MONSTER_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.MapNumber = lpMemScript->GetAsNumber();

			info.MaxLife	= lpMemScript->GetAsFloatNumber();

			info.Element = lpMemScript->GetAsNumber();

			info.MinDmgMakePoints	= lpMemScript->GetAsNumber();
			info.MinDmgMakePerc		= lpMemScript->GetAsNumber();
			info.MaxDmgMakePerc		= lpMemScript->GetAsNumber();
			info.MinDmgGetPoints	= lpMemScript->GetAsNumber();
			info.MaxDmgGetPoints	= lpMemScript->GetAsNumber();

			info.HitDelay			= lpMemScript->GetAsNumber();

			info.PlayerNumber = lpMemScript->GetAsNumber();

			info.TimerRandom	= lpMemScript->GetAsNumber();
			info.Points			= lpMemScript->GetAsNumber();
			info.Resets			= lpMemScript->GetAsNumber();
			info.MinVipResets	= lpMemScript->GetAsNumber();
			info.Ruud			= lpMemScript->GetAsNumber();
			info.WcoinP			= lpMemScript->GetAsNumber();
			info.WcoinC			= lpMemScript->GetAsNumber();
			info.MinVIPCoin		= lpMemScript->GetAsNumber();

			strcpy_s(info.Rank, lpMemScript->GetAsString());

			info.RankColor = lpMemScript->GetAsNumber();

			this->m_CustomMonsterInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CCustomMonster::SetCustomMonsterInfo(LPOBJ lpObj) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(lpObj->Class,lpObj->Map,&CustomMonsterInfo) == 0)
	{
		lpObj->isBoss = 0;
		return;
	}

	lpObj->isBoss = 1;

	if (CustomMonsterInfo.Element == -1)
		lpObj->ElementalAttribute = (GetLargeRand() % 5) + 1;
	else
		lpObj->ElementalAttribute = CustomMonsterInfo.Element;
	
	for (int i = 0; i < MAX_OBJECT_USER; i++)
	{
		lpObj->LastBossHit[i] = GetTickCount();
	}

	lpObj->bossHitDelay = CustomMonsterInfo.HitDelay;

	lpObj->bossMinDmgMakePoints = CustomMonsterInfo.MinDmgMakePoints;
	lpObj->bossMinDmgMakePerc = CustomMonsterInfo.MinDmgMakePerc;
	lpObj->bossMaxDmgMakePerc = CustomMonsterInfo.MaxDmgMakePerc;

	lpObj->bossMinDmgGetPoints = CustomMonsterInfo.MinDmgGetPoints;
	lpObj->bossMaxDmgGetPoints = CustomMonsterInfo.MaxDmgGetPoints;

	lpObj->Life			 *= CustomMonsterInfo.MaxLife;
	lpObj->MaxLife		 *= CustomMonsterInfo.MaxLife;
	lpObj->ScriptMaxLife *= CustomMonsterInfo.MaxLife;

	/*if (CustomMonsterInfo.TimerRandom > 0)
	{
		lpObj->MaxRegenTime = ((GetLargeRand() % CustomMonsterInfo.TimerRandom * 1000) + lpObj->OriginalRegenTime);
		LogAdd(LOG_DEBUG, "Setando regen para %d", lpObj->MaxRegenTime / 1000);
	}*/

	/*lpObj->ElementalDefense = (__int64)((float)lpObj->ElementalDefense * CustomMonsterInfo.Defense);

	lpObj->ElementalDamageMin			= (__int64)((float)lpObj->ElementalDamageMin * CustomMonsterInfo.Damage);
	lpObj->ElementalDamageMax			= (__int64)((float)lpObj->ElementalDamageMax * CustomMonsterInfo.Damage);

	lpObj->ElementalAttackSuccessRate	= (__int64)((float)lpObj->ElementalAttackSuccessRate * CustomMonsterInfo.AttackRate);
	lpObj->ElementalDefenseSuccessRate	= (__int64)((float)lpObj->ElementalDefenseSuccessRate * CustomMonsterInfo.DefenseRate);
	*/
}

void CCustomMonster::MonsterDieProc(LPOBJ lpObj /*monster*/ , LPOBJ lpTarget /*player*/) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(lpObj->Class,lpObj->Map,&CustomMonsterInfo) == 0)
	{
		return;
	}

	int sortedCount = gObjMonsterGetTopHitDamageUser(lpObj, CustomMonsterInfo.PlayerNumber);

	//bool announced = false;

	if (sortedCount > 0)
	{
		for (int i = 0; i < sortedCount; i++)
		{
			if (OBJECT_RANGE(lpObj->HitDamage[i].index) <= 0)
			{
				continue;
			}

			lpTarget = &gObj[lpObj->HitDamage[i].index]; //player

			if (gObjCalcDistance(lpObj, lpTarget) > 11)
			{
				LogAdd(LOG_RED, "[MonsterKillBonus] [%s][%s] Lost monster bonus caused by big distance [Monster %d][Map %d]", lpTarget->Account, lpTarget->Name, lpObj->Class, lpObj->Map);
				continue;
			}

			/*if (announced == false && CustomMonsterInfo.KillMessage != -1)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(CustomMonsterInfo.KillMessage), lpTarget->Name, lpObj->Name);
				announced = true;
			}*/

			if (lpTarget->AccountLevel >= CustomMonsterInfo.MinVIPCoin)
			{
				if (CustomMonsterInfo.Ruud > 0)
				{
					lpTarget->Ruud += CustomMonsterInfo.Ruud;
					gRheaGold.GC_Notify_RuudUpdate(lpTarget->Index, lpTarget->Ruud, CustomMonsterInfo.Ruud, 1);
				}

				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got |%d|%d|%d| coins for killing %d with %d damage", lpTarget->Name, CustomMonsterInfo.Ruud, CustomMonsterInfo.WcoinP, CustomMonsterInfo.WcoinC, CustomMonsterInfo.Index, lpObj->HitDamage[i].damage);

				GDCustomMonsterRewardSaveSend(lpTarget->Index, lpObj->Class, lpObj->Map, 0, CustomMonsterInfo.WcoinP, CustomMonsterInfo.WcoinC, lpObj->HitDamage[i].damage);
			}

			/*if (CustomMonsterInfo.TimerRandom > 0)
			{
				lpTarget->TimerRandom += CustomMonsterInfo.TimerRandom;
				lpTarget->LevelUpPoint += (CustomMonsterInfo.TimerRandom * gServerInfo.m_LevelUpPoint[lpTarget->Class]);				

				if (lpTarget->TimerRandom > MAX_CHARACTER_LEVEL)
				{
					lpTarget->LevelUpPoint -= (lpTarget->TimerRandom - MAX_CHARACTER_LEVEL) * gServerInfo.m_LevelUpPoint[lpTarget->Class];
					lpTarget->TimerRandom = MAX_CHARACTER_LEVEL;
				}

				lpTarget->Experience = gLevelExperience[lpTarget->TimerRandom - 1];
				gObjCalcExperience(lpTarget);

				gObjectManager.CharacterCalcAttribute(lpTarget->Index);

				GCLevelUpSend(lpTarget);
				GCNewCharacterInfoSend(lpTarget);
				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got %d levels for killing %d", lpTarget->Name, CustomMonsterInfo.TimerRandom, CustomMonsterInfo.Index);
			}*/

			if (CustomMonsterInfo.Points > 0)
			{
				lpTarget->LevelUpPoint += CustomMonsterInfo.Points;
				GCLevelUpSend(lpTarget);
				GCNewCharacterInfoSend(lpTarget);
				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got %d points for killing %d", lpTarget->Name, CustomMonsterInfo.Points, CustomMonsterInfo.Index);
			}

			if (CustomMonsterInfo.Resets > 0 && CustomMonsterInfo.MinVipResets <= lpTarget->AccountLevel)
			{
				lpTarget->Reset += CustomMonsterInfo.Resets;

				if (lpTarget->Reset > gServerInfo.m_CommandResetLimit[lpTarget->AccountLevel])
				{
					lpTarget->Reset = gServerInfo.m_CommandResetLimit[lpTarget->AccountLevel];
				}

				GCNewCharacterInfoSend(lpTarget);

				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got %d resets for killing %d", lpTarget->Name, CustomMonsterInfo.Resets, CustomMonsterInfo.Index);
			}
		}
	}
}

bool CCustomMonster::GetCustomMonsterInfo(int index,int map, CUSTOM_MONSTER_INFO* lpInfo) // OK
{
	for (std::vector<CUSTOM_MONSTER_INFO>::iterator it = this->m_CustomMonsterInfo.begin(); it != this->m_CustomMonsterInfo.end(); it++)
	{
		if ((it->Index == -1 || it->Index == index) && (it->MapNumber == -1 || it->MapNumber == map))
		{
			(*lpInfo) = (*it);
			return 1;
		}
	}

	return 0;
}

void CCustomMonster::SendDamageRanking(LPOBJ lpObj)
{
	if (lpObj->isBoss == 0)
	{
		return;
	}

	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if (this->GetCustomMonsterInfo(lpObj->Class, lpObj->Map, &CustomMonsterInfo) == 0)
	{
		return;
	}

	for (int i = 0; i < MAX_HIT_DAMAGE; i++)
	{
		if (lpObj->HitDamage[i].index < 0)
		{
			continue;
		}

		if (gObjIsConnected(lpObj->HitDamage[i].index) == 0)
		{
			lpObj->HitDamage[i].index = -1;
		}
	}

	PMSG_TARGET_BOSS_DATA pBossMsg;

	pBossMsg.h.set(0xF3, 0xE3, sizeof(pBossMsg));

	pBossMsg.Rank[0] = 0;
	strcpy(pBossMsg.Rank, CustomMonsterInfo.Rank);
	pBossMsg.RankColor = CustomMonsterInfo.RankColor;	

	pBossMsg.Level = lpObj->Level;

	if (lpObj->Live == 0)
	{
		pBossMsg.Life = 0;
		pBossMsg.MaxLife = 0;
	}
	else
	{
		pBossMsg.Life = lpObj->Life;
		pBossMsg.MaxLife = lpObj->MaxLife + lpObj->AddLife;
	}

	int sortedCount = gObjMonsterGetTopHitDamageUser(lpObj, 10);

	if (sortedCount > 0)
	{
		for (int n = 0; n < MAX_VIEWPORT; n++)
		{
			if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_USER)
			{
				if (lpObj->Live == 1 && lpObj->X == gObj[lpObj->VpPlayer2[n].index].X && lpObj->Y == gObj[lpObj->VpPlayer2[n].index].Y)
				{
					gObjBackSpring2(&gObj[lpObj->VpPlayer2[n].index], lpObj, 2);

					gObj[lpObj->VpPlayer2[n].index].LastBackSpring = -1;

					/*if (!gEffectManager.CheckEffect(&gObj[lpObj->VpPlayer2[n].index], EFFECT_STERN))
					{
						gEffectManager.AddEffect(&gObj[lpObj->VpPlayer2[n].index], 0, EFFECT_STERN, 3, 0, 0, 0, 0);
					}*/
				}

				int selfRank = gObjMonsterGetDamageByUser(lpObj, lpObj->VpPlayer2[n].index);

				pBossMsg.sRank = selfRank;

				if (selfRank != -1)
				{					
					pBossMsg.sDamage = lpObj->HitDamage[selfRank].damage;
				}

				for (int i = 0; i < sortedCount; i++)
				{
					pBossMsg.Damage[i] = lpObj->HitDamage[i].damage;
					strcpy(pBossMsg.Name[i], gObj[lpObj->HitDamage[i].index].Name);
				}

				pBossMsg.Count = sortedCount;
				
				DataSend(lpObj->VpPlayer2[n].index, (BYTE*)&pBossMsg, pBossMsg.h.size);
			}
		}
	}
}

void CCustomMonster::SendBossChat(LPOBJ lpObj, LPOBJ lpTarget)
{
	switch (GetLargeRand() % 2000)
	{
	case 10:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Chama mais gente ai, que assim vai demorar... =P");
		break;
	case 20:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Nem tinha visto vc, %s! Achei que tinha parado...", lpTarget->Name);
		break;
	case 30:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Alguem da potion ai pro %s, que ele ta meio fudido.", lpTarget->Name);
		break;
	case 40:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Refaz essa DT lixo ai, %s ... da tempo!", lpTarget->Name);
		break;
	case 50:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Imagina tentar me matar com esse set bosta!");
		break;
	case 60:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "AEHUEUHEAHUE fraco demais!");
		break;
	case 70:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "%s nao consegue matar um BOSS, imagina isso no PvP?!", lpTarget->Name);
		break;
	case 80:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Aew %s, vai la no Discord chorar, vai! mimimi", lpTarget->Name);
		break;
	case 90:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "ALLLGGUUEEEEMMM chama o Aldemiro, gente!");
		break;
	case 100:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Meu cheat e pago, igual o do %s! Desiste amigao!", lpTarget->Name);
		break;
	case 110:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Eu to aqui desde Season 1, nunca que perco pra voce!");
		break;
	case 120:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Qual a desculpa, %s? Lagou ai, foi? kkkkkkkk", lpTarget->Name);
		break;
	case 130:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Mete o Seal o Healing %s, senao num vai... kkkk", lpTarget->Name);
		break;
	case 140:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Ordanael ta do meu lado, nunca que eu morro!");
		break;
	case 150:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Isso eh que num podi...");
		break;
	case 160:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "%s eh mais mais um pra eu amassar!", lpTarget->Name);
		break;
	case 170:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Ae %s, so nao vale chorar pro ADM quando morrer, hein?", lpTarget->Name);
		break;
	case 180:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Ta facil pra mim, liga o bang ai %s", lpTarget->Name);
		break;
	case 190:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Ja estamos aqui faz um tempo! Melhor voce desistir, %s.", lpTarget->Name);
		break;
	case 200:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "As placas nao mentem sobre voce, %s...", lpTarget->Name);
		break;
	case 210:
		gNpcTalk.SendNPCMessageToViewPort(lpObj, "Cuidado que o ban ta rolando solto...");
		break;
	}
}