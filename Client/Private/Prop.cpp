#pragma once
#include "stdafx.h"
#include "Prop.h"

#include "GameInstance.h"
#include "MyMath.h"

CProp::CProp(ID3D11)
	:CPartObject{ _pDevice, _pContext }
{
}

CProp::CProp(const CProp& Prototype)
	:CPartObject(Prototype)
{
}

HRESULT CProp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProp::Initialize(void* _pArg)
{
	PROP_DESC* pDesc = static_cast<PROP_DESC*>(_pArg);

	m_pParentMatrix = pDesc->pParentWorldMatrix;
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->ModelTag)))
		return E_FAIL;

	m_pModelCom->Set_NextAnimIndex(0, true, 0.f);

	return S_OK;
}

void CProp::Priority_Update(_float _fTimeDelta) {}

_int CProp::Update(_float _fTimeDelta)
{
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	
	//m_pModelCom->Play_Animation(_fTimeDelta);
		
	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	return OBJ_NOEVENT;
}

void CProp::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CProp::Render()
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CProp::Set_SocketMatrix(const _float4x4* _pSocketMatrix)
{
	m_pSocketMatrix = _pSocketMatrix;
}

void CProp::Set_LocalPos(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CProp::Rotation(_float _fX, _float _fY, _float _fZ)
{
	m_pTransformCom->Rotation(_fX, _fY, _fZ);
}

void CProp::Play_Animaion(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

HRESULT CProp::Ready_Components(_wstring _MoadeTag)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, ComponentTag_Shader_Model,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, _MoadeTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CProp* CProp::Create(ID3D11)
{
	CProp* pInstance = new CProp(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CProp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProp::Clone(void* _pArg)
{
	CProp* pInstance = new CProp(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CProp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProp::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
