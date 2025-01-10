#ifndef GMSYSTEM_H
#define GMSYSTEM_H

class cGmSystem
{
public:
	struct GMSystem
	{
		char Name[11];
		int Drop;
		int Gg;
		int SetZen;
		int Status;
		int Gmove;
		int SetPK;
		int PkClear;
		int BanPlayer;
		int Reload;
		int Skin;
		int BanPost;
		int Voskl;
		int Disconnect;
		int Trace;
		int IsAdmin;
		int DisableChat;

		int Hide;
		int Fireworks;
		int ClearInv;
		int Spawn;

		int MessagePopup;
		int Make;
	};

	enum Commands {
		NONE = -1,
		cDrop,
		cGg,
		cSetZen,
		cStatus,
		cGmove,
		cSetPK,
		cPkClear,
		cBanPlayer,
		cReload,
		cSkin,
		cBanPost,
		cVoskl,
		cDisconnect,
		cTrace,
		cDisableChat,
		cHide,
		cFireworks,
		cClearInv,
		cSpawn,
		cMessagePopup,
		cMake
	};

	struct sConfig
	{
		int IsGMSystem;
		int IsGMInDB;
	};

	sConfig Config;
	GMSystem GM[255];
	int GMCount;

	void ResetConfig();
	void Load();
	int IsCommand(Commands cmd, char* name);
	int IsAdmin(char* name);
	bool IsGMBD(char* name);
	bool IsGMBD(int aIndex);
};
extern cGmSystem GmSystem;

#endif