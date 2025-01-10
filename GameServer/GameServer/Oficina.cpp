#include "stdafx.h"
#include "Oficina.h"
#include "Protocol.h"
#include "ItemManager.h"
#include "MasteryItemExtOptionSet.h"
#include "MemScript.h"
#include "Util.h"
#include "CashShop.h"
#include "Log.h"
#include "DSProtocol.h"
#include "RheaGold.h"
#include "ServerInfo.h"

Oficina gOficina;

Oficina::Oficina()
{

}

Oficina::~Oficina()
{

}

void Oficina::LoadDescriptions(char* path)
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

	ZeroMemory(&this->descriptions, sizeof(this->descriptions));

	int section = 0;

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

			int section = lpMemScript->GetNumber();
			strcpy_s(this->descriptions[section], lpMemScript->GetAsString());
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void Oficina::LoadOptions(char* path)
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

	ZeroMemory(&this->optionInfo, sizeof(this->optionInfo));

	int section = 0;

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();
			int count = 0;

			while (true)
			{				
				if (strcmp("end", lpMemScript->GetAsString()) == 0)
				{
					break;
				}

				optionInfo[section][count].id = lpMemScript->GetNumber();
				optionInfo[section][count].code = lpMemScript->GetAsNumber();
				optionInfo[section][count].icoin = lpMemScript->GetAsNumber();
				optionInfo[section][count].ruud = lpMemScript->GetAsNumber();
				strcpy_s(optionInfo[section][count].name, lpMemScript->GetAsString());

				count++;
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	LogAdd(LOG_BLUE, "[Oficina] Options loaded.");

	delete lpMemScript;
}

