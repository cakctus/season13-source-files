#pragma once

#define MAKE_NUMBERW(x,y) ((WORD)(((BYTE)((y)&0xFF))|((BYTE)((x)&0xFF)<<8)))

#define	MAIN_CONNECTION_STATUS		0x0A1EA600
#define MAIN_CHARACTER_STRUCT		0x0A1EE94C
#define MAIN_CHARACTER_MAP			0x016012CC
#define MAIN_VIEWPORT_STRUCT		0x0833FFCC
#define MAIN_FONT_NORMAL			0x01636A9C
#define MAIN_FONT_BOLD				0x01636AA0

#define MAX_WIN_WIDTH 640
#define MAX_MAIN_VIEWPORT 400

#define pGetAIController			((DWORD(*)())0x004FEF93)

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_HP_HOOK		0x00B7B5CD
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_HP			0x00B7B5D6

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MAX_HP_HOOK	0x00B7B5E7
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MAX_HP		0x00B7B5F0

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MP_HOOK		0x00C56C3F
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MP			0x00C56C48

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MAX_MP_HOOK	0x00C56C59
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MAX_MP		0x00C56C62

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_SD_HOOK		0x00C56C73
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_SD			0x00C56C7C

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MAX_SD_HOOK	0x00C56C8D
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MAX_SD		0x00C56C96

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_BP_HOOK		0x00C56CA7
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_BP			0x00C56CB0

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MAX_BP_HOOK	0x00C56CC1
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_MAX_BP		0x00C56CCA

#define MAIN_OFFSET_PRINT_PLAYER_VIEW_POINTS_HOOK		0x00A8BE69
#define MAIN_OFFSET_PRINT_PLAYER_VIEW_POINTS			0x00A8BE75



//Global
//#define WorldTime					flt_66720B8
//#define Hero						dword_833FFCC | 834D718
//#define CharacterAttribute		dword_A1EE94C
//#define CharacterMachine			dword_A1EE948
//s_szTargetID						unk_162FE58
//s_nTargetFireMemberIndex			dword_162FE64

#define MouseOnWindow				(*(bool*)0x008976450)
//MouseUpdateTime					dword_8976478
//MouseUpdateTimeMax				dword_1600B78
//g_nGuildMemberCount				dword_89A31E8
//g_hFontBold						dword_1636AA0
//MouseWheel						dword_90AD99C
//MouseRButtonPush					byte_90AD9A1
//MouseLButton						byte_90AD9A6
//MouseLButtonPush					byte_90AD9A4
#define MouseX						(*(int*)0x090ADA90)
#define MouseY						(*(int*)0x090ADA94)
#define pSetCursorFocus				*(DWORD*)0x1636F58
//ErrorMessage						dword_1636F58
//InputEnable						byte_1600BEC
//InputNumber						dword_1600BF0
//InputTextMax						dword_1600BFC
//g_iLengthAuthorityCode			dword_1636F80
//InputTextHide						byte_8975400
//g_pUIPopup						dword_162FF74
#define g_fScreenRate_x				(*(float*)0x016296EC)
#define g_fScreenRate_y				(*(float*)0x016296F0)
//g_pSingleTextInputBox				dword_1636A68
//InputText							byte_8975440
//g_iChatInputType					dword_15FF914
//TerrainWall						word_8A9FFD8
//TERRAIN_INDEX_REPEAT				sub_656D31

//ClearInput						sub_6315D1
//CUIPopup::SetPopup				sub_4C856C
//CheckMouseIn						sub_46B101
//PressKey							sub_625422
//ChangeCharacterExt				sub_613F1C

//CUIControl
//GetPosition_x						sub_4BAA0A
//GetPosition_y						sub_4BAA19
//GetState							sub_46B2D9

#define pCameraManager				((DWORD(*)())0x0050C943)

//zzzOpengl
#define EnableAlphaTest				((void(__cdecl*)(bool))0x006A991A)
#define DisableCullFace				((void(__cdecl*)())0x006A9812)
#define EnableDepthMask				((void(__cdecl*)())0x006A97B2)
#define EnableLightMap				((void(__cdecl*)())0x006A9C61)
#define EnableAlphaBlend			((void(__cdecl*)())0x006A99AF)
#define EnableAlphaBlendMinus		((void(__cdecl*)())0x006A9A36)
#define EnableAlphaBlend2			((void(__cdecl*)())0x006A9AC0)
#define DisableAlphaBlend			((void(__cdecl*)())0x006A98A0)
#define EnableCullFace				((void(__cdecl*)())0x006A97F0)
#define EnableTexture2D				((void(__cdecl*)())0x006ABD0D)

//RenderColor						sub_6ABB9C
//RenderGoldRect					sub_47A35E

//common
#define pCharacterManagerInstance	((LPVOID(*)())0x00458067)
#define pGetCharacter				((DWORD(__thiscall*) (DWORD, int Index))0x00A1A9F0)

