#include "stdafx.h"
#include "MysteriousStone.h"
#include "Notice.h"
#include "ItemManager.h"
#include "MasteryItemExtOptionSet.h"
#include "RandomManager.h"
#include "DSProtocol.h"
#include "Util.h"
#include "ServerInfo.h"

CMysteriousStone gMysteriousStone;

CMysteriousStone::CMysteriousStone() // OK
{

}

CMysteriousStone::~CMysteriousStone() // OK
{

}

bool CMysteriousStone::UseMyteriousStone(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (INVENTORY_FULL_RANGE(SourceSlot) == 0)
	{
		return 0;
	}

	if (INVENTORY_FULL_RANGE(TargetSlot) == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[SourceSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[TargetSlot].IsItem() == 0)
	{
		return 0;
	}

	if (gItemManager.InventoryAddItemStack(lpObj, SourceSlot, TargetSlot) != 0)
	{
		return 0;
	}

	if (gMasteryItemExtOptionSet.IsMasterySet(lpObj->Inventory[TargetSlot].m_Index) == 0 &&
		gMasteryItemExtOptionSet.IsMasteryWeapon(lpObj->Inventory[TargetSlot].m_Index) == 0 &&
		gMasteryItemExtOptionSet.IsMasteryRing(lpObj->Inventory[TargetSlot].m_Index) == 0 &&
		gMasteryItemExtOptionSet.IsMasteryPendant(lpObj->Inventory[TargetSlot].m_Index) == 0 &&
		gMasteryItemExtOptionSet.IsMasteryShield(lpObj->Inventory[TargetSlot].m_Index) == 0)
	{
		return 0;
	}

	if (gMasteryItemExtOptionSet.IsMasterySet(lpObj->Inventory[TargetSlot].m_Index))
	{
		return ApplyOnSet(lpObj, SourceSlot, TargetSlot);
	}
	else if (gMasteryItemExtOptionSet.IsMasteryWeapon(lpObj->Inventory[TargetSlot].m_Index))
	{
		return ApplyOnWeapon(lpObj, SourceSlot, TargetSlot);
	}
	else if (gMasteryItemExtOptionSet.IsMasteryShield(lpObj->Inventory[TargetSlot].m_Index))
	{
		return ApplyOnShield(lpObj, SourceSlot, TargetSlot);
	}
	else if (gMasteryItemExtOptionSet.IsMasteryRing(lpObj->Inventory[TargetSlot].m_Index))
	{
		return ApplyOnRing(lpObj, SourceSlot, TargetSlot);
	}
	else if (gMasteryItemExtOptionSet.IsMasteryPendant(lpObj->Inventory[TargetSlot].m_Index))
	{
		return ApplyOnPendant(lpObj, SourceSlot, TargetSlot);
	}

	return 0;
}

bool CMysteriousStone::ApplyOnSet(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (lpObj->Inventory[TargetSlot].IsSetItem() == false)
	{
		return 0;
	}

	if (ITEM_GET_INDEX(lpObj->Inventory[TargetSlot].m_Index) >= 164 && ITEM_GET_INDEX(lpObj->Inventory[TargetSlot].m_Index) <= 173)
	{
		return ApplyOnBlueEyeSet(lpObj, SourceSlot, TargetSlot);
	}

	if (GetLargeRand() % 100 > gServerInfo.m_MysteriouStoneSetApplyRate)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "A aplicacao de maestria falhou.");
		return 1;
	}

	CItem* lpItem = &lpObj->Inventory[TargetSlot];
	CItem itemOld = lpObj->Inventory[TargetSlot];
	CItem item;

	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] == 0xFF)
		{
			CRandomManager RandomOptions;

			for (int y = 6; y <= 15; y++)
			{
				bool jatem = false;

				for (int w = 0; w < 3; w++)
				{
					if (lpItem->m_SocketOption[w] == y)
					{
						jatem = true;
					}
				}

				if (jatem == false)
				{
					RandomOptions.AddElement(y, 100);
				}
			}

			RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[n]);

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida!");

			return 1;
		}
	}

	CRandomManager RandomOptions;

	if (lpItem->m_SocketOption[4] != 1)
		RandomOptions.AddElement(1, 200); //triplo

	if (lpItem->m_SocketOption[4] != 2)
		RandomOptions.AddElement(2, 300); //dano final

	if (lpItem->m_SocketOption[4] != 3)
		RandomOptions.AddElement(3, 100); //50% hp inimigo

	if (lpItem->m_SocketOption[4] != 4)
		RandomOptions.AddElement(4, 400); //double boss

	RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[4]);

	item = lpObj->Inventory[TargetSlot];

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida!");

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

	return 1;
}

