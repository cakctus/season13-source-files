#ifndef Logger_H
#define Logger_H

extern char Message[1024];

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
	t_Default,
};

class Logger
{
public:	   
	void LoggerInit();	
	void CheckProcent(char* message); 	
	void SaveFile(char *logString,char *Message);
	void ConsoleOutPut(int WOL, sColor Color, sLogType Type, const char* Format, ...);
	HANDLE Handle(BOOL Input);
	void CreateLog(sLogType Type,const char* Format, ...);	
	char *LoggerTittle();

	int Online_Max;
	int Online_All;
	int Online_Gms;
};
extern Logger Log;		

#endif Logger_H