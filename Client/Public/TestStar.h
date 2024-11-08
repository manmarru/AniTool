#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer;
END


BEGIN(Client)
class CTestStar :
    public CBlendObject
{
public:
    typedef struct TESTSTAR_DESC : GAMEOBJECT_DESC
    {
        const _float4x4* pSocketBoneMatrix = { nullptr };
        _float fLifeTime = 1.f;
    }TESTSTAR_DESC;

private:
    CTestStar(ID3D11);
    CTestStar(const CTestStar& Prototype);
    virtual ~CTestStar() = default;

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
    _float m_fLifeTime = { 0.f };

private:
    const _float4x4*    m_pSocketMatrix = { nullptr };

private:
    HRESULT Ready_Components();

public:
    static CTestStar* Create(ID3D11);
    CGameObject* Clone(void* _pArg);
    virtual void Free() override;
};

END