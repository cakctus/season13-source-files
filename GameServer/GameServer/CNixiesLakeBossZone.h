#pragma once
#include "Protocol.h"


struct _tagPMSG_REQ_CHK_ENTER_ON_NIXIES_NIX_BOSS_ZONE
{
	PSBMSG_HEAD h;
};
struct _tagPMSG_REQ_ENTER_ON_NIXIES_NIX_BOSS_ZONE
{
	PSBMSG_HEAD h;
};
struct _tagPMSG_ANS_INVITE_ON_NIXIES_NIX_BOSS_ZONE_WITH_PARTYMEMBER
{
	PSBMSG_HEAD h;
	BYTE btResponse;
};
/* 3472 */
struct _tagPMSG_ANS_CHK_ENTER_ON_NIXIES_NIX_BOSS_ZONE
{
	PSBMSG_HEAD h;
	BYTE btResult;
};
/* 3473 */
struct _tagPMSG_ANS_ENTER_ON_NIXIES_NIX_BOSS_ZONE
{
	PSBMSG_HEAD h;
	BYTE btResult;
};
/* 3474 */
struct _tagPMSG_REQ_INVITE_ON_NIXIES_NIX_BOSS_ZONE_TO_PARTYMEMBER
{
	PSBMSG_HEAD h;
};

/* 3475 */
struct _tagPMSG_REQ_RESULT_ON_NIXIES_NIX_BOSS_ZONE_TO_PARTYMEMBER
{
	PSBMSG_HEAD h;
	BYTE btResult;
};
/* 3480 */
struct _tagPMSG_SEND_KICK_OUT_USER_IN_BOSSZONE
{
	PSBMSG_HEAD h;
};
struct  _stBossZonePartyPlayers
{
	_stBossZonePartyPlayers()
	{
		this->Clear();
	}
	void Clear()
	{
	
		memset(&nPlayersIdx, -1, sizeof(nPlayersIdx));
		this->nPlayerCount = 0;
		this->nAcceptCount = 0;
		this->dwAnsWaitTimer = 0;
	}
	int nPlayersIdx[5];
	int nPlayerCount;
	int nAcceptCount;
	unsigned int dwAnsWaitTimer;
};
class CNixiesLakeBossZone
{
public:
	unsigned int m_dwBossRegenTimer;

	CNixiesLakeBossZone();
	virtual ~CNixiesLakeBossZone();
	void Clear(); // idb
	void InitNixiesBossZone( int nAttribute); // idb
	void ChkBossNixRegenTimer(); // idb
	void SetBossNixRegenTimer(); // idb
	void SetBossZoneState( char state); // idb
	BYTE GetBossZoneState(); // idb
	int AddMonster( int iMonIndex, int iBeginX, int iBeginY, int iEndX, int iEndY, int iMonAttr); // idb
	void ClearMonster(); // idb
	bool IsBossZone(int X, int Y);
	bool IsBossZoneMonSummonArea(int X, int Y);
	BYTE SetPosMonster( int aIndex, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY);
	BYTE GetBoxPosition( int mapnumber, int bx, int by, int ex, int ey, __int16 *mx, __int16 *my);
	BYTE ChkBossZonePlayer();
	int GetBossZonePlayerCnt(); // idb
	BYTE ChkEnterBossZone( int aIndex); // idb
	void EnterBossZone( int aIndex); // idb
	void PartyEnterBossZone(); // idb
	int HavePieceOfNixi(int aIndex);
	void UsePieceOfNixi(int aIndex, int nPos);
	BYTE ChkEnterPartyMemberToBossZone( int aIndex); // idb
	BYTE ChkEnterInviteMemberToBossZone( int aIndex);
	void InvitePartyMemberToBossZone( int aIndex); // idb
	void ChkPartyWaitTime(); // idb
	void SetBossDieTimer(); // idb
	void ChkBossDieTime(); // idb
	void LeaveBossZone(); // idb
	bool IsAbleNixiesBossViewPort( LPOBJ lpObj); // idb
	void RecvReqChkEnterBossZone( _tagPMSG_REQ_CHK_ENTER_ON_NIXIES_NIX_BOSS_ZONE *lpMsg, int aIndex); // idb
	void SendAnsChkEnterBossZone( int aIndex, BYTE btResult); // idb
	void RecvReqEnterBossZone( _tagPMSG_REQ_ENTER_ON_NIXIES_NIX_BOSS_ZONE *lpMsg, int aIndex); // idb
	void SendAnsEnterBossZone( int aIndex, BYTE btResult); // idb
	void SendInvitePartyMemberToBossZone( int aIndex); // idb
	void RecvReqEnterBossZoneByInvite( _tagPMSG_ANS_INVITE_ON_NIXIES_NIX_BOSS_ZONE_WITH_PARTYMEMBER *lpMsg, int aIndex); // idb
	void NotifyInviteResultPartyMemberToBossZone(BYTE btResult); // idb
	void SendInviteResultPartyMemberToBossZone( int aIndex, BYTE btResult); // idb
	void SendKickOutUserPopupInBossZone( int aIndex); // idb

	long long m_dwBossLastHitTimer;
private:

	BYTE m_btBossZoneState;
	bool m_bIsBossLive;	
	long long m_dwBossDieTimer;	

	_stBossZonePartyPlayers m_stPartyPlayers;
	_RTL_CRITICAL_SECTION m_criNixiesBossZone;
};
extern CNixiesLakeBossZone g_NixiesLakeBossZone;

