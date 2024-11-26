#pragma once
#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"

#include "RigidBody.h"
#include "Particle_Explosion.h"
#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"
#include "Weapon.h"
#include "FSM_Player.h"
#include "..\Public\Pooling_Manager.h"
#include "MyMath.h"


CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CActor(Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	m_iCurrentFSM = FSM_SH;

	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.fSpeedPerSec = 10.0f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = 5.f;
	m_pAnimationSpeed = (_float*)(pArg);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 5.f, 1.f));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_iState = OBJSTATE_IDLE;

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Priority_Update(fTimeDelta);
}

_int CPlayer::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_POOL;

	Key_Input(fTimeDelta);

	//State_Frame(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);
	m_pRigidBodyCom->isFalling(false);


	for (auto& pPartObject : m_Parts)
		pPartObject->Update(fTimeDelta);

	_matrix RHandMatrix = XMLoadFloat4x4(m_pBodyModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bip001-R-Hand"));
	_matrix LHandMatrix = XMLoadFloat4x4(m_pBodyModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bip001-L-Hand"));

	static_cast<CWeapon*>(m_Parts[PART_AXE])->HandControl(RHandMatrix, LHandMatrix);
	
	return OBJ_NOEVENT;
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, m_Parts[PART_AXE]);
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, m_Parts[PART_SWORD]);
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, m_Parts[PART_SHIELD]);

	Vector3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLightPos = XMVectorSet(vMyPos.x, vMyPos.y + 40.f, vMyPos.z - 30, 0.f);

	 
}

HRESULT CPlayer::Render() { return S_OK; }

void CPlayer::Set_CurrentTrackPosition(_double dPosition)
{
	m_pBodyModelCom->Set_CurrentTrackPosition(dPosition);
}

_double CPlayer::Get_Duration()
{
	return m_Parts[PART_BODY]->Get_Duration();
}

_double* CPlayer::Get_CurrentTrackPosition_ptr()
{
	m_Parts[PART_BODY]->Get_CurrentTrackPosition_ptr();
	m_Parts[PART_HEAD]->Get_CurrentTrackPosition_ptr();
	m_Parts[PART_HAIR]->Get_CurrentTrackPosition_ptr();


	return m_pBodyModelCom->Get_CurrentTrackPosition_ptr();
}

void CPlayer::Register_ModelCom(list<class CModel*>& ModelList)
{
	ModelList.push_back(static_cast<CModel*>(m_Parts[PART_BODY]->Find_Component(TEXT("Com_Model"))));
	ModelList.push_back(static_cast<CModel*>(m_Parts[PART_HEAD]->Find_Component(TEXT("Com_Model"))));
	ModelList.push_back(static_cast<CModel*>(m_Parts[PART_HAIR]->Find_Component(TEXT("Com_Model"))));
}

void CPlayer::Set_State(OBJ_STATE _eState, _bool _bLerp)
{
	m_iState = _eState;
	for (auto part : m_Parts)
	{
		part->Set_State((_uint)_eState, _bLerp);
	}
}

void CPlayer::Set_ChainState(OBJ_STATE _eState, _bool bLerp)
{
	m_iState = _eState;
	for (auto& part : m_Parts)
	{
		part->Set_ChainState(_eState, bLerp);
	}
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(KeyType::UP))
		m_pTransformCom->Go_Straight(fTimeDelta);
	else if (m_pGameInstance->Get_DIKeyState(KeyType::RIGHT))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	else if (m_pGameInstance->Get_DIKeyState(KeyType::DOWN))
		m_pTransformCom->Go_Backward(fTimeDelta);
	else if (m_pGameInstance->Get_DIKeyState(KeyType::LEFT))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta);
	else if (m_pGameInstance->Get_DIKeyState(KeyType::Key1))
	{
		m_iCurrentFSM = FSM_AXE;
		Set_State(OBJSTATE_ATT1);
	}
	else if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key2))
	{
		m_iCurrentFSM = FSM_AXE;
		Set_State(OBJSTATE_ATT2);
	}
	else if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key3))
	{
		m_iCurrentFSM = FSM_AXE;
		Set_State(OBJSTATE_ATT3);
	}
	else if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key4))
	{
		m_iCurrentFSM = FSM_AXE;
		Set_State(OBJSTATE_SH_DRAW);
	}
	else if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key5))
	{
		m_iCurrentFSM = FSM_AXE;
		Set_State(OBJSTATE_SH_UNDRAW);
	}
	else if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key6))
	{
		m_iCurrentFSM = FSM_AXE;
		Set_State(OBJSTATE_ROLL);
	}
	else if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key7))
	{
		m_iCurrentFSM = FSM_AXE;
		Set_State(OBJSTATE_IDLE);
	}

}

