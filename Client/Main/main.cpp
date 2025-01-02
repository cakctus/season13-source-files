#include "stdafx.h"
#include "main.h"
#include "HackCheck.h"
#include "Protocol.h"
#include "Reconnect.h"
#include "Protect.h"
#include "Util.h"
#include "Console.h"
#include "Interface.h"
#include "EventSchedule.h"
#include "Oficina.h"
#include "TrayMode.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>

DWORD state;
HINSTANCE hins;
KERNELGETSTARTUPINFO HookGetStartupInfo;
HHOOK HookKB;
HHOOK HookMS;

#define ITEM_BASE_MODEL		1447
#define MAX_ITEM_TYPE 512
#define GET_ITEM(x,y) (((x)*MAX_ITEM_TYPE)+(y))
#define GET_ITEM_MODEL(x,y) ((((x)*MAX_ITEM_TYPE)+(y))+ITEM_BASE_MODEL)

bool gReconnect;

BOOL AntiLagEffects = 0;
BOOL AntiLagGlow = 0;
BOOL AntiLagSkill = 0;
BOOL AntiLagWings = 0;
BOOL AntiLagObjects = 0;
BOOL AntiLagCharacters = 0;
BOOL InterfaceSwitch = 0;

LRESULT Window(HWND Window, DWORD Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case TRAYMODE_ICON_MESSAGE:
	{
		switch (lParam)
		{
		case WM_LBUTTONDBLCLK:
		{
			gTrayMode.SwitchState();
		}
		break;
		}
	}
	break;
	}

	return CallWindowProc((WNDPROC)gTrayMode.TempWindowProc, Window, Message, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) // OK
{
	if (nCode == HC_ACTION && ((DWORD)lParam & (1 << 30)) != 0 && ((DWORD)lParam & (1 << 31)) != 0)
	{
		SHORT tabKeyState = GetAsyncKeyState(VK_LSHIFT);

		switch (wParam)
		{
		case VK_ESCAPE:
		{
			if (gInterface.Data[ScheduleBg].OnShow)
			{
				gEventSchedule.CloseScheduleWindow();
			}
			else if (gInterface.Data[OficinaBg].OnShow)
			{
				gOficina.CloseOficinaWindow();
			}
		}
		break;
		case 0x48: //H
		{
			if (!IS_CHAT_OPENED)
			{
				if (gInterface.Data[ScheduleBg].OnShow)
				{
					gEventSchedule.CloseScheduleWindow();
				}
				else
				{
					SCHEDULELISTREQ pRequest;
					pRequest.h.set(0xF3, 0xE6, sizeof(pRequest));
					DataSend((LPBYTE)&pRequest, pRequest.h.size);
				}
			}				
		}
		break;

		/*case 0x47: //G
		{
			if (!IS_CHAT_OPENED)
			{				
				pSetCursorFocus = true;
				gInterface.Data[OficinaBg].OnShow = true;
				gInterface.ToggleSceneObjects(1);
			}
		}
		break;*/

		case 0x31: //1
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				AntiLagEffects ^= 1;
				SetByte(0x007CF7DC, 0x55);				
				SetByte(0x0065C41B, 0x55);
				SetByte(0x00803D2D, 0x55);
				SetByte(0x008050E1, 0x55);
				WritePrivateProfileString("ANTILAG", "Effects", (AntiLagEffects) ? "1" : "0", ".\\Config.ini");
			}
		}
		break;

		case 0x32: //2
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				AntiLagGlow ^= 1;
				SetByte(0x0068DB38, 0x55);
				WritePrivateProfileString("ANTILAG", "Glow", (AntiLagGlow) ? "1" : "0", ".\\Config.ini");
			}
		}
		break;

		case 0x33: //3
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				AntiLagSkill ^= 1;
				SetByte(0x0068F7B0, 0x55);
				SetByte(0x006B11FC, 0x55);
				WritePrivateProfileString("ANTILAG", "Skill", (AntiLagSkill) ? "1" : "0", ".\\Config.ini");
			}
		}
		break;

		case 0x34: //4
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				AntiLagWings ^= 1;
				SetCompleteHook(0xE8, 0x005E562C, 0x00690F96);
				SetByte(0x005A41F9, 0x55);
				WritePrivateProfileString("ANTILAG", "WingsWeapons", (AntiLagWings) ? "1" : "0", ".\\Config.ini");
			}
		}
		break;

		case 0x35: //5
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				AntiLagObjects ^= 1;
				SetByte(0x006646E1, 0x55);
				WritePrivateProfileString("ANTILAG", "MapObjects", (AntiLagObjects) ? "1" : "0", ".\\Config.ini");
			}
		}
		break;

		case 0x36: //6
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				AntiLagCharacters ^= 1;
				SetCompleteHook(0xE8, 0x006A2874, 0x00690F96);
				WritePrivateProfileString("ANTILAG", "Characters", (AntiLagCharacters) ? "1" : "0", ".\\Config.ini");
			}
		}
		break;

		case 0x37: //7
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				InterfaceSwitch ^= 1;
				gInterface.ToggleSceneObjects(InterfaceSwitch);
			}
		}
		break;

		case 0x38: //8
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				showPVPon ^= 1;
				SetByte(0x00633FFC + 2, showPVPon);
				SetByte(0x00634603 + 2, showPVPon);
			}
		}
		break;

		case 0x39: //9
		{
			if ((1 << 16) & tabKeyState && !IS_CHAT_OPENED)
			{
				AntiLagEffects ^= 1;
				SetByte(0x007CF7DC, 0x55);
				SetByte(0x0065C41B, 0x55);
				SetByte(0x00803D2D, 0x55);
				SetByte(0x008050E1, 0x55);
				WritePrivateProfileString("ANTILAG", "Effects", (AntiLagEffects) ? "1" : "0", ".\\Config.ini");

				AntiLagGlow ^= 1;
				SetByte(0x0068DB38, 0x55);
				WritePrivateProfileString("ANTILAG", "Glow", (AntiLagGlow) ? "1" : "0", ".\\Config.ini");

				/*AntiLagSkill ^= 1;
				SetByte(0x0068F7B0, 0x55);
				SetByte(0x006B11FC, 0x55);*/

				AntiLagWings ^= 1;
				SetCompleteHook(0xE8, 0x005E562C, 0x00690F96);
				SetByte(0x005A41F9, 0x55);
				WritePrivateProfileString("ANTILAG", "WingsWeapons", (AntiLagWings) ? "1" : "0", ".\\Config.ini");
			}
		}
		break;

		case VK_PRIOR:
			if (GetForegroundWindow() == *(HWND*)(MAIN_WINDOW) && *Camera_Zoom > 14)
			{
				*Camera_Zoom -= 3.0f;
			}
			break;
		case VK_NEXT:
			if (GetForegroundWindow() == *(HWND*)(MAIN_WINDOW) && *Camera_Zoom < 70)
			{
				*Camera_Zoom += 3.0f;
			}
			break;
		case VK_F12:
		{
			//ShowWindow(pGameWindow, SW_MINIMIZE);
			if (gTrayMode.TempWindowProc == NULL)
			{
				gTrayMode.TempWindowProc = SetWindowLong(pGameWindow, GWL_WNDPROC, (long)Window);
			}

			gTrayMode.SwitchState();
		} break;

		default:
			break;
		}

		//Log.ConsoleOutPut(1, c_Blue, t_Default, "Zoom value %f", *Camera_Zoom);
	}

	return CallNextHookEx(HookKB, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) // OK
{
	if (nCode == HC_ACTION)
	{
		MOUSEHOOKSTRUCTEX* HookStruct = (MOUSEHOOKSTRUCTEX*)lParam;

		int direction = HookStruct->mouseData;

		switch (wParam)
		{
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		{
			gEventSchedule.EventWindowMain(wParam);
			gOficina.EventWindowMain(wParam);
		} break;

		case WM_MOUSEWHEEL:
		{
			//Log.ConsoleOutPut(1, c_Red, t_Default, "HookStruct->mouseData: %d", HookStruct->mouseData);			

			if (GetForegroundWindow() == *(HWND*)(MAIN_WINDOW))
			{
				SHORT tabKeyState = GetAsyncKeyState(VK_LSHIFT);

				if ((1 << 16) & tabKeyState)
				{
					if (direction > 0)
					{
						if (*Camera_Zoom > 14)
						{
							*Camera_Zoom -= 2.0f;
							//Log.ConsoleOutPut(1, c_Red, t_Default, "IN");
						}
					}
					else
					{
						if (*Camera_Zoom < 70)
						{
							*Camera_Zoom += 2.0f;
							//Log.ConsoleOutPut(1, c_Red, t_Default, "OUT %f", *Camera_Zoom);
						}
					}					
				}
			}			
		} break;
		default:
			break;
		}
	}

	return CallNextHookEx(HookMS, nCode, wParam, lParam);
}

