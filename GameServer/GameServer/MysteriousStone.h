#pragma once

#include "Item.h"
#include "User.h"

class CMysteriousStone
{
public:
	CMysteriousStone();
	virtual ~CMysteriousStone();

	bool UseMyteriousStone(LPOBJ lpObj, int SourceSlot, int TargetSlot);

	bool ApplyOnSet(LPOBJ lpObj, int SourceSlot, int TargetSlot);

	bool ApplyOnBlueEyeSet(LPOBJ lpObj, int SourceSlot, int TargetSlot);

	bool ApplyOnWeapon(LPOBJ lpObj, int SourceSlot, int TargetSlot);

	bool ApplyOnShield(LPOBJ lpObj, int SourceSlot, int TargetSlot);

	bool ApplyOnRing(LPOBJ lpObj, int SourceSlot, int TargetSlot);

	bool ApplyOnPendant(LPOBJ lpObj, int SourceSlot, int TargetSlot);

	bool ApplyOnBlueEyeWeapon(LPOBJ lpObj, int SourceSlot, int TargetSlot);

	bool ApplyOnBlueEyeShield(LPOBJ lpObj, int SourceSlot, int TargetSlot);

public:
	
};

extern CMysteriousStone gMysteriousStone;
