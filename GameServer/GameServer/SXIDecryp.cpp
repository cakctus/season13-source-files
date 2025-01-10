#include "stdafx.h"
#include "SXIDecryp.h"
#include "DefaultClassInfo.h"
#include "Util.h"
#include "MasterSkillTree.h"
#include "ServerInfo.h"

void C3TwoDecryp(BYTE* m_byBuffer)//C3
{
	if (m_byBuffer[2] == 0x24)//移动装备2次解密
	{
		unsigned char tmp, r1, r4;
		tmp = m_byBuffer[4];
		m_byBuffer[4] = m_byBuffer[9];
		m_byBuffer[9] = tmp;
		tmp = m_byBuffer[0xD];
		m_byBuffer[0xD] = m_byBuffer[7];
		m_byBuffer[7] = tmp;
		tmp = m_byBuffer[0xA];
		m_byBuffer[0xA] = m_byBuffer[0xB];
		m_byBuffer[0xB] = tmp;
		r1 = m_byBuffer[0x12] >> 1;
		r4 = m_byBuffer[0x12] >> 4;
		r1 &= 1;
		r4 &= 1;
		if (r1 == 0 && r4 == 0) {}
		if (r1 == 0 && r4 == 1)
		{
			m_byBuffer[0x12] += 2;
			m_byBuffer[0x12] -= 0x10;
		}
		if (r1 == 1 && r4 == 0)
		{
			m_byBuffer[0x12] -= 2;
			m_byBuffer[0x12] += 0x10;
		}

		if (r1 == 1 && r4 == 1) {}
		m_byBuffer[0xF] ^= 0x9F;


	}

	if (m_byBuffer[2] == 0x0E && m_byBuffer[1] == 0x0D)//C3 13
	{
		unsigned char tmp, p1, r3, r5;
		m_byBuffer[5] ^= 0xC5;
		p1 = m_byBuffer[7];
		__asm Rol p1, 3
		m_byBuffer[7] = p1;
		tmp = m_byBuffer[3];
		m_byBuffer[3] = m_byBuffer[5];
		m_byBuffer[5] = tmp;
		r3 = m_byBuffer[5] >> 3;
		r5 = m_byBuffer[5] >> 5;
		r3 &= 1;
		r5 &= 1;
		if (r3 == 0 && r5 == 0) {}
		if (r3 == 0 && r5 == 1)
		{
			m_byBuffer[5] += 0x8;
			m_byBuffer[5] -= 0x20;
		}
		if (r3 == 1 && r5 == 0)
		{
			m_byBuffer[5] -= 0x8;
			m_byBuffer[5] += 0x20;
		}
		if (r3 == 1 && r5 == 1) {}
		tmp = m_byBuffer[0xA];
		m_byBuffer[0xA] = m_byBuffer[8];
		m_byBuffer[8] = tmp;

		p1 = m_byBuffer[6];
		__asm Rol p1, 2
		m_byBuffer[6] = p1;

	}

	if (m_byBuffer[2] == 0x19)//C1 07 19 释放技能
	{
		unsigned char tmp;
		m_byBuffer[3] ^= 0x5F;
		m_byBuffer[6] ^= 0x67;
		tmp = m_byBuffer[3];
		m_byBuffer[3] = m_byBuffer[6];
		m_byBuffer[6] = tmp;
		tmp = m_byBuffer[4];
		m_byBuffer[4] = m_byBuffer[5];
		m_byBuffer[5] = tmp;
		m_byBuffer[5] ^= 0xCB;
		m_byBuffer[4] ^= 0x74;
		tmp = m_byBuffer[3];
		__asm ROL tmp, 1;
		m_byBuffer[3] = tmp;

	}
}
void TwoDecryp(BYTE* m_byBuffer)//C1
{
	unsigned char tmp;
	unsigned char p1, p2, r2, r5, r6, r1, r4;
	if (*m_byBuffer != 0xC1)
	{
		return;
	}
	if (m_byBuffer[2] == 0xF3)
	{

		if (m_byBuffer[3] == 0x0E || m_byBuffer[3] == 0x18 || m_byBuffer[3] == 0x0C || m_byBuffer[3] == 0x01)
		{
			tmp = m_byBuffer[4];
			__asm
			{
				Rol tmp, 2
			}
			m_byBuffer[4] = m_byBuffer[9];
			m_byBuffer[9] = tmp;
			m_byBuffer[10] ^= 0xAE;
			tmp = m_byBuffer[4];
			m_byBuffer[4] = m_byBuffer[6];
			m_byBuffer[6] = tmp;
			tmp = m_byBuffer[8];
			m_byBuffer[8] = m_byBuffer[10];
			m_byBuffer[10] = tmp;
			m_byBuffer[4] ^= 0x1E;
		}
	}
	if (m_byBuffer[2] == 0xD7 && m_byBuffer[1] != 0x06)
	{
		tmp = m_byBuffer[6];
		m_byBuffer[6] = m_byBuffer[4];
		m_byBuffer[4] = tmp;

		p1 = m_byBuffer[3];
		p2 = m_byBuffer[6];
		__asm
		{
			Rol p1, 4
			Rol p2, 3
		}
		m_byBuffer[3] = p1;
		m_byBuffer[6] = p2;
		tmp = m_byBuffer[3];
		m_byBuffer[3] = m_byBuffer[4];
		m_byBuffer[4] = tmp;

	}
	if (m_byBuffer[2] == 0xF1 && m_byBuffer[1] == 0x32)
	{
		m_byBuffer[0x1F] ^= 0xD;
		p1 = m_byBuffer[0x13];
		__asm ROL p1, 1
		m_byBuffer[0x13] = p1;
		r2 = m_byBuffer[0x5] >> 2;
		r5 = m_byBuffer[0x5] >> 5;
		r2 &= 1;
		r5 &= 1;
		if (r2 == 0 && r5 == 0) {}
		if (r2 == 0 && r5 == 1)
		{
			m_byBuffer[0x5] += 0x20;
			m_byBuffer[0x5] -= 0x4;
		}
		if (r2 == 1 && r5 == 0)
		{
			m_byBuffer[0x5] -= 0x20;
			m_byBuffer[0x5] += 0x4;
		}
		if (r2 == 1 && r5 == 1) {}

		r2 = m_byBuffer[0xC] >> 2;
		r6 = m_byBuffer[0xC] >> 6;
		r2 &= 1;
		r6 &= 1;
		if (r2 == 0 && r6 == 0) {}
		if (r2 == 1 && r6 == 0)
		{
			m_byBuffer[0xC] += 0x4;
			m_byBuffer[0xC] -= 0x40;
		}
		if (r2 == 0 && r6 == 1)
		{
			m_byBuffer[0xC] -= 0x4;
			m_byBuffer[0xC] += 0x40;
		}
		if (r2 == 1 && r6 == 1) {}
		p1 = m_byBuffer[0xA];
		__asm ROL p1, 3
		m_byBuffer[0xA] = p1;
		m_byBuffer[0x1F] ^= 0xB2;
		p1 = m_byBuffer[0x14];
		__asm ROL p1, 3
		m_byBuffer[0x14] = p1;
	}
	if (m_byBuffer[2] == 0xD9)
	{
		tmp = m_byBuffer[5];
		__asm  ROL tmp, 5
		m_byBuffer[5] = tmp;
		m_byBuffer[5] ^= 0x0B;
		tmp = m_byBuffer[3];
		__asm  ROL tmp, 7
		m_byBuffer[3] = tmp;
		r1 = m_byBuffer[3] >> 1;
		r4 = m_byBuffer[3] >> 4;
		r1 &= 1;
		r4 &= 1;
		if (r1 == 0 && r4 == 0) {}
		if (r1 == 1 && r4 == 0)
		{
			m_byBuffer[3] -= 0x02;
			m_byBuffer[3] += 0x10;
		}
		if (r1 == 0 && r4 == 1)
		{

			m_byBuffer[3] += 0x02;
			m_byBuffer[3] -= 0x10;
		}
		if (r1 == 1 && r4 == 1) {}
	}
}

