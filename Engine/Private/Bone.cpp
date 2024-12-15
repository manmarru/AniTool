#include "..\Public\Bone.h"

CBone::CBone()
{

}

_float3* CBone::Get_CombinedPos()
{
	return reinterpret_cast<XMFLOAT3*>(&m_CombinedTransformationMatrix._41);
}

HRESULT CBone::Initialize(DATA_BINNODE* pNode)
{
	m_iParentBoneIndex = pNode->m_iParentBoneIndex;

	strcpy_s(m_szName, pNode->cName);

	XMStoreFloat4x4(&m_TransformationMatrix, (XMLoadFloat4x4(&pNode->mTransform)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_vDivideMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix * m_vDivideMatrix);
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix() * m_vDivideMatrix);
}

void CBone::Update_CombinedTransformationMatrix_PassingRotation(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	_vector vRotation;
	_vector vScale;
	_vector vPos;
	XMMatrixDecompose(&vScale, &vRotation, &vPos, XMLoadFloat4x4(&m_TransformationMatrix));

	vRotation *= -1;

	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixScalingFromVector(vScale) * PreTransformMatrix);
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixScalingFromVector(vScale) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
}

CBone* CBone::Create(DATA_BINNODE* pNode)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pNode)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}


void CBone::Free()
{
	__super::Free();

}
