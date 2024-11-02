#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(DATA_BINANIM* pAIAnimation, vector<_uint>& KeyFrameIndices, class CModel* pModel);
	_bool Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _float fTimeDelta, _float fPlaySpeed = 1.f, _double dSubTime = 0.);

public://get
	vector<class CChannel*>	Get_Channels(void) { return m_Channels; }
	const _char* Get_Name() const { return m_szName; }
	_uint& Get_KeyFrameIdxex() { return m_iKeyFrameIndex; }
private:
	char				m_szName[MAX_PATH] = "";
	/* �� �ִϸ��̼��� ����ϴ� ���� ���� */
	_uint						m_iNumChannels = { 0 };
	/* ���� ������ ����*/
	/* Channel : �� (�ð��� ���� �� ���� ���� ����� �����Ѵ�) */
	vector<class CChannel*>		m_Channels;

	/* �� �ִϸ��̼��� �����ϱ����� �Ÿ��� ��ü �Ÿ�. */
	_double						m_Duration = {};

	/* �ִϸ��̼��� �ʴ� ����ӵ� */
	_double						m_SpeedPerSec = {};

	//Ű������ �ε���
	_uint				m_iKeyFrameIndex = { 0 };


public:
	static CAnimation* Create(DATA_BINANIM* pAIAnimation, vector<_uint>& KeyFrameIndices, class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END