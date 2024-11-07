#pragma once

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"
#include <process.h>
#include "Client_Enums.h"
#include <queue>

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	typedef struct EFFECTTRIGGER
	{
		double TriggerTime;
		char BoneName[MAX_PATH];
	}EFFECTTRIGGER;

	typedef struct BONENAME
	{
		char BoneName[MAX_PATH];
	}BONENAME;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;

#define MiniMax 999


#define ModelTag_Sword TEXT("Prototype_Component_Model_PlayerSword")
#define ModelTag_Shield TEXT("Prototype_Component_Model_PlayerShield")
#define ModelTag_Syar TEXT("Prototype_Component_Model_Syar")

#define GameTag_Weapon TEXT("Prototype_GameObject_Weapon")
#define GameTag_EditObj TEXT("Prototype_GameObject_EditObj")
#define GameTag_BoneFlag TEXT("Prototype_GameObject_BoneFlag")

#define TextureTag_BoneFlag TEXT("Prototype_Component_Texture_BoneFlag")

#define BufferTag_Rect TEXT("Prototype_Component_VIBuffer_Rect")