bool CMysteriousStone::ApplyOnBlueEyeSet(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (GetLargeRand() % 100 > gServerInfo.m_MysteriouStoneBlueEyeSetApplyRate)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "A aplicacao de maestria falhou.");
		return 1;
	}

	CItem* lpItem = &lpObj->Inventory[TargetSlot];
	CItem itemOld = lpObj->Inventory[TargetSlot];
	CItem item;

	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] == 0xFF) //Slot vazio
		{
			CRandomManager RandomOptions;

			for (int y = 46; y <= 55; y++)
			{
				bool jatem = false;

				for (int w = 0; w < 3; w++)
				{
					if (lpItem->m_SocketOption[w] == y)
					{
						jatem = true;
					}
				}

				if (jatem == false)
				{
					RandomOptions.AddElement(y, 100);
				}
			}

			RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[n]);

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida!");

			return 1;
		}
		else if (lpItem->m_SocketOption[n] >= 6 && lpItem->m_SocketOption[n] <= 15) //já tem opcional antigo
		{
			lpItem->m_SocketOption[n] += 40;

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria %d evoluida com sucesso!", (n+1));

			return 1;
		}
	}

	CRandomManager RandomOptions;

	if (lpItem->m_SocketOption[4] >= 1 && lpItem->m_SocketOption[4] <= 4) //tem option antigo
	{
		lpItem->m_SocketOption[4] += 9; //evolui pro add do blue eye
	}
	else
	{
		if (lpItem->m_SocketOption[4] != 10)
			RandomOptions.AddElement(10, 300); //triplo

		if (lpItem->m_SocketOption[4] != 11)
			RandomOptions.AddElement(11, 100); //dano final

		if (lpItem->m_SocketOption[4] != 12)
			RandomOptions.AddElement(12, 200); //50% hp inimigo

		if (lpItem->m_SocketOption[4] != 13)
			RandomOptions.AddElement(13, 400); //double boss

		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[4]);
	}

	item = lpObj->Inventory[TargetSlot];

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de bonus maestria bem sucedida!");

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

	return 1;
}

