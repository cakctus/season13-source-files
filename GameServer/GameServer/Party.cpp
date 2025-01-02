#include "stdafx.h"
#include "Party.h"
#include "EffectManager.h"
#include "Map.h"
#include "PartyMatching.h"
#include "ServerInfo.h"
#include "Util.h"
#include "Notice.h"
#include "GensSystem.h"
#include "MapManager.h"
#include "CNixiesLakeBossZone.h"
#include "GodOfDarkness.h"

CParty gParty;

CParty::CParty() // OK
{
	this->m_PartyCount = 0;
}

CParty::~CParty() // OK
{

}

bool CParty::IsParty(int index) // OK
{
	if(OBJECT_RANGE(index) == 0)
	{
		return 0;
	}

	if(this->m_PartyInfo[index].Count == 0)
	{
		return 0;
	}

	return 1;
}

bool CParty::IsLeader(int index,int aIndex) // OK
{
	if(this->IsParty(index) == 0)
	{
		return 0;
	}

	if(this->m_PartyInfo[index].Index[0] == aIndex)
	{
		return 1;
	}
	
	return 0;
}

bool CParty::IsMember(int index,int aIndex) // OK
{
	if(this->IsParty(index) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(this->m_PartyInfo[index].Index[n] == aIndex)
		{
			return 1;
		}
	}

	return 0;
}

int CParty::GetMemberCount(int index) // OK
{
	if(this->IsParty(index) == 0)
	{
		return 0;
	}

	return this->m_PartyInfo[index].Count;
}

int CParty::GetMemberIndex(int index,int number) // OK
{
	if(this->IsParty(index) == 0)
	{
		return -1;
	}

	if(number < 0 || number >= MAX_PARTY_USER)
	{
		return -1;
	}

	return this->m_PartyInfo[index].Index[number];
}

int CParty::GetMemberNumber(int index,int aIndex) // OK
{
	if(this->IsParty(index) == 0)
	{
		return -1;
	}

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(this->m_PartyInfo[index].Index[n] == aIndex)
		{
			return n;
		}
	}

	return -1;
}

bool CParty::Create(int aIndex) // OK
{
	if(this->IsParty(gObj[aIndex].PartyNumber) != 0)
	{
		return 0;
	}

	int count = this->m_PartyCount;

	for(int n=0;n < MAX_OBJECT;n++)
	{
		if(this->m_PartyInfo[count].Count == 0)
		{
			gObj[aIndex].PartyNumber = count;
			this->m_PartyInfo[count].Count = 1;
			this->m_PartyInfo[count].Index[0] = aIndex;
			this->m_PartyInfo[count].Index[1] = -1;
			this->m_PartyInfo[count].Index[2] = -1;
			this->m_PartyInfo[count].Index[3] = -1;
			this->m_PartyInfo[count].Index[4] = -1;
			this->GCPartyListSend(count);
			this->m_PartyCount = (((++this->m_PartyCount)>=MAX_OBJECT)?0:this->m_PartyCount);
			return 1;
		}
		else
		{
			count = (((++count)>=MAX_OBJECT)?0:count);
		}
	}

	return 0;
}

bool CParty::Destroy(int index) // OK
{
	if(this->IsParty(index) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) != 0)
		{
			gObj[this->m_PartyInfo[index].Index[n]].PartyNumber = -1;
			this->GCPartyDelMemberSend(this->m_PartyInfo[index].Index[n]);
			this->m_PartyInfo[index].Count--;
			this->m_PartyInfo[index].Index[n] = -1;
		}
	}

	return 1;
}

bool CParty::AddMember(int index,int aIndex) // OK
{
	if(this->IsParty(index) == 0)
	{
		return 0;
	}

	if(this->GetMemberNumber(index,aIndex) != -1)
	{
		return 0;
	}

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) == 0)
		{
			gObj[aIndex].PartyNumber = index;
			this->m_PartyInfo[index].Count++;
			this->m_PartyInfo[index].Index[n] = aIndex;
			this->GCPartyListSend(index);
			return 1;
		}
	}

	return 0;
}