LONG WINAPI CheckMacroThread() // OK
{
	while (!SleepEx(500, 0))
	{
		if (AntiLagEffects != 0)
		{
			SetByte(0x007CF7DC, 0xC3); //Dynamic effects			
			SetByte(0x0065C41B, 0xC3);
			SetByte(0x00803D2D, 0xC3);
			SetByte(0x008050E1, 0xC3);
		}

		if (AntiLagGlow != 0)
		{
			SetByte(0x0068DB38, 0xC3);
		}

		if (AntiLagSkill != 0)
		{
			SetByte(0x0068F7B0, 0xC3);
			SetByte(0x006B11FC, 0xC3); //Other important effect like twisting slash
		}

		if (AntiLagWings != 0)
		{
			MemorySet(0x005E562C, 0x90, 0x05);
			SetByte(0x005A41F9, 0xC3);
		}

		if (AntiLagObjects != 0)
		{
			SetByte(0x006646E1, 0xC3);
		}

		if (AntiLagCharacters != 0)
		{
			MemorySet(0x006A2874, 0x90, 0x05);			
		}
	}
	return 0;
}

int read_int(std::istream& input_stream)
{
	int n;
	input_stream >> n;
	return n;
}

BYTE LoginScreenStolen[74] = {
	0x55, 0x8B, 0xEC, 0x51, 0x51,
	0x89, 0x4D, 0xF8, 0x8B, 0x45,
	0xF8, 0x8B, 0x88, 0x24, 0x40,
	0x00, 0x00, 0xE8, 0x1C, 0x03,
	0x00, 0x00, 0x0F, 0xB6, 0xC0,
	0x85, 0xC0, 0x75, 0x29, 0x8B,
	0x45, 0xF8, 0x8B, 0x88, 0x24,
	0x40, 0x00, 0x00, 0xE8, 0xCF,
	0x03, 0x00, 0x00, 0x89, 0x45,
	0xFC, 0x8B, 0x45, 0xF8, 0x8B,
	0x88, 0x24, 0x40, 0x00, 0x00,
	0xE8, 0xB9, 0x02, 0x00, 0x00,
	0x8B, 0x4D, 0xFC, 0xE8, 0xAA,
	0x03, 0x00, 0x00, 0xEB, 0x02,
	0x33, 0xC0, 0xC9, 0xC3
};

