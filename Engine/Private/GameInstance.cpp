#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "Picking.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	// srand(unsigned(time(nullptr)));

	/* �׷��� ī�带 �ʱ�ȭ�ϳ�. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;


	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	/* ���� ī�带 �ʱ�ȭ�ϳ�. */

	/* �Է���ġ�� �ʱ�ȭ�ϳ�. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	//m_pPicking = CPicking::Create(*ppDevice, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	//if (nullptr == m_pPicking)
	//	return E_FAIL;

	/* �������� �Ŵ����� �ʱ�ȭ�ϳ�. */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pSound_Manager = CSoundManager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* ���� Ű����� ���콺�� ���¸� �޾ƿò���. */
	m_pInput_Device->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);	

	m_pPipeLine->Update();

	m_pFrustum->Update();

	m_pObject_Manager->Update(fTimeDelta);
	
	m_pObject_Manager->Late_Update(fTimeDelta);
	
	m_pLevel_Manager->Update(fTimeDelta);		
}

HRESULT CGameInstance::Draw_Engine()
{
	
	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	/* Ư�� ������ ���� �����ϴ� �ڿ����� �ش� ������ ������ ��������. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* ������Ʈ �����鵵 �������� �����߾���. */
	if (iLevelIndex > 1)
		m_pComponent_Manager->Clear(iLevelIndex);

	return S_OK;
}

ID3D11ShaderResourceView * CGameInstance::Get_BackBuffer_SRV() const
{
	return m_pGraphic_Device->Get_BackBuffer_SRV();	
}

void CGameInstance::Render_Begin()
{
	/*m_pGraphic_Device->Render_Begin();*/
	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();
	

}

void CGameInstance::Render_End()
{
	/*m_pGraphic_Device->Render_End(hWnd);*/

	m_pGraphic_Device->Present();
	
	
}
#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_bool CGameInstance::Get_DIKeyState_Once(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState_Once(byKeyID);
}

_bool CGameInstance::Get_DIKeyState_Once_Up(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState_Once_Up(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_bool CGameInstance::Get_DIMouseState_Once(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState_Once(eMouse);
}

_bool CGameInstance::Get_DIMouseState_Once_Up(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState_Once_Up(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel * pNextLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNextLevel);	
}

#pragma endregion

#pragma region OBJECT_MANAGER

HRESULT CGameInstance::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Add_CloneObject_ToLayer_Get(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer_Get(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject * CGameInstance::Clone_GameObject(const _wstring & strPrototypeTag, void * pArg)
{
	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}


CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex, iPartObjIndex);
}

CGameObject* CGameInstance::Get_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Object(iLevelIndex, strLayerTag, iIndex);
}

list<CGameObject*>* CGameInstance::Get_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_ObjectList(iLevelIndex, strLayerTag);
}

void CGameInstance::Clear_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Clear_Layer(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Out_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	return m_pObject_Manager->Out_GameObject(iLevelIndex, strLayerTag, pGameObject);
}

HRESULT CGameInstance::Out_GameObjectList(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Out_GameObjectList(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::ReAdd_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	return m_pObject_Manager->ReAdd_GameObject(iLevelIndex, strLayerTag, pGameObject);
}


#pragma endregion

#pragma region COMPONENT_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, CComponent * pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

#pragma endregion

#pragma region TIMER_MANAGER

HRESULT CGameInstance::Add_Timer(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion


#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject * pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}
#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugObject(CComponent * pDebugObject)
{
	return m_pRenderer->Add_DebugObject(pDebugObject);	
}
#endif
#pragma endregion

#pragma region PIPELINE
void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}
_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}
_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}
_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}
HRESULT CGameInstance::Add_Light(const LIGHT_DESC & LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
const LIGHT_DESC * CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Render_Lights(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

void CGameInstance::Lights_Clear()
{
	return m_pLight_Manager->Lights_Clear();
}


#pragma endregion

#pragma region FONT_MANAGER 

HRESULT CGameInstance::Add_Font(const _wstring & strFontTag, const _tchar * pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}
HRESULT CGameInstance::Render_Text(const _wstring & strFontTag, const _tchar * pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale);
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring & strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);	
}

