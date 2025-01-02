#include "stdafx.h"
#include "MemScript.h"
#include "RuudTokenRewardProbability.h"
#include "Util.h"
#include "RheaGold.h"

RuudTokenRewardProbability g_RuudTokenRewardProbability;

RuudTokenRewardProbability::RuudTokenRewardProbability()
{
	for (int i = 0; i < MAX_RUUD_PROBABILITIES; ++i)
	{
		this->RuudProbabilities[i].nRuudBoxBagNumber = 0;
		this->RuudProbabilities[i].nRuudInserCount = 0;
		this->RuudProbabilities[i].nRuudProbability = 0;
	}
}

RuudTokenRewardProbability::~RuudTokenRewardProbability()
{
}

//----- (00EA53B0) --------------------------------------------------------
void  RuudTokenRewardProbability::InsertRuudPoint(int aIndex, char byType)
{
	int i; // [sp+4Ch] [bp-Ch]@1
	int RuudDropRate; // [sp+50h] [bp-8h]@1

	RuudDropRate = GetLargeRand() % 0xF4240;

	for (i = 0; i < MAX_RUUD_PROBABILITIES; ++i)
	{
		if (this->RuudProbabilities[i].nRuudBoxBagNumber == (unsigned __int8)byType && this->RuudProbabilities[i].nRuudProbability >= RuudDropRate)
		{
			LogAdd(LOG_BLACK,"RuudProbability:%d, RuudDropRate:%d, EA:%d",this->RuudProbabilities[i].nRuudProbability,RuudDropRate,this->RuudProbabilities[i].nRuudInserCount);
			LogAdd(LOG_BLACK,"[RUUD][InsertRuudPoint] ACC:%s, Name:%s, Before:%d, After:%d, byItemBag:%d",gObj[aIndex].Account,gObj[aIndex].Name,gObj[aIndex].Ruud,this->RuudProbabilities[i].nRuudInserCount,(unsigned __int8)byType);
			gObj[aIndex].Ruud += this->RuudProbabilities[i].nRuudInserCount;
			gRheaGold.GD_RuudTokenCountUpdate(&gObj[aIndex], gObj[aIndex].Ruud);
			gRheaGold.GC_Notify_RuudUpdate(aIndex, gObj[aIndex].Ruud, this->RuudProbabilities[i].nRuudInserCount, 1);
			//GD_ReqSetRuudTokenLog(&gObj[aIndex],gObj[aIndex].Ruud,this->RuudProbabilities[i].nRuudInserCount,0);
			return;
		}
	}
}

//----- (00EA5610) --------------------------------------------------------
int  RuudTokenRewardProbability::GetSelectedRuudPoint(int aIndex, char byType)
{
	int i; // [sp+4Ch] [bp-10h]@1
	int RuudDropRate; // [sp+50h] [bp-Ch]@1
	int nSelectedRuudToken; // [sp+54h] [bp-8h]@1

	nSelectedRuudToken = 0;
	RuudDropRate = GetLargeRand() % 0xF4240;
	for (i = 0; i < MAX_RUUD_PROBABILITIES; ++i)
	{
		if (this->RuudProbabilities[i].nRuudBoxBagNumber == (unsigned __int8)byType
			&& this->RuudProbabilities[i].nRuudProbability >= RuudDropRate)
		{
			LogAdd(LOG_BLACK," ·çµåÈ®·ü(#2) nRuudProbability:%d,RuudDropRate:%d, EA:%d",this->RuudProbabilities[i].nRuudProbability,RuudDropRate,this->RuudProbabilities[i].nRuudInserCount);
			LogAdd(LOG_BLACK,"[RUUD][GetSelectedRuudPoint] ACC:%s, Name:%s, Current User Ruud:%d, Selected Ruud:%d, byItemBag:%d",gObj[aIndex].Account,gObj[aIndex].Name,gObj[aIndex].Ruud,this->RuudProbabilities[i].nRuudInserCount,(unsigned __int8)byType);
			return this->RuudProbabilities[i].nRuudInserCount;
		}
	}
	return nSelectedRuudToken;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00EA5780) --------------------------------------------------------
void  RuudTokenRewardProbability::Load_RuudBoxProbability(char* pchFileName)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, pchFileName);
		return;
	}

	if (lpMemScript->SetBuffer(pchFileName) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	try
	{
		int Count = 0;
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
			if (Count >= MAX_RUUD_PROBABILITIES)
			{
				break;;
			}

			this->RuudProbabilities[Count].nRuudBoxBagNumber	= lpMemScript->GetNumber();
			this->RuudProbabilities[Count].nRuudProbability		= lpMemScript->GetAsNumber();
			this->RuudProbabilities[Count].nRuudInserCount		= lpMemScript->GetAsNumber();

			Count++;
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

}