void CPlayer::Key_Input_SH(_float fTimeDelta)
{
	if (!m_pBodyFSM->ActingCheck())
	{
		if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Z))
		{
			m_bDrawWeapon = !m_bDrawWeapon;

			Set_State(OBJSTATE_SH_UNDRAW);
			m_iCurrentFSM = FSM_COMM;
		}
		if (m_pGameInstance->Get_DIMouseState_Once(DIMK_LBUTTON))
		{
			if (OBJSTATE_ATT1 == m_pBodyFSM->Get_CurrentState())
			{
				Set_State(OBJSTATE_ATT2);
				m_pBodyFSM->isActing();
				return;
			}
			else
			{
				Set_State(OBJSTATE_ATT1);
				m_pBodyFSM->isActing();
				return;
			}

		}
		if (m_pGameInstance->Get_DIKeyState(KeyType::LCTRL))
		{
			if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key1))
			{
				Set_State(OBJSTATE_SKILL5);
				return;
			}
			if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key2))
			{
				Set_State(OBJSTATE_SKILL6);
				return;
			}
			if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key3))
			{
				Set_State(OBJSTATE_SKILL7);
				return;
			}
			if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key4))
			{
				Set_State(OBJSTATE_SKILL8);
				return;
			}
		}
		if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key1))
		{
			Set_State(OBJSTATE_SKILL1);
			m_pBodyFSM->isActing();
			return;
		}
		if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key2))
		{
			Set_State(OBJSTATE_SKILL2);
			m_pBodyFSM->isActing();
			return;
		}
		if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key3))
		{
			Set_State(OBJSTATE_SKILL3);
			m_pRigidBodyCom->Strike(m_pTransformCom->Get_State(CTransform::STATE_UP), 1.f);
			m_pRigidBodyCom->isFalling(true);
			m_pBodyFSM->isActing();
			return;
		}
		if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key4))
		{
			Set_State(OBJSTATE_SKILL4);
			m_pBodyFSM->isActing();
			return;
		}

		if (m_pGameInstance->Get_DIKeyState(KeyType::W) || m_pGameInstance->Get_DIKeyState(KeyType::A) | m_pGameInstance->Get_DIKeyState(KeyType::S) | m_pGameInstance->Get_DIKeyState(KeyType::D))
		{
			_matrix CamWorld = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
			_vector moveResult = {};
			if (m_pGameInstance->Get_DIKeyState(KeyType::D))
			{
				moveResult += CamWorld.r[0];
			}
			else if (m_pGameInstance->Get_DIKeyState(KeyType::A))
			{
				moveResult += CamWorld.r[0] * -1.f;
			}
			if (m_pGameInstance->Get_DIKeyState(KeyType::W))
			{
				moveResult += CamWorld.r[2];
			}
			else if (m_pGameInstance->Get_DIKeyState(KeyType::S))
			{
				moveResult += CamWorld.r[2] * -1.f;
			}
			if (m_iState != OBJSTATE_RUN)
			{
				Turn_Move(moveResult);
				m_iState = OBJSTATE_RUN;
			}
			if (m_pBodyFSM->Get_CurrentAniType() != ANI_CHAIN)
				Set_State(OBJSTATE_RUN, false);
			m_pTransformCom->Go_Direction(moveResult, fTimeDelta);
			m_pTransformCom->Set_Look(MYVectorZeroY(moveResult));
		}

		if (m_pGameInstance->Get_DIKeyState_Once(KeyType::SPACE))
		{
			Set_State(OBJSTATE_ROLL);
			m_iTriggerCount = 0;
			m_pBodyFSM->isActing();
		}
		if (m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::S)
			|| m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::A)
			|| m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::D)
			|| m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::W))
		{
			Set_State(OBJSTATE_IDLE);
		}
	}
}