void S12Decryp(BYTE* m_byBuffer)
{
	unsigned char tmp, R2, R5, R1, R4, R7;
	if (m_byBuffer[2] == 0xF1 && m_byBuffer[1] == 0x3C)//帐号登陆包C3
	{
		m_byBuffer[7] ^= 0xFD;
		R2 = m_byBuffer[0xF] >> 2;
		R5 = m_byBuffer[0xF] >> 5;
		R2 &= 1;
		R5 &= 1;
		if (R2 == 0 && R5 == 0) {}
		if (R2 == 0 && R5 == 1)
		{
			m_byBuffer[0xF] += 4;
			m_byBuffer[0xF] -= 0x20;
		}
		if (R2 == 1 && R5 == 0)
		{
			m_byBuffer[0xF] -= 4;
			m_byBuffer[0xF] += 0x20;
		}
		if (R2 == 1 && R5 == 1) {}

		R5 = m_byBuffer[0x8] >> 5;
		R1 = m_byBuffer[0x8] >> 1;
		R5 &= 1;
		R1 &= 1;
		if (R1 == 0 && R5 == 0) {}
		if (R1 == 0 && R5 == 1)
		{
			m_byBuffer[0x8] -= 0x20;
			m_byBuffer[0x8] += 2;
		}
		if (R1 == 1 && R5 == 0)
		{
			m_byBuffer[0x8] += 0x20;
			m_byBuffer[0x8] -= 2;
		}
		if (R1 == 1 && R5 == 1) {}
		tmp = m_byBuffer[0xD];
		__asm ROL tmp, 3
		m_byBuffer[0xD] = tmp;

		tmp = m_byBuffer[0x5];
		m_byBuffer[0x5] = m_byBuffer[0xD];
		m_byBuffer[0xD] = tmp;
		m_byBuffer[0x3] ^= 0x86;
		m_byBuffer[0x12] ^= 0xF7;
	}
	if (m_byBuffer[2] == 0xF3)//创建角色
	{
		if (m_byBuffer[1] == 0x0F || m_byBuffer[1] == 0x0E)
		{
			tmp = m_byBuffer[3];
			__asm ROL tmp, 3
			m_byBuffer[3] = tmp;
			m_byBuffer[6] ^= 0x41;
			m_byBuffer[6] ^= 0xCC;
			R4 = m_byBuffer[5] >> 4;
			R7 = m_byBuffer[5] >> 7;
			R4 &= 1;
			R7 &= 1;
			if (R4 == 0 && R7 == 0) {}
			if (R4 == 0 && R7 == 1)
			{
				m_byBuffer[5] += 0x10;
				m_byBuffer[5] -= 0x80;
			}
			if (R4 == 1 && R7 == 0)
			{
				m_byBuffer[5] -= 0x10;
				m_byBuffer[5] += 0x80;
			}
			if (R4 == 1 && R7 == 1) {}
			tmp = m_byBuffer[0xA];
			__asm ROL tmp, 4
			m_byBuffer[0xA] = tmp;
			m_byBuffer[6] ^= 0x98;
			m_byBuffer[9] ^= 0x21;
			tmp = m_byBuffer[9];
			m_byBuffer[9] = m_byBuffer[3];
			m_byBuffer[3] = tmp;
			tmp = m_byBuffer[5];
			__asm ROL tmp, 2
			m_byBuffer[5] = tmp;
			m_byBuffer[9] ^= 0xAF;
			tmp = m_byBuffer[4];
			__asm ROL tmp, 3
			m_byBuffer[4] = tmp;
			tmp = m_byBuffer[8];
			m_byBuffer[8] = m_byBuffer[9];
			m_byBuffer[9] = tmp;
		}
	}

}

