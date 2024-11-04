#pragma once

#include "Client_Defines.h"
#include "actor.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
class CModel;
class CShader;
class CPartObject;
END

BEGIN(Client)

class CPlayer final : public CActor
{
public:
	enum PARTID { PART_BODY, PART_HEAD, PART_HAIR, PART_SWORD, PART_SHIELD, PART_END };
	enum STATE {
		STATE_IDLE
		, STATE_WALK
		, STATE_ATTACK
		, STATE_END
	};
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual _int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Set_State(OBJ_STATE _eState);
	void Key_Input(_float fTimeDelta);

//For Editing
public:
	virtual void Set_CurrentTrackPosition(_double dPosition) override;
	virtual _double Get_Duration() override; // 그냥 몸 기준으로 받아와라
	virtual _double* Get_CurrentTrackPosition_ptr() override;
	virtual void Register_ModelCom(list<class CModel*>& ModelList) override;


private:
	_bool			m_bDrawWeapon = { true };
	_uint			m_iState = {  };

private:
	CNavigation*	m_pNavigationCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CModel*			m_pBodyModelCom = { nullptr };

private:
	vector<CPartObject*> m_Parts;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Add_PartObject(_uint iPartID, const _wstring& strPrototypeTag, void* pArg = nullptr);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END