BYTE ConnectToServerStolen[92] = {
	0x0F, 0xB7, 0x45, 0x0C, 0x50,
	0xFF, 0x75, 0x08, 0x68, 0x04,
	0x18, 0x43, 0x01, 0x68, 0xE0,
	0x6A, 0x63, 0x01, 0xE8, 0xA5,
	0xEF, 0x18, 0x00, 0x83, 0xC4,
	0x10, 0x6A, 0x01, 0xFF, 0x35,
	0x8C, 0x6A, 0x63, 0x01, 0xB9,
	0x80, 0xA6, 0x1E, 0x0A, 0xE8,
	0x77, 0xE7, 0xFE, 0xFF, 0x68,
	0x00, 0x04, 0x00, 0x00, 0xFF,
	0x75, 0x0C, 0xFF, 0x75, 0x08,
	0xB9, 0x80, 0xA6, 0x1E, 0x0A,
	0xE8, 0x70, 0xE9, 0xFE, 0xFF,
	0x85, 0xC0, 0x0F, 0x85, 0x93,
	0x00, 0x00, 0x00, 0x68, 0xF0,
	0x17, 0x43, 0x01, 0x68, 0xE0,
	0x6A, 0x63, 0x01, 0xE8, 0x64,
	0xEF, 0x18, 0x00, 0x59, 0x59,
	0x6A, 0x01
};

static DWORD ExitAddr2 = 0x00A8B996;
static DWORD CallAddr2 = 0x00589D62;

void __declspec(naked) LevelTest2()
{
	_asm {
		mov ecx, dword ptr ss : [ebp - 0x6C]
		mov dword ptr ds : [ecx + 0x78] , eax
		mov eax, dword ptr ss : [ebp - 0x6C]
		push dword ptr ds : [eax + 0x78]
		//push 0x1421754
		push LevelText
		lea eax, dword ptr ss : [ebp - 0x4C]
		push eax
		call CallAddr2

		mov edx, ExitAddr2
		JMP edx
	}
}

DWORD AllowJewelsInsert_Buff;
void __declspec(naked) AllowJewelsInsert()
{
	_asm
	{
		mov ecx, dword ptr ds : [eax + 0x10]
		mov AllowJewelsInsert_Buff, ecx
	}

	if (AllowJewelsInsert_Buff == GET_ITEM(14, 14))
	{
		_asm
		{
			mov AllowJewelsInsert_Buff, 0x00B63E9E
			jmp AllowJewelsInsert_Buff
		}
	}

	//Is a custom jewel
	if (AllowJewelsInsert_Buff > GET_ITEM(14, 470) && AllowJewelsInsert_Buff < GET_ITEM(15, 0))
	{
		_asm
		{
			mov AllowJewelsInsert_Buff, 0x00B63E82
			jmp AllowJewelsInsert_Buff
		}
	}

	_asm
	{
		mov AllowJewelsInsert_Buff, 0x00B63F8E
		jmp AllowJewelsInsert_Buff
	}
}

DWORD JewelsShine_Buff;

void __declspec(naked) JewelsShine()
{
	_asm
	{
		mov ebx, dword ptr ss : [ebp - 0x1984]
		mov JewelsShine_Buff, ebx
	}

	//Is a custom jewel
	if (JewelsShine_Buff > GET_ITEM_MODEL(14, 470) && JewelsShine_Buff < GET_ITEM_MODEL(15, 0))
	{
		_asm
		{
			mov JewelsShine_Buff, 0x0069180C
			jmp JewelsShine_Buff
		}
	}

	if (JewelsShine_Buff > 0x1FD9)
	{
		_asm
		{
			mov JewelsShine_Buff, 0x006914B7
			jmp JewelsShine_Buff
		}
	}

	_asm
	{
		mov JewelsShine_Buff, 0x0069117C
		jmp JewelsShine_Buff
	}
}


DWORD TeleAlly_Buff;
BYTE X, Y;

void __declspec(naked) TeleportAlly1()
{
	_asm {
		//009FB2CF | FF75 C8 | push dword ptr ss : [ebp - 38] |
		//009FB2D2 | 8D4D D0 | lea ecx, dword ptr ss : [ebp - 30] |
		//009FB2D5 | E8 960C0000 | call <main.sub_9FBF70> |
		//009FB2DA | 0FB640 04 | movzx eax, byte ptr ds : [eax + 4] |

		mov TeleAlly_Buff, 0x9FBF58
		lea ecx, dword ptr ss : [ebp - 0x30]
		call TeleAlly_Buff
		mov dword ptr ss : [ebp - 0x3C] , eax
		mov TeleAlly_Buff, 0x009FB23E
		jmp TeleAlly_Buff
	}
}

DWORD MGCallAddr1 = 0xA91B03;
DWORD MGCallAddr2 = 0xA914A0;
DWORD MGjumpOut = 0xA8F923;

void __declspec(naked) MGDamageMultiplier()
{
	_asm {
		mov eax, dword ptr ss : [ebp - 0x38C]
		movzx eax, word ptr ds : [eax + 0xA6]
		cdq
		push 0xA
		pop ecx
		idiv ecx
		add eax, 0xC8
		push eax
		push 0x24
		lea eax, dword ptr ss : [ebp - 0xF8]
		push eax
		mov ecx, dword ptr ss : [ebp - 0x38C]
		call MGCallAddr1
		movzx eax, word ptr ss : [ebp - 0x6C]
		mov dword ptr ss : [ebp - 0x410] , eax
		lea eax, dword ptr ss : [ebp - 0xF8]
		push eax
		push 0x24
		push dword ptr ss : [ebp - 0x410]
		lea eax, dword ptr ss : [ebp - 0x23C]
		push eax
		mov ecx, dword ptr ss : [ebp - 0x38C]
		call MGCallAddr2
		mov ax, word ptr ss : [ebp - 0x6C]
		add ax, 1
		mov word ptr ss : [ebp - 0x6C] , ax
		jmp MGjumpOut
	}
}

