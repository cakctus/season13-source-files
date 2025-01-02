#include "stdafx.h"
#include "TrayMode.h"
#include "Protect.h"
#include "Offset.h"

TrayMode gTrayMode;

void TrayMode::Load()
{
	this->TempWindowProc	= NULL;
	this->TempIconProc		= NULL;
	this->InTray			= false;
}

void TrayMode::SwitchState()
{
	if( IsWindowVisible(pGameWindow) )
	{
		ShowWindow(pGameWindow, SW_HIDE);
		UpdateWindow(pGameWindow);
		this->ShowNotify(true);
	}
	else
	{
		ShowWindow(pGameWindow, SW_SHOW);
		UpdateWindow(pGameWindow);
		this->ShowNotify(false);
	}
}


void TrayMode::ShowNotify(bool Mode)
{
	this->InTray = Mode;
	NOTIFYICONDATA Icon		= { 0 };
	
	Icon.cbSize				= sizeof(NOTIFYICONDATA);
	Icon.uID				= TRAYMODE_ICON_ID;
	Icon.uFlags				= NIF_ICON|NIF_MESSAGE|NIF_TIP;
	Icon.hIcon				= (HICON)LoadImage(hins, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	this->TempIconProc		= Icon.hIcon;
	Icon.hWnd				= pGameWindow;
	Icon.uCallbackMessage	= TRAYMODE_ICON_MESSAGE;
	
	strcpy_s(Icon.szTip, sizeof(Icon.szTip), "MuOnline");
	
	/*if( pPlayerState == GameProcess )
	{
		char Text[120];
		sprintf(Text, "\n=====================\Personagem: %s\nLevel: %d [%d/%d]\nMap: %s (%d:%d)",
				gObjUser.lpPlayer->Name, gObjUser.lpPlayer->TimerRandom, gObjUser.m_Reset, gObjUser.m_GrandReset,
				pGetMapName(gObjUser.m_MapNumber), gObjUser.lpViewPlayer->MapPosX, gObjUser.lpViewPlayer->MapPosY);
		strcat_s(Icon.szTip, Text);
	}*/
	
	if( Mode )
	{
		Shell_NotifyIcon(NIM_ADD, &Icon);
	}
	else
	{
		Shell_NotifyIcon(NIM_DELETE, &Icon);
	}
	
	DeleteObject(Icon.hIcon);
}


void TrayMode::ShowMessage(DWORD Type, char * Title, char * Message)
{
	NOTIFYICONDATA Icon		= { 0 };
	
	Icon.cbSize				= sizeof(NOTIFYICONDATA);
	Icon.uID				= TRAYMODE_ICON_ID;
	Icon.hWnd				= pGameWindow;
	Icon.uFlags				= NIF_ICON | NIF_MESSAGE | NIF_INFO;
	Icon.hIcon				= this->TempIconProc;
	Icon.uCallbackMessage	= TRAYMODE_ICON_MESSAGE;
	Icon.dwInfoFlags		= Type;
	Icon.uTimeout			= 5000;
	
	strcpy(Icon.szInfo, Message);
	strcpy(Icon.szInfoTitle, Title);
	
	Shell_NotifyIcon(NIM_MODIFY, &Icon);
}
