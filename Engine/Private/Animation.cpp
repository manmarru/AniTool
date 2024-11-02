#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(DATA_BINANIM* pAIAnimation, vector<_uint>& KeyFrameIndices, class CModel* pModel)
{
	strcpy_s(m_szName, pAIAnimation->szName);

	m_Duration = pAIAnimation->Duration;

	m_SpeedPerSec = pAIAnimation->TickPerSecond;

	/* ���� �ִϸ��̼ǿ��� �����ؾ��� ������ ������ �����Ѵ�. */
	m_iNumChannels = pAIAnimation->iNumChannels;


	KeyFrameIndices.resize(m_iNumChannels);

	//m_isLoop = pAIAnimation->bLoop;

	/* ���� �ִϸ��̼ǿ��� �����ؾ��� ���������� �����Ͽ� �����Ѵ�. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(&pAIAnimation->pBinChannel[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _float fTimeDelta, _float fPlaySpeed, _double dSubTime)
{
	/* ���� �����ġ�� ����ϳ�. */
	*pCurrentTrackPosition += m_SpeedPerSec * fTimeDelta * fPlaySpeed;

	if (*pCurrentTrackPosition >= (m_Duration - dSubTime))
	{
		if (false == isLoop)
		{
			return true;
		}
		*pCurrentTrackPosition = 0.f;
	}

	/* ���� �����ġ�� �°� ���� �ִϸ��̼��� ��Ʈ���ؾ� �� ���� ���µ��� �������ش�. */
	_uint		iChannelIndex = { 0 };
	m_iKeyFrameIndex = CurrentKeyFrameIndices.back();
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, &CurrentKeyFrameIndices[iChannelIndex++], *pCurrentTrackPosition);
	}

	return false;
}


CAnimation* CAnimation::Create(DATA_BINANIM* pAIAnimation, vector<_uint>& KeyFrameIndices, class CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, KeyFrameIndices, pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation_Bin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
