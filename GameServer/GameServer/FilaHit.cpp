#include "StdAfx.h"
#include "FilaHit.h"
#include "protocol.h"
#include "SpeedFila.h"
#include "ServerInfo.h"
#include "CheatGuard.h"
#include "ObjectManager.h"
#include "DSProtocol.h"
#include "Log.h"
#include "Util.h"
#include "Attack.h"
#include "SkillManager.h"
#include "CustomAttack.h"

FilaHit g_FilaHit;

DWORD ConfigTimer;
DWORD ProcessarTimer;
int configSum;
int configCount;

FilaHit::FilaHit()
{
	ZeroMemory(HIT_FILA, sizeof(HIT_FILA));

	for (int j = 0; j < MAX_OBJECT_USER; j++)
	{
		HIT_USER[j].timer = GetTickCount();

		for (int i = 0; i < MAXHITFILA; i++)
		{
			HIT_FILA[j][i].tick_liberacao = -1;
		}
	}

	this->tamanhoFila = 0;

	ConfigTimer = -1;
	configSum = configCount = 0;
	ProcessarTimer = -1;

	this->threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FilaHit::FilaThread, 0, 0, NULL);
	this->threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FilaHit::PotionThread, 0, 0, NULL);
}

FilaHit::~FilaHit()
{
}

void FilaHit::InicializarElemento(HitFila* elemento)
{
	ZeroMemory(elemento, sizeof(elemento));
	elemento->tick_liberacao = -1;
	elemento->pacote.alvo = -1;
}