DWORD ChaosBoxItemCode;
DWORD ChaosBoxJump1 = 0x5837FA;
DWORD ChaosBoxJumpOut1 = 0x5837D4;
DWORD ChaosBoxJumpOut2 = 0x5837A5;

void __declspec(naked) ChaosBoxDurationItens()
{
	_asm
	{
		mov ChaosBoxItemCode, eax
	}

	if (ChaosBoxItemCode == GET_ITEM(12, 15) || ChaosBoxItemCode == GET_ITEM(14, 13) ||
		ChaosBoxItemCode == GET_ITEM(14, 14) || ChaosBoxItemCode == GET_ITEM(14, 22) ||
		ChaosBoxItemCode == GET_ITEM(14, 31) || ChaosBoxItemCode == GET_ITEM(14, 42) ||
		(ChaosBoxItemCode >= GET_ITEM(14,474) && ChaosBoxItemCode <= GET_ITEM(14, 511))
		)
	{
		_asm
		{			
			jmp ChaosBoxJump1
		}
	}

	if (ChaosBoxItemCode > 0x1C27)
	{
		_asm
		{
			jmp ChaosBoxJumpOut1
		}
	}

	_asm
	{
		mov eax, ChaosBoxItemCode
		mov dword ptr ss : [ebp - 0x14] , eax
		jmp ChaosBoxJumpOut2
	}
}

DWORD SoulSwordItemCode;
DWORD SoulSwordJump1 = 0x686F21; //se n�o �
DWORD SoulSwordJump2 = 0x686D61; //se � Holy
DWORD SoulSwordJump3 = 0x68D22E; //se � Soul

void __declspec(naked) SoulSwordEffectAndTransparency()
{
	_asm
	{
		mov eax, dword ptr ss : [ebp + 0x10]
		mov SoulSwordItemCode, eax
	}

	//Holy Sword Effect
	if (SoulSwordItemCode == GET_ITEM_MODEL(0, 57) 
		|| SoulSwordItemCode == GET_ITEM_MODEL(6, 46) //Holy shield
		|| SoulSwordItemCode == GET_ITEM_MODEL(6, 47) //Soul shield
		|| SoulSwordItemCode == GET_ITEM_MODEL(6, 48) //Blue Eye shield
		)
		
	{
		_asm
		{
			jmp SoulSwordJump2
		}		
	}
	//Dark Angel Effect
	else if (SoulSwordItemCode == GET_ITEM_MODEL(0, 62)
		|| SoulSwordItemCode == GET_ITEM_MODEL(0, 64)
		|| SoulSwordItemCode == GET_ITEM_MODEL(2, 36) //DL soul scepter
		|| SoulSwordItemCode == GET_ITEM_MODEL(3, 28) //GL soul lance
		|| SoulSwordItemCode == GET_ITEM_MODEL(3, 29) //GL blue eye lance
		|| SoulSwordItemCode == GET_ITEM_MODEL(4, 38) //FE soul bow
		|| SoulSwordItemCode == GET_ITEM_MODEL(4, 40) //FE blue eye bow
		|| SoulSwordItemCode == GET_ITEM_MODEL(4, 41) //FE blue eye quiver
		|| SoulSwordItemCode == GET_ITEM_MODEL(5, 55) //DW soul staff
		|| SoulSwordItemCode == GET_ITEM_MODEL(6, 44) //Blood shield
		|| SoulSwordItemCode == GET_ITEM_MODEL(6, 45) //Dark shield
		|| SoulSwordItemCode == GET_ITEM_MODEL(0, 78) //DK blue eye sw
		|| SoulSwordItemCode == GET_ITEM_MODEL(0, 79) //MG blue eye sw
		|| SoulSwordItemCode == GET_ITEM_MODEL(5, 61) //DW blue eye staff
		|| SoulSwordItemCode == GET_ITEM_MODEL(5, 62) //SU blue eye stick
		)
	{
		_asm
		{
			jmp SoulSwordJump3
		}
	}
	else
	{
		_asm
		{
			jmp SoulSwordJump1
		}
	}
}


DWORD AsaCall1 = 0x107AABA;
DWORD AsaCall2 = 0x4073E0;
DWORD AsaCall3 = 0x407B10;
DWORD AsaPointer = 0x8976C38;
DWORD AsaPointer2 = 0x8976B98;
DWORD AsaJump1 = 0x86A2A9;
DWORD AsaJump2 = 0x86A2FB;
DWORD barraN = 0x14105B8;
char* AsaText = "Opcao nao designada";

