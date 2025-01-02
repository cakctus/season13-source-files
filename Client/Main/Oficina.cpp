#include "stdafx.h"
#include "Oficina.h"
#include "Console.h"
#include "Interface.h"

Oficina gOficina;

Oficina::Oficina()
{

}

void Oficina::ClearData()
{
	ZeroMemory(&descricao, sizeof(descricao));
	ZeroMemory(&itemName, sizeof(itemName));
	ZeroMemory(&selectedOptionCode, sizeof(selectedOptionCode));
	itemIndex = -1;
	itemType = -1;

	for (int i = 0; i < MAX_OPTION_INFO; i++)
	{
		optionInfo[0][i].code = 0;
		optionInfo[0][i].icoin = 0;
		optionInfo[0][i].ruud = 0;
		ZeroMemory(&optionInfo[0][i].name, sizeof(optionInfo[0][i].name));

		optionInfo[1][i].code = 0;
		optionInfo[1][i].icoin = 0;
		optionInfo[1][i].ruud = 0;
		ZeroMemory(&optionInfo[1][i].name, sizeof(optionInfo[1][i].name));
	}
}

void Oficina::DrawOficinaConfirmWindow()
{
	if (!gInterface.Data[OficinaConfirmBg].OnShow)
	{
		return;
	}

	gInterface.DrawGUI(OficinaConfirmBg, 218, 131);

	gInterface.DrawFormat(eGold, 245, 164, 166, eTxtAlign_center, 1, "OFICINA DE ITENS");

	gInterface.DrawItem(242, 209, 20, 20, this->itemIndex, SET_ITEMOPT_LEVEL(15), 1, 5, 0);

	gInterface.DrawFormat(eWhite, 277, 195, 145, eTxtAlign_center, 1, "CONFIRMACAO DE COMPRA");

	gInterface.DrawFormat(eAncient, 285, 210, 145, eTxtAlign_left, 0, "Seu novo item tera:");

	int theY = 220;

	for (int i = 0; i < 5; i++)
	{
		if (this->selectedOptionCode[i] == this->originalOptionCode[i])
		{
			gInterface.DrawFormat(eDarkBlue, 277, theY, 145, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[i], (i == 4)));
		}
		else
		{
			gInterface.DrawFormat(eShinyGreen, 277, theY, 145, eTxtAlign_center, 1, "[+] %s", this->GetOptionName(this->selectedOptionCode[i], (i == 4)));
		}

		theY += 10;
	}

	char textBotao[32];

	if (this->payMethod == 1)
	{
		wsprintfA(textBotao, "A pagar: %d iCoin", this->currentIcoinPrice);
		gInterface.DrawGUIEx(BotaoConfirmBuyIcoin1, 363, 300);
		gInterface.DrawFormat(eShinyGreen, 364, 305, 60, eTxtAlign_center, 1, "PAGAR COM#i C o i n");
	}
	else
	{
		wsprintfA(textBotao, "A pagar: %d Ruud", this->currentRuudPrice);
		gInterface.DrawGUIEx(BotaoConfirmBuyRuud1, 363, 300);
		gInterface.DrawFormat(eShinyGreen, 364, 305, 60, eTxtAlign_center, 1, "PAGAR COM#R u u d");
	}

	gInterface.DrawFormat(eGold, 327, 285, 88, 3, 1, textBotao);

	gInterface.DrawGUI(BotaoCancelar1, 234, 300);
	gInterface.DrawFormat(eShinyGreen, 235, 312, 60, eTxtAlign_center, 1, "CANCELAR");

	if (gInterface.IsWorkZone(BotaoCancelar1))
	{
		if (gInterface.Data[BotaoCancelar1].OnClick)
		{
			gInterface.DrawGUIEx(BotaoCancelar3, 234, 300);
			gInterface.DrawFormat(eWhite180, 235, 312, 60, eTxtAlign_center, 1, "CANCELAR");
		}
		else {
			gInterface.DrawGUIEx(BotaoCancelar2, 234, 300);
			gInterface.DrawFormat(eGold, 235, 312, 60, eTxtAlign_center, 1, "CANCELAR");
		}

		gInterface.DrawToolTip(gInterface.Data[BotaoCancelar2].X - 16, gInterface.Data[BotaoCancelar2].Y - 13, "Cancelar a compra e voltar a oficina");
	}
	else if (this->payMethod == 2 && gInterface.IsWorkZone(BotaoConfirmBuyRuud1))
	{
		if (gInterface.Data[BotaoConfirmBuyRuud1].OnClick)
		{
			gInterface.DrawGUIEx(BotaoConfirmBuyRuud3, 363, 300);
			gInterface.DrawFormat(eWhite180, 364, 305, 60, eTxtAlign_center, 1, "PAGAR COM#R u u d");
		}
		else {
			gInterface.DrawGUIEx(BotaoConfirmBuyRuud2, 363, 300);
			gInterface.DrawFormat(eGold, 364, 305, 60, eTxtAlign_center, 1, "PAGAR COM#R u u d");
		}

		gInterface.DrawToolTip(gInterface.Data[BotaoConfirmBuyRuud1].X - 16, gInterface.Data[BotaoConfirmBuyRuud1].Y + 35, "Confirmar e pagar modificacoes com Ruud");
	}
	else if (this->payMethod == 1 && gInterface.IsWorkZone(BotaoConfirmBuyIcoin1))
	{
		if (gInterface.Data[BotaoConfirmBuyIcoin1].OnClick)
		{
			gInterface.DrawGUIEx(BotaoConfirmBuyIcoin3, 363, 300);
			gInterface.DrawFormat(eWhite180, 364, 305, 60, eTxtAlign_center, 1, "PAGAR COM#i C o i n");
		}
		else {
			gInterface.DrawGUIEx(BotaoConfirmBuyIcoin2, 363, 300);
			gInterface.DrawFormat(eGold, 364, 305, 60, eTxtAlign_center, 1, "PAGAR COM#i C o i n");
		}

		gInterface.DrawToolTip(gInterface.Data[BotaoConfirmBuyIcoin1].X - 16, gInterface.Data[BotaoConfirmBuyIcoin1].Y + 35, "Confirmar e pagar modificacoes com iCoin");
	}
}

