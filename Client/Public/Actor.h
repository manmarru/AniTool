#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CActor abstract : public CGameObject
{
public:
	enum COLLIDERTYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
protected:
	CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor(const CActor& Prototype);
	virtual ~CActor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual _int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual void Free() override;
};

END