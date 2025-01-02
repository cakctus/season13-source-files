#pragma once

typedef unsigned __int64 QWORD;
typedef float flt;

//#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT 0x0600

//#define _CRT_NON_CONFORMING_SWPRINTFS
//#define _CRT_SECURE_NO_WARNINGS

// Windows Header Files
// System Include
//#define NOMINMAX

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <math.h>
#include <stdlib.h>
//#include <winsock2.h>
#include <Mmsystem.h>
#include "detours.h"
#include <vector>
#include <string>
#include <gl/GL.h>
#include <malloc.h>
#include <memory>
#include <tchar.h>
#include <time.h>
#include <stdarg.h>
#include <process.h>
#include "Offset.h"
#include "PrintPlayer.h"
#include "shellapi.h"
#include "resource.h"


#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"detours.lib")
#pragma comment(lib,"WINMM")
#pragma comment(lib,"libcurl.lib")
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"..\\..\\Util\\cryptopp\\Release\\cryptlib.lib")

extern bool gReconnect;
extern int Resets;
extern int GResets;
extern char* LevelText;
extern char UserAccount[11];

extern DWORD ViewIndex;
extern DWORD ViewHP;
extern DWORD ViewSD;
extern DWORD ViewMP;
extern DWORD ViewAG;

extern DWORD ViewMaxHP;
extern DWORD ViewMaxSD;
extern DWORD ViewMaxMP;
extern DWORD ViewMaxAG;

extern DWORD LevelUpPoints;

extern int showCSbar;

extern BOOL showPVPon;

extern BOOL showSchedule;
extern BOOL showOficina;

extern HINSTANCE hins;

extern char* HardwareId;

struct CrownSwitchData
{
	WORD index;
	BYTE state;
	BYTE join_side;
	char guild[9];
	char name[11];
	int accumulated_time;

	CrownSwitchData()
	{
		this->Reset();
	}

	void Reset()
	{
		this->index = -1;
		this->state = 0;
		this->join_side = 0;
		this->accumulated_time = 0;
		memset(this->guild, 0, 9);
		memset(this->name, 0, 11);
	}
};

extern CrownSwitchData g_CrownSwitchData[3];

struct TimeStruct
{
	TimeStruct()
	{
		this->Init();
	}

	void Init()
	{
		this->Tick = GetTickCount64();
		this->Time = 0;
	}

	void Start(DWORD Time)
	{
		this->Tick = GetTickCount64();
		this->Time = Time;
	}

	void Start()
	{
		this->Tick = GetTickCount64();
	}

	bool Elapsed()
	{
		if (GetTickCount64() > (this->Tick + this->Time))
		{
			return true;
		}

		return false;
	}

	ULONGLONG GetRemain()
	{
		return GetTickCount64() - this->Tick;
	}

	ULONGLONG Tick;
	DWORD Time;
	DWORD Progress;
};