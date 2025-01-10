#pragma once
class InstanceCounter
{
public:
	InstanceCounter();
	~InstanceCounter();

	int  CheckAvailableInstanceCounter(int nRequestUserCount, int nRequestMonsterCount);
	__int32 GetUsingInstanceObjCounter();
	void  IncInstanceCounter(int nRequestUserCount, int nRequestMonsterCount); // idb
	void  DecInstanceCounter(int nDecUserCount, int nDecMonsterCount); // idb
	void IncInstanceObjectCounter();
	void DecInstanceObjectCounter();
	__int32 GetReservedUserCounter();
	__int32 GetReservedMonsterCounter();

	__int32 m_nReservedUserCounter; // weak
	__int32 m_nReservedMonsterCounter; // weak
	__int32 m_nUsingInstanceObjCounter; // weak
};

