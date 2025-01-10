#include "stdafx.h"
#include "SkillManager.h"
#include "..\\..\\Util\\Math.h"
#include "Attack.h"
#include "CastleSiege.h"
#include "CustomArena.h"
#include "DoubleGoer.h"
#include "Duel.h"
#include "EffectManager.h"
#include "Fruit.h"
#include "GensSystem.h"
#include "IllusionTemple.h"
#include "ItemManager.h"
#include "Map.h"
#include "MasterSkillTree.h"
#include "MemScript.h"
#include "Message.h"
#include "Monster.h"
#include "Move.h"
#include "MoveSummon.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Party.h"
#include "Quest.h"
#include "ServerInfo.h"
#include "SkillHitBox.h"
#include "Util.h"
#include "Viewport.h"
#include "ComboSkillPath.h"
#include "MasterSkillTree_4th.h"
#include "HuntingGrounds.h"
#include "CheatGuard.h"
#include "GuildClass.h"
#include "Union.h"
#include "Guild.h"

CSkillManager gSkillManager;

CSkillManager::CSkillManager() // OK
{
	this->m_SkillInfo.clear();
}

CSkillManager::~CSkillManager() // OK
{

}

void CSkillManager::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_SkillInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			SKILL_INFO info;

			info.Index = lpMemScript->GetNumber();
			strcpy_s(info.Name, lpMemScript->GetAsString());
			info.Damage = lpMemScript->GetAsNumber();
			info.Mana = lpMemScript->GetAsNumber();
			info.BP = lpMemScript->GetAsNumber();
			info.Range = lpMemScript->GetAsNumber();
			info.Radio = lpMemScript->GetAsNumber();
			info.Delay = lpMemScript->GetAsNumber();
			info.Type = lpMemScript->GetAsNumber();
			info.Effect = lpMemScript->GetAsNumber();
			info.RequireLevel = lpMemScript->GetAsNumber();
			info.RequireEnergy = lpMemScript->GetAsNumber();
			info.RequireLeadership = lpMemScript->GetAsNumber();
			info.RequireKillCount = lpMemScript->GetAsNumber();
			info.RequireGuildStatus = lpMemScript->GetAsNumber();
			info.Element = lpMemScript->GetAsNumber();

			for (int n = 0; n < MAX_CLASS; n++) { info.RequireClass[n] = lpMemScript->GetAsNumber(); }

			this->m_SkillInfo.insert(std::pair<int, SKILL_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CSkillManager::LoadSkillDamageRate(char* path)
{
	for (int n = 0; n < MAX_SKILL; n++)
	{
		this->DamageRatePvM[n] = 100;
		this->DamageRatePvP[n] = 100;
	}

	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	int number;

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			number = lpMemScript->GetNumber();
			this->DamageRatePvP[number] = lpMemScript->GetAsNumber();
			this->DamageRatePvM[number] = lpMemScript->GetAsNumber();
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CSkillManager::LoadSkillBlock(char* path)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	for (int n = 0; n < 100; n++)
	{
		this->SkillBlock[n].Skill = -1;
	}

	int count = 0;

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			this->SkillBlock[count].Skill = lpMemScript->GetNumber();
			this->SkillBlock[count].Map = lpMemScript->GetAsNumber();
			this->SkillBlock[count].X1 = lpMemScript->GetAsNumber();
			this->SkillBlock[count].Y1 = lpMemScript->GetAsNumber();
			this->SkillBlock[count].X2 = lpMemScript->GetAsNumber();
			this->SkillBlock[count].Y2 = lpMemScript->GetAsNumber();

			count++;

			if (count == 100)
				break;
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CSkillManager::GetInfo(int index, SKILL_INFO* lpInfo) // OK
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if (it == this->m_SkillInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

int CSkillManager::GetSkillDamage(int index) // OK
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Damage;
	}
}

int CSkillManager::GetSkillMana(int index) // OK
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Mana;
	}
}

int CSkillManager::GetSkillBP(int index) // OK
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.BP;
	}
}

int CSkillManager::GetSkillType(int index) // OK
{

	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Type;
	}
}

int CSkillManager::GetSkillEffect(int index) // OK
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Effect;
	}
}
int CSkillManager::GetSkillElement(int index) // OK //2017/04/23
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Element;
	}
}
int CSkillManager::GetSkillAngle(int x, int y, int tx, int ty) // OK
{
	float dx = (float)(x - tx);
	float dy = (float)(y - ty);

	float rad = atan2(dy, dx);

	int angle = (int)(((rad * 180) / 3.141592741012573) + 90);

	return ((angle < 0) ? (angle + 360) : angle);
}

int CSkillManager::GetSkillFrustrum(int* SkillFrustrumX, int* SkillFrustrumY, BYTE angle, int x, int y, float sx, float sy, float tx, float ty) // OK
{
	vec3_t p[4];

	Vector(-sx, sy, 0.0f, p[0]);
	Vector(sx, sy, 0.0f, p[1]);
	Vector(tx, ty, 0.f, p[2]);
	Vector(-tx, ty, 0.f, p[3]);

	vec3_t Angle;

	Vector(0.0f, 0.0f, (vec_t)((angle * 360) / 255), Angle);

	float Matrix[3][4];

	AngleMatrix(Angle, Matrix);

	vec3_t vFrustrum[4];

	for (int n = 0; n < 4; n++)
	{
		VectorRotate(p[n], Matrix, vFrustrum[n]);
		SkillFrustrumX[n] = (int)vFrustrum[n][0] + x;
		SkillFrustrumY[n] = (int)vFrustrum[n][1] + y;
	}

	return 1;
}

int CSkillManager::GetSkillNumber(int index, int level) // OK
{
	if (index >= GET_ITEM(15, 0) && index <= GET_ITEM(15, 15))
	{
		return ((index % MAX_ITEM_TYPE) + 1);
	}

	switch (index)
	{
	case GET_ITEM(12, 7):
		return SKILL_TWISTING_SLASH;
	case GET_ITEM(12, 8):
		return SKILL_HEAL;
	case GET_ITEM(12, 9):
		return SKILL_GREATER_DEFENSE;
	case GET_ITEM(12, 10):
		return SKILL_GREATER_DAMAGE;
	case GET_ITEM(12, 11):
		return (SKILL_SUMMON1 + level);
	case GET_ITEM(12, 12):
		return SKILL_RAGEFUL_BLOW;
	case GET_ITEM(12, 13):
		return SKILL_IMPALE;
	case GET_ITEM(12, 14):
		return SKILL_GREATER_LIFE;
	case GET_ITEM(12, 16):
		return SKILL_FIRE_SLASH;
	case GET_ITEM(12, 17):
		return SKILL_PENETRATION;
	case GET_ITEM(12, 18):
		return SKILL_ICE_ARROW;
	case GET_ITEM(12, 19):
		return SKILL_DEATH_STAB;
	case GET_ITEM(12, 21):
		return SKILL_FIRE_BURST;
	case GET_ITEM(12, 22):
		return SKILL_SUMMON_PARTY;
	case GET_ITEM(12, 23):
		return SKILL_GREATER_CRITICAL_DAMAGE;
	case GET_ITEM(12, 24):
		return SKILL_ELECTRIC_SPARK;
	case GET_ITEM(12, 35):
		return SKILL_FIRE_SCREAM;
	case GET_ITEM(12, 44):
		return SKILL_FROZEN_STAB;
	case GET_ITEM(12, 45):
		return SKILL_FIVE_SHOT;
	case GET_ITEM(12, 46):
		return SKILL_SHIELD_RECOVER;
	case GET_ITEM(12, 47):
		return SKILL_FLAME_STRIKE;
	case GET_ITEM(12, 48):
		return SKILL_BIRDS;
	case GET_ITEM(15, 16):
		return SKILL_DECAY;
	case GET_ITEM(15, 17):
		return SKILL_ICE_STORM;
	case GET_ITEM(15, 18):
		return SKILL_NOVA;
	case GET_ITEM(15, 19):
		return SKILL_CHAIN_LIGHTNING;
	case GET_ITEM(15, 20):
		return SKILL_DRAIN_LIFE;
	case GET_ITEM(15, 21):
		return SKILL_RED_STORM;
	case GET_ITEM(15, 22):
		return SKILL_DAMAGE_REFLECT;
	case GET_ITEM(15, 23):
		return SKILL_SWORD_POWER;
	case GET_ITEM(15, 24):
		return SKILL_SLEEP;
	case GET_ITEM(15, 26):
		return SKILL_LESSER_DAMAGE;
	case GET_ITEM(15, 27):
		return SKILL_LESSER_DEFENSE;
	case GET_ITEM(15, 28):
		return SKILL_MAGIC_CIRCLE;
	case GET_ITEM(15, 29):
		return SKILL_LIGHTNING_STORM;
	case GET_ITEM(15, 30):
		return SKILL_CHAIN_DRIVER;
	case GET_ITEM(15, 31):
		return SKILL_DARK_SIDE;
	case GET_ITEM(15, 32):
		return SKILL_DRAGON_LORE;
	case GET_ITEM(15, 33):
		return SKILL_DRAGON_SLAYER;
	case GET_ITEM(15, 34):
		return SKILL_GREATER_IGNORE_DEFENSE_RATE;
	case GET_ITEM(15, 35):
		return SKILL_FITNESS;
	case GET_ITEM(15, 36):
		return SKILL_GREATER_DEFENSE_SUCCESS_RATE;
	case GET_ITEM(12, 271):
		return SKILL_SPIN_STEP;
	case GET_ITEM(12, 272):
		return SKILL_OBSIDIAN;
	case GET_ITEM(12, 273):
		return SKILL_MAGIC_PIN;
	case GET_ITEM(12, 274):
		return SKILL_HARSH_STRIKE;
	case GET_ITEM(12, 275):
		return SKILL_SHINING_PEAK;
	case GET_ITEM(12, 276):
		return SKILL_WRATH;
	case GET_ITEM(12, 277):
		return SKILL_BRECHE;
	case GET_ITEM(12, 289):
		return SKILL_FIRE_BLOW;
	case GET_ITEM(12, 290):
		return SKILL_DW_METEOR_STRIKE;
	case GET_ITEM(12, 291):
		return SKILL_METEOR_STORM;
	case GET_ITEM(12, 292):
		return SKILL_SOUL_SEEKER;
	case GET_ITEM(12, 293):
		return SKILL_FOCUS_SHOT;
	case GET_ITEM(12, 295):
		return SKILL_FIRE_BEAST;
	case GET_ITEM(12, 296):
		return SKILL_AQUA_BEAST;
	case GET_ITEM(12, 297):
		return SKILL_ICE_BLOOD;
	case GET_ITEM(12, 298):
		return SKILL_FIRE_BLOOD;
	case GET_ITEM(12, 299):
		return SKILL_DARK_BLAST;
	case GET_ITEM(12, 300):
		return SKILL_MG_METEOR_STRIKE;
	case GET_ITEM(12, 303):
		return SKILL_WIND_SOUL;
	case GET_ITEM(12, 305):
		return SKILL_DARK_PHOENIX_SHOT;
	case GET_ITEM(12, 413):
		return SKILL_SPIRIT_HOOK;
	}

	return -1;
}

bool CSkillManager::CheckSkillMana(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if (lpObj->Mana < ((SkillInfo.Mana * lpObj->MPConsumptionRate) / 100))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool CSkillManager::CheckSkillBP(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if (lpObj->BP < ((SkillInfo.BP * lpObj->BPConsumptionRate) / 100))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool CSkillManager::CheckSkillRange(int index, int x, int y, int tx, int ty, int add) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}



	if ((SkillInfo.Range + add) == 0)
	{
		return 0;
	}

	if (sqrt(pow(((float)x - (float)tx), 2) + pow(((float)y - (float)ty), 2)) <= (SkillInfo.Range + add))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRadio(int index, int x, int y, int tx, int ty, int add) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if ((SkillInfo.Radio + add) == 0)
	{
		return 0;
	}

	if (sqrt(pow(((float)x - (float)tx), 2) + pow(((float)y - (float)ty), 2)) <= (SkillInfo.Radio + add))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillFrustrum(int* SkillFrustrumX, int* SkillFrustrumY, int x, int y) // OK
{
	for (int n = 0, i = 3; n < 4; i = n, n++)
	{
		if ((((SkillFrustrumX[n] - x) * (SkillFrustrumY[i] - y)) - ((SkillFrustrumX[i] - x) * (SkillFrustrumY[n] - y))) < 0)
		{
			return 0;
		}
	}

	return 1;
}

bool CSkillManager::CheckSkillDelay(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_ELIMINATE_REUSE_TIME, 2) && index == SKILL_METEOR_STORM)
	{
		SkillInfo.Delay = 0;
	}

	if (lpObj->RageFighterSkillIndex == index && lpObj->RageFighterSkillCount > 0)
	{
		return 1;
	}

	if ((GetTickCount() - lpObj->SkillDelay[index]) < ((DWORD)SkillInfo.Delay)   )
	{
		return 0;
	}

	lpObj->SkillDelay[index] = GetTickCount();

	return 1;
}

bool CSkillManager::CheckSkillTarget(LPOBJ lpObj, int aIndex, int bIndex, int type) // OK
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	if (type == OBJECT_NPC)
	{
		return 0;
	}

	if (gObj[aIndex].Live == 0 || gObj[aIndex].State != OBJECT_PLAYING || gObj[aIndex].Teleport != 0)
	{
		return 0;
	}

	if (lpObj->Type == OBJECT_MONSTER && type == OBJECT_USER)
	{
		return 1;
	}

	if (aIndex == bIndex)
	{
		return 1;
	}

	int SummonIndex = aIndex;

	if (type == OBJECT_MONSTER)
	{
		if ((gObj[aIndex].Class >= 100 && gObj[aIndex].Class <= 110) || gObj[aIndex].Class == 523) // Trap
		{
			return 0;
		}

		if (OBJECT_RANGE(gObj[aIndex].SummonIndex) == 0)
		{
			return 1;
		}
		else
		{
			SummonIndex = gObj[aIndex].SummonIndex;
		}
	}

	if (gDuel.CheckDuel(lpObj, &gObj[SummonIndex]) != 0)
	{
		return 1;
	}

	if (gGensSystem.CheckGens(lpObj, &gObj[SummonIndex]) != 0)
	{
		return 1;
	}

	if (gObjTargetGuildWarCheck(lpObj, &gObj[SummonIndex]) != 0)
	{
		return 1;
	}

	if (gCustomArena.CheckPlayerTarget(lpObj, &gObj[SummonIndex]) != 0)
	{
		return 1;
	}

	LPOBJ lpTarget = &gObj[SummonIndex];

	//Dungeon 3 Massacre
	/*if (lpObj->Map == MAP_DUNGEON && lpTarget->Map == MAP_DUNGEON)
	{
		return 1;
	}*/

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

#if(GAMESERVER_TYPE==1)

	if (gObj[SummonIndex].Map == MAP_CASTLE_SIEGE && gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (lpObj->Type == OBJECT_USER && gObj[SummonIndex].Type == OBJECT_USER)
		{
			if (lpObj->CsJoinSide == 0 || gObj[SummonIndex].CsJoinSide == 0 || lpObj->CsJoinSide != gObj[SummonIndex].CsJoinSide)
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}
	}

#endif

	return 0;
}

bool CSkillManager::CheckSkillRequireLevel(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if (lpObj->Level >= SkillInfo.RequireLevel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireEnergy(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if ((lpObj->Energy + lpObj->AddEnergy) >= SkillInfo.RequireEnergy)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireLeadership(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if ((lpObj->Leadership + lpObj->AddLeadership) >= SkillInfo.RequireLeadership)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireKillPoint(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if (lpObj->KillCount >= SkillInfo.RequireKillCount)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireGuildStatus(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if (SkillInfo.RequireGuildStatus == 0 || (SkillInfo.RequireGuildStatus & 0x20) == lpObj->GuildStatus || (SkillInfo.RequireGuildStatus & 0x40) == lpObj->GuildStatus || (SkillInfo.RequireGuildStatus & 0x80) == lpObj->GuildStatus)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireClass(LPOBJ lpObj, int index) // OK
{
	SKILL_INFO SkillInfo;

	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}

	if (CHECK_RANGE(lpObj->Class, MAX_CLASS) == 0)
	{
		return 0;
	}

	if (SkillInfo.RequireClass[lpObj->Class] == 0)
	{
		return 0;
	}

	if ((lpObj->ChangeUp + 1) >= SkillInfo.RequireClass[lpObj->Class])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireWeapon(LPOBJ lpObj, int index) // OK
{
	if (index != SKILL_DEFENSE && index != SKILL_FALLING_SLASH && index != SKILL_LUNGE && index != SKILL_UPPERCUT && index != SKILL_CYCLONE && index != SKILL_SLASH && index != SKILL_TRIPLE_SHOT && index != SKILL_POWER_SLASH && index != SKILL_SAHAMUTT && index != SKILL_NEIL && index != SKILL_GHOST_PHANTOM && index != SKILL_KILLING_BLOW && index != SKILL_UPPER_BEAST && index != SKILL_PHOENIX_SHOT)
	{
		return 1;
	}

	for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
	{
		if (lpObj->Inventory[n].IsItem() != 0)
		{
			if (lpObj->Inventory[n].m_Option1 != 0)
			{
				if (lpObj->Inventory[n].m_SpecialIndex[SPECIAL_OPTION1] == index)
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

int CSkillManager::AddSkillWeapon(LPOBJ lpObj, int index, int level) // OK
{
	int slot = 0, skill = 0;

	if ((slot = this->AddSkill(lpObj, (skill = gMasterSkillTree.GetMasterSkillWeapon(lpObj, index)), level)) >= 0)
	{
		this->GCSkillAddSend(lpObj->Index, slot, skill, (BYTE)level, 0); return 1;
	}
	else
	{
		return 0;
	}
}

int CSkillManager::DelSkillWeapon(LPOBJ lpObj, int index, int level) // OK
{
	int slot = 0, skill = 0, count = 0;

	for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
	{
		if (lpObj->Inventory[n].IsItem() != 0)
		{
			if (lpObj->Inventory[n].m_Option1 != 0)
			{
				if (lpObj->Inventory[n].m_SpecialIndex[SPECIAL_OPTION1] == index)
				{
					count++;
				}
			}
		}
	}

	if (count == 1 && (slot = this->DelSkill(lpObj, (skill = gMasterSkillTree.GetMasterSkillWeapon(lpObj, index)))) >= 0)
	{
		this->GCSkillDelSend(lpObj->Index, slot, skill, (BYTE)level, 0); return 1;
	}
	else
	{
		return 0;
	}
}

int CSkillManager::AddSkill(LPOBJ lpObj, int index, int level) // OK
{
	if (lpObj->Type == OBJECT_USER && (this->CheckSkillRequireLevel(lpObj, index) == 0 || this->CheckSkillRequireEnergy(lpObj, index) == 0 || this->CheckSkillRequireLeadership(lpObj, index) == 0 || this->CheckSkillRequireClass(lpObj, index) == 0))
	{
		return -1;
	}

	if (this->GetSkill(lpObj, index) != 0)
	{
		return -1;
	}

	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() == 0)
		{
			return ((lpObj->Skill[n].Set(lpObj->Index, index, level) == 0) ? -1 : n);
		}
	}

	return -1;
}

int CSkillManager::DelSkill(LPOBJ lpObj, int index) // OK
{
	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() != 0)
		{
			if (lpObj->Skill[n].m_index == index)
			{
				lpObj->Skill[n].Clear();
				return n;
			}
		}
	}

	return -1;
}

CSkill* CSkillManager::GetSkill(LPOBJ lpObj, int index) // OK
{
	index = ((index == SKILL_NOVA_START) ? SKILL_NOVA : index);
	index = ((index == MASTER_SKILL_ADD_NOVA_START_IMPROVED) ? SKILL_NOVA : index);

	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() != 0)
		{
			if (lpObj->Skill[n].m_index == index || lpObj->Skill[n].m_skill == index)
			{
				//gSkillManager.GCSkillAddSend(lpObj->Index, n, index, 0, 0);
				return &lpObj->Skill[n];
			}
		}
	}

	return 0;
}

int CSkillManager::GetSkillSlot(LPOBJ lpObj, int index)
{
	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() == 0)
		{
			continue;
		}
		if (lpObj->Skill[n].m_skill == index)
		{
			return n;
		}

	}
	return -1;
}

int CSkillManager::AddMasterSkill(LPOBJ lpObj, int index, int level) // OK
{
	if (lpObj->Type == OBJECT_USER && (this->CheckSkillRequireLevel(lpObj, index) == 0 || this->CheckSkillRequireEnergy(lpObj, index) == 0 || this->CheckSkillRequireLeadership(lpObj, index) == 0 || this->CheckSkillRequireClass(lpObj, index) == 0))
	{
		return -1;
	}

	if (this->GetMasterSkill(lpObj, index) != 0)
	{
		return -1;
	}

	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() == 0)
		{
			return ((lpObj->MasterSkill[n].Set(lpObj->Index, index, level) == 0) ? -1 : n);
		}
	}

	return -1;
}

