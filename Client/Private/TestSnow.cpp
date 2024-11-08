#pragma once
#include "stdafx.h"
#include "TestSnow.h"
#include "GameInstance.h"


CTestSnow::CTestSnow(ID3D11)
    :CGameObject{_pDevice, _pContext}
{
}

CTestSnow::CTestSnow(const CTestSnow& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CTestSnow::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestSnow::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    TESTSNOW_DESC* pDesc = static_cast<TESTSNOW_DESC*>(_pArg);
    m_fLifeTime = pDesc->fLifeTime;
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->pSocketBoneMatrix._11, pDesc->pSocketBoneMatrix._12, pDesc->pSocketBoneMatrix._13, pDesc->pSocketBoneMatrix._14));
    m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->pSocketBoneMatrix._21, pDesc->pSocketBoneMatrix._22, pDesc->pSocketBoneMatrix._23, pDesc->pSocketBoneMatrix._24));
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->pSocketBoneMatrix._31, pDesc->pSocketBoneMatrix._32, pDesc->pSocketBoneMatrix._33, pDesc->pSocketBoneMatrix._34));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->pSocketBoneMatrix._41, pDesc->pSocketBoneMatrix._42, pDesc->pSocketBoneMatrix._43, pDesc->pSocketBoneMatrix._44));

    m_pTransformCom->Set_Scaled(2.f, 2.f, 2.f);

    return S_OK;
}

HRESULT CTestSnow::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(3)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

_int CTestSnow::Update(_float _fTimeDelta)
{

    m_fLifeTime -= _fTimeDelta;
    if (signbit(m_fLifeTime))
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CTestSnow::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestSnow::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TextureTag_Snow,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, BufferTag_Rect,
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CTestSnow* CTestSnow::Create(ID3D11)
{
    CTestSnow* pInstance = new CTestSnow(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CTestSnow"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestSnow::Clone(void* _pArg)
{
    CTestSnow* pInstance = new CTestSnow(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CTestSnow"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestSnow::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
