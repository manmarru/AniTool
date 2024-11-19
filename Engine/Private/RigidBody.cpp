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
	if (m_bFalling)
	{
		m_vInertia.y -= m_fGravity * fTimeDelta;
	}
	m_vInertia.x *= 0.9f;
	m_vInertia.z *= 0.9f;

	if (abs(m_vInertia.x) < 0.1f)
		m_vInertia.x = 0.f;
	if (abs(m_vInertia.z) < 0.1f)
		m_vInertia.z = 0.f;

	_vector convInertia = XMLoadFloat3(&m_vInertia);
	
	_matrix temp = XMLoadFloat4x4(m_pWorldMatrix);
	temp.r[3] = XMVectorAdd(temp.r[3], convInertia);
	XMStoreFloat4x4(m_pWorldMatrix, temp);
	
	XMStoreFloat3(&m_vInertia, convInertia);
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
