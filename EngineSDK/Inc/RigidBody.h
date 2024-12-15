#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody final :
    public CComponent
{
private:
    CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float4x4* pWorldMatrix);
    virtual ~CRigidBody() = default;


public:
    void Update(_float fTimeDelta);

public:
    void Set_Inertia(_float3 vInertia) { m_vInertia = vInertia; }
    void isFalling(_bool bFalling) { m_bFalling = bFalling; }
    void Strike(_fvector vDir, _float fPower);
    void ZeroInertia() { ZeroMemory(&m_vInertia, sizeof(_float3)); }

private:
	_float4x4* m_pWorldMatrix = { nullptr }; //트랜스폼에서 가져올 월드
    _float3 m_vInertia = {};                // 관성
    _float m_fGravity = {1.f};   // 중력상수
    _bool m_bFalling = { false };

public:
    static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float4x4* pWorldMatrix);
    virtual CComponent* Clone(void* pArg) { return nullptr; }
    virtual void Free() override;
};

END