#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CHair_Player final : public CPartObject
{
public:
	typedef struct HAIR_DESC : EDITING_PARTOBJECT_DESC
	{
		const _uint*	pParentState = { nullptr };
		_uint*			pFSMIndex;
	}HAIR_DESC;

private:
	CHair_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHair_Player(const CHair_Player& Prototype);
	virtual ~CHair_Player() = default;

public:
	virtual void Set_State(_uint _eState, _bool _bLerp = true) override;
	virtual void Set_ChainState(_uint _eState, _bool _bLerp = true) override;
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;

//For Editing
public:
	virtual void Set_CurrentTrackPosition(_double dPosition) override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual _int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
private:
	class CShader*				m_pShaderCom = { nullptr };
	class CModel*				m_pModelCom = { nullptr };
	class CFSM_Player*			m_pFSM = { nullptr };

private:
	const _uint*				m_pParentState = { nullptr };
	_uint*						m_pCurrentFSMIndex = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();

public:
	static CHair_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END