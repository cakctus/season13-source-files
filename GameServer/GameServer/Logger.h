#ifndef Logger_H
#define Logger_H

enum sColor
{
	c_BoldGreen,
	c_BoldRed,
	c_Red,
	c_Green,
	c_Blue,
	c_Cyan,
	c_Yellow,
	c_Magenta,
	c_Grey
};

enum sLogType
{
	t_NULL,
	t_Error,
	t_Default,
	t_GM,
	t_POST,
	t_DROP,
	t_BAN,
	t_COMMANDS,
	t_GPOST,  
	t_VIP,
	t_TRADE,
	t_TERRITORY, 
	t_RESET,
	t_TEST
};

class Logger
{
public:
	void LoggerInit();
	void CheckProcent(char* message);
	void SaveFile(char* logString, char* Message);
	void ConsoleOutPut(bool withTime, sColor Color, sLogType Type, const char* Format, ...);
	HANDLE Handle(BOOL Input);
	void CreateLog(sLogType Type, const char* Format, ...);
	void UpdateTitle();
	char* LoggerTitle(); 
};
extern Logger Log;

#endif Logger_H