#pragma once
#include "Protocol.h"


/* 3461 */
struct  _stMagicSquarePos
{
	BYTE m_btIndex;
	BYTE m_btPosX;
	BYTE m_btPosY;
	unsigned __int16 m_wTarget;
};

struct  _stIceBreathSkillInfo
{
	_stIceBreathSkillInfo()
	{
		this->Clear();
	}
	void Clear()
	{
		this->m_bAttack = 0;
		this->m_nAttackCount = 0;
		this->m_nAttackStartAngle = 0;
	}
	bool m_bAttack;
	int m_nAttackCount;
	unsigned __int16 m_nAttackStartAngle;
};
struct  _stIceBoomSkillInfo
{
	_stIceBoomSkillInfo()
	{
		this->Clear();
	}
	void Clear()
	{
		this->m_bAttack = 0;
		this->m_nAttackCount = 0;
		this->m_nMagicSquareCnt = 0;
		this->m_nAttackCount = 0;
		memset(&m_stIceBoomMagicSquarePos, 0, sizeof(m_stIceBoomMagicSquarePos));
	}
	bool m_bAttack;
	bool m_bAttacking;
	unsigned __int16 m_nMagicSquareCnt;
	int m_nAttackCount;
	_stMagicSquarePos m_stIceBoomMagicSquarePos[5];
};
/* 3463 */
struct _stKnowledgeAbsorbSkillInfo
{
	_stKnowledgeAbsorbSkillInfo()
	{
		this->Clear();
	}
	void Clear()
	{
		this->m_dwKnowledgeAbsorbTimer = 0;
		this->m_nRestrictStoneCount = 0;
		this->m_nRestrictStoneIndex1 = 0;
		this->m_nRestrictStoneIndex2 = 0;

	}
	unsigned int m_dwKnowledgeAbsorbTimer;
	int m_nRestrictStoneCount;
	int m_nRestrictStoneIndex1;
	int m_nRestrictStoneIndex2;
};
/* 3465 */
struct _tagPMSG_ICE_BREATH_ATTACK
{
	PSBMSG_HEAD h;
	unsigned __int16 wAttackerIndex;
	unsigned __int16 wTargetIndex;
};
/* 3464 */
struct  _stDevilIceSkillInfo
{
	_stDevilIceSkillInfo()
	{
		this->Clear();
	}
	void Clear()
	{
		this->m_bAttacking = 0;
		memset(&m_DevilIceMagicSquare, 0, sizeof(m_DevilIceMagicSquare));
	}
	bool m_bAttacking;
	int CountSent;
	_stMagicSquarePos m_DevilIceMagicSquare[3];
};

struct _tagPMSG_SEND_ICE_BOOM_POS
{
	PSBMSG_HEAD h;
	BYTE btStatus;
	BYTE btIndex;
	BYTE btPosX;
	BYTE btPosY;
};
struct _tagPMSG_SEND_DEVILICE_MAGIC_SQUARE_POS
{
	PSBMSG_HEAD h;
	BYTE btStatus;
	BYTE btPosX1;
	BYTE btPosY1;
	BYTE btPosX2;
	BYTE btPosY2;
	BYTE btPosX3;
	BYTE btPosY3;
};
/* 3478 */
struct _tagPMSG_SEND_ICE_BREATH_DIR
{
	PSBMSG_HEAD h;
	BYTE btState;
	unsigned __int16 wTargetIndex;
	BYTE btTargetX;
	BYTE btTargetY;
};
/* 3479 */
struct _tagPMSG_SEND_KNOWLEDGEABSORBINFO
{
	PSBMSG_HEAD h;
	BYTE btStatus;
};
class CNixiesLakeBoss
{
public:
	CNixiesLakeBoss();
	virtual ~CNixiesLakeBoss();
	void InitNixStatus(); // idb
	BYTE NixiesBossSkillRun( LPOBJ lpObj);
	void NixAttack( LPOBJ lpObj); // idb
	double CheckNixLifeStatus(LPOBJ lpObj);
	BYTE SummonRestrictStone( LPOBJ lpObj);
	BYTE SetDevilIceMagicSquare();
	int GetAvailableAttackSkillCount( LPOBJ lpObj); // idb
	void InitIceBreathAttack( LPOBJ lpObj, LPOBJ lpTargetObj); // idb
	void IceBreathAttack( LPOBJ lpObj); // idb
	bool IsIceBreathAttacking(); // idb
	void InitIceBoom(); // idb
	void IceBoomAttackReady( LPOBJ lpObj); // idb
	void SetIceBoomMagicSquare( LPOBJ lpObj); // idb
	void IceBoomAttackDamage( LPOBJ lpObj); // idb
	BYTE SetKnowledgeAbsorb( LPOBJ lpObj);
	void FailRemoveRestrictStone( LPOBJ lpObj); // idb
	void SuccessRemoveRestrictStone( LPOBJ lpObj); // idb
	void RemoveRestrictStone( LPOBJ lpObj); // idb
	void ClearRestrictStone(); // idb
	void SummonMonster();
	void StartImmune( LPOBJ lpObj); // idb
	void EndImmune( LPOBJ lpObj); // idb
	void NotifyIceBoomMagicSquarePos(); // idb
	void NotifyDevilIceMagicSquarePos(); // idb
	void NotifyIceBreathDir(BYTE status, LPOBJ lpTargetObj); // idb
	void DevilIceAttackDamage( LPOBJ lpObj); // idb
	void RefillLife(LPOBJ lpObj);
	void FailBossBattles( LPOBJ lpObj); // idb
	void DieNixiesBoss();
	void NotifyKnowledgeAbsorbInfo(BYTE status); // idb
	void DisableCountSkill(); // idb
	int SearchTarget(LPOBJ lpObj, int Distance);
	void SetRefillLifeDelay(); // idb
	int ChkRefillLifeDelay(); // idb
	void SendIceBoomMagicSquarePos( int aIndex, BYTE status, BYTE index, BYTE x, BYTE y); // idb
	void SendDevilIceMagicSquarePos( int aIndex, BYTE status, _stMagicSquarePos *squarePos); // idb
	void SendIceBreathDir( int aIndex, BYTE status, unsigned __int16 TargetIndex, BYTE x, BYTE y); // idb
	void SendKnowledgeAbsorb( int aIndex, BYTE status); // idb
	void RecvSendIceBreathAttack( _tagPMSG_ICE_BREATH_ATTACK *lpMsg, int nTargetIndex); // idb
private:

	unsigned int m_dwNixAttackDelay;
	bool m_bNixFirstBelowLife90Per;
	bool m_bNixFirstBelowLife30Per;
	bool m_bNixFirstBelowLife10Per;
	bool m_bUsedDevilIceLife80Per;
	bool m_bUsedDevilIceLife50Per;
	bool m_bUsedDevilIceLife5Per;
	ULONGLONG m_bUsedDevilIceDelay;

	bool m_bNixImmune;
	unsigned int m_dwIceRainDelay;
	unsigned int m_dwRefillLifeDelay;
	_stIceBreathSkillInfo m_stIceBreathInfo;
	_stIceBoomSkillInfo m_stIceBoomInfo;
	_stKnowledgeAbsorbSkillInfo m_stKnowAbsorbInfo;
	_stDevilIceSkillInfo m_stDevilIceInfo;
};
extern CNixiesLakeBoss g_NixiesLakeBoss; // idb
