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
		Format_SelectBone();

	if (m_bShow_TriggerSetting)
		Format_Trigger();

	if (m_bShow_Chain)
		Format_AniChain();

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
	
	CBoneFlag::BONEFLAG_DESC desc;
	for (auto bone : *m_pCommander->Get_Bones())
	{
		desc.pSocketBoneMatrix = bone->Get_CombinedTransformationMatrix_Ptr();
		m_vecFlags.push_back(static_cast<CBoneFlag*>(m_pGameInstance->Add_CloneObject_ToLayer_Get(LEVEL_GAMEPLAY, TEXT("Layer_BoneFlag"), GameTag_BoneFlag, &desc)));
	}

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
	ImGui::SameLine();
	if (ImGui::Button("TriggerSetting"))
		m_bShow_TriggerSetting = !m_bShow_TriggerSetting;
	ImGui::SameLine();
	if (ImGui::Button("Chain"))
		m_bShow_Chain = !m_bShow_Chain;

	if (ImGui::Button("Save_Triggers"))
		Save_Triggers();
	ImGui::SameLine();
	if (ImGui::Button("Load_Triggers"))
		Load_Triggers();



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

	ImGui::NewLine();

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

void CLevel_GamePlay::Format_SelectBone()
{
	ImGui::Begin("Trigger_Effect");
	ImGui::SameLine();
	if (ImGui::Button("Show_Flags"))
	{
		m_bShow_BoneFlags = !m_bShow_BoneFlags;
	}
	if (m_bShow_BoneFlags)
	{

		for (auto BoneFLag : m_vecFlags)
		{
			m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, BoneFLag);
		}
	}
	else
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, m_vecFlags[m_iSelectedBone]);
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

	ImGui::End();
}

void CLevel_GamePlay::Save_ChainndeAnimation()
{
	ofstream SaveStream("../Bin/==Export==/CHAIN.dat", ios::binary | ios::trunc | ios::out);
	_int iSize(m_listAniChained.size());
	SaveStream.write((const char*)(&iSize), sizeof(iSize));
	
	for (auto& ChainPair : m_listAniChained)
	{
		SaveStream.write((const char*)(&ChainPair.first), sizeof(_int));
		SaveStream.write((const char*)(&ChainPair.second), sizeof(_int));
	}

	SaveStream.close();
}

void CLevel_GamePlay::Save_Triggers()
{
	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	_uint SizeofSave(m_mapAnimationSave.size());
	_uint SizeofTrigger;

	ofstream SaveStream("../Bin/==Export==/Triggers.dat", ios::out | ios::trunc | ios::binary);
	if (!SaveStream.is_open())
		MSG_BOX(TEXT("파일 스트림 오류!"));

	map<_uint, vector<pair<_double, _bool>>> Triggers;
	queue<BONENAME> listBoneNames;
	BONENAME BONENAME;
	for (auto& pair : m_mapAnimationSave)
	{
		for (auto EventTrigger : pair.second)
		{
			Triggers[pair.first].push_back({ EventTrigger, false });
		}
	}
	for (auto& pair : m_mapEffectTriggers)
	{
		for (auto EffectTrigger : pair.second)
		{
			Triggers[pair.first].push_back({ EffectTrigger.TriggerTime, true });
			strcpy_s(BONENAME.BoneName, MAX_PATH, EffectTrigger.BoneName);
			listBoneNames.push(BONENAME);
		}
	}// 뼈이름은 그냥 순서대로 저장

	for (auto& vecPair : Triggers)
	{
		sort(vecPair.second.begin(), vecPair.second.end());
	}

	//세이브
	SaveStream.write((const char*)&SizeofSave, sizeof(_uint));
	for (auto& pair : Triggers)
	{
		SizeofTrigger = pair.second.size();
		SaveStream.write((const char*)&SizeofTrigger, sizeof(_uint)); // 트리거 갯수 먼저 한번 저장하고

		for (auto& Trigger : pair.second) // <시간, 이펙트여부>
		{
			SaveStream.write((const char*)&pair.first, sizeof(_uint));		//몇번 애니
			SaveStream.write((const char*)&Trigger.first, sizeof(_double));	//몇초에 트리거
			SaveStream.write((const char*)&Trigger.second, sizeof(_bool)); //이펙트여부
			if (Trigger.second) // 뼈이름도저장
			{
				SaveStream.write((const char*)&listBoneNames.front(), sizeof(char) * MAX_PATH);
				listBoneNames.pop();
			}
		}
	}

#pragma region 예전코드

	//SaveStream.write((const char*)&SizeofSave, sizeof(_uint));
	//for (auto& pair : m_mapAnimationSave)
	//{
	//	SizeofTrigger = pair.second.size();
	//	SaveStream.write((const char*)&SizeofTrigger, sizeof(_uint));

	//	for (auto& Trigger : pair.second)
	//	{
	//		SaveStream.write((const char*)&pair.first, sizeof(_uint));
	//		SaveStream.write((const char*)&Trigger, sizeof(_double));
	//	}
	//}
#pragma endregion
	SaveStream.close();
}

