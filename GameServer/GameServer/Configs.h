#ifndef CONFIGS_H
#define CONFIGS_H

#define cfNEWS	"..\\Data\\CustomsIA\\NewsSystem.txt"
#define cfGM	"..\\Data\\CustomsIA\\GMSystem.txt"
#define cfCOMANDS "..\\Data\\CustomsIA\\Commands.ini"
#define cfDROP		"..\\Data\\CustomsIA\\DropSystem.ini"

#define MAX_LEVEL_CMD	1000

class cConfigs
{
private: 
	void LoadCommands();  
public: 
	void LoadAll();
	long GetInt(long Min, long Max, long Default, LPCSTR BlockName, LPCSTR ConfigName, LPCSTR FolderName);
	char GetChar(long Min, long Max, unsigned char Default, LPCSTR BlockName, LPCSTR ConfigName, LPCSTR FolderName);   


	struct sCommands
	{  
		int PostLvl;
		DWORD PostPriceZen; 
		int PostColor;
		int PostDelay;
		 
		int IsPkClear;
		int PkClearForGm;
		int PkClearLevelReq;
		int PkClearType;
		DWORD PkClearPriceZen;
		DWORD PkClearPriceZenForAll; 

		int AddPointEnabled;
		int AddPointLevelReq;
		DWORD AddPriceZen; 
		DWORD MaxAddedPoints;
		DWORD MaxStats;

		int IsGg;
		int IsPost; 
		int IsDrop;
		int IsMake;
		int IsReload;
		int IsGmove;
		int IsSetPK;
		int IsTime;
		int IsOnline;
		int IsZenDrop;
		int IsStatus;
		int IsSetChar;
		int IsSetZen;
		int IsSkin;

		int IsHide;
		int IsFireworks;
		int IsClearInv;
		int IsWare;
		int IsSpawn;

		int IsMessagePopup;

	}Commands;  
	 
};
extern cConfigs Configs;

#endif