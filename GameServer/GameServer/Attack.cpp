#include "stdafx.h"
#include "Attack.h"
#include "BattleSoccerManager.h"
#include "CastleSiege.h"
#include "ChaosCastle.h"
#include "Crywolf.h"
#include "CrywolfAltar.h"
#include "CustomArena.h"
#include "CustomPVPtime.h"
#include "CustomWing.h"
#include "DarkSpirit.h"
#include "Duel.h"
#include "EffectManager.h"
#include "GensSystem.h"
#include "Guild.h"
#include "IllusionTemple.h"
#include "Kalima.h"
#include "Map.h"
#include "MapManager.h"
#include "MasterSkillTree.h"
#include "Monster.h"
#include "MuunSystem.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Party.h"
#include "PentagramSystem.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "Util.h"
#include "Silvester.h"
#include "MasterSkillTree_4th.h"
#include "CNixiesLakeBossZone.h"
#include "CheatGuard.h"
#include "MUFC.h"
#include "CustomMonster.h"
#include "CustomEffectManager.h"
#include "FilaHit.h"
#include "NpcTalk.h"

CAttack gAttack;

CAttack::CAttack() // OK
{

}

CAttack::~CAttack() // OK
{

}

bool CAttack::Attack(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, bool send, BYTE flag, int damageI, int count, bool combo) // OK
{
	long long damage = damageI;

#pragma region ATTACK_CHECK

	if (lpObj->Index == lpTarget->Index)
	{
		return 0;
	}

	if (lpObj->Type == OBJECT_USER && gObjIsConnectedGP(lpObj->Index) == 0)
	{
		return 0;
	}

	if (lpTarget->Type == OBJECT_USER && gObjIsConnectedGP(lpTarget->Index) == 0)
	{
		return 0;
	}

	if (lpObj->Map != lpTarget->Map || lpObj->Teleport == 2)
	{
		return 0;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0 || gMap[lpTarget->Map].CheckAttr(lpTarget->X, lpTarget->Y, 1) != 0)
	{
		return 0;
	}

	if (gDuel.GetDuelArenaBySpectator(lpObj->Index) != 0 || gDuel.GetDuelArenaBySpectator(lpTarget->Index) != 0)
	{
		return 0;
	}

	if (lpObj->Map == MAP_KNICKS && lpObj->Type == OBJECT_USER && g_NixiesLakeBossZone.IsBossZone(lpTarget->X, lpTarget->Y) && !g_NixiesLakeBossZone.IsBossZone(lpObj->X, lpObj->Y))
	{
		return 0;
	}

#if(GAMESERVER_TYPE==1)

	if (gCastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (lpTarget->Type == OBJECT_MONSTER && lpTarget->Map == MAP_CASTLE_SIEGE && (lpTarget->Class == 277 || lpTarget->Class == 283 || lpTarget->Class == 288))
		{
			return 0;
		}
	}

	if (gCrywolf.GetCrywolfState() == CRYWOLF_STATE_READY || gCrywolf.GetCrywolfState() == CRYWOLF_STATE_END)
	{
		if (lpTarget->Type == OBJECT_MONSTER && lpTarget->Map == MAP_CRYWOLF)
		{
			return 0;
		}
	}

#endif

	if (lpObj->GuildNumber > 0 && lpObj->Guild != 0 && lpObj->Guild->WarState != 0)
	{
		if (lpObj->Guild->WarType == 1 && GetBattleSoccerGoalMove(0) == 0)
		{
			return 1;
		}

		if (lpObj->Guild->WarType == 0 && lpTarget->Type == OBJECT_MONSTER)
		{
			return 0;
		}
	}

	if (lpTarget->Class == 216 || lpTarget->Class == 217 || lpTarget->Class == 218)
	{
		LogAdd(LOG_RED, "Ataque a coroa/switch %d por %s", lpTarget->Class, lpObj->Name);
		return 0;
	}

	if (lpTarget->Type == OBJECT_NPC || lpTarget->Live == 0 || lpTarget->State != OBJECT_PLAYING || lpTarget->Teleport != 0)
	{
		return 0;
	}

	if (gEffectManager.CheckEffect(lpObj, EFFECT_PARALYZE_POW_UP) == TRUE)
	{
		return 0;
	}

	if (lpTarget->Type == OBJECT_MONSTER)
	{
		if (lpTarget->Class == 746)
		{
			g_NixiesLakeBossZone.m_dwBossLastHitTimer = GetTickCount64();
		}
		
		if (KALIMA_ATTRIBUTE_RANGE(lpTarget->Attribute) != 0)
		{
			return 0;
		}

		if ((lpTarget->Class >= 100 && lpTarget->Class <= 110) || lpTarget->Class == 523) // Trap
		{
			return 0;
		}

		if (lpTarget->Class == 221 || lpTarget->Class == 222) // Siege
		{
			return 0;
		}

		if (lpTarget->Class == 673 && lpTarget->Map == MAP_URQUE_HILL)
		{
			gSilvester.SilvesterMonsterProc(lpTarget, lpObj);
		}
		if (gEffectManager.CheckEffect(lpTarget, EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY) == TRUE)
		{
			if (lpSkill != 0)
			{
				BOOL bCheckAttackIsMagicType = gSkillManager.GetSkillType(lpSkill->m_index);

				if (bCheckAttackIsMagicType == TRUE)
				{
					gSkillManager.GCSkillAttackSend(lpObj, lpSkill->m_index, lpTarget->Index, 0);
					//GSProtocol.GCMagicAttackNumberSend(lpObj, skill, lpTargetObj->m_Index, 0);
					return TRUE;
				}

				if (lpTarget->Class == 673)
				{
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, 0, 0);
					//GSProtocol.GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, 0, 0);
					return TRUE;
				}

				if (lpTarget->Class == 746)
				{
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, 0, 0);
					return 1;
				}
			}
		}

		if (gEffectManager.CheckEffect(lpTarget, EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY) == TRUE)
		{
			if (lpSkill != 0)
			{
				BOOL bCheckAttackIsMagicType = gSkillManager.GetSkillType(lpSkill->m_index);

				if (bCheckAttackIsMagicType == FALSE)
				{
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, 0, 0);
					//GSProtocol.GCDamageSend(lpObj->m_Index, lpTargetObj->m_Index, 0, 0, 0, 0);
					return TRUE;
				}

				if (lpTarget->Class == 673)
				{
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, 0, 0);
					//GSProtocol.GCDamageSend(lpObj->m_Index, lpTarget->m_Index, 0, 0, 0, 0);
					return TRUE;
				}

				if (lpTarget->Class == 746)
				{
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, 0, 0);
					return 1;
				}
			}
		}
	}

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_MONSTER)
	{
		if (OBJECT_RANGE(lpObj->SummonIndex) != 0)
		{
			if (lpObj->SummonIndex == lpTarget->Index)
			{
				return 0;
			}
		}
	}

	int distance = gObjCalcDistance(lpObj, lpTarget);

	int SummonIndex = lpObj->Index;

	if (lpObj->Type == OBJECT_MONSTER && OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		SummonIndex = lpObj->SummonIndex;
	}

	int SummonTargetIndex = lpTarget->Index;

	if (lpTarget->Type == OBJECT_MONSTER && OBJECT_RANGE(lpTarget->SummonIndex) != 0)
	{
		SummonTargetIndex = lpTarget->SummonIndex;
	}

	if (this->CheckPlayerTarget(&gObj[SummonIndex], &gObj[SummonTargetIndex]) == 0)
	{
		return 0;
	}

#pragma endregion

#pragma region ATTACK_RETURN

	int skill = ((lpSkill == 0) ? SKILL_NONE : lpSkill->m_skill);

	if (damage == 0 && skill != SKILL_PLASMA_STORM && this->DecreaseArrow(lpObj) == 0)
	{
		return 0;
	}

	if (lpObj->Type == OBJECT_USER)
	{
		lpObj->HPAutoRecuperationTime = GetTickCount();
		lpObj->MPAutoRecuperationTime = GetTickCount();
		lpObj->BPAutoRecuperationTime = GetTickCount();
		lpObj->SDAutoRecuperationTime = GetTickCount();
	}

	if (lpTarget->Type == OBJECT_USER)
	{
		lpTarget->HPAutoRecuperationTime = GetTickCount();
		lpTarget->MPAutoRecuperationTime = GetTickCount();
		lpTarget->BPAutoRecuperationTime = GetTickCount();
		lpTarget->SDAutoRecuperationTime = GetTickCount();
	}

	if (OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		gObjSummonSetEnemy(lpObj, lpTarget->Index);
	}

	if (lpObj->Type == OBJECT_USER)
	{
		gDarkSpirit[lpObj->Index].SetTarget(lpTarget->Index);
	}

	bool duel = gDuel.CheckDuel(lpObj, lpTarget);

	if (lpObj->Type == OBJECT_USER && duel != 0)
	{
		lpObj->DuelTickCount = GetTickCount();
	}

	if (lpTarget->Type == OBJECT_USER && duel != 0)
	{
		lpTarget->DuelTickCount = GetTickCount();
	}

	//gEffectManager.DelEffect(lpObj, EFFECT_INVISIBILITY);

	if (gEffectManager.CheckEffect(lpTarget, EFFECT_ORDER_OF_PROTECTION) != 0)
	{
		this->MissSend(lpObj, lpTarget, lpSkill, send, count);
		return 1;
	}

	if (gEffectManager.CheckEffect(lpTarget, EFFECT_PHYSI_DAMAGE_IMMUNITY) != 0 && lpSkill == 0)
	{
		this->MissSend(lpObj, lpTarget, lpSkill, send, count);
		return 1;
	}

	if (gEffectManager.CheckEffect(lpTarget, EFFECT_MAGIC_DAMAGE_IMMUNITY) != 0 && lpSkill != 0)
	{
		this->MissSend(lpObj, lpTarget, lpSkill, send, count);
		return 1;
	}

	if (gEffectManager.CheckStunEffect(lpObj))
	{
		return 0;
	}

	if (lpTarget->Type == OBJECT_MONSTER)
	{
		if (lpTarget->Class == 200 && lpSkill == 0)
		{
			gObjMonsterStateProc(lpTarget, 6, lpObj->Index, 0);
			this->MissSend(lpObj, lpTarget, lpSkill, send, count);
			return 1;
		}

		if (lpTarget->Class == 200 && lpSkill != 0)
		{
			gObjMonsterStateProc(lpTarget, 7, lpObj->Index, 0);
			this->MissSend(lpObj, lpTarget, lpSkill, send, count);
			return 1;
		}

		if (gEffectManager.CheckEffect(lpTarget, EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY) != 0 && skill == SKILL_NONE)
		{
			this->MissSend(lpObj, lpTarget, lpSkill, send, count);
			return 1;
		}

		if (gEffectManager.CheckEffect(lpTarget, EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY) != 0 && skill != SKILL_NONE)
		{
			this->MissSend(lpObj, lpTarget, lpSkill, send, count);
			return 1;
		}

		if (lpTarget->MonsterSkillElementOption.CheckImmuneTime() != 0)
		{
			if (lpTarget->MonsterSkillElementOption.m_SkillElementImmuneNumber == skill)
			{
				this->MissSend(lpObj, lpTarget, lpSkill, send, count);
				return 1;
			}
		}
	}

#pragma endregion

	WORD effect = 0;

	if (lpObj->isBoss > 0 || lpTarget->isBoss > 0)
	{
		if (lpObj->isBoss && lpObj->ElementalAttribute > 0)
		{
			this->AttackBossElemental(lpObj, lpTarget, lpSkill);
		}
		else if (lpTarget->isBoss && lpTarget->ElementalAttribute > 0)
		{
			this->AttackBossElemental(lpObj, lpTarget, lpSkill);
		}
		else
		{
			this->AttackBoss(lpObj, lpTarget, lpSkill);
		}
		return 1;
	}

