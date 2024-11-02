#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "Particle_Explosion.h"
#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"
#include "Weapon.h"
#include "..\Public\Pooling_Manager.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor { pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer & Prototype)
	: CActor(Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.fSpeedPerSec = 10.0f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = 5.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_iState = STATE_IDLE;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.1f, 0.f, 0.1f, 1.f));



	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Priority_Update(fTimeDelta);
}

_int CPlayer::Update(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());


	for (auto& pPartObject : m_Parts)
		pPartObject->Update(fTimeDelta);

	CPooling_Manager::Get_Instance();


	return OBJ_NOEVENT;
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Late_Update(fTimeDelta);
	
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
	m_pGameInstance->Add_DebugObject(m_pNavigationCom);
#endif
}

HRESULT CPlayer::Render()
{
//#ifdef _DEBUG
//	m_pColliderCom->Render();
//	m_pNavigationCom->Render();
//#endif


	return S_OK;
}

void CPlayer::Set_State(OBJ_STATE _eState)
{
	for (auto part : m_Parts)
	{
		part->Set_State((_uint)_eState);
	}
	//static_cast<CBody_Player*>(m_Parts[PART_BODY])->Set_State(_eState);
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(KeyType::DOWN))
		m_pTransformCom->Go_Backward(fTimeDelta);
	if (m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::DOWN))
		Set_State(OBJSTATE_IDLE);
	
	if (m_pGameInstance->Get_DIKeyState(KeyType::LEFT))
		m_pTransformCom->Turn(false, true, false, fTimeDelta * -1.f);
	if (m_pGameInstance->Get_DIKeyState(KeyType::LEFT))
		Set_State(OBJSTATE_IDLE);

	if (m_pGameInstance->Get_DIKeyState(KeyType::RIGHT))
		m_pTransformCom->Turn(false, true, false, fTimeDelta);
	if (m_pGameInstance->Get_DIKeyState(KeyType::RIGHT))
		Set_State(OBJSTATE_IDLE);

	if (m_pGameInstance->Get_DIKeyState(KeyType::UP))
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

		Set_State(OBJSTATE_RUN);
	}
	if (m_pGameInstance->Get_DIKeyState_Once_Up(KeyType::UP))
	{
		Set_State(OBJSTATE_IDLE);
	}
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};

	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 1.0f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
	vector<CPartObject*>().swap(m_Parts); //캐퍼시티가 이상해서 리셋하고 시작함
	m_Parts.resize(PART_END);

	map<OBJ_STATE, pair<_uint, ANITYPE>> AnimationIndex;

	AnimationIndex[OBJSTATE_IDLE] = { 73, ANI_LOOP };

	AnimationIndex[OBJSTATE_ATT1] = { 0, ANI_BACKTOIDLE };
	AnimationIndex[OBJSTATE_ATT2] = { 2, ANI_BACKTOIDLE };
	AnimationIndex[OBJSTATE_ATT3] = { 4, ANI_BACKTOIDLE };
	AnimationIndex[OBJSTATE_ATT4] = { 6, ANI_BACKTOIDLE };

	AnimationIndex[OBJSTATE_ATT1_B] = { 1, ANI_BACKTOIDLE };
	AnimationIndex[OBJSTATE_ATT2_B] = { 3, ANI_BACKTOIDLE };
	AnimationIndex[OBJSTATE_ATT3_B] = { 5, ANI_BACKTOIDLE };
	AnimationIndex[OBJSTATE_ATT4_B] = { 7, ANI_BACKTOIDLE };

	AnimationIndex[OBJSTATE_RUN] = { 34, ANI_LOOP};


	CBody_Player::BODY_DESC		BodyDesc{};
	BodyDesc.mapAnimationIndex = &AnimationIndex;
	BodyDesc.pParentState = &m_iState;
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	if (FAILED(Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
		return E_FAIL;

	CHead_Player::HEAD_DESC HeadDesc{};
	HeadDesc.mapAnimationIndex = &AnimationIndex;
	HeadDesc.pParentState = &m_iState;
	HeadDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	if (FAILED(Add_PartObject(PART_HEAD, TEXT("Prototype_GameObject_Head_Player"), &HeadDesc)))
		return E_FAIL;
	
	CHair_Player::HAIR_DESC HairDesc{};
	HairDesc.mapAnimationIndex = &AnimationIndex;
	HairDesc.pParentState = &m_iState;
	HairDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	if (FAILED(Add_PartObject(PART_HAIR, TEXT("Prototype_GameObject_Hair_Player"), &HairDesc)))
		return E_FAIL;

	CWeapon::WEAPON_DESC WeaponDesc{};
	WeaponDesc.ModelTag = ModelTag_Sword;
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bip001-L-Finger01");
	if (FAILED(Add_PartObject(PART_SWORD, GameTag_Weapon, &WeaponDesc)))
		return E_FAIL;

	WeaponDesc.ModelTag = ModelTag_Shield;
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_Parts[PART_BODY]->Get_BoneCombindTransformationMatrix_Ptr("Bip001-R-Finger01");
	if (FAILED(Add_PartObject(PART_SHIELD, GameTag_Weapon, &WeaponDesc)))
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

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
}