char* Oficina::GetOptionName(int code, int type)
{
	for (int i = 0; i < MAX_OPTION_INFO; i++)
	{
		if (optionInfo[type][i].code == code)
		{
			return optionInfo[type][i].name;
		}
	}

	return "-";
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

void Oficina::UpdatePrices()
{
	this->currentIcoinPrice = this->currentRuudPrice = 0;

	if (CheckAllRepeatedOptions())
	{
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		if (this->originalOptionCode[i] != 254 && this->selectedOptionCode[i] != this->originalOptionCode[i])
		{
			this->currentIcoinPrice += this->GetOptionIcoinPrice(this->selectedOptionCode[i],0);
			this->currentRuudPrice += this->GetOptionRuudPrice(this->selectedOptionCode[i],0);
		}
	}

	if (this->originalOptionCode[4] != 254 && this->selectedOptionCode[4] != this->originalOptionCode[4])
	{
		this->currentIcoinPrice += this->GetOptionIcoinPrice(this->selectedOptionCode[4],1);
		this->currentRuudPrice += this->GetOptionRuudPrice(this->selectedOptionCode[4],1);
	}
}

int Oficina::GetNextCode(int code, int type)
{
	for (int i = 0; i < MAX_OPTION_INFO - 1; i++)
	{
		if (optionInfo[type][i].code == code)
		{
			if (optionInfo[type][i+1].code == 254)
				continue;

			if (optionInfo[type][i + 1].code != 0)
			{
				return optionInfo[type][i + 1].code;
			}
			else
			{
				return optionInfo[type][0].code;
			}
		}
	}

	return optionInfo[type][0].code;
}

int Oficina::GetPreviousCode(int code, int type)
{
	for (int i = 1; i < MAX_OPTION_INFO; i++)
	{
		if (optionInfo[type][i].code == code)
		{
			if (optionInfo[type][i - 1].code == 254)
				continue;

			return optionInfo[type][i - 1].code;
		}
	}

	for (int i = (MAX_OPTION_INFO-1); i > 0; i--)
	{
		if (optionInfo[type][i].code != 0)
		{
			if (optionInfo[type][i].code == 254)
				continue;

			return optionInfo[type][i].code;
		}
	}

	return optionInfo[type][0].code;
}

bool Oficina::CheckRepeatedOptions(int code)
{
	int count = 0;

	for (int i = 0; i < 4; i++)
	{
		if (this->selectedOptionCode[i] > 0 && this->selectedOptionCode[i] < 254 && this->selectedOptionCode[i] == code)
		{
			count++;
		}
	}

	if (count > 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Oficina::CheckNonDesignedOption(int code, int type)
{
	if(code == 254)
		return true;

	return false;
}

bool Oficina::CheckAllRepeatedOptions()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
			{
				continue;
			}

			if (this->selectedOptionCode[i] > 0 && this->selectedOptionCode[i] < 254 && this->selectedOptionCode[i] == this->selectedOptionCode[j])
			{
				//Log.ConsoleOutPut(1, c_Red, t_Default, "Repetida %d %d | %d %d",i,j, this->selectedOptionCode[i], this->selectedOptionCode[j]);
				return true;
			}
		}
	}

	return false;
}

bool Oficina::CheckValidOption(int code, int type)
{
	if (code <= 0)
	{
		return false;
	}

	for (int i = 0; i < MAX_OPTION_INFO; i++)
	{
		if (optionInfo[type][i].code == code)
		{
			return true;
		}
	}

	return false;
}

