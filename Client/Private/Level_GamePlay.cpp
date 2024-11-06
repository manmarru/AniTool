#pragma once
#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "FreeCamera.h"
#include "GameInstance.h"
#include "Commander.h"

#include "EditObj.h"
#include "BoneFlag.h"

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

	if (FAILED(Ready_EditObj()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	//m_pGameInstance->PlayBGM(L"Default.ogg", 0.5f, true);


	// ImGui 초기화
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

	if (m_bEffectTrigger)
		Format_EffectTrigger();

	if (m_bShow_TriggerSetting)
		Format_Trigger();
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
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
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

	Desc.fSensor = 0.7f;
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

HRESULT CLevel_GamePlay::Ready_EditObj()
{
	CEditObj::EDITOBJ_DESC Desc;
	Desc.isShadowObj = true;
	Desc.ModelTag = ModelTag_Syar;
	Desc.pAnimationSpeed = m_pAnimationSpeed;
	m_pCommander->Register(m_pGameInstance->Add_CloneObject_ToLayer_Get(LEVEL_GAMEPLAY, TEXT("Layer_EditObj"), GameTag_EditObj, &Desc));
	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), m_pAnimationSpeed)))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Format_ImGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Hello_World");

	if (ImGui::Button("open_demo"))
		m_bDemoStart = !m_bDemoStart;
	if (ImGui::Button("EffectTrigger"))
		m_bEffectTrigger = !m_bEffectTrigger;
	ImGui::SameLine();
	if (ImGui::Button("TriggerSetting"))
		m_bShow_TriggerSetting = !m_bShow_TriggerSetting;

	ImGui::NewLine();

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


	//애니메이션 진행 퍼센트를 저장하는 버튼 -> (애니번호, 퍼센트) 순서쌍으로 리스트에 넣고 애니별로 맵에 저장
	//최종 저장 버튼 누르면 이거 바이너리화할거임, 그러고 이거 불러와보는거까지 해보자.
	//불러올때는 각 애니메이션의 맴버벡터에 퍼센트를 받아서 저장하고, 마지막으로 모든 애니에 애니 마지막 길이를 넣을거임.
	
	ImGui::Text("Animation : %d / %d", m_pCommander->Get_CurrentAnimationIndex(), m_pCommander->Get_AnimationNum() - 1);

	int aniNum(0);
	if (ImGui::InputInt("set_ani", &aniNum, 0, 0) && ImGui::IsItemDeactivatedAfterEdit())
	{
		m_pCommander->Set_Animation(min(aniNum, m_pCommander->Get_AnimationNum() - 1));
	}

	if (ImGui::Button("-", ImVec2(50.f, 50.f)))
	{
		m_pCommander->Set_Animation(m_pCommander->Get_CurrentAnimationIndex() - 1);
	}
	ImGui::SameLine();
	if (ImGui::Button("+", ImVec2(50.f, 50.f)))
	{
		m_pCommander->Set_Animation(m_pCommander->Get_CurrentAnimationIndex() + 1);
	}

	ImGui::End();
}

