
#include "stdafx.h"

#include "ScreenShot.h"
#include "HandleGuards.h"
#include "ScreenShooter.h"
#include <vector>
#include "gdiplus.h"
#include <Console.h>
#include "Uploader.h"
//#include "Modules.h"
//#include "Processos.h"

using namespace guards;
using namespace Gdiplus;

ScreenShot gSS;

ScreenShot::ScreenShot() {
	this->running = false;
};

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

void ConvertToPng()
{
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CLSID   encoderClsid;
	Status  stat;

	const wchar_t* filename = L"Data\\ab46"; //prename;

	Image* image = new Image(filename);

	// Get the CLSID of the PNG encoder.
	int result = GetEncoderClsid(L"image/jpeg", &encoderClsid);

	if (result == -1)
	{
		Log.ConsoleOutPut(1, c_Blue, t_Default, "GetEncoderClsid error");
		gSS.running = false;
		return;
	}

	stat = image->Save(L"Data\\Interface\\sxa", &encoderClsid, NULL);

	if (stat != Ok)
	{
		Log.ConsoleOutPut(1, c_Blue, t_Default, "Failure: stat = %d", stat);
		gSS.running = false;
		return;
	}

	delete image;
	GdiplusShutdown(gdiplusToken);

	if (DeleteFile("Data\\ab46") == 0)
	{
		Log.ConsoleOutPut(1, c_Blue, t_Default, "# %d - %d", __LINE__, GetLastError());
	}

	gUP.Send();
}

void SaveVectorToFile(const std::wstring& fileName, const std::vector<unsigned char>& data)
{
	HANDLE hFile = CreateFileW(fileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		Log.ConsoleOutPut(1, c_Blue, t_Default, "SaveVectorToFile : can't open file - %d ", GetLastError());
		gSS.running = false;
		return;
	}

	guards::CHandleGuard fileGuard(hFile);

	DWORD bytesWriten = 0;

	if (!WriteFile(hFile, &data[0], (DWORD)data.size(), &bytesWriten, 0))
	{
		gSS.running = false;
		Log.ConsoleOutPut(1, c_Blue, t_Default, "SaveVectorToFile : can't write to file - %d ", GetLastError());
		CloseHandle(hFile);
		return;
	}

	CloseHandle(hFile);
}

void ScreenShotThread(void* lpParam)
{
	int LoopCount = 0;
	bool Sent = false;

	while (Sent == false)
	{
		LoopCount++;

		HANDLE file = CreateFile("Data\\ab46", 0, 0, 0, OPEN_EXISTING, 0, 0);

		if (file == INVALID_HANDLE_VALUE)
		{
			CloseHandle(file);
		}
		else
		{
			CloseHandle(file);
			ConvertToPng();
			break;
		}

		Sleep(500);

		if (LoopCount > 10)
		{
			gSS.running = false;
			break;
		}			
	}

	_endthread();
}

void ScreenShotThread2(void* lpParam)
{
	std::vector<unsigned char> dataScreen;

	Sleep(500);

	if (gSS.running == true)
	{
		Log.ConsoleOutPut(1, c_Blue, t_Default, "routine already running");
	}

	gSS.running == true;

	ScreenShooter::CaptureScreen(dataScreen);	

	const wchar_t* filename = L"Data\\ab46";
	SaveVectorToFile(filename, dataScreen);

	_beginthread(ScreenShotThread, 0, NULL);

	_endthread();
}

void ScreenShot::PrintAltTab()
{
	_beginthread(ScreenShotThread2, 0, NULL);
}

void ScreenShot::PrintScreen()
{
	std::vector<unsigned char> dataScreen;

	ScreenShooter::CaptureScreen(dataScreen);

	if (this->running == true)
	{
		Log.ConsoleOutPut(1, c_Blue, t_Default, "routine already running");
	}

	this->running = true;

	const wchar_t* filename = L"Data\\ab46";
	SaveVectorToFile(filename, dataScreen);

	_beginthread(ScreenShotThread, 0, NULL);	
}