void Oficina::ReceiveOficinaData(PMSG_OFICINA_OPEN* lpMsg)
{
	ClearData();

	strcpy_s(descricao, lpMsg->description);
	strcpy_s(itemName, lpMsg->itemName);
	itemIndex = lpMsg->itemIndex;
	itemType = lpMsg->type;

	this->selectedOptionCode[0] = this->originalOptionCode[0] = lpMsg->options[0];
	this->selectedOptionCode[1] = this->originalOptionCode[1] = lpMsg->options[1];
	this->selectedOptionCode[2] = this->originalOptionCode[2] = lpMsg->options[2];
	this->selectedOptionCode[3] = this->originalOptionCode[3] = lpMsg->options[3];
	this->selectedOptionCode[4] = this->originalOptionCode[4] = lpMsg->options[4];

	for (int i = 0; i < MAX_OPTION_INFO; i++)
	{
		if (lpMsg->optionCodes1[i] != 0)
		{
			optionInfo[0][i].code = lpMsg->optionCodes1[i];
			optionInfo[0][i].icoin = lpMsg->optionIcoin1[i];
			optionInfo[0][i].ruud = lpMsg->optionRuud1[i];
			strcpy_s(optionInfo[0][i].name, lpMsg->optionNames1[i]);
		}

		if (lpMsg->optionCodes2[i] != 0)
		{
			optionInfo[1][i].code = lpMsg->optionCodes2[i];
			optionInfo[1][i].icoin = lpMsg->optionIcoin2[i];
			optionInfo[1][i].ruud = lpMsg->optionRuud2[i];
			strcpy_s(optionInfo[1][i].name, lpMsg->optionNames2[i]);
		}
	}

	pSetCursorFocus = true;
	gInterface.Data[OficinaBg].OnShow = true;
	gInterface.Data[OficinaBg].EventTick = GetTickCount();
	gInterface.ToggleSceneObjects(1);
}