int CSkillManager::AddForMasterSkill(LPOBJ lpObj, int index, int level, int group)
{
#if(GAMESERVER_UPDATE>=401)

	if (lpObj->Type == OBJECT_USER && (this->CheckSkillRequireLevel(lpObj, index) == 0 || this->CheckSkillRequireEnergy(lpObj, index) == 0 || this->CheckSkillRequireLeadership(lpObj, index) == 0 || this->CheckSkillRequireClass(lpObj, index) == 0))
	{
		return -1;
	}

	if (this->GetForMasterSkill(lpObj, index, group) != 0)
	{
		return -1;
	}

	for (int n = 0; n < MAX_FOR_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->ForMasterSkill[n].IsForMasterSkill() == 0)
		{
			return ((lpObj->ForMasterSkill[n].Set1(lpObj->Index, index, level, group) == 0) ? -1 : n);
		}
	}

	return -1;

#else

	return -1;

#endif
}
int CSkillManager::DelMasterSkill(LPOBJ lpObj, int index) // OK
{
	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() != 0)
		{
			if (lpObj->MasterSkill[n].m_index == index)
			{
				lpObj->MasterSkill[n].Clear();
				return n;
			}
		}
	}

	return -1;
}

CSkill* CSkillManager::GetMasterSkill(LPOBJ lpObj, int index) // OK
{
	index = ((index == SKILL_NOVA_START) ? SKILL_NOVA : index);

	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() != 0)
		{
			if (lpObj->MasterSkill[n].m_index == index || lpObj->MasterSkill[n].m_skill == index)
			{
				return &lpObj->MasterSkill[n];
			}
		}
	}

	return 0;
}

CSkill* CSkillManager::GetForMasterSkill(LPOBJ lpObj, int index, int group) // OK
{
	for (int n = 0; n < MAX_FOR_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->ForMasterSkill[n].IsForMasterSkill() != 0)
		{
			if ((lpObj->ForMasterSkill[n].m_index == index || lpObj->ForMasterSkill[n].m_skill == index) && lpObj->ForMasterSkill[n].m_group == group)
			{
				return &lpObj->ForMasterSkill[n];
			}
		}
	}

	return 0;
}

bool CSkillManager::ConvertSkillByte(CSkill* lpSkill, BYTE* lpMsg) // OK
{
	WORD index = MAKE_NUMBERW(((lpMsg[0] == 0xFF && lpMsg[2] == 0x00) ? 0xFF : lpMsg[2]), lpMsg[0]);

	if (index == 0xFFFF)
	{
		return 0;
	}

	lpSkill->m_level = lpMsg[1];
	lpSkill->m_skill = index;
	lpSkill->m_index = index;
	return 1;
}

void CSkillManager::SkillByteConvert(BYTE* lpMsg, CSkill* lpSkill) // OK
{
	lpMsg[0] = SET_NUMBERLB(lpSkill->m_index);
	lpMsg[1] = lpSkill->m_level;
	lpMsg[2] = SET_NUMBERHB(lpSkill->m_index);

}

bool CSkillManager::ConvertForSkillByte(CSkill* lpSkill, BYTE* lpMsg) // OK
{
	WORD index = MAKE_NUMBERW(((lpMsg[0] == 0xFF && lpMsg[2] == 0x00) ? 0xFF : lpMsg[2]), lpMsg[0]);

	if (index == 0xFFFF)
	{
		return 0;
	}

	lpSkill->m_level = lpMsg[1];
	lpSkill->m_skill = index;
	lpSkill->m_index = index;
	lpSkill->m_group = lpMsg[3];
	return 1;
}

void CSkillManager::ForSkillByteConvert(BYTE* lpMsg, CSkill* lpSkill) // OK
{
	lpMsg[0] = SET_NUMBERLB(lpSkill->m_index);
	lpMsg[1] = lpSkill->m_level;
	lpMsg[2] = SET_NUMBERHB(lpSkill->m_index);
	lpMsg[3] = lpSkill->m_group;
}

void CSkillManager::UseAttackSkill(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type == OBJECT_USER && this->CheckSkillRequireWeapon(lpObj, lpSkill->m_skill) == 0)
	{
		return;
	}

	if (lpObj->Type == OBJECT_USER && lpSkill->m_skill == SKILL_TWISTING_SLASH && lpObj->Inventory[0].IsItem() == 0)
	{
		return;
	}

	bool combo = 0;

	/*if (g_ComboSkillPath.m_CustomizeComboSwitch == 1 && g_ComboSkillPath.Combo(lpObj, bIndex, lpSkill))
	{
		if (gServerInfo.m_CheckAutoComboHack == 0 || (GetTickCount() - lpObj->ComboTime) > ((DWORD)gServerInfo.m_CheckAutoComboHackTolerance))
		{
			combo = 1;
			lpObj->ComboTime = GetTickCount();
		}
	}
	else
	{*/
		if (/*gQuest.CheckQuestListState(lpObj, 3, QUEST_FINISH) != 0 &&*/ lpObj->ComboSkill.CheckCombo(lpSkill->m_skill) != 0)
		{
			if (gServerInfo.m_CheckAutoComboHack == 0 || (GetTickCount() - lpObj->ComboTime) > ((DWORD)gServerInfo.m_CheckAutoComboHackTolerance))
			{
				combo = 1;
				lpObj->ComboTime = GetTickCount();
			}
		}
	//}

	if (lpObj->SkillNovaState != 0 && lpSkill->m_skill != SKILL_NOVA && this->GetSkill(lpObj, SKILL_NOVA) != 0)
	{
		this->RunningSkill(aIndex, 0, this->GetSkill(lpObj, SKILL_NOVA), (BYTE)lpObj->X, (BYTE)lpObj->Y, 0, 0);
		return;
	}

	if (lpSkill->m_skill == SKILL_NOVA && bIndex != SKILL_NOVA_START && bIndex != MASTER_SKILL_ADD_NOVA_START_IMPROVED)
	{
		this->RunningSkill(aIndex, bIndex, lpSkill, (BYTE)lpObj->X, (BYTE)lpObj->Y, 0, combo);
		return;
	}

	if ((lpSkill->m_skill == SKILL_UPPER_BEAST || lpSkill->m_skill == SKILL_CHAIN_DRIVER || lpSkill->m_skill == SKILL_DARK_SIDE) && lpObj->RageFighterSkillIndex == lpSkill->m_index && (lpObj->RageFighterSkillCount++) > 0)
	{
		this->RunningSkill(aIndex, bIndex, lpSkill, (BYTE)lpObj->X, (BYTE)lpObj->Y, 0, combo);
		return;
	}

	if (lpObj->Type != OBJECT_USER || (this->CheckSkillMana(lpObj, lpSkill->m_index) != 0 && this->CheckSkillBP(lpObj, lpSkill->m_index) != 0))
	{
		if (this->RunningSkill(aIndex, bIndex, lpSkill, (BYTE)lpObj->X, (BYTE)lpObj->Y, 0, combo) != 0 && lpObj->Type == OBJECT_USER)
		{
			lpObj->Mana -= ((this->GetSkillMana(lpSkill->m_index) * lpObj->MPConsumptionRate) / 100);
			lpObj->BP -= ((this->GetSkillBP(lpSkill->m_index) * lpObj->BPConsumptionRate) / 100);
			GCManaSend(aIndex, 0xFF, (int)lpObj->Mana, lpObj->BP);
		}
	}
}

void CSkillManager::UseDurationSkillAttack(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, BYTE dir, BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type == OBJECT_USER && this->CheckSkillRequireWeapon(lpObj, lpSkill->m_skill) == 0)
	{
		return;
	}

	if (lpObj->Type == OBJECT_USER && lpSkill->m_skill == SKILL_TWISTING_SLASH && lpObj->Inventory[0].IsItem() == 0)
	{
		return;
	}

	bool combo = 0;
	/*if (g_ComboSkillPath.m_CustomizeComboSwitch == 1 && g_ComboSkillPath.Combo(lpObj, bIndex, lpSkill))
	{
		if (gServerInfo.m_CheckAutoComboHack == 0 || (GetTickCount() - lpObj->ComboTime) > ((DWORD)gServerInfo.m_CheckAutoComboHackTolerance))
		{
			combo = 1;
			lpObj->ComboTime = GetTickCount();
		}
	}
	else
	{*/
		if (/*gQuest.CheckQuestListState(lpObj, 3, QUEST_FINISH) != 0 && */lpObj->ComboSkill.CheckCombo(lpSkill->m_skill) != 0)
		{
			if (gServerInfo.m_CheckAutoComboHack == 0 || (GetTickCount() - lpObj->ComboTime) > ((DWORD)gServerInfo.m_CheckAutoComboHackTolerance))
			{
				combo = 1;
				lpObj->ComboTime = GetTickCount();
			}
		}
	//}

	if (lpObj->SkillNovaState != 0 && lpSkill->m_skill != SKILL_NOVA && this->GetSkill(lpObj, SKILL_NOVA) != 0)
	{
		this->RunningSkill(aIndex, 0, this->GetSkill(lpObj, SKILL_NOVA), x, y, 0, 0);
		return;
	}

	this->GCDurationSkillAttackSend(lpObj, lpSkill->m_index, x, y, dir);

	if (lpObj->Type != OBJECT_USER || (this->CheckSkillMana(lpObj, lpSkill->m_index) != 0 && this->CheckSkillBP(lpObj, lpSkill->m_index) != 0))
	{
		if (this->RunningSkill(aIndex, bIndex, lpSkill, x, y, angle, combo) != 0 && lpObj->Type == OBJECT_USER)
		{
			lpObj->Mana -= ((this->GetSkillMana(lpSkill->m_index) * lpObj->MPConsumptionRate) / 100);
			lpObj->BP -= ((this->GetSkillBP(lpSkill->m_index) * lpObj->BPConsumptionRate) / 100);
			GCManaSend(aIndex, 0xFF, (int)lpObj->Mana, lpObj->BP);
		}
	}
}

