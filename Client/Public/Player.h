#pragma once

#include "Client_Defines.h"
#include "actor.h"

BEGIN(Engine)
class CModel;
class CShader;
class CPartObject;
class CRigidBody;
END

BEGIN(Client)

class CPlayer final : public CActor
{
public:
	enum PARTID { PART_BODY, PART_HEAD, PART_HAIR, PART_SWORD, PART_SHIELD, PART_AXE, PART_END };
	enum FSMTYPE { FSM_COMM, FSM_SH, FSM_AXE, FSM_BOW, FSM_END };
	enum WEAPONTYPE { WEAPON_SWORD, WEAPON_SHIELD, WEAPON_AXE, WEAPON_BOW, WEAPON_END };
	enum CCState { CC_TUMBLE, CC_END };
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

//For Editing
public:
	virtual void Set_CurrentTrackPosition(_double dPosition) override;
	virtual _double Get_Duration() override; // 그냥 몸 기준으로 받아와라
	virtual _double* Get_CurrentTrackPosition_ptr() override;
	virtual void Register_ModelCom(list<class CModel*>& ModelList) override;

private:
	void Set_State(OBJ_STATE _eState, _bool _bLerp = true);
	void Set_ChainState(OBJ_STATE _eState, _bool bLerp = true);
	void Key_Input(_float fTimeDelta);
	void Key_Input_SH(_float fTimeDelta);
	void Key_Input_COMM(_float fTimeDelta);
	void State_Frame(_float fTimeDelta);
	void Turn_Move(_fvector vDir);
	void Turn_Tumvle(_float fAngle);

private:
	_bool				m_bDrawWeapon = { true };
	_uint				m_iState = {  };
	_uint				m_iCurrentFSM = { FSM_END };
	_uint				m_iCurrentWeapon = { FSM_SH };
	_uint				m_iTriggerCount = { 0 };

private:
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pBodyModelCom = { nullptr };
	CRigidBody*			m_pRigidBodyCom = { nullptr };
	class CFSM_Player*	m_pBodyFSM = { nullptr };

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