void CPlayer::Key_Input_COMM(_float fTimeDelta)
{
	if (!m_pBodyFSM->ActingCheck())
	{
		if (OBJSTATE_TUMBLEB == m_pBodyFSM->Get_CurrentState() || OBJSTATE_TUMBLEF == m_pBodyFSM->Get_CurrentState() || OBJSTATE_TUMBLEL == m_pBodyFSM->Get_CurrentState() || OBJSTATE_TUMBLER == m_pBodyFSM->Get_CurrentState())
		{
			if (m_pGameInstance->Get_DIKeyState_Once(KeyType::SPACE))
			{
				Set_State(m_pBodyFSM->Get_CurrentState() == OBJSTATE_TUMBLEB ? OBJSTATE_TUMBLEE : OBJSTATE_TUMBLEE_F);
				m_iCurrentFSM = m_iCurrentWeapon;
				m_pBodyFSM->isActing();
			}
		}
		else
		{
			if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Z))
			{
				m_bDrawWeapon = !m_bDrawWeapon;

				if (m_bDrawWeapon)
				{
					Set_State(OBJSTATE_SH_DRAW);
					m_iCurrentFSM = FSM_SH;
					m_pBodyFSM->isActing();
				}
			}
			if (m_pGameInstance->Get_DIKeyState_Once(KeyType::Key1))
			{
				Set_State(OBJSTATE_TUMBLING);
				m_pBodyFSM->isActing();
			}
			if (m_pGameInstance->Get_DIKeyState(KeyType::W) || m_pGameInstance->Get_DIKeyState(KeyType::A) | m_pGameInstance->Get_DIKeyState(KeyType::S) | m_pGameInstance->Get_DIKeyState(KeyType::D))
			{
				_matrix CamWorld = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
				_vector moveResult = {};
				if (m_pGameInstance->Get_DIKeyState(KeyType::D))
				{
					moveResult += CamWorld.r[0];
				}
				else if (m_pGameInstance->Get_DIKeyState(KeyType::A))
				{
					moveResult += CamWorld.r[0] * -1.f;
				}
				if (m_pGameInstance->Get_DIKeyState(KeyType::W))
				{
					moveResult += CamWorld.r[2];
				}
				else if (m_pGameInstance->Get_DIKeyState(KeyType::S))
				{
					moveResult += CamWorld.r[2] * -1.f;
				}
				if (m_iState != OBJSTATE_RUN)
				{
					m_iState = OBJSTATE_RUN;
				}
				if (m_pBodyFSM->Get_CurrentAniType() != ANI_CHAIN)
					Set_State(OBJSTATE_RUN, false);
				m_pTransformCom->Go_Direction(moveResult, fTimeDelta);
				m_pTransformCom->Set_Look(MYVectorZeroY(moveResult));
			}
			if (m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::S)
				|| m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::A)
				|| m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::D)
				|| m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::W))
			{
				Set_State(OBJSTATE_IDLE);
			}
		}
	}
}

