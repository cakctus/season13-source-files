#include "stdafx.h"
#include "InstanceCounter.h"
#include "Util.h"


InstanceCounter::InstanceCounter()
{
	this->m_nReservedMonsterCounter = 0;
	this->m_nReservedUserCounter = 0;
	this->m_nUsingInstanceObjCounter = 0;
}


InstanceCounter::~InstanceCounter()
{
}


//----- (00EB25C0) --------------------------------------------------------
int  InstanceCounter::CheckAvailableInstanceCounter(int nRequestUserCount, int nRequestMonsterCount)
{
	int result; // eax@2

	if (this->m_nUsingInstanceObjCounter < 1000)
	{
		LogAdd(LOG_BLACK,
			"[INSTANCE] [COUNT][%s](#1) gUserCnt(%d), gMonCnt(%d), Reserved User(%d), Reserved Mon(%d), Req User(%d), Req Mon(%d)",
			"InstanceCounter::CheckAvailableInstanceCounter",
			gObjTotalUser,
			gObjMonCount,
			this->m_nReservedUserCounter,
			this->m_nReservedMonsterCounter,
			nRequestUserCount,
			nRequestMonsterCount);
		if (this->m_nReservedMonsterCounter < gObjMonCount)
			this->m_nReservedMonsterCounter = gObjMonCount;
		if (nRequestUserCount + this->m_nReservedUserCounter < gObjCount
			&& nRequestUserCount + this->m_nReservedUserCounter < MAX_OBJECT_USER)
		{
			if (nRequestMonsterCount + this->m_nReservedMonsterCounter < MAX_OBJECT_MONSTER)
			{
				result = 0;
			}
			else
			{
				LogAdd(LOG_BLACK,
					"[INSTANCE][WARN] Instance counter, over the monster count.(Reserve:%d)(Request:%d)",
					this->m_nReservedMonsterCounter,
					nRequestMonsterCount);
				result = 10;
			}
		}
		else
		{
			LogAdd(LOG_BLACK,
				"[INSTANCE][WARN] Instance counter, over the user count.(Reserve:%d)(Request:%d)(%d)(%d)",
				this->m_nReservedUserCounter,
				nRequestUserCount,
				gObjCount,
				1000);
			result = 9;
		}
	}
	else
	{
		LogAdd(LOG_BLACK,
			"[INSTANCE][FATAL] Instance counter, over the using MAX instance object.(%d)",
			this->m_nUsingInstanceObjCounter);
		result = 8;
	}
	return result;
}
// 139B414: using guessed type int gObjCount;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// ABDEE28: using guessed type int gObjTotalUser;
// 104F9328: using guessed type volatile __int32 InstanceCounter::m_nReservedUserCounter;
// 104F932C: using guessed type volatile __int32 InstanceCounter::m_nReservedMonsterCounter;
// 104F9330: using guessed type volatile __int32 InstanceCounter::m_nUsingInstanceObjCounter;

//----- (00EB2710) --------------------------------------------------------
__int32 InstanceCounter::GetUsingInstanceObjCounter()
{
	return this->m_nUsingInstanceObjCounter;
}
// 104F9330: using guessed type volatile __int32 InstanceCounter::m_nUsingInstanceObjCounter;

//----- (00EB2730) --------------------------------------------------------
void  InstanceCounter::IncInstanceCounter(int nRequestUserCount, int nRequestMonsterCount)
{

	LogAdd(LOG_BLACK,
		"[INSTANCE] [COUNT][%s](#1) gUserCnt(%d), gMonCnt(%d), Reserved User(%d), Reserved Mon(%d), Req User(%d), Req Mon(%d)",
		"InstanceCounter::IncInstanceCounter",
		gObjTotalUser,
		gObjMonCount,
		this->m_nReservedUserCounter,
		this->m_nReservedMonsterCounter,
		nRequestUserCount,
		nRequestMonsterCount);
	this->m_nReservedUserCounter += nRequestUserCount;
	this->m_nReservedMonsterCounter += nRequestMonsterCount;
	if (this->m_nReservedUserCounter > gObjCount || this->m_nReservedUserCounter > MAX_OBJECT_USER)
		LogAdd(LOG_BLACK,
			"[INSTANCE][FATAL] Instance counter, over the user count.(Reserve:%d)(%d)(%d)",
			this->m_nReservedUserCounter,
			gObjCount,
			1000);
	if (this->m_nReservedMonsterCounter >= MAX_OBJECT_MONSTER)
		LogAdd(LOG_BLACK,
			"[INSTANCE][FATAL] Instance counter, over the monster count.(Reserve:%d)",
			this->m_nReservedMonsterCounter);
	LogAdd(LOG_BLACK,
		"[INSTANCE] [COUNT][%s](#2) gUserCnt(%d), gMonCnt(%d), Reserved User(%d), Reserved Mon(%d), Req User(%d), Req Mon(%d)",
		"InstanceCounter::IncInstanceCounter",
		gObjTotalUser,
		gObjMonCount,
		this->m_nReservedUserCounter,
		this->m_nReservedMonsterCounter,
		nRequestUserCount,
		nRequestMonsterCount);
	this->IncInstanceObjectCounter();
}


