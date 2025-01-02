#include "stdafx.h"
#include "Reconnect.h"
#include "HackCheck.h"
#include "Protect.h"
#include "Protocol.h"
#include "Util.h"
#include "Interface.h"

char GameServerAddress[16];
WORD GameServerPort;
char ReconnectAccount[11];
char ReconnectPassword[21];
char ReconnectName[11];
char ReconnectMapServerAddress[16];
WORD ReconnectMapServerPort;
DWORD ReconnectStatus = RECONNECT_STATUS_NONE;
DWORD ReconnectProgress = RECONNECT_PROGRESS_NONE;
ULONGLONG ReconnectCurTime = 0;
ULONGLONG ReconnectMaxTime = 0;
DWORD ReconnectCurWait = 0;
DWORD ReconnectMaxWait = 0;
DWORD ReconnectAuthSend = 0;
DWORD ReconnectHelperOn = 0;

void InitReconnect() // OK
{
	SetCompleteHook(0xE8, 0x0088111E, &ReconnectMainProc);

	if (!gReconnect)
		return;

	SetCompleteHook(0xE8,0x0045FA0E,&ReconnectGetAccountInfo);
	SetCompleteHook(0xE8,0x00B2E796,&ReconnectGetAccountInfo);
	SetCompleteHook(0xE9,0x005131EC,&ReconnectCheckConnection);
	SetCompleteHook(0xE9,0x00BE4C8A,&ReconnectCloseSocket);
	SetCompleteHook(0xE9,0x0084760B,&ReconnectMenuExitGame);
	SetCompleteHook(0xE8,0x0045FA6A,&ReconnectCreateConnection);
	SetCompleteHook(0xE8,0x00511BAA,&ReconnectCreateConnection);
	SetCompleteHook(0xE8,0x0053823C,&ReconnectCreateConnection);
	SetCompleteHook(0xE8,0x00B2E7FF,&ReconnectCreateConnection);
	SetCompleteHook(0xE8,0x00C0BBEE,&ReconnectCreateConnection);	
}

void ReconnectMainProc() // OK
{
	((void(*)())0x00626374)();

	if(SceneFlag != 6)
	{
		return;
	}

	gInterface.Run();

	if (!gReconnect)
	{
		return;
	}

	if (ReconnectStatus != RECONNECT_STATUS_RECONNECT)
	{
		return;
	}

	ReconnectDrawInterface();

	if((GetTickCount64()-ReconnectMaxTime) > ReconnectMaxWait)
	{
		ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT,RECONNECT_PROGRESS_NONE,0,0);
		((void(__thiscall*)(void*))0x00BE4C78)((void*)0x0A1EA680);
		return;
	}

	if((GetTickCount64()-ReconnectCurTime) < ReconnectCurWait)
	{
		return;
	}

	switch(ReconnectProgress)
	{
		case RECONNECT_PROGRESS_NONE:
			ReconnecGameServerLoad();
			break;
		case RECONNECT_PROGRESS_CONNECTED:
			ReconnecGameServerAuth();
			break;
	}

	ReconnectCurTime = GetTickCount64();
}

void ReconnectDrawInterface() // OK
{
	if (!gReconnect)
		return;

	float StartX = (MAX_WIN_WIDTH/2.0f)-(160.0f/2.0f);

	float StartY = 100.0f;

	float progress = ((ReconnectMaxWait==0)?0:(((GetTickCount64()-ReconnectMaxTime)*150.0f)/(float)ReconnectMaxWait));

	progress = ((progress>150)?150:progress);

	pRenderImage(0x7A65, StartX, StartY, 160.0f, 18.0f);

	pRenderImage(0x7A66,(StartX+5.0f),(StartY+5.0f),progress,8.0f);

	char buff[256];

	switch(ReconnectProgress)
	{
		case RECONNECT_PROGRESS_NONE:
			wsprintf(buff, "Reconectando...");
			DrawInterfaceText(buff, (int)StartX, (int)(StartY + 5.0f), 0xFFFFFFFF, 0, 3, 160.0f, 0);
			break;
		case RECONNECT_PROGRESS_CONNECTED:
			wsprintf(buff, "Autenticando...");
			DrawInterfaceText(buff, (int)StartX, (int)(StartY + 5.0f), 0xFFFFFFFF, 0, 3, 160.0f, 0);
			break;
		case RECONNECT_PROGRESS_JOINED:
			wsprintf(buff, "Aguardando informacoes do personagem...");
			DrawInterfaceText(buff, (int)StartX, (int)(StartY + 5.0f), 0xFFFFFFFF, 0, 3, 160.0f, 0);
			break;
		case RECONNECT_PROGRESS_CHAR_LIST:
			wsprintf(buff, "Carregando personagem...");
			DrawInterfaceText(buff, (int)StartX, (int)(StartY + 5.0f), 0xFFFFFFFF, 0, 3, 160.0f, 0);
			break;
	}
}