void CLevel_GamePlay::Load_Triggers()
{
	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	_uint SizeofLoad(m_mapAnimationSave.size());
	_uint SizeofTrigger;
	_uint iAnimationNum;
	_double dTriggerPos;

	_bool Temp;
	_char BoneName[260];

	Clear_SaveMap();
	ifstream Loadstream("../Bin/==Export==/Triggers.dat", ios::binary | ios::in);
	if (!Loadstream.is_open())
		MSG_BOX(TEXT("파일 스트림 오류!"));

	Loadstream.read((char*)&SizeofLoad, sizeof(_uint));
	for (size_t i = 0; i < SizeofLoad; i++)
	{
		Loadstream.read((char*)&SizeofTrigger, sizeof(_uint));
		for (size_t i = 0; i < SizeofTrigger; i++)
		{
			Loadstream.read((char*)&iAnimationNum, sizeof(_uint));
			Loadstream.read((char*)&dTriggerPos, sizeof(_double));
			Loadstream.read((char*)&Temp, sizeof(_bool));
			if (Temp)
			{
				Loadstream.read((char*)&BoneName, sizeof(char) * MAX_PATH);
				EFFECTTRIGGER EffectTriggerData;
				//pair<_double, char[MAX_PATH]> Trigger;
				EffectTriggerData.TriggerTime = dTriggerPos;
				strcpy_s(EffectTriggerData.BoneName, MAX_PATH, BoneName);
				m_mapEffectTriggers[iAnimationNum].push_back(EffectTriggerData);
				//strcpy_s(Trigger.second, MAX_PATH, (*m_pCommander->Get_Bones())[m_iSelectedBone]->Get_Name());
			}
			else
			{
				m_mapAnimationSave[iAnimationNum].push_back(dTriggerPos);
			}
		}
	}
	sort(m_mapAnimationSave[iCurrentAnimationIndex].begin(), m_mapAnimationSave[iCurrentAnimationIndex].end());

	Loadstream.close();

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
	if (ImGui::Button("Select_Bone"))
		m_bEffectTrigger = !m_bEffectTrigger;
	ImGui::SameLine();
	if (ImGui::Button("Flag_Trigger"))
	{
		EFFECTTRIGGER Trigger;
		Trigger.TriggerTime = m_pCommander->Get_CurrentTrackPosition();
		strcpy_s(Trigger.BoneName, MAX_PATH, (*m_pCommander->Get_Bones())[m_iSelectedBone]->Get_Name());
		m_mapEffectTriggers[iCurrentAnimationIndex].push_back(Trigger);
		sort(m_mapAnimationSave[iCurrentAnimationIndex].begin(), m_mapAnimationSave[iCurrentAnimationIndex].end());
	}
	ImGui::BeginTable("list", 2);

	for (auto& pair : m_mapEffectTriggers)
	{
		for (auto& Trigger : pair.second)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%u, %f, %s", pair.first, Trigger.TriggerTime, Trigger.BoneName);
		}
		ImGui::TableNextRow();
	}

	ImGui::EndTable();


}

void CLevel_GamePlay::Format_AniChain()
{
	ImGui::Begin("Animation_Chain");
	if (ImGui::Button("Save"))
	{
		Save_ChainndeAnimation();
	}
	ImGui::InputInt2("a -> b", &m_stlChain.first);

	if (ImGui::Button("Chain!"))
	{
		m_listAniChained.push_back(m_stlChain);
	}
	ImGui::NewLine();

	ImGui::BeginGroup();
	ImGui::BeginChild("list");
	char buffer[16];
	int i(0);
	ImGui::BeginTable("list", 2);
	for (auto& pair : m_listAniChained)
	{
		ImGui::TableNextColumn();
		sprintf_s(buffer, "%d -> %d##%d", pair.first, pair.second, i);
		if (ImGui::Button(buffer))
		{
			pair.first = m_stlChain.first;
			pair.second = m_stlChain.second;
		}
		++i;
	}
	ImGui::EndTable();
	ImGui::EndChild();
	ImGui::EndGroup();


	ImGui::End();
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