void CPlayer::State_Frame(_float fTimeDelta)
{
	const _float4x4* pEventBone;
	_bool bTriggerEvent = m_pBodyModelCom->Check_TriggerQueue(pEventBone);
	if (OBJSTATE_ATT1 == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			m_pBodyFSM->isNotActing();
		}
	}
	else if (OBJSTATE_ATT2 == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			m_pBodyFSM->isNotActing();
		}
	}
	else if (OBJSTATE_SKILL1 == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			m_pBodyFSM->isNotActing();
		}
		if (m_pBodyFSM->ActingCheck())
			m_pTransformCom->Go_Straight(fTimeDelta * 0.2f);
	}
	else if (OBJSTATE_SKILL2 == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			m_pBodyFSM->isNotActing();
			cout << "skill2 Trigger Actived" << endl;
		}
		if (m_pBodyFSM->ActingCheck())
			m_pTransformCom->Go_Straight(fTimeDelta * 0.2f);
	}
	else if (OBJSTATE_SKILL6 == m_pBodyFSM->Get_CurrentState())
	{
		m_pTransformCom->Set_Look(MYVectorZeroY(m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW).r[2]));
	}
	else if (OBJSTATE_ROLL == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			switch (m_iTriggerCount) //트리거 한개 : 움직임 멈춤
			{
			case 0:
				m_iTriggerCount = 1;
				m_pBodyFSM->isNotActing();
				break;
			default:
				break;
			}
		}
		if (0 == m_iTriggerCount)
			m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else if (OBJSTATE_SH_DRAW == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			switch (m_iTriggerCount)
			{
			case 0:
				m_Parts[PART_SWORD]->Set_SocketMatrix(m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bip001-R-Finger01"));
				m_iTriggerCount = 1;
				break;
			case 1:
				m_Parts[PART_SHIELD]->Set_SocketMatrix(m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bip001-L-Finger01"));
				static_cast<CWeapon*>(m_Parts[PART_SHIELD])->StoreShield(true);
				m_iTriggerCount = 0;
				break;
			default:
				break;
			}
		}
	}
	else if (OBJSTATE_SH_UNDRAW == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			switch (m_iTriggerCount)
			{
			case 0:
				m_Parts[PART_SHIELD]->Set_SocketMatrix(m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bip001-Spine2"));
				static_cast<CWeapon*>(m_Parts[PART_SHIELD])->StoreShield(false);
				m_iTriggerCount = 1;
				break;
			case 1:
				m_Parts[PART_SWORD]->Set_SocketMatrix(m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bone_Skirt_L_Root"));
				m_iTriggerCount = 0;
				break;
			default:
				break;
			}
		}
	}
	else if (OBJSTATE_ATT1 == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			m_pBodyFSM->isNotActing();
		}
		if (!m_pBodyFSM->ActingCheck()) 
		{
			//원래는 충돌이었음
		}
	}
	else if (OBJSTATE_TUMBLEB == m_pBodyFSM->Get_CurrentState()
		|| OBJSTATE_TUMBLEL == m_pBodyFSM->Get_CurrentState()
		|| OBJSTATE_TUMBLER == m_pBodyFSM->Get_CurrentState()
		|| OBJSTATE_TUMBLEF == m_pBodyFSM->Get_CurrentState())
	{
		if (bTriggerEvent)
		{
			m_pBodyFSM->isNotActing();
		}
	}

}

//달리기 시작모션 + 몸돌리기
void CPlayer::Turn_Move(_fvector vDir)
{
	_float temp = XMConvertToDegrees(Compute_Angle(MYVectorZeroY(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), MYVectorZeroY(vDir)));
	if (abs(temp) >= 45.f)
	{
		if (signbit(temp))
		{
			Set_ChainState(temp <= -135.f ? OBJSTATE_RUN_TURN180L : OBJSTATE_RUN_TURN90L, false);
		}
		else
		{
			Set_ChainState(temp >= 135.f ? OBJSTATE_RUN_TURN180R : OBJSTATE_RUN_TURN90R, false);
		}
	}
	else
		Set_ChainState(OBJSTATE_RUN_S, false);
}

