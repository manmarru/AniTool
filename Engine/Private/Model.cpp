#include "..\Public\Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	// , m_Bones { Prototype.m_Bones }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
	, m_iCurrentAnimIndex{ Prototype.m_iCurrentAnimIndex }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_Animations{ Prototype.m_Animations }
	, m_CurrentTrackPosition{ Prototype.m_CurrentTrackPosition }
	, m_KeyFrameIndices{ Prototype.m_KeyFrameIndices }
{
	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	//m_Bones.emplace_back(�������� �Ķ����.);

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_NewBones.emplace_back(pPrototypeBone->Clone());

	m_StartBonesTransforms.resize(m_Bones.size());

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_AddRef(pTexture);
	}


	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto triggerpair : Prototype.m_mapAnimationTrigger)
	{
		for (auto trigger : triggerpair.second)
		{
			m_mapAnimationTrigger[triggerpair.first].push_back(trigger);
		}
	}
}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_uint	iBoneIndex = { 0 };
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (0 == strcmp(pBone->Get_Name(), pBoneName))
				return true;
			++iBoneIndex;
			return false;
		});

	if (iter == m_Bones.end())
		MSG_BOX(TEXT("���µ�?"));

	return iBoneIndex;
}

_int CModel::Get_AnimIndex(const _char* pAnimName) const
{
	_uint	iAnimIndex = { 0 };
	auto	iter = find_if(m_Animations.begin(), m_Animations.end(), [&](CAnimation* pAnim)->_bool
		{
			if (0 == strcmp(pAnim->Get_Name(), pAnimName))
				return true;
			++iAnimIndex;
			return false;
		});

	if (iter == m_Animations.end())
		MSG_BOX(TEXT("�׷� �ִϸ��̼� ���µ�?"));

	return iAnimIndex;
}

void CModel::Set_Bones(const vector<class CBone*>* vBone)
{
	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pBone : *vBone)
		Safe_AddRef(pBone);

	m_Bones = *vBone;

	/*for (auto mesh : m_Meshes)
	{
		mesh->Set_BoneNum(m_Bones.size());
	}*/
}

void CModel::Set_CurrentTrackPosition(_double dPosition)
{
	m_CurrentTrackPosition = dPosition;
	
	m_Animations[m_iCurrentAnimIndex];
}

void CModel::Change_Bone(CBone* _pBone, _uint _iBoneIndex)
{
	Safe_Release(m_Bones[_iBoneIndex]);
	m_Bones[_iBoneIndex] = _pBone;
	Safe_AddRef(_pBone);
}

CBone* CModel::Get_Bone(const char* BoneName)
{
	for (auto& pBone : m_Bones)
	{
		if (!strcmp(pBone->Get_Name(), BoneName))
		{
			return pBone;
		}
	}

	return nullptr;
}

