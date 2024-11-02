#include "stdafx.h"
#include "..\Public\Actor.h"

#include "GameInstance.h"

CActor::CActor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CActor::CActor(const CActor & Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CActor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CActor::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

_int CActor::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	return OBJ_NOEVENT;
}

void CActor::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CActor::Free()
{
	__super::Free();
}
