#pragma once

#include "Protocol.h"
#include "User.h"

struct PMSG_REQ_ACHERON_EVENT_ENTER
{
	PSBMSG_HEAD h;
};

struct PMSG_ANS_ACHERON_EVENT_ENTER
{
	PSBMSG_HEAD h;
	char btResult;
};

struct PMSG_ANS_NOTI_ENTER_EVENTMAP_ERROR
{
	PSBMSG_HEAD h;
	int nResult;
};

class CAcheronGuardianEvent
{
public:
	CAcheronGuardianEvent();
	virtual ~CAcheronGuardianEvent();
	void __thiscall CGReqAcheronEventEnter(PMSG_REQ_ACHERON_EVENT_ENTER* lpMsg, int iIndex);
};