bool CMysteriousStone::ApplyOnWeapon(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(0,78) ||  //bk sword
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(0, 79) || //mg sword
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(0, 80) || //rf claw
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(2, 40) || //dl scepter
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(3, 29) || //gl lance
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(4, 40) || //elf bow
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(4, 41) || //elf quiver
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(5, 61) || //dw staff
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(5, 62) || //su stick
		lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(5, 63)    //su book
		)
	{
		return ApplyOnBlueEyeWeapon(lpObj, SourceSlot, TargetSlot);
	}

	if (GetLargeRand() % 100 > gServerInfo.m_MysteriouStoneWeaponApplyRate)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "A aplicacao de maestria falhou.");
		return 1;
	}

	CItem* lpItem = &lpObj->Inventory[TargetSlot];
	CItem itemOld = lpObj->Inventory[TargetSlot];
	CItem item;

	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] < 8 || lpItem->m_SocketOption[n] == 0xFF)
		{
			CRandomManager RandomOptions;

			for (int y = 8; y <= 15; y++)
			{
				bool jatem = false;

				for (int w = 0; w < 3; w++)
				{
					if (lpItem->m_SocketOption[w] == y)
					{
						jatem = true;
					}
				}

				if (jatem == false)
				{
					RandomOptions.AddElement(y, 100);
				}
			}

			RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[n]);

			lpItem->m_SocketOption[3] = 255;
			lpItem->m_SocketOptionBonus = 0;

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida! =)");

			return 1;
		}
	}

	CRandomManager RandomOptions;

	if (lpItem->m_SocketOption[4] != MASTERY_BONUS_ULTRAHIT_50_WEAPON)
		RandomOptions.AddElement(MASTERY_BONUS_ULTRAHIT_50_WEAPON, 10); //50 porc

	if (lpItem->m_SocketOption[4] != MASTERY_BONUS_100P_EXCELLENT)
		RandomOptions.AddElement(MASTERY_BONUS_100P_EXCELLENT, 30); //100% exc

	if (lpItem->m_SocketOption[4] != MASTERY_BONUS_REFLECT_DECREASE)
		RandomOptions.AddElement(MASTERY_BONUS_REFLECT_DECREASE, 20); //abs reflect

	if (lpItem->m_SocketOption[4] != MASTERY_BONUS_CURE_HARM)
		RandomOptions.AddElement(MASTERY_BONUS_CURE_HARM, 10); //prejudica cura

	if (lpItem->m_SocketOption[4] != MASTERY_BONUS_STERN_RATE)
		RandomOptions.AddElement(MASTERY_BONUS_STERN_RATE, 10); //stern

	if (lpItem->m_SocketOption[4] != MASTERY_BONUS_BLEEDING)
		RandomOptions.AddElement(MASTERY_BONUS_BLEEDING, 10); //sangramento

	if (lpItem->m_SocketOption[4] != MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON)
		RandomOptions.AddElement(MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON, 30); //duplo boss

	RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[4]);

	lpItem->m_SocketOption[3] = 255;
	lpItem->m_SocketOptionBonus = 0;

	item = lpObj->Inventory[TargetSlot];

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida! =)");

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

	return 1;	
}

bool CMysteriousStone::ApplyOnShield(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (lpObj->Inventory[TargetSlot].m_Index == GET_ITEM(6, 48)) //blue eye
	{
		return ApplyOnBlueEyeShield(lpObj, SourceSlot, TargetSlot);
	}

	if (GetLargeRand() % 100 > gServerInfo.m_MysteriouStoneShieldApplyRate)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "A aplicacao de maestria falhou.");
		return 1;
	}

	CItem* lpItem = &lpObj->Inventory[TargetSlot];
	CItem itemOld = lpObj->Inventory[TargetSlot];
	CItem item;	

	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] < 20 || lpItem->m_SocketOption[n] == 0xFF)
		{
			CRandomManager RandomOptions;

			for (int y = 20; y <= 25; y++)
			{
				bool jatem = false;

				for (int w = 0; w < 3; w++)
				{
					if (lpItem->m_SocketOption[w] == y)
					{
						jatem = true;
					}
				}

				if (jatem == false)
				{
					RandomOptions.AddElement(y, 100);
				}
			}

			RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[n]);

			lpItem->m_SocketOption[3] = 255;
			lpItem->m_SocketOptionBonus = 0;

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida! =)");

			return 1;
		}
	}

	CRandomManager RandomOptions;

	if (lpItem->m_SocketOption[4] != 7)
		RandomOptions.AddElement(7, 15); //Protecao contra Dano Direto no HP

	if (lpItem->m_SocketOption[4] != 8)
		RandomOptions.AddElement(8, 25); //Aumento no Dano Refletido

	if (lpItem->m_SocketOption[4] != 9)
		RandomOptions.AddElement(9, 20); //Reducao de poderes de BOSS

	RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[4]);

	lpItem->m_SocketOption[3] = 255;
	lpItem->m_SocketOptionBonus = 0;

	item = lpObj->Inventory[TargetSlot];

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida! =)");

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);
	
	return 1;
}