bool CParty::DelMember(int index,int aIndex) // OK
{
	if(this->IsParty(index) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(this->m_PartyInfo[index].Index[n] == aIndex)
		{
			gObj[aIndex].PartyNumber = -1;
			this->m_PartyInfo[index].Count--;
			this->m_PartyInfo[index].Index[n] = -1;

			if (n == 0)
			{
				this->ChangeLeader(index, n);
			}

			this->GCPartyDelMemberSend(aIndex);
			this->GCPartyListSend(index);
			return 1;
		}
	}

	return 0;
}

void CParty::ChangeLeader(int index,int number) // OK
{
	if(this->IsParty(index) == 0)
	{
		return;
	}

	if (number > 0)
	{
		int temp = this->m_PartyInfo[index].Index[0];
		this->m_PartyInfo[index].Index[0] = this->m_PartyInfo[index].Index[number];
		this->m_PartyInfo[index].Index[number] = temp;
	}
	else
	{
		for (int n = 1; n < MAX_PARTY_USER; n++)
		{
			if (OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) != 0)
			{
				this->m_PartyInfo[index].Index[0] = this->m_PartyInfo[index].Index[n];
				this->m_PartyInfo[index].Index[n] = -1;
				break;
			}
		}
	}
}

bool CParty::AutoAcceptPartyRequest(LPOBJ lpObj,LPOBJ lpTarget)
{
	if(this->IsParty(lpTarget->PartyNumber) == 0)
	{
		if(gObjCheckAutoParty(lpObj,lpTarget) != 0)
		{
			if(this->IsParty(lpObj->PartyNumber) == 0)
			{
				if(this->Create(lpObj->Index) == 0 || this->AddMember(lpObj->PartyNumber,lpTarget->Index) == 0)
				{
					this->GCPartyResultSend(lpObj->Index,2);
					this->GCPartyResultSend(lpTarget->Index,2);
					this->Destroy(lpObj->PartyNumber);
					return 1;
				}
				else
				{
					gPartyMatching.GDPartyMatchingJoinCancelSend(lpObj->Index,1,1);
					gPartyMatching.GDPartyMatchingJoinCancelSend(lpTarget->Index,0,1);
					gPartyMatching.GDPartyMatchingJoinCancelSend(lpTarget->Index,1,1);
					return 1;
				}
			}
			else
			{
				if(this->IsLeader(lpObj->PartyNumber,lpObj->Index) != 0 || gObjCheckAutoParty(lpTarget,&gObj[this->GetMemberIndex(lpObj->PartyNumber,0)]) != 0)
				{
					if(this->AddMember(lpObj->PartyNumber,lpTarget->Index) == 0)
					{
						this->GCPartyResultSend(lpObj->Index,2);
						this->GCPartyResultSend(lpTarget->Index,2);
						return 1;
					}
					else
					{
						gPartyMatching.GDPartyMatchingJoinCancelSend(lpTarget->Index,0,1);
						gPartyMatching.GDPartyMatchingJoinCancelSend(lpTarget->Index,1,1);
						return 1;
					}
				}
			}
		}
	}
	else
	{
		if(gObjCheckAutoParty(lpObj,&gObj[this->GetMemberIndex(lpTarget->PartyNumber,0)]) != 0)
		{
			if(this->IsParty(lpObj->PartyNumber) == 0)
			{
				if(this->AddMember(lpTarget->PartyNumber,lpObj->Index) == 0)
				{
					this->GCPartyResultSend(lpObj->Index,2);
					this->GCPartyResultSend(lpTarget->Index,2);
					return 1;
				}
				else
				{
					gPartyMatching.GDPartyMatchingJoinCancelSend(lpObj->Index,0,1);
					gPartyMatching.GDPartyMatchingJoinCancelSend(lpObj->Index,1,1);
					return 1;
				}
			}
		}
	}

	return 0;
}

