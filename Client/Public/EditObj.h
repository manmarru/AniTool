#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CEditObj :
    public CGameObject
{
public:
	typedef struct EDITOBJ_DESC : CGameObject::EDITING_DESC
	{
		_wstring ModelTag;
		_bool isShadowObj = false;
	}EDITOBJ_DESC;


private:
    CEditObj(ID3D11);
    CEditObj(const CEditObj& Prototype);
    virtual ~CEditObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual _int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	_bool m_bShadowObj = { false };
	_float* m_pAnimationSpeed = { nullptr };
	_uint TriggerCount = { 0 };

private:
	HRESULT Ready_Components(_wstring& _ModelTag);

public:
	static CEditObj* Create(ID3D11);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free() override;
};

END