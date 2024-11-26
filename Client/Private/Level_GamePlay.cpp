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

	//Imgui_Dialogue();

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
	//Desc.ModelTag = ModelTag_Syar;
	Desc.ModelTag = ModelTag_PlayerBody;
	//Desc.ModelTag = ModelTag_Duaca;
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

	if (ImGui::Button("Chain"))
		m_bShow_Chain = !m_bShow_Chain;
	ImGui::SameLine();
	if (ImGui::Button("Trigger##Setting"))
		m_bShow_TriggerSetting = !m_bShow_TriggerSetting;

	if (ImGui::Button("Speed->1"))
	{
		*m_pAnimationSpeed = 1.f;
	}
	ImGui::SameLine();
	if (ImGui::Button("Speed->0"))
	{
		*m_pAnimationSpeed = 0.f;
	}
	ImGui::SameLine();
	if (ImGui::Button("RePlay"))
	{
		m_pCommander->Reset_Animation();
	}

	ImGui::SliderFloat("AnimationSpeed", m_pAnimationSpeed, 0, 2.f);
	_float CurrentTrackPosition = (_float)*m_pCommander->Get_CurrentTrackPosition_ptr();
	
	ImGui::SliderFloat("Duration", &CurrentTrackPosition, 0, (float)m_pCommander->Get_Duration());

	m_pCommander->Set_CurrentTrackPosition((_double)CurrentTrackPosition);
	


	ImGui::Text("Animation : %d / %d", m_pCommander->Get_CurrentAnimationIndex(), m_pCommander->Get_AnimationNum() - 1);

	int aniNum(0);
	if (ImGui::InputInt("set_ani", &aniNum, 0, 0) && ImGui::IsItemDeactivatedAfterEdit())
	{
		m_pCommander->Set_Animation((_uint)min(aniNum, m_pCommander->Get_AnimationNum() - 1));
	}

	if (ImGui::Button("-", ImVec2(50.f, 50.f)))
	{
		m_pCommander->Set_Animation((_uint)max(0, (_int)m_pCommander->Get_CurrentAnimationIndex() - 1));
	}
	ImGui::SameLine();
	if (ImGui::Button("+", ImVec2(50.f, 50.f)))
	{
		m_pCommander->Set_Animation(min(m_pCommander->Get_AnimationNum() - 1, m_pCommander->Get_CurrentAnimationIndex() + 1));
	}

	ImGui::End();
}