void WebDecryp(BYTE* m_byBuffer)
{

}

void XorData(int iStart, int iEnd, BYTE* m_byBuffer)	// Good
{
	//if ( iStart < iEnd  )
	//{
	//	return;
	//}

	unsigned char byXorFilter[32];

	byXorFilter[0] = 0xAB;
	byXorFilter[1] = 0x11;
	byXorFilter[2] = 0xCD;
	byXorFilter[3] = 0xFE;
	byXorFilter[4] = 0x18;
	byXorFilter[5] = 0x23;
	byXorFilter[6] = 0xC5;
	byXorFilter[7] = 0xA3;
	byXorFilter[8] = 0xCA;
	byXorFilter[9] = 0x33;
	byXorFilter[10] = 0xC1;
	byXorFilter[11] = 0xCC;
	byXorFilter[12] = 0x66;
	byXorFilter[13] = 0x67;
	byXorFilter[14] = 0x21;
	byXorFilter[15] = 0xF3;
	byXorFilter[16] = 0x32;
	byXorFilter[17] = 0x12;
	byXorFilter[18] = 0x15;
	byXorFilter[19] = 0x35;
	byXorFilter[20] = 0x29;
	byXorFilter[21] = 0xFF;
	byXorFilter[22] = 0xFE;
	byXorFilter[23] = 0x1D;
	byXorFilter[24] = 0x44;
	byXorFilter[25] = 0xEF;
	byXorFilter[26] = 0xCD;
	byXorFilter[27] = 0x41;
	byXorFilter[28] = 0x26;
	byXorFilter[29] = 0x3C;
	byXorFilter[30] = 0x4E;
	byXorFilter[31] = 0x4D;

	for (int i = iStart; i != iEnd; i--)
	{
		m_byBuffer[i] ^= m_byBuffer[i - 1] ^ byXorFilter[i % 32];

	}
}

