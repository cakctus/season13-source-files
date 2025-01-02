#include "stdafx.h"
#include "RandomManager.h"
#include "Util.h"

CRandomManager::CRandomManager() // OK
{
	this->Clear();
}

CRandomManager::~CRandomManager() // OK
{

}

void CRandomManager::Clear() // OK
{
	this->m_MaxRate = 0;

	this->m_RandomManagerInfo.clear();
}

bool CRandomManager::GetRandomElement(int* value) // OK
{
	if (this->m_MaxRate == 0)
	{
		return 0;
	}

	if (this->m_RandomManagerInfo.empty() != 0)
	{
		return 0;
	}

	int RandomNumber = GetLargeRand() % this->m_MaxRate;

	for (std::vector<RANDOM_MANAGER_INFO>::iterator it = this->m_RandomManagerInfo.begin(); it != this->m_RandomManagerInfo.end(); it++)
	{
		if (RandomNumber >= it->MinRate && RandomNumber < it->MaxRate)
		{
			(*value) = it->value;
			return 1;
		}
	}

	return 0;
}

bool CRandomManager::GetRandomElementWithPerc(int* value, int* perc) // OK
{
	if (this->m_MaxRate == 0)
	{
		return 0;
	}

	if (this->m_RandomManagerInfo.empty() != 0)
	{
		return 0;
	}

	int RandomNumber = GetLargeRand() % this->m_MaxRate;

	for (std::vector<RANDOM_MANAGER_INFO>::iterator it = this->m_RandomManagerInfo.begin(); it != this->m_RandomManagerInfo.end(); it++)
	{
		if (RandomNumber >= it->MinRate && RandomNumber < it->MaxRate)
		{
			(*value) = it->value;
			(*perc) = (int) ((float)((float)((float)it->MaxRate - (float)it->MinRate) / (float)this->m_MaxRate) * 100.0f);
			return 1;
		}
	}

	return 0;
}

bool CRandomManager::GetRandomElementUnique(int* value) // OK
{
	if (this->m_MaxRate == 0)
	{
		return 0;
	}

	if (this->m_RandomManagerInfo.empty() != 0)
	{
		return 0;
	}

	int RandomNumber = GetLargeRand() % this->m_MaxRate;

	for (std::vector<RANDOM_MANAGER_INFO>::iterator it = this->m_RandomManagerInfo.begin(); it != this->m_RandomManagerInfo.end(); it++)
	{
		if (RandomNumber >= it->MinRate && RandomNumber < it->MaxRate)
		{
			(*value) = it->value;
			this->m_RandomManagerInfo.erase(it);
			return 1;
		}
	}

	return 0;
}

bool CRandomManager::GetRandomElement(WORD* value) // OK
{
	if (this->m_MaxRate == 0)
	{
		return 0;
	}

	if (this->m_RandomManagerInfo.empty() != 0)
	{
		return 0;
	}

	int RandomNumber = GetLargeRand() % this->m_MaxRate;

	for (std::vector<RANDOM_MANAGER_INFO>::iterator it = this->m_RandomManagerInfo.begin(); it != this->m_RandomManagerInfo.end(); it++)
	{
		if (RandomNumber >= it->MinRate && RandomNumber < it->MaxRate)
		{
			(*value) = it->value;
			return 1;
		}
	}

	return 0;
}

bool CRandomManager::GetRandomElement(BYTE* value) // OK
{
	if (this->m_MaxRate == 0)
	{
		return 0;
	}

	if (this->m_RandomManagerInfo.empty() != 0)
	{
		return 0;
	}

	int RandomNumber = GetLargeRand() % this->m_MaxRate;

	for (std::vector<RANDOM_MANAGER_INFO>::iterator it = this->m_RandomManagerInfo.begin(); it != this->m_RandomManagerInfo.end(); it++)
	{
		if (RandomNumber >= it->MinRate && RandomNumber < it->MaxRate)
		{
			(*value) = it->value;
			return 1;
		}
	}

	return 0;
}

bool CRandomManager::GetRandomElementUnique(BYTE* value) // OK
{
	if (this->m_MaxRate == 0)
	{
		return 0;
	}

	if (this->m_RandomManagerInfo.empty() != 0)
	{
		return 0;
	}

	int RandomNumber = GetLargeRand() % this->m_MaxRate;

	int sorteios = 0;

	while (sorteios < 50)
	{
		sorteios++;

		for (std::vector<RANDOM_MANAGER_INFO>::iterator it = this->m_RandomManagerInfo.begin(); it != this->m_RandomManagerInfo.end(); it++)
		{
			if (RandomNumber >= it->MinRate && RandomNumber < it->MaxRate)
			{
				(*value) = it->value;
				this->m_RandomManagerInfo.erase(it);
				return 1;
			}
		}
		RandomNumber = GetLargeRand() % this->m_MaxRate;
	}

	return 0;
}

void CRandomManager::AddElement(int value, int rate) // OK
{
	if (rate == 0)
	{
		return;
	}

	RANDOM_MANAGER_INFO info;

	info.value = value;

	info.MinRate = this->m_MaxRate;

	info.MaxRate = (this->m_MaxRate = (this->m_MaxRate + rate));

	this->m_RandomManagerInfo.push_back(info);
}
