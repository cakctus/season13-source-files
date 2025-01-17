#include "stdafx.h"
#include "Util.h"
#include "GameMain.h"
#include "HackCheck.h"
#include "ItemManager.h"
#include "ServerInfo.h"
#include "SocketManager.h"
#include "Viewport.h"

std::mt19937 seed;
std::uniform_int_distribution<int> dist;
short RoadPathTable[MAX_ROAD_PATH_TABLE] = {-1,-1,0,-1,1,-1,1,0,1,1,0,1,-1,1,-1,0};

int SafeGetItem(int index) // OK
{
	return CHECK_ITEM(index);
}

flt GetRoundValue(float value) // OK
{
	float integral;

	if(modf(value,&integral) > 0.5f)
	{
		return ceil(value);
	}

	return floor(value);
}

BYTE GetNewOptionCount(BYTE NewOption) // OK
{
	BYTE count = 0;

	for(int n=0;n < MAX_EXC_OPTION;n++)
	{
		if((NewOption & (1 << n)) != 0)
		{
			count++;
		}
	}

	return count;
}

BYTE GetSocketOptionCount(BYTE SocketOption[5]) // OK
{
	BYTE count = 0;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		if(SocketOption[n] != 0xFF)
		{
			count++;
		}
	}

	return count;
}

BYTE GetPathPacketDirPos(int px,int py) // OK
{
	if(px <= -1 && py <= -1)
	{
		return 0;
	}
	else if(px <= -1 && py == 0)
	{
		return 7;
	}
	else if(px <= -1 && py >= 1)
	{
		return 6;
	}
	else if(px == 0 && py <= -1)
	{
		return 1;
	}
	else if(px == 0 && py >= 1)
	{
		return 5;
	}
	else if(px >= 1 && py <= -1)
	{
		return 2;
	}
	else if(px >= 1 && py == 0)
	{
		return 3;
	}
	else if(px >= 1 && py >= 1)
	{
		return 4;
	}

	return 0;
}

void PacketArgumentDecrypt(char* out_buff,char* in_buff,int size) // OK
{
	BYTE XorTable[3] = {0xFC,0xCF,0xAB};

	for(int n=0;n < size;n++)
	{
		out_buff[n] = in_buff[n]^XorTable[n%3];
	}
}

void ErrorMessageBox(char* message,...) // OK
{
	char buff[256];

	memset(buff,0,sizeof(buff));

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	MessageBox(0,buff,"Error",MB_OK | MB_ICONERROR);

	ExitProcess(0);
}

void LogAdd(eLogColor color,char* text,...) // OK
{
	tm today;
	time_t ltime;
	time(&ltime);

	if(localtime_s(&today,&ltime) != 0)
	{
		return;
	}

	char time[32];

	if(asctime_s(time,sizeof(time),&today) != 0)
	{
		return;
	}

	char temp[1024];

	va_list arg;
	va_start(arg,text);
	vsprintf_s(temp,text,arg);
	va_end(arg);

	char log[1024];

	wsprintf(log,"%.8s %s",&time[11],temp);

	gServerDisplayer.LogAddText(color,log,strlen(log));
}

bool DataSend(int aIndex,BYTE* lpMsg,DWORD size) // OK
{
	//LogAdd(LOG_RED,"[DataSend] [%s] %x - %x - %x - %x - %x", gObj[aIndex].Account, lpMsg[0],lpMsg[1],lpMsg[2],lpMsg[3],lpMsg[4]);
	return gSocketManager.DataSend(aIndex,lpMsg,size);
}

void DataSendAll(BYTE* lpMsg,int size) // OK
{
	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) != 0)
		{
			DataSend(n,lpMsg,size);
		}
	}
}

