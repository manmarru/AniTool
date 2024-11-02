#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CFSM :
    public CBase
{
private:
    CFSM();
    virtual ~CFSM() = default;

public:
    HRESULT Initialize(CModel* _pModel);
    _bool Update(_float _fTimeDelta);
    void Late_Update();

public:
    void Register_AnimationIndex(OBJ_STATE _eState, _uint _AnimationIndex, ANITYPE _AniType);
    void Set_State(OBJ_STATE _eState);
    void SetUp_Animation(_uint _iAnimationIndex, ANITYPE _AniType);
    _bool Play_Animation(_float _fTimeDelta);
    void Set_RootAnim(const char* _RootName);

public:
    OBJ_STATE Get_CurrentState() { return m_eCurrentState; }

private:
    _uint       m_iCurrentIndex = { 0 };
    OBJ_STATE   m_eCurrentState = { OBJSTATE_END };
    ANITYPE     m_eCurrentAniType = { ANI_END };
    _bool       m_isRootControl = { false };
    const char* m_RootName;
    _matrix     m_RootMatrix_Inverse = {};

private: //컴포넌트 + 컨테이너
    CModel* m_pModelCom = { nullptr };
    map<OBJ_STATE, pair<_uint, ANITYPE>> m_mapAnimationIndex;

public:
    static CFSM* Create(CModel* _pModel);
    virtual void Free();
};

END