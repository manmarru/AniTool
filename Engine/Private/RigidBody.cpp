#pragma once

#include "RigidBody.h"
#include "MyMath.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float4x4* pWorldMatrix)
	:CComponent{ pDevice, pContext }
	, m_pWorldMatrix(pWorldMatrix)
{
}

void CRigidBody::Update(_float fTimeDelta)
{
	if (m_bFalling) // 중력
	{
		m_vInertia.y -= m_fGravity * fTimeDelta;
	}

	_float fForce = XMVectorGetX(XMVector2Length(XMVectorSet(m_vInertia.x, m_vInertia.z, 0.f, 0.f)));
	fForce = max(0.f, fForce * (1.f - fTimeDelta * 1.5f));  // 저항 = 1.5

	if (fForce < 0.05f)
	{
		m_vInertia.x = 0.f;
		m_vInertia.z = 0.f;
	}
	else
	{
		_vector InertiaDir = XMVector2Normalize(XMVectorSet(m_vInertia.x, m_vInertia.z, 0.f, 0.f)) * fForce;
		m_vInertia.x = XMVectorGetX(InertiaDir);
		m_vInertia.z = XMVectorGetY(InertiaDir); //vector2라서 z를 y에 넣고 계산했음
	}

	//계산한거 적용
	_vector convInertia = XMLoadFloat3(&m_vInertia);

	_matrix temp = XMLoadFloat4x4(m_pWorldMatrix);
	temp.r[3] = XMVectorAdd(temp.r[3], convInertia);
	XMStoreFloat4x4(m_pWorldMatrix, temp);
}

void CRigidBody::Strike(_fvector vDir, _float fPower)
{
	float3_Plus_Vector(&m_vInertia, vDir * fPower);
	//XMStoreFloat3(&m_vInertia, XMVectorAdd(XMLoadFloat3(&m_vInertia), vDir * fPower));
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float4x4* pWorldMatrix)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext, pWorldMatrix);

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();
	//가급적이면 트랜스폼보다 먼저 지워주자 (월드 댕글링된다)
}