#pragma region DAMAGE_CALC

	flag = 0;

	BYTE miss = 0;

	if (damage == 0)
	{
		if (lpObj->Index == lpTarget->SummonIndex)
		{
			return 0;
		}

		if (lpTarget->SummonIndex != -1)
		{
			if ((GetLargeRand() % 100) < 30)
			{
				lpTarget = &gObj[lpTarget->SummonIndex];
			}
		}

		/*if ((lpObj->Type != OBJECT_USER || lpTarget->Type != OBJECT_USER) && this->MissCheck(lpObj, lpTarget, lpSkill, send, count, &miss) == 0)
		{
			this->AttackElemental(lpObj, lpTarget, lpSkill, send, flag, damage, count, combo);
			return 1;
		}

		if ((lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER) && this->MissCheckPvP(lpObj, lpTarget, lpSkill, send, count, &miss) == 0)
		{
			this->AttackElemental(lpObj, lpTarget, lpSkill, send, flag, damage, count, combo);
			return 1;
		}*/

		if (!IsPvPTestOn)
		{
			if (/*lpTarget->BlockRate > 0 &&*/ (lpTarget->Inventory[1].m_Index >= GET_ITEM(6, 0) && lpTarget->Inventory[1].m_Index < GET_ITEM(7, 0)))
			{
				float BlockRate = (float)((GetLargeRand() % 10000) / 100.0f);

				if (BlockRate <= (5 + lpTarget->BlockRate))//lpTarget->BlockRate) )
				{
					effect |= 8;
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, effect, 0);
					return 1;
				}
			}
			else if (lpTarget->ParryRate > 0 && (lpTarget->Inventory[0].m_Index >= GET_ITEM(0, 0) && lpTarget->Inventory[0].m_Index < GET_ITEM(4, 0)))
			{
				float ParryRate = (float)((GetLargeRand() % 10000) / 100.0f);

				if (ParryRate <= lpTarget->ParryRate)
				{
					effect |= 9;
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, effect, 0);
					return 1;
				}
			}
		}

		if ((lpObj->UltraHit50perChance - lpTarget->UltraHitProtection) > 0 && (GetLargeRand() % 10000) < ((lpObj->UltraHit50perChance - lpTarget->UltraHitProtection) * 100.0f))
		{
			effect = 6 | 0x80;

			damage = ((lpTarget->MaxLife + lpTarget->AddLife) * 50 / 100);

			if (lpObj->Map == MAP_CASTLE_SIEGE)
			{
				damage = (damage * gServerInfo.m_CastleSiegeDamageRate) / 100;
			}

			if (distance >= 0 && distance <= 12)
			{
				damage = (long long)(damage * gServerInfo.m_DamageRateByDistance[distance] / 100);
			}
			else
			{
				damage = 0;
			}

			if (!lpTarget->isGhost)
			{
				lpTarget->Life -= damage;
				GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
			}

			gObjectManager.CharacterLifeCheck(lpObj, lpTarget, damage, 0, flag, effect, ((lpSkill == 0) ? 0 : lpSkill->m_index), 0);

			if (gMUFC.Status == MUFC_STARTED)
			{
				if (gMUFC.FighterID[0] == lpObj->Index)
				{
					gMUFC.FighterDamage[0] += damage;
					gMUFC.FighterHits[0]++;
				}
				else if (gMUFC.FighterID[1] == lpObj->Index)
				{
					gMUFC.FighterDamage[1] += damage;
					gMUFC.FighterHits[1]++;
				}
			}

			if (effect != 4 && lpTarget->Type == OBJECT_USER)
			{
				if ((GetLargeRand() % 100) < lpTarget->m_MPSkillOpt.FullDamageReflectRate)
				{
					int FinalDamage = (int)(damage * (100 + (lpTarget->AddPlusDamageReflect - lpObj->SubPlusDamageReflect)) / 100);

					if (lpObj->Map == MAP_CASTLE_SIEGE)
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_CastleSiegeReflectRatePvP / 100));
					}
					else
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_ReflectDamageRatePvP / 100));
					}
				}
				else if ((lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect) > 0)
				{
					int DamageReflect = (((lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect) > gServerInfo.m_DamageReflect) ? gServerInfo.m_DamageReflect : (lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect));
					int FinalDamage = (int)(damage * DamageReflect / 100);
					FinalDamage = (int)(FinalDamage * (100 + (lpTarget->AddPlusDamageReflect - lpObj->SubPlusDamageReflect)) / 100);

					if (lpObj->Map == MAP_CASTLE_SIEGE)
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_CastleSiegeReflectRatePvP / 100));
					}
					else
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_ReflectDamageRatePvP / 100));
					}
				}
			}

			return 1;
		}

		if ((lpObj->UltraHit30perChance - lpTarget->UltraHitProtection) > 0 && (GetLargeRand() % 10000) < ((lpObj->UltraHit30perChance - lpTarget->UltraHitProtection) * 100.0f))
		{
			effect = 6 | 0x100;

			damage = ((lpTarget->MaxLife + lpTarget->AddLife) * 30 / 100);

			if (lpObj->Map == MAP_CASTLE_SIEGE)
			{
				damage = (damage * gServerInfo.m_CastleSiegeDamageRate) / 100;
			}

			if (distance >= 0 && distance <= 12)
			{
				damage = (long long)(damage * gServerInfo.m_DamageRateByDistance[distance] / 100);
			}
			else
			{
				damage = 0;
			}

			if (!lpTarget->isGhost)
			{
				lpTarget->Life -= damage;
				GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
			}

			gObjectManager.CharacterLifeCheck(lpObj, lpTarget, damage, 0, flag, effect, ((lpSkill == 0) ? 0 : lpSkill->m_index), 0);

			if (gMUFC.Status == MUFC_STARTED)
			{
				if (gMUFC.FighterID[0] == lpObj->Index)
				{
					gMUFC.FighterDamage[0] += damage;
					gMUFC.FighterHits[0]++;
				}
				else if (gMUFC.FighterID[1] == lpObj->Index)
				{
					gMUFC.FighterDamage[1] += damage;
					gMUFC.FighterHits[1]++;
				}
			}

			if (effect != 4 && lpTarget->Type == OBJECT_USER)
			{
				if ((GetLargeRand() % 100) < lpTarget->m_MPSkillOpt.FullDamageReflectRate)
				{
					int FinalDamage = (int)(damage * (100 + (lpTarget->AddPlusDamageReflect - lpObj->SubPlusDamageReflect)) / 100);

					if (lpObj->Map == MAP_CASTLE_SIEGE)
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_CastleSiegeReflectRatePvP / 100));
					}
					else
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_ReflectDamageRatePvP / 100));
					}
				}
				else if ((lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect) > 0)
				{
					int DamageReflect = (((lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect) > gServerInfo.m_DamageReflect) ? gServerInfo.m_DamageReflect : (lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect));
					int FinalDamage = (int)(damage * DamageReflect / 100);
					FinalDamage = (int)(FinalDamage * (100 + (lpTarget->AddPlusDamageReflect - lpObj->SubPlusDamageReflect)) / 100);

					if (lpObj->Map == MAP_CASTLE_SIEGE)
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_CastleSiegeReflectRatePvP / 100));
					}
					else
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_ReflectDamageRatePvP / 100));
					}
				}
			}

			return 1;
		}

		if (lpTarget->UltraHPRecoveryChance > 0 && (GetLargeRand() % 10000) < (lpTarget->UltraHPRecoveryChance * 100.0f))
		{
			int recoverAmount = (lpTarget->MaxLife + lpTarget->AddLife) * 40 / 100;

			if ((lpTarget->Life + recoverAmount) > (lpTarget->MaxLife + lpTarget->AddLife))
			{
				lpTarget->Life = lpTarget->MaxLife + lpTarget->AddLife;
			}
			else
			{
				lpTarget->Life += recoverAmount;
			}

			GCLifeSend(lpTarget->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);
		}

		int defense = this->GetTargetDefense(lpObj, lpTarget, &effect);

		if (skill == SKILL_PLASMA_STORM)
		{
			damage = this->GetAttackDamageFenrir(lpObj, lpTarget, lpSkill, &effect, defense);
		}
		else if (lpObj->Class == CLASS_SU && (skill == SKILL_SAHAMUTT || skill == SKILL_NEIL || skill == SKILL_GHOST_PHANTOM))
		{
			damage = this->GetAttackDamageCursed(lpObj, lpTarget, lpSkill, &effect, defense);
		}
		else if ((lpObj->Class == CLASS_DW || lpObj->Class == CLASS_MG || lpObj->Class == CLASS_SU) && skill != SKILL_NONE && skill != SKILL_FALLING_SLASH && skill != SKILL_LUNGE && skill != SKILL_UPPERCUT && skill != SKILL_CYCLONE && skill != SKILL_SLASH && skill != SKILL_TWISTING_SLASH && skill != SKILL_IMPALE && skill != SKILL_FIRE_SLASH && skill != SKILL_POWER_SLASH && skill != SKILL_SPIRAL_SLASH && skill != SKILL_FLAME_STRIKE && skill != SKILL_BLOOD_STORM)
		{
			damage = this->GetAttackDamageWizard(lpObj, lpTarget, lpSkill, &effect, defense);
		}
		else
		{
			damage = this->GetAttackDamage(lpObj, lpTarget, lpSkill, &effect, defense);
		}

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		if (damage > 0)
		{
			this->WeaponDurabilityDown(lpObj, lpTarget);
		}

		if (miss != 0)
		{
			damage = (damage * 30) / 100;
		}

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		int DamageReduction = 0;

		for (int n = 0; n < MAX_DAMAGE_REDUCTION; n++)
		{
			DamageReduction += lpTarget->DamageReduction[n];
		}

		DamageReduction = ((DamageReduction > gServerInfo.m_DamageReduction) ? gServerInfo.m_DamageReduction : DamageReduction);

		damage -= damage * DamageReduction / 100;

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		if (lpTarget->EffectOption.AddDamageReduction > 0)
		{
			damage -= (damage * lpTarget->EffectOption.AddDamageReduction) / 100;
		}

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		if (lpObj->AvoidShieldSkill == 0 && (GetTickCount() - lpTarget->ShieldDamageReductionTime) < ((DWORD)(gServerInfo.m_DefenseTimeConstA * 1000)))
		{
			int ShieldDamageReduction = ((lpTarget->ShieldDamageReduction > gServerInfo.m_ShieldDamageReduction) ? gServerInfo.m_ShieldDamageReduction : lpTarget->ShieldDamageReduction);
			damage -= (damage * ShieldDamageReduction) / 100;
		}

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		this->WingSprite(lpObj, lpTarget, &damage);

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		this->HelperSprite(lpObj, lpTarget, &damage);

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		int MinDamage = (lpObj->Level + lpObj->MasterLevel) / 10;

		MinDamage = ((MinDamage < 1) ? 1 : MinDamage);

		damage = ((damage < MinDamage) ? MinDamage : damage);

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		this->DamageSprite(lpTarget, damage);

		this->MountSprite(lpTarget, damage);

		gMuunSystem.MuunSprite(lpTarget, damage);

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		if (lpObj->Class == CLASS_DW || lpObj->Class == CLASS_SU || lpObj->Class == CLASS_FE)
		{
			damage = (damage * 7);
		}

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		if (skill == SKILL_FIRE_BLOW || skill == SKILL_FALLING_SLASH || skill == SKILL_LUNGE || skill == SKILL_UPPERCUT || skill == SKILL_CYCLONE || skill == SKILL_SLASH || skill == SKILL_TWISTING_SLASH || skill == SKILL_RAGEFUL_BLOW || skill == SKILL_DEATH_STAB || skill == SKILL_CRESCENT_MOON_SLASH || skill == SKILL_STAR_FALL || skill == SKILL_IMPALE || skill == SKILL_FIRE_BREATH || skill == SKILL_ICE_ARROW || skill == SKILL_PENETRATION || skill == SKILL_FIRE_SLASH || skill == SKILL_POWER_SLASH || skill == SKILL_SPIRAL_SLASH || skill == SKILL_FROZEN_STAB || skill == SKILL_FLAME_STRIKE || skill == SKILL_CHARGE || skill == SKILL_BLOOD_STORM)
		{
			if (skill != SKILL_IMPALE || lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 2) || lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 3) || lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 37))
			{
				if (lpObj->Class == CLASS_DK || lpObj->Class == CLASS_MG)
				{
					damage = (damage * lpObj->DKDamageMultiplierRate) / 100;
				}
				/*else
				{
					damage = (damage * 200) / 100;
				}*/
			}
		}
		else if (skill == SKILL_FIRE_BLOW ||
			skill == SKILL_DW_METEOR_STRIKE ||
			skill == SKILL_METEOR_STORM ||
			skill == SKILL_SOUL_SEEKER ||
			skill == SKILL_FOCUS_SHOT ||
			skill == SKILL_FIRE_ARROW ||
			skill == SKILL_FIRE_BEAST ||
			skill == SKILL_AQUA_BEAST ||
			skill == SKILL_ICE_BLOOD ||
			skill == SKILL_FIRE_BLOOD ||
			skill == SKILL_DARK_BLAST ||
			skill == SKILL_MG_METEOR_STRIKE ||
			skill == SKILL_FLAME_SPEAR ||
			skill == SKILL_FIRE_WAVE ||
			skill == SKILL_VOLCANO ||
			skill == SKILL_ARCHANGEL_S_WILL)
		{
			damage = (damage * lpSkill->m_DamageMin) / 100;
		}
		else if (skill == SKILL_WIND_SOUL || skill == SKILL_FORCE || skill == SKILL_FIRE_BURST || skill == SKILL_EARTHQUAKE || skill == SKILL_ELECTRIC_SPARK || skill == SKILL_FIRE_BLAST || skill == SKILL_FIRE_SCREAM || skill == SKILL_BIRDS)
		{
			damage = (damage * lpObj->DLDamageMultiplierRate) / 100;
		}
		else if (skill == SKILL_HARSH_STRIKE)
		{
			damage = (damage * lpObj->GLDamageMultiplierRate[0]) / 100;
		}
		else if (skill == SKILL_SPIN_STEP)
		{
			damage = (damage * lpObj->GLDamageMultiplierRate[1]) / 100;
			damage = (damage * 110) / 100;
		}
		else if (skill == SKILL_FK_EXPLOSION)
		{
			damage = (damage * lpObj->GLDamageMultiplierRate[1]) / 100;
			damage = (damage * 70) / 100;
			damage += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GL_SPIN_SETP_MASTERY);
		}
		else if (skill == SKILL_MAGIC_PIN)
		{
			damage = (damage * lpObj->GLDamageMultiplierRate[1]) / 100;
		}
		else if (skill == SKILL_SHINING_PEAK || skill == SKILL_BRECHE)
		{
			damage = (((damage * lpObj->GLDamageMultiplierRate[0]) / 100) + ((damage * lpObj->GLDamageMultiplierRate[1]) / 100));
		}
		else if (skill == SKILL_PLASMA_STORM)
		{
			damage = (damage * (200 + (((lpObj->Level > 300) ? ((lpObj->Level - 300) + lpObj->MasterLevel) : 0) / 5))) / 100;
		}
		else if (skill == SKILL_CHAIN_LIGHTNING)
		{
			damage = (damage * ((count == 2) ? 70 : ((count == 3) ? 50 : 100))) / 100;
		}
		else if (skill == SKILL_RAKLION_SELUPAN1)
		{
			damage = (damage * 200) / 100;
		}
		else if (skill == SKILL_RAKLION_SELUPAN2)
		{
			damage = (damage * 220) / 100;
		}
		else if (skill == SKILL_RAKLION_SELUPAN3)
		{
			damage = (damage * 230) / 100;
		}
		else if (skill == SKILL_RAKLION_SELUPAN4)
		{
			damage = (damage * 250) / 100;
		}
		else if (skill == SKILL_KILLING_BLOW || skill == SKILL_UPPER_BEAST || skill == SKILL_PHOENIX_SHOT)
		{
			damage = (damage * lpObj->RFDamageMultiplierRate[0]) / 100;
		}
		else if (skill == SKILL_CHAIN_DRIVER)
		{
			damage = (damage + ((lpObj->Vitality + lpObj->AddVitality) / gServerInfo.m_RFDamageMultiplierConstB));
			damage = (damage * lpObj->RFDamageMultiplierRate[0]) / 100;
		}
		else if (skill == SKILL_DARK_SIDE || skill == SKILL_SPIRIT_HOOK)
		{
			//damage = (damage + (((lpObj->Dexterity + lpObj->AddDexterity) / gServerInfo.m_RFDamageMultiplierConstA) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_RFDamageMultiplierConstC)));
			damage = (damage * lpObj->RFDamageMultiplierRate[2]) / 100;
		}
		else if (skill == SKILL_DARK_PHOENIX_SHOT)
		{
			damage = (damage * ((lpObj->Dexterity + lpObj->AddDexterity) / gServerInfo.DarkPhoenixShotDexDivisor + gServerInfo.DarkPhoenixShotDexSum) / 100.0);
		}
		else if (skill == SKILL_DRAGON_LORE)
		{
			damage = (damage + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_RFDamageMultiplierConstC));
			damage = (damage * lpObj->RFDamageMultiplierRate[1]) / 100;
		}
		else if (skill == SKILL_DRAGON_SLAYER)
		{
			damage = (damage * lpObj->RFDamageMultiplierRate[1]) / 100;
			damage = ((lpTarget->Type == OBJECT_USER) ? ((damage + 100) * 3) : damage);
		}
		else if (skill == SKILL_CHARGE)
		{
			damage = (damage * (150 + lpObj->RFDamageMultiplierRate[0])) / 100;
		}

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		//damage += gMasterSkillTree_4th.ForMasterSkillAddDamage(lpObj, damage, skill);

		//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

		if (count > 0 && (skill == SKILL_UPPER_BEAST || skill == SKILL_DARK_SIDE || skill == SKILL_DARK_PHOENIX_SHOT || skill == SKILL_SPIRIT_HOOK))
		{
			effect |= (((count % 2) == 0) ? 0x20 : 0x10);
		}
		else if (count > 0 && (skill == SKILL_HARSH_STRIKE || skill == SKILL_MAGIC_PIN))
		{
			effect |= (((count % 3) == 1) ? 0x10 : 0x20);
		}
		else if (count > 0 && (skill == SKILL_KILLING_BLOW || skill == SKILL_CHAIN_DRIVER || skill == SKILL_DRAGON_LORE || skill == SKILL_PHOENIX_SHOT))
		{
			effect |= (((count % 4) == 0) ? 0x20 : 0x10);
		}

		//LogAdd(LOG_DEBUG, "Double: %d | Resist: %d | Calc: %d", lpObj->m_MPSkillOpt.DoubleDamageRate, lpTarget->ResistDoubleDamageRate, (lpObj->m_MPSkillOpt.DoubleDamageRate - lpTarget->ResistDoubleDamageRate));
		//LogAdd(LOG_DEBUG,"Triple: %d | Resist: %f | Calc: %f", lpObj->m_MPSkillOpt.TripleDamageRate, lpTarget->ResistTripleDamageRate, ((lpObj->m_MPSkillOpt.TripleDamageRate - lpTarget->ResistTripleDamageRate) * 100.0f));

		//LogAdd(LOG_DEBUG, "Double: %d", lpObj->m_MPSkillOpt.DoubleDamageRate);
		//LogAdd(LOG_DEBUG, "Triple: %d", lpObj->m_MPSkillOpt.TripleDamageRate);

		if (!IsPvPTestOn && (GetLargeRand() % 10000) < ((lpObj->m_MPSkillOpt.TripleDamageRate - lpTarget->ResistTripleDamageRate) * 100.0f))
		{
			effect |= 0x100;
			damage += damage + damage;
		}
		else if (!IsPvPTestOn && (GetLargeRand() % 100) < (lpObj->m_MPSkillOpt.DoubleDamageRate - lpTarget->ResistDoubleDamageRate))
		{
			effect |= 0x40;
			damage += damage;
		}

		if (combo != 0)
		{
			effect |= 0x80;
			damage += (2 * damage);
			//damage += (((lpObj->Strength + lpObj->AddStrength) + (lpObj->Dexterity + lpObj->AddDexterity) + (lpObj->Energy + lpObj->AddEnergy)) / gServerInfo.m_ComboDamageConstA) * gServerInfo.m_ComboDamageConstB;
			damage += (damage * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_COMBO_DAMAGE)) / 100;
			skill = SKILL_COMBO;
		}

		if (skill == SKILL_ELECTRIC_SPARK)
		{
			if (OBJECT_RANGE(lpObj->PartyNumber) != 0)
			{
				int iPartyCount = gParty.GetNearMemberCount(lpObj);
				effect |= 0x80;
				damage = (damage * iPartyCount);
			}
		}

		/*
		if (lpObj->Class == CLASS_DK && gObjCalcDistance(lpObj, lpTarget) <= 2)
		{
			damage += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DK_BATTLE_MIND);
		}

		if (lpObj->Class == CLASS_DW && gObjCalcDistance(lpObj, lpTarget) >= 5)
		{
			damage += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DW_GRAND_MAGIC_POW_UP);
		}

		if (lpObj->Class == CLASS_FE && gObjCalcDistance(lpObj, lpTarget) >= 4)
		{
			damage += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FE_MARKSMAN);
		}

		if (lpObj->Class == CLASS_SU)
		{
			if ((lpTarget->X - lpObj->X) >= 4 && (lpTarget->Y - lpObj->Y) >= 4)
			{
				damage += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SU_PAIN_OF_CURSE);
			}
		}*/
	}
	else
	{
		if (skill != SKILL_EXPLOSION && skill != SKILL_COMBO)
		{
			effect = 0x04;

			lpTarget->Life -= damage;
			GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);

			gObjectManager.CharacterLifeCheck(lpObj, lpTarget, damage, 0, flag, effect, ((lpSkill == 0) ? 0 : lpSkill->m_index), 0);

			return 1;
		}
	}

	//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

#pragma endregion

#pragma region DAMAGE_CONFIG

	//Item Damage Increase
	int iItemIndex = 0;

	for (iItemIndex = 0; iItemIndex <= 1; iItemIndex++)
	{
		if (lpObj->Inventory[iItemIndex].IsItem() && lpObj->Inventory[iItemIndex].m_Index < GET_ITEM(6, 0))
		{
			if (gItemManager.ItemDamageRate[lpObj->Inventory[iItemIndex].m_Index] != 100)
			{
				damage = (int)damage * gItemManager.ItemDamageRate[lpObj->Inventory[iItemIndex].m_Index] / 100;
			}
		}
	}

	//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

	//Item Damage Reduction
	for (iItemIndex = 1; iItemIndex <= 6; iItemIndex++)
	{
		if (lpTarget->Inventory[iItemIndex].IsItem() && lpTarget->Inventory[iItemIndex].m_Index >= GET_ITEM(6, 0))
		{
			if (lpTarget->Inventory[iItemIndex].m_Index >= GET_ITEM(7, 0) && lpTarget->Inventory[iItemIndex].m_Index < GET_ITEM(12, 0)) //set part
			{
				if (lpTarget->ArmorSetBonus == 0)
				{
					continue;
				}
			}

			if (gItemManager.ItemDamageRate[lpTarget->Inventory[iItemIndex].m_Index] != 100)
			{
				damage = (int)damage * gItemManager.ItemDamageRate[lpTarget->Inventory[iItemIndex].m_Index] / 100;
			}
		}
	}

	//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

	//Pentagram item damage rate
	if (lpTarget->Inventory[236].IsItem() && gItemManager.ItemDamageRate[lpTarget->Inventory[236].m_Index] != 100)
	{
		damage = (int)damage * gItemManager.ItemDamageRate[lpTarget->Inventory[236].m_Index] / 100;
	}

	if (lpObj->Type == OBJECT_USER)
	{
		if (lpTarget->Type == OBJECT_USER)
		{
			//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

			damage = (damage * gSkillManager.GetDamageRatePvP(skill)) / 100;

			//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

			int GeneralRate = 100 + (lpObj->AddGeneralDamageRate + lpObj->AddPVPDamageRate + lpObj->EffectOption.IncreasePvPDamage) - (lpTarget->ReduceGeneralDamageRate + lpTarget->EffectOption.DecreasePvPDamage);

			damage = (damage * GeneralRate) / 100;

			//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

			damage = (damage * gServerInfo.m_GeneralDamageRatePvP) / 1000;

			//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

			damage = (damage * gServerInfo.m_DamageRatePvP[lpObj->Class]) / 100;

			//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

			//if (lpObj->Map != MAP_DUNGEON)
			//{
			damage = (damage * gServerInfo.m_DamageRateTo[lpObj->Class][lpTarget->Class]) / 100;
			//}
			//else
			//{
			//	damage = (damage * gServerInfo.m_X_DamageRateTo[lpObj->Class][lpTarget->Class]) / 100;
			//}

			//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

			damage = (damage * gMapManager.GetMapPvPDamageRate(lpTarget->Map)) / 100;

			//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

			if (lpObj->AddGeneralDamageRateByClass[lpTarget->Class] > 0)
			{
				damage += (damage * lpObj->AddGeneralDamageRateByClass[lpTarget->Class]) / 100;
			}

			//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

			if (lpTarget->DecDamageByShield > 0 && lpTarget->Inventory[1].m_Index >= GET_ITEM(6, 0) && lpTarget->Inventory[1].m_Index < GET_ITEM(7, 0))
			{
				damage -= (long long)(damage * lpTarget->DecDamageByShield / 100.0f);
			}

			int distance = gObjCalcDistance(lpObj, lpTarget);

			if (distance >= 0 && distance <= 12)
			{
				damage = (long long)(damage * gServerInfo.m_DamageRateByDistance[distance] / 100);
			}
			else
			{
				damage = 0;
			}

			if (gMUFC.Status == MUFC_STARTED)
			{
				if (lpTarget->Index == gMUFC.FighterID[0] || lpTarget->Index == gMUFC.FighterID[1] || lpObj->Index == gMUFC.FighterID[0] || lpObj->Index == gMUFC.FighterID[1])
				{
					damage = (damage * gServerInfo.m_MUFCDamageRate) / 100;

					if (gMUFC.FightDamage > 0)
					{
						int MUFCdamageBonus;
						MUFCdamageBonus = (damage * gMUFC.FightDamage) / 100;
						damage += MUFCdamageBonus;
					}
				}
			}

			if (gDuel.CheckDuel(lpObj, lpTarget) != 0)
			{
				damage = (damage * gServerInfo.m_DuelDamageRate) / 100;
			}
			else if (gGensSystem.CheckGens(lpObj, lpTarget) != 0)
			{
				damage = (damage * gServerInfo.m_GensDamageRate) / 100;
			}
			else if (CA_MAP_RANGE(lpObj->Map) != 0 && CA_MAP_RANGE(lpTarget->Map) != 0)
			{
				damage = (damage * gServerInfo.m_CustomArenaDamageRate) / 100;
			}
			else if (CC_MAP_RANGE(lpObj->Map) != 0 && CC_MAP_RANGE(lpTarget->Map) != 0)
			{
				damage = (damage * gServerInfo.m_ChaosCastleDamageRate) / 100;
			}
			else if (IT_MAP_RANGE(lpObj->Map) != 0 && IT_MAP_RANGE(lpTarget->Map) != 0)
			{
				damage = (damage * gServerInfo.m_IllusionTempleDamageRate) / 100;
			}
			else if (lpObj->Map == MAP_CASTLE_SIEGE && lpTarget->Map == MAP_CASTLE_SIEGE)
			{
				if (lpObj->Authority == 32)
				{
					return 0;
				}

				//MG Fire Slash
				if (gEffectManager.CheckEffect(lpTarget, EFFECT_FIRE_SLASH) != 0 || gEffectManager.CheckEffect(lpTarget, EFFECT_FIRE_SLASH_ENHANCED) != 0)
				{
					damage += (int)(damage * (gServerInfo.m_FireSlashMaxRate / 100.0));
				}

				//Summoner Debuffs
				if (gEffectManager.CheckEffect(lpObj, EFFECT_LESSER_DAMAGE) != 0 || gEffectManager.CheckEffect(lpObj, EFFECT_LESSER_DAMAGE_IMPROVED) != 0)
				{
					damage -= (int)(damage * (gServerInfo.m_LesserDamageMaxRate / 100.0));
				}

				if (gEffectManager.CheckEffect(lpTarget, EFFECT_LESSER_DEFENSE) != 0 || gEffectManager.CheckEffect(lpTarget, EFFECT_LESSER_DEFENSE_IMPROVED) != 0)
				{
					damage += (int)(damage * (gServerInfo.m_LesserDefenseMaxRate / 100.0));
				}

				damage = (damage * gServerInfo.m_CastleSiegeDamageRate) / 100;
			}
		}
		else
		{
			damage = (damage * gSkillManager.GetDamageRatePvM(skill)) / 100;
			damage = (damage * gServerInfo.m_GeneralDamageRatePvM) / 100;
			damage = (damage * gServerInfo.m_DamageRatePvM[lpObj->Class]) / 100;
		}
	}

	//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

#pragma endregion

#pragma region DAMAGE_FINISH

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_MONSTER)
	{
		if (lpTarget->Class == 277 || lpTarget->Class == 283) // Castle Gate,Guardian Statue
		{
			if (gEffectManager.CheckEffect(lpObj, EFFECT_BLESS_POTION) != 0)
			{
				damage += (damage * 20) / 100;
			}

			lpObj->AccumulatedDamage = ((lpObj->AccumulatedDamage > 100) ? 0 : (lpObj->AccumulatedDamage + damage));
		}
	}

	damage = ((damage < 0) ? 0 : damage);

#pragma endregion

