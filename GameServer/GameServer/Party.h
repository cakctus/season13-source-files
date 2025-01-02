// Party.h: interface for the CParty class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"
#include "User.h"

#define MAX_PARTY_USER 5
#define MAX_PARTY_DISTANCE 10

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_PARTY_REQUEST_RECV
{
	PBMSG_HEAD header; // C1:40
	BYTE index[2];
};

struct PMSG_PARTY_REQUEST_RESULT_RECV
{
	PBMSG_HEAD header; // C1:41
	BYTE result;
	BYTE index[2];
};

struct PMSG_PARTY_DEL_MEMBER_RECV
{
	PBMSG_HEAD header; // C1:43
	BYTE number;
};

struct PMSG_REQ_CHANGE_PARTY_LEADER
{
	PSBMSG_HEAD header; // C1:EF:07 C->GS
	char name[11];
};

struct PMSG_REQ_PARTY_MOVE_REQUEST
{
	PSBMSG_HEAD header;
	BYTE slot;
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_PARTY_REQUEST_SEND
{
	PBMSG_HEAD header; // C1:40
	BYTE index[2];
};

struct PMSG_PARTY_RESULT_SEND
{
	PBMSG_HEAD header; // C1:41
	BYTE result;
};

struct PMSG_PARTY_LIST_SEND
{
	PBMSG_HEAD header; // C1:42
	BYTE result;
	BYTE count;
};

struct PMSG_PARTY_LIST
{
	char name[10];
	BYTE number;
	BYTE map;
	BYTE x;
	BYTE y;
	DWORD CurLife;
	DWORD MaxLife;
	DWORD ServerCode;
	DWORD CurMana;
	DWORD MaxMana;
	int UserState;
};

struct PMSG_PARTY_LIFE_SEND
{
	PBMSG_HEAD header; // C1:44
	BYTE count;
};

struct PMSG_PARTY_LIFE
{
	BYTE life;
	BYTE mana;
	char name[11];
};

typedef struct PMSG_CHANGE_PARTY_LEADER
{
	PSBMSG_HEAD h; // C1:EF:07 GS->C
	BYTE btResult;
	BYTE btSendMassage;
};

//**********************************************//
//**********************************************//
//**********************************************//

struct PARTY_INFO
{
	int Count;
	int Index[MAX_PARTY_USER];
	bool IllusionTempleAccepted[MAX_PARTY_USER];
};

class CParty
{
public:
	CParty();
	virtual ~CParty();
	bool IsParty(int index);
	bool IsLeader(int index,int aIndex);
	bool IsMember(int index,int aIndex);
	int GetMemberCount(int index);
	int GetMemberIndex(int index,int number);
	int GetMemberNumber(int index,int aIndex);
	bool Create(int aIndex);
	bool Destroy(int index);
	bool AddMember(int index,int aIndex);
	bool DelMember(int index,int aIndex);
	void ChangeLeader(int index,int number);
	bool AutoAcceptPartyRequest(LPOBJ lpObj,LPOBJ lpTarget);
	void CGPartyRequestRecv(PMSG_PARTY_REQUEST_RECV* lpMsg,int aIndex);
	void CGPartyRequestResultRecv(PMSG_PARTY_REQUEST_RESULT_RECV* lpMsg,int aIndex);
	void CGPartyListRecv(int aIndex);
	void CGPartyDelMemberRecv(PMSG_PARTY_DEL_MEMBER_RECV* lpMsg,int aIndex);
	void GCPartyResultSend(int aIndex,BYTE result);
	void GCPartyListSend(int index);
	void GCPartyDelMemberSend(int aIndex);
	void GCPartyLifeSend(int index);
	void CGReqPartyLeaderChange(PMSG_REQ_CHANGE_PARTY_LEADER* lpMsg, int aIndex);
	BOOL RevisionIndexUser(int party_number, BYTE& index, char* szName);
	void GCChangePartyLeaderSend(int aIndex, BYTE btResult, BYTE btType);
	void CGReqMoveToMember(PMSG_REQ_PARTY_MOVE_REQUEST* lpMsg, int aIndex);
	int GetNearMemberCount(LPOBJ lpObj);
	BOOL EnterIllusionTemplePartyAuth(int partynumber, int userindex);
	bool AllAgreeEnterIllusionTemple(int partynumber);
public:
	int m_PartyCount;
	PARTY_INFO m_PartyInfo[MAX_OBJECT];
};

extern CParty gParty;