_double CModel::Get_Duration()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_Duration();
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix, ifstream* LoadStream)
{
	m_eType = eType;

	Read_BinFile(pModelFilePath);
	if (nullptr == m_pBin_Scene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	m_eType = eType;

	/*  ������ ������ �ε��Ѵ�. ���� ( aiNode, aiBone, aiNodeAnim ) */
	/* aiNode : �θ�, �ڽ��� �, �ڽ��� ��������? , �ʱ��������� �����ͼ� �θ�κ��� �Ļ��� ����� ���Ͽ� �������λ�������� ����� �ִ� ������ �������ش�. */
	/* aiBone : �޽ÿ� ���ԵǾ��ִ� ���� ������ �ǹ�. �� ���� � �޽��� �����鿡�� ������ �ִ���? �󸶳� ���⤷���ִ���? �󸶳� ����(offset)���ʿ�����? */
	/* aiNodeAnim(== Channel) : �ִϸ��̼ǿ��� ����ϴ� ���� ������ �ǹ�. �� �ִϸ��̼��� �����Կ� �־� � �ð��뿡 �� ���� � ���¸� ���ؾ��ϴ����� ���� ��������� �����ϱ����� ��Ḧ �����Ѵ�. */
	if (FAILED(Ready_Bones()))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	if (nullptr != LoadStream)
	{
		if (FAILED(Ready_Triggers(LoadStream)))
			return E_FAIL;
	}

	Safe_Release_Scene();

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Read_BinFile(const _tchar* pModelFilePath)
{
	FILE* file = nullptr;

	_wfopen_s(&file, pModelFilePath, L"rb");

	assert(file != nullptr);

	DATA_BINSCENE* Scene = new DATA_BINSCENE;
	ZeroMemory(Scene, sizeof(DATA_BINSCENE));

	// Node
	_int iNodeCount = 0;
	fread(&iNodeCount, sizeof(_int), 1, file);
	Scene->iNodeCount = iNodeCount;
	Scene->pBinNodes = new DATA_BINNODE[iNodeCount];
	for (int i = 0; i < iNodeCount; ++i)
	{
		DATA_BINNODE* pBinNode = &Scene->pBinNodes[i];
		fread(pBinNode, sizeof(DATA_BINNODE), 1, file);
	}

	// Material
	_int iMaterialCount = 0;
	fread(&iMaterialCount, sizeof(_int), 1, file);
	Scene->iMaterialCount = iMaterialCount;
	Scene->pBinMaterial = new DATA_BINMATERIAL[iMaterialCount];
	for (int i = 0; i < iMaterialCount; ++i)
	{
		DATA_BINMATERIAL* pBinMarterial = &Scene->pBinMaterial[i];
		fread(pBinMarterial, sizeof(DATA_BINMATERIAL), 1, file);
	}

	// MashNode
	_int iMashCount = 0;
	fread(&iMashCount, sizeof(_int), 1, file);
	Scene->iMeshCount = iMashCount;
	Scene->pBinMesh = new DATA_BINMESH[iMashCount];
	for (int i = 0; i < iMashCount; ++i)
	{
		DATA_BINMESH* pBinMash = &Scene->pBinMesh[i];

		fread(&pBinMash->cName, sizeof(char) * MAX_PATH, 1, file);
		fread(&pBinMash->iMaterialIndex, sizeof(int), 1, file);

		fread(&pBinMash->NumVertices, sizeof(int), 1, file);

		if (m_eType == TYPE_NONANIM)
		{
			pBinMash->pNonAnimVertices = new VTXMESH[pBinMash->NumVertices];
			pBinMash->pAnimVertices = nullptr;
			for (int j = 0; j < pBinMash->NumVertices; ++j)
			{
				VTXMESH* VtxNonAniModel = &pBinMash->pNonAnimVertices[j];
				fread(VtxNonAniModel, sizeof(VTXMESH), 1, file);
			}
		}
		else
		{
			pBinMash->pNonAnimVertices = nullptr;
			pBinMash->pAnimVertices = new VTXANIMMESH[pBinMash->NumVertices];

			for (int j = 0; j < pBinMash->NumVertices; ++j)
			{
				VTXANIMMESH* VtxAnimModel = &pBinMash->pAnimVertices[j];
				fread(VtxAnimModel, sizeof(VTXANIMMESH), 1, file);
			}
		}

		pBinMash->iNumIndices = 0;
		fread(&pBinMash->iNumIndices, sizeof(int), 1, file);
		pBinMash->pIndices = new _uint[pBinMash->iNumIndices];
		for (int j = 0; j < pBinMash->iNumIndices; ++j)
		{
			_uint* Indices = &pBinMash->pIndices[j];
			fread(Indices, sizeof(_uint), 1, file);
		}


		pBinMash->iNumBones = 0;
		fread(&pBinMash->iNumBones, sizeof(int), 1, file);
		pBinMash->pBones = new DATA_BINBONE[pBinMash->iNumBones];
		for (int j = 0; j < pBinMash->iNumBones; ++j)
		{
			DATA_BINBONE* pBone = &pBinMash->pBones[j];
			fread(pBone, sizeof(DATA_BINBONE), 1, file);
		}
	}

	// Animation
	int iNumAnimations = 0;
	fread(&iNumAnimations, sizeof(int), 1, file);
	Scene->iNumAnimations = iNumAnimations;
	Scene->pBinAnim = new DATA_BINANIM[iNumAnimations];

	for (int i = 0; i < iNumAnimations; ++i)
	{
		fread(&Scene->pBinAnim[i].szName, sizeof(char) * MAX_PATH, 1, file);
		fread(&Scene->pBinAnim[i].iNumChannels, sizeof(_int), 1, file);
		fread(&Scene->pBinAnim[i].Duration, sizeof(double), 1, file);
		fread(&Scene->pBinAnim[i].TickPerSecond, sizeof(double), 1, file);
		//ifs.read((char*)&Scene->pBinAnim[i].bLoop, sizeof(bool));


		Scene->pBinAnim[i].pBinChannel = new DATA_BINCHANNEL[Scene->pBinAnim[i].iNumChannels];
		for (int j = 0; j < Scene->pBinAnim[i].iNumChannels; ++j)
		{
			DATA_BINCHANNEL* pChannel = &Scene->pBinAnim[i].pBinChannel[j];
			fread(&pChannel->szName, sizeof(char) * MAX_PATH, 1, file);
			fread(&pChannel->iNumKeyFrames, sizeof(int), 1, file);

			pChannel->pKeyFrames = new KEYFRAME[pChannel->iNumKeyFrames];
			for (int k = 0; k < pChannel->iNumKeyFrames; ++k)
			{
				KEYFRAME* pKeyFrame = &pChannel->pKeyFrames[k];
				fread((char*)pKeyFrame, sizeof(KEYFRAME), 1, file);
			}
		}
	}

	m_pBin_Scene = Scene;
	fclose(file);

	return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	_bool isFinished = false;
	// �ִϸ��̼� ��ȯ�� �ʿ��� ���
	if (m_iCurrentAnimIndex != m_iNextAnimIndex)
	{
		if (m_bInterupted)
		{
			m_CurrentTrackPosition = m_NextStartTrackPosition;
			m_bLinearFinished = false;
			m_bInterupted = false;

			for (_uint i = m_iNumSkip; i < m_Bones.size(); ++i)
				m_StartBonesTransforms[i] = m_Bones[i]->Get_TransformationMatrix();

			m_Animations[m_iNextAnimIndex]->Update_TransformationMatrices(m_NewBones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iNextAnimIndex], m_isLoop, fTimeDelta);
		}
		if (!m_bLinearFinished)
		{
			m_bLinearFinished = Animation_Interpolation(fTimeDelta);
		}
		// ������ �Ϸ�� ���
		if (m_bLinearFinished)
		{
			for (auto& i : m_KeyFrameIndices[m_iCurrentAnimIndex])
				i = 0;

			for (auto& i : m_KeyFrameIndices[m_iNextAnimIndex])
				i = 0;

			m_iCurrentAnimIndex = m_iNextAnimIndex;
			// ������ �Ϸ�� �� ���ο� �ִϸ��̼� ����
			m_Animations[m_iNextAnimIndex]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iNextAnimIndex], m_isLoop, fTimeDelta);
			TriggerSetting();
		}
	}
	else
	{
		if (!m_bLinearFinished)
		{
			for (auto& i : m_KeyFrameIndices[m_iCurrentAnimIndex])
				i = 0;

			m_bLinearFinished = true;
		}
		isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iCurrentAnimIndex], m_isLoop, fTimeDelta, m_fPlaySpeed, m_dSubTime);
		if (m_bLinearFinished && m_CurrentTrackPosition == 0) // ��������ó�� ���� �ȵ��� �ִ� �����ҋ�
			TriggerSetting();
	}

	//Ʈ���� üũ (�����ϰ� �Ϲ�Ʈ���� �ڵ嵵 �ֱ�
	if (!m_queueTrigger.empty() && m_queueTrigger.front().TriggerTime <= m_CurrentTrackPosition)
	{
		//Ʈ����ó���� ��� �ұ�?
		++m_iCurrentTrigger;

	}
	if (!m_queueTrigger_Speed.empty() && m_queueTrigger_Speed.front().TriggerTime <= m_CurrentTrackPosition)
	{
		while (!m_queueTrigger_Speed.empty() && m_queueTrigger_Speed.front().TriggerTime <= m_CurrentTrackPosition)
		{
			m_fPlaySpeed = m_queueTrigger_Speed.front().AnimationSpeed;
			m_queueTrigger_Speed.pop();
		}
	}

	if (isFinished) // ���� �̰� loop�ִԵ��� ���� ���� ���ϴ°Ű���
		m_iCurrentTrigger = 0;

	// ���� ��ȯ ������Ʈ
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return isFinished;
}

