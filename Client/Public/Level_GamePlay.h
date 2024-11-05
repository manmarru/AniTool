#pragma once

#include "Client_Defines.h"


#include "Level.h"


BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CCommander;
class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();

	HRESULT Ready_EditObj();
	HRESULT Ready_Layer_Player();
	//HRESULT Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

private:
	_bool m_bGuiStart = { false };
	_bool m_bDemoStart = { false };
	_bool m_bControlGUIStart = { false };
	_bool m_bEffectTrigger = { false };
	_float* m_pAnimationSpeed = { nullptr };
	CCommander* m_pCommander = { nullptr };
	map<_uint, vector<_double>> m_mapAnimationSave;
	vector<class CBoneFlag*> m_vecFlags;

	_bool m_bShow_BoneFlags = { false };
	_uint m_iSelectedBone = { 0 };

private:
	void Format_ImGUI();
	void Format_Control();
	void Format_EffectTrigger();
	void Clear_SaveMap();

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