#define pGetModelInstance			((void*(*)())0x0050171F)
#define pGetModel					((DWORD(__thiscall*)(void*,int ObjectIndex))0x00BACA55)

#define pAllocaMemory				((void*(__cdecl*) (int size))0x01068294) // | sub_107ACB8
#define pFreeMemory					((void(__cdecl*)())0x0107A52A)

//RequestTerrainHeight				sub_658C03
//pPlayBuffer						sub_8B2B58
//pSetAction						sub_5BB556

#define pGlobalText					((void*)0x0A1EE958)
#define pGetGlobalText				((char *(__thiscall*) (void *This, int Line))0x00457DD4)
#define pNewUISystemInstance		((LPVOID(*)())0x008893DC)

#define pGameWindow					*(HWND*)0x1636A8C
#define pWindowThis					((LPVOID(*)()) 0x8893DC)
#define pCheckWindow				((bool(__thiscall*) (void*, int a2))0x008880A1)
#define pCloseWindow				((int(__thiscall*) (void*, int a2))0x00888635)

#define pIsKeyDown					((bool(__cdecl*) (int a1))0x00834562)

#define World						*(int*)0x16012CC
#define SceneFlag					*(int*)0xA1EE820
#define WindowWidth					*(int*)0x1601318
#define WindowHeight				*(int*)0x160131C

#define MAIN_WINDOW					0x01636A8C

//Text
#define pRenderTextInstance			((LPVOID(*)())0x00472BA2)
#define pSetFont					((int(__thiscall*) (LPVOID, HFONT Font))0x00472E11)
#define pSetTextColor				((int(__thiscall*) (LPVOID, BYTE R, BYTE G, BYTE B, BYTE A))0x00472D5F)
#define pSetTextColorD				((int(__thiscall*) (LPVOID,DWORD Color))0x00472D90)
#define pSetTextBGColor				((int(__thiscall*) (LPVOID, BYTE R, BYTE G, BYTE B, BYTE A))0x00472DB8)
#define pSetTextBGColorD			((int(__thiscall*) (LPVOID, DWORD Color))0x00472DE9)
//#define pRenderText					((int(__thiscall*) (LPVOID This, int PosX, int PosY, LPCTSTR Text, int nCount, int nTabPositions, int lpnTabStopPositions, tagSIZE * nTabOrigin)) 0x00472E39)
#define pRenderText					((int(__thiscall*) (LPVOID This, int PosX, int PosY, LPCTSTR Text, int nCount, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin)) 0x00472E39)
#define pDrawToolTip				((int(__cdecl*)(int X, int Y, LPCSTR Text)) 0x627A44)

#define pInvokeUI ((DWORD(__stdcall*)(DWORD pFrame, char * method, char * format ...)) 0x00A9167E);

//Texture
#define oLoadSomeForm_Call			0x00859438
#define pLoadBitmap					((bool(__cdecl*)(char * Folder, DWORD Code, int Arg3, int Arg4, int Arg5, int Arg6)) 0x0082AD9A)
#define pRenderBitmap				((void(__cdecl*)(int ImageID, float PosX, float PosY, float Width, float Height, float StartX, float StartY, float ScaleX, float ScaleY, bool fillX, bool fillY, float alpha)) 0x006ABF1B)
#define pRenderImage				((void(__cdecl*)(int i,float x,float y,float w,float h))0x008340A4)
#define pRenderColor				((void(__cdecl*)(float x,float y,float w,float h,float a,int))0x006ABB9C)
#define pDrawButton					((void(__cdecl*)(DWORD, float, float, float, float, float, float)) 0x834362)
#define pDrawColorButton			((void(__cdecl*)(DWORD, float, float, float, float, float, float, DWORD)) 0x83443D)
#define pMakeColor					((DWORD(__cdecl*)(BYTE, BYTE, BYTE, BYTE)) 0x004663E4)


//Effects
//#define pCreateEffect				((int(__cdecl*)(int ModelID, Vec3D * VecPos, Vec3D * WorldPos, Vec3D * Color, int SubType, ObjectModel* lpObj, short PKKey, short Skill, short Scale, BYTE SkillSerial, float Scale2, short SkillHotKey)) 0x0069F5DE)
#define pRenderPartObjectEffect		((void(__cdecl*) (DWORD lpModel, int SkinID, float* vColor, float Alpha, int ItemLevel, BYTE NewOption, int SetOption, int Select, int RenderType, int a12, int a13, int a14, BYTE a15))0x00690F96)

//GFx
#define pGFxCallBack				((int(__cdecl*)(void*,char*func,...))0x00A1C47B)

#define pWindowThisNew				((CGFxProcess*(*)())0x00A2EBE0)()
#define WindowID(Window)			*(DWORD*)(pWindowThisNew() + Window)

#define pCheckGFxWindow				((bool(__thiscall*)(void*))0x00A1C3A8)

#define pAddChatInfo				((void(__thiscall*) (void*, eChatType type, wzstring *name, wzstring* msg))0x00B765CF)