//��Ʈ�ִ� - ������ ���� ȸ���� ������
_bool CModel::Play_Animation(_float fTimeDelta, const char* _BoneName)
{
	_bool isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iCurrentAnimIndex], m_isLoop, fTimeDelta, m_fPlaySpeed, m_dSubTime);

	for (auto& pBone : m_Bones)
	{
		if (0 == strcmp(pBone->Get_Name(), _BoneName))
		{
			pBone->Update_CombinedTransformationMatrix_PassingRotation(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
			continue;
		}
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return isFinished;
}

_bool CModel::Play_TriggerAnimation(_float fTimeDelta)
{
	_bool isFinished = false;
	// �ִϸ��̼� ��ȯ�� �ʿ��� ���
	if (m_iCurrentAnimIndex != m_iNextAnimIndex)
	{
		m_iCurrentTrigger = 0;
		if (m_bInterupted)
		{
			m_CurrentTrackPosition = m_NextStartTrackPosition;
			m_bLinearFinished = false;
			m_bInterupted = false;

			for (_uint i = 0; i < m_Bones.size(); ++i)
				m_StartBonesTransforms[i] = m_Bones[i]->Get_TransformationMatrix();

			m_Animations[m_iNextAnimIndex]->Update_TransformationMatrices(m_NewBones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iNextAnimIndex], m_isLoop, fTimeDelta);
		}
		if (!m_bLinearFinished)
		{
			m_bLinearFinished = Animation_Interpolation(fTimeDelta);
		}
		// ������ �Ϸ�� ���
		if (m_bLinearFinished)
		{
			for (auto& i : m_KeyFrameIndices[m_iCurrentAnimIndex])
				i = 0;

			for (auto& i : m_KeyFrameIndices[m_iNextAnimIndex])
				i = 0;

			m_iCurrentAnimIndex = m_iNextAnimIndex;
			// ������ �Ϸ�� �� ���ο� �ִϸ��̼� ����
			m_Animations[m_iNextAnimIndex]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iNextAnimIndex], m_isLoop, fTimeDelta);
		}
	}
	else
	{
		if (!m_bLinearFinished)
		{
			for (auto& i : m_KeyFrameIndices[m_iCurrentAnimIndex])
				i = 0;

			m_bLinearFinished = true;
		}
		isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iCurrentAnimIndex], m_isLoop, fTimeDelta, m_fPlaySpeed, m_dSubTime);
	
		//Ʈ���� üũ
		if (m_mapAnimationTrigger[m_iCurrentAnimIndex][m_iCurrentTrigger].TriggerTime <= m_CurrentTrackPosition)
		{
			++m_iCurrentTrigger;
		}

		if (!m_queueTrigger_Speed.empty() && m_queueTrigger_Speed.front().TriggerTime <= m_CurrentTrackPosition)
		{
			while (!m_queueTrigger_Speed.empty() && m_queueTrigger_Speed.front().TriggerTime <= m_CurrentTrackPosition)
			{
				m_fPlaySpeed = m_queueTrigger_Speed.front().TriggerTime;
				m_queueTrigger_Speed.pop();
			}
		}

		if (isFinished) // ���� �̰� loop�ִԵ��� ���� ���� ���ϴ°Ű���
			m_iCurrentTrigger = 0;
	}

	// ���� ��ȯ ������Ʈ
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return isFinished;
}

