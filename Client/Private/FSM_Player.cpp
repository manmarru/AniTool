#pragma once
#include "stdafx.h"
#include "FSM_Player.h"

#include "GameInstance.h"

CFSM_Player::CFSM_Player() {}

HRESULT CFSM_Player::Initialize(CModel* _pModel, _uint* pFSMIndex)
{
	if (nullptr == _pModel)
		return E_FAIL;

	m_pModelCom = _pModel;
	Safe_AddRef(m_pModelCom);

	Register_AnimationIndex();

	Register_Chain("../Bin/Resources/Models/Player/CHAIN_Player.dat");

	m_pCurrentFSMIndex = pFSMIndex;
	
	return S_OK;
}

_bool CFSM_Player::Update(_float _fTimeDelta)
{
	if (Play_Animation(_fTimeDelta))
	{
		if (ANI_BACKTOIDLE == m_eCurrentAnitype)
		{
			Set_State(OBJSTATE_IDLE);
			m_isActing = false;
		}
		else if (ANI_CHAIN == m_eCurrentAnitype)
		{
			SetUp_Animation(m_mapChain[m_strCurrentChain].After, ANI_STOP, m_mapChain[m_strCurrentChain].bLerp);
		}
		else // ani_stop 이런거
			m_isActing = false;
		return true;
	}

	return false;
}

void CFSM_Player::Late_Update() {}

void CFSM_Player::Register_Chain(const _char* _pDataFilePath)
{
	ifstream LoadStream(_pDataFilePath, ios::binary | ios::in);
	_int iSize(0);
	LoadStream.read((char*)&iSize, sizeof(_int));
	CHAIN tChain;
	for (_int i = 0; i < iSize; i++)
	{

		LoadStream.read((char*)&tChain, sizeof(CHAIN));

		//받아온 순서쌍을 연결해줌
		m_mapChain[tChain.ChainTag] = { tChain.Before, tChain.After, tChain.bLerp };
	}
}

void CFSM_Player::Set_State(OBJ_STATE _eState, _bool _bManual, _bool bLerp)
{
	// manual -> 상태제어 안하고 값만 set 하겠다
	if (_bManual) 
	{
		m_eCurrentState = _eState;
		return;
	}
	if (m_eCurrentState != _eState)
	{
		m_eCurrentState = _eState;
		m_eCurrentAnitype = m_mapAnimationIndex[*m_pCurrentFSMIndex][_eState].second;
		m_iCurrentIndex = m_mapAnimationIndex[*m_pCurrentFSMIndex][_eState].first;
		m_pModelCom->Set_NextAnimIndex(m_iCurrentIndex, m_eCurrentAnitype == ANI_LOOP, bLerp ? 0.2f : 0.f);
		//if (ANI_LOOP != m_eCurrentAnitype)
		//	m_isActing = true;
		// 모델에서 같은 상태지시를 했지만 트랙포지션이 듀레이션을 넘긴 경우 currenttrackposition을 0으로 만들어줘야한다, 또는 보간을 안하거나(이건 확실하진않음)
	}
}

void CFSM_Player::Set_ChainState(OBJ_STATE _eState, _bool bLerp)
{
	if (m_eCurrentState != _eState)
	{
		m_eCurrentState = _eState;
		m_eCurrentAnitype = ANI_CHAIN;
		m_strCurrentChain = m_mapChainTag[*m_pCurrentFSMIndex][_eState];
		m_iCurrentIndex = m_mapChain[m_strCurrentChain].Before;
		m_pModelCom->Set_NextAnimIndex(m_iCurrentIndex, ANI_STOP, bLerp ? 0.2f : 0.f);
	}
}

//주의! m_eState 를 등록해주지 않고 강제로 set 해버린다!
void CFSM_Player::SetUp_Animation(_uint _iAnimationIndex, ANITYPE _AniType, _bool _bLerp)
{
	m_pModelCom->Set_NextAnimIndex(_iAnimationIndex, _AniType == ANI_LOOP, _bLerp ? 0.2f : 0.f);
	m_iCurrentIndex = _iAnimationIndex;
	m_eCurrentAnitype = _AniType;
}

_bool CFSM_Player::Play_Animation(_float _fTimeDelta)
{
	return m_pModelCom->Play_TriggerAnimation(_fTimeDelta);
}