bool CSkillManager::RunningSkill(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, BYTE angle, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	lpObj->ShieldDamageReductionTime = 0;

	if ((lpObj->Authority & 32) == 0)
	{
		gEffectManager.DelEffect(lpObj, EFFECT_INVISIBILITY);
	}

	if (this->CheckSkillBlock(lpObj, lpSkill->m_skill))
	{
		return false;
	}

	switch (lpSkill->m_skill)
	{
	case SKILL_FLAME:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_TWISTER:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_EVIL_SPIRIT:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_HELL_FIRE:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_AQUA_BEAM:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_BLAST:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_INFERNO:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_MANA_SHIELD:
		return this->SkillManaShield(aIndex, bIndex, lpSkill);
	case SKILL_DEFENSE:
		return this->SkillDefense(aIndex, bIndex, lpSkill);
	case SKILL_TRIPLE_SHOT:
		return this->SkillTripleShot(aIndex, bIndex, lpSkill);
	case SKILL_HEAL:
		return this->SkillHeal(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_DEFENSE:
		return this->SkillGreaterDefense(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_DAMAGE:
		return this->SkillGreaterDamage(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON1:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON2:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON3:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON4:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON5:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON6:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON7:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_DECAY:
		return this->SkillDecay(aIndex, bIndex, lpSkill, x, y);
	case SKILL_ICE_STORM:
		return this->SkillIceStorm(aIndex, bIndex, lpSkill, x, y);
	case SKILL_NOVA:
		return this->SkillNova(aIndex, bIndex, lpSkill);
	case SKILL_TWISTING_SLASH:
		return this->SkillTwistingSlash(aIndex, bIndex, lpSkill, combo);
	case SKILL_RAGEFUL_BLOW:
		return this->SkillRagefulBlow(aIndex, bIndex, lpSkill, combo);
	case SKILL_DEATH_STAB:
		return this->SkillDeathStab(aIndex, bIndex, lpSkill, combo);
	case SKILL_CRESCENT_MOON_SLASH:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill);
	case SKILL_MANA_GLAIVE:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill);
	case SKILL_STAR_FALL:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill);
	case SKILL_IMPALE:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_LIFE:
		return this->SkillGreaterLife(aIndex, bIndex, lpSkill);
	case SKILL_MONSTER_AREA_ATTACK:
		return this->SkillMonsterAreaAttack(aIndex, bIndex, lpSkill);
	case SKILL_PENETRATION:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_FIRE_SLASH:
		return this->SkillFireSlash(aIndex, bIndex, lpSkill);
	case SKILL_POWER_SLASH:
		return this->SkillPowerSlash(aIndex, bIndex, lpSkill, angle);
	case SKILL_SPIRAL_SLASH:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill);
	case SKILL_FORCE:
		return this->SkillForce(aIndex, bIndex, lpSkill);
	case SKILL_FIRE_BURST:
		return this->SkillFireBurst(aIndex, bIndex, lpSkill);
	case SKILL_EARTHQUAKE:
		return this->SkillEarthquake(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON_PARTY:
		return this->SkillSummonParty(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_CRITICAL_DAMAGE:
		return this->SkillGreaterCriticalDamage(aIndex, bIndex, lpSkill);
	case SKILL_ELECTRIC_SPARK:
		return this->SkillElectricSpark(aIndex, bIndex, lpSkill, angle);
	case SKILL_STERN:
		return this->SkillStern(aIndex, bIndex, lpSkill, angle);
	case SKILL_REMOVE_STERN:
		return this->SkillRemoveStern(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_MANA:
		return this->SkillGreaterMana(aIndex, bIndex, lpSkill);
	case SKILL_INVISIBILITY:
		return this->SkillInvisibility(aIndex, bIndex, lpSkill);
	case SKILL_REMOVE_INVISIBILITY:
		return this->SkillRemoveInvisibility(aIndex, bIndex, lpSkill);
	case SKILL_REMOVE_ALL_EFFECT:
		return this->SkillRemoveAllEffect(aIndex, bIndex, lpSkill);
	case SKILL_MANA_RAYS:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill);
	case SKILL_FIRE_BLAST:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill);
	case SKILL_BRAND:
		return this->SkillBrand(aIndex, bIndex, lpSkill);
	case SKILL_PLASMA_STORM:
		return this->SkillPlasmaStorm(aIndex, bIndex, lpSkill);
	case SKILL_INFINITY_ARROW:
		return this->SkillInfinityArrow(aIndex, bIndex, lpSkill);
	case SKILL_FIRE_SCREAM:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill);
	case SKILL_MONSTER_SUMMON:
		return this->SkillMonsterSummon(aIndex, bIndex, lpSkill);
	case SKILL_MAGIC_DAMAGE_IMMUNITY:
		return this->SkillMagicDamageImmunity(aIndex, bIndex, lpSkill);
	case SKILL_PHYSI_DAMAGE_IMMUNITY:
		return this->SkillPhysiDamageImmunity(aIndex, bIndex, lpSkill);
	case SKILL_CHAIN_LIGHTNING:
		return this->SkillChainLightning(aIndex, bIndex, lpSkill);
	case SKILL_DAMAGE_REFLECT:
		return this->SkillDamageReflect(aIndex, bIndex, lpSkill);
	case SKILL_SWORD_POWER:
		return this->SkillSwordPower(aIndex, bIndex, lpSkill);
	case SKILL_SLEEP:
		return this->SkillSleep(aIndex, bIndex, lpSkill);
	case SKILL_LESSER_DEFENSE:
		return this->SkillLesserDefense(aIndex, bIndex, lpSkill);
	case SKILL_LESSER_DAMAGE:
		return this->SkillLesserDamage(aIndex, bIndex, lpSkill);
	case SKILL_SAHAMUTT:
		return this->SkillSahamutt(aIndex, bIndex, lpSkill, x, y);
	case SKILL_NEIL:
		return this->SkillNeil(aIndex, bIndex, lpSkill, x, y);
	case SKILL_GHOST_PHANTOM:
		return this->SkillGhostPhantom(aIndex, bIndex, lpSkill, x, y);
	case SKILL_RED_STORM:
		return this->SkillRedStorm(aIndex, bIndex, lpSkill);
	case SKILL_FROZEN_STAB:
		return this->SkillFrozenStab(aIndex, bIndex, lpSkill, x, y, combo);
	case SKILL_MAGIC_CIRCLE:
		return this->SkillMagicCircle(aIndex, bIndex, lpSkill);
	case SKILL_SHIELD_RECOVER:
		return this->SkillShieldRecover(aIndex, bIndex, lpSkill);
	case SKILL_FIVE_SHOT:
		return this->SkillFiveShot(aIndex, bIndex, lpSkill, angle);
	case SKILL_FLAME_STRIKE:
		return this->SkillSwordSlash(aIndex, bIndex, lpSkill, angle);
	case SKILL_LIGHTNING_STORM:
		return this->SkillLightningStorm(aIndex, bIndex, lpSkill);
	case SKILL_BIRDS:
		return this->SkillBirds(aIndex, bIndex, lpSkill, angle);
	case SKILL_SELF_EXPLOSION:
		return this->SkillSelfExplosion(aIndex, bIndex, lpSkill);
	case SKILL_KILLING_BLOW:
		return this->SkillKillingBlow(aIndex, bIndex, lpSkill);
	case SKILL_UPPER_BEAST:
		return this->SkillUpperBeast(aIndex, bIndex, lpSkill);
	case SKILL_CHAIN_DRIVER:
		return this->SkillChainDriver(aIndex, bIndex, lpSkill);
	case SKILL_DARK_SIDE:
		return this->SkillDarkSide(aIndex, bIndex, lpSkill);
	case SKILL_DRAGON_LORE:
		return this->SkillDragonLore(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_IGNORE_DEFENSE_RATE:
		return this->SkillGreaterIgnoreDefenseRate(aIndex, bIndex, lpSkill);
	case SKILL_FITNESS:
		return this->SkillFitness(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_DEFENSE_SUCCESS_RATE:
		return this->SkillGreaterDefenseSuccessRate(aIndex, bIndex, lpSkill);
	case SKILL_CHARGE:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill);
	case SKILL_PHOENIX_SHOT:
		return this->SkillPhoenixShot(aIndex, bIndex, lpSkill);
	case SKILL_BLOOD_STORM:
		return this->SkillBloodStorm(aIndex, bIndex, lpSkill, x, y, combo);
	case SKILL_CURE:
		return this->SkillCure(aIndex, bIndex, lpSkill);
	case SKILL_PARTY_HEAL:
		return this->SkillPartyHeal(aIndex, bIndex, lpSkill);
	case SKILL_POISON_ARROW:
		return this->SkillPoisonArrow(aIndex, bIndex, lpSkill);
	case SKILL_BLESS:
		return this->SkillBless(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON_SATYROS:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_BLIND:
		return this->SkillBlind(aIndex, bIndex, lpSkill);
	case SKILL_EARTH_PRISON:
		return this->SkillEarthPrison(aIndex, bIndex, lpSkill);
	case SKILL_IRON_DEFENSE1:
	case SKILL_IRON_DEFENSE:
		return this->SkillIronDefense(aIndex, bIndex, lpSkill);
	case SKILL_BLOOD_HOWLING:
		return this->SkillBloodHowling(aIndex, bIndex, lpSkill);
	case SKILL_SPIN_STEP:
		return this->SkillSpinStep(aIndex, bIndex, lpSkill, combo);
	case SKILL_CIRCLE_SHIELD:
		return this->SkillCircleShield(aIndex, bIndex, lpSkill);
	case SKILL_OBSIDIAN:
		return this->SkillObsidian(aIndex, bIndex, lpSkill);
	case SKILL_MAGIC_PIN:
		return this->SkillMagicPin(aIndex, bIndex, lpSkill, combo);
	case SKILL_CLASH:
		return this->SkillClash(aIndex, bIndex, lpSkill);
	case SKILL_HARSH_STRIKE:
		return this->SkillHarshStrike(aIndex, bIndex, lpSkill, combo);
	case SKILL_SHINING_PEAK:
		return this->SkillShiningPeak(aIndex, bIndex, lpSkill, combo);
	case SKILL_WRATH:
		return this->SkillWrath(aIndex, bIndex, lpSkill);
	case SKILL_BRECHE:
		return this->SkillBreche(aIndex, bIndex, lpSkill, combo, angle);
	case SKILL_SPIRIT_HOOK:
		return this->SkillSpiritHook(aIndex, bIndex, lpSkill);
	case SKILL_BURST:
		return this->SkillBurst(aIndex, bIndex, lpSkill);
	case SKILL_RUSH:
		return this->SkillRush(aIndex, bIndex, lpSkill, combo);
	case SKILL_ILLUSION:
		return this->SkillIllusion(aIndex, bIndex, lpSkill);
	case SKILL_SHADOW_STEP:
		return this->SkillShadowStep(aIndex, bIndex, lpSkill);
	case SKILL_EVASION:
		return this->SkillEvasion(aIndex, bIndex, lpSkill);
	case SKILL_PARALYZE:
		return this->SkillParalyze(aIndex, bIndex, lpSkill);
	case SKILL_FIRE_BLOW:
		return this->SkillFireBlow(aIndex, bIndex, lpSkill, combo);
	case SKILL_DW_METEOR_STRIKE:
		return this->MeteorStrike_DW(aIndex, bIndex, lpSkill, combo);
	case SKILL_METEOR_STORM:
		return this->MeteorStorm(aIndex, bIndex, lpSkill, combo);
	case SKILL_SOUL_SEEKER:
		return this->SoulSeeker(aIndex, bIndex, lpSkill, combo);
	case SKILL_FOCUS_SHOT:
		return this->FocusShot(aIndex, bIndex, lpSkill, combo);
	case SKILL_FIRE_ARROW:
	case SKILL_FIRE_BEAST:
		return this->FireBeast(aIndex, bIndex, lpSkill, combo);
	case SKILL_AQUA_BEAST:
		return this->AquaBeast(aIndex, bIndex, lpSkill, combo);
	case SKILL_ICE_BLOOD:
		return this->IceBlood(aIndex, bIndex, lpSkill, combo);
	case SKILL_FIRE_BLOOD:
		return this->FireBlood(aIndex, bIndex, lpSkill, combo);
	case SKILL_DARK_BLAST:
		return this->DarkBlast(aIndex, bIndex, lpSkill, combo);
	case SKILL_MG_METEOR_STRIKE:
		return this->MeteorStrike_MG(aIndex, bIndex, lpSkill, combo);
	case SKILL_FLAME_SPEAR:
	case SKILL_FIRE_WAVE:
		break;
	case SKILL_WIND_SOUL:
		return this->SkillWindSoul(aIndex, bIndex, lpSkill, combo, angle);
	case SKILL_VOLCANO:
		return this->Volcano(aIndex, bIndex, lpSkill, combo);
	case SKILL_DARK_PHOENIX_SHOT:
		return this->DarkPhoenxiShot(aIndex, bIndex, lpSkill);
	case SKILL_ARCHANGEL_S_WILL:
		return this->ArchangelWill(aIndex, bIndex, lpSkill);
		break;
	default:
		return this->BasicSkillAttack(aIndex, bIndex, lpSkill);
	}

	return 0;
}

#if(GAMESERVER_UPDATE>=1100)//2017/04/28

bool CSkillManager::ArchangelWill(int aIndex, int bIndex, CSkill* lpSkill)
{
	return 0;

	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	int count = gServerInfo.m_ArchangelWillCount;
	int value = gServerInfo.m_ArchangelWillValue;

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::DarkPhoenxiShot(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, 0);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillWindSoul(int aIndex, int bIndex, CSkill* lpSkill, bool combo, BYTE angle)
{	
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	int addRange = 0;

	if (lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 4) && lpObj->Inventory[lpObj->m_btInvenPetPos].m_JewelOfHarmonyOption == 1)
	{
		addRange = 2;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y, addRange) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, 0);

	if (lpTarget->Type == OBJECT_USER)
	{
		return 1;
	}

	int SkillFrustrumX[4], SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 6.0f, 7.0f, 2.0f, 0.0f);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (index == bIndex)
		{
			continue;
		}

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y, addRange) == 0)
		{
			continue;
		}

		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::Volcano(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, combo);

	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}
bool CSkillManager::FireBeast(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);

	/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1, 2))
	{
		gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);
	}*/

	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::AquaBeast(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, combo);

	/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1, 1))
	{
		gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 0, combo);
	}*/

	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::FireBlood(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 1;
	}

	if (lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 2, combo);
	//gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 3, combo);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1, 0))
		{
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::IceBlood(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 1;
	}

	if (lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, combo);

	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1, 1))
		{
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		}*/

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}

bool CSkillManager::DarkBlast(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, combo);
	
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1, 3))
		{
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		}
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::MeteorStrike_MG(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, combo);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::FocusShot(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, combo);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 4, combo);

	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE2, 2))
		{
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		}*/

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SoulSeeker(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		//LogAdd(LOG_DEBUG, "%d", __LINE__);
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, 0);

	if (lpTarget->Type == OBJECT_USER)
	{
		//LogAdd(LOG_DEBUG, "%d", __LINE__);
		return 1;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::MeteorStorm(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}


		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 1, combo);
		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 2, combo);

		/*if (this->GetForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1, lpSkill->m_group) != 0)
		{
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 3, 0);
		}*/

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::MeteorStrike_DW(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 1, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillFireBlow(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 0, combo);

	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	if (lpTarget->Type == OBJECT_USER)
	{
		return 1;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}
#endif

bool CSkillManager::SkillParalyze(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];
	
	int value = 50;
	int count = 10;
	int damage = 500 + gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SU_PARALYZE_POW_UP);

	if (lpSkill->m_index == MASTER_SKILL_ADD_SU_PARALYZE_POW_UP)
	{
		gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), 10, lpObj->Index, (1 / 2), SET_NUMBERHW(damage), SET_NUMBERLW(damage));
	}

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

		this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
	}

	return 1;
}

bool CSkillManager::SkillEvasion(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value = lpObj->m_MPSkillOpt.DefenseSuccessRate / 2;

	int count = 7;

	gSkillManager.GCSkillAttackSend(lpObj, lpSkill->m_index, lpObj->Index, 1);

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	//Remove all immobilizing effects
	gEffectManager.DelEffect(lpObj, EFFECT_ICE);
	gEffectManager.DelEffect(lpObj, EFFECT_ICE_ARROW);
	gEffectManager.DelEffect(lpObj, EFFECT_STERN);
	gEffectManager.DelEffect(lpObj, EFFECT_ORDER_OF_RESTRAINT);
	gEffectManager.DelEffect(lpObj, EFFECT_SLEEP);
	gEffectManager.DelEffect(lpObj, EFFECT_BLIND);
	gEffectManager.DelEffect(lpObj, EFFECT_FROZEN_STAB);
	gEffectManager.DelEffect(lpObj, EFFECT_ICE_STORM_ENHANCED);
	gEffectManager.DelEffect(lpObj, EFFECT_EARTH_PRISON);
	gEffectManager.DelEffect(lpObj, EFFECT_PENTAGRAM_JEWEL_STUN);
	gEffectManager.DelEffect(lpObj, EFFECT_BONDAGE);

	return 1;
}

bool CSkillManager::SkillShadowStep(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillIllusion(int aIndex, int bIndex, CSkill* lpSkill)
{
	return 0;

	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		gObjSummonKill(aIndex);
		GCSummonLifeSend(aIndex, 0, 1);
		return 0;
	}

	int index = gObjAddSummon();//10500

	if (OBJECT_RANGE(index) == 0)
	{
		return 0;
	}

	lpObj->SummonIndex = index;

	LPOBJ lpSummon = &gObj[index];
	lpSummon->X = lpObj->X - 1;
	lpSummon->Y = lpObj->Y + 1;
	lpSummon->MTX = lpSummon->X;
	lpSummon->MTY = lpSummon->Y;
	lpSummon->Dir = 2;
	lpSummon->Map = lpObj->Map;

	gObjSetMonster(index, 707);

	lpSummon->Level = lpObj->Level;
	lpSummon->Strength = lpObj->Strength;
	lpSummon->Dexterity = lpObj->Dexterity;
	lpSummon->Vitality = lpObj->Vitality;
	lpSummon->Energy = lpObj->Energy;
	lpSummon->Leadership = lpObj->Leadership;

	lpSummon->Life = lpObj->Life;
	lpSummon->MaxLife = lpObj->MaxLife;
	lpSummon->Mana = lpObj->Mana;
	lpSummon->MaxMana = lpObj->MaxMana;
	lpSummon->ScriptMaxLife = lpObj->ScriptMaxLife;
	lpSummon->Defense = lpObj->Defense;
	lpSummon->PhysiDamageMin = lpObj->PhysiDamageMin;
	lpSummon->PhysiDamageMax = lpObj->PhysiDamageMax;
	lpSummon->SummonIndex = aIndex;
	lpSummon->Attribute = 100;
	lpSummon->TargetNumber = -1;
	lpSummon->ActionState.Attack = 0;
	lpSummon->ActionState.Emotion = 0;
	lpSummon->ActionState.EmotionCount = 0;
	lpSummon->PathCount = 0;
	lpSummon->MoveRange = 15;

	memcpy(&lpSummon->CharSet, &lpObj->CharSet, sizeof(lpObj->CharSet));
	GCSummonLifeSend(lpSummon->SummonIndex, (int)lpSummon->Life, (int)lpSummon->MaxLife);

	return 1;
}

bool CSkillManager::SkillRush(int aIndex, int bIndex, CSkill* lpSkill, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 0, combo);

	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillBurst(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value = 5;

	int count = 60;

	SKILL_INFO SkillInfo;

	if (this->GetInfo(lpSkill->m_index, &SkillInfo) == 0)
	{
		return 0;
	}

	lpObj->SkillDelay[lpSkill->m_index] -= gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_BURST_POW_UP) * 1000;

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillSpinStep(int aIndex, int bIndex, CSkill* lpSkill, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	int skill = lpSkill->m_skill;

	lpSkill->m_skill = SKILL_FK_EXPLOSION;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (index == bIndex)
		{
			continue;
		}

		if (this->CheckSkillTarget(lpObj, index, -1, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	lpSkill->m_skill = skill;

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillCircleShield(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, lpObj->Index, 1);

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), gServerInfo.m_CircleShieldCount, gServerInfo.m_CircleShieldValue, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillObsidian(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value = gServerInfo.m_ObsidianConst + ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_ObsidianConst);

	value += ((value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_OBSIDIAN_POW_UP)) / 100);

	value = ((value > gServerInfo.m_ObsidianMax) ? gServerInfo.m_ObsidianMax : value);

	int count = gServerInfo.m_ObsidianTimeConst + ((lpObj->Strength + lpObj->Strength) / gServerInfo.m_ObsidianTimeConst);

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillWrath(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value = lpObj->Defense * gServerInfo.m_WrathCutDefConst / 100;

	if (gEffectManager.CheckEffect(lpObj, this->GetSkillEffect(lpSkill->m_index)) == FALSE)
	{
		if (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_WRATH_MASTERY))
		{
			value = 0;
			int value1 = lpObj->PhysiDamageMaxRight * gServerInfo.m_WrathConst / 100;
			//int count = gServerInfo.m_WrathTimeConst + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_WrathTimeConst);
			int count = 300;
			count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_WRATH_POW_UP);
			gEffectManager.AddEffect(lpObj, 0, EFFECT_WRATH_MAS, count, value, value1, 0, 0);
		}
		else
		{
			int value1 = lpObj->PhysiDamageMaxRight * gServerInfo.m_WrathConst / 100;
			//int count = gServerInfo.m_WrathTimeConst + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_WrathTimeConst);
			int count = 300;
			count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_WRATH_POW_UP);
			gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, value1, 0, 0);
		}
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	return 1;
}

bool CSkillManager::SkillMagicPin(int aIndex, int bIndex, CSkill* lpSkill, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 1;
	}

	int addRange = 0;

	if (gMasterSkillTree.GetMasterSkillLevel(lpObj, MASTER_SKILL_ADD_FK_MAGIC_PIN_MASTERY) > 0)
	{
		addRange = 1;
	}

	if (lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y, addRange) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 1, combo);
	//gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 2, 0);
	//gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 3, 0);

	/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE2, 1))
	{
		gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 4, 0);
		gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 5, 0);
	}*/

	return 1;
}

bool CSkillManager::SkillClash(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 1;
	}

	if (lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillHarshStrike(int aIndex, int bIndex, CSkill* lpSkill, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 1, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, 0);

	/*if (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_HARSH_STRIKE_MASTERY))
	{
		gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, 0);
	}*/

	return 1;
}