void FilaHit::Add(BYTE tipo, int index, BYTE* pacoteOriginal)
{
	if (gSpeedFila.isTableLoaded == false)
	{
		return;
	}

	if (gServerInfo.FilaHitGenerateConfig == 1)
	{
		int timer = GetTickCount() - ConfigTimer;

		//LogAdd(LOG_DEBUG, "Tipo: %d | Timer: %d", tipo, timer);

		/*if (timer < 40)
			return;*/

		int skill = -1;

		switch (tipo)
		{
			case 1:
			{
				PMSG_SKILL_ATTACK_RECV* pacoteInterno = (PMSG_SKILL_ATTACK_RECV*)pacoteOriginal;
				skill = MAKE_NUMBERW(pacoteInterno->skillH, pacoteInterno->skillL);
			}
			break;

			case 2:
			{
				PMSG_DURATION_SKILL_ATTACK_RECV* pacoteInterno = (PMSG_DURATION_SKILL_ATTACK_RECV*)pacoteOriginal;
				skill = MAKE_NUMBERW(pacoteInterno->skillH, pacoteInterno->skillL);
			}
			break;

			case 3:
			{
				PMSG_ATTACK_RECV* pacoteInterno = (PMSG_ATTACK_RECV*)pacoteOriginal;
				skill = 0;
			}
			break;

			case 5:
			{
				PMSG_MULTI_SKILL_ATTACK_RECV* pacoteInterno = (PMSG_MULTI_SKILL_ATTACK_RECV*)pacoteOriginal;
				skill = MAKE_NUMBERW(pacoteInterno->skillH, pacoteInterno->skillL);
			}
			break;
		}

		if ((gSkillManager.GetBaseSkill(skill) == 263 && tipo != 2) || (skill == 0 && tipo == 5))
		{
			return;
		}

		int velocidade = (gObj[index].Class == CLASS_DW && skill != 0) ? gObj[index].MagicSpeed : gObj[index].PhysiSpeed;

		if (timer > 10 && timer < 2500 && ConfigTimer != -1)
		{
			configSum += timer;
			configCount++;

			LogAdd(LOG_BLUE, "Tipo: %d | Skill: %d | Timer Parcial: %d | AttackSpeed: %d ", tipo, gSkillManager.GetBaseSkill(skill), (configSum / configCount), velocidade);
		}

		ConfigTimer = GetTickCount();

		if (configCount >= 10)
		{
			gLog.Output(FILA_LOG, "%d %d %d %d %d", gObj[index].Class, gSkillManager.GetBaseSkill(skill), velocidade, velocidade, (configSum / configCount));

			configCount = 0;
			configSum = 0;

			if (gObj[index].Dexterity < (65536 - gServerInfo.FilaHitAgiInc))
			{
				gObj[index].Dexterity += gServerInfo.FilaHitAgiInc;
				gObjectManager.CharacterCalcAttribute(index);
				GCNewCharacterInfoSend(&gObj[index]);

				/*if (gObj[index].Class == CLASS_SU && gServerInfo.FilaHitAgiInc > 0)
					gItemManager.GCItemListSend(index);*/
			}
		}

		return;
	}

	//Retorna direto se está dentro do timer de penalidade
	if (GetTickCount() < HIT_USER[index - OBJECT_START_USER].penTimer)
	{
		return;
	}

	//Reduz os acumulos e penalidades via timer
	if (GetTickCount() > (HIT_USER[index - OBJECT_START_USER].timer + (gServerInfo.FilaHitCleanTime * 1000)))
	{
		if (HIT_USER[index - OBJECT_START_USER].acumulo > 0)
		{
			HIT_USER[index - OBJECT_START_USER].acumulo--;
		}
		else
		{
			if (HIT_USER[index - OBJECT_START_USER].penalidades > 0)
			{
				HIT_USER[index - OBJECT_START_USER].penalidades--;
			}
		}

		HIT_USER[index - OBJECT_START_USER].timer = GetTickCount();
	}

	HitFila novo;
	PACOTE pacote;

	this->InicializarElemento(&novo);

	novo.tipo = tipo;

	switch (tipo)
	{
	case 1:
	{
		PMSG_SKILL_ATTACK_RECV* pacoteInterno = (PMSG_SKILL_ATTACK_RECV*)pacoteOriginal;
		pacote.skill = MAKE_NUMBERW(pacoteInterno->skillH, pacoteInterno->skillL);
		pacote.distancia = pacoteInterno->dis;
		pacote.alvo = MAKE_NUMBERW(pacoteInterno->indexH, pacoteInterno->indexL);
	}
	break;
	case 2:
	{
		PMSG_DURATION_SKILL_ATTACK_RECV* pacoteInterno = (PMSG_DURATION_SKILL_ATTACK_RECV*)pacoteOriginal;
		pacote.skill = MAKE_NUMBERW(pacoteInterno->skillH, pacoteInterno->skillL);
		pacote.x = pacoteInterno->x;
		pacote.y = pacoteInterno->y;
		pacote.direcao = pacoteInterno->dir;
		pacote.distancia = pacoteInterno->dis;
		pacote.posicao = pacoteInterno->angle;
		pacote.alvo = MAKE_NUMBERW(pacoteInterno->indexH, pacoteInterno->indexL);
		pacote.magicKey = pacoteInterno->MagicKey;
	}
	break;
	case 3:
	{
		PMSG_ATTACK_RECV* pacoteInterno = (PMSG_ATTACK_RECV*)pacoteOriginal;
		pacote.skill = 0;
		pacote.alvo = MAKE_NUMBERW(pacoteInterno->index[0], pacoteInterno->index[1]);
		pacote.acao = pacoteInterno->action;
		pacote.direcao = pacoteInterno->dir;
	}
	break;
	case 5:
	{
		PMSG_MULTI_SKILL_ATTACK_RECV* pacoteInterno = (PMSG_MULTI_SKILL_ATTACK_RECV*)pacoteOriginal;
		pacote.skill = MAKE_NUMBERW(pacoteInterno->skillH, pacoteInterno->skillL);
		pacote.x = pacoteInterno->x;
		pacote.y = pacoteInterno->y;
		pacote.serial = pacoteInterno->serial;
		pacote.count = pacoteInterno->count;
	}
	break;
	}

	novo.pacote = pacote;

	if (novo.pacote.skill > 1200)
	{
		LogAdd(LOG_BLACK, "[ERRO] novo.pacote.skill %d -- tipo %d", novo.pacote.skill, tipo);
		return;
	}

	if (gObj[index].SkillFilaPen[gSkillManager.GetBaseSkill(novo.pacote.skill)] > 0 && HIT_USER[index - OBJECT_START_USER].acumulo > gObj[index].SkillFilaPen[gSkillManager.GetBaseSkill(novo.pacote.skill)])
	{
		LogAdd(LOG_BLACK, "[%s][%s] [FILA] Penalidade aplicada (%d acúmulos) (%d ms)", gObj[index].Account, gObj[index].Name, HIT_USER[index - OBJECT_START_USER].acumulo, gObj[index].SkillFilaPenaltyTime[gSkillManager.GetBaseSkill(novo.pacote.skill)]);
		gLog.Output(FILA_LOG, "[%s][%s] [FILA] Penalidade aplicada (%d acúmulos) (%d ms)", gObj[index].Account, gObj[index].Name, HIT_USER[index - OBJECT_START_USER].acumulo, gObj[index].SkillFilaPenaltyTime[gSkillManager.GetBaseSkill(novo.pacote.skill)]);
		HIT_USER[index - OBJECT_START_USER].penalidades++;
		HIT_USER[index - OBJECT_START_USER].penTimer = GetTickCount() + gObj[index].SkillFilaPenaltyTime[gSkillManager.GetBaseSkill(novo.pacote.skill)];
		HIT_USER[index - OBJECT_START_USER].acumulo = 0;
		this->LimparFila(index - OBJECT_START_USER);
		return;
	}

	if (gObj[index].SkillFilaDC[gSkillManager.GetBaseSkill(novo.pacote.skill)] > 0 && HIT_USER[index - OBJECT_START_USER].penalidades >= gObj[index].SkillFilaDC[gSkillManager.GetBaseSkill(novo.pacote.skill)])
	{
		LogAdd(LOG_BLACK, "[%s][%s] Desconectado por excesso de fila (%d penalidades)", gObj[index].Account, gObj[index].Name, HIT_USER[index - OBJECT_START_USER].penalidades);
		gLog.Output(FILA_LOG, "[%s][%s] Desconectado por excesso de fila (%d penalidades)", gObj[index].Account, gObj[index].Name, HIT_USER[index - OBJECT_START_USER].penalidades);
		gObjUserKill(index);
		this->LimparFila(index - OBJECT_START_USER);
		return;
	}

	if ((gSkillManager.GetBaseSkill(novo.pacote.skill) == 263 && tipo != 2) || (gSkillManager.GetBaseSkill(novo.pacote.skill) == 0 && tipo == 5))
	{
		return;
	}

	novo.tick_liberacao = this->ProximaLiberacao(index, gSkillManager.GetBaseSkill(novo.pacote.skill), tipo);

	if (novo.tick_liberacao == -1)
	{
		return;
	}

	this->InserirElemento(novo, index);

	if (gObj[index].Class == CLASS_DK && gObj[index].ComboSkill.m_index > -1)
	{
		return;
	}

	if (gObj[index].Class != CLASS_DL)
	{		
		int theSkillIs = gSkillManager.GetBaseSkill(novo.pacote.skill);

		if (theSkillIs == SKILL_SOUL_SEEKER || theSkillIs == SKILL_DECAY ||
			theSkillIs == SKILL_TWISTING_SLASH || theSkillIs == SKILL_FIRE_BLOW ||
			theSkillIs == SKILL_FIRE_SLASH ||
			theSkillIs == SKILL_TRIPLE_SHOT || theSkillIs == SKILL_FIVE_SHOT ||
			theSkillIs == SKILL_CHAIN_LIGHTNING || theSkillIs == SKILL_RED_STORM ||
			theSkillIs == SKILL_DARK_SIDE || theSkillIs == SKILL_SPIRIT_HOOK ||
			theSkillIs == SKILL_SHINING_PEAK || theSkillIs == SKILL_BRECHE)
		{
			novo.tick_liberacao = novo.tick_liberacao + gObj[index].SkillFilaTimer[theSkillIs];
			this->InserirElemento(novo, index);
		}	
	}
}

