#include "stdafx.h"
#include "..\Public\Loader.h"

#include "Sky.h"
#include "Player.h"
#include "Weapon.h"
#include "Terrain.h"
#include "FreeCamera.h"
#include "BackGround.h"

#pragma region PLAYER_PARTS
#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"
#pragma endregion

#include "EditObj.h"
#include "BoneFlag.h"
#include "Prop.h"

#pragma region Effect
#include "TestStar.h"
#include "TestSnow.h"
#pragma endregion

#include "GameInstance.h"


CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	//SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_BoneFlag*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TextureTag_BoneFlag,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Flags/Flag%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;
	 
	/* For. Prototype_Component_Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_TestStar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TextureTag_TestStar,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/flame/cookingFlame%d.png"), 7))))
		return E_FAIL;	

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

#pragma region PLAYERPARTS

	/* For. Prototype_Component_Model_PlayerBody*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	ifstream Loadstream("../Bin/Resources/Models/Player/Trigger_Player.dat", ios::in, ios::binary);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PlayerBody"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Player/Body/body"), PreTransformMatrix/*, &Loadstream*/))))
		return E_FAIL;
	Loadstream.close();
	
	/* For. Prototype_Component_Model_PlayerHead*/
	Loadstream.open("../Bin/Resources/Models/Player/Trigger_Player.dat", ios::in, ios::binary);
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PlayerHead"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Player/Head/Head"), PreTransformMatrix/*, &Loadstream*/))))
		return E_FAIL;
	Loadstream.close();
	
	/* For. Prototype_Component_Model_PlayerHair*/
	Loadstream.open("../Bin/Resources/Models/Player/Trigger_Player.dat", ios::in, ios::binary);
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PlayerHair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Player/Hair/Hair"), PreTransformMatrix/*, &Loadstream*/))))
		return E_FAIL;
	Loadstream.close();
	
	/* For. Prototype_Component_Model_PlayerSword*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixTranslation(0.05f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_Sword,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Models/Player/SH/Sword"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_PlayerShild*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_Shield,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Models/Player/SH/Shield"), PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_GoodAxe*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_GoodAxe,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Models/Player/Axe/GoodAxe"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_GoodBowBox*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_GoodBowBox,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Models/Player/Bow/Good/GoodBowBox"), PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_BadBowBox*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_BadBowBox,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Models/Player/Bow/Bad/BadBowBox"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_BadBow*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_BadBow,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Player/Bow/Bad/BadBow"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_GoodBow*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_GoodBow,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Player/Bow/Good/GoodBow"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_Arrow*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_Arrow,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Models/Player/Bow/Good/Arrow"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_Saddle*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_Saddle,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Lion/Saddle"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_Lion*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_Lion,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Lion/Lion"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_RidingBody*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_RidingPlayer,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Player/Body/RidingBody"), PreTransformMatrix))))
		return E_FAIL;



#pragma endregion

	/* For. Prototype_Component_Model_Syar*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_Syar,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Boss/Syar/Syar"), PreTransformMatrix))))
		return E_FAIL; 

	/* For. Prototype_Component_Model_Evana*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_Evena,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Boss/Evena/Evena"), PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_SummonSyar*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_SummonSyar,
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Boss/Syar/SummonSyar"), PreTransformMatrix))))
		return E_FAIL;


	////For Prototype_Component_Model_Duaca
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelTag_Duaca,
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Boss/Duaca/Duaca"), PreTransformMatrix))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));
  	m_pGameInstance->Load_Sound("BGM");

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For. Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Head_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Head_Player"),
		CHead_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;
		
	/* For. Prototype_GameObject_Hair_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hair_Player"),
		CHair_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(GameTag_Weapon,
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_EditObj */
	if (FAILED(m_pGameInstance->Add_Prototype(GameTag_EditObj,
		CEditObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_BoneFlag */
	if (FAILED(m_pGameInstance->Add_Prototype(GameTag_BoneFlag,
		CBoneFlag::Create(m_pDevice, m_pContext))))
		return E_FAIL;
		
	/* For. Prototype_GameObject_Prop */
	if (FAILED(m_pGameInstance->Add_Prototype(GameTag_Prop,
		CProp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Effect_TestStar */
	if (FAILED(m_pGameInstance->Add_Prototype(GameTag_TestStar,
		CTestStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_TestSnow */
	if (FAILED(m_pGameInstance->Add_Prototype(GameTag_TestSnow,
		CTestSnow::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