bool DataSendSocket(SOCKET socket,BYTE* lpMsg,DWORD size) // OK
{
	if(socket == INVALID_SOCKET)
	{
		return 0;
	}

	#if(ENCRYPT_STATE==1)

	EncryptData(lpMsg,size);

	#endif

	int count=0,result=0;

	while(size > 0)
	{
		if((result=send(socket,(char*)&lpMsg[count],size,0)) == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK)
			{
				return 0;
			}
		}
		else
		{
			count += result;
			size -= result;
		}
	}

	return 1;
}

void MsgSendV2(LPOBJ lpObj,BYTE* lpMsg,int size) // OK
{
	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			//Item Change + Muun Change
			if ((lpMsg[2] == 0x25 || (lpMsg[2] == 0x4E && lpMsg[3] == 0x06)) &&
				(gObj[lpObj->VpPlayer2[n].index].Antilag[0] || gObj[lpObj->VpPlayer2[n].index].Antilag[1]))
			{
				continue;
			}

			//Effect Change
			if (lpMsg[2] == 0x07 && gObj[lpObj->VpPlayer2[n].index].Antilag[2])
			{
				PMSG_EFFECT_STATE_SEND* pMsg = (PMSG_EFFECT_STATE_SEND*)lpMsg;				

				if ((pMsg->effect < 13 || pMsg->effect > 28) && (pMsg->effect < 55 || pMsg->effect > 57) && pMsg->effect != 61)
				{
					continue;
				}
			}

			if ((lpMsg[2] == 0x1E || lpMsg[2] == 0x19 || lpMsg[2] == 0x4A || (lpMsg[2] == 0xBF && lpMsg[3] == 0x0A)) && gObj[lpObj->VpPlayer2[n].index].Antilag[3])
			{
				BYTE m6 = lpMsg[6];
				BYTE m8 = lpMsg[8];
				lpMsg[6] = SET_NUMBERHB(0);
				lpMsg[8] = SET_NUMBERLB(0);
				DataSend(lpObj->VpPlayer2[n].index, lpMsg, size);
				lpMsg[6] = m6;
				lpMsg[8] = m8;
			}
			else
			{
				DataSend(lpObj->VpPlayer2[n].index, lpMsg, size);
			}
		}
	}
}

void CloseClient(int aIndex) // OK
{
	//LogAdd(LOG_RED,"del by closeClient");
	gSocketManager.Disconnect(aIndex);
}

void PostMessage(char* name,char* message,char* text,int type) // OK
{
	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff, type);
		}
	}

	/*char buff[256] = {0};

	wsprintf(buff, message, text);

	int size = strlen(buff);
	int nameSize = strlen(name);

	size = ( (size > MAX_CHAT_MESSAGE_SIZE ) ? MAX_CHAT_MESSAGE_SIZE : size );

	PMSG_CHAT_SEND pMsg = { 0 };

	pMsg.header.set(0x00, sizeof(pMsg));

	char nameV[11] = { 0 };

	int nameLength = strlen(name) >= 10 ? 10 : strlen(name);
	strncpy(nameV, name, nameLength);
	memcpy(pMsg.name, nameV, sizeof(pMsg.name));

	ZeroMemory(pMsg.message, sizeof(pMsg.message));
	memcpy(pMsg.message, buff, size);
	pMsg.message[size] = '\0';

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}*/
}

void SetLargeRand() // OK
{
	std::random_device rd;
	//std::mt19937 mt(rd());
	//seed = std::mt19937(std::random_device());
	seed = std::mt19937(rd());
	dist = std::uniform_int_distribution<int>(0,2147483647);
}

long GetLargeRand() // OK
{
	return dist(seed);
}

int GetWordsCount(char *str)
{	
    int flag = TRUE;
    unsigned int wCounter = 0; // word counter
    // Run until not get null character
    while (*str)
    {
        //Set the flag true if you got the space
        if (*str == ' ')
        {
            flag = TRUE;
        }
        else if (flag == TRUE) //if next word is not empty and flag is true,
        {
            //increment word counter
            flag = FALSE;
            ++wCounter;
        }
        // Move to next character
        ++str;
    }
    return wCounter;
}
