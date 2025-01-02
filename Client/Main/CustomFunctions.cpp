#include "StdAfx.h"
#include "CustomFunctions.h"

#include "Protocol.h"

int CustomFunctions::CallClientMessagePopup(const char* message)
{
	DWORD MessagePopupOriginalFunc = 0x00AA54E9;
	auto Form = *(DWORD*)(pMessageBoxThis() + 0x88);
	int result;
	{
		__asm{
			push 0
			push 0
			push 0
			push 0x14046D5
			push message
			push 0
			push Form
			mov ecx, Form
			call MessagePopupOriginalFunc
			mov result, eax
			}
	}
	return result;
}
