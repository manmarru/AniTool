#include "stdafx.h"
#include "Hair_Player.h"

#include "Player.h"
#include "FSM.h"

#include "GameInstance.h"

CHair_Player::CHair_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CHair_Player::CHair_Player(const CHair_Player & Prototype)
	: CPartObject(Prototype)
{
}
	
void CHair_Player::Set_State(_uint _eState)
{
	m_pFSM->Set_State((OBJ_STATE)_eState);
}

const _float4x4 * CHair_Player::Get_BoneMatrix_Ptr(const _char * pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);	
}

void CHair_Player::Set_CurrentTrackPosition(_double dPosition)
{
	m_pModelCom->Set_CurrentTrackPosition(dPosition);
}

HRESULT CHair_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHair_Player::Initialize(void * pArg)
{
	HAIR_DESC*			pDesc = static_cast<HAIR_DESC*>(pArg);
	m_pParentState = pDesc->pParentState;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_FSM(pDesc->mapAnimationIndex)))
		return E_FAIL;

	m_pAnimationSpeed = pDesc->pAnimationSpeed;

	return S_OK;
}

void CHair_Player::Priority_Update(_float fTimeDelta)
{
}

_int CHair_Player::Update(_float fTimeDelta)
{
	m_pFSM->Update(fTimeDelta * (*m_pAnimationSpeed));

	return OBJ_NOEVENT;
}

void CHair_Player::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CHair_Player::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;	

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();	

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}	

	return S_OK;
}

HRESULT CHair_Player::Render_LightDepth()
{

	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 20.f, -15.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
		return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CHair_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
		
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PlayerHair"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHair_Player::Ready_FSM(map<OBJ_STATE, pair<_uint, ANITYPE>>* _pAnimationIndex)
{
	m_pFSM = CFSM::Create(m_pModelCom);
	if (nullptr == m_pFSM)
		return E_FAIL;

	for (auto pair : *_pAnimationIndex)
	{
		m_pFSM->Register_AnimationIndex(pair.first, pair.second.first, pair.second.second);
	}

	m_pFSM->Set_State(OBJSTATE_IDLE);

	return S_OK;
}

CHair_Player* CHair_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHair_Player*		pInstance = new CHair_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHair_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject * CHair_Player::Clone(void * pArg)
{
	CHair_Player*		pInstance = new CHair_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHair_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHair_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	
	Safe_Release(m_pModelCom);
	
	Safe_Release(m_pFSM);
}
