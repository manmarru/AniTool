#pragma once
#include "Client_Defines.h"
#include "FreeCamera.h"
#include "Base.h"

BEGIN(Client)
class CActor;

class CCameraManager final : public CBase
{
	DECLARE_SINGLETON(CCameraManager)
public:
	enum CAM_STATE { CAM_INIT, CAM_TARGET, CAM_ACTION,  CAM_END };
	enum LOAD_CAMERATYPE { TARGETMODE, ACTION };
public:
	CCameraManager();
	virtual ~CCameraManager() = default;

public: 
	CAM_STATE Get_CamState() { return m_eCamState; }
	CCamera* Get_CurrentCamera() { return m_pCurrentCamera; }
	void Set_CamState(CAM_STATE _eState);
	void Set_CurrentCamera(CCamera* _pCamera) { m_pCurrentCamera = _pCamera; }

public:
	HRESULT Ready_Camera(LEVELID eLevelIndex);

private:
	CCamera* m_pCurrentCamera = nullptr;
	CAM_STATE					m_eCamState = CAM_TARGET;
	LEVELID						m_eCurrentLevel;


	class CGameInstance* m_pGameInstance;
	vector<class CBaseObj*>			m_pActionObjects;

public:
	virtual void Free() override;
};

END
