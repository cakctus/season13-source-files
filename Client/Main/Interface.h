#pragma once

#include "Offset.h"
#include "stdafx.h"

#define INTERFACE_DEBUG		0

#define MAX_OBJECT			100
#define MAX_WIN_WIDTH		640
#define MAX_WIN_HEIGHT		480

#define MAX_WINDOW_EX  100

enum TextAlign
{
	eTxtAlign_left = 0,
	eTxtAlign_center = 3,
	eTxtAlign_right = 4
};

enum TextureID
{
	eImg_ProgressBack = 100000,
	eImg_ProgressBlue,
	eImg_ProgressGreen,
	eImg_ProgressOrange,
	eImg_ProgressRed,
	eImg_ProgressYellow,
	eImg_TextBackground,
	eImg_OficinaBg,
	eImg_OficinaCloseButton1,
	eImg_TextBox,
	eImg_SetaEsq1,
	eImg_SetaEsq2,
	eImg_SetaDir1,
	eImg_SetaDir2,
	eImg_Botao1,
	eImg_Botao2,
	eImg_Botao3,
	eImg_OficinaConfirmBg,
	eImg_ScheduleBg
};

enum ObjectID
{
	ProgressBack = 0,
	ProgressBlue,
	ProgressGreen,
	ProgressOrange,
	ProgressRed,	
	ProgressYellow,
	TextBackground,
	OficinaBg,
	OficinaCloseButton1,
	TextBox,
	SetaEsq1_1,
	SetaEsq1_2,
	SetaEsq1_3,
	SetaEsq1_4,
	SetaEsq1_5,
	SetaEsq2_1,	
	SetaEsq2_2,	
	SetaEsq2_3,	
	SetaEsq2_4,	
	SetaEsq2_5,
	SetaDir1_1,
	SetaDir1_2,
	SetaDir1_3,
	SetaDir1_4,
	SetaDir1_5,
	SetaDir2_1,	
	SetaDir2_2,	
	SetaDir2_3,	
	SetaDir2_4,	
	SetaDir2_5,
	BotaoBuyIcoin1,
	BotaoBuyIcoin2,
	BotaoBuyIcoin3,
	BotaoBuyRuud1,
	BotaoBuyRuud2,
	BotaoBuyRuud3,
	BotaoCancelar1,
	BotaoCancelar2,
	BotaoCancelar3,
	OficinaConfirmBg,
	BotaoConfirmBuyIcoin1,
	BotaoConfirmBuyIcoin2,
	BotaoConfirmBuyIcoin3,
	BotaoConfirmBuyRuud1,
	BotaoConfirmBuyRuud2,
	BotaoConfirmBuyRuud3,
	ScheduleBg
};

#define Color4f(r, b, g, o)			((o << 24) + (g << 16) + (b << 8) + r)

enum ColorMacro
{
	eShinyGreen = Color4f(172, 255, 56, 255),
	eGold = Color4f(255, 189, 25, 255),
	eWhite = Color4f(255, 255, 255, 255),
	eWhite180 = Color4f(255, 255, 255, 180),
	eOrange = Color4f(255, 105, 25, 255),
	eGray100 = Color4f(50, 50, 50, 100),
	eGray150 = Color4f(50, 50, 50, 150),
	eGray200 = Color4f(50, 50, 50, 200),
	eBlowPink = Color4f(220, 20, 60, 255),
	eRed = Color4f(225, 0, 0, 255),
	eExcellent = Color4f(0, 225, 139, 255),
	eAncient = Color4f(1, 223, 119, 255),
	eSocket = Color4f(153, 102, 204, 255),
	eBlue = Color4f(36, 242, 252, 255),
	eDarkBlue = Color4f(80, 80, 255, 255),
	eYellow = Color4f(255, 255, 0, 255),
	eBrown = Color4f(69, 39, 18, 255),
	eMasteryBonus = Color4f(226, 22, 226, 255),
	eBlueItemOption = Color4f(111, 153, 215,255)
};

