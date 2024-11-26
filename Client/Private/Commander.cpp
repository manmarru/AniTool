#include "stdafx.h"
#include "Commander.h"
#include "GameInstance.h"
#include "FSM.h"

CCommander::CCommander()
{
}

CCommander::~CCommander()
{
}

void CCommander::Register(CGameObject* _pObj)
{
	m_pContaining_Models.clear();

	m_pUnit = _pObj;

	m_pUnit->Register_ModelCom(m_pContaining_Models);

	m_pFSM = static_cast<CFSM*>(m_pUnit->Get_FSM());
}

void CCommander::Set_CurrentTrackPosition(_double _dPosition)
{
	m_pUnit->Set_CurrentTrackPosition(_dPosition);
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

void CCommander::Set_Animation(string _strChain)
{
	m_pFSM->SetUp_Animation(_strChain);
}

_uint CCommander::Get_AnimationNum()
{
	return m_pContaining_Models.front()->Get_AnimationNum();
}

const vector<CBone*>* CCommander::Get_Bones()
{
	return m_pContaining_Models.front()->Get_Bones();
}

void CCommander::Register_Trigger(map<_uint, vector<_double>>* _pEventTrigger, map<_uint, vector<EFFECTTRIGGER>>* _pEffectTrigger, map<_uint, queue<SPEEDTRIGGER>>* _pSpeedTrigger)
{
	m_pContaining_Models.front()->Register_Trigger(_pEventTrigger, _pEffectTrigger, _pSpeedTrigger);
}

void CCommander::Setup_Chains(ifstream* _LoadStream)
{
	m_pFSM->Setup_Chains(_LoadStream);
}

void CCommander::Reset_Animation()
{
	m_pContaining_Models.front()->Set_AnimFrameReset();
}

void CCommander::Turn90()
{
	m_pContaining_Models.front()->Turn90();
}

void CCommander::Key_Input(_float _fTimeDelta)
{
	
}
