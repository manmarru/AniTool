#include "stdafx.h"
#include "..\Public\CameraManager.h"
#include "GameInstance.h"
#include "Actor.h"

IMPLEMENT_SINGLETON(CCameraManager);

CCameraManager::CCameraManager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}


void CCameraManager::Set_CamState(CAM_STATE _eState)
{
	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_Object(m_eCurrentLevel, TEXT("Layer_Camera"), _eState));
	if (nullptr == pCamera)
		return;

	m_eCamState = _eState;
	m_pCurrentCamera = pCamera;
}


HRESULT CCameraManager::Ready_Camera(LEVELID eLevelIndex)
{
	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_Object(eLevelIndex, TEXT("Layer_Camera")));

	if (nullptr == pCamera)
		return E_FAIL;

	m_pCurrentCamera = pCamera;
	m_eCamState = CAM_TARGET;
	m_eCurrentLevel = eLevelIndex;

	return S_OK;
}

void CCameraManager::Free()
{
	Safe_Release(m_pGameInstance);
}
