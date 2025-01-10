#include "stdafx.h"
#include "CriticalSection.h"

CCriticalSection::CCriticalSection() // OK
{
	InitializeCriticalSection(&this->m_critical);
}

CCriticalSection::~CCriticalSection() // OK
{
	DeleteCriticalSection(&this->m_critical);
}

void CCriticalSection::lock() // OK
{
	EnterCriticalSection(&this->m_critical);
}

void CCriticalSection::unlock() // OK
{
	LeaveCriticalSection(&this->m_critical);
}
