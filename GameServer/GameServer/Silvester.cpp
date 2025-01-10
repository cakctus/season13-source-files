//Class 673   Lord Silvester
#include "stdafx.h"
#include "Silvester.h"
#include "Monster.h"
#include "Notice.h"
#include "Message.h"
#include "ServerInfo.h"
CSilvester gSilvester;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSilvester::CSilvester() // OK
{
	this->ClearData();
}

CSilvester::~CSilvester() // OK
{

}
void CSilvester::ClearData()
{
	this->SilvesterStar = 0;
	memset(this->OpenName,0,sizeof(this->OpenName));
}
bool CSilvester::CheckRange(BYTE x,BYTE y,BYTE Tx,BYTE Ty)
{
	if(( x - Tx < 10 || y - Ty < 10) || ( Tx - x < 10 || Ty - y < 10))
	{
		return 1;
	}
	else{return 0;}
}
void CSilvester::SilvesterMonsterProc(LPOBJ lpObj,LPOBJ lpTarget)
{
	if(lpObj->Class ==673 && lpTarget->Type == OBJECT_USER)
	{
		if((CheckRange(lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) ==TRUE) && lpObj->LastAttackerID == -1)
		{
			this->SilvesterStar = 1;
			memcpy(this->OpenName, lpTarget->Name, sizeof(this->OpenName));
		}
	}
}

void CSilvester::MainProc() // OK
{
	if(gServerInfo.m_SilvesterEvent == 0)
	{
		return;
	}

	switch(this->SilvesterStar)
	{
	case 1:
		//gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(504),this->OpenName,5);
		this->SilvesterStar = 0;
		break;
	};
}

void CSilvester::SilvesterMonsterDieProc(LPOBJ lpObj,LPOBJ lpTarget)
{
	int aIndex = gObjMonsterGetTopHitDamageUser(lpObj);

	if(OBJECT_RANGE(aIndex) != 0)
	{
		lpTarget = &gObj[aIndex];
	}

	if(lpObj->Class == 673)
	{
		gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(505),lpTarget->Name);
	}
}