bool CSkillManager::SkillBreche(int aIndex, int bIndex, CSkill* lpSkill, bool combo, BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int addRange = 0;

	if (gMasterSkillTree.GetMasterSkillLevel(lpObj, MASTER_SKILL_ADD_FK_BRECHE_MASTERY) > 0)
	{
		addRange = 1;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y, addRange) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillShiningPeak(int aIndex, int bIndex, CSkill* lpSkill, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, 0);

	if (lpTarget->Type == OBJECT_USER)
	{
		return 1;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			return 0;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::BasicSkillAttack(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 1;
	}

	if (lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, 0);

	/*if (lpSkill->m_skill == SKILL_FIRE_SLASH)
	{
		if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1, 0))
		{
			gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 0);
		}
	}*/

	return 1;
}

bool CSkillManager::MultiSkillAttack(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type == OBJECT_USER)
	{
		lpObj->MultiSkillIndex = lpSkill->m_index;

		lpObj->MultiSkillCount = 0;

		switch (lpSkill->m_skill)
		{
		case SKILL_FLAME:
			this->BasicSkillAttack(aIndex, bIndex, lpSkill);
			break;
		case SKILL_BLAST:
			this->BasicSkillAttack(aIndex, bIndex, lpSkill);
			break;
		case SKILL_IMPALE:
			this->BasicSkillAttack(aIndex, bIndex, lpSkill);
			break;
		/*case SKILL_FIRE_SLASH:			
			this->BasicSkillAttack(aIndex, bIndex, lpSkill);
			break;*/
		case SKILL_FIRE_SCREAM:
			this->BasicSkillAttack(aIndex, bIndex, lpSkill);
			break;
		}
	}
	else
	{
		switch (lpSkill->m_skill)
		{
		case SKILL_EVIL_SPIRIT:
			this->SkillEvilSpirit(aIndex, bIndex, lpSkill);
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillChangeUse(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	int change = -1;

	for (int n = 0; n < 2; n++)
	{
		CItem* Ring = &lpObj->Inventory[10 + n];

		if (Ring->IsItem() == 0 || Ring->m_Durability == 0 || change != -1)
		{
			continue;
		}

		if (Ring->m_Index == GET_ITEM(13, 10)) // Transformation Ring
		{
			change = Ring->m_Level;
		}
		else if (Ring->m_Index == GET_ITEM(13, 39)) // Ring 1
		{
			change = 372;
		}
		else if (Ring->m_Index == GET_ITEM(13, 40)) // Halloween Ring
		{
			change = 373;
		}
		else if (Ring->m_Index == GET_ITEM(13, 41)) // Christmas Ring
		{
			change = 374;
		}
		else if (Ring->m_Index == GET_ITEM(13, 42) && lpObj->Authority == 32) // GM Ring
		{
			change = 378;
		}
		else if (Ring->m_Index == GET_ITEM(13, 68)) // Snowman Ring
		{
			change = 477;
		}
		else if (Ring->m_Index == GET_ITEM(13, 76)) // Panda Ring
		{
			change = 503;
		}
		else if (Ring->m_Index == GET_ITEM(13, 77)) // Brown Panda Ring
		{
			change = 616;
		}
		else if (Ring->m_Index == GET_ITEM(13, 78)) // Pink Panda Ring
		{
			change = 617;
		}
		else if (Ring->m_Index == GET_ITEM(13, 122)) // Skeleton Ring
		{
			change = 548;
		}
		else if (Ring->m_Index == GET_ITEM(13, 163)) // Robot Knight Ring
		{
			change = 625;
		}
		else if (Ring->m_Index == GET_ITEM(13, 164)) // Mini Robot Ring
		{
			change = 626;
		}
		else if (Ring->m_Index == GET_ITEM(13, 165)) // Great Heavenly Mage Ring
		{
			change = 642;
		}
		else if (Ring->m_Index == GET_ITEM(13, 268)) // Dark Transform Ring
		{
			change = 706;
		}
	}

	switch (change)
	{
	case 0:
		change = gServerInfo.m_TransformationRing1;
		break;
	case 1:
		change = gServerInfo.m_TransformationRing2;
		break;
	case 2:
		change = gServerInfo.m_TransformationRing3;
		break;
	case 3:
		change = gServerInfo.m_TransformationRing4;
		break;
	case 4:
		change = gServerInfo.m_TransformationRing5;
		break;
	case 5:
		change = gServerInfo.m_TransformationRing6;
		break;
	}

	if (IT_MAP_RANGE(lpObj->Map) != 0 && (gIllusionTemple.GetState() == IT_STATE_STAND || gIllusionTemple.GetState() == IT_STATE_START || gIllusionTemple.GetState() == IT_STATE_CLEAN))
	{
		switch (gIllusionTemple.GetUserJoinTeam(lpObj))
		{
		case IT_TEAM_Y:
			change = 404;
			break;
		case IT_TEAM_B:
			change = 405;
			break;
		}
	}

	if (lpObj->Change == change)
	{
		return 0;
	}
	else
	{
		lpObj->Change = change;
		return 1;
	}
}

bool CSkillManager::SkillEvilSpirit(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillManaShield(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return 0;
	}

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int value1 = gServerInfo.m_ManaShieldConstA + ((lpObj->Dexterity + lpObj->AddDexterity) / gServerInfo.m_ManaShieldConstB) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ManaShieldConstC);

	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MANA_SHIELD_IMPROVED);

	value1 = ((value1 > gServerInfo.m_ManaShieldMaxRate) ? gServerInfo.m_ManaShieldMaxRate : value1);

	value1 = (value1 * ((lpTarget->Type == OBJECT_USER) ? gServerInfo.m_ManaShieldRate[lpTarget->Class] : 100)) / 100;

	//int value2 = (int)((lpObj->MaxMana * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MANA_SHIELD_MASTERED)) / 100);
	int value2 = 0;

	//int count = gServerInfo.m_ManaShieldTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ManaShieldTimeConstB);

	int count = 300;
	count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MANA_SHIELD_ENHANCED);

	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillDefense(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Inventory[1].IsItem() != 0)
	{
		if (lpObj->Inventory[1].m_Index >= GET_ITEM(6, 0) && lpObj->Inventory[1].m_Index < GET_ITEM(7, 0))
		{
			if (lpObj->Inventory[1].m_Option1 != 0)
			{
				lpObj->ShieldDamageReductionTime = GetTickCount();
				GCActionSend(lpObj, SKILL_DEFENSE, lpObj->Index, bIndex);
				return 1;
			}
		}
	}

	return 0;
}

bool CSkillManager::SkillHeal(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = gServerInfo.m_HealConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_HealConstB);

	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_HEAL_IMPROVED)) / 100;

	if ((lpTarget->Life + value) > (lpTarget->MaxLife + lpTarget->AddLife))
	{
		lpTarget->Life = lpTarget->MaxLife + lpTarget->AddLife;
	}
	else
	{
		lpTarget->Life += value;
	}

	if (lpTarget->Type == OBJECT_USER)
	{
		GCLifeSend(bIndex, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
	}

	if (lpTarget->Type == OBJECT_MONSTER && OBJECT_RANGE(lpTarget->SummonIndex) != 0)
	{
		GCSummonLifeSend(lpTarget->SummonIndex, (int)lpTarget->Life, (int)lpTarget->MaxLife);
	}

	//gCGHuntingGrounds.AddHealValue(lpTarget->Index, value);

	/*for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index, (value / 40));
		}
	}*/

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillGreaterDefense(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

#if(GAMESERVER_TYPE==1)

	if (lpObj->Map == MAP_CASTLE_SIEGE && lpTarget->Map == MAP_CASTLE_SIEGE && gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (lpObj->CsJoinSide != 0 && lpTarget->CsJoinSide != 0)
		{
			if (lpObj->CsJoinSide != lpTarget->CsJoinSide)
			{
				return 0;
			}
		}
	}

#endif

	int value = gServerInfo.m_GreaterDefenseConstA;
	value += (int)((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterDefenseConstB);
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_IMPROVED)) / 100;
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_ENHANCED)) / 100;
	value += (value * lpObj->ElfGreaterDefenseReinforcement) / 100;
	value = (value * ((lpTarget->Type == OBJECT_USER) ? gServerInfo.m_GreaterDefenseRate[lpTarget->Class] : 100)) / 100;
	value = ((value > gServerInfo.m_GreaterDefenseMax) ? gServerInfo.m_GreaterDefenseMax : value);

	int count = gServerInfo.m_GreaterDefenseTimeConstA;
	count += gMasterSkillTree.GetMasterSkillLevel(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_ENHANCED);

	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);	

	/*for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index, (value / 10));
		}
	}*/

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillGreaterDamage(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

#if(GAMESERVER_TYPE==1)

	if (lpObj->Map == MAP_CASTLE_SIEGE && lpTarget->Map == MAP_CASTLE_SIEGE && gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (lpObj->CsJoinSide != 0 && lpTarget->CsJoinSide != 0)
		{
			if (lpObj->CsJoinSide != lpTarget->CsJoinSide)
			{
				return 0;
			}
		}
	}

#endif

	int value = gServerInfo.m_GreaterDamageConstA;
	value += (int) ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterDamageConstB);
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DAMAGE_IMPROVED)) / 100;
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DAMAGE_ENHANCED)) / 100;
	value += (value * lpObj->ElfGreaterDamageReinforcement) / 100;	
	value = (value * ((lpTarget->Type == OBJECT_USER) ? gServerInfo.m_GreaterDamageRate[lpTarget->Class] : 100)) / 100;
	value = ((value > gServerInfo.m_GreaterDamageMax) ? gServerInfo.m_GreaterDamageMax : value);

	int count = gServerInfo.m_GreaterDamageTimeConstA;
	count += gMasterSkillTree.GetMasterSkillLevel(lpObj, MASTER_SKILL_ADD_GREATER_DAMAGE_ENHANCED);

	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	/*for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index, (value / 10));
		}
	}*/

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillSummon(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (BC_MAP_RANGE(lpObj->Map) || CC_MAP_RANGE(lpObj->Map) != 0)
	{
		return 0;
	}

	int monster = 0;

	switch (lpSkill->m_index)
	{
	case SKILL_SUMMON1:
		monster = gServerInfo.m_SummonMonster1;
		break;
	case SKILL_SUMMON2:
		monster = gServerInfo.m_SummonMonster2;
		break;
	case SKILL_SUMMON3:
		monster = gServerInfo.m_SummonMonster3;
		break;
	case SKILL_SUMMON4:
		monster = gServerInfo.m_SummonMonster4;
		break;
	case SKILL_SUMMON5:
		monster = gServerInfo.m_SummonMonster5;
		break;
	case SKILL_SUMMON6:
		monster = gServerInfo.m_SummonMonster6;
		break;
	case SKILL_SUMMON7:
		monster = gServerInfo.m_SummonMonster7;
		break;
	case SKILL_SUMMON_SATYROS:
		monster = gServerInfo.m_SummonMonster8;
		break;
	default:
		return 0;
	}

	if (OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		gObjSummonKill(aIndex);
		GCSummonLifeSend(aIndex, 0, 1);
		return 0;
	}

	int index = gObjAddSummon();//10500

	if (OBJECT_RANGE(index) == 0)
	{
		return 0;
	}

	lpObj->SummonIndex = index;

	LPOBJ lpSummon = &gObj[index];

	lpSummon->X = lpObj->X - 1;
	lpSummon->Y = lpObj->Y + 1;
	lpSummon->MTX = lpSummon->X;
	lpSummon->MTY = lpSummon->Y;
	lpSummon->Dir = 2;
	lpSummon->Map = lpObj->Map;

	gObjSetMonster(index, monster);

	lpSummon->Life += (float)((__int64)lpSummon->Life * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
	lpSummon->MaxLife += (float)((__int64)lpSummon->MaxLife * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
	lpSummon->ScriptMaxLife += (float)((__int64)lpSummon->ScriptMaxLife * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
	lpSummon->Defense += ((__int64)lpSummon->Defense * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DEFENSE)) / 100;
	lpSummon->PhysiDamageMin += ((__int64)lpSummon->PhysiDamageMin * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DAMAGE)) / 100;
	lpSummon->PhysiDamageMax += ((__int64)lpSummon->PhysiDamageMax * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DAMAGE)) / 100;

	lpSummon->SummonIndex = aIndex;
	lpSummon->Attribute = 100;
	lpSummon->TargetNumber = -1;
	lpSummon->ActionState.Attack = 0;
	lpSummon->ActionState.Emotion = 0;
	lpSummon->ActionState.EmotionCount = 0;
	lpSummon->PathCount = 0;
	lpSummon->MoveRange = 15;

	GCSummonLifeSend(lpSummon->SummonIndex, (int)lpSummon->Life, (int)lpSummon->MaxLife);
	return 1;
}

bool CSkillManager::SkillDecay(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillIceStorm(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillNova(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (bIndex != SKILL_NOVA_START && bIndex != MASTER_SKILL_ADD_NOVA_START_IMPROVED)
	{
		if (lpObj->SkillNovaState == 0)
		{
			return 0;
		}
	}
	else
	{
		if (lpObj->SkillNovaState != 0)
		{
			return 0;
		}

		lpObj->SkillNovaState = 1;
		lpObj->SkillNovaCount = 0;
		lpObj->SkillNovaTime = GetTickCount();
		this->GCSkillAttackSend(lpObj, 58, aIndex, 1);
		return 1;
	}

	lpObj->SkillNovaState = 0;
	lpObj->SkillNovaTime = 0;

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}

bool CSkillManager::SkillTwistingSlash(int aIndex, int bIndex, CSkill* lpSkill, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillRagefulBlow(int aIndex, int bIndex, CSkill* lpSkill, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillDeathStab(int aIndex, int bIndex, CSkill* lpSkill, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 0, combo);

	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, combo);

		/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE2, 0))
		{
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		}*/

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillGreaterLife(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	int value1 = gServerInfo.m_GreaterLifeConstA + ((lpObj->Vitality + lpObj->AddVitality) / gServerInfo.m_GreaterLifeConstB) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterLifeConstC);

	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_LIFE_IMPROVED);
	value1  = ((value1 > gServerInfo.m_GreaterLifeMaxRate) ? gServerInfo.m_GreaterLifeMaxRate : value1);

	int value2 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_LIFE_ENHANCED);
	int value3 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_LIFE_MASTERED);

	//int count = gServerInfo.m_GreaterLifeTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterLifeTimeConstB);
	int count = 300;

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		value1 = (value1 * ((lpObj->Type == OBJECT_USER) ? gServerInfo.m_GreaterLifeRate[lpObj->Class] : 100)) / 100;
		gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);
		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];

		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];

			if (OBJECT_RANGE(index) == 0)
			{
				continue;
			}

			if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
			{
				continue;
			}

			value1 = (value1 * ((gObj[index].Type == OBJECT_USER) ? gServerInfo.m_GreaterLifeRate[gObj[index].Class] : 100)) / 100;
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillMonsterAreaAttack(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillPowerSlash(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int SkillFrustrumX[4], SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 6.0f, 6.0f, 1.0f, 0.0f);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillFireSlash(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];
	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillTarget(lpObj, bIndex, bIndex, lpTarget->Type) == 0)
	{
		return false;
	}

	if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return false;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillForce(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	if (lpObj->SkillLongSpearChange == 0)
	{
		gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 0, 0);
		return 1;
	}

	int angle = this->GetSkillAngle(lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (gSkillHitBox.HitCheck(angle, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillFireBurst(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 0, 0);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y, 2) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE2, 2))
		{
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);
		}*/

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillEarthquake(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y, 2) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillSummonParty(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	if (OBJECT_RANGE(lpObj->PartyNumber) != 0)
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];

		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];

			if (OBJECT_RANGE(index) == 0 || aIndex == index)
			{
				continue;
			}

			if (gMoveSummon.CheckMoveSummon(&gObj[index], lpObj->Map, lpObj->X, lpObj->Y) == 0)
			{
				gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, "O transporte foi cancelado");
				continue;
			}

			LPOBJ lpTarget = &gObj[index];

#if(GAMESERVER_TYPE==1)

			if (lpObj->Map == MAP_CASTLE_SIEGE && gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				if (lpObj->CsJoinSide != lpTarget->CsJoinSide)
				{
					gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, "O transporte foi cancelado");
					continue;
				}

				if (gCastleSiege.IsInDragonTower(lpObj) && gServerInfo.m_CastleSiegeMaxMembersInTower > 0 && lpTarget->Guild && lpTarget->GuildStatus != GUILD_MASTER)
				{
					int membersInside = 0;
					char szMasterGuildName[8];
					memset(&szMasterGuildName, 0, 8);

					if (lpTarget->Guild->GuildUnion != 0)
					{
						if (gUnionManager.GetUnionName(lpTarget->Guild->GuildUnion, szMasterGuildName) != -1)
						{
							GUILD_INFO_STRUCT* mainGuild = gGuildClass.SearchGuild(szMasterGuildName);
							if (mainGuild != NULL)
							{
								membersInside = mainGuild->membersInTower;
							}
							else
							{
								membersInside = lpTarget->Guild->membersInTower;
							}
						}
					}
					else
					{
						membersInside = lpTarget->Guild->membersInTower;
					}

					if (membersInside >= gServerInfo.m_CastleSiegeMaxMembersInTower)
					{
						gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Entrada bloqueada. Limite atingido: %d/%d", membersInside, gServerInfo.m_CastleSiegeMaxMembersInTower);
						gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, "Entrada bloqueada. Limite atingido: %d/%d", membersInside, gServerInfo.m_CastleSiegeMaxMembersInTower);
						continue;
					}
				}
			}
#endif
			int px = lpObj->X;
			int py = lpObj->Y;

			if (gObjGetRandomFreeLocation(lpObj->Map, &px, &py, 4, 4, 10) == 0)
			{
				gObj[index].SkillSummonPartyTime = 5;
				gObj[index].SkillSummonPartyMap = lpObj->Map;
				gObj[index].SkillSummonPartyX = (BYTE)lpObj->X;
				gObj[index].SkillSummonPartyY = (BYTE)lpObj->Y;

				gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(273), gObj[index].SkillSummonPartyTime);
			}
			else
			{
				gObj[index].SkillSummonPartyTime = 5;
				gObj[index].SkillSummonPartyMap = lpObj->Map;
				gObj[index].SkillSummonPartyX = (BYTE)px;
				gObj[index].SkillSummonPartyY = (BYTE)py;

				gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(273), gObj[index].SkillSummonPartyTime);
			}
		}
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}