void FilaHit::InserirElemento(HitFila elemento, int index)
{
	for (int i = 0; i < MAXHITFILA; i++)
	{
		if (HIT_FILA[index - OBJECT_START_USER][i].tick_liberacao == -1)
		{
			HIT_FILA[index - OBJECT_START_USER][i] = elemento;
			break;
		}

		if (HIT_FILA[index - OBJECT_START_USER][i].tick_liberacao >= elemento.tick_liberacao)
		{
			for (int j = MAXHITFILA - 1; j > i; j--)
			{
				if (HIT_FILA[index - OBJECT_START_USER][j].tick_liberacao == -1)
				{
					continue;
				}

				HIT_FILA[index - OBJECT_START_USER][j] = HIT_FILA[index - OBJECT_START_USER][j - 1];
			}

			HIT_FILA[index - OBJECT_START_USER][i] = elemento;
			break;
		}
	}
}

void FilaHit::RemoverElemento(int posicao, int index)
{
	HIT_FILA[index][posicao].tick_liberacao = -1;
}

void FilaHit::List()
{
	this->tamanhoFila = 0;

	for (int j = 0; j < MAX_OBJECT_USER; j++)
	{
		if (gObj[j + OBJECT_START_USER].Connected >= OBJECT_ONLINE)
		{
			for (int i = 0; i < MAXHITFILA; i++)
			{
				if (HIT_FILA[j][i].tick_liberacao == -1)
				{
					continue;
				}

				if (strlen(gServerInfo.FilaHitLogStalk) > 2)
				{
					if (gServerInfo.FilaHitLogStalk[0] == gObj[j + OBJECT_START_USER].Name[0] || gServerInfo.FilaHitLogStalk[0] == 'A')
					{
						if (gServerInfo.FilaHitLogStalk[1] == gObj[j + OBJECT_START_USER].Name[1] || gServerInfo.FilaHitLogStalk[1] == 'L')
						{
							if (!strcmp(gServerInfo.FilaHitLogStalk, gObj[j + OBJECT_START_USER].Name) || gServerInfo.FilaHitLogStalk[2] == 'L')
							{
								LogAdd(LOG_BLACK, "Index: %d | Skill: %d | Liberação: %d | Tick: %d", i, HIT_FILA[j][i].pacote.skill, HIT_FILA[j][i].tick_liberacao, GetTickCount());
							}
						}
					}
				}

				this->tamanhoFila++;
			}
		}
	}
}