#pragma region APPLY_EFFECT

	if (lpObj->Type == OBJECT_USER && effect != 4)
	{
		/*if ((GetLargeRand() % 100) < lpObj->m_MPSkillOpt.OffensiveFullMPRestoreRate)
		{
			lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
			GCManaSend(lpObj->Index, 0xFF, (int)lpObj->Mana, lpObj->BP);
		}*/

		if ((GetLargeRand() % 100) < lpObj->m_MPSkillOpt.OffensiveFullHPRestoreRate)
		{
			lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);
		}

		if (lpObj->AbsorbLife > 0 && lpObj->Life < (lpObj->MaxLife + lpObj->AddLife) && (GetLargeRand() % 100) < 50)
		{
			lpObj->Life += lpObj->AbsorbLife;
			GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);
		}

		/*if (lpObj->DoubleDamageBoss > 0 && lpObj->Shield < (lpObj->MaxShield + lpObj->m_MPSkillOpt.AddShield) && (GetLargeRand() % 100) < 50)
		{
			lpObj->Shield += (int)lpObj->DoubleDamageBoss;
			GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);
		}*/

		/*if ((GetLargeRand() % 100) < lpObj->m_MPSkillOpt.ResistBossSkills)
		{
			lpObj->Shield = lpObj->MaxShield + lpObj->m_MPSkillOpt.AddShield;
			GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);
		}*/

		/*if ((GetLargeRand() % 100) < lpObj->OffensiveFullBPRestoreRate)
		{
			lpObj->BP = lpObj->MaxBP + lpObj->AddBP;
			GCManaSend(lpObj->Index, 0xFF, (int)lpObj->Mana, lpObj->BP);
		}*/

		if (gEffectManager.CheckEffect(lpObj, EFFECT_BLOOD_HOWLING) != 0)
		{
			if ((GetLargeRand() % 100) < 10)
			{
				gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, 10, lpObj->Index, 1, SET_NUMBERHW(400), SET_NUMBERLW(400));
			}
		}
		else if (gEffectManager.CheckEffect(lpObj, EFFECT_BLOOD_HOWLING_IMPROVED) != 0)
		{
			if ((GetLargeRand() % 100) < (10 + gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLOOD_HOWLING_IMPROVED)))
			{
				int bloodDmg = (int)400 + (400.0 * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLOOD_HOWLING_IMPROVED) / 100.0);
				gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, 10, lpObj->Index, 1, SET_NUMBERHW(bloodDmg), SET_NUMBERLW(bloodDmg));
			}
		}

		/*if (gEffectManager.CheckEffect(lpObj, EFFECT_BLOOD_HOWLING) != 0 || gEffectManager.CheckEffect(lpObj, EFFECT_BLOOD_HOWLING_IMPROVED) != 0)
		{
			int rate = gServerInfo.m_BloodHowlingConstA;

			rate += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLOOD_HOWLING_IMPROVED);

			if ((GetLargeRand() % 100) < rate)
			{
				if (gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE) == 0 && gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE_IMPROVED) == 0)
				{
					int damage = gServerInfo.m_BloodHowlingConstB;

					gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, 10, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
				}
			}
		}*/
	}

	/*if (lpTarget->Type != OBJECT_USER && lpTarget->m_MPSkillOpt.iMpsImmuneRate > 0 && lpTarget->m_MPSkillOpt.iMpsImmuneRate >= (rand() % 100))
	{
		gSkillManager.GCSkillAttackSend(lpTarget, 323, lpTarget->Index, 1);
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_MASTER_CAST_INVINCIBILITY, 5, 0, 0, 0, 0);
	}*/

	/*if (gEffectManager.CheckEffect(lpTarget, EFFECT_MASTER_CAST_INVINCIBILITY) != 0)
	{
		damage = 0;
	}*/

	/*if (lpTarget->Type != OBJECT_USER && gEffectManager.CheckEffect(lpObj, EFFECT_GREATER_IGNORE_DEFENSE_RATE) != 0)
	{
		int rate = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_IGNORE_DEF_UP);
		int rate1 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_IGNORE_DEF_MASTERY);
		if ((GetLargeRand() % 100) < (rate + rate1))
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_PENETRATE_ARMOR, 2, 20, 0, 0, 0);
		}
	}*/

	if (lpTarget->Type != OBJECT_USER && lpTarget->Inventory[1].m_Index >= GET_ITEM(6, 29) && lpTarget->Inventory[1].m_Index <= GET_ITEM(6, 32))
	{
		int CutBP = 10;

		CutBP += gMasterSkillTree.GetMasterSkillValue(lpTarget, MASTER_SKILL_ADD_GL_CIRCLE_SHIELD_POW_UP);
		CutBP += gMasterSkillTree.GetMasterSkillValue(lpTarget, MASTER_SKILL_ADD_GL_CIRCLE_SHIELD_MASTERY);
		int m_rand = 60;
		m_rand += gMasterSkillTree.GetMasterSkillValue(lpTarget, MASTER_SKILL_ADD_GL_CIRCLE_SHIELD_MASTERY);
		if ((GetLargeRand() % 100) < m_rand)
		{
			lpObj->BP -= CutBP;
			GCManaSend(lpObj->Index, 0xFF, (int)lpObj->Mana, lpObj->BP);
		}
	}

	if (lpSkill != 0 && count <= 1)
	{
		if (this->ApplySkillEffect(lpObj, lpTarget, lpSkill, damage) == 0)
		{
			if (send != 0)
			{
				gSkillManager.GCSkillAttackSend(lpObj, lpSkill->m_index, lpTarget->Index, 0);
			}
		}
		else
		{
			if (send != 0)
			{
				gSkillManager.GCSkillAttackSend(lpObj, lpSkill->m_index, lpTarget->Index, 1);
			}
		}
	}

	//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);	

	if (gEffectManager.CheckEffect(lpTarget, EFFECT_MASTER_CAST_INVINCIBILITY))
	{
		damage = (long long)(damage * 0.95f);
	}

	if (lpObj->BleedingRate > 0 && (GetLargeRand() % 100) < lpObj->BleedingRate)
	{
		if ((GetLargeRand() % 100) < lpTarget->MasteryBonusRevertRate)
		{
			if (!gEffectManager.CheckEffect(lpObj, EFFECT_HEMORRHAGE) && (GetLargeRand() % 100) >= lpObj->BleedingResist)
			{
				gEffectManager.AddEffect(lpObj, 0, EFFECT_HEMORRHAGE, 10, lpTarget->Index, 1, SET_NUMBERHW(damage * 2), SET_NUMBERLW(damage * 2));
			}
		}

		if ((GetLargeRand() % 100) >= lpTarget->BleedingResist)
		{
			if (!gEffectManager.CheckEffect(lpTarget, EFFECT_HEMORRHAGE))
			{
				gEffectManager.AddEffect(lpTarget, 0, EFFECT_HEMORRHAGE, 10, lpObj->Index, 1, SET_NUMBERHW(damage * 2), SET_NUMBERLW(damage * 2));
				WORD efeito = 6;
				GCDamageSend(lpObj->Index, lpTarget->Index, 0, (damage * 2), efeito, 0);
			}
			else
			{
				CEffect* lpEffect = gEffectManager.GetEffect(lpTarget, EFFECT_HEMORRHAGE);
				if (lpEffect->m_value[0] == lpObj->Index)
				{
					int currentDamage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
					int newTime = lpEffect->m_count + (int)((10 - lpEffect->m_count) / 2);
					gEffectManager.DelEffect(lpTarget, EFFECT_HEMORRHAGE);
					gEffectManager.AddEffect(lpTarget, 0, EFFECT_HEMORRHAGE, newTime, lpObj->Index, 1, SET_NUMBERHW(currentDamage + damage), SET_NUMBERLW(currentDamage + damage));
					WORD efeito = 6 | 0x80;
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, currentDamage + damage, efeito, 0);
				}
			}
		}
	}

	if (lpObj->SternRate > 0 && (GetLargeRand() % 100) < lpObj->SternRate)
	{
		if ((GetLargeRand() % 100) < lpTarget->MasteryBonusRevertRate)
		{
			if (!gEffectManager.CheckEffect(lpObj, EFFECT_STERN) && (GetLargeRand() % 100) >= lpObj->SternResist)
			{
				gEffectManager.AddEffect(lpObj, 0, EFFECT_STERN, 3, 0, 0, 0, 0);
			}
		}

		if (!gEffectManager.CheckEffect(lpTarget, EFFECT_STERN) && (GetLargeRand() % 100) >= lpTarget->SternResist)
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 3, 0, 0, 0, 0);
		}
	}

	if (lpObj->CureHarmRate > 0 && (GetLargeRand() % 100) < lpObj->CureHarmRate)
	{
		if ((GetLargeRand() % 100) < lpTarget->MasteryBonusRevertRate)
		{
			if (!gEffectManager.CheckEffect(lpObj, EFFECT_BLINDNES) && (GetLargeRand() % 100) >= lpObj->CureHarmResist)
			{
				gEffectManager.AddEffect(lpObj, 0, EFFECT_BLINDNES, 3, 0, 0, 0, 0);
			}
		}

		if (!gEffectManager.CheckEffect(lpTarget, EFFECT_BLINDNES) && (GetLargeRand() % 100) >= lpTarget->CureHarmResist)
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLINDNES, 10, 0, 0, 0, 0);
		}
	}

	if (lpObj->FreezingDamageRate > 0 && (GetLargeRand() % 100) < lpObj->FreezingDamageRate)
	{
		if ((GetLargeRand() % 100) < lpTarget->MasteryBonusRevertRate)
		{
			if (!gEffectManager.CheckEffect(lpObj, EFFECT_FREEZING_DAMAGE))
			{
				gEffectManager.AddEffect(lpObj, 0, EFFECT_FREEZING_DAMAGE, 10, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
			}
		}

		if (!gEffectManager.CheckEffect(lpTarget, EFFECT_FREEZING_DAMAGE))
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING_DAMAGE, 10, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
		}
	}

	//LogAdd(LOG_DEBUG, "Line %d : Damage: %d", __LINE__, damage);

#pragma endregion

#pragma region DAMAGE_APPLY

	int ShieldDamage = 0;

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		if ((GetLargeRand() % 100) < gServerInfo.m_DefenseAbs && lpTarget->PentagramJewelOption.DefenseAbs)
		{
			int ShieldAbs = (damage * lpTarget->PentagramJewelOption.DefenseAbs) / 100;
			lpTarget->Shield += ShieldAbs;
			damage -= ShieldAbs;
			GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
		}

		if ((GetLargeRand() % 100) < gServerInfo.m_LifeAbs && lpTarget->PentagramJewelOption.LifeAbs)
		{
			int LifeAbs = (damage * lpTarget->PentagramJewelOption.LifeAbs) / 100;
			lpTarget->Life += LifeAbs;
			damage -= LifeAbs;
			GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
		}

		ShieldDamage = this->GetShieldDamage(lpObj, lpTarget, damage);

		if (!lpTarget->isGhost)
		{
			if (lpTarget->Life < (damage - ShieldDamage))
			{
				lpTarget->Life = 0;
			}
			else
			{
				lpTarget->Life -= damage - ShieldDamage;
			}
		}

		if (lpTarget->Shield < ShieldDamage)
		{
			lpTarget->Shield = 0;
		}
		else
		{
			lpTarget->Shield -= ShieldDamage;
		}
	}
	else
	{
		if (lpTarget->Life < damage)
		{
			lpTarget->Life = 0;
		}
		else
		{
			lpTarget->Life -= damage;
		}
	}

	if (lpTarget->Type == OBJECT_MONSTER)
	{
		lpTarget->LastAttackerID = lpObj->Index;

		gObjAddMsgSendDelay(lpTarget, 0, lpObj->Index, 100, 0);

		if (lpTarget->CurrentAI != 0)
		{
			lpTarget->Agro.IncAgro(lpObj->Index, (damage / 50));
		}
	}

#pragma endregion

#pragma region CHECK_SELF_DEFENSE

	if (damage > 0)
	{
		this->ArmorDurabilityDown(lpObj, lpTarget);
	}

	lpObj->Rest = 0;

#pragma endregion

#pragma region ATTACK_FINISH

	if (damage > 0)
	{
		gEffectManager.DelEffect(lpTarget, EFFECT_SLEEP);

		if (effect != 4 && lpTarget->Type == OBJECT_USER)
		{
			if (effect != 4 && lpTarget->Type == OBJECT_USER)
			{
				if ((GetLargeRand() % 100) < lpTarget->m_MPSkillOpt.FullDamageReflectRate)
				{
					int FinalDamage = (int)(damage * (100 + (lpTarget->AddPlusDamageReflect - lpObj->SubPlusDamageReflect)) / 100);

					if (lpObj->Map == MAP_CASTLE_SIEGE)
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_CastleSiegeReflectRatePvP / 100));
					}
					else
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_ReflectDamageRatePvP / 100));
					}
				}
				else if ((lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect) > 0)
				{
					int DamageReflect = (((lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect) > gServerInfo.m_DamageReflect) ? gServerInfo.m_DamageReflect : (lpTarget->DamageReflect + lpTarget->EffectOption.AddDamageReflect));
					int FinalDamage = (int)(damage * DamageReflect / 100);
					FinalDamage = (int)(FinalDamage * (100 + (lpTarget->AddPlusDamageReflect - lpObj->SubPlusDamageReflect)) / 100);

					if (lpObj->Map == MAP_CASTLE_SIEGE)
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_CastleSiegeReflectRatePvP / 100));
					}
					else
					{
						gObjAddMsgSendDelay(lpTarget, 10, lpObj->Index, 1, (int)(FinalDamage * gServerInfo.m_ReflectDamageRatePvP / 100));
					}
				}
			}

			if ((GetLargeRand() % 100) < lpTarget->m_MPSkillOpt.DefensiveFullHPRestoreRate)
			{
				lpTarget->Life = lpTarget->MaxLife + lpTarget->AddLife;
				GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
			}

			if (!IsPvPTestOn && /*lpTarget->AbsorbRate > 0 &&*/ (lpTarget->Inventory[1].m_Index >= GET_ITEM(6, 0) && lpTarget->Inventory[1].m_Index < GET_ITEM(7, 0)))
			{
				float AbsorbRate = (float)((GetLargeRand() % 10000) / 100.0f);

				if (AbsorbRate <= 5)//lpTarget->AbsorbRate)
				{
					//lpTarget->Life = ((lpTarget->Life + damage) > (lpTarget->MaxLife + lpTarget->AddLife)) ? (lpTarget->MaxLife + lpTarget->AddLife) : (lpTarget->Life + damage);
					effect |= 10;
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, effect, 0);
					return 0;
				}
			}

			if (!IsPvPTestOn && lpObj->AbsorbLifeFromDamage > 0)
			{
				float AbsorbRate = (float)((GetLargeRand() % 10000) / 100.0f);

				if (AbsorbRate <= lpObj->AbsorbLifeFromDamage)
				{
					lpObj->Life = ((lpObj->Life + damage) > (lpObj->MaxLife + lpObj->AddLife)) ? (lpObj->MaxLife + lpObj->AddLife) : (lpObj->Life + damage);
					effect |= 10;
					GCDamageSend(lpTarget->Index, lpObj->Index, 0, 0, effect, 0);
					return 0;
				}
			}

			if ((GetLargeRand() % 100) < lpTarget->DefensiveFullSDRestoreRate)
			{
				lpTarget->Shield = lpTarget->MaxShield + lpTarget->m_MPSkillOpt.AddShield;
				GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
			}

			if ((GetLargeRand() % 100) < lpTarget->m_MPSkillOpt.DefensiveFullMPRestoreRate)
			{
				lpTarget->Mana = lpTarget->MaxMana + lpTarget->AddMana;
				GCManaSend(lpTarget->Index, 0xFF, (int)lpTarget->Mana, lpTarget->BP);
			}

			if ((GetLargeRand() % 100) < lpTarget->DefensiveFullBPRestoreRate)
			{
				lpTarget->BP = lpTarget->MaxBP + lpTarget->AddBP;
				GCManaSend(lpTarget->Index, 0xFF, (int)lpTarget->Mana, lpTarget->BP);
			}

			/*if (lpTarget->Inventory[lpTarget->m_btInvenPetPos].IsItem() == 0 ||
				(lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_Index != GET_ITEM(13, 2) && lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_Index != GET_ITEM(13, 3) && lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_Index != GET_ITEM(13, 4) && lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_Index != GET_ITEM(13, 37)))
			{
				if ((GetLargeRand() % 100) < gServerInfo.m_DamageStuckRate[lpTarget->Class])
				{
					flag = 1;
				}
			}*/
		}

		/*if (IsPvPTestOn)
		{
			if (lpTarget->Life < lpTarget->LowerHPPvPTest)
			{
				lpTarget->LowerHPPvPTest = lpTarget->Life;
			}

			if (lpTarget->LowerHPPvPTestTimer == 0 || GetTickCount64() > lpTarget->LowerHPPvPTestTimer + 1000)
			{
				gNotice.GCNoticeSend(lpTarget->Index, 1, 0, 0, 0, 0, 0, "Menor HP: %d", lpTarget->LowerHPPvPTest);
				lpTarget->LowerHPPvPTestTimer = GetTickCount64();
				lpTarget->LowerHPPvPTest = 2000000000;
			}
		}*/

		gObjectManager.CharacterLifeCheck(lpObj, lpTarget, (damage - ShieldDamage), 0, flag, effect, ((lpSkill == 0) ? 0 : lpSkill->m_index), ShieldDamage);

		if (lpObj->Map != MAP_ARENA && lpObj->Map != MAP_CASTLE_SIEGE)
		{
			this->AttackElemental(lpObj, lpTarget, lpSkill, send, flag, damage, count, combo);
		}
	}
	else
	{
		GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, effect, 0);
		//this->AttackElemental(lpObj, lpTarget, lpSkill, send, flag, damage, count, combo);
	}

	if (lpObj->Type == OBJECT_USER && lpObj->Life <= 0 && lpObj->CheckLifeTime <= 0)
	{
		lpObj->AttackObj = lpTarget;
		lpObj->AttackerKilled = ((lpTarget->Type == OBJECT_USER) ? 1 : 0);
		lpObj->CheckLifeTime = 3;
	}

	if (gMUFC.Status == MUFC_STARTED)
	{
		if (gMUFC.FighterID[0] == lpObj->Index)
		{
			gMUFC.FighterDamage[0] += damage;
			gMUFC.FighterHits[0]++;
		}
		else if (gMUFC.FighterID[1] == lpObj->Index)
		{
			gMUFC.FighterDamage[1] += damage;
			gMUFC.FighterHits[1]++;
		}
	}

	if (IsPvPTestOn)
	{
		if (PvP_DPT_Timer > 0)
		{
			if (GetTickCount() - PvP_DPT_Timer >= PvP_DPT_Delay)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Dano total: %d", PvP_DPT_Damage);
				gNotice.GCNoticeSendToAll(2, 0, 0, 0, 0, 0, "Dano por potion: %d", ((PvP_DPT_Damage / (GetTickCount() - PvP_DPT_Timer)) * gServerInfo.m_CheckAutoPotionHackTolerance));
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Tempo: %d (%d)", (GetTickCount() - PvP_DPT_Timer), PvP_DPT_Delay);

				PvP_DPT_Timer = 0;
				PvP_DPT_Damage = 0;
				IsPvPTestOn = false;
				g_FilaHit.LimparFila();
			}
			else
			{
				PvP_DPT_Damage += damage;
			}
		}
	}

	//LogAdd(LOG_BLUE, "DAMAGE: %d", damage);

#pragma endregion

	return 1;
}