bool CMysteriousStone::ApplyOnRing(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (GetLargeRand() % 100 > gServerInfo.m_MysteriouStoneRingApplyRate)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "A aplicacao de maestria falhou.");
		return 1;
	}

	CItem* lpItem = &lpObj->Inventory[TargetSlot];
	CItem itemOld = lpObj->Inventory[TargetSlot];
	CItem item;
	
	CRandomManager RandomOptions;
	RandomOptions.AddElement(30, 10);
	RandomOptions.AddElement(31, 10);
	RandomOptions.AddElement(32, 10);
	RandomOptions.AddElement(33, 10);
	RandomOptions.AddElement(34, 10);
	RandomOptions.AddElement(35, 10);
	RandomOptions.AddElement(36, 10);
	RandomOptions.AddElement(37, 10);
	RandomOptions.AddElement(38, 10);
	RandomOptions.AddElement(39, 10);

	CRandomManager RandomBonus;
	RandomBonus.AddElement(20, 10);
	RandomBonus.AddElement(21, 10);
	RandomBonus.AddElement(22, 10);
	RandomBonus.AddElement(23, 10);
	RandomBonus.AddElement(24, 10);
	RandomBonus.AddElement(25, 10);

	int luckyNumber = GetLargeRand() % 100;

	if (luckyNumber < 20) //full 20%
	{
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[0]);
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[1]);
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[2]);
		lpItem->m_SocketOption[3] = 255;
		RandomBonus.GetRandomElement(&lpItem->m_SocketOption[4]);
		lpItem->m_SocketOptionBonus = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria Tier 4 aplicada");
	}
	else if (luckyNumber >= 20 && luckyNumber < 40) //3 add (20%)
	{
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[0]);
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[1]);
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[2]);
		lpItem->m_SocketOption[3] = 255;
		lpItem->m_SocketOption[4] = 0;
		lpItem->m_SocketOptionBonus = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria Tier 3 aplicada");
	}
	else if (luckyNumber >= 40 && luckyNumber < 60) //2 add (20%)
	{
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[0]);
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[1]);
		lpItem->m_SocketOption[2] = 255;
		lpItem->m_SocketOption[3] = 255;
		lpItem->m_SocketOption[4] = 0;
		lpItem->m_SocketOptionBonus = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria Tier 2 aplicada");
	}
	else //1 add (40%)
	{
		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[0]);
		lpItem->m_SocketOption[1] = 255;
		lpItem->m_SocketOption[2] = 255;
		lpItem->m_SocketOption[3] = 255;
		lpItem->m_SocketOption[4] = 0;
		lpItem->m_SocketOptionBonus = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria Tier 1 aplicada");
	}

	item = lpObj->Inventory[TargetSlot];

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);
	
	return 1;
}