bool CSkillManager::SkillGreaterCriticalDamage(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	int value1 = ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterCriticalDamageConstA) + ((lpObj->Leadership + lpObj->AddLeadership) / gServerInfo.m_GreaterCriticalDamageConstB);

	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_CRITICAL_DAMAGE_IMPROVED);

	value1 = ((value1 > gServerInfo.m_GreaterCriticalDamageMax) ? gServerInfo.m_GreaterCriticalDamageMax : value1);

	int value2 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_CRITICAL_DAMAGE_MASTERED);
	int value3 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_CRITICAL_DAMAGE_EXTENDED);

	//int count = gServerInfo.m_GreaterCriticalDamageTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterCriticalDamageTimeConstB);
	int count = 300;
	count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_CRITICAL_DAMAGE_ENHANCED);

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);

		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];

		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];

			if (OBJECT_RANGE(index) == 0)
			{
				continue;
			}

			if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y, 2) == 0)
			{
				continue;
			}

			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);

			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillElectricSpark(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int x = lpObj->X - (8 - (angle & 15));
	int y = lpObj->Y - (8 - ((angle & 240) / 16));

	int nangle = this->GetSkillAngle(lpObj->X, lpObj->Y, x, y);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (gSkillHitBox.HitCheck(nangle, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	this->DecreasePartyMemberHP(lpObj);

	return 1;
}

bool CSkillManager::SkillCastleSiege(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	if (lpSkill->m_skill == SKILL_CRESCENT_MOON_SLASH || lpSkill->m_skill == SKILL_SPIRAL_SLASH || lpSkill->m_skill == SKILL_MANA_RAYS)
	{
		gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 0);
	}
	else if (lpSkill->m_skill == SKILL_MANA_GLAIVE || lpSkill->m_skill == SKILL_FIRE_BLAST)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 50, bIndex, 500, lpSkill->m_index, 0);
	}
	else if (lpSkill->m_skill == SKILL_MANA_GLAIVE || lpSkill->m_skill == SKILL_STAR_FALL)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 50, bIndex, 500, lpSkill->m_index, 0);
	}
	else if (lpSkill->m_skill == SKILL_SPIRAL_SLASH)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 50, bIndex, 500, lpSkill->m_index, 0);
	}
	else if (lpSkill->m_skill == SKILL_CHARGE)
	{
		gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 0);
	}

	return 1;
}

bool CSkillManager::SkillStern(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	int x = lpObj->X - (8 - (angle & 15));
	int y = lpObj->Y - (8 - ((angle & 240) / 16));

	int nangle = this->GetSkillAngle(lpObj->X, lpObj->Y, x, y);

	gObjSetKillCount(aIndex, 0);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		if (gSkillHitBox.HitCheck(nangle, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			if (gObj[index].Teleport == 0)// && gEffectManager.CheckEffect(&gObj[index], EFFECT_IRON_DEFENSE) == 0 && gEffectManager.CheckEffect(&gObj[index], EFFECT_IRON_DEFENSE_IMPROVED) == 0)
			{
				gEffectManager.AddEffect(&gObj[index], 0, EFFECT_STERN, gServerInfo.m_SternTimeConstA, 0, 0, 0, 0);

				gObjSetPosition(index, gObj[index].X, gObj[index].Y);

				this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
			}
		}
	}

	return 1;
}

bool CSkillManager::SkillRemoveStern(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.DelEffect(&gObj[index], EFFECT_STERN);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillGreaterMana(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex, 0);

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), gServerInfo.m_GreaterManaTimeConstA, gServerInfo.m_GreaterManaConstA, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if (lpObj->GuildNumber <= 0 || gObj[index].GuildNumber <= 0)
		{
			continue;
		}

		if (lpObj->GuildNumber != gObj[index].GuildNumber)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), gServerInfo.m_GreaterManaTimeConstA, gServerInfo.m_GreaterManaConstA, 0, 0, 0);

			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillInvisibility(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gObjSetKillCount(aIndex, 0);

	gEffectManager.AddEffect(lpTarget, 0, EFFECT_INVISIBILITY, gServerInfo.m_InvisibilityTimeConstA, 0, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	return 1;
}

bool CSkillManager::SkillRemoveInvisibility(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.DelEffect(&gObj[index], EFFECT_INVISIBILITY);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillRemoveAllEffect(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.ClearAllEffect(&gObj[index]);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillBrand(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex, 0);

	gEffectManager.AddEffect(lpObj, 0, EFFECT_GUILD_STATE5, gServerInfo.m_BrandTimeConstA, 0, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.AddEffect(&gObj[index], 0, EFFECT_GUILD_STATE5, gServerInfo.m_BrandTimeConstA, 0, 0, 0, 0);

			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillPlasmaStorm(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		//gObjAddAttackProcMsgSendDelay(lpObj, 53, index, 300, lpSkill->m_index, 0);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillInfinityArrow(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_INFINITY_ARROW_IMPROVED);

	int count = gServerInfo.m_InfinityArrowTimeConstA;

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}

bool CSkillManager::SkillMonsterSummon(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if ((GetLargeRand() % 10) == 0)
	{
		return 0;
	}

	int MonsterClass = 0;

	switch (lpObj->Class)
	{
	case 161:
		MonsterClass = 147 + GetLargeRand() % 2;
		break;
	case 181:
		MonsterClass = 177 + GetLargeRand() % 2;
		break;
	case 189:
		MonsterClass = 185 + GetLargeRand() % 2;
		break;
	case 197:
		MonsterClass = 193 + GetLargeRand() % 2;
		break;
	case 267:
		MonsterClass = 263 + GetLargeRand() % 2;
		break;
	case 275:
		MonsterClass = 271 + GetLargeRand() % 2;
		break;
#if(GAMESERVER_UPDATE>=901)
	case 673:
		MonsterClass = 670 + GetLargeRand() % 2;
		break;
#endif
	default:
		return 0;
	}

	bool success = 0;

	int x = lpObj->X;
	int y = lpObj->Y;

	if (gObjGetRandomFreeLocation(lpObj->Map, &x, &y, 2, 2, 10) != 0)
	{
		int index = gObjAddMonster(lpObj->Map);

		if (OBJECT_RANGE(index) != 0)
		{
			LPOBJ lpSummon = &gObj[index];

			lpSummon->PosNum = -1;
			lpSummon->X = x;
			lpSummon->Y = y;
			lpSummon->TX = x;
			lpSummon->TY = y;
			lpSummon->OldX = x;
			lpSummon->OldY = y;
			lpSummon->StartX = x;
			lpSummon->StartY = y;
			lpSummon->Dir = GetLargeRand() % 8;
			lpSummon->Map = lpObj->Map;

			gObjSetMonster(index, MonsterClass);

			lpSummon->Attribute = 60;
			lpSummon->MaxRegenTime = 0;

			success = 1;
		}
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, ((y * 256) + x), success);
	return success;
}

bool CSkillManager::SkillMagicDamageImmunity(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	gEffectManager.AddEffect(lpObj, 0, EFFECT_MAGIC_DAMAGE_IMMUNITY, gServerInfo.m_MagicDamageImmunityTimeConstA, 0, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, lpObj->Index, 1);

	return 1;
}

bool CSkillManager::SkillPhysiDamageImmunity(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	gEffectManager.AddEffect(lpObj, 0, EFFECT_PHYSI_DAMAGE_IMMUNITY, gServerInfo.m_PhysiDamageImmunityTimeConstA, 0, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, lpObj->Index, 1);

	return 1;
}

bool CSkillManager::SkillOrderOfProtection(int aIndex, int bIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	gEffectManager.AddEffect(lpObj, 0, EFFECT_ORDER_OF_PROTECTION, 15, SKILL_ORDER_OF_PROTECTION, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillOrderOfRestraint(int aIndex, int bIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(SKILL_ORDER_OF_RESTRAINT, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	gEffectManager.AddEffect(&gObj[bIndex], 0, EFFECT_ORDER_OF_RESTRAINT, 15, SKILL_ORDER_OF_RESTRAINT, 0, 0, 0);

	gObjSetPosition(bIndex, gObj[bIndex].X, gObj[bIndex].Y);

	return 1;
}

bool CSkillManager::SkillOrderOfTracking(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gEffectManager.CheckStunEffect(lpObj) != 0)
	{
		return 0;
	}

	//gObjTeleportMagicUse(aIndex, sx, sy);
	return 1;
}

bool CSkillManager::SkillOrderOfWeaken(int aIndex, int bIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(SKILL_ORDER_OF_WEAKEN, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	int ShieldDamage = (gObj[bIndex].Shield * 50) / 100;

	if (gObj[bIndex].Shield < ShieldDamage)
	{
		ShieldDamage = gObj[bIndex].Shield;
		gObj[bIndex].Shield = 0;
	}
	else
	{
		ShieldDamage = ShieldDamage;
		gObj[bIndex].Shield -= ShieldDamage;
	}

	GCDamageSend(aIndex, bIndex, 0, 0, 0, ShieldDamage);

	return 1;
}

bool CSkillManager::SkillChainLightning(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, 0);

	if (lpTarget->Type == OBJECT_USER)
	{
		BYTE send[256];

		PMSG_SKILL_CHAIN_SKILL_SEND pMsg;
		pMsg.header.set(0xBF, 0x0A, 0);
		int size = sizeof(pMsg);
		pMsg.skill[0] = SET_NUMBERHB(lpSkill->m_index);
		pMsg.skill[1] = SET_NUMBERLB(lpSkill->m_index);
		pMsg.index = aIndex;
		pMsg.count = 0;

		PMSG_SKILL_CHAIN_SKILL info;
		info.index = bIndex;
		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
		pMsg.header.size = size;
		memcpy(send, &pMsg, sizeof(pMsg));
		MsgSendV2(lpObj, send, size);

		return 1;
	}
	
	int count = 1;

	int target[10] = { bIndex,0,0,0,0,0,0,0,0 };

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (bIndex == index)
		{
			continue;
		}

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		target[count++] = index;

		if (count >= _countof(target))
		{
			break;
		}
	}	
	
	BYTE send[256];

	PMSG_SKILL_CHAIN_SKILL_SEND pMsg;
	pMsg.header.set(0xBF, 0x0A, 0);
	int size = sizeof(pMsg);
	pMsg.skill[0] = SET_NUMBERHB(lpSkill->m_index);
	pMsg.skill[1] = SET_NUMBERLB(lpSkill->m_index);
	pMsg.index = aIndex;
	pMsg.count = 0;

	PMSG_SKILL_CHAIN_SKILL info;

	for (int n = 0; n < _countof(target); n++)
	{
		if (n > 0)
		{
			gObjAddAttackProcMsgSendDelay(lpObj, 52, target[n], (50 + (n * 50)), lpSkill->m_index, (n + 1));
		}

		info.index = target[n];

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size = size;

	memcpy(send, &pMsg, sizeof(pMsg));

	if (count > 1)
	{
		DataSend(aIndex, send, size);
		return 1;
	}

	MsgSendV2(lpObj, send, size);

	return 1;
}

bool CSkillManager::SkillDamageReflect(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = gServerInfo.m_ReflectDamageConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ReflectDamageConstB);

	value = ((value > gServerInfo.m_ReflectDamageMaxRate) ? gServerInfo.m_ReflectDamageMaxRate : value);

	value = (value * ((lpTarget->Type == OBJECT_USER) ? gServerInfo.m_ReflectDamageRate[lpTarget->Class] : 100)) / 100;

	int count = gServerInfo.m_ReflectDamageTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ReflectDamageTimeConstB);

	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillSwordPower(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value1 = (lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstA;

	value1 = ((value1 > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value1);

	int value2 = (lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstA;

	value2 = ((value2 > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value2);

	int value3 = (((40 - ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstB)) < 10) ? 10 : (40 - ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstB)));
	int value4 = (((40 - ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstB)) < 10) ? 10 : (40 - ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstB)));

	int count = gServerInfo.m_SwordPowerTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerTimeConstB);

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, value4);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillSwordPowerGetDefense(int aIndex, int* defense) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}

	int value = lpObj->EffectOption.AddSwordPowerDefenseRate;

	(*defense) -= (((lpObj->Dexterity + lpObj->AddDexterity) / gServerInfo.m_ClassDefenseConst[CLASS_SU]) * value) / 100;

	return 1;
}

bool CSkillManager::SkillSwordPowerGetPhysiDamage(int aIndex, int* DamageMin, int* DamageMax) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}

	int value = lpObj->EffectOption.AddSwordPowerDamageRate;

	value = ((value > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value);

	(*DamageMin) += ((((lpObj->Strength + lpObj->AddStrength) + (lpObj->Dexterity + lpObj->AddDexterity)) / gServerInfo.m_SUPhysiDamageMinConstA) * value) / 100;

	(*DamageMax) += ((((lpObj->Strength + lpObj->AddStrength) + (lpObj->Dexterity + lpObj->AddDexterity)) / gServerInfo.m_SUPhysiDamageMaxConstA) * value) / 100;

	(*DamageMin) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);
	(*DamageMax) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);

	return 1;
}

bool CSkillManager::SkillSwordPowerGetMagicDamage(int aIndex, int* DamageMin, int* DamageMax) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}

	int value = lpObj->EffectOption.AddSwordPowerDamageRate;

	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_ENHANCED);

	value = ((value > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value);

	(*DamageMin) += (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SUMagicDamageMinConstA) * value) / 100;
	(*DamageMax) += (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SUMagicDamageMaxConstA) * value) / 100;

	(*DamageMin) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);
	(*DamageMax) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);

	return 1;
}

bool CSkillManager::SkillSwordPowerGetCurseDamage(int aIndex, int* DamageMin, int* DamageMax) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}

	int value = lpObj->EffectOption.AddSwordPowerDamageRate;

#if(GAMESERVER_UPDATE>=602)

	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_IMPROVED);

#endif

	value = ((value > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value);

	(*DamageMin) += (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SUMagicDamageMinConstA) * value) / 100;

	(*DamageMax) += (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SUMagicDamageMaxConstA) * value) / 100;

#if(GAMESERVER_UPDATE>=602)

	(*DamageMin) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);

	(*DamageMax) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);

#endif

	return 1;
}

bool CSkillManager::SkillSleep(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type == OBJECT_USER && lpTarget->Authority == 32)
	{
		return 0;
	}

	if (lpTarget->Type == OBJECT_MONSTER)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	if (lpTarget->Type == OBJECT_USER && this->CheckSkillTarget(lpObj, bIndex, -1, lpTarget->Type) == 0)
	{
		return 0;
	}

	int rate = gServerInfo.m_SleepConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SleepConstB);
	rate += lpObj->Inventory[1].GetBookSuccessRate();
	rate += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SLEEP_IMPROVED);
	
	int count = gServerInfo.m_SleepTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SleepTimeConstB);
	count = ((count > gServerInfo.m_SleepMaxTime) ? gServerInfo.m_SleepMaxTime : count);
	count = ((count < 1) ? 1 : count);

	if ((GetLargeRand() % 100) >= rate)
	{
		gAttack.MissSend(lpObj, lpTarget, lpSkill, 1, 0);
		return 0;
	}

	/*if (gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE) != 0 || gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE_IMPROVED) != 0)
	{
		gAttack.MissSend(lpObj, lpTarget, lpSkill, 1, 0);
		return 0;
	}*/

	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, 0, 0, 0, 0);
	gObjSetPosition(bIndex, lpTarget->X, lpTarget->Y);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}

bool CSkillManager::SkillLesserDefense(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int rate = gServerInfo.m_LesserDefenseConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDefenseConstB);
	//rate += lpObj->Inventory[1].GetBookSuccessRate();

	/*int value = gServerInfo.m_LesserDefenseConstC + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDefenseConstD);
	value = ((value > gServerInfo.m_LesserDefenseMaxRate) ? gServerInfo.m_LesserDefenseMaxRate : value);*/

	int count = gServerInfo.m_LesserDefenseTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDefenseTimeConstB);
	count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LESSER_DEFENSE_IMPROVED);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		if (gMap[lpObj->Map].CheckAttr(gObj[index].X, gObj[index].Y, 1) != 0)
		{
			continue;
		}

		if ((GetLargeRand() % 100) >= rate)
		{
			gAttack.MissSend(lpObj, &gObj[index], lpSkill, 1, 0);
		}
		else
		{
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, gServerInfo.m_LesserDefenseMaxRate, 0, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillLesserDamage(int aIndex, int bIndex, CSkill* lpSkill) // Innovation
{
	LPOBJ lpObj = &gObj[aIndex];

	int rate = gServerInfo.m_LesserDamageConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDamageConstB);
	//rate += lpObj->Inventory[1].GetBookSuccessRate();

	/*int value = gServerInfo.m_LesserDamageConstC + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDamageConstD);
	value = ((value > gServerInfo.m_LesserDamageMaxRate) ? gServerInfo.m_LesserDamageMaxRate : value);*/

	int count = gServerInfo.m_LesserDamageTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDamageTimeConstB);
	count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LESSER_DAMAGE_IMPROVED);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		if (gMap[lpObj->Map].CheckAttr(gObj[index].X, gObj[index].Y, 1) != 0)
		{
			continue;
		}

		if ((GetLargeRand() % 100) >= rate)
		{
			gAttack.MissSend(lpObj, &gObj[index], lpSkill, 1, 0);
		}
		else
		{
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, gServerInfo.m_LesserDamageMaxRate, 0, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillSahamutt(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1000, lpSkill->m_index, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillNeil(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1000, lpSkill->m_index, 0);
		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillGhostPhantom(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}

	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 1000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 2000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 3000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 4000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 5000, lpSkill->m_index, MAKE_NUMBERDW(x, y));

	return 1;
}

bool CSkillManager::SkillGhostPhantomAreaAttack(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillRedStorm(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		//gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 250, lpSkill->m_index, 0);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillFrozenStab(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		//gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 500, lpSkill->m_index, combo);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillMagicCircle(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value1 = (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DWMagicDamageMinConstA) * gServerInfo.m_MagicCircleConstA) / 100.0f;

	int value2 = (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DWMagicDamageMaxConstA) * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MAGIC_CIRCLE_IMPROVED)) / 100.0f;

	int value3 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MAGIC_CIRCLE_ENHANCED);

	int count = gServerInfo.m_MagicCircleTimeConstA;

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}

bool CSkillManager::SkillShieldRecover(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = ((lpSkill->m_level + 1) * gServerInfo.m_ShieldRecoverConstA) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ShieldRecoverConstB);

	if ((lpTarget->Shield + value) > (lpTarget->MaxShield + lpTarget->m_MPSkillOpt.AddShield))
	{
		lpTarget->Shield = lpTarget->MaxShield + lpTarget->m_MPSkillOpt.AddShield;
	}
	else
	{
		lpTarget->Shield += value;
	}

	GCLifeSend(bIndex, 0xFF, (int)lpTarget->Life, lpTarget->Shield);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index, (value / 40));
		}
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}