void CLevel_GamePlay::Format_Trigger()
{
	ImGui::Begin("Triggers");
	ImGui::BeginTabBar("asdf");

	ImGui::NewLine();


	if (ImGui::BeginTabItem("Event##Trigger"))
	{
		TriggerSetting_Event();
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Effect##Trigger"))
	{
		TriggerSetting_Effect();
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Speed##Trigger"))
	{
		TriggerSetting_Speed();
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Data##Trigger"))
	{
		if (ImGui::Button("Save##Triggers", ImVec2(50.f, 50.f)))
			Save_Triggers();
		ImGui::NewLine();
		if (ImGui::Button("Load##Triggers", ImVec2(50.f, 50.f)))
			Load_Triggers();
		ImGui::NewLine();
		if (ImGui::Button("Clear##Trigger", ImVec2(50.f, 50.f)))
			Clear_SaveMap();
		ImGui::NewLine();
		if (ImGui::Button("Apply##Trigger", ImVec2(50.f, 50.f)))
			Passing_Trigger();

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

void CLevel_GamePlay::Format_AniChain()
{
	ImGui::Begin("Animation_Chain");
	if (ImGui::Button("Save"))
	{
		Save_ChainnedAnimation();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		Load_ChainnedAnimation();
	}
	ImGui::SameLine();
	if (ImGui::Button("Apply_Save##Chain"))
	{
		ifstream Loadstream("../Bin/AAExportAA/CHAIN.dat", ios::binary | ios::in);
		m_pCommander->Setup_Chains(&Loadstream);
		Loadstream.close();
	}
	ImGui::InputInt2("a -> b", (int*) & m_tChain.Before);
	ImGui::InputText("Tag##Chain", m_tChain.ChainTag, sizeof(CHAIN::ChainTag));

	ImGui::Checkbox("Lerp", &m_tChain.bLerp);

	if (ImGui::Button("Chain!"))
	{
		m_listAniChained.push_back(m_tChain);
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear##Chain"))
	{
		m_listAniChained.clear();
	}


	if (ImGui::Button("Preview"))
	{
		m_pCommander->Set_Animation(m_tChain.ChainTag);
	}

	ImGui::NewLine();



	ImGui::BeginGroup();
	ImGui::BeginChild("list");
	char buffer[128];
	int i(0);
	ImGui::BeginTable("list", 2);
	//for (auto& pair : m_listAniChained)
	for(auto pair = m_listAniChained.begin(); pair != m_listAniChained.end(); ++pair)
	{
		ImGui::TableNextColumn();
		sprintf_s(buffer, "%s : %u -> %u [%s]##%d", (*pair).ChainTag, (*pair).Before, (*pair).After, (*pair).bLerp ? "true" : "false", i);
		if (ImGui::Button(buffer))
		{
			m_SelectedChain = pair;
			m_bShow_ChainPopup = true;
		}
		++i;
	}
	ImGui::EndTable();
	ImGui::EndChild();
	ImGui::EndGroup();
	if (m_bShow_ChainPopup) 
	{
		ImGui::OpenPopup("Fix Chain?");
	}
	if (ImGui::BeginPopupModal("Fix Chain?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::Text("Before : % u -> % u\nFixed : % u -> % u", (*m_SelectedChain).Before, (*m_SelectedChain).After, m_tChain.Before, m_tChain.After);

		if (ImGui::Button("OK##Chain"))
		{
			(*m_SelectedChain).Before = m_tChain.Before;
			(*m_SelectedChain).After = m_tChain.After;
			m_bShow_ChainPopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##Chain"))
		{
			m_bShow_ChainPopup = false;
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Remove##Chain"))
		{
			m_listAniChained.erase(m_SelectedChain);
			m_bShow_ChainPopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}


	ImGui::End();
}

void CLevel_GamePlay::Imgui_Dialogue()
{
	// 파일 대화 상자 초기화
	ImGuiFileDialog::Instance()->OpenDialog(
		"OpenFileDialog",           // vKey
		"Select a File",           // vTitle
		".bmp,.dat",            // vFilters
		IGFD::FileDialogConfig()   // vConfig (기본 설정)
	);

	// 대화 상자가 열릴 때
	if (ImGuiFileDialog::Instance()->Display("OpenFileDialog"))
	{
		// 사용자가 파일을 선택한 경우
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// 선택된 파일 경로 가져오기
			stlFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
		}

		// 대화 상자 닫기
		ImGuiFileDialog::Instance()->Close();
	}
}

void CLevel_GamePlay::Clear_SaveMap()
{
	for (auto& pair : m_mapEventTriggers)
	{
		pair.second.clear();
	}
	m_mapEventTriggers.clear();

	for (auto& pair : m_mapEffectTriggers)
	{
		pair.second.clear();
	}
	m_mapEffectTriggers.clear();
}

void CLevel_GamePlay::Save_Triggers()
{
	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	_uint SizeofSave;
	_uint SizeofTrigger;

	ofstream SaveStream("../Bin/AAExportAA/Triggers.dat", ios::out | ios::trunc | ios::binary);
	if (!SaveStream.is_open())
		MSG_BOX(TEXT("파일 스트림 오류!"));

	map<_uint, vector<DEFAULTTRIGGER>> Triggers;
	queue<BONENAME> listBoneNames;
	BONENAME tBONENAME;
	// 두 종류의 트리거(이벤트, 이펙트)를 같은 컨테이너에 넣고 시간순 정렬해서 세이브
	for (auto& pair : m_mapEventTriggers)
	{
		for (auto EventTrigger : pair.second)
		{
			Triggers[pair.first].push_back({ EventTrigger, false });
		}
	}
	for (auto& pair : m_mapEffectTriggers)
	{
		for (auto& EffectTrigger : pair.second)
		{
			Triggers[pair.first].push_back({ EffectTrigger.TriggerTime, true });
			strcpy_s(tBONENAME.BoneName, MAX_PATH, EffectTrigger.BoneName);
			listBoneNames.push(tBONENAME);
		}
	}// 뼈이름은 그냥 순서대로 저장
	//sort
	for (auto& vecPair : Triggers)
	{
		sort(vecPair.second.begin(), vecPair.second.end(), [](DEFAULTTRIGGER Temp, DEFAULTTRIGGER Src) {return Temp.TriggerTime < Src.TriggerTime; });
	}

	//세이브
	SizeofSave = ((_uint)Triggers.size());
	SaveStream.write((const char*)&SizeofSave, sizeof(_uint));
	for (auto& pair : Triggers)
	{
		SizeofTrigger = (_uint)pair.second.size();
		SaveStream.write((const char*)&pair.first, sizeof(_uint));		//몇번 애니읹
		SaveStream.write((const char*)&SizeofTrigger, sizeof(_uint)); // 트리거 갯수
		for (auto& Trigger : pair.second)
		{
			SaveStream.write((const char*)&Trigger, sizeof(DEFAULTTRIGGER));
			if (Trigger.isEffectTrigger) // 뼈이름도저장
			{
				SaveStream.write((const char*)&listBoneNames.front(), sizeof(char) * MAX_PATH);
				listBoneNames.pop();
			}
		}
	}

	//스피드트리거 세이브
	SizeofSave = (_uint)m_mapSpeedTriggers.size();
	SaveStream.write((const char*)&SizeofSave, sizeof(_uint));
	for (auto& pair : m_mapSpeedTriggers)
	{
		SizeofTrigger = (_uint)pair.second.size();
		SaveStream.write((const char*)&pair.first, sizeof(_uint)); // 몇번 애니
		SaveStream.write((const char*)&SizeofTrigger, sizeof(_uint)); // 트리거갯수
		for (auto& Trigger : pair.second)
		{
			SaveStream.write((const char*)&Trigger, sizeof(SPEEDTRIGGER));
		}
	}

	SaveStream.close();
}

void CLevel_GamePlay::Load_Triggers()
{
	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	_uint SizeofLoad;
	_uint SizeofTrigger;
	_uint iAnimationNum;
	DEFAULTTRIGGER DefaultTrigger;

	_char BoneName[260];

	Clear_SaveMap();
	ifstream Loadstream("../Bin/AAExportAA/Triggers.dat", ios::binary | ios::in);
	if (!Loadstream.is_open())
		MSG_BOX(TEXT("파일 스트림 오류!"));

	Loadstream.read((char*)&SizeofLoad, sizeof(_uint));
	for (size_t i = 0; i < SizeofLoad; i++)
	{
		Loadstream.read((char*)&iAnimationNum, sizeof(_uint));
		Loadstream.read((char*)&SizeofTrigger, sizeof(_uint));
		for (size_t i = 0; i < SizeofTrigger; i++)
		{
			Loadstream.read((char*)&DefaultTrigger, sizeof(DEFAULTTRIGGER));
			if (DefaultTrigger.isEffectTrigger)
			{
				Loadstream.read((char*)&BoneName, sizeof(char) * MAX_PATH);
				EFFECTTRIGGER EffectTriggerData;
				EffectTriggerData.TriggerTime = DefaultTrigger.TriggerTime;
				strcpy_s(EffectTriggerData.BoneName, MAX_PATH, BoneName);
				m_mapEffectTriggers[iAnimationNum].push_back(EffectTriggerData);
			}
			else
			{
				m_mapEventTriggers[iAnimationNum].push_back(DefaultTrigger.TriggerTime);
			}
		}
	}



	//스피드트리거 불러오기
	//우선 비우고
	for (auto pairtrigger : m_mapSpeedTriggers)
	{
		pairtrigger.second.clear();
	}
	m_mapSpeedTriggers.clear();
	//트리거가 있는 애니 갯수
	//애니에 들어있는 트리거 갯수
	//트리거 내용
	Loadstream.read((char*)&SizeofLoad, sizeof(_uint));
	SPEEDTRIGGER LoadSpeedTrigger;
	for (_uint i = 0; i < SizeofLoad; i++)
	{
		Loadstream.read((char*)&iAnimationNum, sizeof(_uint));
		Loadstream.read((char*)&SizeofTrigger, sizeof(_uint));
		for (_uint j = 0; j < SizeofTrigger; j++)
		{
			Loadstream.read((char*)&LoadSpeedTrigger, sizeof(SPEEDTRIGGER));
			m_mapSpeedTriggers[iAnimationNum].push_back(LoadSpeedTrigger);
		}
	}



	for (auto& vecTrigger : m_mapEventTriggers)
	{
		sort(vecTrigger.second.begin(), vecTrigger.second.end());
	}
	for (auto& vecTrigger : m_mapEffectTriggers)
	{
		sort(vecTrigger.second.begin(), vecTrigger.second.end(), [](EFFECTTRIGGER Temp, EFFECTTRIGGER Src) {return Temp.TriggerTime < Src.TriggerTime; });
	}
	for (auto& vecTrigger : m_mapSpeedTriggers)
	{
		sort(vecTrigger.second.begin(), vecTrigger.second.end(), [](SPEEDTRIGGER Temp, SPEEDTRIGGER Src) {return Temp.TriggerTime < Src.TriggerTime; });
	}


	Loadstream.close();
}

void CLevel_GamePlay::Save_ChainnedAnimation()
{
	ofstream SaveStream("../Bin/AAExportAA/CHAIN.dat", ios::binary | ios::trunc | ios::out);
	_int iSize((_int)m_listAniChained.size());
	SaveStream.write((const char*)(&iSize), sizeof(iSize));
	
	for (auto& ChainPair : m_listAniChained)
	{
		SaveStream.write((const char*)(&ChainPair), sizeof(CHAIN));
	}

	SaveStream.close();
}

void CLevel_GamePlay::Load_ChainnedAnimation()
{
	ifstream LoadStream("../Bin/AAExportAA/CHAIN.dat", ios::binary | ios::in);

	m_listAniChained.clear(); // 로드전에 비우기

	_int iSize(0);
	CHAIN tLoadChain;
	LoadStream.read((char*)&iSize, sizeof(_int));
	for (size_t i = 0; i < iSize; i++)
	{
		LoadStream.read((char*)&tLoadChain, sizeof(CHAIN));
		m_listAniChained.push_back(tLoadChain);
	}
	LoadStream.close();
	
}

void CLevel_GamePlay::Passing_Trigger()
{
	//스피드트리거는 큐로 만들어주고 전달해야된다;;
	map<_uint, queue<SPEEDTRIGGER>> TempSpeedTrigger;
	for (auto pair : m_mapSpeedTriggers)
	{
		for (auto Trigger : pair.second)
			TempSpeedTrigger[pair.first].push(Trigger);
	}
	m_pCommander->Register_Trigger(&m_mapEventTriggers, &m_mapEffectTriggers, &TempSpeedTrigger);
}

void CLevel_GamePlay::TriggerSetting_Event()
{
	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	char Buttonbuffer[128];
	ImGui::SetNextItemWidth(90.f);
	ImGui::InputInt("AniIndex##EventTrigger", &m_iFixEventTirgger_AniIndex, 0, 0);
	ImGui::SameLine(); 
	ImGui::SetNextItemWidth(90.f);
	ImGui::InputDouble("Position##EventTrigger", &m_dFixEventTrigger_TriggerPos);
	ImGui::SameLine();
	if (ImGui::Button("Flag##EventTrigger", ImVec2{50.f, 50.f}))
	{
		m_mapEventTriggers[iCurrentAnimationIndex].push_back(*m_pCommander->Get_CurrentTrackPosition_ptr());
		sort(m_mapEventTriggers[iCurrentAnimationIndex].begin(), m_mapEventTriggers[iCurrentAnimationIndex].end());
	}

	ImGui::BeginGroup();
	ImGui::BeginChild("list##EventTrigger");
	int i(0);
	ImGui::BeginTable("Triggers", 2);
	//for (auto& pair : m_mapEventTriggers)
	for(auto iterPair = m_mapEventTriggers.begin(); iterPair != m_mapEventTriggers.end(); ++iterPair)
	{
		//for (auto& Trigger : (*iterPair).second)
		for(auto& iterTrigger = (*iterPair).second.begin(); iterTrigger != (*iterPair).second.end();)
		{
			ImGui::TableNextColumn();
			//ImGui::Text("%u : %f", pair.first, Trigger);
			sprintf_s(Buttonbuffer, "%u, : %f##EventTrigger%d", (*iterPair).first, (*iterTrigger), i);
			if (ImGui::Button(Buttonbuffer))
			{
				m_bShow_EventTriggerPopup = true;
				m_SelectedEventTrigger = iterTrigger;
				//iterTrigger = (*iterPair).second.erase(iterTrigger);
				//m_mapEventTriggers[m_iFixEventTirgger_AniIndex].push_back(m_dFixEventTrigger_TriggerPos);
				//sort(m_mapEventTriggers[m_iFixEventTirgger_AniIndex].begin(), m_mapEventTriggers[m_iFixEventTirgger_AniIndex].end(), [](_double Temp, _double Src) {return Temp < Src; });
			}
			else
				++iterTrigger;
			++i;
		}
		ImGui::TableNextRow();
	}
	ImGui::EndTable();
	ImGui::EndChild();
	ImGui::EndGroup();


	if (m_bShow_EventTriggerPopup)
	{
		ImGui::OpenPopup("Fix_Trigger?##EventTrigger");
		m_bShow_EventTriggerPopup = false;
	}
	if (ImGui::BeginPopupModal("Fix_Trigger?##EventTrigger", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Before : %f\nFixed : %f", (*m_SelectedEventTrigger), m_pCommander->Get_CurrentTrackPosition());
		if (ImGui::Button("OK##EventPopup"))
		{
			(*m_SelectedEventTrigger) = m_pCommander->Get_CurrentTrackPosition();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No##EventTPopup"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove##EventTrigger"))
		{
			m_mapEventTriggers[iCurrentAnimationIndex].erase(m_SelectedEventTrigger);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
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
		sort(m_mapEventTriggers[iCurrentAnimationIndex].begin(), m_mapEventTriggers[iCurrentAnimationIndex].end());
	}
	ImGui::BeginGroup();
	ImGui::BeginChild("list##EffectTrigger");
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
	ImGui::EndChild();
	ImGui::EndGroup();

}

void CLevel_GamePlay::TriggerSetting_Speed()
{
	_uint iCurrentAnimationIndex = m_pCommander->Get_CurrentAnimationIndex();
	SPEEDTRIGGER InputTrigger;

	ImGui::InputFloat("speed##Animation", &m_fFlag_AnimationSpeed, 0.f, 0.f);
	ImGui::SameLine();
	if (ImGui::Button("Flag##SpeedTriggers", ImVec2(50.f, 50.f)))
	{
		InputTrigger.TriggerTime = m_pCommander->Get_CurrentTrackPosition();
		InputTrigger.AnimationSpeed = m_fFlag_AnimationSpeed;
		m_mapSpeedTriggers[iCurrentAnimationIndex].push_back(InputTrigger);
		sort(m_mapSpeedTriggers[iCurrentAnimationIndex].begin(), m_mapSpeedTriggers[iCurrentAnimationIndex].end(), [](SPEEDTRIGGER tmp, SPEEDTRIGGER Src) {return tmp.TriggerTime < Src.TriggerTime; });
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear##SpeedTriggers", ImVec2(50.f, 50.f)))
	{
		for (auto& pair : m_mapSpeedTriggers)
		{	
			pair.second.clear();
		}
		m_mapSpeedTriggers.clear();
	}


	ImGui::NewLine();

	ImGui::BeginTable("list##SpeedTrigger", 2);

	char Buttonbuffer[256];
	int i(0);

	for (auto& pair : m_mapSpeedTriggers)
	{
		//for (auto& Trigger : pair.second)
		for(auto trigger = pair.second.begin(); trigger != pair.second.end(); ++trigger)
		{
			ImGui::TableNextColumn();
			sprintf_s(Buttonbuffer, "%u, %f, %f##SpeedTrigger%d", pair.first, (*trigger).TriggerTime, (*trigger).AnimationSpeed, i);
			if (ImGui::Button(Buttonbuffer))
			{
				m_SelectedSpeedTrigger = trigger;
				m_bShow_SpeedPopup = true;
			}
			++i;
		}
		ImGui::TableNextRow();
	}


	ImGui::EndTable();


	if (m_bShow_SpeedPopup)
	{
		ImGui::OpenPopup("Fix_Trigger?##SpeedTrigger");
		m_bShow_SpeedPopup = false;
	}
	if (ImGui::BeginPopupModal("Fix_Trigger?##SpeedTrigger", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Before : %f, %f\nFixed : %f, %f", (*m_SelectedSpeedTrigger).TriggerTime, (*m_SelectedSpeedTrigger).AnimationSpeed, m_pCommander->Get_CurrentTrackPosition(), InputTrigger.AnimationSpeed);
		if (ImGui::Button("OK##SpeedPopup"))
		{
			(*m_SelectedSpeedTrigger).AnimationSpeed = InputTrigger.AnimationSpeed;
			(*m_SelectedSpeedTrigger).TriggerTime = m_pCommander->Get_CurrentTrackPosition();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No##SpeedPopup"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove##SpeedPopup"))
		{
			m_mapSpeedTriggers[iCurrentAnimationIndex].erase(m_SelectedSpeedTrigger);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

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