_bool CModel::Animation_Interpolation(_float fTimeDelta)
{
	m_fLinearCurrentTime += fTimeDelta;
	if (m_fLinearCurrentTime >= m_fLinearTotalTime)
	{
		m_fLinearCurrentTime = 0.f;
		return true;
	}

	_float fRatio = m_fLinearCurrentTime / m_fLinearTotalTime;

	_uint iBoneIndex = 0;
	for (auto& Transform : m_StartBonesTransforms)
	{
		// ��� �ε�
		_matrix oldMatrix = XMLoadFloat4x4(&Transform);
		_matrix newMatrix = XMLoadFloat4x4(&m_NewBones[iBoneIndex]->Get_TransformationMatrix());

		// ������, ȸ��, ���� �̵� ���ͷ� �и�
		XMVECTOR vOldScale, vOldRotation, vOldTranslation;
		XMVECTOR vNewScale, vNewRotation, vNewTranslation;
		XMMatrixDecompose(&vOldScale, &vOldRotation, &vOldTranslation, oldMatrix);
		XMMatrixDecompose(&vNewScale, &vNewRotation, &vNewTranslation, newMatrix);

		// ����
		XMVECTOR vScale = XMVectorLerp(vOldScale, vNewScale, fRatio);
		XMVECTOR vRotation = XMQuaternionSlerp(vOldRotation, vNewRotation, fRatio);
		XMVECTOR vTranslation = XMVectorLerp(vOldTranslation, vNewTranslation, fRatio);

		// ���� ��ȯ ��� ���� �� ���� ����


		m_Bones[iBoneIndex++]->Set_TransformationMatrix(
			XMMatrixAffineTransformation(vScale, XMVectorZero(), vRotation, vTranslation)
		);
	}

	return false;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType] == nullptr)
		return S_OK;

	return m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType]->Bind_ShadeResource(pShader, pConstantName, 0);
}

