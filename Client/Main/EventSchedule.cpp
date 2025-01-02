#include "stdafx.h"
#include "Console.h"
#include "Interface.h"
#include "EventSchedule.h"

EventSchedule gEventSchedule;

EventSchedule::EventSchedule()
{

}

void EventSchedule::DrawScheduleWindow()
{
	if (!gInterface.Data[ScheduleBg].OnShow)
	{
		return;
	}

	DWORD ItemNameColor = eWhite;

	gInterface.DrawGUI(ScheduleBg, 115, 76);

	gInterface.DrawFormat(eGold, 136, 106, 352, 3, 1, "AGENDA DE EVENTOS, INVASOES E RESPAWNS");

	gInterface.DrawGUIEx(OficinaCloseButton1, 491, 103);

	float StartY = 80 + 140;
	float StartX = 115 + 15;

	WriteLine("Blood Castle", this->BCtime[0], this->BCtime[1], 0, StartX, StartY);
	StartY += 11;
	WriteLine("Devil Square", this->DStime[0], this->DStime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("Chaos Castle", this->CCtime[0], this->CCtime[1], 0, StartX, StartY);
	StartY += 11;
	WriteLine("Illusion Temple", this->ITtime[0], this->ITtime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("Kantru", this->KantruTime[0], this->KantruTime[1], 0, StartX, StartY);
	StartY += 11;
	WriteLine("Crywolf", this->CrywolfTime[0], this->CrywolfTime[1], 1, StartX, StartY);

	StartY = 80 + 140;
	StartX = 115 + 146;

	for (int i = 0; i < 14; i++)
	{
		WriteLine(this->InvasionsName[i], this->Invasions[i][0], this->Invasions[i][1], i % 2, StartX, StartY);
		StartY += 11;
	}

	StartY = 80 + 140;
	StartX = 115 + 275;

	WriteLine("[Kalima] Kundun", this->KundunTime[0], this->KundunTime[1], 0, StartX, StartY);
	StartY += 11;
	WriteLine("[Raklion] Selupan", this->SelupanTime[0], this->SelupanTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Icarus] Dark Phoenix", this->PhoenixTime[0], this->PhoenixTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Sw Dark] God of Darkness", this->GoDTime[0], this->GoDTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Sw Dark] Monster Swamp", this->MoSTime[0], this->MoSTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Nixies] Monster Water", this->MoWTime[0], this->MoWTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Nixies Lake] Nix", this->NixTime[0], this->NixTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Deep Dungeon 5] Gorgon", this->DpGorgonTime[0], this->DpGorgonTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Ferea] Ferea General", this->FGTime[0], this->FGTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Ferea] Lord of Ferea", this->LoFTime[0], this->LoFTime[1], 1, StartX, StartY);
	StartY += 11;
	WriteLine("[Alcmaru] Gnome", this->GnomeTime[0], this->GnomeTime[1], 1, StartX, StartY);

	if (gInterface.IsWorkZone(OficinaCloseButton1))
	{
		DWORD Color = eGray100;

		if (gInterface.Data[OficinaCloseButton1].OnClick)
		{
			Color = eGray150;
		}

		gInterface.DrawColoredGUIEx(OficinaCloseButton1, gInterface.Data[OficinaCloseButton1].X, gInterface.Data[OficinaCloseButton1].Y, Color);
		gInterface.DrawToolTip(gInterface.Data[OficinaCloseButton1].X + 30, gInterface.Data[OficinaCloseButton1].Y + 8, "Fechar");
	}	
}

bool EventSchedule::ScheduleWindowClose(DWORD Event)
{
	if (!gInterface.Data[ScheduleBg].OnShow || !gInterface.IsWorkZone(OficinaCloseButton1))
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

	gEventSchedule.CloseScheduleWindow();

	return false;
}

