#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CBone;
END

BEGIN(Client)

class CCommander
{
public:
	CCommander();
	~CCommander();


//For Edit
public:
	void Register(CGameObject* pObj);
public:
	_float* m_pAnimationSpeed = { nullptr };//이거 사용할거면 꼭 동적할당 해줘야 함!
	_uint m_AnimationIndesc = { 0 };

public:
	virtual void Set_CurrentTrackPosition(_double _dPosition);
	virtual _double Get_CurrentTrackPosition();
	virtual _double Get_Duration();
	virtual _double* Get_CurrentTrackPosition_ptr();
	virtual _uint Get_CurrentAnimationIndex();
	void Set_Animation(_uint _iAnimationIndex);
	void Set_Animation(string _strChain);
	_uint Get_AnimationNum();
	const vector<CBone*>* Get_Bones();
	void Register_Trigger(map<_uint, vector<_double>>* _pEventTrigger, map<_uint, vector<EFFECTTRIGGER>>* _pEffectTrigger, map<_uint, queue<SPEEDTRIGGER>>* _pSpeedTrigger);
	void Setup_Chains(ifstream* _LoadStream);
	void Reset_Animation();
	void Turn90();

public:
	void Key_Input(_float _fTimeDelta);

private:
	CGameObject* m_pUnit = { nullptr };
	
private:
	list<CModel*>	m_pContaining_Models;
	class CFSM*		m_pFSM = { nullptr };
	map<_uint, vector<_double>> EventTrigger;
	map<_uint, vector<EFFECTTRIGGER>> EffectTrigger;


};

END