DWORD FilaHit::ProximaLiberacao(int index, int skill, int tipo)
{
	DWORD proximaLiberacao = GetTickCount() + ((gObj[index].SkillFilaTimer[skill] == 1) ? 40 : gObj[index].SkillFilaTimer[skill]);

	//LogAdd(LOG_DEBUG, "Skill: %d | Timer: %d", skill, gObj[index].SkillFilaTimer[skill]);

	int acumulo = 0;

	DWORD maiorTimer = 0;

	for (int i = 0; i < MAXHITFILA; i++)
	{
		if (HIT_FILA[index - OBJECT_START_USER][i].tick_liberacao != -1)
		{
			acumulo++;
		}
		else
		{
			continue;
		}

		if (maiorTimer < HIT_FILA[index - OBJECT_START_USER][i].tick_liberacao)
		{
			maiorTimer = HIT_FILA[index - OBJECT_START_USER][i].tick_liberacao;
		}

		proximaLiberacao = maiorTimer + gObj[index].SkillFilaTimer[gSkillManager.GetBaseSkill(HIT_FILA[index - OBJECT_START_USER][i].pacote.skill)];
	}

	if (IsPvPTestOn && gObj[index].TargetPvPIndex > -1)
	{
		return proximaLiberacao;
	}

	if (strlen(gServerInfo.FilaHitLogStalk) > 2)
	{
		if (gServerInfo.FilaHitLogStalk[0] == gObj[index].Name[0] || gServerInfo.FilaHitLogStalk[0] == 'A')
		{
			if (gServerInfo.FilaHitLogStalk[1] == gObj[index].Name[1] || gServerInfo.FilaHitLogStalk[1] == 'L')
			{
				if (!strcmp(gServerInfo.FilaHitLogStalk, gObj[index].Name) || gServerInfo.FilaHitLogStalk[2] == 'L')
				{
					LogAdd(LOG_BLACK, "[%s] Acumulo: %d | Tipo: %d | Skill: %d", gObj[index].Name, acumulo, tipo, skill);
				}
			}
		}
	}

	if (gObj[index].SkillFilaTimer[skill] < 1)
	{
		LogAdd(LOG_BLACK, "[ERRO] gObj[index].SkillFilaTimer[skill] = %d | Skill: %d", gObj[index].SkillFilaTimer[skill], skill);
		return -1;
	}

	int ping = (gObj[index].Ping < 100) ? 100 : gObj[index].Ping;

	float toleranciaCalc = ((ping / gObj[index].SkillFilaTimer[skill])) + 1;
	toleranciaCalc = ((toleranciaCalc * gObj[index].SkillFilaPingTol[skill]) / 100);
	int tolerancia = ceil(toleranciaCalc);
	if (tolerancia > 9) tolerancia = 9;

	if (acumulo > tolerancia && HIT_USER[index - OBJECT_START_USER].flag == false)
	{
		HIT_USER[index - OBJECT_START_USER].flag = true;
		HIT_USER[index - OBJECT_START_USER].acumulo++;
		//LogAdd(LOG_BLUE, "[%s][%s][%d] Acum local: %d/%d | Geral: %d | Ping: %d | Timer: %d", gObj[index].Account, gObj[index].Name, gObj[index].Class, acumulo, tolerancia, HIT_USER[index - OBJECT_START_USER].acumulo, ping, gObj[index].SkillFilaTimer[skill]);
		//gLog.Output(FILA_LOG, "[%s][%s][%d] Acum local: %d/%d | Geral: %d | Ping: %d | Timer: %d", gObj[index].Account, gObj[index].Name, gObj[index].Class, acumulo, tolerancia, HIT_USER[index - OBJECT_START_USER].acumulo, ping, gObj[index].SkillFilaTimer[skill]);
		return -1;
	}

	if (acumulo > 0 && HIT_USER[index - OBJECT_START_USER].flag == true)
	{
		return -1;
	}

	HIT_USER[index - OBJECT_START_USER].flag = false;

	if (strlen(gServerInfo.FilaHitLogStalk) > 2)
	{
		if (gServerInfo.FilaHitLogStalk[0] == gObj[index].Name[0] || gServerInfo.FilaHitLogStalk[0] == 'A')
		{
			if (gServerInfo.FilaHitLogStalk[1] == gObj[index].Name[1] || gServerInfo.FilaHitLogStalk[1] == 'L')
			{
				if (!strcmp(gServerInfo.FilaHitLogStalk, gObj[index].Name) || gServerInfo.FilaHitLogStalk[2] == 'L')
				{
					LogAdd(LOG_BLUE, "[%s] Proxima liberação em %d / %d", gObj[index].Name, proximaLiberacao - GetTickCount(), gObj[index].SkillFilaTimer[skill]);
				}
			}
		}
	}

	return proximaLiberacao;
}

