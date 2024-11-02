#pragma once

#include "Component_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "SoundManager.h"

/* 1. ������ Ŭ���̾�Ʈ�� ������ ���� Ŭ������. */
/* 2. �������� Ŭ���̾�Ʈ�� �����ְ� ���� �Լ����� ��Ƴ��´�. */
/* 3. �Լ��� -> Ŭ������ ����Լ�. ��ü�� �ʿ��ϴ�! �׷��� ������� �������ֱ����� Ŭ���� ��ü�� �����Ѵ�. */
/* 4. ��ü���� �����ϱ����� Initialize_Engine() �Լ� �ȿ��� ����� �������ֱ����� Ŭ�������� ��üȭ�Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	HRESULT Clear(_uint iLevelIndex);
	_float Get_Random_Normal() const {
		return (_float)rand() / RAND_MAX;
	}

	_float Get_Random(_float fMin, _float fMax) const {
		return fMin + Get_Random_Normal() * (fMax - fMin);
	}
	
#pragma region GRAPHIC_DEVICE
public:
	ID3D11ShaderResourceView* Get_BackBuffer_SRV() const;
	void Render_Begin();
	void Render_End();
#pragma endregion

#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_bool	Get_DIKeyState_Once(_ubyte byKeyID);
	_bool	Get_DIKeyState_Once_Up(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_bool	Get_DIMouseState_Once(MOUSEKEYSTATE eMouse);
	_bool	Get_DIMouseState_Once_Up(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_GameObject_Out(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, OUT CGameObject*& pGameObjectOut, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0, _uint iPartObjIndex = 0);
	class CGameObject* Get_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	list<CGameObject*>* Get_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag);
	void Clear_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	HRESULT Out_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, class CGameObject* pGameObject);
	HRESULT Out_GameObjectList(_uint iLevelIndex, const _wstring& strLayerTag);
	HRESULT ReAdd_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, class CGameObject* pGameObject);
#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);	
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);

#ifdef _DEBUG
	HRESULT Add_DebugObject(class CComponent* pDebugObject);
#endif
#pragma endregion

#pragma region PIPELINE
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;

	_float4 Get_CamPosition_Float4() const;
	_vector Get_CamPosition_Vector() const;
#pragma endregion

#pragma region LIGHT_MANAGER
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void Lights_Clear();
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(class CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName);
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture);

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif


#pragma endregion

#pragma region PICKING
	_bool Picking(_float3* pPickPos); 
	_vector Compute_Height(_fvector vWorldPos, _fmatrix ViewMatrix, _fmatrix ProjMatrix);
#pragma endregion

#pragma region FRUSTUM
	public:
		_bool isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius = 0.f);
		_bool isIn_Frustum_LocalSpace(_fvector vPosition, _float fRadius);
		void Transform_ToLocalSpace(_fmatrix WorldMatrix);
#pragma endregion

#pragma region SOUND_MANAGER
public:
	void		Update_Sound();
	HRESULT		PlayDefault(TCHAR* _TagSound, _int _Channel, _float _fVloume, _bool _bLoop);
	HRESULT		PlayBGM(const TCHAR* _TagSound, _float _fVolume, _bool _bLoop);
	HRESULT		Play3D(TCHAR* _TagSound, _int _Channel, _float _fVolume, _float3 _vPos, _bool _bLoop = false);
	HRESULT		Play3DRemain(TCHAR* _TagSound, _float _fVolume, _float3 _vPos, _bool _bLoop = false);
	HRESULT		Play_NonStack(TCHAR* _TagSound, _int _Channel, _float _fVolume, _bool _bLoop);
	void		MuteAll();
	void		StopSound(int _iChannel);
	void		StopSoundALL();
	void		Set_ChannelVolume(int _Channel, _float _fVolume);
	HRESULT		Set_SoundPos(_int _iChannel, _float3 _vPos);
	HRESULT		Add_Sound(const char* _fullpath, const TCHAR* _filename, const TCHAR* _path);
	HRESULT		Load_Sound(const char* _folderName);
	HRESULT		Set_Listener(_float3 _vPos, _float3 vForward);
#pragma endregion

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CSoundManager*			m_pSound_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };

public:	
	void Release_Engine();
	virtual void Free() override;
};

END