bool CAttack::AttackElemental(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, bool send, BYTE flag, int damage, int count, bool combo) // OK
{
	if (lpObj->ElementalAttribute == 0)
	{
		return 0;
	}

	if (lpObj->Type == OBJECT_USER && (lpObj->Inventory[236].IsItem() == 0 || lpObj->Inventory[236].IsPentagramItem() == 0 || lpObj->Inventory[236].m_IsValidItem == 0))
	{
		return 0;
	}

	flag = 0;

	//BYTE miss = 0;

	WORD effect = lpObj->ElementalAttribute;

	int defense = this->GetTargetElementalDefense(lpObj, lpTarget, &effect);

	damage = this->GetAttackDamageElemental(lpObj, lpTarget, lpSkill, &effect, damage, defense);

	//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);

	/*if (miss != 0)
	{
		damage = (damage * 30) / 100;
	}*/

	/*if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage -= (damage * lpTarget->PentagramJewelOption.AddElementalDamageReductionPvP) / 100;
	}*/

	/*if (lpObj->Type != OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage -= (damage * lpTarget->PentagramJewelOption.AddElementalDamageReductionPvM) / 100;
	}*/

	int MinDamage = (lpObj->Level + lpObj->MasterLevel);

	MinDamage = ((MinDamage < 1) ? 1 : MinDamage);

	damage = ((damage < MinDamage) ? MinDamage : damage);

	//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);

	/*if (lpObj->PentagramJewelOption.Bleeding > 0 && (GetLargeRand() % 100) < (gServerInfo.m_BleedingConst - (gServerInfo.m_BleedingConst * lpTarget->AddRadianceResistance / 100)))
	{
		if (!gEffectManager.CheckEffect(lpTarget, EFFECT_HEMORRHAGE))
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_HEMORRHAGE, 10, lpObj->Index, 1, lpObj->PentagramJewelOption.Bleeding, 0);
		}
	}*/

	if ((GetLargeRand() % 100) < (lpObj->PentagramJewelOption.Paralyzing - (lpObj->PentagramJewelOption.Paralyzing * lpTarget->AddRadianceResistance / 100)))
	{
		if (!gEffectManager.CheckEffect(lpTarget, EFFECT_PARALYSIS))
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_PARALYSIS, 10, 0, 0, 0, 0);
		}
	}

	if ((GetLargeRand() % 100) < (lpObj->PentagramJewelOption.Binding - (lpObj->PentagramJewelOption.Binding * lpTarget->AddRadianceResistance / 100)))
	{
		if (!gEffectManager.CheckEffect(lpTarget, EFFECT_BONDAGE))
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_BONDAGE, 3, 0, 0, 0, 0);
		}
	}

	if ((GetLargeRand() % 100) < (gServerInfo.m_BlindConstA - (gServerInfo.m_BlindConstA * lpTarget->AddRadianceResistance / 100)))
	{
		if (!gEffectManager.CheckEffect(lpTarget, EFFECT_BLIND) && lpObj->PentagramJewelOption.Blinding)
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLIND, 3, lpObj->PentagramJewelOption.Blinding, 0, 0, 0);
		}
	}

	if (lpObj->Type == OBJECT_USER)
	{
		if (lpTarget->Type == OBJECT_USER)
		{
			damage = (damage * gServerInfo.m_GeneralElementalDamageRatePvP) / 100;

			//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);

			damage = (damage * gServerInfo.m_ElementalDamageRatePvP[lpObj->Class]) / 100;

			//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);

			damage = (damage * gServerInfo.m_ElementalDamageRateTo[lpObj->Class][lpTarget->Class]) / 100;

			//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);

			if (lpObj->AddElementalDamageRate > 0 || lpObj->EffectOption.IncreasePvPElementalDamage > 0)
			{
				damage = damage + (damage * (lpObj->AddElementalDamageRate + lpObj->EffectOption.IncreasePvPElementalDamage) / 100);
			}

			//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);

			if (lpTarget->ReduceGeneralElementalDamageRate > 0)
			{
				//LogAdd(LOG_DEBUG, "ReduceGeneralElementalDamageRate: %d (%d)", lpTarget->ReduceGeneralElementalDamageRate, __LINE__);
				//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);
				damage = damage - (damage * lpTarget->ReduceGeneralElementalDamageRate / 100);
				//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);
				if (damage < 0) damage = 0;
				//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);
			}

			//LogAdd(LOG_DEBUG, "damage: %d (%d)", damage, __LINE__);

			if (gMUFC.Status == MUFC_STARTED)
			{
				if (lpTarget->Index == gMUFC.FighterID[0] || lpTarget->Index == gMUFC.FighterID[1] || lpObj->Index == gMUFC.FighterID[0] || lpObj->Index == gMUFC.FighterID[1])
				{
					damage = (damage * gServerInfo.m_MUFCDamageRate) / 100;

					if (gMUFC.FightDamage > 0)
					{
						int MUFCdamageBonus;
						MUFCdamageBonus = (damage * gMUFC.FightDamage) / 100;
						damage += MUFCdamageBonus;
					}
				}
			}

			if (gDuel.CheckDuel(lpObj, lpTarget) != 0)
			{
				damage = (damage * gServerInfo.m_DuelElementalDamageRate) / 100;
			}
			else if (gGensSystem.CheckGens(lpObj, lpTarget) != 0)
			{
				damage = (damage * gServerInfo.m_GensElementalDamageRate) / 100;
			}
			else if (CA_MAP_RANGE(lpObj->Map) != 0 && CA_MAP_RANGE(lpTarget->Map) != 0)
			{
				damage = (damage * gServerInfo.m_CustomArenaElementalDamageRate) / 100;
			}
			else if (CC_MAP_RANGE(lpObj->Map) != 0 && CC_MAP_RANGE(lpTarget->Map) != 0)
			{
				damage = (damage * gServerInfo.m_ChaosCastleElementalDamageRate) / 100;
			}
			else if (IT_MAP_RANGE(lpObj->Map) != 0 && IT_MAP_RANGE(lpTarget->Map) != 0)
			{
				damage = (damage * gServerInfo.m_IllusionTempleElementalDamageRate) / 100;
			}
			else if (lpObj->Map == MAP_CASTLE_SIEGE && lpTarget->Map == MAP_CASTLE_SIEGE)
			{
				if (lpObj->Authority == 32)
				{
					return 0;
				}

				damage = (damage * gServerInfo.m_CastleSiegeElementalDamageRate) / 100;
			}
		}
		else
		{
			damage = (damage * gServerInfo.m_GeneralElementalDamageRatePvM) / 100;
			damage = (damage * gServerInfo.m_ElementalDamageRatePvM[lpObj->Class]) / 100;
		}
	}

	if (lpObj->Type == OBJECT_USER)
	{
		if ((GetLargeRand() % 100) < lpObj->PentagramJewelOption.AddElementalSlowRate)
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_PENTAGRAM_JEWEL_SLOW, 20, 0, 0, 0, 0);
		}

		if ((GetLargeRand() % 100) < lpObj->PentagramJewelOption.AddElementalDebuffRate)
		{
			switch (lpObj->ElementalAttribute)
			{
			case ELEMENTAL_ATTRIBUTE_FIRE:
				gEffectManager.AddEffect(lpTarget, 0, EFFECT_PENTAGRAM_JEWEL_HALF_SD, 5, 0, 0, 0, 0);
				break;
			case ELEMENTAL_ATTRIBUTE_WATER:
				gEffectManager.AddEffect(lpTarget, 0, EFFECT_PENTAGRAM_JEWEL_HALF_MP, 5, 0, 0, 0, 0);
				break;
			case ELEMENTAL_ATTRIBUTE_EARTH:
				gEffectManager.AddEffect(lpTarget, 0, EFFECT_PENTAGRAM_JEWEL_HALF_SPEED, 5, 0, 0, 0, 0);
				break;
			case ELEMENTAL_ATTRIBUTE_WIND:
				gEffectManager.AddEffect(lpTarget, 0, EFFECT_PENTAGRAM_JEWEL_HALF_HP, 5, 0, 0, 0, 0);
				break;
			case ELEMENTAL_ATTRIBUTE_DARK:
				gEffectManager.AddEffect(lpTarget, 0, EFFECT_PENTAGRAM_JEWEL_STUN, 5, 0, 0, 0, 0);
				break;
			}
		}
	}

	if (!IsPvPTestOn && (GetLargeRand() % 10000) < ((lpObj->ElementalTripleDamageRate + lpObj->EffectOption.IncreaseElementalTripleDamageRate) * 100.0f))
	{
		effect = 6;
		damage += damage + damage;
	}

	int ShieldDamage = 0;

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		ShieldDamage = this->GetShieldDamage(lpObj, lpTarget, damage);

		if (!lpTarget->isGhost)
		{
			if (lpTarget->Life < (damage - ShieldDamage))
			{
				lpTarget->Life = 0;
			}
			else
			{
				lpTarget->Life -= damage - ShieldDamage;
			}
		}

		if (lpTarget->Shield < ShieldDamage)
		{
			lpTarget->Shield = 0;
		}
		else
		{
			lpTarget->Shield -= ShieldDamage;
		}

		GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
	}
	else
	{
		if (lpTarget->Life < damage)
		{
			lpTarget->Life = 0;

		}
		else
		{
			lpTarget->Life -= damage;
		}
	}

	if (damage > 0)
	{
		gObjectManager.CharacterLifeCheck(lpObj, lpTarget, (damage - ShieldDamage), 4, flag, effect, ((lpSkill == 0) ? 0 : lpSkill->m_index), ShieldDamage);
	}
	else
	{
		GCElementalDamageSend(lpObj->Index, lpTarget->Index, (BYTE)effect, 0);
	}

	if (lpObj->PentagramJewelOption.Punish > 0)
	{
		//if (send != 0)
		//{
		if (lpTarget->isBoss == 0 && effect >= 6)
		{
			int punishRate = (15 + lpObj->AddPunishRate - lpTarget->AddRadianceResistance);

			if (punishRate > 0 && (rand() % 100) < punishRate)
			{
				int nDamage = lpTarget->MaxLife * lpObj->PentagramJewelOption.Punish / 100;

				if (!lpTarget->isGhost)
				{
					if (lpTarget->Life > nDamage)
					{
						lpTarget->Life -= nDamage;
					}
					else
					{
						lpTarget->Life = 0.0;
					}
				}

				gObjectManager.CharacterLifeCheck(lpObj, lpTarget, nDamage, 1, 0, send, 0, 0);
				GCElementalDamageSend(lpObj->Index, lpTarget->Index, (BYTE)effect, nDamage);
			}
		}
		//}
	}

	if (gMUFC.Status == MUFC_STARTED)
	{
		if (gMUFC.FighterID[0] == lpObj->Index)
		{
			gMUFC.FighterDamage[0] += damage;
		}
		else if (gMUFC.FighterID[1] == lpObj->Index)
		{
			gMUFC.FighterDamage[1] += damage;
		}
	}

	return 1;
}

