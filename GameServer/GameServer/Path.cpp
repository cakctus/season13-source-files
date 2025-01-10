#include "stdafx.h"
#include "Path.h"

CPath gPath;

CPath::CPath() // OK
{

}

CPath::~CPath() // OK
{

}

void CPath::SetMainPath(char* path) // OK
{
	strcpy_s(this->m_MainPath,path);
}

void CPath::SetGsPath(char* path) // OK
{
	strcpy_s(this->m_GsPath, path);
}

char* CPath::GetFullPath(char* file) // OK
{
	strcpy_s(this->m_FullPath,this->m_MainPath);
	strcat_s(this->m_FullPath,file);

	return this->m_FullPath;
}

char* CPath::GetGsPath(char* file) // OK
{
	strcpy_s(this->m_FullPath, this->m_GsPath);
	strcat_s(this->m_FullPath, file);

	return this->m_FullPath;
}

bool CPath::CheckFileExists(char* path)
{
	FILE* file;

	if (file = fopen(path, "r"))
	{
		fclose(file);
		return true;
	}
	else
	{
		return false;
	}
}

char* CPath::GetRightPath(char* file)
{
	char* fileName = this->GetGsPath(file);

	if (this->CheckFileExists(fileName))
		return fileName;

	return this->GetFullPath(file);
}
