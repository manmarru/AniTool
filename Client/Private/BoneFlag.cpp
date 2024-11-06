#pragma once
#include "stdafx.h"
#include "BoneFlag.h"
#include "GameInstance.h"

CBoneFlag::CBoneFlag(ID3D11)
    :CBlendObject{_pDevice, _pContext}
{
}

CBoneFlag::CBoneFlag(const CBoneFlag& Prototype)
    :CBlendObject(Prototype)
{
}

HRESULT CBoneFlag::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoneFlag::Initialize(void* _pArg)
{

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    BONEFLAG_DESC* pDesc = static_cast<BONEFLAG_DESC*>(_pArg);

    m_pSocketMatrix = pDesc->pSocketBoneMatrix;

    m_pTransformCom->Set_Scaled(0.5f, 0.5f, 0.5f);

    return S_OK;
}

_int CBoneFlag::Update(_float _fTimeDelta)
{
    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, SocketMatrix.r[3]);
    // 그냥 포지션만 더해주면 되는거 아님?
    //일단 객체가 0,0,0에 있어서 그것도 안해도 되긴 하는데 객체 움직일거면 객체포지션 + 뼈포지션으로 보정해줘야됨
    return OBJ_NOEVENT;
}

void CBoneFlag::Late_Update(_float _fTimeDelta)
{
    m_pTransformCom->LookAt(m_pGameInstance->Get_CamPosition_Vector());
}

HRESULT CBoneFlag::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
        return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

}

HRESULT CBoneFlag::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
              
    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TextureTag_BoneFlag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, BufferTag_Rect,
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CBoneFlag* CBoneFlag::Create(ID3D11)
{
    CBoneFlag* pInstance = new CBoneFlag(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CBoneFlag"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoneFlag::Clone(void* _pArg)
{
    CBoneFlag* pInstance = new CBoneFlag(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CBoneFlag"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoneFlag::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
