#include "stdafx.h"
#include "Console.h" 

char Message[1024];

int WriteToFile;

Logger Log;	   
DWORD PiD;

void __stdcall LoggerCore(PVOID pVoid)
{		  	  
	AllocConsole();
	while(true)
	{
		SetConsoleTitleA(Log.LoggerTittle()); 
		Sleep(5000);
	}
	_endthread();
}			 

char* Logger::LoggerTittle()
{
	static char Tittle[55]; 
	sprintf_s(Tittle, "MU Logger");  
	return Tittle;
} 

void Logger::LoggerInit()
{ 
	CreateThread( 0 , 0 , (LPTHREAD_START_ROUTINE) LoggerCore , 0 , 0 , &PiD );
	WriteToFile = GetPrivateProfileInt("CONSOLE", "WriteToFile", 0, ".\\Config.ini");
} 		

void Logger::CheckProcent(char* message)
{
	for (UINT i = 0; i <= strlen(message); i++)
	{                          						   
		if(message[i] == '%')		  
		{
			for(UINT j = strlen(message); j >= i; j--)		  
				message[j+1] = message[j];
			i++;
		}
	}
}

void Logger::ConsoleOutPut(int WOL, sColor Color, sLogType Type, const char* Format, ...)
{					
	SYSTEMTIME t;
	GetLocalTime(&t);
	DWORD dwBytesWritten;
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf_s(Message,Format, pArguments);
	CheckProcent(Message); // "%" Bug Fix 
	va_end(pArguments);

	char currdate[11] = {0};
	char outputmsg[2048];
	if(WOL == 1)
	{
		sprintf_s(currdate, "(%02d:%02d:%02d)", t.wHour, t.wMinute, t.wSecond);
		sprintf_s(outputmsg,"%s %s\n", currdate,Message);
	}
	else
		sprintf_s(outputmsg,"%s\n", Message); 

	switch(Color)
	{
	case c_BoldGreen: 
		SetConsoleTextAttribute(this->Handle(FALSE),FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY);
		break;
	case c_BoldRed: 
		SetConsoleTextAttribute(this->Handle(FALSE),FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_RED );
		break;
	case c_Red: 
		SetConsoleTextAttribute(this->Handle(FALSE),FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case c_Green: 
		SetConsoleTextAttribute(this->Handle(FALSE),FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case c_Blue: 
		SetConsoleTextAttribute(this->Handle(FALSE),FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case c_Cyan: 
		SetConsoleTextAttribute(this->Handle(FALSE),FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case c_Yellow: 
		SetConsoleTextAttribute(this->Handle(FALSE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case c_Magenta: 
		SetConsoleTextAttribute(this->Handle(FALSE),FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case c_Grey:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	} 
	
	if (WriteToFile == 1)
		CreateLog(Type,outputmsg);	

	WriteFile(this->Handle(FALSE), outputmsg, strlen(outputmsg), &dwBytesWritten, NULL);
	SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

HANDLE Logger::Handle(BOOL Input)
{
	if(Input==TRUE)
		return GetStdHandle(STD_INPUT_HANDLE);
	else
		return GetStdHandle(STD_OUTPUT_HANDLE);
}

void Logger::CreateLog(sLogType Type,const char* Format, ...)
{
	SYSTEMTIME now;
	GetLocalTime(&now);  
																														 
	char ConsoleLog[55]; 
																													   
	sprintf_s(ConsoleLog, ".\\Logger-%02d-%02d-%02d.log", now.wDay, now.wMonth, now.wYear); 

	va_list pArguments1;
	va_start(pArguments1, Format);
	vsprintf_s(Message,Format, pArguments1);
	va_end(pArguments1);

	switch (Type)
	{
		case t_NULL:
		break; 
		case t_Default: 
		{
			SaveFile(ConsoleLog,Message);
		}
		break; 	 
	}
}

void Logger::SaveFile(char *logString,char *Message)
{
	FILE * stream;  
	fopen_s(&stream, logString, "a+" );
	fprintf(stream, "%s", Message);
	fclose(stream);
}