void GCCharacterInfo(LPOBJ lpObj)
{
	PMSG_SXUPPRONT pMsg;
	pMsg.h.set(0xEC, 0x25, sizeof(PMSG_SXUPPRONT));
	pMsg.h.size = sizeof(PMSG_SXUPPRONT);
	pMsg.Str = lpObj->Strength;
	pMsg.AddStr = lpObj->AddStrength;
	pMsg.Dex = lpObj->Dexterity;
	pMsg.AddDex = lpObj->AddDexterity;
	pMsg.Vit = lpObj->Vitality;
	pMsg.AddVit = lpObj->AddVitality;
	pMsg.Ene = lpObj->Energy;
	pMsg.AddEne = lpObj->AddEnergy;
	pMsg.Leadership = lpObj->Leadership;
	pMsg.AddLeadership = lpObj->AddLeadership;
	pMsg.mPrec = lpObj->m_MPSkillOpt.MPRecoveryRate + lpObj->MPRecovery;
	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(PMSG_SXUPPRONT));
}

void SXGetMosaic(int aIndex)
{
	PMSG_SXMOSAIC pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	LPOBJ lpObj = &gObj[aIndex];
	pMsg.h.set(0xEC, 0x26, sizeof(pMsg));

	pMsg.nEmeAttackType = 13;
	pMsg.nEmeAttackValue = 14;
	pMsg.nSapphDefeseType = 15;
	pMsg.nSapphDefeseValue = 16;
	pMsg.nTopaAttackType = 17;
	pMsg.nTopaAttackAttack = 18;
	
	pMsg.nAttributeResistantToAttributeAttacks_info = 32;
	pMsg.nAttributeAddBunNoErtelOptionEffect_info = 33;
	pMsg.nAttributeAddGaHoErtelOptionEffect_info = 34;

	pMsg.nAttributeAttackPowerMin_info = lpObj->ElementalDamageMin;
	pMsg.nAttributeAttackPowerMax_info = lpObj->ElementalDamageMax;
	pMsg.nAttributeAttackPowerPVP_Min_info = lpObj->ElementalDamageMin;
	pMsg.nAttributeAttackPowerPVP_Max_info = lpObj->ElementalDamageMax;
	pMsg.nAttributeAttackSuccessRatePVE_info = lpObj->ElementalAttackSuccessRate;
	pMsg.nAttributeAttackSuccessRatePVP_info = lpObj->ElementalAttackSuccessRate;
	pMsg.nAttributeDefencePowerPVE_info = lpObj->ElementalDefense;
	pMsg.nAttributeDefencePowerPVP_info = lpObj->ElementalDefense;
	pMsg.nAttributeDefenceSuccessRatePVE_info = lpObj->ElementalDefenseSuccessRate;
	pMsg.nAttributeDefenceSuccessRatePVP_info = lpObj->ElementalDefenseSuccessRate;

	pMsg.nRubyAttackType = lpObj->ElementalAttribute;
	pMsg.nRubyAttackValue = 12;
	
	//Taxa de dano critico
	pMsg.nAttributeCriticalDamageRate_info = (lpObj->PentagramOption.AddElementalCriticalDamageRate + lpObj->EffectOption.IncreaseElementalCriticalRate);
	
	//Pode de Punish
	pMsg.PunishRate = lpObj->PentagramJewelOption.Punish;

	//Poder de Paralyzing
	pMsg.ParalyzingRate = lpObj->PentagramJewelOption.Paralyzing;
	
	//Chance de Blinding
	pMsg.BlidingRate = lpObj->PentagramJewelOption.Blinding; 

	//Binding
	pMsg.BindingRate = lpObj->PentagramJewelOption.Binding;

	//Bleeding
	pMsg.BleedingDamage = lpObj->PentagramJewelOption.Bleeding;

	//Absorbs do Radiance
	pMsg.AbsorbDamage = lpObj->PentagramJewelOption.DefenseAbs;
	pMsg.AbsorbDamageAsLife = lpObj->PentagramJewelOption.LifeAbs;

	//Aumento de dano em Boss
	pMsg.IncreaseBossElementalDamage = (int)(lpObj->PentagramJewelOption.AddElementalDamageRatioPvM + lpObj->PentagramJewelOption.MulElementalDamagePvM + lpObj->AddBossElementalDamage + lpObj->EffectOption.IncreaseBossElementalDamage);
	//Redu玢o de dano de Boss
	pMsg.ReduceBossElementalDamage = lpObj->PentagramJewelOption.AddElementalDamageReductionPvM + lpObj->DecBossElementalDamage + lpObj->EffectOption.DecreaseBossElementalDamage + (lpObj->ReduceGeneralElementalDamageRate / 2);
	//Aumento de dano em PVP
	pMsg.IncreasePVPElementalDamage = (lpObj->PentagramJewelOption.AddElementalDamageRatioPvP + lpObj->AddElementalDamageRate + lpObj->EffectOption.IncreasePvPElementalDamage);
	//Redu玢o de dano em PVP
	pMsg.ReducePVPElementalDamage = (lpObj->PentagramOption.AddElementalDefenseTransferRate + lpObj->PentagramJewelOption.AddElementalDamageReductionPvP + lpObj->ReduceGeneralElementalDamageRate);

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(PMSG_SXMOSAIC));
}

