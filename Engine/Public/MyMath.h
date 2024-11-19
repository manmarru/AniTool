#pragma once
#include "math.h"

#include "GameInstance.h"

//�Ÿ����ϱ�
inline _float Compute_Distance(_fvector _vPos1, _fvector _vpos2)
{
	return XMVectorGetX(XMVector3Length(_vPos1 - _vpos2));
}

//�������� ��ȯ��
inline _float Compute_Angle(_fvector _vMyLook, _fvector _vMyPos, _fvector _vTargetPos)
{
	return acos(XMVectorGetX(XMVector3Dot(XMVector3Normalize(_vMyLook), XMVector3Normalize(_vTargetPos - _vMyPos))));
}

//�������� ��ȯ��
inline _float Compute_Angle(_fvector _vMyLook, _fvector _vTargetLook)
{
	_vector MYNorLook = XMVector3Normalize(_vMyLook);
	_vector TargetNorLook = XMVector3Normalize(_vTargetLook);

	_float fDot = XMVectorGetX(XMVector3Dot(MYNorLook, TargetNorLook));
	//fDot = clamp(fDot, -1.f, 1.f);
	fDot = max(fDot, -1.f);
	fDot = min(fDot, 1.f);
	_float fAngle = acosf(fDot);

	_float vCross = XMVectorGetY(XMVector3Cross(MYNorLook, TargetNorLook));
	 
	if (signbit(vCross))
	{
		fAngle = -fAngle;
	}

	return fAngle;
}

//Ÿ������ ���� ���� (�븻������ ����) �̷��� �ʿ��ϴٴ�
inline _fvector MYCourseVevtor(_fvector _vMyPos, _fvector _vTargetPos)
{
	return _vTargetPos - _vMyPos;
}

//Ÿ���� �տ� �ִ���
inline _bool FaceCheck(_fvector _vMyLook, _fvector _vMyPos, _fvector _vTargetPos)
{
	return 0 <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(_vMyLook), XMVector3Normalize(_vTargetPos - _vMyPos)));
}

//���� ���� swap�ؼ� ��ȯ
inline _fvector MYVectorSwapXY(_fvector _vector)
{
	return XMVectorSet(XMVectorGetY(_vector), XMVectorGetX(_vector), XMVectorGetZ(_vector), XMVectorGetW(_vector));
}

//���� ���� swap�ؼ� ��ȯ
inline _fvector MYVectorSwapYZ(_fvector _vector)
{
	return XMVectorSet(XMVectorGetX(_vector), XMVectorGetZ(_vector), XMVectorGetY(_vector), XMVectorGetW(_vector));
}

//���� ���� swap�ؼ� ��ȯ
inline _fvector MYVectorSwapZX(_fvector _vector)
{
	return XMVectorSet(XMVectorGetZ(_vector), XMVectorGetY(_vector), XMVectorGetX(_vector), XMVectorGetW(_vector));
}

//target�� �����ϰ� �������
inline _fvector MYVectorSameY(_fvector _fMyVector, _fvector TargetVector)
{
	return XMVectorSetY(_fMyVector, XMVectorGetY(TargetVector));
}

//return Zero Y vector
inline _fvector MYVectorZeroY(_fvector _MyVector)
{
	return XMVectorSetY(_MyVector, 0.f);
}

//temp �� src�� ������
inline void float3_Plus_Vector(_float3* Temp, _fvector Src)
{
	XMStoreFloat3(Temp, XMVectorAdd(XMLoadFloat3(Temp), Src));
}

//XMVector -> XMfloat3
inline _float3 MYGetFloat3(_fvector _pSource)
{
	_float3 vResult{};
	
	XMStoreFloat3(&vResult, _pSource);

	return vResult;
}