//----- (00EB2870) --------------------------------------------------------
void __thiscall InstanceCounter::DecInstanceCounter(int nDecUserCount, int nDecMonsterCount)
{

	LogAdd(LOG_BLACK,
		"[INSTANCE] [COUNT][%s](#1) gUserCnt(%d), gMonCnt(%d), Reserved User(%d), Reserved Mon(%d), Req User(%d), Req Mon(%d)",
		"InstanceCounter::DecInstanceCounter",
		gObjTotalUser,
		gObjMonCount,
		this->m_nReservedUserCounter,
		this->m_nReservedMonsterCounter,
		nDecUserCount,
		nDecMonsterCount);
	this->m_nReservedUserCounter -= nDecUserCount;
	this->m_nReservedMonsterCounter -= nDecMonsterCount;
	if (this->m_nReservedUserCounter < 0 || this->m_nReservedMonsterCounter < 0)
		LogAdd(LOG_BLACK,
			"[INSTANCE][FATAL] Instance counter is error.(RU:%d)(RM:%d)",
			this->m_nReservedUserCounter,
			this->m_nReservedMonsterCounter);
	if (this->m_nReservedUserCounter < 0)
		this->m_nReservedUserCounter = 0;
	if (this->m_nReservedMonsterCounter < 0)
		this->m_nReservedMonsterCounter = 0;
	LogAdd(LOG_BLACK,
		"[INSTANCE] [COUNT][%s](#2) gUserCnt(%d), gMonCnt(%d), Reserved User(%d), Reserved Mon(%d), Req User(%d), Req Mon(%d)",
		"InstanceCounter::DecInstanceCounter",
		gObjTotalUser,
		gObjMonCount,
		this->m_nReservedUserCounter,
		this->m_nReservedMonsterCounter,
		nDecUserCount,
		nDecMonsterCount);
	this->DecInstanceObjectCounter();
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// ABDEE28: using guessed type int gObjTotalUser;
// 104F9328: using guessed type volatile __int32 InstanceCounter::m_nReservedUserCounter;
// 104F932C: using guessed type volatile __int32 InstanceCounter::m_nReservedMonsterCounter;

//----- (00EB29B0) --------------------------------------------------------
void InstanceCounter::IncInstanceObjectCounter()
{
	++this->m_nUsingInstanceObjCounter;
}
// 104F9330: using guessed type volatile __int32 InstanceCounter::m_nUsingInstanceObjCounter;

//----- (00EB29E0) --------------------------------------------------------
void InstanceCounter::DecInstanceObjectCounter()
{
	if (--this->m_nUsingInstanceObjCounter < 0)
	{
		LogAdd(LOG_BLACK,"[INSTANCE][FATAL] Instance counter is minus value. (%d)", this->m_nUsingInstanceObjCounter);
		this->m_nUsingInstanceObjCounter = 0;
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// 104F9330: using guessed type volatile __int32 InstanceCounter::m_nUsingInstanceObjCounter;

//----- (00EB2A30) --------------------------------------------------------
__int32 InstanceCounter::GetReservedUserCounter()
{
	return this->m_nReservedUserCounter;
}
// 104F9328: using guessed type volatile __int32 InstanceCounter::m_nReservedUserCounter;

//----- (00EB2A50) --------------------------------------------------------
__int32 InstanceCounter::GetReservedMonsterCounter()
{
	return this->m_nReservedMonsterCounter;
}
// 104F932C: using guessed type volatile __int32 InstanceCounter::m_nReservedMonsterCounter;