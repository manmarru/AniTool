#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer;
END


BEGIN(Client)
class CBoneFlag :
    public CBlendObject
{
public:
    typedef struct BONEFLAG_DESC : GAMEOBJECT_DESC
    {
        const _float4x4* pSocketBoneMatrix = { nullptr };
    }BONEFLAG_DESC;

private:
    CBoneFlag(ID3D11);
    CBoneFlag(const CBoneFlag& Prototype);
    virtual ~CBoneFlag() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;
    virtual _int Update(_float _fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_TextureNum(_uint _iNum) { m_iTextureNum = _iNum; }

private:
    CTexture*   m_pTextureCom = { nullptr };
    CShader*    m_pShaderCom = { nullptr };
    CVIBuffer*  m_pVIBufferCom = { nullptr };
    _float4x4*  m_pParentMatrix = { nullptr }; // 부모월드

    _uint m_iTextureNum = { 0 };
private:
    const _float4x4*    m_pSocketMatrix = { nullptr };

private:
    HRESULT Ready_Components();

public:
    static CBoneFlag* Create(ID3D11);
    CGameObject* Clone(void* _pArg);
    virtual void Free() override;
};

END