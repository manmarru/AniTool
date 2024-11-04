#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "FreeCamera.h"
#include "GameInstance.h"
#include "Commander.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	m_pAnimationSpeed = new _float(1.f);

	m_pCommander = new CCommander();

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Paticle()))
		return E_FAIL;

	//m_pGameInstance->PlayBGM(L"Default.ogg", 0.5f, true);


	// ImGui �ʱ�ȭ
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	Format_ImGUI();

	if (m_bDemoStart)
		ImGui::ShowDemoWindow(&m_bDemoStart);
}

HRESULT CLevel_GamePlay::Render()
{
	if (!m_bGuiStart)
	{
		m_bGuiStart = true;
		return S_OK;
	}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	/* �����÷��� ������ �ʿ��� ������ �غ��Ѵ�. */
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))	
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	CFreeCamera::CAMERA_FREE_DESC		Desc{};

	Desc.fSensor = 0.2f;
	Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Paticle()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), m_pAnimationSpeed)))
		return E_FAIL;

	m_pCommander->Register(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player")));

	return S_OK;
}

void CLevel_GamePlay::Format_ImGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Hello_World");
	if (ImGui::Button("open_demo"))
		m_bDemoStart = true;

	if (ImGui::Button("Speed->1"))
	{
		*m_pAnimationSpeed = 1.f;
	}
	ImGui::SameLine();
	if (ImGui::Button("Speed->0"))
	{
		*m_pAnimationSpeed = 0.f;
	}

	ImGui::SliderFloat("AnimationSpeed", m_pAnimationSpeed, 0, 2.f);
	_float CurrentTrackPosition = (_float)*m_pCommander->Get_CurrentTrackPosition_ptr();
	
	ImGui::SliderFloat("Duration", &CurrentTrackPosition, 0, m_pCommander->Get_Duration());

	m_pCommander->Set_CurrentTrackPosition((_double)CurrentTrackPosition);

	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	if (ImGui::Button("Flag_Trigger"))
	{
		m_mapAnimationSave[iCurrentAnimationIndex].push_back(*m_pCommander->Get_CurrentTrackPosition_ptr());
		sort(m_mapAnimationSave[iCurrentAnimationIndex].begin(), m_mapAnimationSave[iCurrentAnimationIndex].end());
	}
	
#pragma region ���̺�ε�

	_uint SizeofSave(m_mapAnimationSave.size());
	_uint SizeofTrigger;
	if (ImGui::Button("Save_Trigger"))
	{
		ofstream SaveStream("../Bin/Resources/Export.dat", ios::trunc || ios::binary);
		if (!SaveStream.is_open())
			MSG_BOX(TEXT("���� ��Ʈ�� ����!"));

		//���̺�
		SaveStream.write((const char*)&SizeofSave, sizeof(_uint));
		for (auto pair : m_mapAnimationSave)
		{
			SizeofTrigger = pair.second.size();
			SaveStream.write((const char*)&SizeofTrigger, sizeof(_uint));

			for (auto Trigger : pair.second)
			{
				SaveStream.write((const char*)&pair.first, sizeof(_uint));
				SaveStream.write((const char*)&Trigger, sizeof(_double));
			}
		}
		SaveStream.close();
	}
	ImGui::SameLine();
	_uint iAnimationNum;
	_double dTriggerPos;
	if (ImGui::Button("Load_Trigger"))
	{
		Clear_SaveMap();
		ifstream Loadstream("../Bin/Resources/Export.dat", ios::binary);
		if (!Loadstream.is_open())
			MSG_BOX(TEXT("���� ��Ʈ�� ����!"));
		//�ε�
		Loadstream.read((char*)&SizeofSave, sizeof(_uint));
		for (size_t i = 0; i < SizeofSave; i++)
		{
			Loadstream.read((char*)&SizeofTrigger, sizeof(_uint));
			for (size_t i = 0; i < SizeofTrigger; i++)
			{
				Loadstream.read((char*)&iAnimationNum, sizeof(_uint));
				Loadstream.read((char*)&dTriggerPos, sizeof(_double));
				m_mapAnimationSave[iAnimationNum].push_back(dTriggerPos);
			}
		}
		sort(m_mapAnimationSave[iCurrentAnimationIndex].begin(), m_mapAnimationSave[iCurrentAnimationIndex].end());

		Loadstream.close();
	}

#pragma endregion

	ImGui::BeginTable("Triggers", 2);
	for (auto pair : m_mapAnimationSave)
	{
		for (auto Trigger : pair.second)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%d : %d", (int)pair.first, (int)Trigger);
		}
		ImGui::TableNextRow();
	}
	ImGui::EndTable();

	//�ִϸ��̼� ���� �ۼ�Ʈ�� �����ϴ� ��ư -> (�ִϹ�ȣ, �ۼ�Ʈ) ���������� ����Ʈ�� �ְ� �ִϺ��� �ʿ� ����
	//���� ���� ��ư ������ �̰� ���̳ʸ�ȭ�Ұ���, �׷��� �̰� �ҷ��ͺ��°ű��� �غ���.
	//�ҷ��ö��� �� �ִϸ��̼��� �ɹ����Ϳ� �ۼ�Ʈ�� �޾Ƽ� �����ϰ�, ���������� ��� �ִϿ� �ִ� ������ ���̸� ��������.
	
	ImGui::Text("CurrentAnim : %d", m_pCommander->Get_CurrentAnimationIndex());

	ImGui::End();
}

void CLevel_GamePlay::Clear_SaveMap()
{
	for (auto pair : m_mapAnimationSave)
	{
		pair.second.clear();
	}
	m_mapAnimationSave.clear();
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Safe_Delete(m_pCommander);
	Safe_Delete(m_pAnimationSpeed);
}
