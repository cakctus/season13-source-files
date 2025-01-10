#pragma once
#include "DataServerProtocol.h"
struct _SDHP_ANS_RESTORE_ITEM_LIST
{
	PSWMSG_HEAD h;
	int userIndex;
	BYTE result;
	BYTE dbRestoreItemData[150];
	BYTE dbVersion;
};

class CRestoreItemDBSet
{
public:
	CRestoreItemDBSet();
	~CRestoreItemDBSet();

	int DSDB_SelectRestoreItem( char *AccountID, char *Name, int userIndex, _SDHP_ANS_RESTORE_ITEM_LIST *lpSendMsg); // idb
	void  DSDB_UpdateRestoreItem(char *AccountID, char *Name, BYTE *RestoreInventoryBuf);

};
extern CRestoreItemDBSet g_CRestoreItemDBSet;