void __declspec(naked) AddOpcaoNaoDesignadaAsa()
{
	/*
	Hook point
		0086A2EF | 0FB685 23FAFFFF      | movzx eax,byte ptr ss:[ebp-5DD]             |
		0086A2F6 | 83F8 0F              | cmp eax,F                                   |
	*/

	_asm {

		movzx eax, byte ptr ss : [ebp - 0x5DD]
		cmp eax, 0x0F
		JNE EXIT_NOT_EQ

		mov eax, dword ptr ss : [ebp + 0x10]
		add eax, dword ptr ss : [ebp - 0x5C0]
		movzx eax, byte ptr ds : [eax + 0x5D]
		and eax, 0x0F
		//mov byte ptr ss : [ebp - 0x60D] , al
		//movzx   ebx, [ebp + 0x60D]
		cmp     eax, 0x0E
		JE EXIT_EQ
		JMP EXIT_NOT_EQ

	EXIT_EQ:

		mov byte ptr ss : [ebp - 0x04], 0x0A
		movzx eax, byte ptr ss : [ebp - 0x60D]
		push dword ptr ss : [ebp + eax * 0x04 - 0x608]
		lea ecx, dword ptr ss : [ebp - 0x5DC]
		call AsaCall2

		push AsaText
		mov eax, dword ptr ss : [ebp - 0xE4]
		imul eax, eax, 0x64
		add eax, AsaPointer
		push eax
		call AsaCall1

		add esp, 0x0C
		mov eax, dword ptr ss : [ebp - 0xE4]
		mov dword ptr ds : [eax * 0x04 + AsaPointer] , 0x01
		mov eax, dword ptr ss : [ebp - 0xE4]
		inc eax
		mov dword ptr ss : [ebp - 0xE4] , eax
		mov byte ptr ss : [ebp - 0x04] , 0x00
		//lea ecx, dword ptr ss : [ebp - 0x5DC]
		//call AsaCall3

		JMP AsaJump1

	EXIT_NOT_EQ :
		JMP AsaJump2
	}
}

DWORD GameShopCupomJump1 = 0x9A75FA;

void __declspec(naked) GameShopCupomDescontos()
{
	gInterface.DrawFormat(eGold, 498, 78, 90, eTxtAlign_left, 0, "Seu Desconto Atual: -");

	_asm
	{
		and dword ptr ss : [ebp - 0x04] , 0
		mov byte ptr ss : [ebp - 0x408] , 0
		JMP GameShopCupomJump1
	}	
}

DWORD GameShopValorFinalJump1 = 0x9ADE8F;
DWORD GameShopValorFinalCall = 0x87B751;
DWORD GameShopValorOriginal;
char* GameShopValorModificado = (char*) malloc(16);
char* ValueFromString;
int coinValue;
char* fakeOutput;

void CopyString(DWORD original)
{
	memcpy(GameShopValorModificado,(char*)original,16);
}

void __declspec(naked) GameShopValorFinalCompra1()
{
	_asm
	{
		push 0x00
		push 0x04
		push 0x00
		push 0x42
		mov eax, dword ptr ss : [ebp - 0x10]
		add eax, 0x4B7
		mov GameShopValorOriginal, eax
	}	

	Log.ConsoleOutPut(1, c_Blue, t_Default, "Ponteiro original %p", GameShopValorOriginal);
	Log.ConsoleOutPut(1, c_Blue, t_Default, "Ponteiro modificado %p", GameShopValorModificado);

	Log.ConsoleOutPut(1, c_Blue, t_Default, "Conteudo original %s", GameShopValorOriginal);
	Log.ConsoleOutPut(1, c_Blue, t_Default, "Conteudo modificado %s", GameShopValorModificado);
	
	CopyString(GameShopValorOriginal);

	Log.ConsoleOutPut(1, c_Blue, t_Default, "Ponteiro original %p", GameShopValorOriginal);
	Log.ConsoleOutPut(1, c_Blue, t_Default, "Ponteiro modificado %p", GameShopValorModificado);

	Log.ConsoleOutPut(1, c_Blue, t_Default, "Conteudo original %s", GameShopValorOriginal);
	Log.ConsoleOutPut(1, c_Blue, t_Default, "Conteudo modificado %s", GameShopValorModificado);

	//GameShopValorModificado = "-";

	//Log.ConsoleOutPut(1, c_Blue, t_Default, "GameShopValorModificado %s", GameShopValorModificado);

	ValueFromString = strtok(GameShopValorModificado, " ");
	Log.ConsoleOutPut(1, c_Green, t_Default, "ValueFromString 1 %s", ValueFromString);

	coinValue = 0;
	coinValue = strtol(ValueFromString, &fakeOutput, 10);
	sprintf(GameShopValorModificado, "De %d por %d iCoin", coinValue, (int)((float)coinValue * (1.0f - 0.15f)));

	_asm
	{
		push GameShopValorModificado
		mov ecx, dword ptr ss : [ebp - 0x10]
		call GameShopValorFinalCall

		mov eax, dword ptr ds : [eax + 0x04]
		add eax, 0x172
		push eax
		mov ecx, dword ptr ss : [ebp - 0x10]
		call GameShopValorFinalCall
	}

	_asm
	{
		JMP GameShopValorFinalJump1
	}
}