bool CAttack::DecreaseArrow(LPOBJ lpObj) // OK
{
	return 1;

	if (lpObj->Type != OBJECT_USER || lpObj->Class != CLASS_FE)
	{
		return 1;
	}

	if (gEffectManager.CheckEffect(lpObj, EFFECT_INFINITY_ARROW) != 0 || gEffectManager.CheckEffect(lpObj, EFFECT_INFINITY_ARROW_IMPROVED) != 0)
	{
		return 1;
	}

	if (lpObj->Inventory[0].m_Index >= GET_ITEM(4, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(5, 0))
	{
		if ((lpObj->Inventory[1].m_Index >= GET_ITEM(4, 32) && lpObj->Inventory[1].m_Index <= GET_ITEM(4, 35)))
		{
			return 1;
		}

		if (lpObj->Inventory[1].m_Index == GET_ITEM(4, 15) || lpObj->Inventory[1].m_Index == GET_ITEM(4, 7) || lpObj->Inventory[1].m_Durability >= 1)
		{
			gItemManager.DecreaseItemDur(lpObj, 1, 1);
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

void CAttack::WingSprite(LPOBJ lpObj, LPOBJ lpTarget, long long* damage) // OK
{
	if (lpObj != 0 && lpObj->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpObj->Inventory[7];

		if (lpItem->IsItem() != 0 && lpItem->m_Durability > 0)
		{
			/*if (lpObj->Class == CLASS_DW || lpObj->Class == CLASS_FE || lpObj->Class == CLASS_SU)
			{
				lpObj->Life -= 1;
			}
			else
			{
				lpObj->Life -= 3;
			}*/

			GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);

			if ((lpItem->m_Index >= GET_ITEM(12, 0) && lpItem->m_Index <= GET_ITEM(12, 2)) || lpItem->m_Index == GET_ITEM(12, 41)) // 1st wing
			{
				(*damage) = ((*damage) * (112 + (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 269) || (lpItem->m_Index >= GET_ITEM(12, 3) && lpItem->m_Index <= GET_ITEM(12, 6)) || lpItem->m_Index == GET_ITEM(12, 42) || //2nd Wing normal
				(lpItem->m_Index >= GET_ITEM(12, 422) && lpItem->m_Index <= GET_ITEM(12, 429))) //2nd Wing Bound
			{
				(*damage) = ((*damage) * (132 + (lpItem->m_Level * 1))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 270) || (lpItem->m_Index >= GET_ITEM(12, 36) && lpItem->m_Index <= GET_ITEM(12, 40)) || lpItem->m_Index == GET_ITEM(12, 43) || lpItem->m_Index == GET_ITEM(12, 50) ||  // 3rd wing
				(lpItem->m_Index >= GET_ITEM(12, 430) && lpItem->m_Index <= GET_ITEM(12, 437))) // 3rd wing Bound
			{
				(*damage) = ((*damage) * (139 + (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 49)) // Cloak of Fighter
			{
				(*damage) = ((*damage) * (120 + (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index >= GET_ITEM(12, 130) && lpItem->m_Index <= GET_ITEM(12, 135)) // Mini Wings
			{
				(*damage) = ((*damage) * (112 + (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 262)) // Cloak of Death
			{
				(*damage) = ((*damage) * (121 + (lpItem->m_Level * 1))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 263)) // Wings of Chaos
			{
				(*damage) = ((*damage) * (133 + (lpItem->m_Level * 1))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 264)) // Wings of Magic
			{
				(*damage) = ((*damage) * (135 + (lpItem->m_Level * 1))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 265)) // Wings of Life
			{
				(*damage) = ((*damage) * (135 + (lpItem->m_Level * 1))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 266)) // Wings of Conqueror
			{
				(*damage) = ((*damage) * 170) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 267)) // Wings of Angel and Devil
			{
				(*damage) = ((*damage) * (160 + lpItem->m_Level)) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(13, 30)) // Cloak of Lord
			{
				(*damage) = ((*damage) * (120 + (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index >= GET_ITEM(12, 414) && lpItem->m_Index <= GET_ITEM(12, 421)) //4th Wings
			{
				(*damage) = ((*damage) * (155 + lpItem->m_Level)) / 100;
			}
			/*else if (gCustomWing.CheckCustomWingByItem(lpItem->m_Index) != 0)
			{
				(*damage) = ((*damage) * gCustomWing.GetCustomWingIncDamage(lpItem->m_Index, lpItem->m_Level)) / 100;
			}*/
		}
	}

	if (lpTarget != 0 && lpTarget->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpTarget->Inventory[7];

		if (lpItem->IsItem() != 0 && lpItem->m_Durability > 0)
		{
			if ((lpItem->m_Index >= GET_ITEM(12, 0) && lpItem->m_Index <= GET_ITEM(12, 2)) || lpItem->m_Index == GET_ITEM(12, 41)) // 1st wing
			{
				(*damage) = ((*damage) * (88 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 269) || (lpItem->m_Index >= GET_ITEM(12, 3) && lpItem->m_Index <= GET_ITEM(12, 6)) || lpItem->m_Index == GET_ITEM(12, 42) || // 2nd wing
				(lpItem->m_Index >= GET_ITEM(12, 422) && lpItem->m_Index <= GET_ITEM(12, 429))) //2nd Wing Bound
			{
				(*damage) = ((*damage) * (75 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 270) || (lpItem->m_Index >= GET_ITEM(12, 36) && lpItem->m_Index <= GET_ITEM(12, 40)) || lpItem->m_Index == GET_ITEM(12, 43) || lpItem->m_Index == GET_ITEM(12, 50) || // 3rd wing
				(lpItem->m_Index >= GET_ITEM(12, 430) && lpItem->m_Index <= GET_ITEM(12, 437))) // 3rd wing Bound
			{
				(*damage) = ((*damage) * (61 - (lpItem->m_Level * 2))) / 100;
			}
			/*else if (lpItem->m_Index == GET_ITEM(12, 40)) // Mantle of Monarch
			{
				(*damage) = ((*damage) * (76 - (lpItem->m_Level * 2))) / 100;
			}*/
			else if (lpItem->m_Index == GET_ITEM(12, 49)) // Cloak of Fighter
			{
				(*damage) = ((*damage) * (90 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index >= GET_ITEM(12, 130) && lpItem->m_Index <= GET_ITEM(12, 135)) // Mini Wings
			{
				(*damage) = ((*damage) * (88 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 262)) // Cloak of Death
			{
				(*damage) = ((*damage) * (87 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 263)) // Wings of Chaos
			{
				(*damage) = ((*damage) * (70 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 264)) // Wings of Magic
			{
				(*damage) = ((*damage) * (71 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 265)) // Wings of Life
			{
				(*damage) = ((*damage) * (71 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 266)) // Wings of Conqueror
			{
				(*damage) = ((*damage) * 27) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 267)) // Wings of Angel and Devil
			{
				(*damage) = ((*damage) * (40 - lpItem->m_Level)) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(13, 30)) // Cloak of Lord
			{
				(*damage) = ((*damage) * (90 - lpItem->m_Level)) / 100;
			}
			else if ((lpItem->m_Index >= GET_ITEM(12, 414) && lpItem->m_Index <= GET_ITEM(12, 417)) || (lpItem->m_Index >= GET_ITEM(12, 419) && lpItem->m_Index <= GET_ITEM(12, 421))) //4th Wings
			{
				(*damage) = ((*damage) * (57 - (lpItem->m_Level * 2))) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(12, 418)) // DL 4th cape
			{
				(*damage) = ((*damage) * (63 - (lpItem->m_Level * 2))) / 100;
			}
		}
	}
}

void CAttack::HelperSprite(LPOBJ lpObj, LPOBJ lpTarget, long long* damage) // OK
{
	if (lpObj != 0 && lpObj->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpObj->Inventory[8];

		/*if (lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 3) && lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_JewelOfHarmonyOption == 1) // Dinorant
		{
			lpObj->Life -= 1;

			GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);

			(*damage) = ((*damage) * (100 + gServerInfo.m_DinorantIncDamageConstA)) / 100;
		}*/
		/*else if (lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 37) && lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_JewelOfHarmonyOption == 1)
		{
			if ((lpObj->Inventory[lpObj->m_btInvenPetPos].m_NewOption & 1) != 0 || (lpObj->Inventory[lpObj->m_btInvenPetPos].m_NewOption & 4) != 0)
			{
				(*damage) = ((*damage) * (100 + gServerInfo.m_BlackFenrirIncDamageConstA)) / 100;
			}
		}*/

		if (lpItem->IsItem() != 0 && lpItem->m_Durability > 0)
		{
			if (lpItem->m_Index == GET_ITEM(13, 1)) // Satan
			{
				/*lpObj->Life -= 3;

				GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);*/

				(*damage) = ((*damage) * (100 + gServerInfo.m_SatanIncDamageConstA)) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(13, 64)) // Demon
			{
				/*lpObj->Life -= 4;
				GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);*/
				(*damage) = ((*damage) * (100 + gServerInfo.m_DemonIncDamageConstA)) / 100;
			}
			else if (lpItem->m_Index == GET_ITEM(13, 123)) // Skeleton
			{
				/*lpObj->Life -= 2;
				GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);*/
				(*damage) = ((*damage) * (100 + gServerInfo.m_SkeletonIncDamageConstA)) / 100;
			}
		}
	}

	if (lpTarget != 0 && lpTarget->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpTarget->Inventory[8];

		if (lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_Index == GET_ITEM(13, 4) && lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_JewelOfHarmonyOption == 1)
		{
			(*damage) = ((*damage) * (100 - ((gServerInfo.m_DarkHorseDecDamageConstA + 50/*lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_PetItemLevel*/) / gServerInfo.m_DarkHorseDecDamageConstB))) / 100;
		}

		/*if (lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_Index == GET_ITEM(13, 37) && lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_JewelOfHarmonyOption == 1)
		{
			if ((lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_NewOption & 2) != 0 || (lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_NewOption & 4) != 0)
			{
				(*damage) = ((*damage) * (100 - gServerInfo.m_BlueFenrirDecDamageConstA)) / 100;
			}
		}*/

		if (lpItem->IsItem() != 0 && lpItem->m_Durability > 0)
		{
			if (lpItem->m_Index == GET_ITEM(13, 0)) // Angel
			{
				(*damage) = ((*damage) * (100 - gServerInfo.m_AngelDecDamageConstA)) / 100;
			}
			/*else if (lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_Index == GET_ITEM(13, 3)) // Dinorant
			{
				(*damage) = ((*damage) * (100 - gServerInfo.m_DinorantDecDamageConstA - (((lpTarget->Inventory[lpTarget->m_btInvenPetPos].m_NewOption & 1) == 0) ? 0 : gServerInfo.m_DinorantDecDamageConstB))) / 100;
			}*/
			else if (lpItem->m_Index == GET_ITEM(13, 65)) // Maria
			{
				(*damage) = ((*damage) * (100 - gServerInfo.m_MariaDecDamageConstA)) / 100;
			}
		}
	}
}

void CAttack::DamageSprite(LPOBJ lpObj, int damage) // OK
{
	if (lpObj->Type != OBJECT_USER)
	{
		return;
	}

	CItem* lpItem = &lpObj->Inventory[8];

	float DurabilityValue = (1.0f / gServerInfo.m_GuardianDurabilityRate) * 100;
	DurabilityValue = (DurabilityValue / lpObj->m_MPSkillOpt.GuardianDurabilityRate) * 100;

	if (lpItem->IsItem() == 0 || lpItem->m_IsPeriodicItem != 0)
	{
		return;
	}

	if (lpItem->m_Index == GET_ITEM(13, 0)) // Angel
	{
		lpItem->m_Durability -= (damage * (3.0f * DurabilityValue)) / 100;
	}
	else if (lpItem->m_Index == GET_ITEM(13, 1)) // Satan
	{
		lpItem->m_Durability -= (damage * (2.0f * DurabilityValue)) / 100;
	}
	else if (lpItem->m_Index == GET_ITEM(13, 64)) // Demon
	{
		lpItem->m_Durability -= (damage * (1.0f * DurabilityValue)) / 100;
	}
	else if (lpItem->m_Index == GET_ITEM(13, 65)) // Maria
	{
		lpItem->m_Durability -= (damage * (2.0f * DurabilityValue)) / 100;
	}
	else if (lpItem->m_Index == GET_ITEM(13, 67)) // Rudolf
	{
		lpItem->m_Durability -= (damage * (1.0f * DurabilityValue)) / 100;
	}
	else if (lpItem->m_Index == GET_ITEM(13, 80)) // Panda
	{
		lpItem->m_Durability -= (damage * (1.0f * DurabilityValue)) / 100;
	}
	else if (lpItem->m_Index == GET_ITEM(13, 106)) // Unicorn
	{
		lpItem->m_Durability -= (damage * (1.0f * DurabilityValue)) / 100;
	}
	else if (lpItem->m_Index == GET_ITEM(13, 123)) // Skeleton
	{
		lpItem->m_Durability -= (damage * (1.0f * DurabilityValue)) / 100;
	}
	else
	{
		return;
	}

	gItemManager.GCItemDurSend(lpObj->Index, 8, (BYTE)lpItem->m_Durability, 0);

	if (lpItem->m_Durability < 1)
	{
		gItemManager.InventoryDelItem(lpObj->Index, 8);

		gItemManager.GCItemDeleteSend(lpObj->Index, 8, 0);

		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		gItemManager.GCItemChangeSend(lpObj->Index, 8);
	}
}

void CAttack::MountSprite(LPOBJ lpObj, int damage) // OK
{
	if (lpObj->Type != OBJECT_USER)
	{
		return;
	}

	CItem* lpItem = &lpObj->Inventory[lpObj->m_btInvenPetPos];

	float DurabilityValue = (1.0f / gServerInfo.m_GuardianDurabilityRate) * 100;
	DurabilityValue = (DurabilityValue / lpObj->m_MPSkillOpt.GuardianDurabilityRate) * 100;

	if (lpItem->IsItem() == 0 || lpItem->m_IsPeriodicItem != 0)
	{
		return;
	}

	if (lpItem->m_Index == GET_ITEM(13, 2) || lpItem->m_Index == GET_ITEM(13, 3)) // Uniria or Dinorant
	{
		lpItem->m_Durability -= (damage * (1.0f * DurabilityValue)) / 100;
		gItemManager.GCItemDurSend(lpObj->Index, lpObj->m_btInvenPetPos, (BYTE)lpItem->m_Durability, 0);
	}
	else if (lpItem->m_Index == GET_ITEM(13, 4)) // Dark Horse
	{
		if (this->DarkHorseSprite(lpObj, damage) == 0)
		{
			return;
		}
	}
	else if (lpItem->m_Index == GET_ITEM(13, 37)) // Fenrir
	{
		if (this->FenrirSprite(lpObj, damage) == 0)
		{
			return;
		}
	}
	else
	{
		return;
	}

	gItemManager.GCItemDurSend(lpObj->Index, lpObj->m_btInvenPetPos, (BYTE)lpItem->m_Durability, 0);

	if (lpItem->m_Durability < 1)
	{
		gItemManager.InventoryDelItem(lpObj->Index, lpObj->m_btInvenPetPos);

		gItemManager.GCItemDeleteSend(lpObj->Index, lpObj->m_btInvenPetPos, 0);

		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		gItemManager.GCItemChangeSend(lpObj->Index, lpObj->m_btInvenPetPos);

		if (lpObj->Map == MAP_ICARUS && lpObj->Inventory[7].IsItem() == 0 && (lpItem->m_Index == GET_ITEM(13, 3) || lpItem->m_Index == GET_ITEM(13, 37)))
		{
			gObjMoveGate(lpObj->Index, 22);
		}
	}
}

bool CAttack::DarkHorseSprite(LPOBJ lpObj, int damage) // OK
{
	CItem* lpItem = &lpObj->Inventory[lpObj->m_btInvenPetPos];

	if (lpItem->m_Durability < 1)
	{
		return 0;
	}

	lpItem->m_DurabilitySmall += ((damage * 2) / 100) + 1;

	int MaxSmallDur = (1500 * gServerInfo.m_PetDurabilityRate) / 100;

	MaxSmallDur = (MaxSmallDur * lpObj->m_MPSkillOpt.PetDurabilityRate) / 100;

	if (lpItem->m_DurabilitySmall > MaxSmallDur)
	{
		lpItem->m_Durability = (((--lpItem->m_Durability) < 1) ? 0 : lpItem->m_Durability);
		lpItem->m_DurabilitySmall = 0;

		if (lpItem->CheckDurabilityState() != 0)
		{
			gObjectManager.CharacterCalcAttribute(lpObj->Index);
		}

		gItemManager.GCItemDurSend(lpObj->Index, lpObj->m_btInvenPetPos, (BYTE)lpItem->m_Durability, 0);

		GCPetItemInfoSend(lpObj->Index, 1, 0, lpObj->m_btInvenPetPos, lpItem->m_PetItemLevel, lpItem->m_PetItemExp, (BYTE)lpItem->m_Durability);
	}

	return 0;
}

bool CAttack::FenrirSprite(LPOBJ lpObj, int damage) // OK
{
	CItem* lpItem = &lpObj->Inventory[lpObj->m_btInvenPetPos];

	if (lpItem->m_Durability < 1)
	{
		return 0;
	}

	lpItem->m_DurabilitySmall += ((damage * 2) / 100) + 1;

	int MaxSmallDur = (200 * gServerInfo.m_GuardianDurabilityRate) / 100;

	MaxSmallDur = (MaxSmallDur * lpObj->m_MPSkillOpt.GuardianDurabilityRate) / 100;

	if (lpItem->m_DurabilitySmall > MaxSmallDur)
	{
		lpItem->m_Durability = (((--lpItem->m_Durability) < 1) ? 0 : lpItem->m_Durability);
		lpItem->m_DurabilitySmall = 0;

		if (lpItem->CheckDurabilityState() != 0)
		{
			gObjectManager.CharacterCalcAttribute(lpObj->Index);
		}

		gItemManager.GCItemDurSend(lpObj->Index, lpObj->m_btInvenPetPos, (BYTE)lpItem->m_Durability, 0);
	}

	return 1;
}

void CAttack::WeaponDurabilityDown(LPOBJ lpObj, LPOBJ lpTarget) // OK
{
	if (lpObj->Type != OBJECT_USER)
	{
		return;
	}

	if (gEffectManager.GetEffect(lpObj, EFFECT_TALISMAN_OF_PROTECTION) != 0)
	{
		return;
	}

	for (int n = 0; n < 2; n++)
	{
		if (lpObj->Inventory[n].IsItem() != 0)
		{
			bool result = 0;

			switch ((lpObj->Inventory[n].m_Index / MAX_ITEM_TYPE))
			{
			case 0:
				result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index, lpTarget->Defense, 0);
				break;
			case 1:
				result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index, lpTarget->Defense, 0);
				break;
			case 2:
				result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index, lpTarget->Defense, 0);
				break;
			case 3:
				result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index, lpTarget->Defense, 0);
				break;
			case 4:
				result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index, lpTarget->Defense, 1);
				break;
			case 5:
				result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index, lpTarget->Defense, ((lpObj->Inventory[n].m_Slot == 0) ? 2 : 3));
				break;
			}

			if (result != 0)
			{
				gItemManager.GCItemDurSend(lpObj->Index, n, (BYTE)lpObj->Inventory[n].m_Durability, 0);
			}
		}
	}
}

void CAttack::ArmorDurabilityDown(LPOBJ lpObj, LPOBJ lpTarget) // OK
{
	if (lpTarget->Type != OBJECT_USER)
	{
		return;
	}

	if (gEffectManager.GetEffect(lpTarget, EFFECT_TALISMAN_OF_PROTECTION) != 0)
	{
		return;
	}

	int slot = 1 + (GetLargeRand() % 6);

	if (lpTarget->Inventory[slot].IsItem() != 0)
	{
		if (slot != 1 || (lpTarget->Inventory[slot].m_Index >= GET_ITEM(6, 0) && lpTarget->Inventory[slot].m_Index < GET_ITEM(7, 0)))
		{
			if (lpObj->DirectDurabilityDamage > 0 && (GetLargeRand() % 10000) < (lpObj->DirectDurabilityDamage * 100.0f))
			{
				if (lpTarget->Inventory[slot].m_Durability > 0)
				{
					lpTarget->Inventory[slot].m_Durability--;
					gItemManager.GCItemDurSend(lpTarget->Index, slot, (BYTE)lpTarget->Inventory[slot].m_Durability, 0);
				}
			}

			if (lpTarget->Inventory[slot].ArmorDurabilityDown(lpTarget->Index, lpObj->PhysiDamageMin) != 0)
			{
				gItemManager.GCItemDurSend(lpTarget->Index, slot, (BYTE)lpTarget->Inventory[slot].m_Durability, 0);
			}
		}
	}
}

bool CAttack::CheckPlayerTarget(LPOBJ lpObj, LPOBJ lpTarget) // OK
{
	if (lpObj->Type != OBJECT_USER || lpTarget->Type != OBJECT_USER)
	{
		return 1;
	}

	if (lpObj->MUFCFighter > 0 && lpTarget->MUFCFighter > 0 && gMUFC.Status != MUFC_STARTED)
	{
		return 0;
	}

	if (lpTarget->Authority == 32)
	{
		return 0;
	}

	if (lpTarget->Map == gCustomPvPTime.PvPtimeMap)
	{
		return 1;
	}

	//Evento Party vs Party
	if (lpObj->Map == MAP_ARENA && lpTarget->Map == MAP_ARENA)
	{
		if (lpObj->X >= 185 && lpObj->Y >= 8 && lpObj->X <= 207 && lpObj->Y <= 14)
		{
			if (lpTarget->X >= 185 && lpTarget->Y >= 8 && lpTarget->X <= 207 && lpTarget->Y <= 14)
			{
				if (lpObj->PartyNumber == lpTarget->PartyNumber)
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
		}
	}

	//Mapa da Temporada
	if (lpObj->Map == MAP_CRYWOLF && lpTarget->Map == MAP_CRYWOLF)
	{
		if (lpObj->PartyNumber > 0 && lpTarget->PartyNumber > 0 && lpObj->PartyNumber == lpTarget->PartyNumber)
		{
			return 0;
		}
	}

	if (lpObj->HelperDelayTime != 0 && gParty.IsMember(lpObj->PartyNumber, lpTarget->Index) != 0)
	{
		return 0;
	}

#if(GAMESERVER_TYPE==1)

	if (lpTarget->Map == MAP_CASTLE_SIEGE && lpTarget->X >= 30 && lpTarget->Y >= 30 && lpTarget->X <= 40 && lpTarget->Y <= 45)
	{
		return 0;
	}

	/*if (lpTarget->Map == MAP_CRYWOLF && lpTarget->X >= 96 && lpTarget->Y >= 0 && lpTarget->X <= 145 && lpTarget->Y <= 55)
	{
		return 0;
	}*/

	if (lpTarget->Map == MAP_CRYWOLF)
	{
		if (gCrywolf.GetCrywolfState() == CRYWOLF_STATE_START || gCrywolf.GetCrywolfState() == CRYWOLF_STATE_READY)
		{
			for (int i = 205; i <= 209; i++)
			{
				if (gCrywolfAltar.GetAltarUserIndex(i) == lpTarget->Index)
				{
					return 0;
				}
			}
		}
	}

	if (lpObj->Map == MAP_CASTLE_SIEGE && lpTarget->Map == MAP_CASTLE_SIEGE && gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (lpObj->CsJoinSide != 0 && lpTarget->CsJoinSide != 0)
		{
			if (lpObj->CsJoinSide == lpTarget->CsJoinSide)
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}

#endif

	if (gMapManager.GetPvPZone(lpTarget) == 0)
	{
		return 1;
	}

	if (gObjGetRelationShip(lpObj, lpTarget) == 2 && gMapManager.GetMapNonPK(lpTarget->Map) == 0)
	{
		return 1;
	}

	if (lpObj->Guild != 0 && lpTarget->Guild != 0)
	{
		if (lpObj->Guild->WarState != 0 && lpTarget->Guild->WarState != 0)
		{
			if (lpObj->Guild->Number == lpTarget->Guild->Number)
			{
				return 0;
			}
		}
	}

	if (gObjTargetGuildWarCheck(lpObj, lpTarget) == 0)
	{
		if (lpTarget->Guild != 0 && lpTarget->Guild->WarState != 0)
		{
			if (lpTarget->Guild->WarType == 1 && lpTarget->Map != MAP_ARENA && gMapManager.GetMapNonPK(lpTarget->Map) == 0)
			{
				return 1;
			}

			if (CA_MAP_RANGE(lpTarget->Map) == 0 && CC_MAP_RANGE(lpTarget->Map) == 0 && IT_MAP_RANGE(lpTarget->Map) == 0 && gDuel.CheckDuel(lpObj, lpTarget) == 0 && gGensSystem.CheckGens(lpObj, lpTarget) == 0)
			{
				return 0;
			}
		}
	}

	if (gDuel.CheckDuel(lpObj, lpTarget) == 0 && OBJECT_RANGE(lpTarget->DuelUser) != 0)
	{
		return 0;
	}

	if (gMapManager.GetMapGensBattle(lpObj->Map) != 0 && gMapManager.GetMapGensBattle(lpTarget->Map) != 0)
	{
		return ((gMapManager.GetMapNonPK(lpTarget->Map) == 0) ? gGensSystem.CheckGens(lpObj, lpTarget) : 0);
	}

	if (CA_MAP_RANGE(lpObj->Map) != 0 && CA_MAP_RANGE(lpTarget->Map) != 0)
	{
		return ((gCustomArena.CheckPlayerTarget(lpObj, lpTarget) == 0) ? 0 : 1);
	}

	if (DS_MAP_RANGE(lpObj->Map) != 0 && DS_MAP_RANGE(lpTarget->Map) != 0)
	{
		return 0;
	}

	if (BC_MAP_RANGE(lpObj->Map) != 0 && BC_MAP_RANGE(lpTarget->Map) != 0)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && CC_MAP_RANGE(lpTarget->Map) != 0)
	{
		return ((gChaosCastle.GetState(GET_CC_LEVEL(lpObj->Map)) == CC_STATE_START) ? 1 : 0);
	}

	if (lpObj->Map == MAP_KANTURU3 && lpTarget->Map == MAP_KANTURU3)
	{
		return 0;
	}

	if (IT_MAP_RANGE(lpObj->Map) != 0 && IT_MAP_RANGE(lpTarget->Map) != 0)
	{
		return gIllusionTemple.CheckPlayerTarget(lpObj, lpTarget);
	}

	if (lpObj->Map == MAP_RAKLION2 && lpTarget->Map == MAP_RAKLION2)
	{
		return 0;
	}

	if (DG_MAP_RANGE(lpObj->Map) != 0 && DG_MAP_RANGE(lpTarget->Map) != 0)
	{
		return 0;
	}

	if (IG_MAP_RANGE(lpObj->Map) != 0 && IG_MAP_RANGE(lpTarget->Map) != 0)
	{
		return 0;
	}

	if (gMapManager.GetMapNonPK(lpTarget->Map) != 0)
	{
		return 0;
	}

	return 1;
}

void CAttack::MissSend(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int send, int count) // OK
{
	WORD effect = 0;

	if (count > 0 && lpSkill != 0 && (lpSkill->m_skill == SKILL_UPPER_BEAST || lpSkill->m_skill == SKILL_DARK_SIDE || lpSkill->m_skill == SKILL_DARK_PHOENIX_SHOT || lpSkill->m_skill == SKILL_SPIRIT_HOOK))
	{
		effect |= (((count % 2) == 0) ? 0x20 : 0x10);
	}

	if (count > 0 && lpSkill != 0 && (lpSkill->m_skill == SKILL_KILLING_BLOW || lpSkill->m_skill == SKILL_CHAIN_DRIVER || lpSkill->m_skill == SKILL_DRAGON_LORE || lpSkill->m_skill == SKILL_PHOENIX_SHOT))
	{
		effect |= (((count % 4) == 0) ? 0x20 : 0x10);
	}

	GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, effect, 0);

	if (send != 0 && lpSkill != 0)
	{
		gSkillManager.GCSkillAttackSend(lpObj, lpSkill->m_index, lpTarget->Index, 0);
	}
}

bool CAttack::MissCheck(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int send, int count, BYTE* miss) // OK
{
	return 1;

	int AttackSuccessRate = lpObj->m_MPSkillOpt.AttackSuccessRate;

	AttackSuccessRate += lpObj->EffectOption.AddAttackSuccessRate;

	AttackSuccessRate += (AttackSuccessRate * lpObj->EffectOption.MulAttackSuccessRate) / 100;

	AttackSuccessRate -= (AttackSuccessRate * lpObj->EffectOption.DivAttackSuccessRate) / 100;

	AttackSuccessRate = ((AttackSuccessRate < 0) ? 0 : AttackSuccessRate);

	int DefenseSuccessRate = lpTarget->m_MPSkillOpt.DefenseSuccessRate;

	DefenseSuccessRate += lpTarget->EffectOption.AddDefenseSuccessRate;

	DefenseSuccessRate += (DefenseSuccessRate * lpTarget->EffectOption.MulDefenseSuccessRate) / 100;

	DefenseSuccessRate -= (DefenseSuccessRate * lpTarget->EffectOption.DivDefenseSuccessRate) / 100;

	DefenseSuccessRate = ((DefenseSuccessRate < 0) ? 0 : DefenseSuccessRate);

	if (AttackSuccessRate < DefenseSuccessRate)
	{
		(*miss) = 1;

		if ((GetLargeRand() % 100) >= 5)
		{
			this->MissSend(lpObj, lpTarget, lpSkill, send, count);
			return 0;
		}
	}
	else
	{
		(*miss) = 0;

		if ((GetLargeRand() % ((AttackSuccessRate == 0) ? 1 : AttackSuccessRate)) < DefenseSuccessRate)
		{
			this->MissSend(lpObj, lpTarget, lpSkill, send, count);
			return 0;
		}
	}

	return 1;
}

bool CAttack::MissCheckPvP(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int send, int count, BYTE* miss) // OK
{
	return 1;

	(*miss) = 0;

	int AttackSuccessRate = (int)(((100 * (((lpObj->m_MPSkillOpt.AttackSuccessRatePvP * 10000.0f) / (lpObj->m_MPSkillOpt.AttackSuccessRatePvP + lpTarget->m_MPSkillOpt.DefenseSuccessRatePvP)) / 10000.0f)) * gServerInfo.m_ShieldGaugeAttackRate) * (((lpObj->Level * 10000.0f) / (lpObj->Level + lpTarget->Level)) / 10000.0f));

	if ((lpTarget->Level - lpObj->Level) >= 100)
	{
		AttackSuccessRate -= 5;
	}
	else if ((lpTarget->Level - lpObj->Level) >= 200)
	{
		AttackSuccessRate -= 10;
	}
	else if ((lpTarget->Level - lpObj->Level) >= 300)
	{
		AttackSuccessRate -= 15;
	}

	if ((GetLargeRand() % 100) > AttackSuccessRate)
	{
		this->MissSend(lpObj, lpTarget, lpSkill, send, count);

		//LogAdd(LOG_DEBUG, "Line %d", __LINE__);

		if (gServerInfo.m_ShieldGaugeAttackComboMiss != 0 && lpObj->ComboSkill.m_index >= 0)
		{
			lpObj->ComboSkill.Init();
		}

		return 0;
	}

	return 1;
}

bool CAttack::MissCheckElemental(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int send, int count, BYTE* miss) // OK
{
	lpObj->ElementalAttackSuccessRate += (lpObj->Inventory[236].m_Index != GET_ITEM(12, 241)) ? 0 : (lpObj->Inventory[236].m_SocketOption[0] >= 0xFE) ? 0 : lpObj->PentagramOption.AddIntegrityRank1;
	lpObj->ElementalAttackSuccessRate += (lpObj->Inventory[236].m_Index != GET_ITEM(12, 261)) ? 0 : (lpObj->Inventory[236].m_SocketOption[0] >= 0xFE) ? 0 : lpObj->PentagramOption.AddRadianceRank1;

	int ElementalAttackSuccessRate = lpObj->ElementalAttackSuccessRate;

	if (lpObj->Type == OBJECT_USER)
	{
		ElementalAttackSuccessRate += lpObj->PentagramJewelOption.MulElementalAttackSuccessRatePVP;
	}

	else
	{
		ElementalAttackSuccessRate += lpObj->PentagramJewelOption.MulElementalAttackSuccessRatePVM;
	}

	ElementalAttackSuccessRate += (ElementalAttackSuccessRate * lpObj->PentagramOption.MulElementalAttackSuccessRate) / 100;

	ElementalAttackSuccessRate += (ElementalAttackSuccessRate * lpObj->PentagramJewelOption.MulElementalAttackSuccessRate) / 100;

	ElementalAttackSuccessRate = ((ElementalAttackSuccessRate < 0) ? 0 : ElementalAttackSuccessRate);

	lpTarget->ElementalDefenseSuccessRate += (lpTarget->Inventory[236].m_Index != GET_ITEM(12, 251)) ? 0 : (lpTarget->Inventory[236].m_SocketOption[0] >= 0xFE) ? 0 : lpTarget->PentagramOption.AddDivinityRank1;
	lpTarget->ElementalDefenseSuccessRate += (lpTarget->Inventory[236].m_Index != GET_ITEM(12, 261)) ? 0 : (lpTarget->Inventory[236].m_SocketOption[0] >= 0xFE) ? 0 : lpTarget->PentagramOption.AddRadianceRank1;

	int ElementalDefenseSuccessRate = lpTarget->ElementalDefenseSuccessRate;

	if (lpTarget->Type == OBJECT_USER)
	{
		ElementalDefenseSuccessRate += lpTarget->PentagramJewelOption.MulElementalDefenseSuccessRatePVP;
	}
	else
	{
		ElementalDefenseSuccessRate += lpTarget->PentagramJewelOption.MulElementalDefenseSuccessRatePVM;
	}

	ElementalDefenseSuccessRate += (ElementalDefenseSuccessRate * lpTarget->PentagramOption.MulElementalDefenseSuccessRate) / 100;

	ElementalDefenseSuccessRate += (ElementalDefenseSuccessRate * lpTarget->PentagramJewelOption.MulElementalDefenseSuccessRate) / 100;

	ElementalDefenseSuccessRate = ((ElementalDefenseSuccessRate < 0) ? 0 : ElementalDefenseSuccessRate);

	if ((GetLargeRand() % 100) <= lpTarget->PentagramOption.MulPentagramDamagePerfectDodgeRate)
	{
		GCElementalDamageSend(lpObj->Index, lpTarget->Index, lpObj->ElementalAttribute, 0);
		return 0;
	}

	if (ElementalAttackSuccessRate < ElementalDefenseSuccessRate)
	{
		(*miss) = 1;

		if ((GetLargeRand() % 100) >= 5)
		{
			GCElementalDamageSend(lpObj->Index, lpTarget->Index, lpObj->ElementalAttribute, 0);
			return 0;
		}
	}
	else
	{
		(*miss) = 0;

		if ((GetLargeRand() % ((ElementalAttackSuccessRate == 0) ? 1 : ElementalAttackSuccessRate)) < ElementalDefenseSuccessRate)
		{
			GCElementalDamageSend(lpObj->Index, lpTarget->Index, lpObj->ElementalAttribute, 0);
			return 0;
		}
	}

	return 1;
}

bool CAttack::ApplySkillEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if (lpTarget->Type != OBJECT_USER && ((lpTarget->Class >= 204 && lpTarget->Class <= 209) || (lpTarget->Class >= 215 && lpTarget->Class <= 219) || lpTarget->Class == 277 || lpTarget->Class == 278 || lpTarget->Class == 283 || lpTarget->Class == 288))
	{
		return 0;
	}

	/*if (lpTarget->Type == OBJECT_USER && (gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE) != 0 || gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE_IMPROVED) != 0))
	{
		return 0;
	}*/

	if (gSkillManager.GetSkillType(lpSkill->m_index) != -1 && gObjCheckResistance(lpTarget, gSkillManager.GetSkillType(lpSkill->m_index)) != 0)
	{
		return 0;
	}

	switch (lpSkill->m_skill)
	{
	case SKILL_POISON: //Decay
		if (gEffectManager.GetEffect(lpTarget, gSkillManager.GetSkillEffect(lpSkill->m_index)) == 0)
		{
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, lpObj->Index, 2, 3, 0);
		}
		break;
	case SKILL_METEORITE:
		gSkillManager.ApplyMeteoriteEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_LIGHTNING:
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 50, 0);
		break;
	case SKILL_ICE:
		if (gEffectManager.GetEffect(lpTarget, gSkillManager.GetSkillEffect(lpSkill->m_index)) == 0)
		{
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 5, 0, 0, 0, 0);
		}
		break;
	case SKILL_FALLING_SLASH:
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 50, 0);
		break;
	case SKILL_LUNGE:
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 50, 0);
		break;
	case SKILL_UPPERCUT:
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 50, 0);
		break;
	case SKILL_CYCLONE:
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 50, 0);
		break;
	case SKILL_SLASH:
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 50, 0);
		break;
	case SKILL_DECAY:
		if (gEffectManager.GetEffect(lpTarget, gSkillManager.GetSkillEffect(lpSkill->m_index)) == 0)
		{
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, lpObj->Index, 2, 3, 0);
		}
		break;
	case SKILL_ICE_STORM:
		gSkillManager.ApplyIceStormEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_ICE_ARROW:
		if (gEffectManager.GetEffect(lpTarget, gSkillManager.GetSkillEffect(lpSkill->m_index)) == 0)
		{
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 3, 0, 0, 0, 0);
		}
		break;
	case SKILL_RAGEFUL_BLOW:
		gSkillManager.ApplyRagefulBlowEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_DEATH_STAB:
		gSkillManager.ApplyDeathStabEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FIRE_SLASH:
		gSkillManager.ApplyFireSlashEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FIRE_BURST:
		gSkillManager.ApplyFireBurstEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_PLASMA_STORM:
		gSkillManager.ApplyPlasmaStormEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FIRE_SCREAM:
		gSkillManager.ApplyFireScreamEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_EARTHQUAKE:
		gSkillManager.ApplyEarthquakeEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_DRAIN_LIFE:
		gSkillManager.ApplyDrainLifeEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_SAHAMUTT:
		gSkillManager.ApplySahamuttEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_NEIL:
		gSkillManager.ApplyNeilEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_GHOST_PHANTOM:
		gSkillManager.ApplyGhostPhantomEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_RED_STORM:
		if (gEffectManager.GetEffect(lpTarget, gSkillManager.GetSkillEffect(lpSkill->m_index)) == 0)
		{
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 1, 0, 0, 0, 0);
		}
		gSkillManager.ApplyRedStormEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FROZEN_STAB:
		gSkillManager.ApplyFrozenStabEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FIVE_SHOT:
		gSkillManager.ApplyFiveShotEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FLAME_STRIKE:
		gSkillManager.ApplySwordSlashEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_LIGHTNING_STORM:
		if (gEffectManager.GetEffect(lpTarget, gSkillManager.GetSkillEffect(lpSkill->m_index)) == 0)
		{
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 1, 0, 0, 0, 0);
		}
		gSkillManager.ApplyLightningStormEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_KILLING_BLOW:
		gSkillManager.ApplyKillingBlowEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_UPPER_BEAST:
		gSkillManager.ApplyBeastUppercutEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_CHAIN_DRIVER:
		gSkillManager.ApplyChainDriverEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_DRAGON_LORE:
		gSkillManager.ApplyDragonLoreEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_DRAGON_SLAYER:
		gSkillManager.ApplyDragonSlayerEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_PHOENIX_SHOT:
		gSkillManager.ApplyPhoenixShotEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_POISON_ARROW:
		if (gEffectManager.GetEffect(lpTarget, gSkillManager.GetSkillEffect(lpSkill->m_index)) == 0)
		{
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, lpObj->Index, 1, (lpObj->PhysiDamageMinLeft / 10), 0); //TODO config poison arrow
		}
		break;
	case SKILL_EARTH_PRISON:
		gSkillManager.ApplyEarthPrisonEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_MAGIC_PIN:
		gSkillManager.ApplyMagicPinEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_ICE_BLOOD:
		gSkillManager.ApplyIceBloodEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FIRE_BLOOD:
		gSkillManager.ApplyFireBloodEffect(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_DW_METEOR_STRIKE:
		gSkillManager.ApplyMeteoriteEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_SHOCKING_BOOM:
		gSkillManager.ApplyShockingEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_EVIL_SPIRIT:
		gSkillManager.ApplyEvilSpiritEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FREEZING_SLAUGHTER:
		gSkillManager.ApplyFreezingSlaughterEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_METEOR_STORM:
		gSkillManager.ApplyMeteorStormEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case FOR_MASTER_SKILL_ADD_POISONING_STORM:
		gSkillManager.ApplyPoisoningStormEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case FOR_MASTER_SKILL_ADD_BLEEDING_HEART:
		gSkillManager.ApplyBleedingHeartEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FIRE_BLOW:
		gSkillManager.ApplyFireBlowEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_TRIPLE_SHOT:
		gSkillManager.ApplyTripleShotEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FOCUS_SHOT:
		gSkillManager.ApplyFocusShotEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_DARK_BLAST:
		gSkillManager.ApplyDarkBlastEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_BIRDS:
		gSkillManager.ApplyBirdsEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_WIND_SOUL:
		gSkillManager.ApplyWindSoulEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_AQUA_BEAST:
		gSkillManager.ApplyAquaBeastEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_FIRE_BEAST:
		gSkillManager.ApplyFireBeastEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_DARK_SIDE:
		gSkillManager.ApplyDarkSideEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_BRECHE:
		gSkillManager.ApplyBrecheEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	case SKILL_SHINING_PEAK:
		gSkillManager.ApplyShiningPeakEffect_4th(lpObj, lpTarget, lpSkill, damage);
		break;
	}

	return 1;
}

int CAttack::GetTargetDefense(LPOBJ lpObj, LPOBJ lpTarget, WORD* effect) // OK
{
	int defense = lpTarget->Defense;

	defense += lpTarget->EffectOption.AddDefense;
	defense -= lpTarget->EffectOption.SubDefense;

	gSkillManager.SkillSwordPowerGetDefense(lpObj->Index, &defense);

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		defense += lpTarget->DefensePvP;
	}

	if (lpTarget->MonsterSkillElementOption.CheckDefenseTime() != 0)
	{
		defense += lpTarget->MonsterSkillElementOption.m_SkillElementDefense;
	}

	if (lpTarget->EffectOption.MulDefense > 0)
	{
		defense += (defense * lpTarget->EffectOption.MulDefense) / 100;
	}

	if (lpTarget->EffectOption.DivDefense > 0)
	{
		defense -= (defense * lpTarget->EffectOption.DivDefense) / 100;
	}

	/*if (lpTarget->Type == OBJECT_USER)
	{
		defense = (defense * 50) / 100;
	}*/

	//LogAdd(LOG_DEBUG, "Ignore: %d", (lpObj->m_MPSkillOpt.IgnoreDefenseRate + lpObj->EffectOption.AddIgnoreDefenseRate));

	if (!IsPvPTestOn && (GetLargeRand() % 100) < ((lpObj->m_MPSkillOpt.IgnoreDefenseRate + lpObj->EffectOption.AddIgnoreDefenseRate) - lpTarget->ResistIgnoreDefenseRate))
	{
		(*effect) = 1;
		defense = 0;
		defense += lpTarget->ImpenetrableDefense;
	}

	defense = ((defense < 0) ? 0 : defense);

	//LogAdd(LOG_DEBUG, "Defesa de %s: %d", lpTarget->Name, defense);

	return defense;
}

int CAttack::GetTargetElementalDefense(LPOBJ lpObj, LPOBJ lpTarget, WORD* effect) // OK
{
	lpTarget->ElementalDefense += (lpTarget->Inventory[236].m_Index != GET_ITEM(12, 261)) ? 0 : (lpTarget->Inventory[236].m_SocketOption[0] >= 0xFE) ? 0 : lpTarget->PentagramOption.AddRadianceRank1;

	int defense = lpTarget->ElementalDefense;

	defense += (defense / 100) * lpTarget->PentagramJewelOption.AddElementalDefenseRatio;

	if (lpTarget->Type == OBJECT_USER)
	{
		defense += ((lpTarget->Inventory[236].m_Index != GET_ITEM(12, 231)) ? 0 : (lpTarget->Inventory[236].m_SocketOption[0] >= 0xFE) ? 0 : lpTarget->PentagramOption.AddBlessingRank1);

		defense += (((lpTarget->Inventory[236].IsItem() == 0) ? 0 : lpTarget->Inventory[236].m_Defense) * lpTarget->PentagramOption.MulPentagramDefense) / 100;

		defense += (lpTarget->Defense * lpTarget->PentagramOption.AddElementalDefenseTransferRate) / 100;

		defense += (lpTarget->Defense * lpTarget->PentagramJewelOption.AddElementalDamageReductionPvP) / 100;

		defense += lpTarget->PentagramJewelOption.AddElementalDefense;

		if (lpObj->Type == OBJECT_USER)
		{
			defense += lpTarget->PentagramJewelOption.AddElementalDefensePvP;

			/*if (lpObj->Class == CLASS_DW || lpObj->Class == CLASS_FE || lpObj->Class == CLASS_MG || lpObj->Class == CLASS_SU)
			{
				defense += lpTarget->PentagramJewelOption.AddElementalDefenseRange;
			}
			else
			{
				defense += lpTarget->PentagramJewelOption.AddElementalDefenseMelee;
			}*/
		}
		else
		{
			defense += lpTarget->PentagramJewelOption.AddElementalDefensePvM;
		}
	}

	gPentagramSystem.GetPentagramRelationshipDefense(lpTarget, lpObj, &defense);

	defense = ((defense < 0) ? 0 : defense);

	return defense;
}

int CAttack::GetAttEngraving(LPOBJ lpObj, CSkill* lpSkill, int damage) // OK
{
	if (lpObj->ElementalAttribute != 0)
	{
		if (lpObj->ElementalAttribute == gSkillManager.GetSkillElement(lpSkill->m_index))
		{
			return damage += 9 + lpObj->Inventory[236].m_Level * 3;
		}
	}

	return 0;
}

int CAttack::GetAttackDamage(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, WORD* effect, int TargetDefense) // OK
{
	CItem* Right = &lpObj->Inventory[0];
	CItem* Left = &lpObj->Inventory[1];

	int damage = 0;
	int DamageMin = 0;
	int DamageMax = 0;
	int SkillDamageMin = 0;
	int SkillDamageMax = 0;
	bool DualHandWeapon = 0;

	if (lpObj->Type == OBJECT_MONSTER || lpObj->Type == OBJECT_NPC)
	{
		DamageMin = lpObj->PhysiDamageMin;
		DamageMax = lpObj->PhysiDamageMax;

		DamageMin += lpObj->EffectOption.AddPhysiDamage;
		DamageMax += lpObj->EffectOption.AddPhysiDamage;

		DamageMin += lpObj->EffectOption.AddMinPhysiDamage;
		DamageMax += lpObj->EffectOption.AddMaxPhysiDamage;

		DamageMin += (DamageMin * lpObj->EffectOption.MulPhysiDamage) / 100;
		DamageMax += (DamageMax * lpObj->EffectOption.MulPhysiDamage) / 100;

		DamageMin -= (DamageMin * lpObj->EffectOption.DivPhysiDamage) / 100;
		DamageMax -= (DamageMax * lpObj->EffectOption.DivPhysiDamage) / 100;

		int range = (DamageMax - DamageMin);

		range = ((range < 1) ? 1 : range);

		damage = DamageMin + (GetLargeRand() % range);
	}
	else
	{
		if (lpObj->Class == CLASS_FE || lpObj->Class == CLASS_DK || lpObj->Class == CLASS_MG || lpObj->Class == CLASS_RF || lpObj->Class == CLASS_SU)
		{
			if (Right->m_Index >= GET_ITEM(0, 0) && Right->m_Index < GET_ITEM(6, 0) && Left->m_Index >= GET_ITEM(0, 0) && Left->m_Index < GET_ITEM(6, 0))
			{
				if (Right->m_IsValidItem != 0 && Left->m_IsValidItem != 0)
				{
					DualHandWeapon = 1;
				}
			}
		}

		if (lpSkill != 0)
		{
			SkillDamageMin = lpSkill->m_DamageMin;
			SkillDamageMax = lpSkill->m_DamageMax;

			SkillDamageMin += gMasterSkillTree.GetMasterSkillDamageMin(lpObj, lpSkill->m_skill);
			SkillDamageMax += gMasterSkillTree.GetMasterSkillDamageMax(lpObj, lpSkill->m_skill);

			SkillDamageMin = SkillDamageMin + this->GetAttEngraving(lpObj, lpSkill, lpSkill->m_DamageMin);
			SkillDamageMax = SkillDamageMax + this->GetAttEngraving(lpObj, lpSkill, lpSkill->m_DamageMin);

			SkillDamageMin += lpObj->SkillDamageBonus;
			SkillDamageMax += lpObj->SkillDamageBonus;

			int type = gSkillManager.GetSkillType(lpSkill->m_index);

			if (CHECK_RANGE(type, MAX_RESISTANCE_TYPE) != 0)
			{
				SkillDamageMin += lpObj->AddResistance[type];
				SkillDamageMax += lpObj->AddResistance[type];
			}

			if (lpObj->Class == CLASS_DL)
			{
				switch (lpSkill->m_skill)
				{
					/*case SKILL_EARTHQUAKE:
						SkillDamageMin += ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_EarthquakeDamageConstA) + ((lpObj->Leadership + lpObj->AddLeadership) / gServerInfo.m_EarthquakeDamageConstB) + (lpObj->Inventory[lpObj->m_btInvenPetPos].m_PetItemLevel * gServerInfo.m_EarthquakeDamageConstC);
						SkillDamageMax += ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_EarthquakeDamageConstA) + ((lpObj->Leadership + lpObj->AddLeadership) / gServerInfo.m_EarthquakeDamageConstB) + (lpObj->Inventory[lpObj->m_btInvenPetPos].m_PetItemLevel * gServerInfo.m_EarthquakeDamageConstC);
						break;
					case SKILL_ELECTRIC_SPARK:
						SkillDamageMin += ((lpObj->Leadership + lpObj->AddLeadership) / gServerInfo.m_ElectricSparkDamageConstA) + gServerInfo.m_ElectricSparkDamageConstB;
						SkillDamageMax += ((lpObj->Leadership + lpObj->AddLeadership) / gServerInfo.m_ElectricSparkDamageConstA) + gServerInfo.m_ElectricSparkDamageConstB;
						break;*/
				default:
					SkillDamageMin += ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_DLSkillDamageConstA) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DLSkillDamageConstB);
					SkillDamageMax += ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_DLSkillDamageConstA) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DLSkillDamageConstB);
					break;
				}
			}
		}

		if (DualHandWeapon != 0)
		{
			DamageMin = lpObj->PhysiDamageMinRight + lpObj->PhysiDamageMinLeft + SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxRight + lpObj->PhysiDamageMaxLeft + SkillDamageMax;
		}
		else if ((Right->m_Index >= GET_ITEM(0, 0) && Right->m_Index < GET_ITEM(4, 0)) || (Right->m_Index >= GET_ITEM(5, 0) && Right->m_Index < GET_ITEM(6, 0)))
		{
			DamageMin = lpObj->PhysiDamageMinRight + SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxRight + SkillDamageMax;
		}
		else if (Right->m_Index >= GET_ITEM(4, 0) && Right->m_Index < GET_ITEM(5, 0) && (Left->m_Index != GET_ITEM(4, 7) || Left->m_Index != GET_ITEM(4, 15) || Left->m_Index != GET_ITEM(4, 32) || Left->m_Index != GET_ITEM(4, 33) || Left->m_Index != GET_ITEM(4, 34) || Left->m_Index != GET_ITEM(4, 35)) && Left->m_Slot == 0)
		{
			DamageMin = lpObj->PhysiDamageMinLeft + SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxLeft + SkillDamageMax;
		}
		else
		{
			DamageMin = lpObj->PhysiDamageMinLeft + SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxLeft + SkillDamageMax;
		}

		//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

		DamageMin += lpObj->EffectOption.AddPhysiDamage;
		DamageMax += lpObj->EffectOption.AddPhysiDamage;

		//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

		DamageMin += lpObj->EffectOption.AddMinPhysiDamage;
		DamageMax += lpObj->EffectOption.AddMaxPhysiDamage;

		//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

		gSkillManager.SkillSwordPowerGetPhysiDamage(lpObj->Index, &DamageMin, &DamageMax);

		//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

		DamageMin += (DamageMin * lpObj->EffectOption.MulPhysiDamage) / 100;
		DamageMax += (DamageMax * lpObj->EffectOption.MulPhysiDamage) / 100;

		//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

		DamageMin -= (DamageMin * lpObj->EffectOption.DivPhysiDamage) / 100;
		DamageMax -= (DamageMax * lpObj->EffectOption.DivPhysiDamage) / 100;

		//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

		int range = (DamageMax - DamageMin);

		range = ((range < 1) ? 1 : range);

		damage = DamageMin + (GetLargeRand() % range);

		//LogAdd(LOG_BLUE, "damage: %d [%d]", damage, __LINE__);

		int CriticalDamageRate = (((lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate) > gServerInfo.m_CriticalDamageRate) ? gServerInfo.m_CriticalDamageRate : (lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate));

		if ((GetLargeRand() % 100) < (CriticalDamageRate - lpTarget->ResistCriticalDamageRate))
		{
			(*effect) = 3;

			damage = DamageMax;
			damage += (damage * lpObj->CriticalDamagePerc / 100.0f);
			damage += lpObj->CriticalDamage;
			damage += lpObj->EffectOption.AddCriticalDamage;
		}

		//LogAdd(LOG_BLUE, "damage: %d [%d]", damage, __LINE__);

		int ExcellentDamageRate = (((lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate) > gServerInfo.m_ExcellentDamageRate) ? gServerInfo.m_ExcellentDamageRate : (lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate));

		if (IsPvPTestOn || lpObj->ExcellentDamageOnly || (GetLargeRand() % 100) < (ExcellentDamageRate - lpTarget->ResistExcellentDamageRate))
		{
			(*effect) = 2;

			damage = (DamageMax * 110) / 100;
			damage += (damage * lpObj->ExcellentDamagePerc / 100.0f);
			damage += lpObj->ExcellentDamage;
		}

		//LogAdd(LOG_BLUE, "damage: %d [%d]", damage, __LINE__);
	}
	//LogAdd(LOG_BLUE, "damage: %d [%d]", damage, __LINE__);

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage += lpObj->DamagePvP;
	}
	//LogAdd(LOG_BLUE, "damage: %d [%d]", damage, __LINE__);

	if (lpObj->MonsterSkillElementOption.CheckAttackTime() != 0)
	{
		damage += lpObj->MonsterSkillElementOption.m_SkillElementAttack;
	}

	//LogAdd(LOG_RED, "damage: %d | defense: %d",damage,TargetDefense);
	//LogAdd(LOG_BLUE, "damage: %d [%d]", damage, __LINE__);

	float TrueDamageRate = (float)((GetLargeRand() % 10000) / 100.0f);

	if ((lpObj->TrueDamageRate - lpTarget->TrueDamageProtection) > 0 && TrueDamageRate <= (lpObj->TrueDamageRate - lpTarget->TrueDamageProtection))
	{
		(*effect) = 1;
		(*effect) |= 0x80;
		return (int)(damage * 2);
	}

	damage -= TargetDefense;

	damage = ((damage < 0) ? 0 : damage);

	return damage;
}