void Oficina::NpcOpen(LPOBJ lpObj)
{
	if (gServerInfo.m_OficinaSwitch == 0)
	{
		GCMessagePopupSend(lpObj, "Po, %s! Estamos fechados para alguns ajustes no momento.\nVolta mais tarde ai, blz? =)", lpObj->Name);
		return;
	}

	if (lpObj->Inventory[INVENTORY_WEAR_SIZE].IsItem() == 0)
	{
		GCMessagePopupSend(lpObj, "Salve salve, %s! Beleza?\nPor questoes de seguranca, o Admin so deixa eu ter acesso ao primeiro item do seu inventario.\nColoque o item que deseja trabalhar na primeira posicao do seu inventario, e me chama novamente.", lpObj->Name);
		return;
	}

	if (lpObj->PersonalCodeChecked == false)
	{
		GCMessagePopupSend(lpObj, "Voce nao pode usar a Oficina sem antes autenticar o PIN. Utilize o comando /pin seguido de seu PIN para faze-lo agora mesmo.");
		return;
	}

	CItem* lpItem = &lpObj->Inventory[INVENTORY_WEAR_SIZE];

	PMSG_OFICINA_OPEN pMsg;
	ZeroMemory(&pMsg, sizeof(pMsg));
	pMsg.h.set(0xF3, 0xE9, sizeof(PMSG_OFICINA_OPEN));

	if (lpItem->m_Index >= GET_ITEM(12, 414) && lpItem->m_Index <= GET_ITEM(12, 421))
	{
		pMsg.type = 0; //Wings/Capes

		pMsg.options[0] = lpItem->m_SocketOption[0];
		pMsg.options[1] = lpItem->m_SocketOption[1];
		pMsg.options[2] = lpItem->m_SocketOption[2];
		pMsg.options[3] = lpItem->m_SocketOption[3];
		pMsg.options[4] = lpItem->m_SocketOption[4];		
	}
	else if (gMasteryItemExtOptionSet.IsMasteryPendant(lpItem->m_Index))
	{
		pMsg.type = 10; //Pendant
		pMsg.options[0] = lpItem->m_SocketOption[0];
		pMsg.options[1] = lpItem->m_SocketOption[1];
		pMsg.options[2] = lpItem->m_SocketOption[2];
		pMsg.options[4] = lpItem->m_SocketOption[4];
	}
	else if (gMasteryItemExtOptionSet.IsMasteryRing(lpItem->m_Index))
	{
		pMsg.type = 8; //Rings
		pMsg.options[0] = lpItem->m_SocketOption[0];
		pMsg.options[1] = lpItem->m_SocketOption[1];
		pMsg.options[2] = lpItem->m_SocketOption[2];
		pMsg.options[4] = lpItem->m_SocketOption[4];
	}
	else if (gMasteryItemExtOptionSet.IsMasteryShield(lpItem->m_Index))
	{
		if (lpItem->m_Index == GET_ITEM(6, 48))
		{
			pMsg.type = 16; //Shield Blue Eye

			if (!CheckValidOption(16, lpItem->m_SocketOption[0]) || !CheckValidOption(16, lpItem->m_SocketOption[1]) || !CheckValidOption(16, lpItem->m_SocketOption[2]) || !CheckValidOption(17, lpItem->m_SocketOption[4]))
			{
				GCMessagePopupSend(lpObj, "O item que voce esta tentando modificar esta apropriado para modificacao, pois possui opcoes nao atualizadas.");
				return;
			}
		}
		else
		{
			pMsg.type = 6; //Shield até Soul
		}
		
		pMsg.options[0] = lpItem->m_SocketOption[0];
		pMsg.options[1] = lpItem->m_SocketOption[1];
		pMsg.options[2] = lpItem->m_SocketOption[2];
		pMsg.options[4] = lpItem->m_SocketOption[4];
	}
	else if (gMasteryItemExtOptionSet.IsMasteryWeapon(lpItem->m_Index))
	{
		if (lpItem->m_Index == GET_ITEM(0, 78) ||  //bk sword
			lpItem->m_Index == GET_ITEM(0, 79) || //mg sword
			lpItem->m_Index == GET_ITEM(0, 80) || //rf claw
			lpItem->m_Index == GET_ITEM(2, 40) || //dl scepter
			lpItem->m_Index == GET_ITEM(3, 29) || //gl lance
			lpItem->m_Index == GET_ITEM(4, 40) || //elf bow
			lpItem->m_Index == GET_ITEM(4, 41) || //elf quiver
			lpItem->m_Index == GET_ITEM(5, 61) || //dw staff
			lpItem->m_Index == GET_ITEM(5, 62) || //su stick
			lpItem->m_Index == GET_ITEM(5, 63)    //su book
			)
		{
			pMsg.type = 12; //Arma Blue Eye

			if (!CheckValidOption(12, lpItem->m_SocketOption[0]) || !CheckValidOption(12, lpItem->m_SocketOption[1]) || !CheckValidOption(12, lpItem->m_SocketOption[2]) || !CheckValidOption(13, lpItem->m_SocketOption[4]))
			{
				GCMessagePopupSend(lpObj, "O item que voce esta tentando modificar esta apropriado para modificacao, pois possui opcoes nao atualizadas.");
				return;
			}
		}
		else
		{
			pMsg.type = 4; //Arma até soul
		}
		
		pMsg.options[0] = lpItem->m_SocketOption[0];
		pMsg.options[1] = lpItem->m_SocketOption[1];
		pMsg.options[2] = lpItem->m_SocketOption[2];
		pMsg.options[4] = lpItem->m_SocketOption[4];
	}
	else if (gMasteryItemExtOptionSet.IsMasterySet(lpItem->m_Index))
	{
		if (ITEM_GET_INDEX(lpItem->m_Index) >= 164 && ITEM_GET_INDEX(lpItem->m_Index) <= 173)
		{
			pMsg.type = 14; //Set Blue Eye

			if (!CheckValidOption(14, lpItem->m_SocketOption[0]) || !CheckValidOption(14, lpItem->m_SocketOption[1]) || !CheckValidOption(14, lpItem->m_SocketOption[2]) || !CheckValidOption(15, lpItem->m_SocketOption[4]))
			{
				GCMessagePopupSend(lpObj, "O item que voce esta tentando modificar esta apropriado para modificacao, pois possui opcoes nao atualizadas.");
				return;
			}
		}
		else
		{
			pMsg.type = 2; //Set até Soul
		}
		
		if (lpItem->IsSetItem())
		{
			pMsg.options[0] = lpItem->m_SocketOption[0];
			pMsg.options[1] = lpItem->m_SocketOption[1];
			pMsg.options[2] = lpItem->m_SocketOption[2];
			pMsg.options[4] = lpItem->m_SocketOption[4];
		}
	}
	else
	{
		GCMessagePopupSend(lpObj, "O item na primeira posicao de seu inventario nao pode ser modificado na oficina de itens.\nSao aceitos apenas Asas/Capas Level 4, e Sets, Armas, Escudos, Rings ou Pendants com Maestrias.");
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_SHOP;
	lpObj->Interface.state = 0;

	gLog.Output(LOG_OFICINA, "--------------------------------------------------------");
	gLog.Output(LOG_OFICINA, "[%s][%s] Abrindo oficina com Type %d", lpObj->Account, lpObj->Name, pMsg.type);
	gLog.Output(LOG_OFICINA, "[%s][%s] Item: %s | Serial: %d", lpObj->Account, lpObj->Name, gItemManager.GetName(lpItem->m_Index), lpItem->m_Serial);
	gLog.Output(LOG_OFICINA, "[%s][%s] Options: [%3d] [%3d] [%3d] [%3d] [%3d]", lpObj->Account, lpObj->Name, lpItem->m_SocketOption[0], lpItem->m_SocketOption[1], lpItem->m_SocketOption[2], lpItem->m_SocketOption[3], lpItem->m_SocketOption[4]);
	gLog.Output(LOG_OFICINA, "--------------------------------------------------------");

	for (int i = 0; i < MAX_OPTION_INFO; i++)
	{
		if (optionInfo[pMsg.type][i].code != 0)
		{
			pMsg.optionCodes1[i] = optionInfo[pMsg.type][i].code;
			strcpy_s(pMsg.optionNames1[i], optionInfo[pMsg.type][i].name);
			pMsg.optionIcoin1[i] = optionInfo[pMsg.type][i].icoin;
			pMsg.optionRuud1[i] = optionInfo[pMsg.type][i].ruud;
		}

		if (optionInfo[pMsg.type+1][i].code != 0)
		{
			pMsg.optionCodes2[i] = optionInfo[pMsg.type+1][i].code;
			strcpy_s(pMsg.optionNames2[i], optionInfo[pMsg.type+1][i].name);
			pMsg.optionIcoin2[i] = optionInfo[pMsg.type+1][i].icoin;
			pMsg.optionRuud2[i] = optionInfo[pMsg.type+1][i].ruud;
		}
	}

	strcpy(pMsg.description, this->descriptions[pMsg.type/2]);
	strcpy(pMsg.itemName, gItemManager.GetName(lpItem->m_Index));
	pMsg.itemIndex = lpItem->m_Index;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(pMsg));

	lpObj->OficinaOpen = true;
	lpObj->OficinaItem = lpItem->m_Serial;
}