void CParty::CGPartyRequestRecv(PMSG_PARTY_REQUEST_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	if(gObjIsConnectedGP(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(lpObj->Interface.use != 0 || lpTarget->Interface.use != 0)
	{
		return;
	}

	if(CA_MAP_RANGE(lpTarget->Map) != 0 || CC_MAP_RANGE(lpTarget->Map) != 0 || IT_MAP_RANGE(lpTarget->Map) != 0 || DA_MAP_RANGE(lpTarget->Map) != 0 || DG_MAP_RANGE(lpTarget->Map) != 0 || IG_MAP_RANGE(lpTarget->Map) != 0)
	{
		this->GCPartyResultSend(aIndex,0);
		return;
	}

	if(OBJECT_RANGE(lpObj->PartyTargetUser) != 0 || OBJECT_RANGE(lpTarget->PartyTargetUser) != 0)
	{
		this->GCPartyResultSend(aIndex,0);
		return;
	}

	if(gServerInfo.m_GensSystemPartyLock != 0 && lpObj->GensFamily != lpTarget->GensFamily)
	{
		this->GCPartyResultSend(aIndex,6);
		return;
	}

	if(this->AutoAcceptPartyRequest(lpObj,lpTarget) != 0)
	{
		return;
	}

	if(this->IsParty(lpObj->PartyNumber) != 0 && this->IsLeader(lpObj->PartyNumber,aIndex) == 0)
	{
		this->GCPartyResultSend(aIndex,0);
		return;
	}

	if((lpTarget->Option & 1) == 0)
	{
		this->GCPartyResultSend(aIndex,1);
		return;
	}

	if(this->IsParty(lpTarget->PartyNumber) != 0)
	{
		this->GCPartyResultSend(aIndex,4);
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_PARTY;
	lpObj->Interface.state = 0;
	lpObj->InterfaceTime = GetTickCount();
	lpObj->TargetNumber = bIndex;
	lpObj->PartyTargetUser = bIndex;

	lpTarget->Interface.use = 1;
	lpTarget->Interface.type = INTERFACE_PARTY;
	lpTarget->Interface.state = 0;
	lpTarget->InterfaceTime = GetTickCount();
	lpTarget->TargetNumber = aIndex;

	PMSG_PARTY_REQUEST_SEND pMsg;

	pMsg.header.set(0x40,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);
	pMsg.index[1] = SET_NUMBERLB(aIndex);

	DataSend(bIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CParty::CGPartyRequestResultRecv(PMSG_PARTY_REQUEST_RESULT_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	if(gObjIsConnectedGP(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_PARTY)
	{
		return;
	}

	if(lpTarget->Interface.use == 0 || lpTarget->Interface.type != INTERFACE_PARTY)
	{
		return;
	}

	if(CA_MAP_RANGE(lpObj->Map) != 0 || CC_MAP_RANGE(lpObj->Map) != 0 || IT_MAP_RANGE(lpObj->Map) != 0 || DA_MAP_RANGE(lpObj->Map) != 0 || DG_MAP_RANGE(lpObj->Map) != 0 || IG_MAP_RANGE(lpObj->Map) != 0)
	{
		this->GCPartyResultSend(aIndex,0);
		goto CLEAR_JUMP;
	}

	if(lpMsg->result == 0)
	{
		this->GCPartyResultSend(aIndex,1);
		goto CLEAR_JUMP;
	}

	if(this->Create(bIndex) == 0)
	{
		if(this->AddMember(lpTarget->PartyNumber,aIndex) == 0)
		{
			this->GCPartyResultSend(aIndex,2);
			this->GCPartyResultSend(bIndex,2);
		}
		else
		{
			gPartyMatching.GDPartyMatchingJoinCancelSend(aIndex,0,1);
			gPartyMatching.GDPartyMatchingJoinCancelSend(aIndex,1,1);
		}
	}
	else
	{
		if(this->AddMember(lpTarget->PartyNumber,aIndex) == 0)
		{
			this->GCPartyResultSend(aIndex,2);
			this->GCPartyResultSend(bIndex,2);
			this->Destroy(lpTarget->PartyNumber);
		}
		else
		{
			gPartyMatching.GDPartyMatchingJoinCancelSend(aIndex,0,1);
			gPartyMatching.GDPartyMatchingJoinCancelSend(aIndex,1,1);
			gPartyMatching.GDPartyMatchingJoinCancelSend(bIndex,1,1);
		}
	}

	CLEAR_JUMP:

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
	lpObj->InterfaceTime = 0;
	lpObj->TargetNumber = -1;
	lpObj->PartyTargetUser = -1;

	lpTarget->Interface.use = 0;
	lpTarget->Interface.type = INTERFACE_NONE;
	lpTarget->Interface.state = 0;
	lpTarget->InterfaceTime = 0;
	lpTarget->TargetNumber = -1;
	lpTarget->PartyTargetUser = -1;
}

void CParty::CGPartyListRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_PARTY_LIST_SEND pMsg;

	pMsg.header.set(0x42,0);

	int size = sizeof(pMsg);

	pMsg.result = 0;

	pMsg.count = 0;

	if(this->IsParty(lpObj->PartyNumber) != 0)
	{
		PMSG_PARTY_LIST info;

		for(int n=0;n < MAX_PARTY_USER;n++)
		{
			if(OBJECT_RANGE(this->m_PartyInfo[lpObj->PartyNumber].Index[n]) == 0)
			{
				continue;
			}

			LPOBJ lpTarget = &gObj[this->m_PartyInfo[lpObj->PartyNumber].Index[n]];

			memcpy(info.name,lpTarget->Name,sizeof(info.name));

			info.number = n;

			info.map = lpTarget->Map;

			info.x = (BYTE)lpTarget->X;

			info.y = (BYTE)lpTarget->Y;

			info.CurLife = (DWORD)(lpTarget->Life);

			info.MaxLife = (DWORD)(lpTarget->MaxLife+lpTarget->AddLife);

			#if(GAMESERVER_UPDATE>=801)

			info.ServerCode = gServerInfo.m_ServerCode+1;

			info.CurMana = (DWORD)(lpTarget->Mana);

			info.MaxMana = (DWORD)(lpTarget->MaxMana+lpTarget->AddMana);

			#endif

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}

		pMsg.result = 1;
	}

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(aIndex,send,size);
}

void CParty::CGPartyDelMemberRecv(PMSG_PARTY_DEL_MEMBER_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	int bIndex = this->GetMemberIndex(lpObj->PartyNumber,lpMsg->number);

	if(gObjIsConnectedGP(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(aIndex != bIndex && this->IsLeader(lpObj->PartyNumber,aIndex) == 0)
	{
		return;
	}

	if(IT_MAP_RANGE(lpTarget->Map) != 0 || DG_MAP_RANGE(lpTarget->Map) != 0 || IG_MAP_RANGE(lpTarget->Map) != 0)
	{
		return;
	}

	if(this->GetMemberCount(lpObj->PartyNumber) <= 2)
	{
		this->Destroy(lpObj->PartyNumber);
	}
	else
	{
		this->DelMember(lpObj->PartyNumber,bIndex);
	}
}

void CParty::GCPartyResultSend(int aIndex,BYTE result) // OK
{
	PMSG_PARTY_RESULT_SEND pMsg;

	pMsg.header.set(0x41,sizeof(pMsg));

	pMsg.result = result;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CParty::GCPartyListSend(int index) // OK
{
	if(this->IsParty(index) == 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_PARTY_LIST_SEND pMsg;

	pMsg.header.set(0x42,0);

	int size = sizeof(pMsg);

	pMsg.result = 1;

	pMsg.count = 0;

	PMSG_PARTY_LIST info;

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[this->m_PartyInfo[index].Index[n]];

		memcpy(info.name,lpObj->Name,sizeof(info.name));

		info.number = n;
		info.map = lpObj->Map;
		info.x = (BYTE)lpObj->X;
		info.y = (BYTE)lpObj->Y;
		info.CurLife = (DWORD)(lpObj->Life);
		info.MaxLife = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
		info.ServerCode = gServerInfo.m_ServerCode+1;
		info.CurMana = (DWORD)(lpObj->Mana);
		info.MaxMana = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
		info.UserState = lpObj->Connected;
		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) != 0)
		{
			DataSend(this->m_PartyInfo[index].Index[n],send,size);
		}
	}

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) != 0)
		{
			gEffectManager.GCPartyEffectListSend(&gObj[this->m_PartyInfo[index].Index[n]]);
		}
	}
}

void CParty::GCPartyDelMemberSend(int aIndex) // OK
{
	PBMSG_HEAD pMsg;
	pMsg.set(0x43,sizeof(pMsg));
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.size);
}

void CParty::GCPartyLifeSend(int index) // OK
{
	if(this->IsParty(index) == 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_PARTY_LIFE_SEND pMsg;

	pMsg.header.set(0x44,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_PARTY_LIFE info;

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[this->m_PartyInfo[index].Index[n]];

		info.life = (BYTE)(lpObj->Life/((lpObj->MaxLife+lpObj->AddLife)/100));
		info.mana = (BYTE)(lpObj->Mana/((lpObj->MaxMana+lpObj->AddMana)/100));

		memcpy(info.name,lpObj->Name,sizeof(info.name));

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size = size;
	
	memcpy(send,&pMsg,sizeof(pMsg));

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) != 0)
		{
			DataSend(this->m_PartyInfo[index].Index[n],send,size);
		}
	}

	for(int n=0;n < MAX_PARTY_USER;n++)
	{
		if(OBJECT_RANGE(this->m_PartyInfo[index].Index[n]) != 0)
		{
			gEffectManager.GCPartyEffectListSend(&gObj[this->m_PartyInfo[index].Index[n]]);
		}
	}

	/*for(int n=0;n < MAX_PARTY_USER;n++)
	{
		LPOBJ lpObj = &gObj[this->m_PartyInfo[index].Index[n]];
		if(lpObj->Map == MAP_FEREA)
		{
			if(!gEffectManager.CheckEffect(lpObj,EFFECT_FEREA_PARTY_EXP))
			{
				gEffectManager.AddEffect(lpObj,0,EFFECT_FEREA_PARTY_EXP,0,n+5,0,0,0);
			}
		}
	}*/
}

void CParty::CGReqPartyLeaderChange(PMSG_REQ_CHANGE_PARTY_LEADER* lpMsg, int aIndex)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}	

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (this->IsLeader(lpObj->PartyNumber, aIndex) == 0)
	{
		GCChangePartyLeaderSend(aIndex, 0, 0);
		return;
	}

	BYTE btResult = 0x00;
	int nPartyNum = lpObj->PartyNumber;

	for (int n = 0; n < MAX_PARTY_USER; n++)
	{
		int nIndex = gParty.m_PartyInfo[nPartyNum].Index[n];

		if (OBJECT_RANGE(nIndex) == FALSE)
		{
			continue;
		}

		if (CA_MAP_RANGE(gObj[nIndex].Map) != 0 || CC_MAP_RANGE(gObj[nIndex].Map) != 0 || IT_MAP_RANGE(gObj[nIndex].Map) != 0 || DA_MAP_RANGE(gObj[nIndex].Map) != 0 || DG_MAP_RANGE(gObj[nIndex].Map) != 0 || IG_MAP_RANGE(gObj[nIndex].Map) != 0)
		{
			this->GCChangePartyLeaderSend(aIndex, 0, 0);
			return;
		}
	}

	char szNewLeaderName[11] = { 0 };
	char szOldLeaderName[11] = { 0 };
	int nNewLeaderNumber = -1;
	int nNewLeaderDBnumber = -1;
	BYTE btPartyIdx;

	memcpy(szOldLeaderName, gObj[aIndex].Name, 10);
	memcpy(szNewLeaderName, lpMsg->name, 10);

	if (this->RevisionIndexUser(nPartyNum, btPartyIdx, szNewLeaderName) == FALSE)
	{
		//GDReqSwapLeader(aIndex, szOldLeaderName, szNewLeaderName);
		return;
	}

	nNewLeaderNumber = this->GetMemberIndex(lpObj->PartyNumber, btPartyIdx);

	if (nNewLeaderNumber == -1)
	{
		GCChangePartyLeaderSend(aIndex, 0, 0);
		return;
	}

	if (gObjIsConnectedGP(nNewLeaderNumber) == FALSE)
	{
		GCChangePartyLeaderSend(aIndex, 0, 0);
		return;
	}

	if (CA_MAP_RANGE(gObj[nNewLeaderNumber].Map) != 0 || CC_MAP_RANGE(gObj[nNewLeaderNumber].Map) != 0 || IT_MAP_RANGE(gObj[nNewLeaderNumber].Map) != 0 || DA_MAP_RANGE(gObj[nNewLeaderNumber].Map) != 0 || DG_MAP_RANGE(gObj[nNewLeaderNumber].Map) != 0 || IG_MAP_RANGE(gObj[nNewLeaderNumber].Map) != 0)
	{
		GCChangePartyLeaderSend(aIndex, 0, 0);
		return;
	}

	this->ChangeLeader(lpObj->PartyNumber, btPartyIdx);

	this->GCPartyListSend(lpObj->PartyNumber);

	this->GCChangePartyLeaderSend(aIndex, 1, 1);
}