int CAttack::GetAttackDamageWizard(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, WORD* effect, int TargetDefense) // OK
{
	CItem* Right = &lpObj->Inventory[0];
	CItem* Left = &lpObj->Inventory[1];

	int DamageMin = lpObj->MagicDamageMin;
	int DamageMax = lpObj->MagicDamageMax;

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)",lpObj->Name, DamageMax, __LINE__);

	if (lpSkill->m_skill == SKILL_NOVA && lpObj->SkillNovaCount >= 0)
	{
		DamageMin += (gServerInfo.m_NovaDamageConstA * (lpObj->SkillNovaCount * gServerInfo.m_NovaDamageConstB)) + ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_NovaDamageConstC);
		DamageMax += (gServerInfo.m_NovaDamageConstA * (lpObj->SkillNovaCount * gServerInfo.m_NovaDamageConstB)) + ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_NovaDamageConstC);
	}
	else
	{
		DamageMin += lpSkill->m_DamageMin;
		DamageMax += lpSkill->m_DamageMax;
	}

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	DamageMin += gMasterSkillTree.GetMasterSkillDamageMin(lpObj, lpSkill->m_skill);
	DamageMax += gMasterSkillTree.GetMasterSkillDamageMax(lpObj, lpSkill->m_skill);

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	DamageMin = DamageMin + this->GetAttEngraving(lpObj, lpSkill, lpSkill->m_DamageMin);
	DamageMax = DamageMax + this->GetAttEngraving(lpObj, lpSkill, lpSkill->m_DamageMin);

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	DamageMin += lpObj->SkillDamageBonus;
	DamageMax += lpObj->SkillDamageBonus;

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	DamageMin += lpObj->EffectOption.AddMagicDamage;
	DamageMax += lpObj->EffectOption.AddMagicDamage;

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	DamageMin += lpObj->EffectOption.AddMinMagicDamage;
	DamageMax += lpObj->EffectOption.AddMaxMagicDamage;

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	gSkillManager.SkillSwordPowerGetMagicDamage(lpObj->Index, &DamageMin, &DamageMax);

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	int type = gSkillManager.GetSkillType(lpSkill->m_index);

	if (CHECK_RANGE(type, MAX_RESISTANCE_TYPE) != 0)
	{
		DamageMin += lpObj->AddResistance[type];
		DamageMax += lpObj->AddResistance[type];
	}

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	DamageMin += (DamageMin * lpObj->EffectOption.MulMagicDamage) / 100;
	DamageMax += (DamageMax * lpObj->EffectOption.MulMagicDamage) / 100;

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	DamageMin -= (DamageMin * lpObj->EffectOption.DivMagicDamage) / 100;
	DamageMax -= (DamageMax * lpObj->EffectOption.DivMagicDamage) / 100;

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	if (Right->IsItem() != 0 && Right->m_IsValidItem != 0 && ((Right->m_Index >= GET_ITEM(0, 0) && Right->m_Index < GET_ITEM(1, 0)) || (Right->m_Index >= GET_ITEM(5, 0) && Right->m_Index < GET_ITEM(6, 0))))
	{
		int rise = (int)(((Right->m_MagicDamageRate / 100)));// +(Right->m_Level * 2)) * Right->m_CurrentDurabilityState);
		DamageMin += (DamageMin * rise) / 100;
		DamageMax += (DamageMax * rise) / 100;
	}

	//LogAdd(LOG_DEBUG, "[%s] DamageMax: %d (%d)", lpObj->Name, DamageMax, __LINE__);

	int range = (DamageMax - DamageMin);

	range = ((range < 1) ? 1 : range);

	int damage = DamageMin + (GetLargeRand() % range);

	int CriticalDamageRate = (((lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate) > gServerInfo.m_CriticalDamageRate) ? gServerInfo.m_CriticalDamageRate : (lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate));

	if ((GetLargeRand() % 100) < (CriticalDamageRate - lpTarget->ResistCriticalDamageRate))
	{
		(*effect) = 3;

		damage = DamageMax;
		damage += (damage * lpObj->CriticalDamagePerc / 100.0f);
		damage += lpObj->CriticalDamage;
		damage += lpObj->EffectOption.AddCriticalDamage;
	}

	int ExcellentDamageRate = (((lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate) > gServerInfo.m_ExcellentDamageRate) ? gServerInfo.m_ExcellentDamageRate : (lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate));

	if (IsPvPTestOn || lpObj->ExcellentDamageOnly || Right->m_Index == GET_ITEM(0, 51) || Right->m_Index == GET_ITEM(0, 60) || Right->m_Index == GET_ITEM(2, 25) || Right->m_Index == GET_ITEM(3, 26) || Right->m_Index == GET_ITEM(4, 30) || Right->m_Index == GET_ITEM(5, 49) || Right->m_Index == GET_ITEM(5, 50) ||
		(GetLargeRand() % 100) < (ExcellentDamageRate - lpTarget->ResistExcellentDamageRate))
	{
		(*effect) = 2;

		damage = (DamageMax * 110) / 100;
		damage += (damage * lpObj->ExcellentDamagePerc / 100.0f);
		damage += lpObj->ExcellentDamage;
	}

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage += lpObj->DamagePvP;
	}

	//LogAdd(LOG_DEBUG, "damage: %d | defense: %d", damage, TargetDefense);

	float TrueDamageRate = (float)((GetLargeRand() % 10000) / 100.0f);

	if ((lpObj->TrueDamageRate - lpTarget->TrueDamageProtection) > 0 && TrueDamageRate <= (lpObj->TrueDamageRate - lpTarget->TrueDamageProtection))
	{
		(*effect) = 1;
		(*effect) |= 0x80;
		return (int)(damage * 2);
	}

	damage -= TargetDefense;

	damage = ((damage < 0) ? 0 : damage);

	return damage;
}