void ReconnectSetInfo(DWORD status,DWORD progress,DWORD CurWait,DWORD MaxWait) // OK
{
	if (!gReconnect)
		return; 
	
	ReconnectStatus = status;

	ReconnectProgress = progress;

	ReconnectCurTime = GetTickCount64();

	ReconnectMaxTime = GetTickCount64();

	ReconnectCurWait = CurWait;

	ReconnectMaxWait = MaxWait;

	ReconnectAuthSend = ((status==RECONNECT_STATUS_NONE)?0:ReconnectAuthSend);

	ReconnectHelperOn = ((status==RECONNECT_STATUS_NONE)?0:ReconnectHelperOn);
}

void ReconnecGameServerLoad() // OK
{
	if (!gReconnect)
		return; 
	
	if(ReconnectCreateConnection(GameServerAddress,GameServerPort) != 0)
	{
		*(DWORD*)(MAIN_CONNECTION_STATUS) = 1;

		ReconnectSetInfo(RECONNECT_STATUS_RECONNECT,RECONNECT_PROGRESS_CONNECTED,10000,30000);
	}
}

void ReconnecGameServerAuth() // OK
{
	if (!gReconnect)
		return; 
	
	if(((ReconnectAuthSend==0)?(ReconnectAuthSend++):ReconnectAuthSend) != 0)
	{
		return;
	}

	PMSG_CONNECT_ACCOUNT_SEND pMsg;

	pMsg.header.setE(0xF1,0x01,sizeof(pMsg));

	PacketArgumentEncrypt(pMsg.account,ReconnectAccount,(sizeof(ReconnectAccount)-1));

	PacketArgumentEncrypt(pMsg.password,ReconnectPassword,(sizeof(ReconnectPassword)-1));

	pMsg.TickCount = GetTickCount();

	pMsg.ClientVersion[0] = (*(BYTE*)(0x01609500))-1;
	pMsg.ClientVersion[1] = (*(BYTE*)(0x01609501))-2;
	pMsg.ClientVersion[2] = (*(BYTE*)(0x01609502))-3;
	pMsg.ClientVersion[3] = (*(BYTE*)(0x01609503))-4;
	pMsg.ClientVersion[4] = (*(BYTE*)(0x01609504))-5;

	memcpy(pMsg.ClientSerial,(void*)0x01609508,sizeof(pMsg.ClientSerial));

	DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void ReconnectOnCloseSocket() // OK
{
	if (!gReconnect)
		return; 
	
	if(SceneFlag == 6 && ReconnectStatus != RECONNECT_STATUS_DISCONNECT)
	{
		ReconnectSetInfo(RECONNECT_STATUS_RECONNECT,RECONNECT_PROGRESS_NONE,10000,600000);
		ReconnectAuthSend = 0;
		ReconnectHelperOn = *(BYTE*)(pGetAIController()+0x08);
		ReconnectViewportDestroy();
		memcpy(ReconnectName,(void*)(*(DWORD*)(MAIN_CHARACTER_STRUCT)+0x54),sizeof(ReconnectName));
	}
}

void ReconnectOnMapServerMove(char* address,WORD port) // OK
{
	if (!gReconnect)
		return; 
	
	if(ReconnectStatus != RECONNECT_STATUS_RECONNECT || ReconnectProgress == RECONNECT_PROGRESS_CHAR_LIST)
	{
		strcpy_s(ReconnectMapServerAddress,address);
		ReconnectMapServerPort = port;
		ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT,((ReconnectProgress==RECONNECT_PROGRESS_CHAR_LIST)?ReconnectProgress:RECONNECT_PROGRESS_NONE),0,0);
	}
}