void FilaHit::Velocidade(LPOBJ lpObj)
{
	for (int i = 0; i < 1200; i++)
	{
		lpObj->SkillFilaTimer[i] = 1;
		lpObj->SkillFilaPen[i] = 0;
		lpObj->SkillFilaDC[i] = 0;
		lpObj->SkillFilaPingTol[i] = 100;
		lpObj->SkillFilaPenaltyTime[i] = 1000;
	}

	int velocidade;

	for (int n = 0; n < gSpeedFila.m_count[lpObj->Class]; n++)
	{
		velocidade = (lpObj->Class == CLASS_DW && gSpeedFila.SpeedFila[lpObj->Class][n].Skill != 0) ? lpObj->MagicSpeed : lpObj->PhysiSpeed;

		if (velocidade >= gSpeedFila.SpeedFila[lpObj->Class][n].MinSpeed && velocidade <= gSpeedFila.SpeedFila[lpObj->Class][n].MaxSpeed)
		{
			if (gSpeedFila.SpeedFila[lpObj->Class][n].Skill > -1)
			{
				lpObj->SkillFilaTimer[gSpeedFila.SpeedFila[lpObj->Class][n].Skill] = gSpeedFila.SpeedFila[lpObj->Class][n].Timer;
				lpObj->SkillFilaPen[gSpeedFila.SpeedFila[lpObj->Class][n].Skill] = gSpeedFila.SpeedFila[lpObj->Class][n].LimPen;
				lpObj->SkillFilaDC[gSpeedFila.SpeedFila[lpObj->Class][n].Skill] = gSpeedFila.SpeedFila[lpObj->Class][n].LimDC;
				lpObj->SkillFilaPingTol[gSpeedFila.SpeedFila[lpObj->Class][n].Skill] = gSpeedFila.SpeedFila[lpObj->Class][n].PingTol;
				lpObj->SkillFilaPenaltyTime[gSpeedFila.SpeedFila[lpObj->Class][n].Skill] = gSpeedFila.SpeedFila[lpObj->Class][n].PenaltyTime;
			}
			else
			{
				for (int j = 0; j < 1200; j++)
				{
					if (lpObj->SkillFilaTimer[j] == 1)
					{
						lpObj->SkillFilaTimer[j] = gSpeedFila.SpeedFila[lpObj->Class][n].Timer;
						lpObj->SkillFilaPen[j] = gSpeedFila.SpeedFila[lpObj->Class][n].LimPen;
						lpObj->SkillFilaDC[j] = gSpeedFila.SpeedFila[lpObj->Class][n].LimDC;
						lpObj->SkillFilaPingTol[j] = gSpeedFila.SpeedFila[lpObj->Class][n].PingTol;
						lpObj->SkillFilaPenaltyTime[j] = gSpeedFila.SpeedFila[lpObj->Class][n].PenaltyTime;
					}
				}
			}
		}
	}	
}

