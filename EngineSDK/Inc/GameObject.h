#pragma once

#include "Base.h"
#include "Transform.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		
	} GAMEOBJECT_DESC;
	typedef struct EDITING_DESC : GAMEOBJECT_DESC
	{
		_float* pAnimationSpeed = nullptr;
	}EDITING_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual _int Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_LightDepth() { return S_OK; }

	virtual void Re_Initialize(void* pArg = nullptr) {};

public:
	_bool Get_Dead() { return m_bDead; }
	void Set_Dead(_bool bDead) { m_bDead = bDead; }
	_vector		Get_TransformState(CTransform::STATE eState);

public:
	virtual class CComponent* Find_Component(const _wstring& strComponentTag, _uint iPartObjIndex = 0);

//For Editing
public:
	_float* m_pAnimationSpeed = { nullptr };//이거 그냥 값 넣지 말고 꼭 동적할당 해줘야 함;;
public:
	virtual void Set_CurrentTrackPosition(_double dPosition) {} // 다형성
	virtual _double Get_Duration() { return (_double)0; }
	virtual _double Get_CurrentTrackPosition() { return (_double)0; };
	virtual _double* Get_CurrentTrackPosition_ptr() { return nullptr; }
	virtual void Register_ModelCom(list<class CModel*>& ModelList);


protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };


protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	_bool m_bDead = { false };

protected:
	
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END