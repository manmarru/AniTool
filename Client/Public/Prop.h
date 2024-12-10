#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CProp final :
    public CPartObject
{
public:
    typedef struct PROP_DESC : PARTOBJ_DESC
    {
        _wstring ModelTag;
        const _float4x4* pSocketBoneMatrix = { nullptr };
    }PROP_DESC;
private:
    CProp(ID3D11);
    CProp(const CProp& Prototype);
    virtual ~CProp() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;
    virtual void Priority_Update(_float _fTimeDelta) override;
    virtual _int Update(_float _fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual void Set_SocketMatrix(const _float4x4* _pSocketMatrix) override;
    void Set_LocalPos(_fvector vPos);
    void Rotation(_float fX, _float fY, _float fZ);
    void Play_Animaion(_float fTimeDelta);

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };

private:
    const _float4x4*    m_pSocketMatrix = { nullptr };
    const _uint*        m_pParentState = { nullptr };
    _bool               m_isStored = { false };

private:
    HRESULT Ready_Components(_wstring _MoadeTag);

public:
    static CProp* Create(ID3D11);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

END