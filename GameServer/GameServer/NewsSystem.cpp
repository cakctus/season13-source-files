#include "StdAfx.h" 
#include "Configs.h"
#include "ChatSystem.h"
#include "NewsSystem.h"
#include "Logger.h"

cNews News;

void cNews::ResetConfig()
{
	Config.IsNews = 0;
	Sections = 0;
	for (int i = 0; i < 100; i++)
	{
		NewsSys[i].Time = 0;
		NewsSys[i].Type = 0;
		NewsSys[i].MSG[0][0] = NULL;
		NewsSys[i].MSG[1][0] = NULL;
		NewsSys[i].MSG[2][0] = NULL;
	}
}

void cNews::LoadNotice()
{
	ConnectInfo = Configs.GetInt(0, 1, 1, "Notice", "ConnectInfo", cfNEWS);
	GetPrivateProfileString("Notice", "ConnectNotice", "Welcome to the Jurassic World", ConnectNotice, sizeof(ConnectNotice), cfNEWS);
	Log.CheckProcent(ConnectNotice);
}
void cNews::Load()
{
	ResetConfig();
	Config.IsNews = Configs.GetInt(0, 1, 1, "News", "NewsSystemEnabled", cfNEWS);
	if (!Config.IsNews)return;

	Sections = Configs.GetInt(1, 99, 3, "News", "NewsSections", cfNEWS);
	char PState[10];

	for (int i = 1; i <= Sections; i++)
	{
		wsprintf(PState, "Section%d", i);
		NewsSys[i].Time = Configs.GetInt(1, 32767, 5, PState, "ShowTime", cfNEWS) * 60000;
		NewsSys[i].Type = Configs.GetInt(0, 1, 0, PState, "ShowType", cfNEWS);

		char PNews[6];
		for (int j = 0; j < 3; j++)
		{
			wsprintf(PNews, "News%d", j + 1);
			GetPrivateProfileStringA(PState, PNews, NULL, NewsSys[i].MSG[j], 50, cfNEWS);
		}
	}

	Log.ConsoleOutPut(1, c_Magenta, t_Default, "[V] [News System]\tLoaded %d news.", Sections);
}

void NewsSystem(void* lpParam)
{
	News.Config.IsNews = Configs.GetInt(0, 1, 1, "News", "NewsSystemEnabled", cfNEWS);
	while (News.Config.IsNews)
	{
		for (int i = 1; i <= News.Sections; i++)
		{
			Sleep(News.NewsSys[i].Time);
			for (int j = 0; j < 3; j++)
			{
				if (News.NewsSys[i].MSG[j] == NULL)
					continue;
				Log.CheckProcent(News.NewsSys[i].MSG[j]);
				Chat.MessageAll(MessageType(News.NewsSys[i].Type), NULL, News.NewsSys[i].MSG[j]);
			}
		}
	}
	_endthread();
} 