void Oficina::BuyReceived(int aIndex, PMSG_OFICINA_CONFIRM_BUY_SEND* pMsg)
{
	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];	

	if (lpObj->Inventory[INVENTORY_WEAR_SIZE].IsItem() == 0)
	{
		gLog.Output(LOG_OFICINA, "[%s][%s] Tentando modificar non-item", lpObj->Account, lpObj->Name);
		GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #001");
		CloseWindow(lpObj);
		return;
	}

	CItem* lpItem = &lpObj->Inventory[INVENTORY_WEAR_SIZE];

	if (lpItem->m_Serial != lpObj->OficinaItem)
	{
		gLog.Output(LOG_OFICINA, "[%s][%s] Serial diferente %d %d", lpObj->Account, lpObj->Name, lpItem->m_Serial, lpObj->OficinaItem);
		GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #002");
		CloseWindow(lpObj);
		return;
	}

	if (CheckRepeatedOption(pMsg->options))
	{
		gLog.Output(LOG_OFICINA, "[%s][%s] Tentando colocar option repetida", lpObj->Account, lpObj->Name);
		GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #009");
		CloseWindow(lpObj);
		return;
	}

	int type = -1;

	if (lpItem->m_Index >= GET_ITEM(12, 414) && lpItem->m_Index <= GET_ITEM(12, 421))
	{
		type = 0; //Wings/Capes
	}
	else if (gMasteryItemExtOptionSet.IsMasteryPendant(lpItem->m_Index))
	{
		type = 10; //Pendants
	}
	else if (gMasteryItemExtOptionSet.IsMasteryRing(lpItem->m_Index))
	{
		type = 8; //Rings
	}
	else if (gMasteryItemExtOptionSet.IsMasteryShield(lpItem->m_Index))
	{
		if (lpItem->m_Index == GET_ITEM(6, 48))
		{
			type = 16; //Shield Blue Eye
		}
		else
		{
			type = 6; //Shields ate Soul
		}		
	}
	else if (gMasteryItemExtOptionSet.IsMasteryWeapon(lpItem->m_Index))
	{
		if (lpItem->m_Index == GET_ITEM(0, 78) ||  //bk sword
			lpItem->m_Index == GET_ITEM(0, 79) || //mg sword
			lpItem->m_Index == GET_ITEM(0, 80) || //rf claw
			lpItem->m_Index == GET_ITEM(2, 40) || //dl scepter
			lpItem->m_Index == GET_ITEM(3, 29) || //gl lance
			lpItem->m_Index == GET_ITEM(4, 40) || //elf bow
			lpItem->m_Index == GET_ITEM(4, 41) || //elf quiver
			lpItem->m_Index == GET_ITEM(5, 61) || //dw staff
			lpItem->m_Index == GET_ITEM(5, 62) || //su stick
			lpItem->m_Index == GET_ITEM(5, 63)    //su book
			)
		{
			type = 12; //Arma Blue Eye
		}
		else
		{
			type = 4; //Weapons
		}		
	}
	else if (gMasteryItemExtOptionSet.IsMasterySet(lpItem->m_Index) && lpItem->IsSetItem())
	{
		if (ITEM_GET_INDEX(lpItem->m_Index) >= 164 && ITEM_GET_INDEX(lpItem->m_Index) <= 173)
		{
			type = 14; //Set Blue Eye
		}
		else
		{
			type = 2; //Sets
		}		
	}
	else
	{
		gLog.Output(LOG_OFICINA, "[%s][%s] Item invalido, Type nao encontrado", lpObj->Account, lpObj->Name);
		GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #003");
		CloseWindow(lpObj);
		return;
	}

	if (type < 0 || type >= MAX_OPTION_TYPE)
	{
		gLog.Output(LOG_OFICINA, "[%s][%s] Type invalido, %d", lpObj->Account, lpObj->Name, type);
		GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #004");
		CloseWindow(lpObj);
		return;
	}

	int icoinCost = 0;
	int ruudCost = 0;

	for (int i = 0; i < 5; i++)
	{
		if (type != 0 && i == 3)
		{
			continue;
		}
		
		if (type != 0)
		{
			if (lpItem->m_SocketOption[i] == 254)
			{
				continue;
			}
		}
		else
		{
			if (lpItem->m_SocketOption[i] == 254 || lpItem->m_SocketOption[i] == 234)
			{
				continue;
			}
		}

		if (pMsg->options[i] != lpItem->m_SocketOption[i])
		{
			if (i == 4)
			{
				if (!CheckValidOption(type + 1, pMsg->options[i]))
				{
					gLog.Output(LOG_OFICINA, "[%s][%s] Opção inválida para o Type %d : %d", lpObj->Account, lpObj->Name, (type+1), pMsg->options[i]);
					GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #007");
					CloseWindow(lpObj);
					return;
				}

				icoinCost += GetOptionIcoinPrice(pMsg->options[i], (type + 1));
				ruudCost  +=  GetOptionRuudPrice(pMsg->options[i], (type + 1));
			}
			else
			{
				if (!CheckValidOption(type, pMsg->options[i]))
				{
					gLog.Output(LOG_OFICINA, "[%s][%s] Opção inválida para o Type %d : %d", lpObj->Account, lpObj->Name, type, pMsg->options[i]);
					GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #008");
					CloseWindow(lpObj);
					return;
				}

				icoinCost += GetOptionIcoinPrice(pMsg->options[i], type);
				ruudCost  +=  GetOptionRuudPrice(pMsg->options[i], type);
			}
		}
	}

	CItem lpItemOriginal = lpObj->Inventory[INVENTORY_WEAR_SIZE];

	if (pMsg->payMethod == 1)
	{
		if (icoinCost == 0) //opcoes sem designacao
		{
			for (int i = 0; i < 5; i++)
			{
				if (type != 0 && i == 3)
				{
					continue;
				}
				
				if (lpItem->m_SocketOption[i] != 254)
				{
					gLog.Output(LOG_OFICINA, "[%s][%s] Valor zero com opcao de designacao pendente, %d", lpObj->Account, lpObj->Name, type);
					GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #010");
					CloseWindow(lpObj);
					return;
				}
			}

			gLog.Output(LOG_OFICINA, "------------------------------------------------------");
			gLog.Output(LOG_OFICINA, "[%s][%s] iniciando primeira designacao de item", lpObj->Account, lpObj->Name);

			gLog.Output(LOG_OFICINA, "[%s][%s] - Type: %d", lpObj->Account, lpObj->Name, type);
			gLog.Output(LOG_OFICINA, "[Item: %s] [ANTES] [%3d][%3d][%3d][%3d][%3d]",
				gItemManager.GetName(lpItem->m_Index),
				lpItem->m_SocketOption[0], lpItem->m_SocketOption[1], lpItem->m_SocketOption[2], lpItem->m_SocketOption[3], lpItem->m_SocketOption[4]);

			for (int i = 0; i < 5; i++)
			{
				if (type != 0 && i == 3)
					continue;

				if (pMsg->options[i] != lpItem->m_SocketOption[i])
				{
					lpItem->m_SocketOption[i] = pMsg->options[i];
				}
			}

			gItemManager.GCItemModifySend(lpObj->Index, INVENTORY_WEAR_SIZE);

			gLog.Output(LOG_OFICINA, "[Item: %s] [DPOIS] [%3d][%3d][%3d][%3d][%3d]",
				gItemManager.GetName(lpItem->m_Index),
				lpItem->m_SocketOption[0], lpItem->m_SocketOption[1], lpItem->m_SocketOption[2], lpItem->m_SocketOption[3], lpItem->m_SocketOption[4]);

			gLog.Output(LOG_OFICINA, "------------------------------------------------------");

			GDItemTrackingSaveSend(lpItemOriginal, lpObj->Inventory[INVENTORY_WEAR_SIZE], lpObj->Account, lpObj->Account, 6, type);

			CloseWindow(lpObj);

			GCMessagePopupSend(lpObj, "Prontinho, %s! Seu novo item esta agora pronto pra detonar!", lpObj->Name);
		}
		else if (icoinCost > 0)
		{
			gLog.Output(LOG_OFICINA, "------------------------------------------------------");
			gLog.Output(LOG_OFICINA, "[%s][%s] iniciando pagamento por iCoin: %d", lpObj->Account, lpObj->Name, icoinCost);
			lpObj->OficinaOptions[0] = pMsg->options[0];
			lpObj->OficinaOptions[1] = pMsg->options[1];
			lpObj->OficinaOptions[2] = pMsg->options[2];
			lpObj->OficinaOptions[3] = pMsg->options[3];
			lpObj->OficinaOptions[4] = pMsg->options[4];
			gCashShop.GDCashShopRecievePointSend(aIndex, (DWORD)&Oficina::OnBuyCallbackRecv, (DWORD)icoinCost, (DWORD)type);
		}
	}
	else if (pMsg->payMethod == 2)
	{
		if (ruudCost > 0 && lpObj->Ruud >= ruudCost)
		{
			gLog.Output(LOG_OFICINA, "------------------------------------------------------");
			gLog.Output(LOG_OFICINA, "[%s][%s] iniciando pagamento por Ruud: %d", lpObj->Account, lpObj->Name, ruudCost);

			lpObj->Ruud -= ruudCost;
			gRheaGold.GC_Notify_RuudUpdate(lpObj->Index, lpObj->Ruud, ruudCost, 0);

			gLog.Output(LOG_OFICINA, "[%s][%s] - Type: %d - Ruud: %d", lpObj->Account, lpObj->Name, type, ruudCost);
			gLog.Output(LOG_OFICINA, "[Item: %s] [ANTES] [%3d][%3d][%3d][%3d][%3d]", 
				gItemManager.GetName(lpItem->m_Index), 
				lpItem->m_SocketOption[0], lpItem->m_SocketOption[1], lpItem->m_SocketOption[2], lpItem->m_SocketOption[3], lpItem->m_SocketOption[4]);

			for (int i = 0; i < 5; i++)
			{
				if (type != 0 && i == 3)
					continue;

				if (pMsg->options[i] != lpItem->m_SocketOption[i])
				{
					lpItem->m_SocketOption[i] = pMsg->options[i];
				}
			}

			gItemManager.GCItemModifySend(lpObj->Index, INVENTORY_WEAR_SIZE);

			gLog.Output(LOG_OFICINA, "[Item: %s] [DPOIS] [%3d][%3d][%3d][%3d][%3d]",
				gItemManager.GetName(lpItem->m_Index),
				lpItem->m_SocketOption[0], lpItem->m_SocketOption[1], lpItem->m_SocketOption[2], lpItem->m_SocketOption[3], lpItem->m_SocketOption[4]);

			gLog.Output(LOG_OFICINA, "------------------------------------------------------");

			GDItemTrackingSaveSend(lpItemOriginal, lpObj->Inventory[INVENTORY_WEAR_SIZE], lpObj->Account, lpObj->Account, 6, type);

			CloseWindow(lpObj);

			switch (GetLargeRand() % 11)
			{
			case 0:
				GCMessagePopupSend(lpObj, "Caraca, %s! Arte inigualavel, hein?! Sua arma esta pronta. <3", lpObj->Name);
				break;
			case 1:
				GCMessagePopupSend(lpObj, "Forja mestra! A sua batalha aguarda o meu feito, %s. ;)", lpObj->Name);
				break;
			case 2:
				GCMessagePopupSend(lpObj, "Obra prima! Seu novo item de guerra reluz, %s.", lpObj->Name);
				break;
			case 3:
				GCMessagePopupSend(lpObj, "Craft impecavel! O item epico agora e seu, %s.", lpObj->Name);
				break;
			case 4:
				GCMessagePopupSend(lpObj, "Artesao supremo! Sua arma esta forjada, %s.", lpObj->Name);
				break;
			case 5:
				GCMessagePopupSend(lpObj, "Poder encantado! Seu item esta completo, %s.", lpObj->Name);
				break;
			case 6:
				GCMessagePopupSend(lpObj, "Habilidade unica! O guerreiro %s aguarda essa belezura.", lpObj->Name);
				break;
			case 7:
				GCMessagePopupSend(lpObj, "Forca moldada! Sua arma esta pronta, %s.", lpObj->Name);
				break;
			case 8:
				GCMessagePopupSend(lpObj, "Aco, ferro e magia! Sua criacao esta pronta, %s.", lpObj->Name);
				break;
			case 9:
				GCMessagePopupSend(lpObj, "Maestria revelada! O item esta em suas maos, %s.", lpObj->Name);
				break;
			case 10:
			default:
				GCMessagePopupSend(lpObj, "Seu item esta pronto, %s!\nAproveite o novo brinquedo <3", lpObj->Name);
				break;
			}
		}
		else
		{
			gLog.Output(LOG_OFICINA, "[%s][%s] Ruud zero ou insuficiente: [Custo %d] [Saldo %d]", lpObj->Account, lpObj->Name, ruudCost, lpObj->Ruud);
			GCMessagePopupSend(lpObj, "Oops!\nVoce nao tem o saldo de Ruud necessario para essa modificacao.");
			CloseWindow(lpObj);
		}
	}
	else
	{
		gLog.Output(LOG_OFICINA, "[%s][%s] Metodo de pagamento invalido: %d", lpObj->Account, lpObj->Name, pMsg->payMethod);
		CloseWindow(lpObj);
	}
}

