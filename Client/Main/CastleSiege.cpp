#include "stdafx.h"
#include "CastleSiege.h"
#include "Offset.h"
#include "Console.h"
#include "Interface.h"

CastleSiege g_CastleSiege;

CastleSiege::CastleSiege()
{
	this->SwitchState = CASTLE_SIEGE_SWITCH_STATE_NONE;
	this->CrownState = CASTLE_SIEGE_CROWN_STATE_NONE;
	this->CrownMasterState = CASTLE_SIEGE_CROWN_MASTER_STATE_NONE;
	this->SignStatus = 0;
	this->SignTimer = GetTickCount64();
}

CastleSiege::~CastleSiege()
{

}

void CastleSiege::RunTimer()
{
	if ((GetTickCount64() - this->SignTimer) > 200)
	{
		this->SignStatus = (this->SignStatus >= 3) ? 0 : this->SignStatus+1;
		this->SignTimer = GetTickCount64();
	}
}

void CastleSiege::SetSwitchData(BYTE state, WORD index, WORD monster)
{
	this->SwitchState = state;

	this->SwitchTime.Start(2000);

	char buff[256];
	switch(this->SwitchState)
	{
	case CASTLE_SIEGE_SWITCH_STATE_ON:
		{
			sprintf(buff, "%s", GetTextBMD(1485));
		} break;

	case CASTLE_SIEGE_SWITCH_STATE_OFF:
		{
			sprintf(buff, "%s", GetTextBMD(1484));
		} break;

	case CASTLE_SIEGE_SWITCH_STATE_BUSY:
		{
			sprintf(buff, "Switch ja esta ocupada");
		} break;
	}

	this->SwitchString = buff;
}

void CastleSiege::PrintSwitch()
{
	if (!IsCastleSiege())
	{
		return;
	}

	if(this->SwitchState == CASTLE_SIEGE_SWITCH_STATE_NONE)
	{
		return;
	}

	if(this->SwitchTime.Elapsed())
	{
		this->SwitchState = CASTLE_SIEGE_SWITCH_STATE_NONE;
		return;
	}

	float StartX = (MAX_WIN_WIDTH/2.0f)-(160.0f/2.0f);

	float StartY = 350.0f;

	float progress = (this->SwitchTime.GetRemain() * 150.0f)/(float)this->SwitchTime.Time;

	progress = ((progress>150)?150:progress);

	//pRenderImage(0x7B14,StartX - 25.0f,StartY - 40.0f, 209.0f, 80.0f);

	pRenderImage(eImg_ProgressBack, StartX, StartY, 156.0f, 8.0f);
	pRenderImage(eImg_ProgressYellow, (StartX + 2.0f), (StartY + 2.0f), progress, 4.0f);

	/*pRenderImage(0x7A65,StartX,StartY,160.0f,18.0f);
	pRenderImage(0x7A66,(StartX+5.0f),(StartY+5.0f),progress,8.0f);*/

	char buff[256];
	sprintf(buff, "%s", this->SwitchString.c_str());

	DrawInterfaceText(buff,(int)StartX,(int)(StartY-10.0f),0xFFFFFFFF,0,3,160.0f,1);
}

void CastleSiege::SetCrownData(BYTE state)
{
	this->CrownState = state;

	this->CrownTime.Start(3000);

	char buff[256];
	switch(this->CrownState)
	{
	case 2:
	{
		sprintf(buff, "%s", GetTextBMD(1490));
	} break;

	case CASTLE_SIEGE_CROWN_STATE_ON:
		{
			sprintf(buff, "%s", GetTextBMD(1494));
		} break;

	case CASTLE_SIEGE_CROWN_STATE_OFF:
		{
			sprintf(buff, "%s", GetTextBMD(1493));
		} break;
	}

	this->CrownString = buff;
}

void CastleSiege::PrintCrown()
{
	if (!IsCastleSiege())
	{
		return;
	}

	if(this->CrownState == CASTLE_SIEGE_CROWN_STATE_NONE)
	{
		return;
	}

	if(this->CrownTime.Elapsed())
	{
		this->CrownState = CASTLE_SIEGE_CROWN_STATE_NONE;
		return;
	}

	float StartX = (MAX_WIN_WIDTH/2.0f)-(160.0f/2.0f);

	float StartY = 330.0f;

	float progress = (this->CrownTime.GetRemain() * 150.0f)/(float)this->CrownTime.Time;

	progress = ((progress>150)?150:progress);

	pRenderImage(eImg_ProgressBack, StartX, StartY, 156.0f, 8.0f);

	switch (this->CrownState)
	{
	case CASTLE_SIEGE_CROWN_STATE_ON:
		pRenderImage(eImg_ProgressRed, (StartX + 2.0f), (StartY + 2.0f), progress, 4.0f);
		break;
	case CASTLE_SIEGE_CROWN_STATE_OFF:
		pRenderImage(eImg_ProgressBlue, (StartX + 2.0f), (StartY + 2.0f), progress, 4.0f);
		break;
	case 2:
		pRenderImage(eImg_ProgressGreen, (StartX + 2.0f), (StartY + 2.0f), progress, 4.0f);
		break;
	}

	char buff[256];
	sprintf(buff, "%s", this->CrownString.c_str());

	DrawInterfaceText(buff,(int)StartX,(int)(StartY-15.0f),0xFFFFFFFF,0,3,160.0f,1);
}