void ReconnectOnMapServerMoveAuth(BYTE result) // OK
{
	if (!gReconnect)
		return; 
	
	if(ReconnectStatus != RECONNECT_STATUS_RECONNECT)
	{
		if(result == 1)
		{
			ReconnectSetInfo(((ReconnectProgress==RECONNECT_PROGRESS_CHAR_LIST)?ReconnectStatus:RECONNECT_STATUS_NONE),((ReconnectProgress==RECONNECT_PROGRESS_CHAR_LIST)?ReconnectProgress:RECONNECT_PROGRESS_NONE),0,0);
		}
		else
		{
			ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT,RECONNECT_PROGRESS_NONE,0,0);
		}
	}
}

void ReconnectOnConnectAccount(BYTE result) // OK
{
	if (!gReconnect)
		return; 
	
	if(ReconnectProgress == RECONNECT_PROGRESS_CONNECTED)
	{
		if(ReconnectAuthSend != 0)
		{
			if(result == 1)
			{
				PMSG_CHARACTER_LIST_SEND pMsg;
				pMsg.header.set(0xF3,0x00,sizeof(pMsg));
				DataSend((BYTE*)&pMsg,pMsg.header.size);
				ReconnectSetInfo(RECONNECT_STATUS_RECONNECT,RECONNECT_PROGRESS_JOINED,10000,30000);
			}
			else
			{
				if(result == 3)
				{
					ReconnectSetInfo(RECONNECT_STATUS_RECONNECT,RECONNECT_PROGRESS_CONNECTED,10000,30000);
					ReconnectAuthSend = 0;
				}
				else
				{
					ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT,RECONNECT_PROGRESS_NONE,0,0);
					((void(__thiscall*)(void*))0x00BE4C78)((void*)0x0A1EA680);
				}
			}
		}
	}
}

void ReconnectOnCloseClient(BYTE result) // OK
{
	if (!gReconnect)
		return; 
	
	if(ReconnectStatus != RECONNECT_STATUS_RECONNECT)
	{
		if(result == 0 || result == 2)
		{
			ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT,RECONNECT_PROGRESS_NONE,0,0);
		}
	}
}

void ReconnectOnCharacterList() // OK
{
	if (!gReconnect)
		return; 
	
	if(ReconnectProgress == RECONNECT_PROGRESS_JOINED)
	{
		PMSG_CHARACTER_INFO_SEND pMsg;
		pMsg.header.set(0xF3,0x03,sizeof(pMsg));
		memcpy(pMsg.name,ReconnectName,sizeof(pMsg.name));
		DataSend((BYTE*)&pMsg,pMsg.header.size);
		ReconnectSetInfo(RECONNECT_STATUS_RECONNECT,RECONNECT_PROGRESS_CHAR_LIST,10000,30000);
	}
}

void ReconnectOnCharacterInfo() // OK
{
	if (!gReconnect)
		return; 
	
	if(ReconnectProgress == RECONNECT_PROGRESS_CHAR_LIST)
	{
		if(ReconnectHelperOn != 0)
		{
			PMSG_HELPER_START_SEND pMsg;
			pMsg.header.set(0xBF,0x51,sizeof(pMsg));
			pMsg.type = 0;
			DataSend((BYTE*)&pMsg,pMsg.header.size);
		}
	}

	ReconnectSetInfo(RECONNECT_STATUS_NONE,RECONNECT_PROGRESS_NONE,0,0);
}

