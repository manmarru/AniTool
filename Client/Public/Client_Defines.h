#pragma once

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/ImGuiFileDialog.h"
#include <process.h>
#include "Client_Enums.h"
#include <queue>

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

#define MiniMax 999

#define ModelTag_Sword TEXT("Prototype_Component_Model_PlayerSword")
#define ModelTag_Shield TEXT("Prototype_Component_Model_PlayerShield")
#define ModelTag_Syar TEXT("Prototype_Component_Model_Syar")
#define ModelTag_SummonSyar TEXT("Prototype_Component_Model_SummonSyar")
#define ModelTag_Duaca TEXT("Prototyoe_Component_Model_Duaca")
#define ModelTag_PlayerBody TEXT("Prototype_Component_Model_PlayerBody")
#define ModelTag_GoodAxe TEXT("Prototype_Component_Model_GoodAxe")
#define ModelTag_BadAxe TEXT("Prototype_Component_Model_BadAxe")
#define ModelTag_GoodBow TEXT("Prototype_Component_Model_GoodBow")
#define ModelTag_BadBow TEXT("Prototype_Component_Model_BadBow")
#define ModelTag_GoodBowBox TEXT("Prototype_Component_Model_GoodBowBox")
#define ModelTag_BadBowBox TEXT("Prototype_Component_Model_BadBowBox")
#define ModelTag_Arrow TEXT("Prototype_Component_Model_Arrow")
#define ModelTag_Lion TEXT("Prototype_Component_Model_Lion")
#define ModelTag_Saddle TEXT("Prototype_Component_Model_Saddle")
#define ModelTag_RidingPlayer TEXT("Prototype_Component_Model_RidingPlayer")

#define GameTag_Weapon TEXT("Prototype_GameObject_Weapon")
#define GameTag_EditObj TEXT("Prototype_GameObject_EditObj")
#define GameTag_BoneFlag TEXT("Prototype_GameObject_BoneFlag")
#define GameTag_Prop TEXT("Prototype_Gameobject_Prop")
#define GameTag_TestStar TEXT("Prototype_GameObject_TestStar")
#define GameTag_TestSnow TEXT("Prototype_GameObject_TestSnow")

#define TextureTag_BoneFlag TEXT("Prototype_Component_Texture_BoneFlag")
#define TextureTag_TestStar TEXT("Prototype_Component_Texture_TestStar")
#define TextureTag_Snow TEXT("Prototype_Component_Texture_Particle")

#define BufferTag_Rect TEXT("Prototype_Component_VIBuffer_Rect")

#define ComponentTag_Shader_Model TEXT("Prototype_Component_Shader_VtxModel")
#define ComponentTag_Shader_AnimModel TEXT("Prototype_Component_Shader_VtxAnimModel")