int CAttack::GetAttackDamageCursed(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, WORD* effect, int TargetDefense) // OK
{
	CItem* Right = &lpObj->Inventory[0];
	CItem* Left = &lpObj->Inventory[1];

	int DamageMin = lpObj->CurseDamageMin;
	int DamageMax = lpObj->CurseDamageMax;

	DamageMin += lpSkill->m_DamageMin;
	DamageMax += lpSkill->m_DamageMax;

	DamageMin += gMasterSkillTree.GetMasterSkillDamageMin(lpObj, lpSkill->m_skill);
	DamageMax += gMasterSkillTree.GetMasterSkillDamageMax(lpObj, lpSkill->m_skill);

	DamageMin = DamageMin + this->GetAttEngraving(lpObj, lpSkill, lpSkill->m_DamageMin);
	DamageMax = DamageMax + this->GetAttEngraving(lpObj, lpSkill, lpSkill->m_DamageMin);

	DamageMin += lpObj->SkillDamageBonus;
	DamageMax += lpObj->SkillDamageBonus;

	DamageMin += lpObj->EffectOption.AddCurseDamage;
	DamageMax += lpObj->EffectOption.AddCurseDamage;

	DamageMin += lpObj->EffectOption.AddMinCurseDamage;
	DamageMax += lpObj->EffectOption.AddMaxCurseDamage;

	gSkillManager.SkillSwordPowerGetCurseDamage(lpObj->Index, &DamageMin, &DamageMax);

	int type = gSkillManager.GetSkillType(lpSkill->m_index);

	if (CHECK_RANGE(type, MAX_RESISTANCE_TYPE) != 0)
	{
		DamageMin += lpObj->AddResistance[type];
		DamageMax += lpObj->AddResistance[type];
	}

	DamageMin += (DamageMin * lpObj->EffectOption.MulCurseDamage) / 100;
	DamageMax += (DamageMax * lpObj->EffectOption.MulCurseDamage) / 100;

	DamageMin -= (DamageMin * lpObj->EffectOption.DivCurseDamage) / 100;
	DamageMax -= (DamageMax * lpObj->EffectOption.DivCurseDamage) / 100;

	if (Left->IsItem() != 0 && Left->m_IsValidItem != 0 && Left->m_Index >= GET_ITEM(5, 21) && Left->m_Index <= GET_ITEM(5, 23))
	{
		int rise = (int)(((Left->m_MagicDamageRate / 2) + (Left->m_Level * 2)) * Left->m_CurrentDurabilityState);
		DamageMin += (DamageMin * rise) / 100;
		DamageMax += (DamageMax * rise) / 100;
	}

	int range = (DamageMax - DamageMin);

	range = ((range < 1) ? 1 : range);

	int damage = DamageMin + (GetLargeRand() % range);

	int CriticalDamageRate = (((lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate) > gServerInfo.m_CriticalDamageRate) ? gServerInfo.m_CriticalDamageRate : (lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate));

	if ((GetLargeRand() % 100) < (CriticalDamageRate - lpTarget->ResistCriticalDamageRate))
	{
		(*effect) = 3;

		damage = DamageMax;
		damage += (damage * lpObj->CriticalDamagePerc / 100.0f);
		damage += lpObj->CriticalDamage;
		damage += lpObj->EffectOption.AddCriticalDamage;
	}

	int ExcellentDamageRate = (((lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate) > gServerInfo.m_ExcellentDamageRate) ? gServerInfo.m_ExcellentDamageRate : (lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate));

	if (IsPvPTestOn || ((GetLargeRand() % 100) < (ExcellentDamageRate - lpTarget->ResistExcellentDamageRate)))
	{
		(*effect) = 2;

		damage = (DamageMax * 110) / 100;
		damage += (damage * lpObj->ExcellentDamagePerc / 100.0f);
		damage += lpObj->ExcellentDamage;
		//damage += lpObj->EffectOption.AddExcellentDamage;
	}

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage += lpObj->DamagePvP;
	}

	float TrueDamageRate = (float)((GetLargeRand() % 10000) / 100.0f);

	if ((lpObj->TrueDamageRate - lpTarget->TrueDamageProtection) > 0 && TrueDamageRate <= (lpObj->TrueDamageRate - lpTarget->TrueDamageProtection))
	{
		(*effect) = 1;
		(*effect) |= 0x80;
		return (int)(damage * 2);
	}

	damage -= TargetDefense;

	damage = ((damage < 0) ? 0 : damage);

	return damage;
}

int CAttack::GetAttackDamageFenrir(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, WORD* effect, int TargetDefense) // OK
{
	int BaseDamage = 0;

	if (lpObj->Class == CLASS_DW)
	{
		BaseDamage = (lpObj->Strength / gServerInfo.m_DWPlasmaStormDamageConstA) + (lpObj->Dexterity / gServerInfo.m_DWPlasmaStormDamageConstB) + (lpObj->Vitality / gServerInfo.m_DWPlasmaStormDamageConstC) + (lpObj->Energy / gServerInfo.m_DWPlasmaStormDamageConstD);
	}
	else if (lpObj->Class == CLASS_DK)
	{
		BaseDamage = (lpObj->Strength / gServerInfo.m_DKPlasmaStormDamageConstA) + (lpObj->Dexterity / gServerInfo.m_DKPlasmaStormDamageConstB) + (lpObj->Vitality / gServerInfo.m_DKPlasmaStormDamageConstC) + (lpObj->Energy / gServerInfo.m_DKPlasmaStormDamageConstD);
	}
	else if (lpObj->Class == CLASS_FE)
	{
		BaseDamage = (lpObj->Strength / gServerInfo.m_FEPlasmaStormDamageConstA) + (lpObj->Dexterity / gServerInfo.m_FEPlasmaStormDamageConstB) + (lpObj->Vitality / gServerInfo.m_FEPlasmaStormDamageConstC) + (lpObj->Energy / gServerInfo.m_FEPlasmaStormDamageConstD);
	}
	else if (lpObj->Class == CLASS_MG)
	{
		BaseDamage = (lpObj->Strength / gServerInfo.m_MGPlasmaStormDamageConstA) + (lpObj->Dexterity / gServerInfo.m_MGPlasmaStormDamageConstB) + (lpObj->Vitality / gServerInfo.m_MGPlasmaStormDamageConstC) + (lpObj->Energy / gServerInfo.m_MGPlasmaStormDamageConstD);
	}
	else if (lpObj->Class == CLASS_DL)
	{
		BaseDamage = (lpObj->Strength / gServerInfo.m_DLPlasmaStormDamageConstA) + (lpObj->Dexterity / gServerInfo.m_DLPlasmaStormDamageConstB) + (lpObj->Vitality / gServerInfo.m_DLPlasmaStormDamageConstC) + (lpObj->Energy / gServerInfo.m_DLPlasmaStormDamageConstD) + (lpObj->Leadership / gServerInfo.m_DLPlasmaStormDamageConstE);
	}
	else if (lpObj->Class == CLASS_SU)
	{
		BaseDamage = (lpObj->Strength / gServerInfo.m_SUPlasmaStormDamageConstA) + (lpObj->Dexterity / gServerInfo.m_SUPlasmaStormDamageConstB) + (lpObj->Vitality / gServerInfo.m_SUPlasmaStormDamageConstC) + (lpObj->Energy / gServerInfo.m_SUPlasmaStormDamageConstD);
	}
	else if (lpObj->Class == CLASS_RF)
	{
		BaseDamage = (lpObj->Strength / gServerInfo.m_RFPlasmaStormDamageConstA) + (lpObj->Dexterity / gServerInfo.m_RFPlasmaStormDamageConstB) + (lpObj->Vitality / gServerInfo.m_RFPlasmaStormDamageConstC) + (lpObj->Energy / gServerInfo.m_RFPlasmaStormDamageConstD);
	}
	else if (lpObj->Class == CLASS_GL)
	{
		BaseDamage = (lpObj->Strength / gServerInfo.m_FKPlasmaStormDamageConstA) + (lpObj->Dexterity / gServerInfo.m_FKPlasmaStormDamageConstB) + (lpObj->Vitality / gServerInfo.m_FKPlasmaStormDamageConstC) + (lpObj->Energy / gServerInfo.m_FKPlasmaStormDamageConstD);
	}

	int range = (lpSkill->m_DamageMax - lpSkill->m_DamageMin);

	range = ((range < 1) ? 1 : range);

	int damage = (BaseDamage + lpSkill->m_DamageMin) + (GetLargeRand() % range);

	int CriticalDamageRate = (((lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate) > gServerInfo.m_CriticalDamageRate) ? gServerInfo.m_CriticalDamageRate : (lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate));

	if ((GetLargeRand() % 100) < (CriticalDamageRate - lpTarget->ResistCriticalDamageRate))
	{
		(*effect) = 3;

		damage = (BaseDamage + lpSkill->m_DamageMax);
		damage += (damage * lpObj->CriticalDamagePerc / 100.0f);
		damage += lpObj->CriticalDamage;
		damage += lpObj->EffectOption.AddCriticalDamage;
	}

	int ExcellentDamageRate = (((lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate) > gServerInfo.m_ExcellentDamageRate) ? gServerInfo.m_ExcellentDamageRate : (lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate));

	if ((GetLargeRand() % 100) < (ExcellentDamageRate - lpTarget->ResistExcellentDamageRate))
	{
		(*effect) = 2;

		damage = ((BaseDamage + lpSkill->m_DamageMax) * 110) / 100;
		damage += (damage * lpObj->ExcellentDamagePerc / 100.0f);
		damage += lpObj->ExcellentDamage;
		//damage += lpObj->EffectOption.AddExcellentDamage;
	}

	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage += lpObj->DamagePvP;
	}

	damage -= TargetDefense;

	damage = ((damage < 0) ? 0 : damage);

	return damage;
}

int CAttack::GetAttackDamageElemental(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, WORD* effect, int AttackDamage, int TargetDefense) // OK
{
	lpObj->ElementalDamageMin += (lpObj->Inventory[236].m_Index != GET_ITEM(12, 261)) ? 0 : (lpObj->Inventory[236].m_SocketOption[0] >= 0xFE) ? 0 : lpObj->PentagramOption.AddRadianceRank1;
	lpObj->ElementalDamageMax += (lpObj->Inventory[236].m_Index != GET_ITEM(12, 261)) ? 0 : (lpObj->Inventory[236].m_SocketOption[0] >= 0xFE) ? 0 : lpObj->PentagramOption.AddRadianceRank1;

	int DamageMin = lpObj->ElementalDamageMin;
	int DamageMax = lpObj->ElementalDamageMax;

	/*if (lpTarget->Type == OBJECT_USER)
	{
		if (lpTarget->Life <= ((lpTarget->MaxLife + lpTarget->AddLife) * 10 / 100))
		{
			if ((GetLargeRand() % 100) < 50 && !gEffectManager.CheckEffect(lpTarget, EFFECT_BASTION))
			{
				if (lpTarget->PentagramJewelOption.Bastion > 0)
				{
					gEffectManager.AddEffect(lpTarget, 0, EFFECT_BASTION, lpTarget->PentagramJewelOption.Bastion, 0, 0, 0, 0);
				}
			}
		}
	}*/

	if (lpObj->Type == OBJECT_USER)
	{
		DamageMin += (((lpObj->Inventory[236].IsItem() == 0) ? 0 : lpObj->Inventory[236].m_DamageMin) * lpObj->PentagramOption.MulPentagramDamage) / 100;
		DamageMax += (((lpObj->Inventory[236].IsItem() == 0) ? 0 : lpObj->Inventory[236].m_DamageMax) * lpObj->PentagramOption.MulPentagramDamage) / 100;

		DamageMin += (AttackDamage * lpObj->PentagramOption.AddElementalAttackTransferRate) / 100;
		DamageMax += (AttackDamage * lpObj->PentagramOption.AddElementalAttackTransferRate) / 100;

		DamageMin += lpObj->PentagramJewelOption.AddElementalDamage;
		DamageMax += lpObj->PentagramJewelOption.AddElementalDamage;

		DamageMin += (DamageMin / 100) * lpObj->PentagramJewelOption.AddElementalDamageRatio;
		DamageMax += (DamageMax / 100) * lpObj->PentagramJewelOption.AddElementalDamageRatio;

		/*if (gEffectManager.CheckEffect(lpTarget, EFFECT_BASTION))
		{
			DamageMin -= (DamageMin / 100) * 90;
			DamageMax -= (DamageMax / 100) * 90;
		}*/

		if (lpTarget->Type == OBJECT_USER)
		{
			DamageMin += lpObj->PentagramJewelOption.AddElementalDamagePvP;
			DamageMax += lpObj->PentagramJewelOption.AddElementalDamagePvP;

			DamageMin += (DamageMin / 100) * lpObj->PentagramJewelOption.AddElementalDamageRatioPvP;
			DamageMax += (DamageMax / 100) * lpObj->PentagramJewelOption.AddElementalDamageRatioPvP;

			//DamageMin += (DamageMin * lpObj->PentagramJewelOption.MulElementalDamagePvP) / 100;
			//DamageMax += (DamageMax * lpObj->PentagramJewelOption.MulElementalDamagePvP) / 100;
		}
		else
		{
			DamageMin += lpObj->PentagramJewelOption.AddElementalDamagePvM;
			DamageMax += lpObj->PentagramJewelOption.AddElementalDamagePvM;

			DamageMin += (DamageMin / 100) * lpObj->PentagramJewelOption.AddElementalDamageRatioPvM;
			DamageMax += (DamageMax / 100) * lpObj->PentagramJewelOption.AddElementalDamageRatioPvM;

			DamageMin += (DamageMin * lpObj->PentagramJewelOption.MulElementalDamagePvM) / 100;
			DamageMax += (DamageMax * lpObj->PentagramJewelOption.MulElementalDamagePvM) / 100;
		}
	}

	int range = (DamageMax - DamageMin);

	range = ((range < 1) ? 1 : range);

	int damage = DamageMin + (GetLargeRand() % range);

	if (IsPvPTestOn || (GetLargeRand() % 100) < (lpObj->PentagramOption.AddElementalCriticalDamageRate + lpObj->EffectOption.IncreaseElementalCriticalRate))
	{
		(*effect) = 7;
		damage = DamageMax;
	}

	/*if ((GetLargeRand() % 100) < ((lpTarget->Type == OBJECT_USER) ? lpObj->PentagramJewelOption.AddElementalExcellentDamageRatePvP : lpObj->PentagramJewelOption.AddElementalExcellentDamageRatePvM))
	{
		(*effect) = 6;
		damage = (DamageMax * 120) / 100;
	}*/

	gPentagramSystem.GetPentagramRelationshipDamage(lpObj, lpTarget, &damage);

	damage -= TargetDefense;

	damage = ((damage < 0) ? 0 : damage);

	return damage;
}

int CAttack::GetShieldDamage(LPOBJ lpObj, LPOBJ lpTarget, int damage) // OK
{
	if (lpTarget->MUFCFighter > 0 || lpTarget->Map == MAP_CASTLE_SIEGE)
	{
		return 0;
	}

	if (lpTarget->ShieldGaugeRate <= 0)
	{
		return 0;
	}

	int rate = lpTarget->ShieldGaugeRate;

	if ((GetLargeRand() % 100) < ((lpObj->IgnoreShieldGaugeRate) - lpTarget->ResistIgnoreShieldGaugeRate))
	{
		rate = 0;
	}
	else
	{
		rate -= lpObj->DecreaseShieldGaugeRate;
	}

	rate = ((rate < 0) ? 0 : ((rate > 100) ? 100 : rate));

	int SDDamage = (damage * rate) / 100;
	int HPDamage = damage - SDDamage;

	if (lpTarget->Shield < SDDamage)
	{
		HPDamage = HPDamage + (SDDamage - lpTarget->Shield);
		SDDamage = lpTarget->Shield;

		if (lpTarget->Shield > 0 && HPDamage > (((lpTarget->MaxLife + lpTarget->AddLife) * 20) / 100))
		{
			if (CC_MAP_RANGE(lpTarget->Map) == 0 || IT_MAP_RANGE(lpTarget->Map) == 0)
			{
				GCEffectInfoSend(lpTarget->Index, 17);
			}
		}
	}

	return SDDamage;
}

void CAttack::GetPreviewDefense(LPOBJ lpObj, DWORD* defense) // OK
{
	(*defense) = lpObj->Defense;

	(*defense) += lpObj->EffectOption.AddDefense;

	(*defense) -= lpObj->EffectOption.SubDefense;

	gSkillManager.SkillSwordPowerGetDefense(lpObj->Index, (int*)defense);

	(*defense) += ((*defense) * lpObj->EffectOption.MulDefense) / 100;

	(*defense) -= ((*defense) * lpObj->EffectOption.DivDefense) / 100;
}

void CAttack::GetPreviewPhysiDamage(LPOBJ lpObj, DWORD* DamageMin, DWORD* DamageMax, DWORD* MulDamage, DWORD* DivDamage) // OK
{
	CItem* Right = &lpObj->Inventory[0];
	CItem* Left = &lpObj->Inventory[1];

	bool DualHandWeapon = 0;

	if (lpObj->Class == CLASS_GL || lpObj->Class == CLASS_DK || lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		if (Right->m_Index >= GET_ITEM(0, 0) && Right->m_Index < GET_ITEM(4, 0) && Left->m_Index >= GET_ITEM(0, 0) && Left->m_Index < GET_ITEM(4, 0))
		{
			if (Right->m_IsValidItem != 0 && Left->m_IsValidItem != 0)
			{
				DualHandWeapon = 1;
			}
		}
	}

	if (DualHandWeapon != 0)
	{
		(*DamageMin) = lpObj->PhysiDamageMinRight + lpObj->PhysiDamageMinLeft;
		(*DamageMax) = lpObj->PhysiDamageMaxRight + lpObj->PhysiDamageMaxLeft;
	}
	else if ((Right->m_Index >= GET_ITEM(0, 0) && Right->m_Index < GET_ITEM(4, 0)) || (Right->m_Index >= GET_ITEM(5, 0) && Right->m_Index < GET_ITEM(6, 0)))
	{
		(*DamageMin) = lpObj->PhysiDamageMinRight;
		(*DamageMax) = lpObj->PhysiDamageMaxRight;
	}
	else if (Left->m_Index >= GET_ITEM(4, 0) && Left->m_Index < GET_ITEM(5, 0) && Left->m_Index != GET_ITEM(4, 7) || Left->m_Index != GET_ITEM(4, 15) && Left->m_Slot == 1)
	{
		(*DamageMin) = lpObj->PhysiDamageMinLeft;
		(*DamageMax) = lpObj->PhysiDamageMaxLeft;
	}
	else
	{
		(*DamageMin) = lpObj->PhysiDamageMinLeft;
		(*DamageMax) = lpObj->PhysiDamageMaxLeft;
	}

	(*DamageMin) += lpObj->EffectOption.AddPhysiDamage;
	(*DamageMax) += lpObj->EffectOption.AddPhysiDamage;

	(*DamageMin) += lpObj->EffectOption.AddMinPhysiDamage;
	(*DamageMax) += lpObj->EffectOption.AddMaxPhysiDamage;

	gSkillManager.SkillSwordPowerGetPhysiDamage(lpObj->Index, (int*)DamageMin, (int*)DamageMax);

	(*MulDamage) = lpObj->EffectOption.MulPhysiDamage;
	(*DivDamage) = lpObj->EffectOption.DivPhysiDamage;
}

void CAttack::GetPreviewMagicDamage(LPOBJ lpObj, DWORD* DamageMin, DWORD* DamageMax, DWORD* MulDamage, DWORD* DivDamage, DWORD* DamageRate) // OK
{
	CItem* Right = &lpObj->Inventory[0];
	CItem* Left = &lpObj->Inventory[1];

	(*DamageMin) = lpObj->MagicDamageMin;
	(*DamageMax) = lpObj->MagicDamageMax;

	(*DamageMin) += lpObj->EffectOption.AddMagicDamage;
	(*DamageMax) += lpObj->EffectOption.AddMagicDamage;

	(*DamageMin) += lpObj->EffectOption.AddMinMagicDamage;
	(*DamageMax) += lpObj->EffectOption.AddMaxMagicDamage;

	gSkillManager.SkillSwordPowerGetMagicDamage(lpObj->Index, (int*)DamageMin, (int*)DamageMax);

	(*MulDamage) = lpObj->EffectOption.MulMagicDamage;
	(*DivDamage) = lpObj->EffectOption.DivMagicDamage;

	if (Right->IsItem() != 0 && Right->m_IsValidItem != 0 && ((Right->m_Index >= GET_ITEM(0, 0) && Right->m_Index < GET_ITEM(1, 0)) || (Right->m_Index >= GET_ITEM(5, 0) && Right->m_Index < GET_ITEM(6, 0))))
	{
		(*DamageRate) = (int)(((Right->m_MagicDamageRate / 2) + (Right->m_Level * 2)) * Right->m_CurrentDurabilityState);
	}
	else
	{
		(*DamageRate) = 0;
	}
}