bool CSkillManager::SkillFiveShot(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) > 0)
	{
		LPOBJ lpTarget = &gObj[bIndex];

		if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
		{
			return 0;
		}

		if (lpTarget->isBoss)
		{
			gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 0, 0);
			return 1;
		}
	}

	int SkillFrustrumX[4], SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 6.0f, 7.0f, 2.0f, 0.0f);

	vec3_t Angle;

	Vector(0.0f, 6.0f, 0.0f, Angle);

	vec3_t p[4];

	Vector(0.0f, 0.0f, 40.0f, p[0]);
	Vector(0.0f, 0.0f, 20.0f, p[1]);
	Vector(0.0f, 0.0f, 340.0f, p[2]);
	Vector(0.0f, 0.0f, 320.0f, p[3]);

	float Matrix[3][4];

	vec3_t vFrustrum[5], vFrustrum2[5];

	AngleMatrix(p[0], Matrix);
	VectorRotate(Angle, Matrix, vFrustrum[0]);

	AngleMatrix(p[1], Matrix);
	VectorRotate(Angle, Matrix, vFrustrum[1]);

	Vector(0.0f, 6.0f, 0.0f, vFrustrum[2]);

	AngleMatrix(p[2], Matrix);
	VectorRotate(Angle, Matrix, vFrustrum[3]);

	AngleMatrix(p[3], Matrix);
	VectorRotate(Angle, Matrix, vFrustrum[4]);

	Vector(0.0f, 0.0f, (vec_t)((angle * 360) / 255), p[0]);

	AngleMatrix((float*)p, Matrix);

	int ArrowFrustrum[5], ArrowFrustrumX[5], ArrowFrustrumY[5];

	for (int n = 0; n < 5; n++)
	{
		VectorRotate(vFrustrum[n], Matrix, vFrustrum2[n]);
		ArrowFrustrumX[n] = (int)vFrustrum2[n][0] + lpObj->X;
		ArrowFrustrumY[n] = (int)vFrustrum2[n][1] + lpObj->Y;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, 0);
		/*
		for (int i = 0; i < 5; i++)
		{
			if ((ArrowFrustrum[i] = ((ArrowFrustrumX[i] - lpObj->X) * (gObj[index].Y - lpObj->Y)) - ((ArrowFrustrumY[i] - lpObj->Y) * (gObj[index].X - lpObj->X))) > -5 && ArrowFrustrum[i] < 5)
			{
				gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, 0);
			}
		}*/

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillTripleShot(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillSwordSlash(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int SkillFrustrumX[4], SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 2.0f, 4.0f, 5.0f, 0.0f);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, 0);

		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillLightningStorm(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		//gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 400, lpSkill->m_index, 0);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillBirds(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int SkillFrustrumX[4], SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 1.5f, 6.0f, 1.5f, 0.0f);

	int count = 0;

	int addRange = 0;

	if (lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 4) && lpObj->Inventory[lpObj->m_btInvenPetPos].m_JewelOfHarmonyOption == 1)
	{
		addRange = 2;
	}

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y, addRange) == 0)
		{
			continue;
		}

		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		/*if (OBJECT_RANGE(bIndex) != 0 && index == bIndex)
		{
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 200, lpSkill->m_index, 0);
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 300, lpSkill->m_index, 0);
		}
		else
		{
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 200, lpSkill->m_index, 0);
		}*/

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillSelfExplosion(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	lpObj->Life = 0;

	gObjectManager.CharacterLifeCheck(lpObj, lpObj, 0, 1, 0, 0, 0, 0);

	return 1;
}

bool CSkillManager::SkillKillingBlow(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 1, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 4, 0);

	return 1;
}

bool CSkillManager::SkillUpperBeast(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 1, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, 0);

	return 1;
}

bool CSkillManager::SkillChainDriver(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 1, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, 0);
	//gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 4, 0);

	/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE5, 0))
	{
		gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 4, 0);
		gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 5, 0);
	}*/

	return 1;
}

bool CSkillManager::SkillDarkSide(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	PMSG_SKILL_DARK_SIDE_SEND pMsg;

	pMsg.header.setE(0x4B, sizeof(pMsg));

	pMsg.skill = lpSkill->m_index;

	pMsg.index[0] = 20000;
	pMsg.index[1] = 20000;
	pMsg.index[2] = 20000;
	pMsg.index[3] = 20000;
	pMsg.index[4] = 20000;

	this->SkillDarkSideGetTargetIndex(aIndex, ((bIndex == 0) ? -1 : bIndex), lpSkill, pMsg.index);

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

	this->GCRageFighterSkillAttackSend(lpObj, lpSkill->m_index, lpObj->RageFighterSkillTarget, 1);

	for (int n = 0; n < _countof(pMsg.index); n++)
	{
		if (pMsg.index[n] != 20000)
		{
			//if (gObj[pMsg.index[n]].Type == OBJECT_USER)
			//{
				gAttack.Attack(lpObj, &gObj[pMsg.index[n]], lpSkill, 1, 1, 0, 0, 0);
			//}
			//else
			//{
			//	gObjAddAttackProcMsgSendDelay(lpObj, 54, pMsg.index[n], (10 + (n * 10)), lpSkill->m_index, 1);
			//	gObjAddAttackProcMsgSendDelay(lpObj, 52, pMsg.index[n], (10 + (n * 20)), lpSkill->m_index, 2);
			//}
		}
	}

	return 1;
}

bool CSkillManager::SkillDarkSideGetTargetIndex(int aIndex, int bIndex, CSkill* lpSkill, WORD* target) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	if (OBJECT_RANGE(bIndex) != 0 && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) != 0)
	{
		target[count++] = bIndex;
	}

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (index == bIndex) //avoid same target to be targeted again by being at viewport
		{
			continue;
		}

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		target[count] = index;

		if ((++count) >= 5)
		{
			break;
		}
	}

	lpObj->RageFighterSkillTarget = target[0];

	return 1;
}

bool CSkillManager::SkillDragonLore(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 1, 0);
		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 2, 0);
		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 3, 0);
		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 4, 0);

		/*if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE2, 2))
		{
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 5, 0);
			gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 6, 0);
		}*/

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillGreaterIgnoreDefenseRate(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value = gServerInfo.m_GreaterIgnoreDefenseRateConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterIgnoreDefenseRateConstB);

	value = ((value > gServerInfo.m_GreaterIgnoreDefenseMax) ? gServerInfo.m_GreaterIgnoreDefenseMax : value);

	int count = gServerInfo.m_GreaterIgnoreDefenseRateTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterIgnoreDefenseRateTimeConstB);

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	return 1;
}

bool CSkillManager::SkillFitness(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	int value = gServerInfo.m_FitnessConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_FitnessConstB);

	value = ((value > gServerInfo.m_FitnessMax) ? gServerInfo.m_FitnessMax : value);

	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_STAMINA_INCREASE_STRENGTHENER);

	int count = gServerInfo.m_FitnessTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_FitnessTimeConstB);

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];

		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];

			if (OBJECT_RANGE(index) == 0)
			{
				continue;
			}

			if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
			{
				continue;
			}

			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillGreaterDefenseSuccessRate(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	int value1 = gServerInfo.m_GreaterDefenseSuccessRateConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterDefenseSuccessRateConstB);

	value1 = ((value1 > gServerInfo.m_GreaterIgnoreDefenseMax) ? gServerInfo.m_GreaterIgnoreDefenseMax : value1);

	value1 += lpObj->m_MPSkillOpt.DefenseSuccessRate * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_SUCCESS_RATE_IMPROVED) / 100;

	int value2 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_SUCCESS_RATE_ENHANCED);

	int count = gServerInfo.m_GreaterDefenseSuccessRateTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterDefenseSuccessRateTimeConstB);

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, 0, 0);
		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];

		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];

			if (OBJECT_RANGE(index) == 0)
			{
				continue;
			}

			if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
			{
				continue;
			}

			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillPhoenixShot(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 1, 0);
		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 2, 0);
		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 3, 0);
		//gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 4, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillBloodStorm(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		gObjAddAttackProcMsgSendDelay(lpObj, 53, index, 1000, lpSkill->m_index, combo);
		//gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1500, lpSkill->m_index, 0);

		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillCure(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gEffectManager.ClearDebuffEffect(lpTarget, 1);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillPartyHeal(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		return 0;
	}

	if (CA_MAP_RANGE(lpObj->Map) != 0)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0)
	{
		return 0;
	}

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER)
	{
		return 0;
	}

	if (OBJECT_RANGE(lpTarget->PartyNumber) == 0)
	{
		return 0;
	}

	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int count = 1;

	int target[3] = { bIndex,-1,-1 };

	PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];

	for (int n = 0; n < MAX_PARTY_USER; n++)
	{
		int index = lpParty->Index[n];

		if (OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		if (bIndex == index)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		target[count++] = index;

		if (count >= _countof(target))
		{
			break;
		}
	}

	int value = gServerInfo.m_PartyHealConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_PartyHealConstB);

	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_PARTY_HEAL_IMPROVED)) / 100;

	BYTE send[256];

	PMSG_SKILL_CHAIN_SKILL_SEND pMsg;

	pMsg.header.set(0xBF, 0x18, 0);

	int size = sizeof(pMsg);

	pMsg.skill[0] = SET_NUMBERHB(lpSkill->m_index);

	pMsg.skill[1] = SET_NUMBERLB(lpSkill->m_index);

	pMsg.index = aIndex;

	pMsg.count = 0;

	PMSG_SKILL_CHAIN_SKILL info;

	for (int n = 0; n < count; n++)
	{
		gObjAddMsgSendDelay(&gObj[target[n]], 16, target[n], (200 + (n * 200)), ((value * (100 - (n * 20))) / 100));

		info.index = target[n];

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size = size;

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(aIndex, send, size);

	MsgSendV2(lpObj, send, size);

	return 1;
}

bool CSkillManager::SkillPoisonArrow(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillBless(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return 0;
	}

	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = (lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_BlessConstA;

	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLESS_IMPROVED);

	value += lpObj->ElfBlessReinforcement;

	int count = gServerInfo.m_BlessTimeConstA;

	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	gFruit.GCFruitResultSend(lpTarget, 17, value, 7);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillBlind(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (lpTarget->Type == OBJECT_USER && lpTarget->Authority == 32)
	{
		return 0;
	}

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	if (lpTarget->Type == OBJECT_USER && this->CheckSkillTarget(lpObj, bIndex, -1, lpTarget->Type) == 0)
	{
		return 0;
	}

	int rate = gServerInfo.m_BlindConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_BlindConstB);

	if ((GetLargeRand() % 100) >= rate)
	{
		gAttack.MissSend(lpObj, lpTarget, lpSkill, 1, 0);
		return 0;
	}

	/*if (gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE) != 0 || gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE_IMPROVED) != 0)
	{
		gAttack.MissSend(lpObj, lpTarget, lpSkill, 1, 0);
		return 0;
	}*/

	if (lpSkill->m_index == MASTER_SKILL_ADD_BLIND_IMPROVED && (GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLIND_IMPROVED))
	{
		gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), 2, 0, 0, 0, 0);
	}
	else
	{
		gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), gServerInfo.m_BlindTimeConstA, gServerInfo.m_BlindConstC, 0, 0, 0);
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	return 1;
}

bool CSkillManager::SkillEarthPrison(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);
		gObjAddAttackProcMsgSendDelay(lpObj, 53, index, 1200, lpSkill->m_index, 0);

		//gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1500, lpSkill->m_index, 0);

		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillIronDefense(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	return 0;

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value1 = gServerInfo.m_IronDefenseConstA;

	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, (MASTER_SKILL_ADD_IRON_DEFENSE_IMPROVED || MASTER_SKILL_ADD_DK_IRON_DEFENSE_POW_IP));

	int value2 = gServerInfo.m_IronDefenseConstB;

	value2 += gMasterSkillTree.GetMasterSkillValue(lpObj, (MASTER_SKILL_ADD_IRON_DEFENSE_IMPROVED || MASTER_SKILL_ADD_DK_IRON_DEFENSE_POW_IP));

	int count = gServerInfo.m_IronDefenseTimeConstA;

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	return 1;
}

bool CSkillManager::SkillBloodHowling(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int count = gServerInfo.m_BloodHowlingTimeConstA;

	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, 0, 0, 0, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	return 1;
}

void CSkillManager::ApplyMeteoriteEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_METEORITE_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 3, 0, 0, 0, 0);
	}
}

void CSkillManager::ApplyIceStormEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if (gEffectManager.GetEffect(lpTarget, EFFECT_ICE_STORM_ENHANCED) != 0 || gEffectManager.GetEffect(lpTarget, EFFECT_ICE) != 0)
	{
		return;
	}

	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_STORM_ENHANCED))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_ICE_STORM_ENHANCED, 3, 0, 0, 0, 0);
	}
	else
	{
		if ((GetLargeRand() % 100) < 50)
		{
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 0, 0, 0, 0);
		}
	}
}

void CSkillManager::ApplyRagefulBlowEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2, 2);

	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1, 2);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2, 2);

	float value1 = 0;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1, 2);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2, 2);

	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_PLAGUE, time, lpObj->Index, 1, 2000 + value1, 0);
	}
}

void CSkillManager::ApplyDeathStabEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	/*if( (GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj,MASTER_SKILL_ADD_DEATH_STAB_MASTERED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget,0,EFFECT_STERN,2,0,0,0,0);
	}*/
	/*else if((GetLargeRand()%100) < gMasterSkillTree.GetMasterSkillValue(lpObj,MASTER_SKILL_ADD_DEATH_STAB_ENHANCED))
	{
		damage = ((lpObj->Strength+lpObj->AddStrength)*10)/100;
		gEffectManager.AddEffect(lpTarget,0,EFFECT_BLEEDING,10,lpObj->Index,1,SET_NUMBERHW(damage),SET_NUMBERLW(damage));
	}*/

	float Rand = 0;
	float Value = 0;
	float time = 0;

	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1, 0);
	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2, 0);

	if (Rand > 0)
	{
		Value = 30;
		time = 10;
	}

	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1, 0);
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2, 0);

	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2, 0);

	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, time, lpObj->Index, 1, Value, 0);
	}
}

void CSkillManager::ApplyFireSlashEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if (gEffectManager.CheckEffect(lpTarget, EFFECT_FIRE_SLASH) != 0 || gEffectManager.CheckEffect(lpTarget, EFFECT_FIRE_SLASH_ENHANCED) != 0)
	{
		return;
	}

	/*int value = gServerInfo.m_FireSlashConstA + ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_FireSlashConstB);
	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_SLASH_ENHANCED);
	value = ((value > gServerInfo.m_FireSlashMaxRate) ? gServerInfo.m_FireSlashMaxRate : value);*/
	int value = 1;

	int count = gServerInfo.m_FireSlashTimeConstA;

	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);

	/*
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2, 2);

	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1, 2);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2, 2);

	float value1 = 0;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1, 2);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2, 2);

	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_PLAGUE, time, lpObj->Index, 1, 2000 + value1, 0);
	}*/
}

void CSkillManager::ApplyFireBurstEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_BURST_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 2, 0, 0, 0, 0);
	}

	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2, 2);

	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1, 2);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2, 2);

	float value1 = 0;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1, 2);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2, 2);

	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_PLAGUE, time, lpObj->Index, 1, 2000 + value1, 0);
	}
}

void CSkillManager::ApplyPlasmaStormEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpTarget->Inventory[(damage = (2 + (GetLargeRand() % 5)))];

		if (lpItem->IsItem() != 0 && lpItem->IsLuckyItem() == 0 && lpItem->m_IsPeriodicItem == 0)
		{
			lpItem->m_Durability = (lpItem->m_Durability * 10) / 100;
			gItemManager.GCItemDurSend(lpTarget->Index, damage, (BYTE)lpItem->m_Durability, 0);
		}
	}
}

void CSkillManager::ApplyFireScreamEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{

}

void CSkillManager::ApplyEarthquakeEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if (GetLargeRand() % 100 < lpTarget->BackSpringProtection)
	{
		return;
	}
	
	int maxDistance = (int) ((lpObj->Leadership + lpObj->AddLeadership) / 13000);
	int distance = gObjCalcDistance(lpObj, lpTarget);
	int howLong = (distance > maxDistance) ? 0 : (maxDistance - distance + 1);

	if (howLong > 0)
	{
		gObjBackSpring2(lpTarget, lpObj, howLong);
	}
}

void CSkillManager::ApplyDrainLifeEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	damage = ((damage * gServerInfo.m_DrainLifeConstA) / 100) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DrainLifeConstB);

	if ((lpObj->Life + damage) > (lpObj->MaxLife + lpObj->AddLife))
	{
		lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
	}
	else
	{
		lpObj->Life += damage;
	}

	GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);

	if ((GetLargeRand() % 100) < 30)
	{
		damage = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAIN_LIFE_ENHANCED);
		if (damage > 0)
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_DRAIN_LIFE_ENHANCED, 5, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
		}
	}
}

void CSkillManager::ApplySahamuttEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	damage = (damage * gServerInfo.m_SahamuttConstA) / 100;

	damage += (damage * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SAHAMUTT_ENHANCED)) / 100;

	gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 5, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
}

void CSkillManager::ApplyNeilEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_NEIL_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 2, 0, 0, 0, 0);
	}

	damage = (damage * gServerInfo.m_NeilConstA) / 100;
	gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 5, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
}

void CSkillManager::ApplyGhostPhantomEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GHOST_PHANTOM_ENHANCED))
	{
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 50, 0);
	}
	else
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 2, 0, 0, 0, 0);
	}
}

void CSkillManager::ApplyFrozenStabEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	/*if((GetLargeRand()%100) < gMasterSkillTree.GetMasterSkillValue(lpObj,MASTER_SKILL_ADD_FROZEN_STAB_MASTERED))
	{
		gEffectManager.AddEffect(lpTarget,0,EFFECT_FROZEN_STAB_MASTERED,5,0,0,0,0);
	}
	else if((GetLargeRand()%100) < gMasterSkillTree.GetMasterSkillValue(lpObj,MASTER_SKILL_ADD_FROZEN_STAB_ENHANCED))
	{
		gEffectManager.AddEffect(lpTarget,0,EFFECT_ICE_ARROW,3,0,0,0,0);
	}
	else
	{*/
	gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 0, 0, 0, 0);
	//}
}