void CastleSiege::SetCrownMasterData(BYTE state, DWORD time)
{
	this->CrownMasterState = state;

	this->CrownMasterTime.Start(2000);

	char buff1[256];
	char buff2[256];
	this->CrownMasterString2.clear();
	this->CrownMasterString1.clear();
	switch(this->CrownMasterState)
	{
	case CASTLE_SIEGE_CROWN_MASTER_STATE_ATTEMPT:
		{
			sprintf(buff1, "%s", GetTextBMD(1489));
			sprintf(buff2, "(Tempo acumulado: %d segundos)", time / 1000);

			this->CrownMasterTime.Start(61000);
			this->CrownMasterTime.Time -= time;
			this->CrownMasterTime.Progress = time;

			this->CrownMasterString2 = buff2;
		} break;

	case CASTLE_SIEGE_CROWN_MASTER_STATE_SUCCESS:
		{
			sprintf(buff1, "%s", GetTextBMD(1490));
		} break;

	case CASTLE_SIEGE_CROWN_MASTER_STATE_FAIL:
		{
			sprintf(buff1, "%s", GetTextBMD(1491));
			sprintf(buff2, "(Tempo acumulado: %d segundos)", time / 1000);
			this->CrownMasterTime.Progress = time;
			this->CrownMasterString2 = buff2;
		} break;

	case CASTLE_SIEGE_CROWN_MASTER_STATE_TAKEN:
		{
			sprintf(buff1, "%s", GetTextBMD(1492));
		} break;

	case CASTLE_SIEGE_CROWN_MASTER_STATE_WRONG_SWITCH:
		{
			sprintf(buff1, "%s", GetTextBMD(1982));
		} break;
	}

	this->CrownMasterString1 = buff1;
}
	
void CastleSiege::PrintMasterCrown()
{
	if (!IsCastleSiege())
	{
		return;
	}

	if( this->CrownMasterState == CASTLE_SIEGE_CROWN_MASTER_STATE_NONE ||
		this->CrownMasterState == CASTLE_SIEGE_CROWN_MASTER_STATE_TAKEN ||
		this->CrownMasterState == CASTLE_SIEGE_CROWN_MASTER_STATE_WRONG_SWITCH)
	{
		return;
	}

	if(this->CrownMasterTime.Elapsed())
	{
		this->CrownMasterState = CASTLE_SIEGE_CROWN_MASTER_STATE_NONE;
		return;
	}

	float StartX = (MAX_WIN_WIDTH/2.0f)-(156.0f/2.0f);

	float StartY = 61.0f;

	float progress;
	char buff2[256];	

	if (this->CrownMasterState == CASTLE_SIEGE_CROWN_MASTER_STATE_ATTEMPT)
	{		
		this->CrownMasterString2.clear();
		sprintf(buff2, "(Tempo acumulado: %d segundos)", (this->CrownMasterTime.Progress + this->CrownMasterTime.GetRemain()) / 1000);
		this->CrownMasterString2 = buff2;
		progress = (float(this->CrownMasterTime.Progress + this->CrownMasterTime.GetRemain()) * 152.0f) / 61000.0f;
	}
	else
	{
		progress = (float(this->CrownMasterTime.Progress) * 152.0f) / 61000.0f;
	}

	progress = ((progress>152)?152:progress);

	int status[5] = { eImg_ProgressRed, eImg_ProgressOrange, eImg_ProgressYellow, eImg_ProgressBlue, eImg_ProgressGreen };

	pRenderImage(eImg_ProgressBack, StartX, StartY, 156.0f, 8.0f);

	pRenderImage(status[(int)(progress / 30.4f)],(StartX+2.0f),(StartY+2.0f),progress,4.0f);

	char buff1[256];
	sprintf(buff1, "%s", this->CrownMasterString1.c_str());

	DrawInterfaceText(buff1,(int)StartX,(int)(StartY-20.0f),0xFFFFFFFF,0,3,160.0f,1);

	if(!this->CrownMasterString2.empty())
	{
		char buff2[256];
		sprintf(buff2, "%s", this->CrownMasterString2.c_str());

		DrawInterfaceText(buff2,(int)StartX,(int)(StartY-10.0f),0xFFFFFFFF,0,3,160.0f,0);
	}
}

