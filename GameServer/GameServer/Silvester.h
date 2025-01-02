//Class 673   Lord Silvester
#include "User.h"

class CSilvester
{
	public:
	CSilvester();
	virtual ~CSilvester();
	void ClearData();
	void Load(char* path);
	void MainProc();
	void SilvesterMonsterProc(LPOBJ lpObj,LPOBJ lpTarget);
	void SilvesterMonsterDieProc(LPOBJ lpObj,LPOBJ lpTarget);
	bool CheckRange(BYTE x,BYTE y,BYTE Tx,BYTE Ty);
	private:
	int SilvesterStar;
	char OpenName[11];
};
extern CSilvester gSilvester;