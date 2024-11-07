#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CBone;
END

BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct BODY_DESC : public CPartObject::EDITING_PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		map<OBJ_STATE, pair<_uint, ANITYPE>>* mapAnimationIndex;
	}BODY_DESC;

private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual _int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	virtual const _float4x4* Get_BoneCombindTransformationMatrix_Ptr(const _char* pBoneName) const override;
	virtual void Set_State(_uint _eState) override;
	void Change_Bone(CBone* pBone, _uint iBoneIndex);
	CBone* Get_Bone(const char* BoneName);

//For Editing
public:
	virtual void Set_CurrentTrackPosition(_double dPosition) override;
	virtual _double Get_Duration() override;
	virtual _double* Get_CurrentTrackPosition_ptr() override;

private:
	class CShader*				m_pShaderCom = { nullptr };
	class CModel*				m_pModelCom = { nullptr };
	class CFSM*					m_pFSM = { nullptr };


private:
	const _uint*				m_pParentState = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM(map<OBJ_STATE, pair<_uint, ANITYPE>>* _pAnimationIndex);

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END