#pragma once
#include "math.h"

#include "GameInstance.h"

//거리구하기
inline _float Compute_Distance(_fvector _vPos1, _fvector _vpos2)
{
	return XMVectorGetX(XMVector3Length(_vPos1 - _vpos2));
}

//라디안으로 반환됨
inline _float Compute_Angle(_fvector _vMyLook, _fvector _vMyPos, _fvector _vTargetPos)
{
	return acos(XMVectorGetX(XMVector3Dot(XMVector3Normalize(_vMyLook), XMVector3Normalize(_vTargetPos - _vMyPos))));
}

//라디안으로 반환됨
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

//타겟으로 가는 벡터 (노말라이즈 안함) 이런게 필요하다니
inline _fvector MYCourseVevtor(_fvector _vMyPos, _fvector _vTargetPos)
{
	return _vTargetPos - _vMyPos;
}

//타겟이 앞에 있는지
inline _bool FaceCheck(_fvector _vMyLook, _fvector _vMyPos, _fvector _vTargetPos)
{
	return 0 <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(_vMyLook), XMVector3Normalize(_vTargetPos - _vMyPos)));
}

//벡터 인자 swap해서 반환
inline _fvector MYVectorSwapXY(_fvector _vector)
{
	return XMVectorSet(XMVectorGetY(_vector), XMVectorGetX(_vector), XMVectorGetZ(_vector), XMVectorGetW(_vector));
}

//벡터 인자 swap해서 반환
inline _fvector MYVectorSwapYZ(_fvector _vector)
{
	return XMVectorSet(XMVectorGetX(_vector), XMVectorGetZ(_vector), XMVectorGetY(_vector), XMVectorGetW(_vector));
}

//벡터 인자 swap해서 반환
inline _fvector MYVectorSwapZX(_fvector _vector)
{
	return XMVectorSet(XMVectorGetZ(_vector), XMVectorGetY(_vector), XMVectorGetX(_vector), XMVectorGetW(_vector));
}

//target과 수평하게 만들어줌
inline _fvector MYVectorSameY(_fvector _fMyVector, _fvector TargetVector)
{
	return XMVectorSetY(_fMyVector, XMVectorGetY(TargetVector));
}

//return Zero Y vector
inline _fvector MYVectorZeroY(_fvector _MyVector)
{
	return XMVectorSetY(_MyVector, 0.f);
}

//temp 에 src를 더해줌
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