void ReconnectViewportDestroy() // OK
{
	if (!gReconnect)
		return;
	
	DWORD count = 0;

	DWORD ViewportAddress = 0;

	((void(__thiscall*)(int))0x61114F)(-1);

	return;

	do
	{
		BYTE send[256];
		PMSG_VIEWPORT_DESTROY_RECV pMsg;
		pMsg.header.set(0x14,0);
		int size = sizeof(pMsg);
		pMsg.count = 0;

		PMSG_VIEWPORT_DESTROY info;

		for(;count < MAX_MAIN_VIEWPORT;count++)
		{
			if((ViewportAddress=((DWORD(__thiscall*)(void*,DWORD))0x00A1A9F0)(((void*(*)())0x00458067)(),count)) == 0)
			{
				continue;
			}

			if(*(BYTE*)(ViewportAddress+0x404) == 0)
			{
				continue;
			}

			if(*(BYTE*)(ViewportAddress+0x408) == 0)
			{
				continue;
			}

			info.index[0] = SET_NUMBERHB((*(WORD*)(ViewportAddress+0x3E)));
			info.index[1] = SET_NUMBERLB((*(WORD*)(ViewportAddress+0x3E)));

			if((size+sizeof(info)) > sizeof(send))
			{
				break;
			}
			else
			{
				memcpy(&send[size],&info,sizeof(info));
				size += sizeof(info);

				pMsg.count++;
			}
		}

		pMsg.header.size = size;

		memcpy(send,&pMsg,sizeof(pMsg));

		ProtocolCoreEx(pMsg.header.head,send,size,-1);
	}
	while(count < MAX_MAIN_VIEWPORT);
}

BOOL ReconnectCreateConnection(char* address,WORD port) // OK
{
	if(PORT_RANGE(port) != 0 && GameServerAddress != address)
	{
		if(strcmp(ReconnectMapServerAddress,address) != 0 || ReconnectMapServerPort != port)
		{
			wsprintf(GameServerAddress,"%s",address);
			GameServerPort = port;
			memset(ReconnectMapServerAddress,0,sizeof(ReconnectMapServerAddress));
			ReconnectMapServerPort = 0;
		}
	}

	return ((BOOL(*)(char*,WORD))0x00BF63FA)(address,port);
}

void ReconnectGetAccountInfo(char* Account, char* Password)
{
	memcpy(UserAccount, Account, sizeof(UserAccount));
	UserAccount[10] = '\0';

	if (!gReconnect)
		return; 
	
	memcpy(ReconnectAccount, Account, sizeof(ReconnectAccount));
	memcpy(ReconnectPassword, Password, sizeof(ReconnectPassword));
	((void(*)(char*, char*))0x0045FA9A)(Account,Password);
}

__declspec(naked) void ReconnectCheckConnection() // OK
{
	static DWORD ReconnectCheckConnectionAddress1 = 0x005131F5;
	static DWORD ReconnectCheckConnectionAddress2 = 0x0051330A;

	_asm
	{
		Cmp Eax,-1
		Jnz EXIT
		Mov Ecx,ReconnectStatus
		Cmp Ecx,RECONNECT_STATUS_RECONNECT
		Je EXIT
		Jmp [ReconnectCheckConnectionAddress1]
		EXIT:
		Jmp [ReconnectCheckConnectionAddress2]
	}
}

__declspec(naked) void ReconnectCloseSocket() // OK
{
	static DWORD ReconnectCloseSocketAddress1 = 0x00BE4C91;

	_asm
	{
		Mov Eax,Dword Ptr Ds:[MAIN_CONNECTION_STATUS]
		Cmp Eax,0x00
		Je EXIT
		Mov Ecx,Dword Ptr Ss:[Ebp-0x0C]
		Mov Edx,Dword Ptr Ds:[Ecx+0x0C]
		Push Edx
		Call [CheckSocketPort]
		Add Esp,0x04
		Je EXIT
		Call [ReconnectOnCloseSocket]
		EXIT:
		Mov Dword Ptr Ds:[MAIN_CONNECTION_STATUS],0x00
		Jmp [ReconnectCloseSocketAddress1]
	}
}

__declspec(naked) void ReconnectMenuExitGame() // OK
{
	static DWORD ReconnectMenuExitGameAddress1 = 0x00847610;

	_asm
	{
		Mov Eax,ReconnectStatus
		Cmp Eax,RECONNECT_STATUS_RECONNECT
		Jnz EXIT
		Push 0
		Call [ExitProcess]
		EXIT:
		Mov ReconnectStatus,RECONNECT_STATUS_DISCONNECT
		Push 0x0140F968
		Jmp [ReconnectMenuExitGameAddress1]
	}
}
