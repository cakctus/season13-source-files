#include "stdafx.h"
#include "CBombHuntInfo.h"


CBombHuntInfo::CBombHuntInfo()
{
	this->Init();
}


CBombHuntInfo::~CBombHuntInfo()
{
}


//----- (00A64460) --------------------------------------------------------
void __thiscall CBombHuntInfo::Init()
{
	this->m_bBombHuntInfoLoad = 0;
}

//----- (00A64490) --------------------------------------------------------
void  CBombHuntInfo::Clear()
{
	int i; // [sp+4Ch] [bp-8h]@1

	this->m_iGameState = 0;
	this->m_iTotScore = 0;
	for (i = 0; i < 48; ++i)
	{
		this->m_tileAdjustBombCount[i] = 0;
		this->m_tileState[i] = 0;
		this->m_bBombExist[i] = 0;
	}
	this->m_nRemainBombCard = 11;
}

//----- (00A64530) --------------------------------------------------------
void  CBombHuntInfo::SetScore(int iScore)
{
	this->m_iTotScore = iScore;
}

//----- (00A64560) --------------------------------------------------------
void  CBombHuntInfo::SetGameState(int iGameState)
{
	this->m_iGameState = iGameState;
}

//----- (00A64590) --------------------------------------------------------
int  CBombHuntInfo::GetScore()
{
	return this->m_iTotScore;
}

//----- (00A645C0) --------------------------------------------------------
int  CBombHuntInfo::GetGameState()
{
	return this->m_iGameState;
}

//----- (00A645F0) --------------------------------------------------------
void  CBombHuntInfo::SetBombHuntInfoLoad(bool bBombHuntInfoLoad)
{
	this->m_bBombHuntInfoLoad = bBombHuntInfoLoad;
}

//----- (00A64620) --------------------------------------------------------
bool  CBombHuntInfo::GetBombHuntInfoLoad()
{
	return this->m_bBombHuntInfoLoad;
}
