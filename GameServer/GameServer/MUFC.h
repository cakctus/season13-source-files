#include "user.h"

enum MUFC_Status
{
	MUFC_IDLE		= 0,
	MUFC_MOVED		= 1,
	MUFC_PREPARED	= 2,
	MUFC_STARTED	= 3,
	MUFC_DEATH		= 4,
	MUFC_FINISHED	= 5
};

struct MUFC_Damage
{
	int Timer;
	int DamageRate;
};

class CMUFC
{
public:
	CMUFC();
	virtual ~CMUFC();

	void FightStart(LPOBJ lpObj, int rounds);
	void ResetFight(LPOBJ lpObj);
	void FightRun();
	void FighterKilled(LPOBJ lpObj, LPOBJ lpTargetObj);
	void PrepareFighter(int number, LPOBJ lpTargetObj, LPOBJ lpObj);
	void SetNewRound();
	void Load(char* filename);
	void SendNotice(char* message, ...);

	void CalcWeaponOption(LPOBJ lpObj, CItem* lpItem);

	short Status;
	int FighterID[2];
	char FighterName[2][11];
	int StartCounter;
	int Rounds;	
	int ReSetTimer;	
	
	int FightTimer;
	MUFC_Damage FightDamageRate[32];
	int FightDamage;

	int fuckyou;

	int FighterVictories[2];
	int FighterDamage[2];
	int FighterHits[2];

	int fuckyouagain;
};

extern CMUFC gMUFC;