#pragma once

#define MAX_CRYWOLF_OBJ_INFO 500

class CCrywolfObjInfo
{
public:
	CCrywolfObjInfo();
	void Reset();
	bool AddObj(int aIndex);
public:
	int m_ObjCount;
	int m_ObjIndex[MAX_CRYWOLF_OBJ_INFO];
};