void Oficina::DrawMasteryOptions()
{
	int StartY = 180;
	int OptionTextColor;
	
	gInterface.DrawFormat(eWhite, 301, StartY, 167, eTxtAlign_center, 0, "Opcoes de Maestria");

	StartY += 15;

	//Option 1
	if (!this->CheckValidOption(this->selectedOptionCode[0],0)) //disabled
	{
		gInterface.DrawGUI(SetaEsq2_1, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUI(SetaDir2_1, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUI(SetaEsq1_1, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUI(SetaDir1_1, 466, StartY);

		if (this->CheckRepeatedOptions(this->selectedOptionCode[0]) || CheckNonDesignedOption(this->selectedOptionCode[0], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eBlueItemOption;
		}		

		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[0],0));
	}

	StartY += 20;

	//Option 2
	if (!this->CheckValidOption(this->selectedOptionCode[1],0)) //disabled
	{
		gInterface.DrawGUI(SetaEsq2_2, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUI(SetaDir2_2, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUI(SetaEsq1_2, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUI(SetaDir1_2, 466, StartY);

		if (this->CheckRepeatedOptions(this->selectedOptionCode[1]) || CheckNonDesignedOption(this->selectedOptionCode[1], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eBlueItemOption;
		}

		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[1],0));
	}

	StartY += 20;

	//Option 3
	if (!this->CheckValidOption(this->selectedOptionCode[2],0)) //disabled
	{
		gInterface.DrawGUI(SetaEsq2_3, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUI(SetaDir2_3, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUI(SetaEsq1_3, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUI(SetaDir1_3, 466, StartY);
		if (this->CheckRepeatedOptions(this->selectedOptionCode[2]) || CheckNonDesignedOption(this->selectedOptionCode[2], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eBlueItemOption;
		}
		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[2],0));
	}

	StartY += 25;

	gInterface.DrawFormat(eMasteryBonus, 301, StartY, 167, 3, 0, "Bonus de Maestria");

	StartY += 15;

	//Bonus Maestria
	if (!this->CheckValidOption(this->selectedOptionCode[4],1)) //disabled
	{
		gInterface.DrawGUIEx(SetaEsq2_5, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUIEx(SetaDir2_5, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUIEx(SetaEsq1_5, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUIEx(SetaDir1_5, 466, StartY);

		if (CheckNonDesignedOption(this->selectedOptionCode[4], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eBlueItemOption;
		}

		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[4],1));
	}
}

void Oficina::DrawWingOptions()
{
	int StartY = 180;
	int OptionTextColor;

	gInterface.DrawFormat(eWhite, 301, StartY, 167, eTxtAlign_center, 0, "Opcoes Normais");

	StartY += 15;

	//Option 1
	if (!this->CheckValidOption(this->selectedOptionCode[0],0)) //disabled
	{
		gInterface.DrawGUI(SetaEsq2_1, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUI(SetaDir2_1, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUI(SetaEsq1_1, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUI(SetaDir1_1, 466, StartY);

		if (this->CheckRepeatedOptions(this->selectedOptionCode[0]) || CheckNonDesignedOption(this->selectedOptionCode[0], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eBlueItemOption;
		}

		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[0],0));
	}

	StartY += 20;

	//Option 2
	if (!this->CheckValidOption(this->selectedOptionCode[1],0)) //disabled
	{
		gInterface.DrawGUI(SetaEsq2_2, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUI(SetaDir2_2, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUI(SetaEsq1_2, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUI(SetaDir1_2, 466, StartY);

		if (this->CheckRepeatedOptions(this->selectedOptionCode[1]) || CheckNonDesignedOption(this->selectedOptionCode[1], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eBlueItemOption;
		}

		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[1],0));
	}

	StartY += 20;

	//Option 3
	if (!this->CheckValidOption(this->selectedOptionCode[2],0)) //disabled
	{
		gInterface.DrawGUI(SetaEsq2_3, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUI(SetaDir2_3, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUI(SetaEsq1_3, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUI(SetaDir1_3, 466, StartY);
		
		if (this->CheckRepeatedOptions(this->selectedOptionCode[2]) || CheckNonDesignedOption(this->selectedOptionCode[2], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eBlueItemOption;
		}

		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[2],0));
	}

	StartY += 20;

	//Option 4
	if (!this->CheckValidOption(this->selectedOptionCode[3],0)) //disabled
	{
		gInterface.DrawGUIEx(SetaEsq2_4, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUIEx(SetaDir2_4, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUIEx(SetaEsq1_4, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUIEx(SetaDir1_4, 466, StartY);
		
		if (this->CheckRepeatedOptions(this->selectedOptionCode[3]) || CheckNonDesignedOption(this->selectedOptionCode[3], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eBlueItemOption;
		}

		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[3],0));
	}

	StartY += 25;

	gInterface.DrawFormat(eWhite, 301, StartY, 167, 3, 0, "Opcao Elemental");

	StartY += 15;

	//Option Elemental
	if (!this->CheckValidOption(this->selectedOptionCode[4],1)) //disabled
	{
		gInterface.DrawGUIEx(SetaEsq2_5, 281, StartY);
		gInterface.DrawColoredGUI(TextBox, 301, StartY, eGray150);
		gInterface.DrawGUIEx(SetaDir2_5, 466, StartY);
		gInterface.DrawFormat(eGray200, 301, StartY + 4, 167, eTxtAlign_center, 0, "- i n d i s p o n i v e l -");
	}
	else
	{
		gInterface.DrawGUIEx(SetaEsq1_5, 281, StartY);
		gInterface.DrawGUI(TextBox, 301, StartY);
		gInterface.DrawGUIEx(SetaDir1_5, 466, StartY);

		if (CheckNonDesignedOption(this->selectedOptionCode[4], itemType))
		{
			OptionTextColor = eRed;
		}
		else
		{
			OptionTextColor = eMasteryBonus;
		}

		gInterface.DrawFormat(OptionTextColor, 301, StartY + 4, 167, eTxtAlign_center, 0, this->GetOptionName(this->selectedOptionCode[4],1));
	}
}

void Oficina::DrawOficinaPanelWindow()
{
	if (!gInterface.Data[OficinaBg].OnShow)
	{
		return;
	}

	gInterface.DrawGUI(OficinaBg, 115, 2);

	gInterface.DrawGUI(OficinaCloseButton1, 488, 28);

	//Alinhamento de texto
	// 4 = direita
	// 3 = centralizado
	// 0 = esquerda

	//Título
	gInterface.DrawFormat(eGold, 181, 33, 258, eTxtAlign_center, 1, "OFICINA DE ITENS");

	//INFORMAÇÕES
	gInterface.DrawFormat(eGold, 142, 66, 114, eTxtAlign_left, 1, "LEIA COM BASTANTE ATENCAO!");
	gInterface.DrawFormat(eWhite, 133, 82, 128, eTxtAlign_left, 0, descricao);

	gInterface.DrawFormat(eGold, 326, 66, 258, eTxtAlign_left, 1, "Item selecionado para modificar");

	gInterface.DrawFormat(eShinyGreen, 319, 82, 130, eTxtAlign_left, 1, itemName);

	//bool itemEffect = (MouseX >= 352 && MouseX <= 416 && MouseY >= 104 && MouseY <= 168) ? 1 : 0;

	gInterface.DrawItem(364, 115, 40, 40, itemIndex, SET_ITEMOPT_LEVEL(15), 1, 0, 0);

	//Options
	if (itemType == 0)
		this->DrawWingOptions();
	else
		this->DrawMasteryOptions();

	this->UpdatePrices();

	//Pagamento
	gInterface.DrawFormat(eShinyGreen, 305, 327, 82, eTxtAlign_center, 1, "FORMA DE PAGAMENTO");
	char icoinPrice[32];
	wsprintf(icoinPrice, "%d %s", this->currentIcoinPrice, "iCoins");
	gInterface.DrawFormat(eGold, 308, 343, 80, eTxtAlign_center, 1, icoinPrice);
	gInterface.DrawFormat(eShinyGreen, 306, 359, 80, eTxtAlign_center, 1, "ou");
	char ruudPrice[32];
	wsprintf(ruudPrice, "%d %s", this->currentRuudPrice, "Ruud");
	gInterface.DrawFormat(eGold, 308, 375, 80, eTxtAlign_center, 1, ruudPrice);

	//Botões
	if (this->currentRuudPrice > 0)
	{
		gInterface.DrawGUIEx(BotaoBuyRuud1, 410, 363);
		gInterface.DrawFormat(eShinyGreen, 411, 368, 60, eTxtAlign_center, 1, "PAGAR COM#R u u d");
	}
	else
	{
		gInterface.DrawColoredGUI(BotaoBuyRuud1, 410, 363, eGray150);
		gInterface.DrawFormat(eGray200, 411, 368, 60, eTxtAlign_center, 1, "PAGAR COM#R u u d");
	}

	if ((this->currentIcoinPrice > 0 || (this->currentIcoinPrice == 0 &&
		!CheckNonDesignedOption(this->selectedOptionCode[0], itemType) &&
		!CheckNonDesignedOption(this->selectedOptionCode[1], itemType) &&
		!CheckNonDesignedOption(this->selectedOptionCode[2], itemType) &&
		!CheckNonDesignedOption(this->selectedOptionCode[4], itemType))) && !this->CheckAllRepeatedOptions())
	{
		gInterface.DrawGUIEx(BotaoBuyIcoin1, 410, 329);
		gInterface.DrawFormat(eShinyGreen, 411, 334, 60, eTxtAlign_center, 1, "PAGAR COM#i C o i n s");
	}
	else
	{
		gInterface.DrawColoredGUI(BotaoBuyIcoin1, 410, 329, eGray150);
		gInterface.DrawFormat(eGray200, 411, 334, 60, eTxtAlign_center, 1, "PAGAR COM#i C o i n s");
	}

	if ((this->currentIcoinPrice > 0 || (this->currentIcoinPrice == 0 &&
		!CheckNonDesignedOption(this->selectedOptionCode[0], itemType) &&
		!CheckNonDesignedOption(this->selectedOptionCode[1], itemType) &&
		!CheckNonDesignedOption(this->selectedOptionCode[2], itemType) &&
		!CheckNonDesignedOption(this->selectedOptionCode[4], itemType))) 
		&& gInterface.IsWorkZone(BotaoBuyIcoin1))
	{
		if (gInterface.Data[BotaoBuyIcoin1].OnClick)
		{
			gInterface.DrawGUIEx(BotaoBuyIcoin3, 410, 329);
			gInterface.DrawFormat(eWhite180, 411, 334, 60, eTxtAlign_center, 1, "PAGAR COM#i C o i n s");
		}
		else {
			gInterface.DrawGUIEx(BotaoBuyIcoin2, 410, 329);
			gInterface.DrawFormat(eGold, 411, 334, 60, eTxtAlign_center, 1, "PAGAR COM#i C o i n s");
		}

		gInterface.DrawToolTip(MouseX + 16, MouseY - 4, "Pagar modificacoes com iCoin");
	}
	else if (this->currentRuudPrice > 0 && gInterface.IsWorkZone(BotaoBuyRuud1))
	{
		if (gInterface.Data[BotaoBuyRuud1].OnClick)
		{
			gInterface.DrawGUIEx(BotaoBuyRuud3, 410, 363);
			gInterface.DrawFormat(eWhite180, 411, 368, 60, eTxtAlign_center, 1, "PAGAR COM#R u u d");
		}
		else {
			gInterface.DrawGUIEx(BotaoBuyRuud2, 410, 363);
			gInterface.DrawFormat(eGold, 411, 368, 60, eTxtAlign_center, 1, "PAGAR COM#R u u d");
		}

		gInterface.DrawToolTip(MouseX + 16, MouseY - 4, "Pagar modificacoes com Ruud");
	}
	else if (gInterface.IsWorkZone(OficinaCloseButton1))
	{
		DWORD Color = eGray100;

		if (gInterface.Data[OficinaCloseButton1].OnClick)
		{
			Color = eGray150;
		}

		gInterface.DrawColoredGUI(OficinaCloseButton1, gInterface.Data[OficinaCloseButton1].X, gInterface.Data[OficinaCloseButton1].Y, Color);
		gInterface.DrawToolTip(gInterface.Data[OficinaCloseButton1].X + 30, gInterface.Data[OficinaCloseButton1].Y + 8, "Fechar");
	}
	else
	{
		for (int i = SetaEsq1_1; i <= SetaEsq1_5; i++)
		{
			if (gInterface.IsWorkZone(i))
			{
				DWORD Color = eGray100;
				if (gInterface.Data[i].OnClick) { Color = eGray150; }

				gInterface.DrawColoredGUIEx(i, gInterface.Data[i].X, gInterface.Data[i].Y, Color);
				gInterface.DrawToolTip(gInterface.Data[i].X - 40, gInterface.Data[i].Y + 8, "Opcao Anterior");
			}
		}

		for (int i = SetaEsq2_1; i <= SetaEsq2_5; i++)
		{
			if (gInterface.IsWorkZone(i))
			{
				gInterface.DrawToolTip(gInterface.Data[i].X - 35, gInterface.Data[i].Y + 8, "Indisponivel");
			}
		}

		for (int i = SetaDir1_1; i <= SetaDir1_5; i++)
		{
			if (gInterface.IsWorkZone(i))
			{
				DWORD Color = eGray100;

				if (gInterface.Data[i].OnClick) { Color = eGray150; }

				gInterface.DrawColoredGUIEx(i, gInterface.Data[i].X, gInterface.Data[i].Y, Color);
				gInterface.DrawToolTip(gInterface.Data[i].X + 16, gInterface.Data[i].Y + 8, "Proxima Opcao");
			}
		}

		for (int i = SetaDir2_1; i <= SetaDir2_5; i++)
		{
			if (gInterface.IsWorkZone(i))
			{
				gInterface.DrawToolTip(gInterface.Data[i].X + 16, gInterface.Data[i].Y + 8, "Indisponivel");
			}
		}
	}
}

bool Oficina::EventWindowMain(DWORD Event)
{
	this->OficinaWindowClose(Event);
	this->OficinaSetas(Event);
	this->OficinaBuyIcoinButton(Event);
	this->OficinaBuyRuudButton(Event);
	this->OficinaCancelButton(Event);
	this->OficinaConfirmBuyIcoinButton(Event);
	this->OficinaConfirmBuyRuudButton(Event);
	return true;
}

bool Oficina::OficinaWindowClose(DWORD Event)
{
	if (!gInterface.Data[OficinaBg].OnShow || !gInterface.IsWorkZone(OficinaCloseButton1))
	{
		return false;
	}

	if (Event == WM_LBUTTONDOWN)
	{
		gInterface.Data[OficinaCloseButton1].OnClick = true;
		return true;
	}

	gInterface.Data[OficinaCloseButton1].OnClick = false;

	if ((GetTickCount() - gInterface.Data[OficinaCloseButton1].EventTick) < 500)
	{
		return false;
	}

	gInterface.Data[OficinaCloseButton1].EventTick = GetTickCount();

	this->CloseOficinaWindow();	

	return false;
}

bool Oficina::OficinaSetas(DWORD Event)
{
	if (!gInterface.Data[OficinaBg].OnShow || (GetTickCount() - gInterface.Data[OficinaBg].EventTick) < 1000)
	{
		return false;
	}

	for (int i = SetaEsq1_1; i < SetaDir2_5; i++)
	{
		if (gInterface.IsWorkZone(i))
		{
			if (Event == WM_LBUTTONDOWN)
			{
				gInterface.Data[i].OnClick = true;
				return true;
			}

			gInterface.Data[i].OnClick = false;

			if ((GetTickCount() - gInterface.Data[i].EventTick) < 200)
			{
				return false;
			}

			gInterface.Data[i].EventTick = GetTickCount();

			//realizar a operação
			if (i <= SetaEsq1_5 && this->selectedOptionCode[i - 10] > 0)
				this->selectedOptionCode[i - 10] = this->GetPreviousCode(this->selectedOptionCode[i - 10], ((i == SetaEsq1_5) ? 1 : 0));
			else if (i >= SetaDir1_1 && i <= SetaDir1_5)
				this->selectedOptionCode[i - 20] = this->GetNextCode(this->selectedOptionCode[i - 20], ((i == SetaDir1_5) ? 1 : 0));
		}
	}

	return false;
}

bool Oficina::OficinaBuyIcoinButton(DWORD Event)
{
	if (!gInterface.Data[OficinaBg].OnShow || !gInterface.IsWorkZone(BotaoBuyIcoin1))
	{
		return false;
	}

	if ((this->currentIcoinPrice == 0 &&
			(CheckNonDesignedOption(this->selectedOptionCode[0], itemType) ||
			CheckNonDesignedOption(this->selectedOptionCode[1], itemType) ||
			CheckNonDesignedOption(this->selectedOptionCode[2], itemType) ||
			CheckNonDesignedOption(this->selectedOptionCode[4], itemType))
			)
		)
	{
		return false;
	}

	if (Event == WM_LBUTTONDOWN)
	{
		gInterface.Data[BotaoBuyIcoin1].OnClick = true;
		return true;
	}

	gInterface.Data[BotaoBuyIcoin1].OnClick = false;

	if ((GetTickCount() - gInterface.Data[BotaoBuyIcoin1].EventTick) < 500)
	{
		return false;
	}

	gInterface.Data[BotaoBuyIcoin1].EventTick = GetTickCount();

	//ação de comprar por icoin
	this->payMethod = 1;
	gInterface.Data[OficinaBg].OnShow = false;
	gInterface.Data[OficinaConfirmBg].OnShow = true;
	gInterface.Data[OficinaConfirmBg].EventTick = GetTickCount();

	return false;
}

bool Oficina::OficinaBuyRuudButton(DWORD Event)
{
	if (!gInterface.Data[OficinaBg].OnShow || !gInterface.IsWorkZone(BotaoBuyRuud1) || this->currentRuudPrice <= 0)
	{
		return false;
	}

	if (Event == WM_LBUTTONDOWN)
	{
		gInterface.Data[BotaoBuyRuud1].OnClick = true;
		return true;
	}

	gInterface.Data[BotaoBuyRuud1].OnClick = false;

	if ((GetTickCount() - gInterface.Data[BotaoBuyRuud1].EventTick) < 500)
	{
		return false;
	}

	gInterface.Data[BotaoBuyRuud1].EventTick = GetTickCount();

	//ação de comprar por ruud
	this->payMethod = 2;
	gInterface.Data[OficinaBg].OnShow = false;
	gInterface.Data[OficinaConfirmBg].OnShow = true;
	gInterface.Data[OficinaConfirmBg].EventTick = GetTickCount();

	return false;
}

bool Oficina::OficinaCancelButton(DWORD Event)
{
	if (!gInterface.Data[OficinaConfirmBg].OnShow || !gInterface.IsWorkZone(BotaoCancelar1))
	{
		return false;
	}

	if (Event == WM_LBUTTONDOWN)
	{
		gInterface.Data[BotaoCancelar1].OnClick = true;
		return true;
	}

	gInterface.Data[BotaoCancelar1].OnClick = false;

	if ((GetTickCount() - gInterface.Data[BotaoCancelar1].EventTick) < 500)
	{
		return false;
	}

	gInterface.Data[BotaoCancelar1].EventTick = GetTickCount();

	gInterface.Data[OficinaConfirmBg].OnShow = false;

	gInterface.ResetObject(OficinaConfirmBg);
	gInterface.ResetObject(BotaoCancelar1);
	gInterface.ResetObject(BotaoCancelar2);
	gInterface.ResetObject(BotaoCancelar3);
	gInterface.ResetObject(BotaoConfirmBuyIcoin1);
	gInterface.ResetObject(BotaoConfirmBuyIcoin2);
	gInterface.ResetObject(BotaoConfirmBuyIcoin3);
	gInterface.ResetObject(BotaoConfirmBuyRuud1);
	gInterface.ResetObject(BotaoConfirmBuyRuud2);
	gInterface.ResetObject(BotaoConfirmBuyRuud3);

	gInterface.Data[OficinaBg].OnShow = true;

	return false;
}

bool Oficina::OficinaConfirmBuyIcoinButton(DWORD Event)
{
	if (!gInterface.Data[OficinaConfirmBg].OnShow || !gInterface.IsWorkZone(BotaoConfirmBuyIcoin1))
	{
		return false;
	}

	if (Event == WM_LBUTTONDOWN)
	{
		gInterface.Data[BotaoConfirmBuyIcoin1].OnClick = true;
		return true;
	}

	gInterface.Data[BotaoConfirmBuyIcoin1].OnClick = false;

	if ((GetTickCount() - gInterface.Data[BotaoConfirmBuyIcoin1].EventTick) < 500)
	{
		return false;
	}

	gInterface.Data[BotaoConfirmBuyIcoin1].EventTick = GetTickCount();

	//ENVIAR PACOTE
	Log.ConsoleOutPut(1, c_Green, t_Default, "Enviar pacote de compra por iCoin %d", payMethod);

	gInterface.Data[OficinaConfirmBg].OnShow = false;

	gInterface.ToggleSceneObjects(0);
	pSetCursorFocus = false;

	gInterface.ResetObject(OficinaConfirmBg);
	gInterface.ResetObject(BotaoCancelar1);
	gInterface.ResetObject(BotaoCancelar2);
	gInterface.ResetObject(BotaoCancelar3);
	gInterface.ResetObject(BotaoConfirmBuyIcoin1);
	gInterface.ResetObject(BotaoConfirmBuyIcoin2);
	gInterface.ResetObject(BotaoConfirmBuyIcoin3);
	gInterface.ResetObject(BotaoConfirmBuyRuud1);
	gInterface.ResetObject(BotaoConfirmBuyRuud2);
	gInterface.ResetObject(BotaoConfirmBuyRuud3);

	SendConfirmBuyPacket();

	return false;
}

bool Oficina::OficinaConfirmBuyRuudButton(DWORD Event)
{
	if (!gInterface.Data[OficinaConfirmBg].OnShow || !gInterface.IsWorkZone(BotaoConfirmBuyRuud1))
	{
		return false;
	}

	if (Event == WM_LBUTTONDOWN)
	{
		gInterface.Data[BotaoConfirmBuyRuud1].OnClick = true;
		return true;
	}

	gInterface.Data[BotaoConfirmBuyRuud1].OnClick = false;

	if ((GetTickCount() - gInterface.Data[BotaoConfirmBuyRuud1].EventTick) < 500)
	{
		return false;
	}

	gInterface.Data[BotaoConfirmBuyRuud1].EventTick = GetTickCount();

	//ENVIAR PACOTE
	Log.ConsoleOutPut(1, c_Green, t_Default, "Enviar pacote de compra por Ruud %d", payMethod);

	gInterface.Data[OficinaConfirmBg].OnShow = false;

	gInterface.ToggleSceneObjects(0);
	pSetCursorFocus = false;

	gInterface.ResetObject(OficinaConfirmBg);
	gInterface.ResetObject(BotaoCancelar1);
	gInterface.ResetObject(BotaoCancelar2);
	gInterface.ResetObject(BotaoCancelar3);
	gInterface.ResetObject(BotaoConfirmBuyIcoin1);
	gInterface.ResetObject(BotaoConfirmBuyIcoin2);
	gInterface.ResetObject(BotaoConfirmBuyIcoin3);
	gInterface.ResetObject(BotaoConfirmBuyRuud1);
	gInterface.ResetObject(BotaoConfirmBuyRuud2);
	gInterface.ResetObject(BotaoConfirmBuyRuud3);

	SendConfirmBuyPacket();

	return false;
}

void Oficina::CloseOficinaWindow()
{
	gInterface.Data[OficinaBg].OnShow = false;
	pSetCursorFocus = false;
	gInterface.ToggleSceneObjects(0);
	//*(BYTE*)0xE8C5AC = 1;

	gInterface.ResetObject(OficinaBg);
	gInterface.ResetObject(OficinaCloseButton1);
	gInterface.ResetObject(TextBox);
	gInterface.ResetObject(SetaEsq1_1);
	gInterface.ResetObject(SetaEsq1_2);
	gInterface.ResetObject(SetaEsq1_3);
	gInterface.ResetObject(SetaEsq1_4);
	gInterface.ResetObject(SetaEsq1_5);
	gInterface.ResetObject(SetaEsq2_1);
	gInterface.ResetObject(SetaEsq2_2);
	gInterface.ResetObject(SetaEsq2_3);
	gInterface.ResetObject(SetaEsq2_4);
	gInterface.ResetObject(SetaEsq2_5);
	gInterface.ResetObject(SetaDir1_1);
	gInterface.ResetObject(SetaDir1_2);
	gInterface.ResetObject(SetaDir1_3);
	gInterface.ResetObject(SetaDir1_4);
	gInterface.ResetObject(SetaDir1_5);
	gInterface.ResetObject(SetaDir2_1);
	gInterface.ResetObject(SetaDir2_2);
	gInterface.ResetObject(SetaDir2_3);
	gInterface.ResetObject(SetaDir2_4);
	gInterface.ResetObject(SetaDir2_5);
	gInterface.ResetObject(BotaoBuyIcoin1);
	gInterface.ResetObject(BotaoBuyIcoin2);
	gInterface.ResetObject(BotaoBuyIcoin3);
	gInterface.ResetObject(BotaoBuyRuud1);
	gInterface.ResetObject(BotaoBuyRuud2);
	gInterface.ResetObject(BotaoBuyRuud3);

	SendClosePacket();
};

void Oficina::SendClosePacket()
{
	PMSG_OFICINA_CLOSE_SEND pRequest;
	pRequest.h.set(0xF3, 0xEA, sizeof(pRequest));
	DataSend((LPBYTE)&pRequest, pRequest.h.size);
}

void Oficina::SendConfirmBuyPacket()
{
	PMSG_OFICINA_CONFIRM_BUY_SEND pMsg;
	pMsg.h.set(0xF3, 0xEB, sizeof(pMsg));

	pMsg.options[0] = selectedOptionCode[0];
	pMsg.options[1] = selectedOptionCode[1];
	pMsg.options[2] = selectedOptionCode[2];
	pMsg.options[3] = selectedOptionCode[3];
	pMsg.options[4] = selectedOptionCode[4];

	pMsg.payMethod = payMethod;

	DataSend((LPBYTE)&pMsg, pMsg.h.size);
}

void Oficina::CloseAndResetAll()
{
	gInterface.Data[OficinaConfirmBg].OnShow = false;

	gInterface.ToggleSceneObjects(0);
	pSetCursorFocus = false;

	gInterface.ResetObject(OficinaConfirmBg);
	gInterface.ResetObject(BotaoCancelar1);
	gInterface.ResetObject(BotaoCancelar2);
	gInterface.ResetObject(BotaoCancelar3);
	gInterface.ResetObject(BotaoConfirmBuyIcoin1);
	gInterface.ResetObject(BotaoConfirmBuyIcoin2);
	gInterface.ResetObject(BotaoConfirmBuyIcoin3);
	gInterface.ResetObject(BotaoConfirmBuyRuud1);
	gInterface.ResetObject(BotaoConfirmBuyRuud2);
	gInterface.ResetObject(BotaoConfirmBuyRuud3);

	CloseOficinaWindow();

	ClearData();
}