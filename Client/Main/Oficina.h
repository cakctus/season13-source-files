#pragma once

#include "Protocol.h"

#define MAX_OPTION_INFO 16
#define MAX_OPTION_TYPE 18

#define TOTAL_SET_OPTS 20
#define TOTAL_SET_BONUS 8

#define TOTAL_SHIELD_OPTS 12
#define TOTAL_SHIELD_BONUS 6

#define TOTAL_WEAPON_OPTS 16
#define TOTAL_WEAPON_BONUS 14

#define TOTAL_PENDANT_OPTS 5
#define TOTAL_PENDANT_BONUS 5

#define TOTAL_RING_OPTS 10
#define TOTAL_RING_BONUS 6

#define TOTAL_WING_NORMAL_OPTS 15
#define TOTAL_WING_ELEMENTAL_OPTS 14

struct PMSG_OFICINA_OPEN
{
	PSWMSG_HEAD	h;

	BYTE type;
	BYTE options[MAX_OPTION_TYPE]; //current item options

	int optionCodes1[MAX_OPTION_INFO]; //all available option 1 codes
	int optionCodes2[MAX_OPTION_INFO]; //all available option 2 codes
	char optionNames1[MAX_OPTION_INFO][64]; //option names 1
	char optionNames2[MAX_OPTION_INFO][64]; //option names 2
	int optionIcoin1[MAX_OPTION_INFO]; //option icoin value 1
	int optionIcoin2[MAX_OPTION_INFO]; //option icoin value 2
	int optionRuud1[MAX_OPTION_INFO]; //option ruud value 1
	int optionRuud2[MAX_OPTION_INFO]; //option ruud value 2

	char description[1024]; //oficina full description texto
	char itemName[64]; //current item name
	int itemIndex; //current item index
};

struct OPTIONS_INFO
{
	int id;
	int code;
	char name[64];
	int icoin;
	int ruud;
};

struct PMSG_OFICINA_CONFIRM_BUY_SEND
{
	PSBMSG_HEAD h;
	BYTE options[5];
	BYTE payMethod;
};

struct PMSG_OFICINA_CLOSE_SEND
{
	PSBMSG_HEAD h;
};

class Oficina
{

public:
	Oficina();

	void ClearData();

	void DrawOficinaConfirmWindow();
	char* GetOptionName(int code, int type);
	int GetOptionIcoinPrice(int code, int type);
	int GetOptionRuudPrice(int code, int type);
	void UpdatePrices();
	int GetNextCode(int code, int type);
	int GetPreviousCode(int code, int type);
	bool CheckRepeatedOptions(int code);
	bool CheckNonDesignedOption(int code, int type);
	bool CheckAllRepeatedOptions();
	bool CheckValidOption(int code, int type);
	void DrawOficinaPanelWindow();

	void ReceiveOficinaData(PMSG_OFICINA_OPEN* lpMsg);

	void DrawMasteryOptions();

	void DrawWingOptions();

	bool EventWindowMain(DWORD Event);

	bool OficinaWindowClose(DWORD Event);
	bool OficinaSetas(DWORD Event);
	bool OficinaBuyIcoinButton(DWORD Event);
	bool OficinaBuyRuudButton(DWORD Event);
	bool OficinaCancelButton(DWORD Event);
	bool OficinaConfirmBuyIcoinButton(DWORD Event);
	bool OficinaConfirmBuyRuudButton(DWORD Event);

	void CloseOficinaWindow();

	void SendClosePacket();

	void SendConfirmBuyPacket();

	void CloseAndResetAll();

	int originalOptionCode[5];
	int selectedOptionCode[5];

	int currentIcoinPrice;
	int currentRuudPrice;
	int payMethod;

	char descricao[1024];
	char itemName[64];
	int itemIndex;
	int itemType;

	OPTIONS_INFO optionInfo[2][MAX_OPTION_INFO];

private:

};

extern Oficina gOficina;