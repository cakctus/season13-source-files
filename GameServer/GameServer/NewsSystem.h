#ifndef NEWS_H
#define NEWS_H

void NewsSystem(void* lpParam); 
class cNews
{
public:
	char ServerName[25];
	char ConnectNotice[100];
	int ConnectInfo;

	void ResetConfig();
	void LoadNotice();
	void Load();

	int Sections;
	struct sNews
	{
		char MSG[3][50];
		int Time;
		int Type;
	};

	struct sConfig
	{
		int IsNews;
	};

	sNews NewsSys[100];
	sConfig Config;
};
extern cNews News;
#endif