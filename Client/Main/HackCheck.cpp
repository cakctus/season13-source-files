#include "stdafx.h"
#include "HackCheck.h"
#include "Protect.h"
#include "Util.h"
#include "Console.h"
#include "Protocol.h"

typedef DWORD(WINAPI* KERNELGETTICKCOUNT)();
typedef DWORD(WINAPI* WINMMTIMEGETTIME)();

KERNELGETTICKCOUNT HookGetTickCount;
WINMMTIMEGETTIME HooktimeGetTime;

BYTE GetTickCountTable[5];
BYTE timeGetTimeTable[5];

DWORD GetTickCountAddress;
DWORD timeGetTimeAddress;

typedef int(WINAPI*WSRECV)(SOCKET, char*, int, int);
typedef int(WINAPI*WSSEND)(SOCKET, char*, int, int);

WSRECV HookRecv;
WSSEND HookSend;
BYTE EncDecKey1;
BYTE EncDecKey2;

void DecryptData(BYTE* lpMsg, int size) // OK
{
	for (int n = 0; n < size; n++)
	{
		lpMsg[n] = (lpMsg[n] ^ EncDecKey1) - EncDecKey2;
	}
}

void EncryptData(BYTE* lpMsg, int size) // OK
{
	for (int n = 0; n < size; n++)
	{
		lpMsg[n] = (lpMsg[n] + EncDecKey2) ^ EncDecKey1;
	}
}

bool CheckSocketPort(SOCKET s) // OK
{
	SOCKADDR_IN addr;
	int addr_len = sizeof(addr);

	if (getpeername(s, (SOCKADDR*)&addr, &addr_len) == SOCKET_ERROR)
	{
		return 0;
	}

	if (PORT_RANGE(ntohs(addr.sin_port)) == 0)
	{
		return 0;
	}

	return 1;
}

int WINAPI MyRecv(SOCKET s, char* buf, int len, int flags) // OK
{
	int result = HookRecv(s, (char*)buf, len, flags);

	if (result == SOCKET_ERROR || result == 0)
	{
		return result;
	}

	if (CheckSocketPort(s) != 0)
	{
		DecryptData((BYTE*)buf, result);
	}

	return result;
}

int WINAPI MySend(SOCKET s, char* buf, int len, int flags) // OK
{
	if (CheckSocketPort(s) != 0)
	{
		EncryptData((BYTE*)buf, len);
	}

	return HookSend(s, buf, len, flags);
}

//By Leo - Fix some speedhacks
DWORD WINAPI MyGetTickCount() // OK
{
	if (GetTickCountTable[0] == 0xE9 || memcmp(GetTickCountTable, (void*)GetTickCountAddress, sizeof(GetTickCountTable)) != 0)
	{
		ExitProcess(0);
	}

	return HookGetTickCount();
}

DWORD WINAPI MytimeGetTime() // OK
{
	if (timeGetTimeTable[0] == 0xE9 || memcmp(timeGetTimeTable, (void*)timeGetTimeAddress, sizeof(timeGetTimeTable)) != 0)
	{
		ExitProcess(0);
	}

	return HooktimeGetTime();
}

void InitHackCheck() // OK
{
	/*WORD EncDecKey = 0;

	for (int n = 0; n < sizeof(gProtect.m_MainInfo.CustomerName); n++)
	{
		EncDecKey += (BYTE)(gProtect.m_MainInfo.CustomerName[n] ^ gProtect.m_MainInfo.ClientSerial[(n % sizeof(gProtect.m_MainInfo.ClientSerial))]);
	}

	EncDecKey1 = (BYTE)0xE2;
	EncDecKey2 = (BYTE)0x76;

	EncDecKey1 += LOBYTE(EncDecKey);
	EncDecKey2 += HIBYTE(EncDecKey);

	HookRecv = *(WSRECV*)(0x01403948);
	HookSend = *(WSSEND*)(0x01403938);

	SetDword(0x01403948, (DWORD)&MyRecv);
	SetDword(0x01403938, (DWORD)&MySend);*/

	GetTickCountAddress = (DWORD)GetProcAddress(GetModuleHandle("Kernel32.dll"), "GetTickCount");
	timeGetTimeAddress = (DWORD)GetProcAddress(GetModuleHandle("WINMM.dll"), "timeGetTime");

	memcpy(GetTickCountTable, (void*)GetTickCountAddress, sizeof(GetTickCountTable));
	memcpy(timeGetTimeTable, (void*)timeGetTimeAddress, sizeof(timeGetTimeTable));

	HookGetTickCount = *(KERNELGETTICKCOUNT*)(0x01403568);
	HooktimeGetTime = *(WINMMTIMEGETTIME*)(0x014038E0);

	*(DWORD*)(0x01403568) = (DWORD)MyGetTickCount;
	*(DWORD*)(0x014038E0) = (DWORD)MytimeGetTime;
}

BYTE CoordX = 0;
BYTE CoordY = 0;
int OldX = 0;
int OldY = 0;
ULONGLONG timer;
int stuckCount;
ULONGLONG stuckTimer;
ULONGLONG avoidByProtocol;

int durationSkillCount;
int attackSkillCount;
int attackCount;

void TestMoveHack()
{
	bool checkDistance = true;

	ULONGLONG elapsed = GetTickCount64() - timer;

	if (elapsed > 500)
	{
		PMSG_HACKLOG pMsg;
		pMsg.h.setE(0xF3, 0xE4, sizeof(pMsg));

		pMsg.durationSkillDelay = (durationSkillCount > 0) ? (WORD)(elapsed / durationSkillCount) : 0;
		pMsg.attackSkillDelay = (attackSkillCount > 0) ? (WORD)(elapsed / attackSkillCount) : 0;
		pMsg.attackDelay = (attackCount > 0) ? (WORD)(elapsed / attackCount) : 0;

		if (OldX == 0)
		{
			OldX = CoordX;
			OldY = CoordY;
			checkDistance = false;
		}

		if ((GetTickCount64() - avoidByProtocol) < 5000)
		{
			OldX = CoordX;
			OldY = CoordY;
			checkDistance = false;
			//Log.ConsoleOutPut(1, c_Blue, t_Default, "return Protocol");
		}

		if (checkDistance)
		{
			float ix = (float) (abs(OldX - CoordX) * abs(OldX - CoordX));
			float iy = (float) (abs(OldY - CoordY) * abs(OldY - CoordY));
			float distance = sqrt(ix + iy);

			//Log.ConsoleOutPut(1, c_Blue, t_Default, "Position distance: %f | From (%d , %d) To (%d , %d)",distance,OldX,OldY,CoordX,CoordY);

			if (distance > 5.0f)
			{
				stuckTimer = GetTickCount64();
				stuckCount++;

				if (stuckCount > 1)
				{
					Sleep((int)distance * 200);
				}
			}

			OldX = CoordX;
			OldY = CoordY;
		}		

		pMsg.stucks = stuckCount;
		DataSend((BYTE*)&pMsg, pMsg.h.size);

		durationSkillCount = 0;
		attackSkillCount = 0;
		attackCount = 0;

		timer = GetTickCount64();
	}

	if ((GetTickCount64() - stuckTimer) > 10000)
	{
		if (stuckCount > 0)
		{
			stuckCount--;
		}

		stuckTimer = GetTickCount64();
	}

	/*if (stuckCount > 5)
	{
		ExitProcess(0);
	}*/
}