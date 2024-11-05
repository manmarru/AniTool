#include "stdafx.h"
#include "Commander.h"
#include "GameInstance.h"

CCommander::CCommander()
{
}

CCommander::~CCommander()
{
}

void CCommander::Register(CGameObject* pObj)
{
	m_pContaining_Models.clear();

	m_pUnit = pObj;

	m_pUnit->Register_ModelCom(m_pContaining_Models);
}

void CCommander::Set_CurrentTrackPosition(_double dPosition)
{
	m_pUnit->Set_CurrentTrackPosition(dPosition);
}

_double CCommander::Get_CurrentTrackPosition()
{
	return m_pContaining_Models.front()->Get_CurrentTrackPosition();
}

_double CCommander::Get_Duration()
{
	return m_pContaining_Models.front()->Get_Duration();
}

_double* CCommander::Get_CurrentTrackPosition_ptr()
{
	return m_pContaining_Models.front()->Get_CurrentTrackPosition_ptr();
}

_uint CCommander::Get_CurrentAnimationIndex()
{
	return m_pContaining_Models.front()->Get_CurrentAnimationIndex();
}

void CCommander::Set_Animation(_uint _iAnimationIndex)
{
	for (auto model : m_pContaining_Models)
	{
		model->Set_NextAnimIndex(_iAnimationIndex, true);
	}
}

_uint CCommander::Get_AnimationNum()
{
	return m_pContaining_Models.front()->Get_AnimationNum();
}

void CCommander::Key_Input(_float _fTimeDelta)
{
	
}