void SXGetMosaicOLD(int aIndex)
{
	PMSG_SXMOSAIC pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	LPOBJ lpObj = &gObj[aIndex];
	pMsg.h.set(0xEC, 0x26, sizeof(pMsg));

	pMsg.nAttributeAttackPowerMin_info = 1;
	pMsg.nAttributeAttackPowerMax_info = 2;
	pMsg.nAttributeAttackPowerPVP_Min_info = 3;
	pMsg.nAttributeAttackPowerPVP_Max_info = 4;
	pMsg.nAttributeAttackSuccessRatePVE_info = 5;
	pMsg.nAttributeAttackSuccessRatePVP_info = 6;
	pMsg.nAttributeDefencePowerPVE_info = 7;
	pMsg.nAttributeDefencePowerPVP_info = 8;
	pMsg.nAttributeDefenceSuccessRatePVE_info = 9;
	pMsg.nAttributeDefenceSuccessRatePVP_info = 10;
	pMsg.nRubyAttackType = 11;
	pMsg.nRubyAttackValue = 12;
	pMsg.nEmeAttackType = 13;
	pMsg.nEmeAttackValue = 14;
	pMsg.nSapphDefeseType = 15;
	pMsg.nSapphDefeseValue = 16;
	pMsg.nTopaAttackType = 17;
	pMsg.nTopaAttackAttack = 18;
	pMsg.nAttributeCriticalDamageRate_info = 19;
	pMsg.IncreaseBossElementalDamage = 20;
	pMsg.IncreasePVPElementalDamage = 21;
	pMsg.ReducePVPElementalDamage = 22;
	pMsg.ReduceBossElementalDamage = 23;
	pMsg.AbsorbDamage = 24;
	pMsg.AbsorbDamageAsLife = 25;
	pMsg.BleedingDamage = 26;
	pMsg.BindingRate = 27;
	pMsg.ParalyzingRate = 28;
	pMsg.PunishRate = 29;
	pMsg.BlidingRate = 30;
	pMsg.ReducePVPElementalDamage = 31;
	pMsg.nAttributeResistantToAttributeAttacks_info = 32;
	pMsg.nAttributeAddBunNoErtelOptionEffect_info = 33;
	pMsg.nAttributeAddGaHoErtelOptionEffect_info = 34;

	pMsg.nAttributeAttackPowerMin_info = lpObj->ElementalDamageMin;
	pMsg.nAttributeAttackPowerMax_info = lpObj->ElementalDamageMax;
	pMsg.nAttributeAttackPowerPVP_Min_info = lpObj->ElementalDamageMin;
	pMsg.nAttributeAttackPowerPVP_Max_info = lpObj->ElementalDamageMax;
	pMsg.nAttributeAttackSuccessRatePVE_info = lpObj->ElementalAttackSuccessRate;
	pMsg.nAttributeAttackSuccessRatePVP_info = lpObj->ElementalAttackSuccessRate;
	pMsg.nAttributeDefencePowerPVE_info = lpObj->ElementalDefense;
	pMsg.nAttributeDefencePowerPVP_info = lpObj->ElementalDefense;
	pMsg.nAttributeDefenceSuccessRatePVE_info = lpObj->ElementalDefenseSuccessRate;
	pMsg.nAttributeDefenceSuccessRatePVP_info = lpObj->ElementalDefenseSuccessRate;

	pMsg.nRubyAttackType = lpObj->ElementalAttribute;
	pMsg.nRubyAttackValue = 0;

	pMsg.nEmeAttackType = 0;
	pMsg.nEmeAttackValue = 0;
	pMsg.nSapphDefeseType = 0;
	pMsg.nSapphDefeseValue = 0;
	pMsg.nTopaAttackType = 0;
	pMsg.nTopaAttackAttack = 0;

	pMsg.IncreaseBossElementalDamage = 0;
	pMsg.IncreasePVPElementalDamage = 0;
	pMsg.ReducePVPElementalDamage = 0;
	pMsg.ReduceBossElementalDamage = 0;
	pMsg.AbsorbDamage = 0;
	pMsg.AbsorbDamageAsLife = 0;
	pMsg.BleedingDamage = lpObj->PentagramJewelOption.Bleeding;
	pMsg.BindingRate = lpObj->PentagramJewelOption.Paralyzing;
	pMsg.ParalyzingRate = 0;
	pMsg.PunishRate = 0;
	pMsg.BlidingRate = lpObj->PentagramJewelOption.Binding;
	pMsg.ReducePVPElementalDamage = 0;
	pMsg.nAttributeResistantToAttributeAttacks_info = 0;
	pMsg.nAttributeAddBunNoErtelOptionEffect_info = 0;
	pMsg.nAttributeAddGaHoErtelOptionEffect_info = 0;
	pMsg.nAttributeCriticalDamageRate_info = lpObj->PentagramOption.AddElementalCriticalDamageRate;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(PMSG_SXMOSAIC));
}