void CastleSiege::PrintSwitchStatus()
{
	if (!IsCastleSiege())
	{
		ResetCrownSwitchData();
		return;
	}

	char switch_message[150];
	memset(switch_message, 0, 150);

	//Print Crown GUI
	pRenderImage(0x7E6A, 0.0f, 15.0f, 86.0f, 80.0f);
	DrawInterfaceText("C O R O A", 4, 35, eExcellent, 0, 3, 82.0f, 3);

	if (g_CrownSwitchData[2].state)
	{
		sprintf(switch_message, "[%s] %s", g_CrownSwitchData[2].guild, g_CrownSwitchData[2].name);
		DrawInterfaceText(switch_message, 4, 56, eWhite, 0, 3, 82.0f, 0);

		this->RunTimer();

		int status[5] = { eImg_ProgressGreen, eImg_ProgressBlue, eImg_ProgressYellow, eImg_ProgressOrange, eImg_ProgressRed };

		int StartX = (int) ((MAX_WIN_WIDTH / 2.0f) - (7.0f / 2.0f));
		int StartXText = (int) ((MAX_WIN_WIDTH / 2.0f) - (160.0f / 2.0f));
		int StartY = 300;

		if (showCSbar > 0)
		{
			float progress;
			progress = (float(g_CrownSwitchData[2].accumulated_time) * 77.0f) / 61000.0f;
			progress = ((progress > 77) ? 77 : progress);
			int status[5] = { eImg_ProgressGreen, eImg_ProgressBlue, eImg_ProgressYellow, eImg_ProgressOrange, eImg_ProgressRed };
			pRenderImage(status[(int)(progress / 15.4f)], 7.0f, 67.0f, progress, 2.0f);
		}
		else
		{
			sprintf(switch_message, "Alianca [%s] esta tentando o selamento", g_CrownSwitchData[2].guild);
			DrawInterfaceText(switch_message, StartXText, StartY - 10, eExcellent, 0, 3, 160.0f, 0);

			switch (this->SignStatus)
			{
			case 0:
				pRenderImage(status[rand() % 5], 7.0f, 52.0f, 77.0f, 2.0f);
				pRenderImage(status[rand() % 5], 7.0f, 67.0f, 77.0f, 2.0f);
				pRenderImage(0x7BA4, (float) (StartX - 12), (float)StartY, 7.0f, 7.0f);
				break;
			case 1:
				pRenderImage(status[rand() % 5], 7.0f, 53.0f, 77.0f, 2.0f);
				pRenderImage(status[rand() % 5], 7.0f, 66.0f, 77.0f, 2.0f);
				pRenderImage(0x7BA5, (float)(StartX - 4), (float)StartY, 7.0f, 7.0f);
				break;
			case 2:
				pRenderImage(status[rand() % 5], 7.0f, 52.0f, 77.0f, 2.0f);
				pRenderImage(status[rand() % 5], 7.0f, 67.0f, 77.0f, 2.0f);
				pRenderImage(0x7BA4, (float)(StartX + 4), (float)StartY, 7.0f, 7.0f);
				break;
			case 3:
				pRenderImage(status[rand() % 5], 7.0f, 53.0f, 77.0f, 2.0f);
				pRenderImage(status[rand() % 5], 7.0f, 66.0f, 77.0f, 2.0f);
				pRenderImage(0x7BA3, (float)(StartX + 12), (float)StartY, 7.0f, 7.0f);
				break;
			}
		}
	}
	else
	{
		DrawInterfaceText("-", 4, 56, eWhite, 0, 3, 82.0f, 0);
	}

	//Print Switch 1 GUI
	pRenderImage(0x7E6A, 0.0f, 67.0f, 86.0f, 80.0f);
	DrawInterfaceText("SWITCH 1", 4, 87, eGold, 0, 3, 82.0f, 3);
	if (!g_CrownSwitchData[0].state)
	{
		DrawInterfaceText("-", 4, 108, eWhite, 0, 3, 82.0f, 0);
	}
	else
	{
		sprintf(switch_message, "[%s] %s", g_CrownSwitchData[0].guild, g_CrownSwitchData[0].name);
		DrawInterfaceText(switch_message, 4, 108, eWhite, 0, 3, 82.0f, 0);
	}

	//Print Switch 2 GUI
	pRenderImage(0x7E6A, 0.0f, 119.0f, 86.0f, 80.0f);
	DrawInterfaceText("SWITCH 2", 4, 139, eGold, 0, 3, 82.0f, 3);
	if (!g_CrownSwitchData[1].state)
	{
		DrawInterfaceText("-", 4, 160, eWhite, 0, 3, 82.0f, 0);
	}
	else
	{
		sprintf(switch_message, "[%s] %s", g_CrownSwitchData[1].guild, g_CrownSwitchData[1].name);
		DrawInterfaceText(switch_message, 4, 160, eWhite, 0, 3, 82.0f, 0);
	}	
}

void ResetCrownSwitchData()
{
	for (int i = 0; i < 2; ++i)
	{
		if (!g_CrownSwitchData[i].state)
		{
			continue;
		}

		g_CrownSwitchData[i].Reset();
	}
}