BOOL CParty::RevisionIndexUser(int party_number, BYTE &index, char* szName)
{
	if (this->IsParty(party_number) == FALSE)
	{
		return FALSE;
	}

	char szMemberName[11] = { 0 };
	memcpy(szMemberName, szName, 10);

	for (int i = 0; i < MAX_PARTY_USER; i++)
	{
		int nNumber = m_PartyInfo[party_number].Index[i];

		if (nNumber >= 0 && nNumber < MAX_OBJECT)
		{
			if (strcmp(szMemberName, gObj[nNumber].Name) == 0)
			{
				index = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CParty::GCChangePartyLeaderSend(int aIndex, BYTE btResult, BYTE btType)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	PMSG_CHANGE_PARTY_LEADER pMsg;

	pMsg.h.set(0xEF, 0x07, sizeof(pMsg));
	pMsg.btResult = btResult;
	pMsg.btSendMassage = btType;

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}

void CParty::CGReqMoveToMember(PMSG_REQ_PARTY_MOVE_REQUEST* lpMsg, int aIndex)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (gEffectManager.CheckStunEffect(lpObj) != 0 || gEffectManager.CheckImmobilizeEffect(lpObj) != 0)
	{
		return;
	}

	if (GetTickCount64() - lpObj->LastPartyTeleportTime < gServerInfo.m_PartyTeleportDelay)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Voce ainda nao pode teleportar novamente.");
		return;
	}

	if (lpObj->AccountLevel < 1)
	{
		GCMessagePopupSend(lpObj, "Recurso disponivel apenas para contas VIP.");
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		return;
	}

	if (lpMsg->slot > MAX_PARTY_USER)
	{
		return;
	}

	int partyNumber = lpObj->PartyNumber;
	int partySlot = lpMsg->slot;

	if (this->IsParty(partyNumber) == 0)
	{
		return;
	}

	int bIndex = this->GetMemberIndex(partyNumber, partySlot);

	if (aIndex == bIndex)
	{
		return;
	}

	if (gObjIsConnectedGP(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	int map = lpTarget->Map;

	if (KALIMA_MAP_RANGE(map) || CA_MAP_RANGE(map) || DS_MAP_RANGE(map) || BC_MAP_RANGE(map) || CC_MAP_RANGE(map) ||
		IT_MAP_RANGE(map) || DA_MAP_RANGE(map) || DG_MAP_RANGE(map) || IG_MAP_RANGE(map) ||
		map == MAP_CASTLE_SIEGE || map == MAP_LAND_OF_TRIALS || map == MAP_DUNGEON || map == MAP_CRYWOLF ||
		(map == MAP_KNICKS && g_NixiesLakeBossZone.IsBossZone(lpTarget->X, lpTarget->Y) == 1) ||
		g_GodOfDarkness.IsBossZone(lpTarget->Map, lpTarget->X, lpTarget->Y) == 1
		)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "O alvo esta em local nao permitido para teleportar.");
		return;
	}

	if (map == MAP_ATLANS && (lpObj->Inventory[8].IsItem() != 0 && (lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 2) || lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index == GET_ITEM(13, 3)))) // Uniria,Dinorant
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Equipamento inapropriado para teleportar ao local do alvo");
		return;
	}

	if ((map == MAP_ICARUS || map == MAP_KANTURU3) && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index != GET_ITEM(13, 3) && lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index != GET_ITEM(13, 37)))
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Equipamento inapropriado para teleportar ao local do alvo");
		return;
	}

	if (gMapManager.GetMapGensBattle(map) != 0)
	{
		if (lpObj->GensFamily == GENS_FAMILY_NONE || lpObj->GensFamily != lpTarget->GensFamily)
		{
			gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Existem restricoes de Gens que impedem esse teleporte");
			return;
		}
	}

	int px = lpTarget->X;
	int py = lpTarget->Y;

	if (gObjGetRandomFreeLocation(lpTarget->Map, &px, &py, 4, 4, 10) == 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Nao foi possivel mover neste momento.");
	}
	else
	{
		lpObj->LastPartyTeleportTime = GetTickCount64();
		gObjSummonAlly(lpObj, map, px, py);
	}
}