void CFSM_Player::Register_AnimationIndex()
{

#pragma region COMM

	m_mapAnimationIndex[FSM_COMM][OBJSTATE_THROW] = { 22, ANI_BACKTOIDLE };

	m_mapAnimationIndex[FSM_COMM][OBJSTATE_TUMBLEE] = { 32, ANI_BACKTOIDLE };
	m_mapAnimationIndex[FSM_COMM][OBJSTATE_TUMBLEE_F] = { 29, ANI_BACKTOIDLE };

	m_mapAnimationIndex[FSM_COMM][OBJSTATE_TUMBLING] = { 40, ANI_BACKTOIDLE };

	m_mapAnimationIndex[FSM_COMM][OBJSTATE_RUN_B] = { 57, ANI_LOOP };
	m_mapAnimationIndex[FSM_COMM][OBJSTATE_RUN] = { 58, ANI_LOOP };
	m_mapAnimationIndex[FSM_COMM][OBJSTATE_RUSH] = { 59, ANI_LOOP };
	m_mapAnimationIndex[FSM_COMM][OBJSTATE_WALK_B] = { 60, ANI_LOOP };
	m_mapAnimationIndex[FSM_COMM][OBJSTATE_WALK] = { 61, ANI_LOOP };
	m_mapAnimationIndex[FSM_COMM][OBJSTATE_IDLE] = { 62, ANI_LOOP };
	
	m_mapAnimationIndex[FSM_COMM][OBJSTATE_SH_DRAW] = { 5, ANI_BACKTOIDLE };

	m_mapChainTag[FSM_COMM][OBJSTATE_TUMBLEB] = "TumbleB";
	m_mapChainTag[FSM_COMM][OBJSTATE_TUMBLER] = "TumbleR";
	m_mapChainTag[FSM_COMM][OBJSTATE_TUMBLEL] = "TumbleL";
	m_mapChainTag[FSM_COMM][OBJSTATE_TUMBLEF] = "TumbleF";
#pragma endregion

#pragma region SH

	m_mapAnimationIndex[FSM_SH][OBJSTATE_IDLE] = { 20, ANI_LOOP };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_ATT1] = { 0, ANI_BACKTOIDLE };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_ATT2] = { 1, ANI_BACKTOIDLE };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SKILL1] = { 2, ANI_BACKTOIDLE };	// 강공1
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SKILL2] = { 3, ANI_BACKTOIDLE };	// 강공2
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SKILL3] = { 7, ANI_BACKTOIDLE };	// 점프스매쉬
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SKILL4] = { 11, ANI_BACKTOIDLE };	// 배쉬 (패링)
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SKILL5] = { 12, ANI_BACKTOIDLE };	// 쉴드 러쉬 (12E) (13L) (14S) 
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SKILL6] = { 16, ANI_LOOP };		// 가드 (15E) (16L), (17S)
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SKILL7] = { 18, ANI_BACKTOIDLE };	// 차지검기
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SKILL8] = { 19, ANI_BACKTOIDLE };	// 방패날리기

	m_mapAnimationIndex[FSM_SH][OBJSTATE_BACKROLL] = { 4, ANI_BACKTOIDLE };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_ROLL] = { 8, ANI_BACKTOIDLE };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_SH_UNDRAW] = { 6, ANI_BACKTOIDLE };

	m_mapAnimationIndex[FSM_SH][OBJSTATE_RUN_B] = { 9, ANI_LOOP };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_RUN] = { 10, ANI_LOOP };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_RUN_S] = { 63, ANI_CHAIN };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_RUN_TURN90L] = { 64, ANI_CHAIN };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_RUN_TURN180L] = { 65, ANI_CHAIN };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_RUN_TURN90R] = { 66, ANI_CHAIN };
	m_mapAnimationIndex[FSM_SH][OBJSTATE_RUN_TURN180R] = { 67, ANI_CHAIN };
	
	m_mapChainTag[FSM_SH][OBJSTATE_RUN_S]			= "SH_Run_S";
	m_mapChainTag[FSM_SH][OBJSTATE_RUN_TURN90L]		= "SH_Run_90L";
	m_mapChainTag[FSM_SH][OBJSTATE_RUN_TURN180L]	= "SH_Run_180L";
	m_mapChainTag[FSM_SH][OBJSTATE_RUN_TURN90R]		= "SH_Run_90R";
	m_mapChainTag[FSM_SH][OBJSTATE_RUN_TURN180R]	= "SH_Run_180R";

#pragma endregion

#pragma region AXE

#pragma endregion

#pragma region BOW

#pragma endregion
}

CFSM_Player* CFSM_Player::Create(CModel* _pModel, _uint* pFSMIndex)
{
	CFSM_Player* pInstance = new CFSM_Player();

	if (FAILED(pInstance->Initialize(_pModel, pFSMIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CFSM_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFSM_Player::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	for (size_t i = 0; i < FSM_END; i++)
	{
		m_mapAnimationIndex[i].clear();
	}
	m_mapChain.clear();
}
