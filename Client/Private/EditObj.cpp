#pragma once
#include "stdafx.h"
#include "EditObj.h"
#include "GameInstance.h"
#include "TestStar.h"
#include "TestSnow.h"
#include "PartObject.h"
#include "FSM.h"

CEditObj::CEditObj(ID3D11)
	:CGameObject{_pDevice, _pContext}
{
}

CEditObj::CEditObj(const CEditObj& Prototype)
	:CGameObject(Prototype)
{
}

HRESULT CEditObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEditObj::Initialize(void* _pArg)
{
	EDITOBJ_DESC* pDesc = static_cast<EDITOBJ_DESC*>(_pArg);

	pDesc->fRotationPerSec = 5.f;
	pDesc->fSpeedPerSec = 10.f;
	
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->ModelTag)))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_pAnimationSpeed = pDesc->pAnimationSpeed;

	return S_OK;
}

void CEditObj::Priority_Update(_float fTimeDelta) {}

_int CEditObj::Update(_float fTimeDelta)
{
	m_pFSM->Update(fTimeDelta * (*m_pAnimationSpeed));
	//m_pModelCom->Play_Animation(fTimeDelta * (*m_pAnimationSpeed));

#pragma region 요호 이펙트트리거(주석)

	//const _float4x4* BoneMatrix;
	//if (m_pModelCom->Check_TriggerQueue(BoneMatrix))
	//{
	//	switch (TriggerCount)
	//	{
	//	case 0:
	//	{
	//		CTestStar::TESTSTAR_DESC desc;
	//		desc.pSocketBoneMatrix = BoneMatrix;
	//		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), GameTag_TestStar, &desc);
	//		TriggerCount = 1;
	//		break;
	//	}
	//	case 1:
	//	{
	//		CTestSnow::TESTSNOW_DESC desc;
	//		desc.pSocketBoneMatrix = *BoneMatrix;
	//		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), GameTag_TestSnow, &desc);
	//		TriggerCount = 0;
	//	}
	//	break;
	//	default:
	//		break;
	//	}
	//}
#pragma endregion

	return OBJ_NOEVENT;
}

void CEditObj::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	if (m_bShadowObj)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CEditObj::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

HRESULT CEditObj::Render_LightDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

const _float4x4* CEditObj::Get_WorldMatrix_Ptr()
{
	return m_pTransformCom->Get_WorldMatrix_Ptr();
}

HRESULT CEditObj::Ready_Components(_wstring& _ModelTag)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, _ModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(0, true);

	return S_OK;
}

HRESULT CEditObj::Ready_FSM()
{
	m_pFSM = CFSM::Create(m_pModelCom);
	if (nullptr == m_pFSM)
		return E_FAIL;

	//애니 등록 할 필요 없다. 체인거는거만 테스틀할거임

	return S_OK;
}

CEditObj* CEditObj::Create(ID3D11)
{
	CEditObj* pInstance = new CEditObj(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CeditObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEditObj::Clone(void* _pArg)
{
	CEditObj* pInstance = new CEditObj(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CeditObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditObj::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFSM);
}