HRESULT CModel::Check_Material(aiTextureType eMaterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_Meshes[iMeshIndex]->IsNameEqual("SM_ENV_KNVLLG_ground_11.md"))
		int a = 3;

	if (m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType] != nullptr)
		return S_OK;

	return E_FAIL;
}

HRESULT CModel::Bind_MeshBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_BoneMatrices(this, pShader, pConstantName);

	return S_OK;
}

HRESULT CModel::Bind_Bone_Mesh(CModel* pOtherModel)
{
	vector<class CMesh*>* pOtherMeshes = pOtherModel->Get_Meshs();

	return S_OK;
}

void CModel::Register_Trigger(map<_uint, vector<_double>>* pEventTrigger, map<_uint, vector<EFFECTTRIGGER>>* _pEffectTrigger, map<_uint, queue<SPEEDTRIGGER>>* _pSpeedTrigger)
{

	for (auto& pair : *pEventTrigger)
	{
		for (auto EventTrigger : pair.second)
		{
			m_mapAnimationTrigger[pair.first].push_back({ EventTrigger, false });
		}
	}
	BONENAME tBONENAME;
	for (auto& pair : *_pEffectTrigger)
	{
		for (auto& EffectTrigger : pair.second)
		{
			m_mapAnimationTrigger[pair.first].push_back({ EffectTrigger.TriggerTime, true });
			strcpy_s(tBONENAME.BoneName, MAX_PATH, EffectTrigger.BoneName);
			m_mapTrigger_BoneNames[pair.first].push(tBONENAME);
		}
	}// ���̸��� �׳� ������� ����
	for (auto& vecPair : m_mapAnimationTrigger)
	{
		sort(vecPair.second.begin(), vecPair.second.end(), [](DEFAULTTRIGGER a, DEFAULTTRIGGER b) {return a.TriggerTime < b.TriggerTime; });
	}

	for (auto& Triggerpair : *_pSpeedTrigger)
	{
		m_mapTrigger_Speed[Triggerpair.first] = Triggerpair.second;
	}
}

