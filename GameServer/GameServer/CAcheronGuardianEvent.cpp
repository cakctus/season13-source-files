#include "StdAfx.h"
#include "CAcheronGuardianEvent.h"

CAcheronGuardianEvent gAcheronGuardianEvent;

CAcheronGuardianEvent::CAcheronGuardianEvent() // OK
{

}

CAcheronGuardianEvent::~CAcheronGuardianEvent() // OK
{

}

//----- (00E47200) --------------------------------------------------------
/*void __thiscall CAcheronGuardianEvent::CGReqAcheronEventEnter(PMSG_REQ_ACHERON_EVENT_ENTER* lpMsg, int iIndex)
{
	int v3;
	int v4;
	//MapClassType* v5;
	BOOL v6;
	PMSG_ANS_ACHERON_EVENT_ENTER pMsg;
	PMSG_ANS_NOTI_ENTER_EVENTMAP_ERROR stEnterResult;
	char attr;
	OBJECTSTRUCT* lpObj;

	if (iIndex >= 0)
		v6 = iIndex <= 11999;
	else
		v6 = 0;
	if (v6)
	{
		lpObj = &gObj[iIndex];
		if (gObjIsConnected(iIndex))
		{
			if (!gObj[iIndex].Teleport && !((*(_DWORD*)&gObj[iIndex].m_ActState >> 2) & 1))
			{
				v3 = lpObj->Y;
				v4 = lpObj->X;
				v5 = MapClass::operator[](&MapC, lpObj->MapNumber);
				attr = MapClassType::GetAttr(v5, v4, v3);
				if (attr & 1)
				{
					if (lpObj->m_bUsePartyMatching)
					{
						PHeadSubSetB((char*)&stEnterResult.h, 239, 9, 8);
						stEnterResult.nResult = -1;
						DataSend(iIndex, (char*)&stEnterResult.h, 8u);
					}
					else
					{
						PMSG_ANS_ACHERON_EVENT_ENTER::PMSG_ANS_ACHERON_EVENT_ENTER(&pMsg);
						pMsg.h.c = -63;
						pMsg.h.size = 5;
						pMsg.h.headcode = -8;
						pMsg.h.subcode = 76;
						pMsg.btResult = 1;
						if (CAcheronGuardianEvent::IsPlayStart(thisa) == 1)
						{
							LogAddTD("[AcheronGuardianEvent] [%s][%s] Acheron Event Enter", gObj[iIndex].AccountID, gObj[iIndex].Name);
							gObjMoveGate(iIndex, 426);
						}
						else
						{
							DataSend(lpObj->m_Index, (char*)&pMsg.h, 5u);
						}
					}
				}
			}
		}
	}
}*/