enum ObjWindow // -> Season 6.3
{
	FriendList = 1,
	MoveList = 2,
	Party = 3,
	Quest = 4,
	Guild = 6,
	Trade = 7,
	Warehouse = 8,
	ChaosBox = 9,
	CommandWindow = 10,
	PetInfo = 11,
	Shop = 12,
	Inventory = 13,
	Store = 14,
	OtherStore = 15,
	Character = 16,
	ChatWindow = 33,
	FastMenu = 34,
	Options = 35,
	Help = 36,
	FastDial = 39,
	SkillTree = 57,
	NPC_Titus = 63,
	CashShop = 65,
	FullMap = 72,
	NPC_Dialog = 73,
	GensInfo = 74,
	NPC_Julia = 75,
	ExpandInventory = 77,
	ExpandWarehouse = 78,
	MuHelper = 79
};

enum ObjWindowsEx
{
	exWinNews,
	exWinReset,
	exWinGrandReset,
	exWinRanking,
	exWinDonate,
	exWinPremium,
	exWinMiniMenu,
	exWinSettings,
	exWinPTSearchMaster,
	exWinPTSearchUser,
	exWinPTSearchPsw,
	exWinPTSearchError,
	exWinCheckOffAfk,
	exWinPersonalSearch,
	exWinPersonalPrice,
	exWinPKClear,
	exWinNPCBuffer,
	exWianNews,
	exWinQuest,
	exWinGuestAccount,
	exWinExchange,
	exWinTeleport,
	exWinBoxSearch,
	exWinAddPoints,
	exWinAutoReset,
};

struct InterfaceObject
{
	DWORD	ModelID;
	float	Width;
	float	Height;
	float	X;
	float	Y;
	float	MaxX;
	float	MaxY;
	DWORD	EventTick;
	bool	OnClick;
	bool	OnShow;
	BYTE	Attribute;
};

class Interface
{
public:
	Interface();

	void		Load();
	void BindObject(short ObjectID, DWORD ModelID, float Width, float Height, float X, float Y);
	static void	LoadImages();

	void Run();

	char* ParsingHP(DWORD Num);

	bool		CheckWindow(int WindowID);
	int			CloseWindow(int WindowID);

	void PrintPing();

	void AutoPVP();

	void UpdateFPS();

	void DrawItem(float PosX, float PosY, float Width, float Height, int ItemID, int Level, int Excl, int Anc, bool OnMouse);

	int DrawFormat(DWORD Color, int PosX, int PosY, int Width, int Align, int Bold, LPCSTR Text, ...);

	void DrawFormatEx(DWORD Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, ...);

	float DrawRepeatGUI(short MonsterID, float X, float Y, int Count);

	void DrawGUI(short ObjectID, float PosX, float PosY);

	void DrawGUIEx(short ObjectID, float PosX, float PosY);

	void DrawColoredGUI(short ObjectID, float X, float Y, DWORD Color);

	void DrawColoredGUIEx(short ObjectID, float X, float Y, DWORD Color);

	int DrawToolTip(int X, int Y, LPCSTR Text, ...);

	int DrawToolTipEx(int X, int Y, LPCSTR Text, ...);

	bool IsWorkZone(short ObjectID);

	bool IsWorkZone(float X, float Y, float MaxX, float MaxY);

	void OpenWindowEx(int WindowID);

	bool CursorWindowEx(int WindowID);

	void ToggleSceneObjects(BYTE status);

	void ResetObject(int ObjectID);

	void CloseWindowEx(int WindowID);

	bool CheckWindowEx(int WindowID);

	bool Button(DWORD Event, int WinID, int ButtonID, bool Type);

	bool ButtonEx(DWORD Event, int ButtonID, bool Type);

	void DrawGUIY(short ObjectID, float PosX, float PosY, int Type);

	void SetTextColor(BYTE Red, BYTE Greed, BYTE Blue, BYTE Opacity);

	void DrawText(int X, int Y, LPCTSTR Text);

	void PrintBossRanking();

	int ping;

	//Boss ranking
	ULONGLONG BossTimer;
	int Life, MaxLife, Level, RankColor, sDamage, sRank, Count, Damage[10];
	char Rank[32], Name[10][11];

	//FPS measure
	int lastReport;
	int frameCount;
	int frameRate;
	char FPS_REAL[30];

	InterfaceObject Data[MAX_OBJECT];
	bool  WindowEx[MAX_WINDOW_EX];

private:

};

extern Interface gInterface;