void FilaHit::LimparFila(int index)
{
	if (index > -1)
	{
		for (int i = 0; i < MAXHITFILA; i++)
		{
			HIT_FILA[index][i].tick_liberacao = -1;
		}
	}
	else
	{
		for (int j = 0; j < MAX_OBJECT_USER; j++)
		{
			for (int i = 0; i < MAXHITFILA; i++)
			{
				HIT_FILA[j][i].tick_liberacao = -1;
			}
		}
	}	
}

DWORD WINAPI FilaHit::FilaThread(LPVOID lpdwThreadParam)
{
	int i;

	while (true)
	{
		if (gObjTotalMonster < 1) { // check if objects are already loaded before run the thread routine
			Sleep(500);
			continue;
		}

		gCustomAttack.FilaPvPTest();

		for (int j = 0; j < MAX_OBJECT_USER; j++)
		{
			if (gObj[OBJECT_START_USER + j].Connected != OBJECT_ONLINE)
			{
				continue;
			}

			i = 0;

			while (true)
			{
				if (g_FilaHit.HIT_FILA[j][i].tick_liberacao != -1 && g_FilaHit.HIT_FILA[j][i].tick_liberacao <= GetTickCount())
				{
					switch (g_FilaHit.HIT_FILA[j][i].tipo)
					{
					case 1:
						gSkillManager.CGSkillAttack(OBJECT_START_USER + j, g_FilaHit.HIT_FILA[j][i].pacote.alvo, g_FilaHit.HIT_FILA[j][i].pacote.skill);
						break;
					case 2:
						gSkillManager.CGDurationSkillAttack(OBJECT_START_USER + j, g_FilaHit.HIT_FILA[j][i].pacote.skill, g_FilaHit.HIT_FILA[j][i].pacote.x, g_FilaHit.HIT_FILA[j][i].pacote.y, g_FilaHit.HIT_FILA[j][i].pacote.direcao, g_FilaHit.HIT_FILA[j][i].pacote.distancia, g_FilaHit.HIT_FILA[j][i].pacote.posicao, g_FilaHit.HIT_FILA[j][i].pacote.alvo);
						break;
					case 3:
						gAttack.CGAttack(OBJECT_START_USER + j, g_FilaHit.HIT_FILA[j][i].pacote.alvo, g_FilaHit.HIT_FILA[j][i].pacote.acao, g_FilaHit.HIT_FILA[j][i].pacote.direcao);
						break;
					}

					g_FilaHit.RemoverElemento(i, j);
				}

				i++;

				if (i >= MAXHITFILA)
					break;
			}
		}

		Sleep(10);
	}
}