void EventSchedule::CloseScheduleWindow()
{
	gInterface.Data[ScheduleBg].OnShow = false;
	gInterface.ToggleSceneObjects(0);
	pSetCursorFocus = false;

	gInterface.ResetObject(ScheduleBg);
	gInterface.ResetObject(OficinaCloseButton1);
};

bool EventSchedule::EventWindowMain(DWORD Event)
{
	this->ScheduleWindowClose(Event);
	return true;
}

void EventSchedule::ProcessScheduleData( PMSG_SCHEDULE_EVENTS * Data)
{
	this->MakeDate(Data->BCtime, this->BCtime);
	this->MakeDate(Data->DStime, this->DStime);
	this->MakeDate(Data->CCtime, this->CCtime);
	this->MakeDate(Data->ITtime, this->ITtime);
	this->MakeDate(Data->KantruTime, this->KantruTime);
	this->MakeDate(Data->SelupanTime, this->SelupanTime);
	this->MakeDate(Data->KundunTime, this->KundunTime);
	this->MakeDate(Data->CrywolfTime, this->CrywolfTime);
	this->MakeDate(Data->PhoenixTime, this->PhoenixTime);
	this->MakeDate(Data->GoDTime, this->GoDTime);
	this->MakeDate(Data->MoSTime, this->MoSTime);
	this->MakeDate(Data->MoWTime, this->MoWTime);
	this->MakeDate(Data->NixTime, this->NixTime);
	this->MakeDate(Data->DpGorgonTime, this->DpGorgonTime);
	this->MakeDate(Data->FGTime, this->FGTime);
	this->MakeDate(Data->LoFTime, this->LoFTime);
	this->MakeDate(Data->GnomeTime, this->GnomeTime);

	gInterface.Data[ScheduleBg].OnShow = true;
	gInterface.ToggleSceneObjects(1);
	pSetCursorFocus = true;
}

void EventSchedule::ProcessScheduleData2( PMSG_SCHEDULE_INVASIONS * Data)
{
	this->Invasions[Data->Order][0] = -1;
	this->Invasions[Data->Order][1] = -1;
	ZeroMemory(this->InvasionsName[Data->Order],32);
	Data->Name[31] = '\0';

	this->MakeDate( Data->Time, this->Invasions[Data->Order] );
	strcpy(this->InvasionsName[Data->Order],Data->Name);
	this->InvasionsName[Data->Order][31] = '\0';
	//Log.ConsoleOutPut(1, c_Blue, t_Default, "Invasion: %d | Name: %s | Horas: %d | Minutos: %d",Data->Order,this->InvasionsName[Data->Order],this->Invasions[Data->Order][0],this->Invasions[Data->Order][1]);	
}

void EventSchedule::MakeDate( int value, int * var )
{
	if(value < 1)
	{
		var[0] = -1;
		var[1] = -1;
		return;
	}
	var[0] = (int) (value / 60);
	var[1] = value - (var[0] * 60);
}

void EventSchedule::WriteLine(char * EventName, int Hours, int Minutes, int lineOdd, int PositionX, int PositionY)
{
	int EventColor;
	char text[64];

	if(lineOdd == 0)
		EventColor = eWhite;
	else
		EventColor = eWhite180;

	if(Hours == 0 && Minutes < 10)
	{
		EventColor = eRed;
		wsprintf(text, "%d min.", Minutes);
		
	}
	else if(Hours == 0 && Minutes <= 59)
	{
		EventColor = eOrange;
		wsprintf(text, "%d min.", Minutes);
	}
	else if (Hours > -1 && Minutes > -1)
	{
		wsprintf(text, "%dh %02dm", Hours, Minutes);
	}
	else
	{
		EventColor = eExcellent;
		wsprintf(text, "Ativo");
	}

	DrawInterfaceText(text, PositionX, PositionY, EventColor, 0, 4, 118, 0);
	DrawInterfaceText(EventName, PositionX, PositionY, EventColor, 0, 1, 118, 0);
}