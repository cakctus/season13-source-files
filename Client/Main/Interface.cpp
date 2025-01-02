#include "stdafx.h"
#include "Interface.h"
#include "Protocol.h"
#include "CastleSiege.h"
#include "Util.h"
#include <Console.h>
#include "HackCheck.h"
#include "EventSchedule.h"
#include "Oficina.h"
#include "Offset.h"

Interface	gInterface;

int colors[15] = { eShinyGreen, eGold, eWhite, eWhite180, eOrange, eGray100, eGray150, eBlowPink, eRed, eExcellent, eAncient, eSocket, eBlue, eYellow, eBrown };

Interface::Interface()
{
	ZeroMemory(this->Data, sizeof(this->Data));
	ZeroMemory(this->WindowEx, sizeof(this->WindowEx));
}

void Interface::Load()
{
	pLoadBitmap("Interface\\Custom\\ProgressBg.tga", eImg_ProgressBack, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(ProgressBack, eImg_ProgressBack, 156.0, 8.0, (MAX_WIN_WIDTH / 2.0f) - (160.0f / 2.0f), 350.0);

	pLoadBitmap("Interface\\Custom\\ProgressBarBlue.jpg", eImg_ProgressBlue, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(ProgressBlue, eImg_ProgressBlue, 0, 0, 0, 0);
	
	pLoadBitmap("Interface\\Custom\\ProgressBarGreen.jpg", eImg_ProgressGreen, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(ProgressGreen, eImg_ProgressGreen, 0, 0, 0, 0);

	pLoadBitmap("Interface\\Custom\\ProgressBarOrange.jpg", eImg_ProgressOrange, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(ProgressOrange, eImg_ProgressOrange, 0, 0, 0, 0);

	pLoadBitmap("Interface\\Custom\\ProgressBarRed.jpg", eImg_ProgressRed, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(ProgressRed, eImg_ProgressRed, 0, 0, 0, 0);

	pLoadBitmap("Interface\\Custom\\ProgressBarYellow.jpg", eImg_ProgressYellow, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(ProgressYellow, eImg_ProgressYellow, 0, 0, 0, 0);

	pLoadBitmap("Interface\\Custom\\TimeBar_12.tga", eImg_TextBackground, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(TextBackground, eImg_TextBackground, 131, 20, 0, 0);

	pLoadBitmap("Interface\\Custom\\oficina.tga", eImg_OficinaBg, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(OficinaBg, eImg_OficinaBg, 410.0, 410.0, 115.0, 2.0);

	pLoadBitmap("Interface\\Custom\\close1.tga", eImg_OficinaCloseButton1, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(OficinaCloseButton1, eImg_OficinaCloseButton1, 20, 20, 488, 28);

	pLoadBitmap("Interface\\Custom\\textBox.jpg", eImg_TextBox, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(TextBox, eImg_TextBox, 162, 18, -1, -1);

	pLoadBitmap("Interface\\Custom\\seta_dir_1.tga", eImg_SetaDir1, GL_LINEAR, GL_CLAMP, 1, 0);
	for (int i = SetaDir1_1; i <= SetaDir1_5; i++)
	{
		this->BindObject(i, eImg_SetaDir1, 18, 18, -1, -1);
	}

	pLoadBitmap("Interface\\Custom\\seta_dir_2.tga", eImg_SetaDir2, GL_LINEAR, GL_CLAMP, 1, 0);
	for (int i = SetaDir2_1; i <= SetaDir2_5; i++)
	{
		this->BindObject(i, eImg_SetaDir2, 18, 18, -1, -1);
	}

	pLoadBitmap("Interface\\Custom\\seta_esq_1.tga", eImg_SetaEsq1, GL_LINEAR, GL_CLAMP, 1, 0);
	for (int i = SetaEsq1_1; i <= SetaEsq1_5; i++)
	{
		this->BindObject(i, eImg_SetaEsq1, 18, 18, -1, -1);
	}	

	pLoadBitmap("Interface\\Custom\\seta_esq_2.tga", eImg_SetaEsq2, GL_LINEAR, GL_CLAMP, 1, 0);
	for (int i = SetaEsq2_1; i <= SetaEsq2_5; i++)
	{
		this->BindObject(i, eImg_SetaEsq2, 18, 18, -1, -1);
	}

	pLoadBitmap("Interface\\Custom\\botao1.tga", eImg_Botao1, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(BotaoBuyIcoin1, eImg_Botao1, 60, 30, -1, -1);
	this->BindObject(BotaoBuyRuud1, eImg_Botao1, 60, 30, -1, -1);
	this->BindObject(BotaoConfirmBuyIcoin1, eImg_Botao1, 60, 30, -1, -1);
	this->BindObject(BotaoConfirmBuyRuud1, eImg_Botao1, 60, 30, -1, -1);
	this->BindObject(BotaoCancelar1, eImg_Botao1, 60, 30, -1, -1);

	pLoadBitmap("Interface\\Custom\\botao2.tga", eImg_Botao2, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(BotaoBuyIcoin2, eImg_Botao2, 60, 30, -1, -1);
	this->BindObject(BotaoBuyRuud2, eImg_Botao2, 60, 30, -1, -1);
	this->BindObject(BotaoConfirmBuyIcoin2, eImg_Botao2, 60, 30, -1, -1);
	this->BindObject(BotaoConfirmBuyRuud2, eImg_Botao2, 60, 30, -1, -1);
	this->BindObject(BotaoCancelar2, eImg_Botao2, 60, 30, -1, -1);

	pLoadBitmap("Interface\\Custom\\botao3.tga", eImg_Botao3, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(BotaoBuyIcoin3, eImg_Botao3, 60, 30, -1, -1);
	this->BindObject(BotaoBuyRuud3, eImg_Botao3, 60, 30, -1, -1);
	this->BindObject(BotaoConfirmBuyIcoin3, eImg_Botao3, 60, 30, -1, -1);
	this->BindObject(BotaoConfirmBuyRuud3, eImg_Botao3, 60, 30, -1, -1);
	this->BindObject(BotaoCancelar3, eImg_Botao3, 60, 30, -1, -1);

	pLoadBitmap("Interface\\Custom\\confirm.tga", eImg_OficinaConfirmBg, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(OficinaConfirmBg, eImg_OficinaConfirmBg, 220, 218, 210, 131);

	pLoadBitmap("Interface\\Custom\\schedule.tga", eImg_ScheduleBg, GL_LINEAR, GL_CLAMP, 1, 0);
	this->BindObject(ScheduleBg, eImg_ScheduleBg, 410, 328, 115, 76);
}

void Interface::BindObject(short ObjectID, DWORD ModelID, float Width, float Height, float X, float Y)
{
	this->Data[ObjectID].EventTick = 0;
	this->Data[ObjectID].OnClick = false;
	this->Data[ObjectID].OnShow = false;
	this->Data[ObjectID].ModelID = ModelID;
	this->Data[ObjectID].Width = Width;
	this->Data[ObjectID].Height = Height;
	this->Data[ObjectID].X = X;
	this->Data[ObjectID].Y = Y;
	this->Data[ObjectID].MaxX = X + Width;
	this->Data[ObjectID].MaxY = Y + Height;
	this->Data[ObjectID].Attribute = 0;
}

void Interface::LoadImages()
{
	gInterface.Load();
}

void Interface::Run()
{
	g_CastleSiege.PrintSwitch();
	g_CastleSiege.PrintCrown();
	g_CastleSiege.PrintMasterCrown();
	g_CastleSiege.PrintSwitchStatus();

	this->UpdateFPS();
	this->PrintPing();
	this->AutoPVP();
	this->PrintBossRanking();

	gOficina.DrawOficinaPanelWindow();
	gOficina.DrawOficinaConfirmWindow();
	gEventSchedule.DrawScheduleWindow();

	CoordX = (*(BYTE*)(*(DWORD*)(MAIN_VIEWPORT_STRUCT)+0x180));
	CoordY = (*(BYTE*)(*(DWORD*)(MAIN_VIEWPORT_STRUCT)+0x184));
	TestMoveHack();

	switch (((*(BYTE*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+0x18B)) & 7))
	{
	case 6:
		SetByte(0x005CFB37, 0x0F);
		break;
	default:
		SetByte(0x005CFB37, 0x02);
		break;
	}	
}

char* Interface::ParsingHP(DWORD Num)
{
	char Text[50];
	if (Num >= 10000000000)
		sprintf(Text, "%ldkkk", Num / 1000000000);
	else if (Num >= 10000000)
		sprintf(Text, "%ldkk", Num / 1000000);
	else if (Num >= 10000)
		sprintf(Text, "%ldk", Num / 1000);
	else
		sprintf(Text, "%ld", Num);

	return _strdup(Text);
}


bool Interface::CheckWindow(int WindowID)
{
	return pCheckWindow(pWindowThis(), WindowID);
}

int Interface::CloseWindow(int WindowID)
{
	return pCloseWindow(pWindowThis(), WindowID);
}

void Interface::PrintPing()
{
	if (gInterface.ping <= 0)
	{
		return;
	}

	int color;
	int progress;
	int ping = (int)(gInterface.ping / 1.5f);

	if (ping <= 150)
	{
		color = eExcellent;
		progress = eImg_ProgressGreen;
	}
	else if (ping > 150 && ping <= 250)
	{
		color = eYellow;
		progress = eImg_ProgressYellow;
	}
	else if (ping > 250 && ping <= 350)
	{
		color = eOrange;
		progress = eImg_ProgressOrange;
	}
	else if (ping > 350)
	{
		color = eRed;
		progress = eImg_ProgressRed;
	}

	char buff[16];
	sprintf(buff, "Ping: %dms", ping);

	pRenderImage(progress, 0, 475, 30, 2.0f);
	DrawInterfaceText(buff, 1, 465, color, 0, 0, 30.0f, 0);
}

void Interface::AutoPVP()
{
	if (showPVPon == 1)
	{
		return;
	}

	DrawInterfaceText("PVP MODE ON", MAX_WIN_WIDTH - 50, 2, eRed, 0, 0, 30.0f, 1);
}

void Interface::PrintBossRanking()
{
	if ((GetTickCount64() - gInterface.BossTimer) > 5000 || gInterface.Life <= 0)
	{
		return;
	}

	char Text[50];

	int dX = MAX_WIN_WIDTH - 140;
	int dY = 182;

	sprintf(Text, "[ %s ]", gInterface.Rank);
	pRenderImage(eImg_TextBackground, (float)dX, (float)dY, 131, 20);
	DrawInterfaceText(Text, dX, dY + 2, colors[gInterface.RankColor], 0, 3, 131, 1);
	sprintf(Text, "HP:  %s / %s", ParsingHP(gInterface.Life), ParsingHP(gInterface.MaxLife));
	DrawInterfaceText(Text, dX, dY + 9, eRed, 0, 3, 131, 1);

	dY += 19;

	pRenderImage(eImg_TextBackground, (float)dX, (float)dY, 131, 14);

	DrawInterfaceText("#", dX + 8, dY + 2, eYellow, 0, 0, 123, 1);
	DrawInterfaceText("Nome", dX + 20, dY + 2, eYellow, 0, 1, 55, 1);
	DrawInterfaceText("Dano", dX + 80, dY + 2, eYellow, 0, 1, 30, 1);
	DrawInterfaceText("Drop", dX + 105, dY + 2, eYellow, 0, 1, 30, 1);

	dY += 12;

	//Log.ConsoleOutPut(1, c_Blue, t_Default, "Boss %d", gInterface.Count);

	for (int j = 0; j < gInterface.Count; j++)
	{
		DWORD choosenColor = (sRank > -1 && sRank == j) ? eExcellent : eWhite;
		BYTE textType = (sRank > -1 && sRank == j) ? 1 : 0;

		pRenderImage(eImg_TextBackground, (float)dX, (float)dY, 131, 20);

		sprintf(Text, "%d", j + 1);
		DrawInterfaceText(Text, dX + 8, dY + 5, choosenColor, 0, 1, 123, textType);

		DrawInterfaceText(Name[j], dX + 20, dY + 5, choosenColor, 0, 1, 55, textType);

		sprintf(Text, "%s", ParsingHP(Damage[j]));
		DrawInterfaceText(Text, dX + 80, dY + 5, choosenColor, 0, 1, 30, textType);

		sprintf(Text, "  %d%%", (int)((float)Damage[j] / (float)gInterface.MaxLife * 100.0f));
		DrawInterfaceText(Text, dX + 105, dY + 5, choosenColor, 0, 1, 30, textType);

		dY += 16;
	}

	if (gInterface.sRank != -1 && gInterface.sRank >= 10)
	{
		pRenderImage(eImg_TextBackground, (float)dX, (float)dY, 131, 14);
		DrawInterfaceText("Voce:", dX + 8, dY + 2, eRed, 0, 1, 115, 3);

		dY += 12;

		pRenderImage(eImg_TextBackground, (float)dX, (float)dY, 131, 20);

		sprintf(Text, "%d", sRank + 1);
		DrawInterfaceText(Text, dX + 8, dY + 5, eBlue, 0, 1, 123, 2);

		char myName[11];
		memcpy(myName, (void*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+0x54), sizeof(myName));

		DrawInterfaceText(myName, dX + 20, dY + 5, eBlue, 0, 1, 55, 2);

		sprintf(Text, "%s", ParsingHP(sDamage));
		DrawInterfaceText(Text, dX + 80, dY + 5, eBlue, 0, 1, 30, 3);

		sprintf(Text, "  %d%%", (int)((float)sDamage / (float)gInterface.MaxLife * 100.0f));
		DrawInterfaceText(Text, dX + 105, dY + 5, eBlue, 0, 1, 30, 3);
	}	
}

void Interface::UpdateFPS()
{
	gInterface.frameCount++;

	if (GetTickCount() - gInterface.lastReport >= 1000)
	{
		gInterface.frameRate = gInterface.frameCount / ((GetTickCount() - gInterface.lastReport) / 1000);
		sprintf(gInterface.FPS_REAL, "FPS: %d", gInterface.frameRate);
		gInterface.lastReport = GetTickCount();
		gInterface.frameCount = 0;
	}

	//Log.ConsoleOutPut(1, c_Green, t_Default, "FPS: %d", gInterface.frameRate);

	if (gInterface.frameRate <= 0)
	{
		return;
	}

	int color;
	int progress;

	if (gInterface.frameRate >= 22)
	{
		color = eExcellent;
		progress = eImg_ProgressGreen;
	}
	else if (gInterface.frameRate >= 18 && gInterface.frameRate < 22)
	{
		color = eYellow;
		progress = eImg_ProgressYellow;
	}
	else if (gInterface.frameRate >= 12 && gInterface.frameRate < 18)
	{
		color = eOrange;
		progress = eImg_ProgressOrange;
	}
	else if (gInterface.frameRate < 12)
	{
		color = eRed;
		progress = eImg_ProgressRed;
	}

	char buff[16];
	sprintf(buff, "FPS: %d", gInterface.frameRate);

	pRenderImage(progress, 0, 460, 30, 2.0f);
	DrawInterfaceText(buff, 1, 450, color, 0, 0, 30.0f, 0);
}

void Interface::DrawItem(float PosX, float PosY, float Width, float Height, int ItemID, int Level, int Excl, int Anc, bool OnMouse)
{
	glMatrixMode(0x1701);
	glPushMatrix();
	glLoadIdentity();

	sub_6A9CEB_Addr(0, 0, WindowWidth, WindowHeight);

	float screenProportion = (float)WindowWidth / (float)WindowHeight;
	sub_6A90C5_Addr(1.0, screenProportion, *(float*)0x16012F0, *(float*)0x16012F4);

	glMatrixMode(0x1700);
	glPushMatrix();
	glLoadIdentity();

	sub_6A9068_Addr((LPVOID)0x90AD9F0);
	sub_6A976E_Addr();
	EnableDepthMask();

	pDrawItemModel(PosX, PosY, Width, Height, ItemID, Level, Excl, Anc, OnMouse);

	glMatrixMode(0x1700u);
	glPopMatrix();
	glMatrixMode(0x1701u);
	glPopMatrix();

	glColor3f(1, 1, 1);
	EnableAlphaTest(false);
}

void Interface::SetTextColor(BYTE Red, BYTE Green, BYTE Blue, BYTE Opacity)
{
	pSetTextColor(pRenderTextInstance(), Red, Green, Blue, Opacity);
}

void Interface::DrawText(int X, int Y, LPCTSTR Text)
{
	pRenderText(pRenderTextInstance(), X, Y, Text, 0, 0, (LPINT)1, 0);
}

int Interface::DrawFormat(DWORD Color, int PosX, int PosY, int Width, int Align, int Bold, LPCSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff) - 1;
	ZeroMemory(Buff, BuffLen);

	va_list args;
	va_start(args, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);

	int LineCount = 0;

	char* Line = strtok(Buff, "#");

	while (Line != NULL)
	{
		DrawInterfaceText(Line, PosX, PosY, Color, 1, Align, Width, Bold);
		PosY += 10;
		Line = strtok(NULL, "#");
	}

	return PosY;
}

void Interface::DrawFormatEx(DWORD Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff) - 1;
	ZeroMemory(Buff, BuffLen);

	va_list args;
	va_start(args, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);

	DrawInterfaceText(Buff, PosX, PosY, Width, 0, Color, 0, Align);
}

float Interface::DrawRepeatGUI(short ObjectID, float X, float Y, int Count)
{
	float StartY = Y;

	for (int i = 0; i < Count; i++)
	{
		pRenderImage(this->Data[ObjectID].ModelID, X, StartY, this->Data[ObjectID].Width, this->Data[ObjectID].Height);
		StartY += this->Data[ObjectID].Height;
	}

	return StartY;
}

void Interface::DrawGUI(short ObjectID, float PosX, float PosY)
{
	if (this->Data[ObjectID].X == -1 || this->Data[ObjectID].Y == -1)
	{
		this->Data[ObjectID].X = PosX;
		this->Data[ObjectID].Y = PosY;
		this->Data[ObjectID].MaxX = PosX + this->Data[ObjectID].Width;
		this->Data[ObjectID].MaxY = PosY + this->Data[ObjectID].Height;
	}

	//Log.ConsoleOutPut(1, c_Blue, t_Default, "Render %d %d %d %d %d", this->Data[ObjectID].ModelID, PosX, PosY, this->Data[ObjectID].Width, this->Data[ObjectID].Height);

	pRenderImage(this->Data[ObjectID].ModelID, PosX, PosY, this->Data[ObjectID].Width, this->Data[ObjectID].Height);
}

void Interface::DrawGUIEx(short ObjectID, float PosX, float PosY)
{
	this->Data[ObjectID].X = PosX;
	this->Data[ObjectID].Y = PosY;
	this->Data[ObjectID].MaxX = PosX + this->Data[ObjectID].Width;
	this->Data[ObjectID].MaxY = PosY + this->Data[ObjectID].Height;

	pRenderImage(this->Data[ObjectID].ModelID, PosX, PosY, this->Data[ObjectID].Width, this->Data[ObjectID].Height);
}

void Interface::DrawColoredGUI(short ObjectID, float X, float Y, DWORD Color)
{
	if ((this->Data[ObjectID].X == -1 || this->Data[ObjectID].Y == -1))
	{
		this->Data[ObjectID].X = X;
		this->Data[ObjectID].Y = Y;
		this->Data[ObjectID].MaxX = X + this->Data[ObjectID].Width;
		this->Data[ObjectID].MaxY = Y + this->Data[ObjectID].Height;
	}

	pDrawColorButton(this->Data[ObjectID].ModelID, X, Y, this->Data[ObjectID].Width, this->Data[ObjectID].Height, 0, 0, Color);
}

void Interface::DrawColoredGUIEx(short ObjectID, float X, float Y, DWORD Color)
{
	this->Data[ObjectID].X = X;
	this->Data[ObjectID].Y = Y;
	this->Data[ObjectID].MaxX = X + this->Data[ObjectID].Width;
	this->Data[ObjectID].MaxY = Y + this->Data[ObjectID].Height;

	pDrawColorButton(this->Data[ObjectID].ModelID, X, Y, this->Data[ObjectID].Width, this->Data[ObjectID].Height, 0, 0, Color);
}

int Interface::DrawToolTip(int X, int Y, LPCSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff);
	ZeroMemory(Buff, BuffLen);

	va_list args;
	va_start(args, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);

	return pDrawToolTip(MouseX + 16, MouseY - 4, Buff);
}

int Interface::DrawToolTipEx(int X, int Y, LPCSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff);
	ZeroMemory(Buff, BuffLen);

	va_list args;
	va_start(args, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);

	int LineCount = 0;

	char* Line = strtok(Buff, "#");

	while (Line != NULL)
	{
		pDrawToolTip(MouseX + 16, MouseY - 4, Line);
		Y += 10;
		Line = strtok(NULL, "#");
	}

	return Y;
}

bool Interface::IsWorkZone(short ObjectID)
{
	float PosX = this->Data[ObjectID].X;
	float MaxX = PosX + this->Data[ObjectID].Width;

	/*if (ObjectID == eCAMERA_MAIN || ObjectID == eCAMERA_BUTTON1 || ObjectID == eCAMERA_BUTTON2 || ObjectID == SCHEDULE_BUTTON)
	{
		PosX = this->GetResizeX(ObjectID);
		MaxX = PosX + this->Data[ObjectID].Width;
	}*/

	if ((MouseX < PosX || MouseX > MaxX) || (MouseY < this->Data[ObjectID].Y || MouseY > this->Data[ObjectID].MaxY))
	{
		return false;
	}

	return true;
}

bool Interface::IsWorkZone(float X, float Y, float MaxX, float MaxY)
{
	if ((MouseX < X || MouseX > MaxX) || (MouseY < Y || MouseY > MaxY))
		return false;

	return true;
}

void Interface::OpenWindowEx(int WindowID)
{
	if (WindowID < 0 || WindowID > MAX_WINDOW_EX)
	{
		return;
	}

	if (this->CursorWindowEx(WindowID))
	{
		pSetCursorFocus = true;
	}

	if (WindowID == exWinAddPoints)
	{
		*(BYTE*)0x1636AC0 = 0;
	}

	if (WindowID == exWinAutoReset)
	{
		*(BYTE*)0x1636AC0 = 0;
	}

	this->WindowEx[WindowID] = 1;
}

void Interface::CloseWindowEx(int WindowID)
{
	if (WindowID < 0 || WindowID > MAX_WINDOW_EX)
	{
		return;
	}

	if (this->CursorWindowEx(WindowID))
	{
		pSetCursorFocus = false;
	}

	if (WindowID == exWinAddPoints)
	{
		*(BYTE*)0x1636AC0 = 1;
	}

	if (WindowID == exWinAutoReset)
	{
		*(BYTE*)0x1636AC0 = 1;
	}

	this->WindowEx[WindowID] = 0;
}

bool Interface::CheckWindowEx(int WindowID)
{
	if (WindowID < 0 || WindowID > MAX_WINDOW_EX)
	{
		return 0;
	}
	return this->WindowEx[WindowID];
}

void Interface::DrawGUIY(short ObjectID, float PosX, float PosY, int Type)
{
	this->Data[ObjectID].X = PosX;
	this->Data[ObjectID].Y = PosY;
	this->Data[ObjectID].MaxX = PosX + this->Data[ObjectID].Width;
	this->Data[ObjectID].MaxY = PosY + this->Data[ObjectID].Height;

	pDrawButton(this->Data[ObjectID].ModelID, PosX, PosY, this->Data[ObjectID].Width, this->Data[ObjectID].Height, 0.0, (Type * this->Data[ObjectID].Height));
}

bool Interface::CursorWindowEx(int WindowID)
{
	if (WindowID < 0 || WindowID > MAX_WINDOW_EX)
	{
		return false;
	}

	return true;
}

void Interface::ToggleSceneObjects(BYTE status)
{
	DWORD CallAddr1 = 0x889670;
	DWORD CallAddr2 = 0xB85B88;
	_asm {
		push status
		mov ecx, 0xA119C3C
		CALL CallAddr1
		mov ecx, eax
		CALL CallAddr2
	}
}

void Interface::ResetObject(int ObjectID)
{
	this->Data[ObjectID].X = -1;
	this->Data[ObjectID].Y = -1;
	this->Data[ObjectID].MaxX = -1;
	this->Data[ObjectID].MaxY = -1;
}