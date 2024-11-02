#include "stdafx.h"

#include "FSM.h"
#include "GameInstance.h"

CFSM::CFSM()
{
}

HRESULT CFSM::Initialize(CModel* _pModel)
{
    if (nullptr == _pModel)
        return E_FAIL;

    m_pModelCom = _pModel;
    Safe_AddRef(_pModel);

    return S_OK;
}

_bool CFSM::Update(_float _fTimeDelta)
{
    if (Play_Animation(_fTimeDelta))
    {
        if (m_eCurrentAniType == ANI_BACKTOIDLE)
            Set_State(OBJSTATE_IDLE);

        return true;
    }

    return false;
}

void CFSM::Late_Update()
{
}

void CFSM::Register_AnimationIndex(OBJ_STATE _eState, _uint _AnimationIndex, ANITYPE _eAniType)
{
    m_mapAnimationIndex[_eState] = pair<_uint, ANITYPE>(_AnimationIndex, _eAniType);
}

//SetUP_Animation이랑 같은거임
void CFSM::Set_State(OBJ_STATE _eState)
{
    if (m_eCurrentState != _eState)
    {
        m_eCurrentState = _eState;
        m_eCurrentAniType = m_mapAnimationIndex[_eState].second;
        m_iCurrentIndex = m_mapAnimationIndex[_eState].first;
        m_pModelCom->Set_NextAnimIndex(m_iCurrentIndex, m_eCurrentAniType == ANI_LOOP);
    }
}

//주의! m_eState를 등록해주지 않는다!
void CFSM::SetUp_Animation(_uint _iAnimationIndex, ANITYPE _AniType)
{
    m_pModelCom->Set_NextAnimIndex(_iAnimationIndex, _AniType == ANI_LOOP);
    m_iCurrentIndex = _iAnimationIndex;
    m_eCurrentAniType = _AniType;
}

_bool CFSM::Play_Animation(_float _fTimeDelta)
{
    if (m_isRootControl)
        return m_pModelCom->Play_Animation(_fTimeDelta, m_RootName);

    return m_pModelCom->Play_Animation(_fTimeDelta);
}

void CFSM::Set_RootAnim(const char* _RootName)
{
    m_RootName = _RootName;
    m_isRootControl = true;
}

CFSM* CFSM::Create(CModel* _pModel)
{
    CFSM* pInstance = new CFSM();

    if (FAILED(pInstance->Initialize(_pModel)))
    {
        MSG_BOX(TEXT("Failed To Create : CFSM"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFSM::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
}
