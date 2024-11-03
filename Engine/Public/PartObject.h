#pragma once

/* �����̳� ������Ʈ�ȿ� �߰��� �� �ִ� �ϳ��� ��ǰ��ü���� �θ� Ŭ���� */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct
	{
		const _float4x4* pParentWorldMatrix = { nullptr };
	}PARTOBJ_DESC;
	typedef struct EDITING_PARTOBJECT_DESC : PARTOBJ_DESC
	{
		_float* pAnimationSpeed;
	};

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual _int Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Set_State(_uint _eState) {};
	virtual const _float4x4* Get_BoneCombindTransformationMatrix_Ptr(const _char* pBoneName) const { return nullptr; }
	virtual void Set_SocketMatrix(const _float4x4* pSocketMatrix) {};

protected:
	/* m_pTransformCom->m_WorldMatrix * �θ��� ���� */

	const _float4x4*		m_pParentMatrix = { nullptr };
	_float4x4				m_WorldMatrix = {};

protected:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pContantName);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END