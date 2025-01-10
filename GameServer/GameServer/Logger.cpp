#include "stdafx.h"
#include "Logger.h" 
#include "Configs.h" 
#include "ServerInfo.h"

Logger Log;

void Logger::LoggerInit()
{
    //Create a console for this application
	AllocConsole();
	SetConsoleTitleA(Log.LoggerTitle());

    // Get STDOUT handle
    // HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    // int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
    // FILE *COutputHandle = _fdopen(SystemOutput, "w");

    // Get STDERR handle
    // HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
    // int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
    // FILE *CErrorHandle = _fdopen(SystemError, "w");

    // Get STDIN handle
    // HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    // int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
    // FILE *CInputHandle = _fdopen(SystemInput, "r");

    //make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
    // ios::sync_with_stdio(true);

    // Redirect the CRT standard input, output, and error handles to the console
    // freopen_s(&CInputHandle, "CONIN$", "r", stdin);
    // freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
    // freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

}

HANDLE Logger::Handle(BOOL Input)
{
	if (Input == TRUE)
		return GetStdHandle(STD_INPUT_HANDLE);
	else
		return GetStdHandle(STD_OUTPUT_HANDLE);
}

void Logger::UpdateTitle()
{
	SetConsoleTitleA(LoggerTitle());
}


char* Logger::LoggerTitle()
{
	static char Tittle[105];

	#if(GAMESERVER_TYPE==1)
	#define GType "(GS_CS)"
	#else													   
	#define GType "(GS)"
	#endif  

	sprintf_s(Tittle, "[%s] [%d/%d] Mu Online S13 + Emulator %s", gServerInfo.m_ServerName, gObjTotalUser, gServerInfo.m_ServerMaxUserNumber, GType);
	return Tittle;
}

void Logger::CheckProcent(char* message)
{
	for (UINT i = 0; i <= strlen(message); i++)
	{
		if (message[i] == '%')
		{
			for (UINT j = strlen(message); j >= i; j--)
				message[j + 1] = message[j];
			i++;
		}
	}
}

void Logger::ConsoleOutPut(bool withTime, sColor Color, sLogType Type, const char* Format, ...)
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	DWORD dwBytesWritten;
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);


	char buff[1024] = {0};

	va_list arg;
	va_start(arg,Format);
	vsprintf_s(buff,Format,arg);
	CheckProcent(buff); // "%" Bug Fix 
	va_end(arg);
	

	char currdate[11] = { 0 };
	char outputmsg[2048];
	if (withTime)
	{
		sprintf_s(currdate, "(%02d:%02d:%02d)", t.wHour, t.wMinute, t.wSecond);
		sprintf_s(outputmsg, "%s %s\n", currdate, buff);
	}
	else
		sprintf_s(outputmsg, "%s\n", buff);

	switch (Color)
	{
	case c_BoldGreen:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY);
		break;
	case c_BoldRed:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_RED);
		break;
	case c_Red:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case c_Green:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case c_Blue:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case c_Cyan:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case c_Yellow:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case c_Magenta:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case c_Grey:
		SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	}

	CreateLog(Type, outputmsg);
	WriteFile(this->Handle(FALSE), outputmsg, strlen(outputmsg), &dwBytesWritten, NULL);
	SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return;
}

void Logger::CreateLog(sLogType Type, const char* Format, ...)
{
	SYSTEMTIME now;
	GetLocalTime(&now);

	char ConsoleLog[55];
	char GMLog[55];
	char CommandsLog[55];
	char GPostLog[55];
	char PostLog[55];
	char DropLog[55];
	char BanLog[55];  
	char ErrorLog[55];
	char Date[55]; 
	char VipLog[55];
	char TradeLog[55];
	char TerritoryLog[55]; 
	char ResetLog[55];
	char TestLog[55];

	sprintf_s(Date, ".\\Logger\\%02d-%02d-%02d\\", now.wDay, now.wMonth, now.wYear);
	CreateDirectory(".\\Logger\\", NULL);
	CreateDirectory(Date, NULL);

	sprintf_s(CommandsLog, ".\\Logger\\%02d-%02d-%02d\\Commands.log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(ConsoleLog, ".\\Logger\\%02d-%02d-%02d\\CONSOLE.log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(GPostLog, ".\\Logger\\%02d-%02d-%02d\\[GPOST].log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(PostLog, ".\\Logger\\%02d-%02d-%02d\\[POST].log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(DropLog, ".\\Logger\\%02d-%02d-%02d\\DROP.log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(BanLog, ".\\Logger\\%02d-%02d-%02d\\BAN.log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(GMLog, ".\\Logger\\%02d-%02d-%02d\\GM.log", now.wDay, now.wMonth, now.wYear); 
	sprintf_s(ErrorLog, ".\\Logger\\%02d-%02d-%02d\\Error.log", now.wDay, now.wMonth, now.wYear); 
	sprintf_s(VipLog, ".\\Logger\\%02d-%02d-%02d\\Vip.log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(TradeLog, ".\\Logger\\%02d-%02d-%02d\\Trade.log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(TerritoryLog, ".\\Logger\\%02d-%02d-%02d\\Territory.log", now.wDay, now.wMonth, now.wYear); 
	sprintf_s(ResetLog, ".\\Logger\\%02d-%02d-%02d\\Reset.log", now.wDay, now.wMonth, now.wYear);
	sprintf_s(TestLog, ".\\Logger\\%02d-%02d-%02d\\Test.log", now.wDay, now.wMonth, now.wYear);
	
	char buff[1024] = {0};

	va_list arg;
	va_start(arg,Format);
	vsprintf_s(buff,Format,arg);
	va_end(arg);
	

	switch (Type)
	{
	case t_NULL:
		break;

	case t_Error:
	{
		SaveFile(ErrorLog, buff);
	}
	break;
	case t_Default:
	{
		SaveFile(ConsoleLog, buff);
	}
	break;  
	case t_GM:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(GMLog, buff);
	}
	break;
	case t_COMMANDS:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(CommandsLog, buff);
	}
	break;
	case t_POST:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(PostLog, buff);
		SaveFile(CommandsLog, buff);
	}
	break;
	case t_GPOST:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(GPostLog, buff);
		SaveFile(CommandsLog, buff);
		SaveFile(GMLog, buff);
	}
	break;
	case t_DROP:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(DropLog, buff);
		SaveFile(CommandsLog, buff);
		SaveFile(GMLog, buff);
	}
	break;
	case t_BAN:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(BanLog, buff);
		SaveFile(CommandsLog, buff);
		SaveFile(GMLog, buff);
	}
	break;
	case t_VIP:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(VipLog, buff);
	}
	break;
	case t_TRADE:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(TradeLog, buff);
	}
	break;
	case t_TERRITORY:
	{
		SaveFile(ConsoleLog, buff);
		SaveFile(TerritoryLog, buff);
	}
	break; 
	case t_RESET:
	{
		SaveFile(ResetLog, buff);
	}
	break;
	case t_TEST:
	{
		SaveFile(TestLog, buff);
	}
	break;
	}
}

void Logger::SaveFile(char* logString, char* Message)
{
	FILE* stream;
	stream = fopen(logString, "a+");
	fprintf(stream, "%s", Message);
	fclose(stream);
}