void CLevel_GamePlay::Format_Trigger()
{
	ImGui::Begin("Triggers");
	ImGui::BeginTabBar("asdf");
	if (ImGui::BeginTabItem("Event"))
	{
		TriggerSetting_Event();

		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Effect"))
	{
		TriggerSetting_Effect();


		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();


	ImGui::End();
}

void CLevel_GamePlay::Format_EffectTrigger()
{
	ImGui::Begin("Trigger_Effect");
	//뼈 위치에 플래그들 보여주기 (add_renderobj)
	//커멘더에서 뼈 받아와서 뼈목록 보여주기
	if (ImGui::Button("Show_Flags"))
	{
		m_bShow_BoneFlags = !m_bShow_BoneFlags;
		switch (m_bShow_BoneFlags)
		{
		case true:
			// 플래그 생성
		{
			CBoneFlag::BONEFLAG_DESC desc;
			for (auto bone : *m_pCommander->Get_Bones())
			{
				desc.pSocketBoneMatrix = bone->Get_CombinedTransformationMatrix_Ptr();
				m_vecFlags.push_back(static_cast<CBoneFlag*>(m_pGameInstance->Add_CloneObject_ToLayer_Get(LEVEL_GAMEPLAY, TEXT("Layer_BoneFlag"), GameTag_BoneFlag, &desc)));
			}
		}
			break;
		case false:
			// 플래그 삭제
			break;
		}
	}


	ImGui::BeginTable("bones", 2);
	
	int i(0);
	for (auto bone : *m_pCommander->Get_Bones())
	{
		if (i == m_iSelectedBone)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 1.f, 1.f));

		if (ImGui::Button(bone->Get_Name()))
		{
			if (i != m_iSelectedBone)
			{
				m_vecFlags[m_iSelectedBone]->Set_TextureNum(0);
				m_iSelectedBone = i;
				m_vecFlags[i]->Set_TextureNum(1);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 1.f, 1.f));
			}
		}

		if (i == m_iSelectedBone)
			ImGui::PopStyleColor();

		ImGui::TableNextColumn();
		++i;
	}
	ImGui::EndTable();

	//선택한 뼈의 플래그 색상 변경






	ImGui::End();
}

void CLevel_GamePlay::Clear_SaveMap()
{
	for (auto& pair : m_mapAnimationSave)
	{
		pair.second.clear();
	}
	m_mapAnimationSave.clear();
}

void CLevel_GamePlay::TriggerSetting_Event()
{
	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	if (ImGui::Button("Flag_Trigger"))
	{
		m_mapAnimationSave[iCurrentAnimationIndex].push_back(*m_pCommander->Get_CurrentTrackPosition_ptr());
		sort(m_mapAnimationSave[iCurrentAnimationIndex].begin(), m_mapAnimationSave[iCurrentAnimationIndex].end());
	}

#pragma region 세이브로드

	_uint SizeofSave(m_mapAnimationSave.size());
	_uint SizeofTrigger;
	if (ImGui::Button("Save_Trigger"))
	{
		ofstream SaveStream("../Bin/Resources/Export.dat", ios::trunc || ios::binary);
		if (!SaveStream.is_open())
			MSG_BOX(TEXT("파일 스트림 오류!"));

		//세이브
		SaveStream.write((const char*)&SizeofSave, sizeof(_uint));
		for (auto& pair : m_mapAnimationSave)
		{
			SizeofTrigger = pair.second.size();
			SaveStream.write((const char*)&SizeofTrigger, sizeof(_uint));

			for (auto& Trigger : pair.second)
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
			MSG_BOX(TEXT("파일 스트림 오류!"));
		//로드
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
	for (auto& pair : m_mapAnimationSave)
	{
		for (auto& Trigger : pair.second)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%u : %f", pair.first, Trigger);
		}
		ImGui::TableNextRow();
	}
	ImGui::EndTable();
}

void CLevel_GamePlay::TriggerSetting_Effect()
{
	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	if (ImGui::Button("Flag_Trigger"))
	{
		pair<_double, char[MAX_PATH]> Trigger;
		Trigger.first = m_pCommander->Get_CurrentTrackPosition();
		m_mapEffectTriggers[iCurrentAnimationIndex].push_back(Trigger);
		sort(m_mapAnimationSave[iCurrentAnimationIndex].begin(), m_mapAnimationSave[iCurrentAnimationIndex].end());
	}
	ImGui::BeginTable("list", 2);

	for (auto& pair : m_mapEffectTriggers)
	{
		for (auto& Trigger : pair.second)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%u, %f, %s", pair.first, Trigger.first, Trigger.second);
		}
		ImGui::TableNextRow();
	}

	ImGui::EndTable();


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
