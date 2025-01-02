#pragma once

class ScreenShot
{
public:

	ScreenShot();
	void PrintAltTab();
	void PrintScreen();
	//void PrintTimer();
	bool running;

};

extern ScreenShot gSS;

void ScreenShotThread2(void* lpParam);
