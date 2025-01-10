#pragma once

#include "User.h"

#define MAX_CUSTOM_EFFECT 10

#define CUSTOM_EFFECT_RANGE(x) (((x)<0)?0:((x)>=MAX_CUSTOM_EFFECT)?0:1)

enum eCustomEffectNumber
{
	CUSTOM_EFFECT_ReduceDamage = 1,
	CUSTOM_EFFECT_IncreaseDamage,
	CUSTOM_EFFECT_DecreaseBossDelay
};

struct CUSTOM_EFFECT_INFO
{
	int EffectIndex;
	int EffectType[3];
	int EffectValue[3];
};

class CCustomEffectManager
{
public:
	CCustomEffectManager();
	virtual ~CCustomEffectManager();
	void Init();
	void Load(char* path);
	void CheckCustomEffects(LPOBJ lpObj, int type1);
	CUSTOM_EFFECT_INFO m_CustomEffectInfo[MAX_CUSTOM_EFFECT];
};

extern CCustomEffectManager gCustomEffectManager;