void SXGetCharacterInfo(int aIndex)
{
	PMSG_SXCHARINFO pMsg;
	memset(&pMsg, 0, sizeof(pMsg));

	LPOBJ lpObj = &gObj[aIndex];

	pMsg.h.set(0xEC, 0x29, sizeof(PMSG_SXCHARINFO));

	pMsg.Str = (short)lpObj->Strength;
	pMsg.AddStr = (short)lpObj->AddStrength;
	pMsg.Dex = (short)lpObj->Dexterity;
	pMsg.AddDex = (short)lpObj->AddDexterity;
	pMsg.Vit = (short)lpObj->Vitality;
	pMsg.AddVit = (short)lpObj->AddVitality;
	pMsg.Energy = (short)lpObj->Energy;
	pMsg.AddEnergy = (short)lpObj->AddEnergy;
	pMsg.Leadership = (short)lpObj->Leadership;
	pMsg.AddLeadership = (short)lpObj->AddLeadership;

	pMsg.CriticalDamageRate = (float)(((lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate) > gServerInfo.m_CriticalDamageRate) ? gServerInfo.m_CriticalDamageRate : (lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate));
	
	if (lpObj->ExcellentDamageOnly)
	{
		pMsg.ExcellentDamageRate = 100.0f;
	}
	else
	{
		pMsg.ExcellentDamageRate = (float)(((lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate) > gServerInfo.m_ExcellentDamageRate) ? gServerInfo.m_ExcellentDamageRate : (lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate));
	}
	
	pMsg.DoubleDamageRate = (float)lpObj->m_MPSkillOpt.DoubleDamageRate;
	pMsg.TripleDamageRate = (float)lpObj->m_MPSkillOpt.TripleDamageRate;
	pMsg.IgnoreDefenseRate = (float)(lpObj->m_MPSkillOpt.IgnoreDefenseRate + lpObj->EffectOption.AddIgnoreDefenseRate);
	pMsg.FullDamageReflectRate = (float)lpObj->m_MPSkillOpt.FullDamageReflectRate;

	pMsg.SkillDamageBonus = (short)lpObj->SkillDamageBonus;

	pMsg.CriticalDamage = (short)(lpObj->CriticalDamagePerc);
	pMsg.ExcellentDamage = (short)(lpObj->ExcellentDamagePerc);

	pMsg.DamageIncreaseRate = (short)(lpObj->DamageIncRate + lpObj->AddGeneralDamageRate + lpObj->EffectOption.IncreasePvPDamage);
	
	short DamageReduction = (lpObj->ReduceGeneralDamageRate + lpObj->EffectOption.DecreasePvPDamage + lpObj->DecDamageByShield);
	pMsg.FinalDamageReductionRate = (BYTE)DamageReduction;

	DamageReduction = 0;

	for (int n = 0; n < MAX_DAMAGE_REDUCTION; n++) { DamageReduction += lpObj->DamageReduction[n]; }
	DamageReduction += lpObj->EffectOption.AddDamageReduction;
	DamageReduction = ((DamageReduction > gServerInfo.m_DamageReduction) ? gServerInfo.m_DamageReduction : DamageReduction);

	pMsg.DamageReduction[0] = LOBYTE(DamageReduction);
	pMsg.DamageReduction[1] = HIBYTE(DamageReduction);

	pMsg.DamageReflect = (BYTE)(((lpObj->DamageReflect + lpObj->EffectOption.AddDamageReflect) > gServerInfo.m_DamageReflect) ? gServerInfo.m_DamageReflect : (lpObj->DamageReflect + lpObj->EffectOption.AddDamageReflect));

	pMsg.DamageReductionRate = (float)lpObj->DamageDecRate;

	pMsg.HPRecovery = (float)(lpObj->m_MPSkillOpt.HPRecoveryRate + lpObj->EffectOption.AddHPRecoveryRate);

	pMsg.FullHPRestoreRate = (float)(lpObj->m_MPSkillOpt.DefensiveFullMPRestoreRate + lpObj->m_MPSkillOpt.OffensiveFullHPRestoreRate);

	pMsg.AbsorbLife = lpObj->AbsorbLife;

	pMsg.ANULADO = 0.0f;  //983 - buga quando zera saporra
	pMsg.HP50percDmgRate = lpObj->UltraHit50perChance; //50% HP 999
	pMsg.TripleDamageResist = lpObj->ResistTripleDamageRate; //998
	pMsg.DoubleDamageResist = lpObj->ResistDoubleDamageRate; //984
	pMsg.HP30percDmgRate = (float)lpObj->UltraHit30perChance; //997.0f; //Algo de boss
	
	pMsg.AddBossDamage = (float)(lpObj->AddBossDamage + lpObj->EffectOption.IncreaseBossDamage); //996.0f;
	pMsg.DecBossHitDelay = (float)((lpObj->DecBossHitDelay + lpObj->EffectOption.DecreaseBossHitDelay) < 70 ? (lpObj->DecBossHitDelay + lpObj->EffectOption.DecreaseBossHitDelay) : 70); //994.0f;
	pMsg.RedBossDamage = (float)(lpObj->DecBossDamage + lpObj->EffectOption.DecreaseBossDamage); //993.0f;
	pMsg.ResistBossSkills = (float)lpObj->BossSkillsResistance + lpObj->EffectOption.IncreaseBossSkillResistance; //992.0f; //Algo de boss
	pMsg.DoubleDamageBoss = (float)(lpObj->BossDoubleDamageRate + lpObj->EffectOption.BossDoubleDamage);//lpObj->BossDropIncrease + lpObj->EffectOption.BossDropIncrease; //
	pMsg.EraDoubleEnaoFunfa = (short)999; //990.0f; //disponivel
	
	pMsg.SDAttack1 = 989.0f; //disponivel
	pMsg.IgnoreShieldGaugeRate = 988.0f; //disponivel
	pMsg.StunRate = 987.0f; //disponivel
	pMsg.ResistStunRate = 986.0f; //disponivel
	pMsg.MoneyAmountDropRate = 985.0f; //disponivel	
	pMsg.BPConsumptionRate = 995.0f; //disponivel
	
	pMsg.Defense = lpObj->ImpenetrableDefense;

	pMsg.ShieldDamageReduction = (float)lpObj->ShieldDamageReduction;

	pMsg.BlockRate = lpObj->BlockRate;
	pMsg.ParryRate = lpObj->ParryRate;	

	pMsg.unk_data_1 = (short)1;
	pMsg.unk_data_2 = (short)2;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(PMSG_SXCHARINFO));
}

void EventIcon(BYTE type, BYTE Switch)
{
	PMSG_TIME_ICON_SEND	ICpMsg;
	ICpMsg.header.set(0xF1, sizeof(ICpMsg));
	ICpMsg.sub = 0xFE;
	ICpMsg.type = type;
	ICpMsg.Switch = Switch;
	DataSendAll((BYTE*)&ICpMsg, ICpMsg.header.size);
}