void CModel::TriggerSetting()
{
	//�ִϸ��̼��� ��ȯ�Ǹ鼭 Ʈ���� ������ �ٽ� ���ִ� ���� (������ �����ų� �� �ؼ� �Ѿ ����)
	//currentanimationindex�� next�� �ٲ���־�� ��

	//���� ť ����
	queue<DEFAULTTRIGGER> emptyTrigger;
	m_queueTrigger.swap(emptyTrigger);
	queue<BONENAME> emptyNameTrigger;
	m_queueTrigger_BoneName.swap(emptyNameTrigger);

	// �ش� �ִϸ��̼��� Ʈ���ŵ�� ���̸�_����ƮƮ���� �� Queue�� �־�� ��

	/*
	queue<�ð���ġ, ����ƮƮ��������> -> DEFAULTTRIGGER -> ���°������ ��ü ����� �̷��� ���� �˾Ƽ� �ض�;; �� �����ϰԴ� ���ϰڴ�.
	play_animation ���Ŀ� check_queue �Լ��� �־ ���� �ִ� �ð���ġ�� queue.top�� ������ �ð���ġ�� �Ѿ������ üũ�ϴ°� �����Ű���.
	check_queue(&��Ʈ���� ������) �� ���� �����Ϳ� �� ��Ʈ���� �����͸� �־��ְ�, Ʈ���� ��Ҵ����� bool �� ��ȯ����.
	*/

	if (m_mapAnimationTrigger.end() != m_mapAnimationTrigger.find(m_iCurrentAnimIndex))
	{
		int i(0);
		for (auto Trigger : m_mapAnimationTrigger[m_iCurrentAnimIndex])
		{
			m_queueTrigger.push(Trigger);
		}
		if (m_mapTrigger_BoneNames.end() != m_mapTrigger_BoneNames.find(m_iCurrentAnimIndex))
			m_queueTrigger_BoneName = m_mapTrigger_BoneNames[m_iCurrentAnimIndex];
	}

	//���ǵ�Ʈ����
	if (m_mapTrigger_Speed.end() != m_mapTrigger_Speed.find(m_iCurrentAnimIndex))
	{
		m_queueTrigger_Speed = m_mapTrigger_Speed[m_iCurrentAnimIndex];
	}
}

// �� �� ������ true�ϱ� Ʈ���� ������� false ���ö����� �ݺ��ϸ� ��
_bool CModel::Check_TriggerQueue(const _float4x4* &BoneMatrix)
{
	if (m_queueTrigger.empty())
		return false;

	if (m_CurrentTrackPosition >= m_queueTrigger.front().TriggerTime)
	{
		if (m_queueTrigger.front().isEffectTrigger)
		{
			BoneMatrix = Get_Bone(m_queueTrigger_BoneName.front().BoneName)->Get_CombinedTransformationMatrix_Ptr();
			m_queueTrigger.pop();
		}
		return true;
	}

	return false;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pBin_Scene->iMeshCount;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, this, &m_pBin_Scene->pBinMesh[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _tchar* pModelFilePath)
{
	if (nullptr == m_pBin_Scene)
		return E_FAIL;

	m_iNumMaterials = m_pBin_Scene->iMaterialCount;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESH_MATERIAL		MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof(MESH_MATERIAL));

		DATA_BINMATERIAL		pAIMaterial = m_pBin_Scene->pBinMaterial[i];

		DATA_BINMATERIAL		DataMaterialDesc;
		ZeroMemory(&DataMaterialDesc, sizeof(DATA_BINMATERIAL));

		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			if (!strcmp(pAIMaterial.cNames[j], ""))
				continue;


			char			szFullPath[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			//_splitpath_s(pAIMaterial.cNames[j], nullptr, 0, nullptr, 0, szFullPath, MAX_PATH, szExt, MAX_PATH);

			WideCharToMultiByte(CP_ACP, 0, pModelFilePath, (_int)wcslen(pModelFilePath), szFullPath, MAX_PATH, NULL, NULL);
			char* lastBackslash = strrchr(szFullPath, '/');
			if (lastBackslash != nullptr) {
				// ������ �齽���� ���� ���ڸ� null�� �����Ͽ� �齽���ø� ����
				*(lastBackslash + 1) = '\0';
			}
			//strcpy_s(szWideFullPath, pModelFilePath);
			strcat_s(szFullPath, pAIMaterial.cNames[j]);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szWideFullPath, MAX_PATH);

			memcpy(&DataMaterialDesc.cNames[j], &pAIMaterial.cNames[j], sizeof(char) * MAX_PATH);
			MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath, 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
			{
				MSG_BOX(szWideFullPath);
				continue; //return E_FAIL;
			}

		}
		m_Materials.emplace_back(MeshMaterial);
		//m_DataMaterials.push_back(DataMaterialDesc);
	}
	return S_OK;
}

