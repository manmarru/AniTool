#pragma once

#include "Client_Defines.h"


#include "Level.h"


BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CCommander;
class CProp;
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
	_bool m_bEffectTrigger = { false };
	_bool m_bShow_TriggerSetting = { false };

	_bool m_bShow_BoneFlags = { false };
	_bool m_bShow_Chain = { false };
	_bool m_bShow_Prop = { false };

	_bool m_bShow_EventTriggerPopup = { false };
	_bool m_bShow_ChainPopup = { false };
	_bool m_bShow_SpeedPopup = { false };

	_float m_fFlagScale = { 1.f };
	_float* m_pAnimationSpeed = { nullptr };
	_float m_fFlag_AnimationSpeed = { 1.f };
	CCommander* m_pCommander = { nullptr };
	CProp* m_pProp = { nullptr };
	_bool m_bPropRotationSyncro = { false };
	_float m_fPropRotationX = { 0.f }; // 사원수임
	_float m_fPropRotationY = { 0.f };
	_float m_fPropRotationZ = { 0.f };
	map<_uint, vector<_double>>			m_mapEventTriggers;	// 애니번호, 시간 순서쌍의 트리거
	map<_uint, vector<EFFECTTRIGGER>>	m_mapEffectTriggers;// 이펙트트리거 : 애니번호 -> 시간, 뼈이름
	map<_uint, vector<SPEEDTRIGGER>>	m_mapSpeedTriggers;	// 스피드트리거 : 애니번호 -> 시간, 스피드 (가급적이면 1로 돌아오는 포인트도 찍어주자.)
	vector<class CBoneFlag*>		m_vecFlags;
	list<CHAIN>						m_listAniChained;
	CHAIN							m_tChain = { 0,0 };

	//팝업용
	list<CHAIN>::iterator			m_SelectedChain = {};
	vector<SPEEDTRIGGER>::iterator	m_SelectedSpeedTrigger = {};
	vector<_double>::iterator		m_SelectedEventTrigger = {};

	_uint	m_iSelectedBone = { 0 };
	_int	m_iInput = { 0 };

	_int	m_iFixEventTirgger_AniIndex = {};
	_double m_dFixEventTrigger_TriggerPos = {};

	string	stlFilePath;

private:
	void Format_ImGUI();
	void Format_Trigger();
	void Format_SelectBone();
	void Format_AniChain();
	void Format_Prop();
	void Imgui_Dialogue();


	void TriggerSetting_Event();
	void TriggerSetting_Effect();
	void TriggerSetting_Speed();

	void Clear_SaveMap();
	void Save_Triggers();
	void Load_Triggers();
	void Save_ChainnedAnimation();
	void Load_ChainnedAnimation();
	void Passing_Trigger(); // 파싱해서 editobj에 적용하기

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
