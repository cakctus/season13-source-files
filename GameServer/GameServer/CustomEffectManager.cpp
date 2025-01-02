#include "stdafx.h"
#include "MemScript.h"
#include "CustomEffectManager.h"
#include "Util.h"
#include "EffectManager.h"

CCustomEffectManager gCustomEffectManager;

CCustomEffectManager::CCustomEffectManager() // OK
{
	this->Init();
}

CCustomEffectManager::~CCustomEffectManager() // OK
{

}

void CCustomEffectManager::Init() // OK
{
	for (int n = 0; n < MAX_CUSTOM_EFFECT; n++)
	{
		this->m_CustomEffectInfo[n].EffectIndex = -1;
	}
}

void CCustomEffectManager::Load(char* path) // OK
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

	this->Init();

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

			this->m_CustomEffectInfo[count].EffectIndex = lpMemScript->GetNumber();
			
			this->m_CustomEffectInfo[count].EffectType[0] = lpMemScript->GetAsNumber();
			this->m_CustomEffectInfo[count].EffectValue[0] = lpMemScript->GetAsNumber();
			this->m_CustomEffectInfo[count].EffectType[1] = lpMemScript->GetAsNumber();
			this->m_CustomEffectInfo[count].EffectValue[1] = lpMemScript->GetAsNumber();
			this->m_CustomEffectInfo[count].EffectType[2] = lpMemScript->GetAsNumber();
			this->m_CustomEffectInfo[count].EffectValue[2] = lpMemScript->GetAsNumber();

			count++;
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CCustomEffectManager::CheckCustomEffects(LPOBJ lpObj, int type)
{
	lpObj->SpecialEffectDamageIncrease = 0;
	lpObj->SpecialEffectDamageDecrease = 0;

	for (int n = 0; n < MAX_CUSTOM_EFFECT; n++)
	{
		if (this->m_CustomEffectInfo[n].EffectIndex == -1)
		{
			return;
		}

		if (gEffectManager.CheckEffect(lpObj, this->m_CustomEffectInfo[n].EffectIndex))
		{
			switch (this->m_CustomEffectInfo[n].EffectType[type])
			{
			case CUSTOM_EFFECT_ReduceDamage:
				lpObj->SpecialEffectDamageDecrease += this->m_CustomEffectInfo[n].EffectValue[type];
				break;
			case CUSTOM_EFFECT_IncreaseDamage:
				lpObj->SpecialEffectDamageIncrease += this->m_CustomEffectInfo[n].EffectValue[type];
				break;
			case CUSTOM_EFFECT_DecreaseBossDelay:
				lpObj->DecBossHitDelay += this->m_CustomEffectInfo[n].EffectValue[type];
				break;
			}
		}
	}
}