HRESULT CModel::Ready_Bones()
{
	for (int i = 0; i < m_pBin_Scene->iNodeCount; ++i)
	{
		DATA_BINNODE Node = m_pBin_Scene->pBinNodes[i];

		CBone* pBone = CBone::Create(&Node);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pBin_Scene->iNumAnimations;
	m_KeyFrameIndices.resize(m_iNumAnimations);

	for (_int i = 0; i < m_pBin_Scene->iNumAnimations; ++i)
	{
		DATA_BINANIM* pAIAnimation = &m_pBin_Scene->pBinAnim[i];

		CAnimation* pAnimation = CAnimation::Create(pAIAnimation, m_KeyFrameIndices[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HRESULT CModel::Ready_Triggers(ifstream* LoadStream)
{
	if (!LoadStream->is_open())
		return E_FAIL;

	_uint iNumAnimations(0); // Ʈ���� �ִ� �ִ� ����
	_uint iNumTriggers(0); // �ִϸ��̼Ǵ� Ʈ���� ����
	LoadStream->read((char*)&iNumAnimations, sizeof(_uint));
	
	_uint iAnimationIndex(0); // �ִ� �ִϸ��̼� (���� ��������)
	DEFAULTTRIGGER TriggerPos; // Ʈ���� ��ġ
	for (size_t i = 0; i < iNumAnimations; i++)
	{
		m_mapAnimationTrigger;
		LoadStream->read((char*)&iNumTriggers, sizeof(_uint));
		for (size_t i = 0; i < iNumTriggers; i++)
		{
			LoadStream->read((char*)&iAnimationIndex, sizeof(_uint));
			LoadStream->read((char*)&TriggerPos.TriggerTime, sizeof(_double));
			m_mapAnimationTrigger[iAnimationIndex].push_back(TriggerPos);
		}
		sort(m_mapAnimationTrigger[iAnimationIndex].begin(), m_mapAnimationTrigger[iAnimationIndex].end(), []
		(DEFAULTTRIGGER a, DEFAULTTRIGGER b) 
			{
				return a.TriggerTime < b.TriggerTime;
			});
	}

	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		m_mapAnimationTrigger[i].push_back({ m_Animations[i]->Get_Duration() , false });
	}

	return S_OK;
}

HRESULT CModel::Safe_Release_Scene()
{
	Safe_Delete_Array(m_pBin_Scene->pBinNodes);
	Safe_Delete_Array(m_pBin_Scene->pBinMaterial);

	for (_int i = 0; i < m_pBin_Scene->iMeshCount; ++i)
	{
		Safe_Delete_Array(m_pBin_Scene->pBinMesh[i].pAnimVertices);
		Safe_Delete_Array(m_pBin_Scene->pBinMesh[i].pNonAnimVertices);
		Safe_Delete_Array(m_pBin_Scene->pBinMesh[i].pIndices);
		Safe_Delete_Array(m_pBin_Scene->pBinMesh[i].pBones);
	}
	Safe_Delete_Array(m_pBin_Scene->pBinMesh);


	for (_int i = 0; i < m_pBin_Scene->iNumAnimations; ++i)
	{
		for (_int j = 0; j < m_pBin_Scene->pBinAnim[i].iNumChannels; ++j)
		{
			Safe_Delete_Array(m_pBin_Scene->pBinAnim[i].pBinChannel[j].pKeyFrames);
		}
		Safe_Delete_Array(m_pBin_Scene->pBinAnim[i].pBinChannel);
	}
	Safe_Delete_Array(m_pBin_Scene->pBinAnim);

	Safe_Delete(m_pBin_Scene);

	return S_OK;
}


CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix, ifstream* LoadStream)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix, LoadStream)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pBone : m_NewBones)
		Safe_Release(pBone);
	m_NewBones.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();
}