void CSkillManager::ApplyFiveShotEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	/*if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIVE_SHOT_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 3, 0, 0, 0, 0);
	}*/

	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING1, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT1, 1);

	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING2, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT2, 1);

	float time = 3;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION1, 1);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION2, 1);

	if ((GetLargeRand() % 10) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < 10)
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, 10, 0, 0, 0, 0);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 0, 0, 0, 0);
	}
}

void CSkillManager::ApplySwordSlashEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 1, 0, 0, 0, 0);
}

void CSkillManager::ApplyKillingBlowEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < (5 + gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_KILLING_BLOW_MASTERY)))
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 5, 0, 0, 0);
	}
}

void CSkillManager::ApplyBeastUppercutEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < (5 + gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_BEAST_UPPERCUT_MASTERY)))
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 10, 0, 0, 0);
	}
}

void CSkillManager::ApplyChainDriverEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	/*if((GetLargeRand()%100) < gMasterSkillTree.GetMasterSkillValue(lpObj,MASTER_SKILL_ADD_CHAIN_DRIVER_ENHANCED))
	{
		damage = 100;
		gEffectManager.AddEffect(lpTarget,0,EFFECT_FREEZING_DAMAGE,10,lpObj->Index,1,SET_NUMBERHW(damage),SET_NUMBERLW(damage));
	}
	else
	{*/
	gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 0, 0, 0, 0);
	//}

	float Rand = 0;
	float Value = 0;
	float time = 0;

	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1, 0);
	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2, 0);

	if (Rand > 0)
	{
		Value = 30;
		time = 10;
	}

	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1, 0);
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2, 0);

	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2, 0);

	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, time, lpObj->Index, 1, Value, 0);
	}
}

void CSkillManager::ApplyDragonLoreEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	/*if((GetLargeRand()%100) < gMasterSkillTree.GetMasterSkillValue(lpObj,MASTER_SKILL_ADD_DRAGON_LORE_ENHANCED))
	{
		damage = 100;
		gEffectManager.AddEffect(lpTarget,0,EFFECT_DRAGON_ROAR_ENHANCED,10,lpObj->Index,1,SET_NUMBERHW(damage),SET_NUMBERLW(damage));
	}*/

	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2, 2);

	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1, 2);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2, 2);

	float value1 = 0;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1, 2);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2, 2);

	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_PLAGUE, time, lpObj->Index, 1, 2000 + value1, 0);
	}
}

void CSkillManager::ApplyDragonSlayerEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	/*if((GetLargeRand()%100) < (gMasterSkillTree.GetMasterSkillValue(lpObj,MASTER_SKILL_ADD_DRAGON_SLAYER_ENHANCED)-lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget,0,EFFECT_STERN,2,0,0,0,0);
	}*/

	if (lpTarget->Type != OBJECT_USER)
	{
		return;
	}

	int rate = gServerInfo.m_DragonSlayerConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DragonSlayerConstB);

	if ((GetLargeRand() % 100) >= rate)
	{
		return;
	}

	int value = gServerInfo.m_DragonSlayerConstC + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DragonSlayerConstD);

	value = ((value > gServerInfo.m_DragonSlayerMaxRate) ? gServerInfo.m_DragonSlayerMaxRate : value);

	int ShieldDamage = ((lpTarget->MaxShield + lpTarget->m_MPSkillOpt.AddShield) * value) / 100;

	if (lpTarget->Shield < ShieldDamage)
	{
		ShieldDamage = lpTarget->Shield;
		lpTarget->Shield = 0;
	}
	else
	{
		ShieldDamage = ShieldDamage;
		lpTarget->Shield -= ShieldDamage;
	}

	GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, 0, ShieldDamage);
}

void CSkillManager::ApplyPhoenixShotEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < 10)
	{
		int effect = (lpTarget->Type == OBJECT_USER) ? 20 : 50;
		effect += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_PHOENIX_SHOT_MASTERY);
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_DECREASE_DEFENSE_RATE, 10, effect, 0, 0, 0);
	}

	gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 50, 0);
}

void CSkillManager::ApplyEarthPrisonEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < gServerInfo.m_EarthPrisonConstA)
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 5, 0, 0, 0, 0);
	}
}

void CSkillManager::ApplyMagicPinEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	/*if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FK_MAGIC_PIN_PROFICIENCY))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 2, 0, 0, 0, 0);
	}*/

	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING1, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT1, 1);

	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING2, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT2, 1);
	float time = 3;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION1, 1);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION2, 1);

	if ((GetLargeRand() % 10) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < 10)
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, 10, 0, 0, 0, 0);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 0, 0, 0, 0);
	}
}

void CSkillManager::CGMultiSkillAttackRecv(PMSG_MULTI_SKILL_ATTACK_RECV* lpMsg, int aIndex, bool type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Live == 0 || lpObj->Teleport != 0)
	{
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
	{
		return;
	}

	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0)
	{
		return;
	}

	int skill = MAKE_NUMBERW(lpMsg->skillH, lpMsg->skillL);

	CSkill* lpSkill = 0;

	if (lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj, skill);

		if (lpSkill == 0)
		{
			return;
		}

		if (this->CheckSkillDelay(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		if (this->CheckSkillRequireClass(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];

		if (lpSkill->IsSkill() == 0)
		{
			return;
		}
	}

	if (lpSkill->m_skill != SKILL_FLAME && lpSkill->m_skill != SKILL_TWISTER && lpSkill->m_skill != SKILL_EVIL_SPIRIT && lpSkill->m_skill != SKILL_HELL_FIRE && lpSkill->m_skill != SKILL_AQUA_BEAM && lpSkill->m_skill != SKILL_BLAST && lpSkill->m_skill != SKILL_INFERNO && lpSkill->m_skill != SKILL_IMPALE && lpSkill->m_skill != SKILL_MONSTER_AREA_ATTACK && lpSkill->m_skill != SKILL_PENETRATION && lpSkill->m_skill != SKILL_FIRE_SCREAM)
	{
		return;
	}

	if (lpObj->Type == OBJECT_USER && (lpObj->MultiSkillIndex == 0 || lpObj->MultiSkillIndex != lpSkill->m_index || (lpObj->MultiSkillCount++) >= 5))
	{
		return;
	}

	/*if (gCheatGuard.CheckSpeedHack(aIndex, 0, lpSkill->m_skill, "CGMultiSkillAttackRecv") == 0)
	{
		return;
	}*/

	lpMsg->count = ((lpMsg->count > 5) ? 5 : lpMsg->count);

	for (int n = 0; n < lpMsg->count; n++)
	{
		PMSG_MULTI_SKILL_ATTACK* lpInfo = (PMSG_MULTI_SKILL_ATTACK*)(((BYTE*)lpMsg) + sizeof(PMSG_MULTI_SKILL_ATTACK_RECV) + (sizeof(PMSG_MULTI_SKILL_ATTACK) * n));

		int bIndex = MAKE_NUMBERW(lpInfo->indexH, lpInfo->indexL);

		if (OBJECT_RANGE(bIndex) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[bIndex];

		if (lpObj->Type == OBJECT_USER && this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
		{
			continue;
		}

		if (type != 0)
		{
			gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 1, 0, 0, 0);
		}
		/*else if (lpSkill->m_skill == SKILL_FIRE_SLASH)
		{
			gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 1, 0, 0, 0);
		}*/
		else
		{
			gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 1, 0, 0, 0);

			if (lpSkill->m_skill == SKILL_EVIL_SPIRIT && lpObj->Type == OBJECT_USER)
			{
				if (gMasterSkillTree_4th.CheckForMasterSkill(lpObj, FOR_MASTER_SKILL_ADD_INCREASE_THE_NUMBER_OF_DAMAGE1, ((lpObj->Class == CLASS_DW) ? 1 : 5)))
				{
					gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 1, 0, 0, 0);
				}
			}
		}
	}
}

void CSkillManager::CGSkillAttackRecv(PMSG_SKILL_ATTACK_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Live == 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->indexH, lpMsg->indexL);

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (gObjIsConnectedGS(bIndex) == 0)
	{
		return;
	}

	if (lpTarget->Live == 0)
	{
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0 || gMap[lpTarget->Map].CheckAttr(lpTarget->X, lpTarget->Y, 1) != 0)
	{
		return;
	}

	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0 || gDuel.GetDuelArenaBySpectator(bIndex) != 0)
	{
		return;
	}

	int skill = MAKE_NUMBERW(lpMsg->skillH, lpMsg->skillL);

	CSkill* lpSkill = 0;

	if (lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj, skill);

		if (lpSkill == 0)
		{
			return;
		}

		if (this->CheckSkillDelay(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		if (this->CheckSkillRequireKillPoint(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		if (this->CheckSkillRequireGuildStatus(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		if (this->CheckSkillRequireClass(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];

		if (lpSkill->IsSkill() == 0)
		{
			return;
		}
	}

	/*if (gCheatGuard.CheckSpeedHack(aIndex, bIndex, lpSkill->m_index, "CGSkillAttack") == 0)
	{
		return;
	}*/

	lpObj->MultiSkillIndex = 0;
	lpObj->MultiSkillCount = 0;

	this->UseAttackSkill(aIndex, ((skill == SKILL_NOVA_START || skill == MASTER_SKILL_ADD_NOVA_START_IMPROVED) ? skill : bIndex), lpSkill);
}

void CSkillManager::CGDurationSkillAttackRecv(PMSG_DURATION_SKILL_ATTACK_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Live == 0)
	{
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
	{
		return;
	}

	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0)
	{
		return;
	}

	int skill = MAKE_NUMBERW(lpMsg->skillH, lpMsg->skillL);

	CSkill* lpSkill = 0;

	if (lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj, skill);

		if (lpSkill == 0)
		{
			return;
		}

		if (this->CheckSkillDelay(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		if (this->CheckSkillRequireKillPoint(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		if (this->CheckSkillRequireGuildStatus(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		if (this->CheckSkillRequireClass(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];

		if (lpSkill->IsSkill() == 0)
		{
			return;
		}
	}

	lpObj->MultiSkillIndex = 0;
	lpObj->MultiSkillCount = 0;

	int bIndex = MAKE_NUMBERW(lpMsg->indexH, lpMsg->indexL);

	/*if (gCheatGuard.CheckSpeedHack(aIndex, bIndex, lpSkill->m_index, "CGDurationSkillAttackRecv") == 0)
	{
		return;
	}*/

	this->UseDurationSkillAttack(lpObj->Index, bIndex, lpSkill, lpMsg->x, lpMsg->y, lpMsg->dir, lpMsg->angle);
}

void CSkillManager::CGSkillCancelRecv(PMSG_SKILL_CANCEL_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	gEffectManager.DelEffect(lpObj, this->GetSkillEffect(MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1])));
}

void CSkillManager::CGRageFighterSkillAttackRecv(PMSG_RAGE_FIGHTER_SKILL_ATTACK_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Live == 0 || lpObj->Teleport != 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->indexH, lpMsg->indexL);

	CSkill* lpSkill = this->GetSkill(lpObj, MAKE_NUMBERW(lpMsg->skillH, lpMsg->skillL));

	if (lpSkill == 0)
	{
		return;
	}

	if (lpSkill->m_skill != SKILL_UPPER_BEAST && lpSkill->m_skill != SKILL_CHAIN_DRIVER && lpSkill->m_skill != SKILL_DARK_SIDE)
	{
		return;
	}

	/*if (gCheatGuard.CheckSpeedHack(aIndex, bIndex, lpSkill->m_index, "CGRageFighterSkillAttackRecv") == 0)
	{
		return;
	}*/

	lpObj->RageFighterSkillIndex = lpSkill->m_index;

	lpObj->RageFighterSkillCount = 0;

	if (lpSkill->m_skill == SKILL_DARK_SIDE)
	{
		this->GCRageFighterSkillAttackSend(lpObj, lpSkill->m_index, lpObj->RageFighterSkillTarget, 1);
	}
	else if (OBJECT_RANGE(bIndex) != 0)
	{
		this->GCRageFighterSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	}
}

void CSkillManager::CGSkillDarkSideRecv(PMSG_SKILL_DARK_SIDE_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if (lpObj->Live == 0 || lpObj->Teleport != 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);

	CSkill* lpSkill = this->GetSkill(lpObj, MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]));

	if (lpSkill == 0)
	{
		return;
	}

	if (lpSkill->m_skill != SKILL_DARK_SIDE)
	{
		return;
	}

	PMSG_SKILL_DARK_SIDE_SEND pMsg;

	pMsg.header.setE(0x4B, sizeof(pMsg));

	pMsg.skill = lpSkill->m_index;

	pMsg.index[0] = 20000;
	pMsg.index[1] = 20000;
	pMsg.index[2] = 20000;
	pMsg.index[3] = 20000;
	pMsg.index[4] = 20000;

	this->SkillDarkSideGetTargetIndex(aIndex, ((bIndex == 0) ? -1 : bIndex), lpSkill, pMsg.index);

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CSkillManager::CGSkillTeleportAllyRecv(PMSG_SKILL_TELEPORT_ALLY_RECV* lpMsg, int aIndex) // OK
{
	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (OBJECT_RANGE(lpMsg->index) == 0)
	{
		return;
	}

	if (gObj[lpMsg->index].Connected != OBJECT_ONLINE || gObj[lpMsg->index].CloseCount > 0)
	{
		lpMsg->index = aIndex;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (gSkillManager.CheckSkillBlock(lpObj, SKILL_TELEPORT_ALLY))
	{
		return;
	}

	LPOBJ lpTarget = &gObj[lpMsg->index];

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0 || OBJECT_RANGE(lpTarget->PartyNumber) == 0)
	{
		return;
	}

	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return;
	}

	if (gEffectManager.CheckStunEffect(lpObj) != 0)
	{
		return;
	}

	if (lpObj->Map != lpTarget->Map)
	{
		return;
	}

	if (gObjCheckTeleportArea(lpMsg->index, lpMsg->x, lpMsg->y) == 0)
	{
		return;
	}

#if(GAMESERVER_TYPE==1)

	if (lpObj->Map == MAP_CASTLE_SIEGE && gCastleSiege.CheckTeleportMagicAxisY(lpObj->Y, lpMsg->x, lpMsg->y) == 0)
	{
		lpMsg->y = (BYTE)lpObj->Y;
	}

#endif

	CSkill* lpSkill = gSkillManager.GetSkill(lpObj, SKILL_TELEPORT_ALLY);

	if (lpSkill != 0)
	{
		if (IT_MAP_RANGE(lpObj->Map) != 0 && gIllusionTemple.GetState() != IT_STATE_START)
		{
			lpMsg->x = (BYTE)lpObj->X;
			lpMsg->y = (BYTE)lpObj->Y;
		}

		if (DG_MAP_RANGE(lpObj->Map) != 0 && gDoubleGoer.GetState() != DG_STATE_START)
		{
			lpMsg->x = (BYTE)lpObj->X;
			lpMsg->y = (BYTE)lpObj->Y;
		}

		if (gSkillManager.CheckSkillMana(lpObj, lpSkill->m_index) == 0 || gSkillManager.CheckSkillBP(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		gSkillManager.GCSkillAttackSend(lpObj, lpSkill->m_index, lpMsg->index, 1);

		gObjTeleportMagicUse(lpMsg->index, lpMsg->x, lpMsg->y);

		lpObj->Mana -= (gSkillManager.GetSkillMana(lpSkill->m_index) * lpObj->MPConsumptionRate) / 100;
		lpObj->BP -= (gSkillManager.GetSkillBP(lpSkill->m_index) * lpObj->BPConsumptionRate) / 100;

		GCManaSend(lpObj->Index, 0xFF, (int)lpObj->Mana, lpObj->BP);
	}
}

void CSkillManager::GCSkillAttackSend(LPOBJ lpObj, int skill, int aIndex, BYTE type) // OK
{
	PMSG_SKILL_ATTACK_SEND pMsg;

	pMsg.header.setE(0x19, sizeof(pMsg));

	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);

	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

	pMsg.target[0] = SET_NUMBERHB(aIndex) | (type * 0x80);
	pMsg.target[1] = SET_NUMBERLB(aIndex);

	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}

void CSkillManager::GCSkillCancelSend(LPOBJ lpObj, int skill) // OK
{
	PMSG_SKILL_CANCEL_SEND pMsg;

	pMsg.header.set(0x1B, sizeof(pMsg));

	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);

	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}

void CSkillManager::GCDurationSkillAttackSend(LPOBJ lpObj, int skill, BYTE x, BYTE y, BYTE dir) // OK
{
	PMSG_DURATION_SKILL_ATTACK_SEND pMsg;

	pMsg.header.setE(0x1E, sizeof(pMsg));

	pMsg.skillH = SET_NUMBERHB(skill);
	pMsg.skillL = SET_NUMBERLB(skill);

	pMsg.indexH = SET_NUMBERHB(lpObj->Index);
	pMsg.indexL = SET_NUMBERLB(lpObj->Index);

	pMsg.x = x;
	pMsg.y = y;
	pMsg.dir = dir;

	if (lpObj->Type == OBJECT_USER &&
		gSkillManager.GetBaseSkill(skill) != SKILL_FIRE_SLASH &&
		gSkillManager.GetBaseSkill(skill) != SKILL_TRIPLE_SHOT)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}

void CSkillManager::GCRageFighterSkillAttackSend(LPOBJ lpObj, int skill, int aIndex, BYTE type) // OK
{
	PMSG_RAGE_FIGHTER_SKILL_ATTACK_SEND pMsg;

	pMsg.header.setE(0x4A, sizeof(pMsg));

	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);

	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

	pMsg.target[0] = SET_NUMBERHB(aIndex) | (type * 0x80);
	pMsg.target[1] = SET_NUMBERLB(aIndex);

	/*if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}*/

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}

void CSkillManager::GCSkillAddSend(int aIndex, BYTE slot, int skill, BYTE level, BYTE type) // OK
{
	BYTE send[256];

	PMSG_SKILL_LIST_SEND pMsg;

	pMsg.header.set(0xF3, 0x11, 0);

	int size = sizeof(pMsg);

	pMsg.count = 0xFE;

	pMsg.type = type;

	PMSG_SKILL_LIST info;

	info.slot = slot;

	info.skill = skill;

	info.level = (level << 3) | ((skill / 255) & 7);

	memcpy(&send[size], &info, sizeof(info));
	size += sizeof(info);

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(aIndex, send, size);
}

void CSkillManager::GCSkillDelSend(int aIndex, BYTE slot, int skill, BYTE level, BYTE type) // OK
{
	BYTE send[256];

	PMSG_SKILL_LIST_SEND pMsg;
	pMsg.header.set(0xF3, 0x11, 0);

	int size = sizeof(pMsg);

	pMsg.count = 0xFF;

	pMsg.type = type;

	PMSG_SKILL_LIST info;

	info.slot = slot;

	info.skill = skill;

	info.level = (level << 3) | ((skill / 255) & 7);

	memcpy(&send[size], &info, sizeof(info));
	size += sizeof(info);

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(aIndex, send, size);
}

void CSkillManager::GCSkillListSend(LPOBJ lpObj, BYTE type) // OK
{
	BYTE send[2048];

	PMSG_SKILL_LIST_SEND pMsg;

	pMsg.header.set(0xF3, 0x11, 0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	pMsg.type = type;

	PMSG_SKILL_LIST info;

	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() == 0)
		{
			continue;
		}
		info.slot = n;

		info.skill = lpObj->Skill[n].m_index;

		info.level = (lpObj->Skill[n].m_level << 3) | ((lpObj->Skill[n].m_index / 255) & 7);

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

#if(GAMESERVER_UPDATE>=1000)//2017/02/28
	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	//pMsg.btListType=0;
#elif(GAMESERVER_UPDATE>=902)//2017/02/28
	//pMsg.btListType=0;
	pMsg.header.size = size;
#else
	pMsg.header.size = size;
#endif

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(lpObj->Index, send, size);
#if(GAMESERVER_UPDATE>=1220)//2018/04/26
	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() == 0)
		{
			continue;
		}

		if (lpObj->Skill[n].IsForMasterSkill())
		{
			this->GCSkillAddSend(lpObj->Index, n, lpObj->Skill[n].m_skill, lpObj->Skill[n].m_level, 0);
			this->GCSkillAddSend(lpObj->Index, n, lpObj->Skill[n].m_index, lpObj->Skill[n].m_level, 0);
			continue;
		}

	}
#endif
}

void CSkillManager::ApplyIceBloodEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{

	if ((GetLargeRand() % 100) < 20)
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 2, 0, 0, 0, 0);
	}
#if(GAMESERVER_UPDATE>=1220)//2018/05/08
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2, 1);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1, 1);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2, 1);


	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 20, 0, 0, 0);
	}
