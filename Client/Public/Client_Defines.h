#pragma once

#include <process.h>
#include "Client_Enums.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;




#define ModelTag_Sword TEXT("Prototype_Component_Model_PlayerSword")
#define ModelTag_Shield TEXT("Prototype_Component_Model_PlayerShield")

#define GameTag_Weapon TEXT("Prototype_GameObject_Weapon")