int CParty::GetNearMemberCount(LPOBJ lpObj)
{
	LPOBJ lpPartyObj;
	int partynum = lpObj->PartyNumber;

	if (gParty.IsParty(partynum) == 0)
	{
		return 0;
	}

	int partycount = gParty.m_PartyInfo[partynum].Count;
	int retcount = 1;

	for (int n = 0; n < MAX_PARTY_USER; n++)
	{
		int number = gParty.m_PartyInfo[partynum].Index[n];

		if (number >= 0)
		{
			lpPartyObj = &gObj[number];

			if (lpObj->Map == lpPartyObj->Map && lpObj->Index != number)
			{
				int dis = gObjCalcDistance(lpObj, &gObj[number]);

				if (dis < 10)
				{
					retcount++;
				}
			}
		}
	}

	return retcount;
}

BOOL CParty::EnterIllusionTemplePartyAuth(int partynumber, int userindex)
{
	if (IsParty(partynumber) == FALSE)
	{
		return FALSE;
	}

	for (int i = 0; i < MAX_PARTY_USER; i++)
	{
		if (m_PartyInfo[partynumber].Index[i] == userindex)
		{
			m_PartyInfo[partynumber].IllusionTempleAccepted[i] = true;
			return TRUE;
		}
	}

	return FALSE;
}

bool CParty::AllAgreeEnterIllusionTemple(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return false;
	}

	BYTE bReturn = 0;

	for (int i = 0; i < MAX_PARTY_USER; i++)
	{
		if (m_PartyInfo[partynumber].IllusionTempleAccepted[i] == true)
		{
			bReturn++;
		}
	}

	if (bReturn < gServerInfo.m_IllusionTempleMinUser)
	{
		return false;
	}

	return true;
}