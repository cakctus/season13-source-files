#include "stdafx.h"
#include "CRestoreItemDBSet.h"
#include"QueryManager.h"
CRestoreItemDBSet g_CRestoreItemDBSet;

CRestoreItemDBSet::CRestoreItemDBSet()
{
}


CRestoreItemDBSet::~CRestoreItemDBSet()
{
}
//void __thiscall CRestoreItemDBSet::CRestoreItemDBSet(CRestoreItemDBSet *this)
//{
//	char v1; // [sp+Ch] [bp-D8h]@1
//	CRestoreItemDBSet *const thisa; // [sp+D0h] [bp-14h]@1
//	int v3; // [sp+E0h] [bp-4h]@1
//
//	memset(&v1, 0xCCu, 0xCCu);
//	thisa = this;
//	CDBConBase::CDBConBase((CDBConBase *)&this->vfptr);
//	v3 = 0;
//	thisa->vfptr = (CDBConBaseVtbl *)&CRestoreItemDBSet::`vftable';
//		CQuery::CQuery(&thisa->m_DBQuery);
//}
//// 5911DC: using guessed type int (__stdcall *CRestoreItemDBSet::`vftable')(int);
//
//
//
////----- (0044BBC0) --------------------------------------------------------
//void __thiscall CRestoreItemDBSet::~CRestoreItemDBSet(CRestoreItemDBSet *this)
//{
//	char v1; // [sp+Ch] [bp-D8h]@1
//	CRestoreItemDBSet *const thisa; // [sp+D0h] [bp-14h]@1
//	int v3; // [sp+E0h] [bp-4h]@1
//
//	memset(&v1, 0xCCu, 0xCCu);
//	thisa = this;
//	this->vfptr = (CDBConBaseVtbl *)&CRestoreItemDBSet::`vftable';
//		v3 = 0;
//	CQuery::~CQuery(&thisa->m_DBQuery);
//	v3 = -1;
//	CDBConBase::~CDBConBase((CDBConBase *)&thisa->vfptr);
//}
// 5911DC: using guessed type int (__stdcall *CRestoreItemDBSet::`vftable')(int);

//----- (0044BC50) --------------------------------------------------------
//int  CRestoreItemDBSet::Connect()
//{
//	int result; // eax@2
//	char v2; // [sp+Ch] [bp-CCh]@1
//	CRestoreItemDBSet *const thisa; // [sp+D0h] [bp-8h]@1
//
//	memset(&v2, 0xCCu, 0xCCu);
//
//	if (CQuery::Connect(&this->m_DBQuery, 3, szDbConnectDsn, szDbConnectID, szDbConnectPass))
//	{
//		result = 1;
//	}
//	else
//	{
//		MsgBox("CMonsterKillInfoDbSet ODBC Connect Fail");
//		result = 0;
//	}
//	return result;
//}

//----- (0044BCC0) --------------------------------------------------------
int  CRestoreItemDBSet::DSDB_SelectRestoreItem(char *AccountID, char *Name, int userIndex, _SDHP_ANS_RESTORE_ITEM_LIST *lpSendMsg)
{



	gQueryManager.ExecQuery("EXEC WZ_GetLoadRestoreInventory '%s', '%s'", AccountID, Name);

	lpSendMsg->userIndex = userIndex;
	gQueryManager.Fetch();
	gQueryManager.GetAsBinary("", lpSendMsg->dbRestoreItemData, sizeof(lpSendMsg->dbRestoreItemData));
	gQueryManager.Close();
	if (lpSendMsg->dbRestoreItemData[0]==-1)
	{
		lpSendMsg->result = 1;
		return 0;
	}
	else
	{
		
		if (gQueryManager.ExecQuery("SELECT DbVersion FROM T_RestoreItem_Inventory WHERE AccountID='%s' AND [Name]='%s'", AccountID, Name))
		{
			if (gQueryManager.Fetch()== SQL_NO_DATA)
			{
				gQueryManager.Close();
				lpSendMsg->result = 5;
				return 0;
			}
			else
			{
				lpSendMsg->dbVersion = gQueryManager.GetAsInteger("DbVersion");
				lpSendMsg->result = 0;
				gQueryManager.Close();
				return 1;
			}
		}
		else
		{
			gQueryManager.Close();
			lpSendMsg->result = 4;
			return 0;

		}
	}
	return 0;
}
// 5CE110: using guessed type int `CRestoreItemDBSet::DSDB_SelectRestoreItem'::`2'::__LINE__Var;
// 68372C4: using guessed type void (*LogAddTD)(char *, ...);

//----- (0044BF70) --------------------------------------------------------
void CRestoreItemDBSet::DSDB_UpdateRestoreItem( char *AccountID, char *Name, BYTE *RestoreInventoryBuf)
{
	gQueryManager.BindParameterAsBinary(1, RestoreInventoryBuf, 150);
	gQueryManager.ExecQuery("EXEC WZ_SetSaveRestoreInventory '%s', '%s', ?", AccountID, Name);
	gQueryManager.Close();

	return;
}