bool CMysteriousStone::ApplyOnPendant(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (GetLargeRand() % 100 > gServerInfo.m_MysteriouStonePendantApplyRate)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "A aplicacao de maestria falhou.");
		return 1;
	}

	CItem* lpItem = &lpObj->Inventory[TargetSlot];
	CItem itemOld = lpObj->Inventory[TargetSlot];
	CItem item;

	CRandomManager RandomOptions2;
	RandomOptions2.AddElement(30, 10);
	RandomOptions2.AddElement(31, 10);
	RandomOptions2.AddElement(32, 10);
	RandomOptions2.AddElement(33, 10);
	RandomOptions2.AddElement(34, 10);
	RandomOptions2.AddElement(35, 10);
	RandomOptions2.AddElement(36, 10);

	int luckyNumber = GetLargeRand() % 100;

	if (luckyNumber < 20) //full 20%
	{
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[0]);
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[1]);
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[2]);
		lpItem->m_SocketOption[3] = 255;

		CRandomManager RandomBonus2;
		RandomBonus2.AddElement(20, 101);
		RandomBonus2.AddElement(21, 102);
		RandomBonus2.AddElement(22, 103);
		RandomBonus2.AddElement(23, 104);
		RandomBonus2.AddElement(24, 105);

		RandomBonus2.GetRandomElement(&lpItem->m_SocketOption[4]);

		lpItem->m_SocketOptionBonus = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria Tier 4 aplicada");
	}
	else if (luckyNumber >= 20 && luckyNumber < 40) //3 add (20%)
	{
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[0]);
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[1]);
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[2]);
		lpItem->m_SocketOption[3] = 255;
		lpItem->m_SocketOption[4] = 255;
		lpItem->m_SocketOptionBonus = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria Tier 3 aplicada");
	}
	else if (luckyNumber >= 40 && luckyNumber < 60) //2 add (20%)
	{
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[0]);
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[1]);
		lpItem->m_SocketOption[2] = 255;
		lpItem->m_SocketOption[3] = 255;
		lpItem->m_SocketOption[4] = 255;
		lpItem->m_SocketOptionBonus = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria Tier 2 aplicada");
	}
	else //1 add (40%)
	{
		RandomOptions2.GetRandomElementUnique(&lpItem->m_SocketOption[0]);
		lpItem->m_SocketOption[1] = 255;
		lpItem->m_SocketOption[2] = 255;
		lpItem->m_SocketOption[3] = 255;
		lpItem->m_SocketOption[4] = 255;
		lpItem->m_SocketOptionBonus = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria Tier 1 aplicada");
	}

	item = lpObj->Inventory[TargetSlot];

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

	return 1;
}

bool CMysteriousStone::ApplyOnBlueEyeWeapon(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (GetLargeRand() % 100 > gServerInfo.m_MysteriouStoneBlueEyeWeaponApplyRate)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "A aplicacao de maestria falhou.");
		return 1;
	}

	CItem* lpItem = &lpObj->Inventory[TargetSlot];
	CItem itemOld = lpObj->Inventory[TargetSlot];
	CItem item;

	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] < 8 || lpItem->m_SocketOption[n] == 0xFF)
		{
			CRandomManager RandomOptions;

			for (int y = 16; y <= 23; y++)
			{
				bool jatem = false;

				for (int w = 0; w < 3; w++)
				{
					if (lpItem->m_SocketOption[w] == y)
					{
						jatem = true;
					}
				}

				if (jatem == false)
				{
					RandomOptions.AddElement(y, 100);
				}
			}

			RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[n]);

			lpItem->m_SocketOption[3] = 255;
			lpItem->m_SocketOptionBonus = 0;

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida! =)");

			return 1;
		}
		else if (lpItem->m_SocketOption[n] >= 8 && lpItem->m_SocketOption[n] <= 15) //já tem opcional antigo
		{
			lpItem->m_SocketOption[n] += 8;

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria %d evoluida com sucesso!", (n + 1));

			return 1;
		}
	}

	if (lpItem->m_SocketOption[4] == 1 || (lpItem->m_SocketOption[4] >= 3 && lpItem->m_SocketOption[4] <= 7)) //tem option antigo
	{
		lpItem->m_SocketOption[4] += 7; //evolui pro add do blue eye
	}
	else
	{
		CRandomManager RandomOptions;

		if (lpItem->m_SocketOption[4] != MASTERY_BONUS_ULTRAHIT_50_WEAPON_2)
			RandomOptions.AddElement(MASTERY_BONUS_ULTRAHIT_50_WEAPON_2, 10); //50 porc

		if (lpItem->m_SocketOption[4] != MASTERY_BONUS_100P_EXCELLENT)
			RandomOptions.AddElement(MASTERY_BONUS_100P_EXCELLENT, 10); //100% exc

		if (lpItem->m_SocketOption[4] != MASTERY_BONUS_FREEZING_DAMAGE)
			RandomOptions.AddElement(MASTERY_BONUS_FREEZING_DAMAGE, 10); //dano congelante

		if (lpItem->m_SocketOption[4] != MASTERY_BONUS_REFLECT_DECREASE_2)
			RandomOptions.AddElement(MASTERY_BONUS_REFLECT_DECREASE_2, 10); //abs reflect

		if (lpItem->m_SocketOption[4] != MASTERY_BONUS_CURE_HARM_2)
			RandomOptions.AddElement(MASTERY_BONUS_CURE_HARM_2, 10); //prejudica cura

		if (lpItem->m_SocketOption[4] != MASTERY_BONUS_STERN_RATE_2)
			RandomOptions.AddElement(MASTERY_BONUS_STERN_RATE_2, 10); //stern

		if (lpItem->m_SocketOption[4] != MASTERY_BONUS_BLEEDING_2)
			RandomOptions.AddElement(MASTERY_BONUS_BLEEDING_2, 10); //sangramento

		if (lpItem->m_SocketOption[4] != MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON_2)
			RandomOptions.AddElement(MASTERY_BONUS_DOUBLE_DAMAGE_BOSS_WEAPON_2, 10); //duplo boss

		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[4]);
	}

	lpItem->m_SocketOption[3] = 255;
	lpItem->m_SocketOptionBonus = 0;

	item = lpObj->Inventory[TargetSlot];

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de bonus de maestria bem sucedida! =)");

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

	return 1;
}

