#pragma once

#define MAIN_PATH	"..\\Data\\"
#define GS_PATH		".\\Data\\"

class CPath
{
public:
	CPath();
	virtual ~CPath();
	void SetMainPath(char* path);
	void SetGsPath(char* path);
	char* GetFullPath(char* file);
	char* GetGsPath(char* file);
	bool CheckFileExists(char* file);
	char* GetRightPath(char* file);
private:
	char m_MainPath[128];
	char m_GsPath[128];
	char m_FullPath[128];
};

extern CPath gPath;