void Oficina::OnBuyCallbackRecv(LPOBJ lpObj, DWORD icoinCost, DWORD type, DWORD WCoinC, DWORD WCoinP, DWORD GoblinPoint) // OK
{
	if (lpObj->Inventory[INVENTORY_WEAR_SIZE].IsItem() == 0)
	{
		GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #005");
		gOficina.CloseWindow(lpObj);
		return;
	}

	CItem* lpItem = &lpObj->Inventory[INVENTORY_WEAR_SIZE];

	if (lpItem->m_Serial != lpObj->OficinaItem)
	{
		gLog.Output(LOG_OFICINA, "[%s][%s] Serial diferente pós iCoins %d %d", lpObj->Account, lpObj->Name, lpItem->m_Serial, lpObj->OficinaItem);
		GCMessagePopupSend(lpObj, "Oops!\nAlgo de errado nao deu certo. :/\nErro: #006");
		gOficina.CloseWindow(lpObj);
		return;
	}

	if (WCoinC < icoinCost)
	{
		gLog.Output(LOG_OFICINA, "[%s][%s] iCoin insuficiente: [Custo %d] [Saldo %d]", lpObj->Account, lpObj->Name, icoinCost, WCoinC);
		GCMessagePopupSend(lpObj, "Oops!\nVoce nao tem o saldo de iCoin necessario para essa modificacao.");
		gOficina.CloseWindow(lpObj);
		return;
	}

	CItem lpItemOriginal = lpObj->Inventory[INVENTORY_WEAR_SIZE];

	
	gLog.Output(LOG_OFICINA, "[%s][%s] - Type: %d - iCoin: %d", lpObj->Account, lpObj->Name, type, icoinCost);
	gLog.Output(LOG_OFICINA, "[Item: %s] [ANTES] [%3d][%3d][%3d][%3d][%3d]",
		gItemManager.GetName(lpItem->m_Index),
		lpItem->m_SocketOption[0], lpItem->m_SocketOption[1], lpItem->m_SocketOption[2], lpItem->m_SocketOption[3], lpItem->m_SocketOption[4]);

	for (int i = 0; i < 5; i++)
	{
		if (type != 0 && i == 3)
			continue;

		if (lpObj->OficinaOptions[i] != lpItem->m_SocketOption[i])
		{
			lpItem->m_SocketOption[i] = lpObj->OficinaOptions[i];
		}
	}

	gItemManager.GCItemModifySend(lpObj->Index, INVENTORY_WEAR_SIZE);

	gLog.Output(LOG_OFICINA, "[Item: %s] [DPOIS] [%3d][%3d][%3d][%3d][%3d]",
		gItemManager.GetName(lpItem->m_Index),
		lpItem->m_SocketOption[0], lpItem->m_SocketOption[1], lpItem->m_SocketOption[2], lpItem->m_SocketOption[3], lpItem->m_SocketOption[4]);

	gLog.Output(LOG_OFICINA, "------------------------------------------------------");

	GDItemTrackingSaveSend(lpItemOriginal, lpObj->Inventory[INVENTORY_WEAR_SIZE], lpObj->Account, lpObj->Account, 6, type);

	gOficina.CloseWindow(lpObj);

	gCashShop.GDCashShopSubPointSaveSend(lpObj->Index, 0, icoinCost, 0, 0, "Oficina");

	GDCharacterInfoSaveSend(lpObj->Index);

	ZeroMemory(&lpObj->OficinaOptions, sizeof(lpObj->OficinaOptions));

	switch (GetLargeRand() % 11)
	{
		case 0:
			GCMessagePopupSend(lpObj, "Caraca, %s! Arte inigualavel, hein?! Sua arma esta pronta. <3", lpObj->Name);
			break;
		case 1:
			GCMessagePopupSend(lpObj, "Forja mestra! A sua batalha aguarda o meu feito, %s. ;)", lpObj->Name);
			break;
		case 2:
			GCMessagePopupSend(lpObj, "Obra prima! Seu novo item de guerra reluz, %s.", lpObj->Name);
			break;
		case 3:
			GCMessagePopupSend(lpObj, "Craft impecavel! O item epico agora e seu, %s.", lpObj->Name);
			break;
		case 4:
			GCMessagePopupSend(lpObj, "Artesao supremo! Sua arma esta forjada, %s.", lpObj->Name);
			break;
		case 5:
			GCMessagePopupSend(lpObj, "Poder encantado! Seu item esta completo, %s.", lpObj->Name);
			break;
		case 6:
			GCMessagePopupSend(lpObj, "Habilidade unica! O guerreiro %s aguarda essa belezura.", lpObj->Name);
			break;
		case 7:
			GCMessagePopupSend(lpObj, "Forca moldada! Sua arma esta pronta, %s.", lpObj->Name);
			break;
		case 8:
			GCMessagePopupSend(lpObj, "Aco, ferro e magia! Sua criacao esta pronta, %s.", lpObj->Name);
			break;
		case 9:
			GCMessagePopupSend(lpObj, "Maestria revelada! O item esta em suas maos, %s.", lpObj->Name);
			break;
		case 10:
		default:
			GCMessagePopupSend(lpObj, "Seu item esta pronto, %s!\nAproveite o novo brinquedo <3", lpObj->Name);
			break;
	}
}

bool Oficina::CheckValidOption(int type, int option)
{
	for (int i = 0; i < MAX_OPTION_INFO; i++)
	{
		if (optionInfo[type][i].code == option)
		{
			return true;
		}
	}
	return false;
}

bool Oficina::CheckRepeatedOption(BYTE option[5])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
			{
				continue;
			}

			if (option[i] > 0 && option[i] < 254 && option[i] == option[j])
			{
				return true;
			}
		}
	}

	return false;
}

void Oficina::CloseWindow(LPOBJ lpObj)
{
	lpObj->OficinaOpen = false;
	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
}

int Oficina::GetOptionRuudPrice(int code, int type)
{
	for (int i = 0; i < MAX_OPTION_INFO; i++)
	{
		if (optionInfo[type][i].code == code)
		{
			return optionInfo[type][i].ruud;
		}
	}
	return 200000000;
}

int Oficina::GetOptionIcoinPrice(int code, int type)
{
	for (int i = 0; i < MAX_OPTION_INFO; i++)
	{
		if (optionInfo[type][i].code == code)
		{
			return optionInfo[type][i].icoin;
		}
	}
	return 9999;
}