//Other
#define DrawInterfaceText			((void(*)(char*,int PosX,int PosY,int Cor,int Arg5,int Align,float Width,int Style))0x0088E6E9)

//protocol
#define MAIN_PACKET_SERIAL			0x0A1EA5FC
#define ProtocolCore				((BOOL(*)(DWORD,BYTE*,DWORD,DWORD))0x00C4401E)
#define pParsePacket				((BYTE*(__thiscall*)(void * Buffer))0x00BE5341)
#define pChatProtocolCore			((BOOL(__cdecl*)(DWORD,DWORD,BYTE*,DWORD,DWORD))0x00C183E9)

//OPENGL
#define pDraw_RenderObject			((void(_cdecl*)(OBJECT *a1, int a2, int a3, int a4, int a5, int a6))0x00661B34)

template<typename Type, class T>
inline Type GetResizeX(T in)
{
	return static_cast<Type>(static_cast<float>(in) / g_fScreenRate_x);
}

template<typename Type, class T>
inline Type GetResizeY(T in)
{
	return static_cast<Type>(static_cast<float>(in) / g_fScreenRate_y);
}

#define MAIN_OFFSET_GET_TEXT_BMD_1					0x00457DD4
#define MAIN_OFFSET_GET_TEXT_BMD_2					0x0A1EE958

inline const char* GetTextBMD(int id)
{
	return ((const char* (__thiscall*)(int, int))MAIN_OFFSET_GET_TEXT_BMD_1)(MAIN_OFFSET_GET_TEXT_BMD_2, id);
}

inline bool IsCastleSiege()
{
	return *(DWORD*)MAIN_CHARACTER_MAP == 30;
}

#define MAIN_OFFSET_MESSAGE_BUILD					0x00406FC0
#define MAIN_OFFSET_MESSAGE_FREE					0x00407B10
#define MAIN_OFFSET_MESSAGE_PREPARE_PTR				0x0A119C44
#define MAIN_OFFSET_MESSAGE_SEND					0x00B765CF

#define IS_CHAT_OPENED	(*(BYTE*)(*(DWORD*)(MAIN_OFFSET_MESSAGE_PREPARE_PTR)+118))

inline void WzMessageSend(BYTE type, const char* message, const char* name)
{
	char memory_1[616];
	char memory_2[616];

	((void(__thiscall*)(void*, void*))MAIN_OFFSET_MESSAGE_BUILD)(memory_1, (void*)message);
	((void(__thiscall*)(void*, void*))MAIN_OFFSET_MESSAGE_BUILD)(memory_2, "");

	((void(__thiscall*)(void*, int, void*, void*))MAIN_OFFSET_MESSAGE_SEND)( (void*) (*(DWORD*)MAIN_OFFSET_MESSAGE_PREPARE_PTR), type, memory_2, memory_1);

	((void(__thiscall*)(void*))MAIN_OFFSET_MESSAGE_FREE)(memory_1);
	((void(__thiscall*)(void*))MAIN_OFFSET_MESSAGE_FREE)(memory_2);
}

enum CustomMessageID
{
	CUSTOM_MESSAGE_ID_NONE,
	CUSTOM_MESSAGE_ID_NORMAL,
	CUSTOM_MESSAGE_ID_WHISPER,
	CUSTOM_MESSAGE_ID_BLUE,
	CUSTOM_MESSAGE_ID_RED,
	CUSTOM_MESSAGE_ID_PARTY,
	CUSTOM_MESSAGE_ID_GUILD,
	CUSTOM_MESSAGE_ID_ALLIANCE,
	CUSTOM_MESSAGE_ID_ADMIN_NORMAL,
	CUSTOM_MESSAGE_ID_GENS,
	CUSTOM_MESSAGE_ID_POST,
	CUSTOM_MESSAGE_ID_MAX
};


// -> Interface draw Item 
#define sub_6A9CEB_Addr ((void(__cdecl*)(GLint x, int a2, GLsizei width, GLsizei height)) 0x6A9CEB)
#define sub_6A90C5_Addr ((int(__cdecl*)(float a1, float a2, float a3, float a4)) 0x6A90C5)
#define sub_6A9068_Addr ((void(__cdecl*)(LPVOID a1)) 0x6A9068)
#define sub_6A976E_Addr ((void(*)()) 0x6A976E)
#define pDrawItemModel			((int(__cdecl*)(float PosX, float PosY, float Width, float Height, int ItemID, int a6, int a7, float a8, char a9)) 0x64E805)

//#define sub_636720_Addr ((int(*)())0x636720)
#define sub_637770_Addr ((void(*)()) 0x637770)
#define pGetItemBmdStruct(ItemId)   (ItemBmdStruct*)(*(DWORD*)0x8128AC0 + sizeof(ItemBmdStruct) * ItemId)
#define SET_ITEMOPT_LEVEL(x)  (x << 3)