void CPlayer::Turn_Tumvle(_float fAngle)
{
	if (abs(fAngle) <= 45.f)
	{
		Set_ChainState(OBJSTATE_TUMBLEB);
	}
	else if (abs(fAngle) <= 135.f)
	{
		if (signbit(fAngle))
		{
			Set_ChainState(OBJSTATE_TUMBLEL, false);
		}
		else
		{
			Set_ChainState(OBJSTATE_TUMBLER, false);
		}
	}
	else if (abs(fAngle) > 135.f)
	{
		Set_ChainState(OBJSTATE_TUMBLEF, false);
	}
	m_pBodyFSM->isActing();
}

HRESULT CPlayer::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pRigidBodyCom = CRigidBody::Create(m_pDevice, m_pContext, m_pTransformCom->Get_WorldMatrix_Ptr(true));
	if (nullptr == m_pRigidBodyCom)
		return E_FAIL;
	m_Components.emplace(TEXT("Com_RigidBody"), m_pRigidBodyCom);
	Safe_AddRef(m_pRigidBodyCom);

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
	vector<CPartObject*>().swap(m_Parts); //캐퍼시티가 이상해서 리셋하고 시작함
	m_Parts.resize(PART_END);

	CBody_Player::BODY_DESC		BodyDesc{};
	BodyDesc.pParentState = nullptr;
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pFSMIndex = &m_iCurrentFSM;
	BodyDesc.pAnimationSpeed = m_pAnimationSpeed;
	if (FAILED(Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
		return E_FAIL;

	CHead_Player::HEAD_DESC HeadDesc{};
	HeadDesc.pParentState = nullptr;
	HeadDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	HeadDesc.pFSMIndex = &m_iCurrentFSM;
	HeadDesc.pAnimationSpeed = m_pAnimationSpeed;
	if (FAILED(Add_PartObject(PART_HEAD, TEXT("Prototype_GameObject_Head_Player"), &HeadDesc)))
		return E_FAIL;

	CHair_Player::HAIR_DESC HairDesc{};
	HairDesc.pParentState = nullptr;
	HairDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	HairDesc.pFSMIndex = &m_iCurrentFSM;
	HairDesc.pAnimationSpeed = m_pAnimationSpeed;
	if (FAILED(Add_PartObject(PART_HAIR, TEXT("Prototype_GameObject_Hair_Player"), &HairDesc)))
		return E_FAIL;



	CWeapon::WEAPON_DESC WeaponDesc{};
	WeaponDesc.ModelTag = ModelTag_Sword;
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bip001-R-Hand");
	if (FAILED(Add_PartObject(PART_SWORD, GameTag_Weapon, &WeaponDesc)))
		return E_FAIL;

	WeaponDesc.ModelTag = ModelTag_Shield;
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bip001-L-Hand");
	if (FAILED(Add_PartObject(PART_SHIELD, GameTag_Weapon, &WeaponDesc)))
		return E_FAIL;

	WeaponDesc.ModelTag = ModelTag_GoodAxe;
	WeaponDesc.pSocketBoneMatrix = m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bip001-R-Hand");
	if (FAILED(Add_PartObject(PART_AXE, GameTag_Weapon, &WeaponDesc)))
		return E_FAIL;

	m_pBodyModelCom = static_cast<CModel*>(m_Parts[PART_BODY]->Find_Component(TEXT("Com_Model")));
	Safe_AddRef(m_pBodyModelCom);

	m_pBodyFSM = static_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_FSM(0);
	if (nullptr == m_pBodyFSM)
		return E_FAIL;




	return S_OK;
}

HRESULT CPlayer::Add_PartObject(_uint iPartID, const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPartObject = m_pGameInstance->Clone_GameObject(strPrototypeTag, pArg);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_Parts[iPartID] = dynamic_cast<CPartObject*>(pPartObject);

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pPartObject : m_Parts)
		Safe_Release(pPartObject);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBodyModelCom);
	Safe_Release(m_pRigidBodyCom);
}
