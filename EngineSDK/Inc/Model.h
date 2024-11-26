#pragma once

#include "Component.h"
#include "Bone.h"
#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes;
	}

	TYPE Get_ModelType() const { return m_eType;
	}

	_int Get_BoneIndex(const _char* pBoneName) const;
	_int Get_AnimIndex(const _char* pAnimName) const;
	_matrix Get_BoneCombindTransformationMatrix(_uint iBoneIndex) const {
		return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
	}
	const _float4x4* Get_BoneCombindTransformationMatrix_Ptr(const _char* pBoneName) const {
		return m_Bones[Get_BoneIndex(pBoneName)]->Get_CombinedTransformationMatrix_Ptr();
	}
	_float3* Get_BoneCombindPos(const _char* pBoneName) { return m_Bones[Get_BoneIndex(pBoneName)]->Get_CombinedPos(); }
	void Set_Bones(const vector<class CBone*>* vBone);
	const vector<class CBone*>* Get_Bones() { return &m_Bones; };
	void Set_AnimFrameReset()
	{
		for (auto& i : m_KeyFrameIndices[m_iCurrentAnimIndex])
			i = 0;
		m_CurrentTrackPosition = 0;
		Play_Animation(0.f);
		TriggerSetting();
	}

	void Set_CurrentTrackPosition(_double dPosition);
	void Change_Bone(CBone* _pBone, _uint _iBoneIndex);
	CBone* Get_Bone(const char* BoneName);
	void Set_Skip(_int _iSkip) { m_iNumSkip = _iSkip; }

	_double& Get_CurrentTrackPosition() { return m_CurrentTrackPosition; }
	_double* Get_CurrentTrackPosition_ptr() { return &m_CurrentTrackPosition; }
	_double Get_Duration();
	_uint Get_CurrentAnimationIndex() { return m_iCurrentAnimIndex; }
	_uint Get_AnimationNum() { return m_iNumAnimations; }
	_uint Get_CurrentTrigger() { return m_iCurrentTrigger; }
	void SetUp_Animation(_uint iAnimationIndex, _bool isLoop = false)
	{
		m_iCurrentAnimIndex = iAnimationIndex;
		m_isLoop = isLoop;
	}
	void Set_NextAnimIndex(_uint iAnimIndex, _bool isLoop = false, _float fTime = 0.2f, _float fPlaySpeed = 1.f, _double dSubTime = 0., _double dStartPosition = 0)
	{
		if (m_iNextAnimIndex != iAnimIndex) { m_bInterupted = true; }
		m_iNextAnimIndex = iAnimIndex;
		m_fPlaySpeed = fPlaySpeed;
		m_fLinearTotalTime = fTime;
		m_dSubTime = dSubTime;
		m_isLoop = isLoop;
		m_NextStartTrackPosition = dStartPosition;
	}
	 
	vector<class CMesh*>* Get_Meshs() { return &m_Meshes; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix, ifstream* LoadStream = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);




	/* 뼈를 움직인다.(CBone`s m_TransformationMatrix행렬을 갱신한다.) */
	/* 모든 뼈가 가지고 있는 m_CombinedTransformationMatrix를 갱신한다. */
	_bool Play_Animation(_float fTimeDelta);
	_bool Play_Animation(_float fTimeDelta, const char* _BoneName); // 루트애님
	_bool Play_TriggerAnimation(_float fTimeDelta); // 밟은 트리거 반환 -> 밟은 트리거를 스텍에 넣어주는걸로 변경
	_bool Animation_Interpolation(_float fTimeDelta);

	HRESULT Read_BinFile(const _tchar* pModelFilePath);

public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex);
	HRESULT Check_Material(aiTextureType eMaterialType, _uint iMeshIndex);
	HRESULT Bind_MeshBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

	HRESULT Bind_Bone_Mesh(CModel* pOtherModel);

private:
	DATA_BINSCENE* m_pBin_Scene = nullptr;
	TYPE							m_eType = { TYPE_END };

private: /* 메시의 정보를 저장한다. */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	_float4x4						m_PreTransformMatrix = {};

private:
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;

private:
	vector<class CBone*>			m_Bones;
	vector< _float4x4>				m_StartBonesTransforms;
	vector<class CBone*>			m_NewBones;

private:
	_bool							m_isLoop = { false };
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

	/* 현재 애니메이션의 재생 위치. */
	_double							m_CurrentTrackPosition = {};
	_double							m_NextStartTrackPosition = { 0 };
	/*For. NextIndex*/
	_uint							m_iNextAnimIndex = 0;
	_float							m_fLinearTotalTime = 0.f;
	_float							m_fLinearCurrentTime = 0.f;
	/*For. Lineared*/
	_bool							m_bLinearFinished = true;
	_bool							m_bInterupted = false;

	/* 각 애니메이션당 각 채널들의 현재 키프레임인덱스 */
	vector<vector<_uint>>			m_KeyFrameIndices;
	_float							m_fPlaySpeed = 1.f;
	_double							m_dSubTime = 0.;

	//애니메이션 트리거
	map<_uint, vector<DEFAULTTRIGGER>> m_mapAnimationTrigger;	// 얘도 큐로 바꾸고싶긴 한데....문제 없나?
	map<_uint, queue<BONENAME>> m_mapTrigger_BoneNames;			// 이펙트 트리거가 발생할 뼈의 이름 -> 쓸때마다 여기서 복사해감
	map<_uint, queue<SPEEDTRIGGER>>	m_mapTrigger_Speed;			// 스피드트리거
	_uint						m_iCurrentTrigger = { 0 };
	queue<DEFAULTTRIGGER>		m_queueTrigger;					// 실행해야 할 트리거, 뼈이름 큐는 동시에 세팅해준다.
	queue<BONENAME>				m_queueTrigger_BoneName;		// 현재 이벤트에서 사용할 예정인 뼈 이름들
	queue<SPEEDTRIGGER>			m_queueTrigger_Speed;			// 스피드트리거 -> 얜 밖에서 확인도 못하고 걍 내부에서 다 처리될거임

	//몇개 건너뛰면 되는지(뼈 통합하면서 중복업뎃을 막는 상황의 경우
	_int					m_iNumSkip = { 0 };

public:
	void Register_Trigger(map<_uint, vector<_double>>* pEventTrigger, map<_uint, vector<EFFECTTRIGGER>>* _pEffectTrigger, map<_uint, queue<SPEEDTRIGGER>>* _pSpeedTrigger); // 툴에서만 쓰는거고 클라에선 모델생성할떄 받는거 수정해서 받아야됨;
	_bool Check_TriggerQueue(const _float4x4*& BoneMatrix);

private:
	void TriggerSetting(); // 애니 전환할때 트리거 세팅

public:
	HRESULT	Ready_Meshes();
	HRESULT Ready_Materials(const _tchar* pModelFilePath);
	HRESULT Ready_Bones();
	HRESULT Ready_Animations();
	HRESULT Ready_Triggers(ifstream* LoadStream);

	HRESULT	Safe_Release_Scene();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity(), ifstream* LoadStream = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END