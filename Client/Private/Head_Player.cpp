#include "stdafx.h"
#include "Head_Player.h"

#include "FSM.h"

#include "Player.h"

#include "GameInstance.h"

CHead_Player::CHead_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CHead_Player::CHead_Player(const CHead_Player & Prototype)
	: CPartObject(Prototype)
{
}
	
const _float4x4 * CHead_Player::Get_BoneMatrix_Ptr(const _char * pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);	
}

HRESULT CHead_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHead_Player::Initialize(void * pArg)
{
	HEAD_DESC*			pDesc = static_cast<HEAD_DESC*>(pArg);
	m_pParentState = pDesc->pParentState;
	m_pAnimationSpeed = pDesc->pAnimationSpeed;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_FSM(pDesc->mapAnimationIndex)))
		return E_FAIL;

	return S_OK;
}

void CHead_Player::Priority_Update(_float fTimeDelta)
{
}

_int CHead_Player::Update(_float fTimeDelta)
{
	m_pFSM->Update(fTimeDelta * (*m_pAnimationSpeed));

	return OBJ_NOEVENT;
}

void CHead_Player::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CHead_Player::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;	

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();	

	for (_uint i = 0; i < iNumMeshes; i++)
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

HRESULT CHead_Player::Render_LightDepth()
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

	for (_uint i = 0; i < iNumMeshes; i++)
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

void CHead_Player::Set_State(_uint _eState)
{
	m_pFSM->Set_State((OBJ_STATE)_eState);
}

void CHead_Player::Change_Bone(CBone* pBone, _uint iBoneIndex)
{
	m_pModelCom->Change_Bone(pBone, iBoneIndex);
}

void CHead_Player::Set_Skip(_int iSkip)
{
	m_pModelCom->Set_Skip(iSkip);
}

void CHead_Player::Set_CurrentTrackPosition(_double dPosition)
{
	m_pModelCom->Set_CurrentTrackPosition(dPosition);
}

HRESULT CHead_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
		
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PlayerHead"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHead_Player::Ready_FSM(map<OBJ_STATE, pair<_uint, ANITYPE>>* _pAnimationIndex)
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

CHead_Player* CHead_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHead_Player*		pInstance = new CHead_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHead_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject * CHead_Player::Clone(void * pArg)
{
	CHead_Player*		pInstance = new CHead_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHead_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHead_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	
	Safe_Release(m_pModelCom);
	
	Safe_Release(m_pFSM);
}
