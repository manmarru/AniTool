#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer;
END

BEGIN(Client)
class CTestSnow :
    public CGameObject
{
public:
    typedef struct TESTSNOW_DESC : GAMEOBJECT_DESC
    {
        _float4x4 pSocketBoneMatrix;
        _float fLifeTime = 1.f;
    }TESTSNOW_DESC;
private:
    CTestSnow(ID3D11);
    CTestSnow(const CTestSnow& Prototype);
    virtual ~CTestSnow() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;
    virtual HRESULT Render() override;
    virtual _int Update(_float _fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta) override;


private:
    CTexture* m_pTextureCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer* m_pVIBufferCom = { nullptr };

    _uint m_iTextureNum = { 0 };
    _float m_fLifeTime = { 0.f };


public:
    HRESULT Ready_Components();

public:
    static CTestSnow* Create(ID3D11);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

END