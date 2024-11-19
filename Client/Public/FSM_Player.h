#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CFSM_Player :
    public CBase
{
public:
    enum FSM_STATE { FSM_COMM, FSM_SH, FSM_AXE, FSM_BOW, FSM_END };
private:
    CFSM_Player();
    virtual ~CFSM_Player() = default;

public:
    HRESULT Initialize(CModel* _pModel, _uint* pFSMIndex);
    _bool Update(_float _fTimeDelta);
    void Late_Update();

public:
    //Set
    void Register_Chain(const _char* _pDataFilePath);
    void Set_State(OBJ_STATE _eState, _bool _bManual = false, _bool bLerp = true);
    void Set_ChainState(OBJ_STATE _eState, _bool bLerp = true);
    void isNotActing() { m_isActing = false; }
    void isActing() { m_isActing = true; }
    void SetUp_Animation(_uint _iAnimationIndex, ANITYPE _AniType, _bool bLerp = true);

    //Get
    _bool ActingCheck() { return m_isActing; }
    OBJ_STATE Get_CurrentState() { return m_eCurrentState; }
    ANITYPE Get_CurrentAniType() { return m_eCurrentAnitype; }

public:
    _bool Play_Animation(_float _fTimeDelta); //무조건 트리거임

private:
    _uint m_iCurrentIndex = { 0 };
	_uint* m_pCurrentFSMIndex = { nullptr };
    OBJ_STATE m_eCurrentState = { OBJSTATE_END };
    ANITYPE m_eCurrentAnitype = { ANI_END };
    _bool m_isActing = { false };
    _bool m_isLerpChain = { false };
    string m_strCurrentChain;


private:
    CModel* m_pModelCom = { nullptr };
    map <OBJ_STATE, pair<_uint, ANITYPE>> m_mapAnimationIndex[FSM_END]; // 무기별 상태-애니
    map<OBJ_STATE, string> m_mapChainTag[FSM_END];                      // 체인
    map<string, CHAIN_FOR_MAP> m_mapChain;
private:
    void Register_AnimationIndex();

public:
    static CFSM_Player* Create(CModel* _pModel, _uint* pFSMIndex);
    virtual void Free() override;
};

END