DWORD WINAPI FilaHit::PotionThread(LPVOID lpdwThreadParam)
{
	int i;
	LPOBJ lpObj;

	while (true)
	{
		if (gObjTotalMonster < 1) { // check if objects are already loaded before run the thread routine
			Sleep(500);
			continue;
		}

		for (i = OBJECT_START_USER; i < MAX_OBJECT; i++)
		{
			lpObj = &gObj[i];

			if (lpObj->Type != OBJECT_USER)
			{
				continue;
			}

			if (lpObj->Connected != OBJECT_ONLINE)
			{
				continue;
			}

			if ((GetTickCount64() - lpObj->PotionTime) > 0 && (GetTickCount64() - lpObj->PotionTime) < ((DWORD)gServerInfo.m_CheckAutoPotionHackTolerance))
			{
				continue;
			}			

			if (gServerInfo.m_PotionMode == 3 || lpObj->AutoPotionBot)
			{
				lpObj->PotionTime = GetTickCount64();

				int HPValue = (int)(((lpObj->MaxLife + lpObj->AddLife) * gServerInfo.m_LargeLifePotionRate) / 100);

				if (lpObj->CureImproveRate > 0)
				{
					HPValue += (HPValue * lpObj->CureImproveRate / 100);
				}

				if (gEffectManager.CheckEffect(lpObj, EFFECT_BLINDNES))
				{
					HPValue -= (HPValue * gServerInfo.MASTERY_BONUS_CURE_HARM_FACTOR / 100);
				}

				//LogAdd(LOG_BLUE, "Recuperar: %d", HPValue);

				if ((lpObj->Life + HPValue) > (lpObj->MaxLife + lpObj->AddLife))
				{
					lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
				}
				else
				{
					lpObj->Life += HPValue;
				}

				GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);
			}	
			else if (gServerInfo.m_PotionMode == 2)
			{
				if ((GetTickCount() - lpObj->AutoPotionTime) > (int)(lpObj->Ping * 1.5))
				{
					lpObj->AutoPotion = false;
					continue;
				}

				CItem* citem = &gObj[lpObj->Index].Inventory[lpObj->AutoPotionPos];

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

				CItem* citem = &gObj[lpObj->Index].Inventory[lpObj->AutoPotionPos];

				if (gObjectManager.CharacterUsePotion(lpObj, citem))
				{
					gItemManager.DecreaseItemDur(lpObj, lpObj->AutoPotionPos, 1);
				}

				continue;
			}
							
		}

		Sleep(1);
	}
}