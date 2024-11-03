#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
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
	_float* m_pAnimationSpeed = { nullptr };//이거 사용할거면 꼭 동적할당 해줘라;;

public:
	virtual void Set_CurrentTrackPosition(_double dPosition);
	virtual _double Get_CurrentTrackPosition();
	virtual _double Get_Duration();
	virtual _double* Get_CurrentTrackPosition_ptr();
	virtual _uint Get_CurrentAnimationIndex();

private:
	CGameObject* m_pUnit = { nullptr };
	
private:
	list<CModel*> m_pContaining_Models;

};

END