bool CMysteriousStone::ApplyOnBlueEyeShield(LPOBJ lpObj, int SourceSlot, int TargetSlot) // OK
{
	if (GetLargeRand() % 100 > gServerInfo.m_MysteriouStoneBlueEyeWeaponApplyRate)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "A aplicacao de maestria falhou.");
		return 1;
	}

	CItem* lpItem = &lpObj->Inventory[TargetSlot];
	CItem itemOld = lpObj->Inventory[TargetSlot];
	CItem item;

	for (int n = 0; n < 3; n++)
	{
		if (lpItem->m_SocketOption[n] < 20 || lpItem->m_SocketOption[n] == 0xFF)
		{
			CRandomManager RandomOptions;

			for (int y = 20; y <= 25; y++)
			{
				bool jatem = false;

				for (int w = 0; w < 3; w++)
				{
					if (lpItem->m_SocketOption[w] == y)
					{
						jatem = true;
					}
				}

				if (jatem == false)
				{
					RandomOptions.AddElement(y, 100);
				}
			}

			RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[n]);

			lpItem->m_SocketOption[3] = 255;
			lpItem->m_SocketOptionBonus = 0;

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida! =)");

			return 1;
		}
		else if (lpItem->m_SocketOption[n] >= 20 && lpItem->m_SocketOption[n] <= 25) //já tem opcional antigo
		{
			lpItem->m_SocketOption[n] += 40;

			item = lpObj->Inventory[TargetSlot];

			GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Maestria %d evoluida com sucesso!", (n + 1));

			return 1;
		}
	}

	if ((lpItem->m_SocketOption[4] >= 7 && lpItem->m_SocketOption[4] <= 9)) //tem option antigo
	{
		lpItem->m_SocketOption[4] += 7; //evolui pro add do blue eye
	}
	else
	{
		CRandomManager RandomOptions;

		if (lpItem->m_SocketOption[4] != 14)
			RandomOptions.AddElement(14, 15); //Protecao contra Dano Direto no HP

		if (lpItem->m_SocketOption[4] != 15)
			RandomOptions.AddElement(15, 25); //Aumento no Dano Refletido

		if (lpItem->m_SocketOption[4] != 16)
			RandomOptions.AddElement(16, 20); //Reducao de poderes de BOSS

		RandomOptions.GetRandomElementUnique(&lpItem->m_SocketOption[4]);
	}

	lpItem->m_SocketOption[3] = 255;
	lpItem->m_SocketOptionBonus = 0;

	item = lpObj->Inventory[TargetSlot];

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Aplicacao de maestria bem sucedida! =)");

	GDItemTrackingSaveSend(itemOld, item, lpObj->Account, lpObj->Account, 1, 3);

	return 1;
}