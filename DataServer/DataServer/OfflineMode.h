#pragma once

#include "StdAfx.h"
#include "DataServerProtocol.h"
#include <vector>

#define OFFLINE_MODE_RESTORE	1
#define OFFLINEMODE_DBMAX		8
#define OFFLINEMODE_DBPRICE		544
#define OFFLINEMODE_DBCREDIT	128

struct PMSG_DGANS_OFFMODE_START
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Password[21];
	char Name[11];
	DWORD TickCount;
};

struct PMSG_GDREQ_OFFMODE_CHAR
{
	PSBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short Number;
};

struct PMSG_GDLOAD_OFFMODE_DATA
{
	PWMSG_HEAD h;
	short Number;
	BYTE Attack;
	BYTE Ghost;
	BYTE PShopOpen;
	char PShopText[36];
};

struct PMSG_GDSAVE_OFFMODE_DATA
{
	PSBMSG_HEAD h;
	char Name[11];
	bool Status;
	BYTE Attack;
	BYTE Ghost;
	BYTE PShopOpen;
	char PShopText[36];
};

struct PMSG_GDREQ_OFFMODE_STATUS
{
	PSBMSG_HEAD h;
	char Name[11];
	BYTE Status;
};

struct OFFMODEUSER_DATA
{
	int uIndex;
	char AccountID[11];
	char Password[21];
	char Name[11];
};

class COfflineMode
{
public:
			COfflineMode();
			~COfflineMode();

	void	GDReqAllPlayer(int index);
	void	GDReqOffCharacter(PMSG_GDREQ_OFFMODE_CHAR* lpMsg, int uIndex);
	void	SelectData(int uIndex, int  Number, char* Name);
	void	UpdateData(PMSG_GDSAVE_OFFMODE_DATA* lpMsg, int uIndex);
	void	Run();
	
	bool	m_bQueenQuery;
	bool    m_RunActive;
	bool	m_ConnectedUser;
	DWORD   m_ConnectedTick;

	std::vector<OFFMODEUSER_DATA> m_users;
};

extern COfflineMode gOfflineMode;