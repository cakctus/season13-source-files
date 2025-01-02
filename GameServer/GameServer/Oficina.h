#pragma once

#include "user.h"
#include "Protocol.h"

#define MAX_OPTION_INFO 16
#define MAX_OPTION_TYPE 18

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

struct PMSG_OFICINA_CONFIRM_BUY_SEND
{
	PSBMSG_HEAD h;
	BYTE options[5];
	BYTE payMethod;
};

struct OPTIONS_INFO
{
	int id;
	int code;
	char name[64];
	int icoin;
	int ruud;
};

class Oficina
{
public:
	Oficina();
	virtual ~Oficina();

	void LoadDescriptions(char* path);
	void LoadOptions(char* path);
	void NpcOpen(LPOBJ lpObj);
	void BuyReceived(int aIndex, PMSG_OFICINA_CONFIRM_BUY_SEND* pMsg);
	bool CheckValidOption(int type, int option);
	bool CheckRepeatedOption(BYTE option[5]);
	void CloseWindow(LPOBJ lpObj);
	int GetOptionRuudPrice(int code, int type);
	int GetOptionIcoinPrice(int code, int type);
	static void OnBuyCallbackRecv(LPOBJ lpObj, DWORD none1, DWORD none2, DWORD WCoinC, DWORD WCoinP, DWORD GoblinPoint);

private:
	char descriptions[10][1024];
	OPTIONS_INFO optionInfo[MAX_OPTION_TYPE][MAX_OPTION_INFO];
};



extern Oficina gOficina;