HRESULT CGameInstance::Add_MRT(const _wstring & strMRTTag, const _wstring & strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring & strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(CShader * pShader, const _wstring & strTargetTag, const _char * pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pConstantName);
}

HRESULT CGameInstance::Copy_RenderTarget(const _wstring & strTargetTag, ID3D11Texture2D * pTexture)
{
	return m_pTarget_Manager->Copy_RenderTarget(strTargetTag, pTexture);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_MRT_Debug(const _wstring & strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pTarget_Manager->Render(strMRTTag, pShader, pVIBuffer);	
}

#endif

#pragma endregion

#pragma region PICKING

_bool CGameInstance::Picking(_float3 * pPickPos)
{
	return m_pPicking->Picking(pPickPos);
}

_vector CGameInstance::Compute_Height(_fvector vWorldPos, _fmatrix ViewMatrix, _fmatrix ProjMatrix)
{

	return m_pPicking->Compute_Height(vWorldPos, ViewMatrix, ProjMatrix);
}

#pragma endregion

#pragma region FRUSTUM

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_WorldSpace(vPosition, fRadius);
}

_bool CGameInstance::isIn_Frustum_LocalSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_LocalSpace(vPosition, fRadius);
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	return m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}

#pragma endregion

#pragma region SOUND
void CGameInstance::Update_Sound()
{
	m_pSound_Manager->Update();
}

HRESULT CGameInstance::PlayDefault(TCHAR* _TagSound, _int _Channel, _float _fVloume, _bool _bLoop)
{
	return m_pSound_Manager->PlayDefault(_TagSound, CSoundManager::SOUND_CHANNEL(_Channel), _fVloume, _bLoop);
}

HRESULT CGameInstance::PlayBGM(const TCHAR* _TagSound, _float _fVolume, _bool _bLoop)
{
	return m_pSound_Manager->PlayBGM(_TagSound, _fVolume, _bLoop);
}

HRESULT CGameInstance::Play3D(TCHAR* _TagSound, _int _Channel, _float _fVolume, _float3 _vPos, _bool _bLoop)
{
	return m_pSound_Manager->Play3D(_TagSound, _Channel, _fVolume, _vPos, _bLoop);
}

HRESULT CGameInstance::Play3DRemain(TCHAR* _TagSound, _float _fVolume, _float3 _vPos, _bool _bLoop)
{
	return m_pSound_Manager->Play3DRemain(_TagSound, _fVolume, _vPos, _bLoop);
}

HRESULT CGameInstance::Play_NonStack(TCHAR* _TagSound, _int _Channel, _float _fVolume, _bool _bLoop)
{
	return m_pSound_Manager->Play_NonStack(_TagSound, _Channel, _fVolume, _bLoop);
}

void CGameInstance::MuteAll()
{
	m_pSound_Manager->MuteAll();
}

void CGameInstance::StopSound(int _iChannel)
{
	m_pSound_Manager->StopSound(CSoundManager::SOUND_CHANNEL(_iChannel));
}

void CGameInstance::StopSoundALL()
{
	m_pSound_Manager->StopSoundALL();
}

void CGameInstance::Set_ChannelVolume(int _Channel, _float _fVolume)
{
	m_pSound_Manager->SetChannelVolume(CSoundManager::SOUND_CHANNEL(_Channel), _fVolume);
}

HRESULT CGameInstance::Set_SoundPos(_int _iChannel, _float3 _vPos)
{
	return m_pSound_Manager->Set_SoundPos(_iChannel, _vPos);
}

HRESULT CGameInstance::Add_Sound(const char* _fullpath, const TCHAR* _filename, const TCHAR* _path)
{
	return m_pSound_Manager->Add_Sound(_fullpath, _filename, _path);
}

HRESULT CGameInstance::Load_Sound(const char* _folderName)
{
	return m_pSound_Manager->Load_Sound(_folderName);
}

HRESULT CGameInstance::Set_Listener(_float3 _vPos, _float3 vForward)
{
	return m_pSound_Manager->Set_Listener(_vPos, vForward);
}
#pragma endregion

void CGameInstance::Release_Engine()
{	
	Safe_Release(m_pFrustum);
	Safe_Release(m_pPicking);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::Get_Instance()->Destroy_Instance();	
}

void CGameInstance::Free()
{	
	__super::Free();

}