#endif
}
void CSkillManager::ApplyFireBloodEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	if ((GetLargeRand() % 100) < 20)
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 1, 0, 0, 0, 0);
	}

	float Rand = 0;
	float Value = 0;
	float time = 0;

	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1, 0);
	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2, 0);

	if (Rand > 0)
	{
		Value = 30;
		time = 10;
	}

	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1, 0);
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2, 0);

	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2, 0);

	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, time, lpObj->Index, 1, Value, 0);
	}
}

void CSkillManager::ApplyMeteoriteEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING1, 0);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT1, 0);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING2, 0);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT2, 0);
	float time = 3;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION2, 0);

	if ((GetLargeRand() % 10) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < 10)
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, 10, 0, 0, 0, 0);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 0, 0, 0, 0);
	}
}
void CSkillManager::ApplyShockingEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_SHOCKING_UPGRADE_PROBABILITY_STUN1, 0);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_SHOCKING_UPGRADE_PROBABILITY_STUN2, 0);
	float time = 3;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_SHOCKING_EFFECT1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_SHOCKING_EFFECT2, 0);
	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION1, 0))
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_SHOCK, 5, 0, 0, 0, 0);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_SHOCK, time, 0, 0, 0, 0);
	}

}
void CSkillManager::ApplyEvilSpiritEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if (lpObj->Class == CLASS_DW)
	{
		float Rand = 0;
		Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1, 1);
		Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2, 1);
		float time = 10;
		time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1, 1);
		time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2, 1);


		if ((GetLargeRand() % 100) < Rand)
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 20, 0, 0, 0);
		}
	}
	if (lpObj->Class == CLASS_MG)
	{

		//ÎÁÒß
#if(GAMESERVER_UPDATE>=1220)//2018/05/08
		float Rand = 0;
		Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1, 5);
		Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2, 5);
		float time = 10;
		time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1, 5);
		time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2, 5);
		float value1 = 0;
		value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1, 5);
		value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2, 5);
		if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
		{
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_PLAGUE, time, lpObj->Index, 1, 2000 + value1, 0);
		}
#endif
	}



}
void CSkillManager::ApplyFreezingSlaughterEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{

	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_FREEZING_UPGRADE_PROBABILITY_FREEZE1, 1);
	float time = 3;

	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_FREEZING_UPGRADE_PROBABILITY_FREEZE2, 1);
	float value1 = 0;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_FREEZING_EFFECT1, 1);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_FREEZING_EFFECT2, 1);
	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION1, 1))
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING1, time + 5, value1, 0, 0, 0);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING1, time, value1, 0, 0, 0);
	}

}
void CSkillManager::ApplyMeteorStormEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2, 2);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1, 2);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2, 2);
	float value1 = 0;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1, 2);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2, 2);
	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_PLAGUE, time, lpObj->Index, 1, 2000 + value1, 0);
	}
}

void CSkillManager::ApplyPoisoningStormEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{

	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_POISONING_UPGRADE_PROBABILITY_ADDICTION1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_POISONING_UPGRADE_PROBABILITY_ADDICTION2, 2);
	float time = 10;

	float value1 = 2000;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_POISONING_EFFECT1, 2);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_POISONING_EFFECT2, 2);
	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION1, 2))
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_ADDICTED, time + 5, value1, 0, 0, 0);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_ADDICTED, time, value1, 0, 0, 0);
	}

}
void CSkillManager::ApplyBleedingHeartEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{

	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_BLEEDING_UPGRADE_PROBABILITY_EXCESSIVE_BLEEDING1, 0);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_BLEEDING_UPGRADE_PROBABILITY_EXCESSIVE_BLEEDING2, 0);
	float time = 10;

	float value1 = 30;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_BLEEDING_EFFECT1, 0);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_UPGRADED_BLEEDING_EFFECT2, 0);
	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_PROBABILITY_TO_INCREASE_MAXIMUM_DURATION1, 0))
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_ADDICTED, time + 5, lpObj->Index, 1, value1, 15);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_ADDICTED, time, lpObj->Index, 1, value1, 15);
	}

}
void CSkillManager::ApplyFireBlowEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING1, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT1, 1);

	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING2, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT2, 1);
	float time = 3;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION1, 1);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION2, 1);

	if ((GetLargeRand() % 10) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < 10)
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, 10, 0, 0, 0, 0);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 0, 0, 0, 0);
	}

}
void CSkillManager::ApplyTripleShotEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	return;

	float Rand = 0;
	float Value = 0;
	float time = 0;

	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1, 0);
	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2, 0);

	if (Rand > 0)
	{
		Value = 30;
		time = 10;
	}
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1, 0);
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2, 0);

	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2, 0);

	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, time, lpObj->Index, 1, Value, 0);
	}
}

void CSkillManager::ApplyFocusShotEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2, 2);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1, 2);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2, 2);
	float value1 = 0;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1, 2);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2, 2);
	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_PLAGUE, time, lpObj->Index, 1, 2000 + value1, 0);
	}
}

void CSkillManager::ApplyDarkBlastEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING1, 3);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT1, 3);

	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_SHOCKING2, 3);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_EFFECT2, 3);
	float time = 3;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION1, 3);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_SHOCKING_DURATION2, 3);

	if ((GetLargeRand() % 10) < (Rand - lpTarget->ResistStunRate))
	{
		if ((GetLargeRand() % 100) < 10)
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, 10, 0, 0, 0, 0);
		else
			gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 0, 0, 0, 0);
	}

}
void CSkillManager::ApplyLightningStormEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	//±ù¶³
#if(GAMESERVER_UPDATE>=1220)//2018/05/08
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1, 4);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2, 4);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1, 4);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2, 4);


	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 20, 0, 0, 0);
	}
#endif

}

void CSkillManager::ApplyBirdsEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	return;

	float Rand = 0;
	float Value = 0;
	float time = 0;

	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1, 0);
	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2, 0);

	if (Rand > 0)
	{
		Value = 30;
		time = 10;
	}

	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1, 0);
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2, 0);

	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2, 0);

	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, time, lpObj->Index, 1, Value, 0);
	}
}

void CSkillManager::ApplyWindSoulEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	//±ù¶³
#if(GAMESERVER_UPDATE>=1220)//2018/05/08
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2, 1);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1, 1);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2, 1);


	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 20, 0, 0, 0);
	}
#endif


}
void CSkillManager::ApplyRedStormEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	//³öÑª
#if(GAMESERVER_UPDATE>=1220)//2018/05/07
	float Rand = 0;
	float Value = 0;
	float time = 0;
	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1, 0);
	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2, 0);
	if (Rand > 0)
	{
		Value = 30;
		time = 10;
	}
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1, 0);
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2, 0);

	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2, 0);
	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, time, lpObj->Index, 1, Value, 0);
	}


#endif


}
void CSkillManager::ApplyAquaBeastEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	//±ù¶³
#if(GAMESERVER_UPDATE>=1220)//2018/05/08
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2, 1);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1, 1);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2, 1);


	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 20, 0, 0, 0);
	}
#endif


}
void CSkillManager::ApplyFireBeastEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	//ÎÁÒß
#if(GAMESERVER_UPDATE>=1220)//2018/05/08
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_POISONING2, 2);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION1, 2);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_DURATION2, 2);
	float value1 = 0;
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT1, 2);
	value1 += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_POISONING_EFFECT2, 2);
	if ((GetLargeRand() % 100) < (Rand - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_PLAGUE, time, lpObj->Index, 1, 2000 + value1, 0);
	}
#endif


}
void CSkillManager::ApplyDarkSideEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	//±ù¶³
#if(GAMESERVER_UPDATE>=1220)//2018/05/08
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1, 1);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2, 1);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1, 1);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2, 1);


	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 20, 0, 0, 0);
	}
#endif


}
void CSkillManager::ApplyBrecheEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	return;

	float Rand = 0;
	float Value = 0;
	float time = 0;

	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING1, 0);
	Rand = gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_BLEEDING2, 0);

	if (Rand > 0)
	{
		Value = 30;
		time = 10;
	}

	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT1, 0);
	Value += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_EFFECT2, 0);

	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION1, 0);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_BLEEDING_DURATION2, 0);

	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_BLEEDING, time, lpObj->Index, 1, Value, 0);
	}
}
void CSkillManager::ApplyShiningPeakEffect_4th(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage) // OK
{
	float Rand = 0;
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING1, 2);
	Rand += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_PROBABILITY_OF_FREEZING2, 2);
	float time = 10;
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION1, 2);
	time += gMasterSkillTree_4th.GetForMasterSkillValue(lpObj, FOR_MASTER_SKILL_ADD_INCREASES_FREEZING_DURATION2, 2);

	if ((GetLargeRand() % 100) < Rand)
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_FREEZING, time, 20, 0, 0, 0);
	}
}









//////////////////////////////////////////////////
///SKILL FUNCTIONS RECODED FOR HIT QUEUE SYSTEM///
//////////////////////////////////////////////////

void CSkillManager::CGSkillAttack(int aIndex, int bIndex, int skill)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
		return;

	if (lpObj->Live == 0)
		return;

	if (OBJECT_RANGE(bIndex) == 0)
		return;

	LPOBJ lpTarget = &gObj[bIndex];

	if (gObjIsConnectedGS(bIndex) == 0)
		return;

	if (lpTarget->Live == 0)
		return;

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0 || gMap[lpTarget->Map].CheckAttr(lpTarget->X, lpTarget->Y, 1) != 0)
		return;

	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0 || gDuel.GetDuelArenaBySpectator(bIndex) != 0)
		return;

	CSkill* lpSkill = 0;

	if (lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj, skill);

		if (lpSkill == 0)
			return;

		if (this->CheckSkillDelay(lpObj, lpSkill->m_index) == 0)
			return;

		if (this->CheckSkillRequireKillPoint(lpObj, lpSkill->m_index) == 0)
			return;

		if (this->CheckSkillRequireGuildStatus(lpObj, lpSkill->m_index) == 0)
			return;

		if (this->CheckSkillRequireClass(lpObj, lpSkill->m_index) == 0)
			return;
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];

		if (lpSkill->IsSkill() == 0)
			return;
	}

	lpObj->MultiSkillIndex = 0;
	lpObj->MultiSkillCount = 0;

	this->UseAttackSkill(aIndex, ((skill == SKILL_NOVA_START || skill == MASTER_SKILL_ADD_NOVA_START_IMPROVED) ? skill : bIndex), lpSkill);
}

void CSkillManager::CGDurationSkillAttack(int aIndex, int skill, int x, int y, int dir, int dis, int angle, int bIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
		return;

	if (lpObj->Live == 0)
		return;

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
		return;

	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0)
		return;

	CSkill* lpSkill = 0;

	if (lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj, skill);

		if (lpSkill == 0)
			return;

		if (this->CheckSkillDelay(lpObj, lpSkill->m_index) == 0)
			return;

		if (this->CheckSkillRequireKillPoint(lpObj, lpSkill->m_index) == 0)
			return;

		if (this->CheckSkillRequireGuildStatus(lpObj, lpSkill->m_index) == 0)
			return;

		if (this->CheckSkillRequireClass(lpObj, lpSkill->m_index) == 0)
			return;
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];

		if (lpSkill->IsSkill() == 0)
			return;
	}

	lpObj->MultiSkillIndex = 0;
	lpObj->MultiSkillCount = 0;

	this->UseDurationSkillAttack(lpObj->Index, bIndex, lpSkill, x, y, dir, angle);
}

void CSkillManager::CGSkillDarkSide(int aIndex, int bIndex, int skill)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
		return;

	if (lpObj->Live == 0 || lpObj->Teleport != 0)
		return;

	CSkill* lpSkill = this->GetSkill(lpObj, skill);

	if (lpSkill == 0)
		return;

	if (lpSkill->m_skill != SKILL_DARK_SIDE)
		return;

	PMSG_SKILL_DARK_SIDE_SEND pMsg;

	pMsg.header.setE(0x4B, sizeof(pMsg));

	pMsg.skill = lpSkill->m_index;

	pMsg.index[0] = 20000;
	pMsg.index[1] = 20000;
	pMsg.index[2] = 20000;
	pMsg.index[3] = 20000;
	pMsg.index[4] = 20000;

	this->SkillDarkSideGetTargetIndex(aIndex, ((bIndex == 0) ? -1 : bIndex), lpSkill, pMsg.index);

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CSkillManager::CGRageFighterSkillAttack(int aIndex, int bIndex, int skill)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGS(aIndex) == 0)
		return;

	if (lpObj->Live == 0 || lpObj->Teleport != 0)
		return;

	CSkill* lpSkill = this->GetSkill(lpObj, skill);

	if (lpSkill == 0)
		return;

	if (lpSkill->m_skill != SKILL_UPPER_BEAST && lpSkill->m_skill != SKILL_CHAIN_DRIVER && lpSkill->m_skill != SKILL_DARK_SIDE)
		return;

	lpObj->RageFighterSkillIndex = lpSkill->m_index;

	lpObj->RageFighterSkillCount = 0;

	if (lpSkill->m_skill == SKILL_DARK_SIDE)
	{
		this->GCRageFighterSkillAttackSend(lpObj, lpSkill->m_index, lpObj->RageFighterSkillTarget, 1);
	}
	else if (OBJECT_RANGE(bIndex) != 0)
	{
		this->GCRageFighterSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	}
}

bool CSkillManager::SkillSpiritHook(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);

	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 0, 0);

	if (lpTarget->Type == OBJECT_USER)
	{
		return 1;
	}

	int count = 1;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);

		count++;

		if (count > 2)
		{
			break;
		}
	}

	return 1;
}

int CSkillManager::GetBaseSkill(int skill)
{
	int base;
	return (((base = gMasterSkillTree.GetMasterSkillRelated(skill)) == 0) ? skill : base);
}

int CSkillManager::GetDamageRatePvP(int skill)
{
	return this->DamageRatePvP[this->GetBaseSkill(skill)];
}

int CSkillManager::GetDamageRatePvM(int skill)
{
	return this->DamageRatePvM[this->GetBaseSkill(skill)];
}

bool CSkillManager::CheckSkillBlock(LPOBJ lpObj, int Skill)
{
	Skill = this->GetBaseSkill(Skill);

	for (int n = 0; n < 100; n++)
	{
		if (this->SkillBlock[n].Skill == -1)
			return false;

		if (this->SkillBlock[n].Skill == Skill)
		{
			if (lpObj->Map == this->SkillBlock[n].Map)
			{
				if (lpObj->X >= this->SkillBlock[n].X1 && lpObj->X <= this->SkillBlock[n].X2)
				{
					if (lpObj->Y >= this->SkillBlock[n].Y1 && lpObj->Y <= this->SkillBlock[n].Y2)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

void CSkillManager::DecreasePartyMemberHP(LPOBJ lpObj)
{	
	int partynum = lpObj->PartyNumber;

	if (gParty.IsParty(partynum) == 0)
	{
		return;
	}

	LPOBJ lpPartyObj;

	int partycount = gParty.m_PartyInfo[partynum].Count;

	for (int n = 0; n < MAX_PARTY_USER; n++)
	{
		int number = gParty.m_PartyInfo[partynum].Index[n];

		if (number >= 0)
		{
			lpPartyObj = &gObj[number];

			if (lpObj->Map == lpPartyObj->Map && lpObj->Index != number)
			{
				int dis = gObjCalcDistance(lpObj, &gObj[number]);

				if (dis < 10)
				{
					lpPartyObj = &gObj[number];
					int declife = lpPartyObj->MaxLife * 20.0f / 100.0f;
					lpPartyObj->Life = lpPartyObj->Life - declife;
					GCLifeSend(lpPartyObj->Index, 0xFF, (int)lpPartyObj->Life, lpPartyObj->Shield);
					gObjectManager.CharacterLifeCheck(lpObj, lpPartyObj, declife, 0, 0, 0, 0, 0);
				}
			}
		}
	}
}