void CAttack::GetPreviewCurseDamage(LPOBJ lpObj, DWORD* DamageMin, DWORD* DamageMax, DWORD* MulDamage, DWORD* DivDamage, DWORD* DamageRate) // OK
{
	CItem* Right = &lpObj->Inventory[0];
	CItem* Left = &lpObj->Inventory[1];

	(*DamageMin) = lpObj->CurseDamageMin;
	(*DamageMax) = lpObj->CurseDamageMax;

	(*DamageMin) += lpObj->EffectOption.AddCurseDamage;
	(*DamageMax) += lpObj->EffectOption.AddCurseDamage;

	(*DamageMin) += lpObj->EffectOption.AddMinCurseDamage;
	(*DamageMax) += lpObj->EffectOption.AddMaxCurseDamage;

	gSkillManager.SkillSwordPowerGetCurseDamage(lpObj->Index, (int*)DamageMin, (int*)DamageMax);

	(*MulDamage) = lpObj->EffectOption.MulCurseDamage;
	(*DivDamage) = lpObj->EffectOption.DivCurseDamage;

	if (Left->IsItem() != 0 && Left->m_IsValidItem != 0 && Left->m_Index >= GET_ITEM(5, 21) && Left->m_Index <= GET_ITEM(5, 23))
	{
		(*DamageRate) = (int)(((Left->m_MagicDamageRate / 2) + (Left->m_Level * 2)) * Left->m_CurrentDurabilityState);
	}
	else
	{
		(*DamageRate) = 0;
	}
}

void CAttack::GetPreviewDamageMultiplier(LPOBJ lpObj, DWORD* DamageMultiplier, DWORD* RFDamageMultiplierA, DWORD* RFDamageMultiplierB, DWORD* RFDamageMultiplierC) // OK
{
	switch (lpObj->Class)
	{
	case CLASS_GL:
		(*DamageMultiplier) = 200;
		(*RFDamageMultiplierA) = lpObj->GLDamageMultiplierRate[0];
		(*RFDamageMultiplierB) = lpObj->GLDamageMultiplierRate[1];
		(*RFDamageMultiplierC) = 100;
		break;
	case CLASS_DW:
		(*DamageMultiplier) = 200;
		(*RFDamageMultiplierA) = 100;
		(*RFDamageMultiplierB) = 100;
		(*RFDamageMultiplierC) = 100;
		break;
	case CLASS_DK:
		(*DamageMultiplier) = lpObj->DKDamageMultiplierRate;
		(*RFDamageMultiplierA) = 100;
		(*RFDamageMultiplierB) = 100;
		(*RFDamageMultiplierC) = 100;
		break;
	case CLASS_FE:
		(*DamageMultiplier) = 200;
		(*RFDamageMultiplierA) = 100;
		(*RFDamageMultiplierB) = 100;
		(*RFDamageMultiplierC) = 100;
		break;
	case CLASS_MG:
		(*DamageMultiplier) = 200;
		(*RFDamageMultiplierA) = 100;
		(*RFDamageMultiplierB) = 100;
		(*RFDamageMultiplierC) = 100;
		break;
	case CLASS_DL:
		(*DamageMultiplier) = lpObj->DLDamageMultiplierRate;
		(*RFDamageMultiplierA) = 100;
		(*RFDamageMultiplierB) = 100;
		(*RFDamageMultiplierC) = 100;
		break;
	case CLASS_SU:
		(*DamageMultiplier) = 200;
		(*RFDamageMultiplierA) = 100;
		(*RFDamageMultiplierB) = 100;
		(*RFDamageMultiplierC) = 100;
		break;
	case CLASS_RF:
		(*DamageMultiplier) = 200;
		(*RFDamageMultiplierA) = lpObj->RFDamageMultiplierRate[0];
		(*RFDamageMultiplierB) = lpObj->RFDamageMultiplierRate[1];
		(*RFDamageMultiplierC) = lpObj->RFDamageMultiplierRate[2];
		break;
	}
}

void CAttack::CGAttackRecv(PMSG_ATTACK_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Live == 0)
	{
		return;
	}

	if (lpObj->Map != lpTarget->Map)
	{
		return;
	}

	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0 || gDuel.GetDuelArenaBySpectator(bIndex) != 0)
	{
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0 || gMap[lpTarget->Map].CheckAttr(lpTarget->X, lpTarget->Y, 1) != 0)
	{
		return;
	}

	if (lpObj->Type == OBJECT_USER && sqrt(pow(((float)lpObj->X - (float)lpTarget->X), 2) + pow(((float)lpObj->Y - (float)lpTarget->Y), 2)) > ((lpObj->Class == CLASS_FE) ? 6 : 3))
	{
		return;
	}

	lpObj->Dir = lpMsg->dir;

	lpObj->MultiSkillIndex = 0;

	lpObj->MultiSkillCount = 0;

	GCActionSend(lpObj, lpMsg->action, aIndex, bIndex);

	lpObj->ComboSkill.Init();

	this->Attack(lpObj, lpTarget, 0, 0, 0, 0, 0, 0);
}

void CAttack::CGAttack(int aIndex, int bIndex, int action, int dir)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnected(aIndex) == 0)
		return;

	if (OBJECT_RANGE(bIndex) == 0)
		return;

	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Live == 0)
		return;

	if (lpObj->Map != lpTarget->Map)
		return;

	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0 || gDuel.GetDuelArenaBySpectator(bIndex) != 0)
		return;

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0 || gMap[lpTarget->Map].CheckAttr(lpTarget->X, lpTarget->Y, 1) != 0)
		return;

	if (lpObj->Type == OBJECT_USER && sqrt(pow(((float)lpObj->X - (float)lpTarget->X), 2) + pow(((float)lpObj->Y - (float)lpTarget->Y), 2)) > ((lpObj->Class == CLASS_FE) ? 6 : 3))
		return;

	lpObj->Dir = dir;

	lpObj->MultiSkillIndex = 0;

	lpObj->MultiSkillCount = 0;

	GCActionSend(lpObj, action, aIndex, bIndex);

	lpObj->ComboSkill.Init();

	this->Attack(lpObj, lpTarget, 0, 0, 0, 0, 0, 0);
}

void CAttack::AttackBoss(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill)
{
	int i;
	int MonsterDamageInPlayer = 0;
	int PlayerDamageInMonster = 0;
	WORD effect = 0;

	//atacker is a custom monster
	if (lpObj->Type == OBJECT_MONSTER)
	{
		if (lpTarget->Authority == 32)
		{
			return;
		}

		if (lpObj->bossMinDmgMakePerc > -1 || lpObj->bossMaxDmgMakePerc > -1)
		{
			long long MonsterDamageInPlayerFactor = lpObj->bossMinDmgMakePerc + (GetLargeRand() % (1 + lpObj->bossMaxDmgMakePerc - lpObj->bossMinDmgMakePerc));
			MonsterDamageInPlayer = (lpTarget->MaxLife + lpTarget->AddLife) * MonsterDamageInPlayerFactor / 100;

			if (MonsterDamageInPlayer < lpObj->bossMinDmgMakePoints)
			{
				MonsterDamageInPlayer = lpObj->bossMinDmgMakePoints;
			}

			MonsterDamageInPlayer -= (int)(MonsterDamageInPlayer * (lpTarget->DecBossDamage + lpTarget->EffectOption.DecreaseBossDamage) / 100.0f);

			if (lpTarget->Life < MonsterDamageInPlayer)
			{
				gNpcTalk.SendNPCMessageToViewPort(lpObj, "Eita, sentiu essa hein %s!? kkkkkk", lpTarget->Name);
				lpTarget->Life = 0;
			}
			else
			{
				lpTarget->Life -= MonsterDamageInPlayer;
			}

			gCustomMonster.SendBossChat(lpObj, lpTarget);

			GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
			gObjectManager.CharacterLifeCheck(lpObj, lpTarget, MonsterDamageInPlayer, 0, 0, 0, ((lpSkill == 0) ? 0 : lpSkill->m_index), 0);

			return;
		}
	}
	//player attacking boss
	else if (lpTarget->Type == OBJECT_MONSTER)
	{
		if ((lpObj->Index - OBJECT_START_USER) >= 500 || (lpObj->Index - OBJECT_START_USER) < 0)
		{
			return;
		}

		int delay = lpTarget->bossHitDelay;

		int delayReduce = ((lpObj->DecBossHitDelay + lpObj->EffectOption.DecreaseBossHitDelay) < 70) ? (lpObj->DecBossHitDelay + lpObj->EffectOption.DecreaseBossHitDelay) : 70;

		if (delayReduce > 0)
		{
			delay -= (lpTarget->bossHitDelay * delayReduce / 100);
		}

		delay = (delay > 0) ? delay : 0;

		if ((GetTickCount64() - lpTarget->LastBossHit[lpObj->Index - OBJECT_START_USER]) < delay)
		{
			WORD effect = 0;
			if (rand() % 4 != 0)
				effect |= (8 + rand() % 3);

			GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, effect, 0);
			return;
		}

		lpTarget->LastBossHit[lpObj->Index - OBJECT_START_USER] = GetTickCount64();

		if (lpTarget->bossMinDmgGetPoints > -1 || lpTarget->bossMaxDmgGetPoints > -1)
		{
			int ExcellentDamageRate = (((lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate) > gServerInfo.m_ExcellentDamageRate) ? gServerInfo.m_ExcellentDamageRate : (lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate));

			if (lpObj->ExcellentDamageOnly || (GetLargeRand() % 100) < (ExcellentDamageRate))
			{
				effect = 2;
				PlayerDamageInMonster = (lpTarget->bossMaxDmgGetPoints * 110) / 100;
				PlayerDamageInMonster += (PlayerDamageInMonster * lpObj->ExcellentDamagePerc / 100.0f);
			}
			else
			{
				PlayerDamageInMonster = lpTarget->bossMinDmgGetPoints + (GetLargeRand() % (lpTarget->bossMaxDmgGetPoints - lpTarget->bossMinDmgGetPoints));
			}

			PlayerDamageInMonster += lpObj->Strength + lpObj->Energy + lpObj->Dexterity + ((lpObj->AddStrength + lpObj->AddEnergy + lpObj->AddDexterity) / 2);

			PlayerDamageInMonster -= ((lpObj->Vitality + lpObj->AddVitality) * 2);

			if (lpSkill != NULL && lpSkill->m_index > SKILL_NONE) { PlayerDamageInMonster *= 1.10; } //10% more for using skill

			PlayerDamageInMonster = (PlayerDamageInMonster < 0) ? 0 : PlayerDamageInMonster;

			PlayerDamageInMonster += (int)(PlayerDamageInMonster * (lpObj->AddBossDamage + lpObj->EffectOption.IncreaseBossDamage + (lpObj->AddGeneralDamageRate / 1.5)) / 100);

			/*int distance = gObjCalcDistance(lpObj, lpTarget);

			if (distance >= 0 && distance <= 12)
			{
				PlayerDamageInMonster = (PlayerDamageInMonster * gServerInfo.m_DamageRateByDistance[distance] / 100);
			}
			else
			{
				PlayerDamageInMonster = 0;
			}*/

			if ((GetLargeRand() % 100) < (lpObj->BossDoubleDamageRate + lpObj->EffectOption.BossDoubleDamage))
			{
				effect |= 0x40;
				PlayerDamageInMonster += PlayerDamageInMonster;
			}

			if (gEffectManager.CheckEffect(lpTarget, EFFECT_BLINDNES))
			{
				PlayerDamageInMonster = (PlayerDamageInMonster * 105) / 100;
			}

			if (lpTarget->Life < PlayerDamageInMonster)
			{
				lpTarget->Life = 0;
			}
			else
			{
				lpTarget->Life -= PlayerDamageInMonster;
			}

			if (lpObj->BleedingRate > 0 && (GetLargeRand() % 100) < lpObj->BleedingRate)
			{
				if (!gEffectManager.CheckEffect(lpTarget, EFFECT_HEMORRHAGE))
				{
					gEffectManager.AddEffect(lpTarget, 0, EFFECT_HEMORRHAGE, 10, lpObj->Index, 1, SET_NUMBERHW(PlayerDamageInMonster * 2), SET_NUMBERLW(PlayerDamageInMonster * 2));
					WORD efeito = 6;
					GCDamageSend(lpObj->Index, lpTarget->Index, 0, (PlayerDamageInMonster * 2), efeito, 0);
				}
			}

			if (lpObj->FreezingDamageRate > 0 && (GetLargeRand() % 100) < lpObj->FreezingDamageRate)
			{
				if (!gEffectManager.CheckEffect(lpTarget, EFFECT_FREEZING_DAMAGE))
				{
					gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING_DAMAGE, 10, lpObj->Index, 1, SET_NUMBERHW(PlayerDamageInMonster), SET_NUMBERLW(PlayerDamageInMonster));
				}
			}

			if (lpObj->SternRate > 0 && (GetLargeRand() % 100) < lpObj->SternRate)
			{
				if (!gEffectManager.CheckEffect(lpTarget, EFFECT_STERN))
				{
					gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 3, 0, 0, 0, 0);
				}
			}

			if (lpObj->CureHarmRate > 0 && (GetLargeRand() % 100) < lpObj->CureHarmRate)
			{
				if (!gEffectManager.CheckEffect(lpTarget, EFFECT_BLINDNES) && (GetLargeRand() % 100) >= lpTarget->CureHarmResist)
				{
					gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLINDNES, 10, 0, 0, 0, 0);
				}
			}

			GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);

			gObjectManager.CharacterLifeCheck(lpObj, lpTarget, PlayerDamageInMonster, 0, 0, effect, ((lpSkill == 0) ? 0 : lpSkill->m_index), 0);
		}
	}
}

void CAttack::AttackBossElemental(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill) // OK
{
	if (lpObj->ElementalAttribute == 0)
	{
		return;
	}

	if (lpObj->Type == OBJECT_USER && (lpObj->Inventory[236].IsItem() == 0 || lpObj->Inventory[236].IsPentagramItem() == 0 || lpObj->Inventory[236].m_IsValidItem == 0))
	{
		return;
	}

	int flag = 0;

	WORD effect = lpObj->ElementalAttribute;

	int i;
	int MonsterDamageInPlayer = 0;
	int PlayerDamageInMonster = 0;

	//atacker is a custom monster
	if (lpObj->Type == OBJECT_MONSTER)
	{
		if (lpTarget->Authority == 32)
		{
			return;
		}

		if (lpObj->bossMinDmgMakePerc > -1 || lpObj->bossMaxDmgMakePerc > -1)
		{
			long long MonsterDamageInPlayerFactor = lpObj->bossMinDmgMakePerc + (GetLargeRand() % (1 + lpObj->bossMaxDmgMakePerc - lpObj->bossMinDmgMakePerc));
			MonsterDamageInPlayer = (lpTarget->MaxLife + lpTarget->AddLife) * MonsterDamageInPlayerFactor / 100;

			if (MonsterDamageInPlayer < lpObj->bossMinDmgMakePoints)
			{
				MonsterDamageInPlayer = lpObj->bossMinDmgMakePoints;
			}

			MonsterDamageInPlayer -= (int)(MonsterDamageInPlayer * (lpTarget->PentagramJewelOption.AddElementalDamageReductionPvM + lpTarget->DecBossElementalDamage + lpTarget->EffectOption.DecreaseBossElementalDamage + (lpTarget->ReduceGeneralElementalDamageRate / 2)) / 100.0f);

			gPentagramSystem.GetPentagramRelationshipDamageDecrease(lpObj, lpTarget, &MonsterDamageInPlayer);

			if (lpTarget->PentagramJewelOption.AddElementalDefensePvM < 20)
			{
				MonsterDamageInPlayer *= 2;
			}

			//Summoner Debuffs
			if (gEffectManager.CheckEffect(lpTarget, EFFECT_LESSER_DEFENSE) != 0)
			{
				MonsterDamageInPlayer += (int)(MonsterDamageInPlayer * 0.10f);
			}

			if (lpTarget->Life < MonsterDamageInPlayer)
			{
				gNpcTalk.SendNPCMessageToViewPort(lpObj, "Eita, sentiu essa hein %s!? kkkkkk", lpTarget->Name);
				lpTarget->Life = 0;
			}
			else
			{
				lpTarget->Life -= MonsterDamageInPlayer;
			}

			gCustomMonster.SendBossChat(lpObj, lpTarget);

			GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
			gObjectManager.CharacterLifeCheck(lpObj, lpTarget, MonsterDamageInPlayer, 4, 0, effect, ((lpSkill == 0) ? 0 : lpSkill->m_index), 0);

			return;
		}
	}
	else if (lpTarget->Type == OBJECT_MONSTER) //player attacking boss
	{
		if ((lpObj->Index - OBJECT_START_USER) >= 500 || (lpObj->Index - OBJECT_START_USER) < 0)
		{
			return;
		}

		int delay = lpTarget->bossHitDelay;

		int delayReduce = ((lpObj->DecBossHitDelay + lpObj->EffectOption.DecreaseBossHitDelay) < 70) ? (lpObj->DecBossHitDelay + lpObj->EffectOption.DecreaseBossHitDelay) : 70;

		if (delayReduce > 0)
		{
			delay -= (lpTarget->bossHitDelay * delayReduce / 100);
		}

		delay = (delay > 0) ? delay : 0;

		if ((GetTickCount64() - lpTarget->LastBossHit[lpObj->Index - OBJECT_START_USER]) < delay)
		{
			return;
		}

		lpTarget->LastBossHit[lpObj->Index - OBJECT_START_USER] = GetTickCount64();

		if (lpTarget->bossMinDmgGetPoints > -1 || lpTarget->bossMaxDmgGetPoints > -1)
		{
			if (lpObj->PentagramJewelOption.MulElementalAttackSuccessRatePVM > 0 &&
				(GetLargeRand() % 100) < ((lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate + lpObj->PentagramOption.AddElementalCriticalDamageRate + lpObj->EffectOption.IncreaseElementalCriticalRate + lpObj->CriticalDamage) / 2))
			{
				effect = 7;
				PlayerDamageInMonster = (lpTarget->bossMaxDmgGetPoints * 110) / 100;
			}
			else
			{
				PlayerDamageInMonster = lpTarget->bossMinDmgGetPoints + (GetLargeRand() % (lpTarget->bossMaxDmgGetPoints - lpTarget->bossMinDmgGetPoints));
			}

			if (lpSkill != NULL && lpSkill->m_index > SKILL_NONE) { PlayerDamageInMonster *= 1.10; } //10% more for using skill

			PlayerDamageInMonster = (PlayerDamageInMonster < 0) ? 0 : PlayerDamageInMonster;

			PlayerDamageInMonster += (int)(PlayerDamageInMonster * (lpObj->PentagramJewelOption.AddElementalDamageRatioPvM + lpObj->PentagramJewelOption.MulElementalDamagePvM + lpObj->AddBossElementalDamage + lpObj->EffectOption.IncreaseBossElementalDamage + ((lpObj->AddBossDamage + lpObj->AddElementalDamageRate) / 2)) / 100);

			gPentagramSystem.GetPentagramRelationshipDamageIncrease(lpObj, lpTarget, &PlayerDamageInMonster);

			if (lpObj->PentagramJewelOption.AddElementalDamagePvM < 1 || lpObj->PentagramJewelOption.AddElementalDamageRatioPvM < 1)
			{
				PlayerDamageInMonster /= 2;
			}
			else
			{
				PlayerDamageInMonster += lpObj->PentagramJewelOption.AddElementalDamagePvM * 10;
			}

			PlayerDamageInMonster -= ((lpObj->Vitality + lpObj->AddVitality) * 3);

			/*int distance = gObjCalcDistance(lpObj, lpTarget);

			if (distance >= 0 && distance <= 12)
			{
				PlayerDamageInMonster = (PlayerDamageInMonster * gServerInfo.m_DamageRateByDistance[distance] / 100);
			}
			else
			{
				PlayerDamageInMonster = 0;
			}*/

			if ((GetLargeRand() % 10000) < ((lpObj->ElementalTripleDamageRate + lpObj->EffectOption.IncreaseElementalTripleDamageRate) * 100.0f))
			{
				effect = 6;
				PlayerDamageInMonster += PlayerDamageInMonster + PlayerDamageInMonster;
			}
			else if ((GetLargeRand() % 100) < (lpObj->BossDoubleDamageRate + lpObj->EffectOption.BossDoubleDamage))
			{
				effect = 7;
				PlayerDamageInMonster += PlayerDamageInMonster;
			}

			PlayerDamageInMonster /= 2;

			//Summoner Debuffs
			if (gEffectManager.CheckEffect(lpObj, EFFECT_LESSER_DAMAGE) != 0)
			{
				PlayerDamageInMonster -= (int)(PlayerDamageInMonster * 0.10f);
			}

			if (PlayerDamageInMonster < 0)
				PlayerDamageInMonster = 0;

			if (lpTarget->Life < PlayerDamageInMonster)
			{
				lpTarget->Life = 0;
			}
			else
			{
				lpTarget->Life -= PlayerDamageInMonster;
			}

			GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);

			gObjectManager.CharacterLifeCheck(lpObj, lpTarget, PlayerDamageInMonster, 4, 0, effect, ((lpSkill == 0) ? 0 : lpSkill->m_index), 0);
		}
	}
}