void EntryProc()
{
	Sleep(500);

	MemoryCpy(0x00BE5341, LoginScreenStolen, sizeof(LoginScreenStolen));

	MemoryCpy(0x00BF6423, ConnectToServerStolen, sizeof(ConnectToServerStolen));

	SetByte(0x0050E275, 0xEB); //ResourceGuard
	SetByte(0x00C1A31F, 0xEB); //ResourceGuard
	SetByte(0x00CF24A4, 0xEB); //ResourceGuard
	SetByte(0x00CF25DD, 0xEB); //ResourceGuard

	//Nix antilag
	//SetByte(0x5111F9, 0x05); //Neve
	//SetByte(0x629706, 0x05); //Fuma�a
	//SetByte(0x65A31B, 0x05); //Brilho do ch�o
	//SetByte(0x6667A1, 0x05); //Feixes de luz

	//Options da asa/capa lvl4
	MemorySet(0x86A2EF, 0x90, 10);
	SetCompleteHook(0xE9, 0x86A2EF, &AddOpcaoNaoDesignadaAsa);

	//SoulSword Effect
	MemorySet(0x686D54, 0x90, 7);
	SetCompleteHook(0xE9, 0x686D54, &SoulSwordEffectAndTransparency);

	//GameShop Legenda Desconto
	//MemorySet(0x9A75EF, 0x90, 11);
	//SetCompleteHook(0xE9, 0x9A75EF, &GameShopCupomDescontos);

	//GameShop String Pre�o
	//MemorySet(0x9ADE65, 0x90, 25);
	//SetCompleteHook(0xE9, 0x9ADE65, &GameShopValorFinalCompra1);

	//Chaos Machine Duration Jewels
	SetCompleteHook(0xE9, 0x583799, &ChaosBoxDurationItens);

	//Print level test
	SetCompleteHook(0xE9, 0x00A8B97C, &LevelTest2);
	LevelText = "%d";

	//MG Damage Multiplier
	SetCompleteHook(0xE9, 0xA8F63D, &MGDamageMultiplier);

	//CustomJewelUse
	SetCompleteHook(0xE9, 0x00B63E91, &AllowJewelsInsert);

	//CustomJewelShine
	SetCompleteHook(0xE9, 0x0069116C, &JewelsShine);

	//Liberar Teleport Ally na sala da coroa
	//SetCompleteHook(0xE9, 0x009FB233, &TeleportAlly1);

	HookKB = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hins, GetCurrentThreadId());
	HookMS = SetWindowsHookEx(WH_MOUSE, MouseProc, hins, GetCurrentThreadId());
	
	//Better to our launcher check if the resolution is available, as main does =)
	/*BYTE tempnull[2] = { 0xEB,0x09 };
	MemoryCpy(0x00BB182D, tempnull, sizeof(tempnull));
	Sleep(500);*/

	AntiLagEffects = GetPrivateProfileInt("ANTILAG", "Effects", 0, ".\\Config.ini");
	AntiLagGlow = GetPrivateProfileInt("ANTILAG", "Glow", 0, ".\\Config.ini");
	AntiLagSkill = GetPrivateProfileInt("ANTILAG", "Skill", 0, ".\\Config.ini");
	AntiLagWings = GetPrivateProfileInt("ANTILAG", "WingsWeapons", 0, ".\\Config.ini");
	AntiLagObjects = GetPrivateProfileInt("ANTILAG", "MapObjects", 0, ".\\Config.ini");
	AntiLagCharacters = GetPrivateProfileInt("ANTILAG", "Characters", 0, ".\\Config.ini");

	int loggerSwitch = GetPrivateProfileInt("CONSOLE", "Enabled", 0, ".\\Config.ini");
	if(loggerSwitch == 1)
		Log.LoggerInit();

	showCSbar = GetPrivateProfileInt("CUSTOM", "showCSbar", 0, ".\\Config.ini");

	//LoadLibrary("PS.dll");
	//LoadLibrary("labmu.dll");	

	SetByte(0x00C3F053, 0x09); //Fix DL inventory | Class Byte, 0x04 = DL, 0x09 = none

	char ClientVersion[8]; 
	GetPrivateProfileString("LOGIN","Version","1.18.70",ClientVersion,sizeof(ClientVersion),".\\Config.ini");

	SetByte(0x01609500, (ClientVersion[0] + 1)); // Version
	SetByte(0x01609501, (ClientVersion[2] + 2)); // Version
	SetByte(0x01609502, (ClientVersion[3] + 3)); // Version
	SetByte(0x01609503, (ClientVersion[5] + 4)); // Version
	SetByte(0x01609504, (ClientVersion[6] + 5)); // Version

	char ClientIp[16];
	GetPrivateProfileString("LOGIN","IpAddress","192.168.100.127",ClientIp,sizeof(ClientIp),".\\Config.ini"); 
	MemoryCpy(0x01600560, ClientIp, sizeof(ClientIp));

	char ClientSerial[17];
	GetPrivateProfileString("LOGIN","Serial","0hfqckZw30CKOzZA",ClientSerial,sizeof(ClientSerial),".\\Config.ini"); 
	MemoryCpy(0x01609508, ClientSerial, sizeof(ClientSerial)); // ClientSerial

	int FontSizeGuild = GetPrivateProfileInt("FONT", "GuildName", 8, ".\\Config.ini");
	int FontSizeChar  = GetPrivateProfileInt("FONT", "CharName",  9, ".\\Config.ini");	

	char GuildName[48];
	char FontSizeString[10];
	wsprintf(FontSizeString, "size='%d'", FontSizeGuild);
	FontSizeString[9] = '\0';
	strcpy(GuildName, "<font color='%s' ");
	strcat(GuildName, FontSizeString);
	strcat(GuildName, ">[%s]</font>");
	MemoryCpy(0x0142F4A0, GuildName, sizeof(GuildName));

	char CharacterName[40];
	char FontSizeString2[10];
	wsprintf(FontSizeString2, "size='%d'", FontSizeChar);
	FontSizeString[9] = '\0';
	strcpy(CharacterName, "<font color='%s' ");
	strcat(CharacterName, FontSizeString2);
	strcat(CharacterName, ">[%s]</font>");
	MemoryCpy(0x0142F558, CharacterName, sizeof(CharacterName));
	MemoryCpy(0x0142F69C, CharacterName, sizeof(CharacterName));

	char gameshopUpdate[18];
	wsprintf(gameshopUpdate, "%s", "up.iconemu.com");
	MemoryCpy(0x01419F80, gameshopUpdate, sizeof(gameshopUpdate));
	MemoryCpy(0x0141A00C, gameshopUpdate, sizeof(gameshopUpdate));

	SetByte(0x0050597C, 0xEB); //disable original mouse scroll
	
	SetByte(0x0050C717, 0xFF); //enable arena zoom
	
	//reduce level up effects
	SetByte(0x00C1F8F5, 0x01);
	SetByte(0x00C1F97C, 0x01);

	MemorySet(0x00D8535E, 0x90, 5); //Disable crypt error log

	MemorySet(0x00A62136, 0x90, 5); //mouse hover zen
	MemorySet(0x00A62555, 0x90, 5); //mouse hover ruud
	
	//MemorySet(0xB627ED, 0x90, 4);
	MemorySet(0xB627ED, 0x90, 51);//MStone overlap erro message

	MemorySet(0xA5F44A, 0x90, 2); //Grow Lancer equip 2 lances

	MemorySet(0x00ABCF37, 0x90, 8); //Remove Gens Map on move list
	
	SetByte(0x00B75A8A, 0x32); // chat message limit from 33 to 60

	MemorySet(0x00B7B822, 0x90, 11); // disable click sound when using potion

	SetByte(0x0063680F, 0xEB); //Allow guild request to anyone
	SetByte(0x00636801, 0x90);
	SetByte(0x00636802, 0x90);

	//Some workaround for Webzen bug in skill icon on castle siege special skills
	SetByte(0x008A7B4E, 0x30);
	SetByte(0x008A7B1F, 0x32);
	MemorySet(0x008A7B20, 0x90, 12);

	//Allow disassembly for all items
	SetByte(0x00A5D841, 0xE9);
	SetByte(0x00A5D841 + 1, 0x02);
	SetByte(0x00A5D841 + 2, 0x01);
	SetByte(0x00A5D841 + 3, 0x00);
	SetByte(0x00A5D841 + 4, 0x00);

	//AttackSpeed 270F
	SetWord(0x00BDA3E2, 0x270F); //DW
	SetWord(0x00BD8BF0, 0x270F); //DK
	SetWord(0x00BDADE6, 0x270F); //ELF
	SetWord(0x00BDC802, 0x270F); //MG
	SetWord(0x00BD98C8, 0x270F); //DL
	SetWord(0x00BDE5EE, 0x270F); //SU
	SetWord(0x00BDDB1F, 0x270F); //RF
	SetWord(0x00BDBD9E, 0x270F); //GL

	//Socket Limit for Elf (default 0x06)
	SetByte(0x00A5F11E, 0x0B);

	//PK/Hero messages
	MemorySet(0x00C411EA, 0x90, 5);
	MemorySet(0x00C417FF, 0x90, 5);
	MemorySet(0x00C41A32, 0x90, 5);
	MemorySet(0x00C4139D, 0x90, 5);
	MemorySet(0x00C41550, 0x90, 5);

	//Mensagens da porra do buff de natal
	MemorySet(0x00C24C2F, 0x90, 5);
	MemorySet(0x00C24CC4, 0x90, 5);
	MemorySet(0x00C24D74, 0x90, 5);
	MemorySet(0x00C24E24, 0x90, 5);
	MemorySet(0x00C24EC9, 0x90, 5);
	MemorySet(0x00C24F6E, 0x90, 5);
	MemorySet(0x00C25016, 0x90, 5);

	//Timer dos buffs Master
	MemorySet(0x00C2526F, 0x05, 9);
	MemorySet(0x00C2527D, 0x05, 1);

	//Liberar op��o 15 das asas/capas lvl 4
	//SetByte(0x0086A2F8, 0x11);

	//Aumenta FPS
	SetByte(0x00513188, 0x22);
	SetByte(0x0051318C, 0x22);

	//Libera monstros de barraks/refuge
	SetByte(0x8BF4D9, 0xEB);
	
	//65k stats
	SetByte(0x00A8C981 + 1, 0xB7);
	SetByte(0x00A8C996 + 1, 0xB7);
	SetByte(0x00A8C9AB + 1, 0xB7);
	SetByte(0x00A8CA4F + 1, 0xB7);
	SetByte(0x00A8CA66 + 1, 0xB7);
	SetByte(0x00A8CA7B + 1, 0xB7);
	SetByte(0x00A8CA90 + 1, 0xB7);
	SetByte(0x00A8CB34 + 1, 0xB7);
	SetByte(0x00A8CB4B + 1, 0xB7);
	SetByte(0x00A8CB60 + 1, 0xB7);
	SetByte(0x00A8CB75 + 1, 0xB7);
	SetByte(0x00A8CC19 + 1, 0xB7);
	SetByte(0x00A8CC30 + 1, 0xB7);
	SetByte(0x00A8CC45 + 1, 0xB7);
	SetByte(0x00A8CC5A + 1, 0xB7);
	SetByte(0x00A8CCFE + 1, 0xB7);
	SetByte(0x00A8CD15 + 1, 0xB7);
	SetByte(0x00A8CD2A + 1, 0xB7);
	SetByte(0x00A8CD3F + 1, 0xB7);
	SetByte(0x00A97B3F + 1, 0xB7);
	SetByte(0x00A986C2 + 1, 0xB7);
	
	SetByte(0x00A8C98C + 1, 0xB7);
	SetByte(0x00A8C9A1 + 1, 0xB7);
	SetByte(0x00A8CA71 + 1, 0xB7);
	SetByte(0x00A8CA86 + 1, 0xB7);
	SetByte(0x00A8CB56 + 1, 0xB7);
	SetByte(0x00A8CB6B + 1, 0xB7);
	SetByte(0x00A8CC3B + 1, 0xB7);
	SetByte(0x00A8CC50 + 1, 0xB7);
	SetByte(0x00A8CD20 + 1, 0xB7);
	SetByte(0x00A8CD35 + 1, 0xB7);
	SetByte(0x00A8CF96 + 1, 0xB7);
	SetByte(0x00A8CFAE + 1, 0xB7);
	SetByte(0x00A8CFEF + 1, 0xB7);
	SetByte(0x00A8D007 + 1, 0xB7);
	SetByte(0x00A8D048 + 1, 0xB7);
	SetByte(0x00A8D060 + 1, 0xB7);
	SetByte(0x00A8D0A1 + 1, 0xB7);
	SetByte(0x00A8D0B9 + 1, 0xB7);
	SetByte(0x00A8D0FA + 1, 0xB7);
	SetByte(0x00A8D112 + 1, 0xB7);
	SetByte(0x00A8D12A + 1, 0xB7);
	SetByte(0x00A8D143 + 1, 0xB7);
	SetByte(0x00A8D188 + 1, 0xB7);
	SetByte(0x00A8D1A0 + 1, 0xB7);
	SetByte(0x00A8D1B8 + 1, 0xB7);
	SetByte(0x00A8D1D1 + 1, 0xB7);
	SetByte(0x00A8D21C + 1, 0xB7);
	SetByte(0x00A8D234 + 1, 0xB7);
	SetByte(0x00A8D24C + 1, 0xB7);
	SetByte(0x00A8D265 + 1, 0xB7);
	SetByte(0x00A8D2B0 + 1, 0xB7);
	SetByte(0x00A8D2C8 + 1, 0xB7);
	SetByte(0x00A8D2E0 + 1, 0xB7);
	SetByte(0x00A8D2F9 + 1, 0xB7);
	SetByte(0x00A8D344 + 1, 0xB7);
	SetByte(0x00A8D35C + 1, 0xB7);
	SetByte(0x00A8D374 + 1, 0xB7);
	SetByte(0x00A8D38D + 1, 0xB7);
	SetByte(0x00A8D3D8 + 1, 0xB7);
	SetByte(0x00A8D3F0 + 1, 0xB7);
	SetByte(0x00A8D43B + 1, 0xB7);
	SetByte(0x00A8D453 + 1, 0xB7);
	SetByte(0x00A8D49A + 1, 0xB7);
	SetByte(0x00A8D4B2 + 1, 0xB7);
	SetByte(0x00A8D4F9 + 1, 0xB7);
	SetByte(0x00A8D511 + 1, 0xB7);
	SetByte(0x00A8DEB9 + 1, 0xB7);
	SetByte(0x00A8DED1 + 1, 0xB7);
	SetByte(0x00A97EB5 + 1, 0xB7);
	SetByte(0x00A97EC6 + 1, 0xB7);
	SetByte(0x00A97F04 + 1, 0xB7);
	SetByte(0x00A97F15 + 1, 0xB7);
	SetByte(0x00A97F68 + 1, 0xB7);
	SetByte(0x00A97F79 + 1, 0xB7);
	SetByte(0x00A97FCC + 1, 0xB7);
	SetByte(0x00A97FDD + 1, 0xB7);
	SetByte(0x00A98030 + 1, 0xB7);
	SetByte(0x00A98041 + 1, 0xB7);
	SetByte(0x00A980BE + 1, 0xB7);
	SetByte(0x00A980CF + 1, 0xB7);
	SetByte(0x00A9811F + 1, 0xB7);
	SetByte(0x00A9812E + 1, 0xB7);
	SetByte(0x00A9813F + 1, 0xB7);
	SetByte(0x00A98178 + 1, 0xB7);
	SetByte(0x00A98189 + 1, 0xB7);
	SetByte(0x00AB97C7 + 1, 0xB7);

	//MemorySet(0x00508209, 0x90, 6); //SetTimer

	InitProtocolCoreEx();

	InitHackCheck();

	//This is how logs works
	Log.ConsoleOutPut(1,c_Red,t_Default,"Seems like everything is hooked");	

	std::ifstream input_file("LauncherOption.if");
	std::string name;
	gReconnect = true;

	while (std::getline(input_file, name, ':'))
	{
		if (name.compare(0, 9, "Reconnect") == 0)
		{
			if (read_int(input_file) != 1)
			{
				Log.ConsoleOutPut(1, c_Blue, t_Default, "Reconnect disabled");
				gReconnect = false;
			}
		}
	}

	input_file.close();
	std::cin.get();

	InitReconnect();

	InitPrintPlayer();

	SetCompleteHook(0xE8, oLoadSomeForm_Call, &gInterface.LoadImages);
	//gInterface.Load();

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheckMacroThread, 0, 0, 0);
}

void WINAPI MyGetStartupInfo(LPSTARTUPINFO lpStartupInfo) // OK
{
	if ((state++) == 0)
	{
		EntryProc();
	}

	HookGetStartupInfo(lpStartupInfo);
}


void InitEntryProc() // OK
{
	state = 0;

	HookGetStartupInfo = (KERNELGETSTARTUPINFO)GetProcAddress(GetModuleHandle("Kernel32.dll"), "GetStartupInfoA");

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)HookGetStartupInfo, MyGetStartupInfo);
	DetourTransactionCommit();
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReverse)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitEntryProc();
		hins = hModule;
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return 1;
}