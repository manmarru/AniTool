#include "..\Public\VIBuffer_Mesh_Instance.h"

#include "GameInstance.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype }
, m_pTextureCom{ Prototype.m_pTextureCom }
{
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix)
{
	if (FAILED(__super::Initialize_Prototype(Desc)))
		return E_FAIL;

	if(FAILED(Read_File(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_VertexBuffer_NonAnim(m_pBin_Scene->pBinMesh,PreTransformMatrix)))
 		return E_FAIL;

	Ready_Materials(pModelFilePath);

	Safe_Release_Scene();
#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXMESHINSTANCE);
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* �������۷� �����Ѵ�. */
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXMESHINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXMESHINSTANCE) * m_iNumInstance);

	VTXMESHINSTANCE* pInstanceVertices = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);

		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f),
			m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f),
			m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f),
			1.f);

		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion




	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion
	m_fjumpSpeed = new _float[m_iNumInstance];
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_fjumpSpeed[i] = 5.f;
	}

	return S_OK;
}

void CVIBuffer_Mesh_Instance::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// ȸ�� ȿ�� �߰�: �������� �����Ӱ� ȸ���ϵ��� (Z�� ȸ��)
		_float fRotationAngle = fTimeDelta * (1.0f + (rand() % 3)); // �ұ�Ģ�� ȸ�� �ӵ�
		_matrix RotationMatrix = XMMatrixRotationZ(fRotationAngle);

		// vRight, vUp, vLook ���͸� ȸ����Ŵ (�ұ�Ģ�ϰ� ȸ��)
		XMStoreFloat4(&pVertices[i].vRight, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vRight), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vUp, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vUp), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vLook, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vLook), RotationMatrix));

		_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);

		/*pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}*/
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Mesh_Instance::SpreadGravity(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// ȸ�� ȿ�� �߰�: �������� �����Ӱ� ȸ���ϵ��� (Z�� ȸ��)
		_float fRotationAngle = fTimeDelta * (1.0f + (rand() % 3)); // �ұ�Ģ�� ȸ�� �ӵ�
		_matrix RotationMatrix = XMMatrixRotationZ(fRotationAngle);

		// vRight, vUp, vLook ���͸� ȸ����Ŵ (�ұ�Ģ�ϰ� ȸ��)
		XMStoreFloat4(&pVertices[i].vRight, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vRight), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vUp, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vUp), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vLook, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vLook), RotationMatrix));

		_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Mesh_Instance::Fountain(_float fTimeDelta, _float fgravity)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// �ұ�Ģ�� ȸ�� ó�� (ȸ�� �ӵ� 10��� ����)
		_float fRotationAngle = fTimeDelta * 10.0f * (1.0f + (rand() % 3));
		_matrix RotationMatrix = XMMatrixRotationZ(fRotationAngle);  // Z�� ȸ��

		// vRight, vUp, vLook ȸ�� ����
		XMStoreFloat4(&pVertices[i].vRight, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vRight), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vUp, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vUp), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vLook, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vLook), RotationMatrix));

		// ��ƼŬ Y�� Ƣ��
		pVertices[i].vTranslation.y += m_fjumpSpeed[i] * fTimeDelta;   // Y�� ��ġ �̵�
		m_fjumpSpeed[i] -= fgravity * fTimeDelta;                      // �߷¿� ���� �ӵ� ����

		// X, Z ���� �̵� ó��
		_vector vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);

		// ���� �� ����
		if (pVertices[i].vTranslation.y <= m_vPivotPos.y)        // �ٴڿ� ������
		{
			pVertices[i].vTranslation.y = m_vPivotPos.y;         // Y�� ���� (�� �̻� Ƣ�� ����)
			m_fjumpSpeed[i] = 0.f;

			// ������ (���� �ӵ��� 1/3 ��� XZ �̵�)
			XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta * 0.33f);
		}
		else
		{
			// ���߿� ���� �� XZ �̵�
			XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);
		}

		// ���� ó�� (���� �ڵ�)
		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
			m_fjumpSpeed[i] = 5.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Mesh_Instance::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// �������� �⺻ �ϰ� ���� (Y��)
		_vector vMoveDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

		// �¿�� ��Ǯ�Ÿ��� ���� ���� (X��)
		float fSwingX = sinf(pVertices[i].vLifeTime.y * 3.0f); // �¿� ���� ��
		_vector vSwingDirX = XMVectorSet(fSwingX, 0.f, 0.f, 0.f);

		// �յڷ� �����̴� ���� (Z��, �ణ�� ���� ������ �߰�)
		float fSwingZ = cosf(pVertices[i].vLifeTime.y * 2.0f); // �յ� ���� ��
		_vector vSwingDirZ = XMVectorSet(0.f, 0.f, fSwingZ, 0.f);

		// �������� ��ü �̵�: ���� �ϰ� + �¿� ��鸲 + �յ� ��鸲
		_vector vTotalMove = (XMVector3Normalize(vMoveDir) + vSwingDirX + vSwingDirZ) * m_pSpeed[i] * fTimeDelta;

		// ���� ��ġ�� �̵� ���� ����
		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + vTotalMove);

		// ȸ�� ȿ�� �߰�: �������� �����Ӱ� ȸ���ϵ��� (Z�� ȸ��)
		float fRotationAngle = fTimeDelta * (1.0f + (rand() % 3)); // �ұ�Ģ�� ȸ�� �ӵ�
		_matrix RotationMatrix = XMMatrixRotationZ(fRotationAngle);

		// vRight, vUp, vLook ���͸� ȸ����Ŵ (�ұ�Ģ�ϰ� ȸ��)
		XMStoreFloat4(&pVertices[i].vRight, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vRight), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vUp, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vUp), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vLook, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vLook), RotationMatrix));

		// ���� �ð� ������Ʈ
		pVertices[i].vLifeTime.y += fTimeDelta;

		// ���� ������ ���� �������� �� �������� �ʱ� ���·� �ǵ���
		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;

			// �ʱ� ���� ����
			pVertices[i].vRight = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vRight;
			pVertices[i].vUp = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vUp;
			pVertices[i].vLook = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vLook;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Mesh_Instance::Bind_Texture(CShader* pShader, const _char* pConstantName)
{
	return m_pTextureCom->Bind_ShadeResource(pShader, pConstantName, 0);
}

HRESULT CVIBuffer_Mesh_Instance::Read_File(const _tchar* pModelFilePath)
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

		//if (m_eType == TYPE_NONANIM)

		{
			pBinMash->pNonAnimVertices = new VTXMESH[pBinMash->NumVertices];
			pBinMash->pAnimVertices = nullptr;
			for (int j = 0; j < pBinMash->NumVertices; ++j)
			{
				VTXMESH* VtxNonAniModel = &pBinMash->pNonAnimVertices[j];
				fread(VtxNonAniModel, sizeof(VTXMESH), 1, file);
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

	}
	m_pBin_Scene = Scene;
	fclose(file);

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Safe_Release_Scene()
{
	Safe_Delete_Array(m_pBin_Scene->pBinNodes);
	Safe_Delete_Array(m_pBin_Scene->pBinMaterial);

	for (_int i = 0; i < m_pBin_Scene->iMeshCount; ++i)
	{
		Safe_Delete_Array(m_pBin_Scene->pBinMesh[i].pAnimVertices);
		Safe_Delete_Array(m_pBin_Scene->pBinMesh[i].pNonAnimVertices);
		Safe_Delete_Array(m_pBin_Scene->pBinMesh[i].pIndices);
		//Safe_Delete_Array(m_pBin_Scene->pBinMesh[i].pBones);
	}
	Safe_Delete_Array(m_pBin_Scene->pBinMesh);

	Safe_Delete(m_pBin_Scene);

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Ready_VertexBuffer_NonAnim(const DATA_BINMESH* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXMESH);
	m_iNumVertices = pAIMesh->NumVertices;
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i] = pAIMesh->pNonAnimVertices[i];
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));
	}

	ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitialData.pSysMem = pVertices;

  	if (FAILED(__super::Create_Buffer(&m_pVB)))
 		return E_FAIL;


	Safe_Delete_Array(pVertices);

	//IndexBuffer

	m_iNumIndices = pAIMesh->iNumIndices;
	m_iIndexCountPerInstance = m_iNumIndices;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = sizeof(_ushort);

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	_uint		iNumIndices = { 0 };
	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = pAIMesh->pIndices[i];
	}


	ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitialData.pSysMem = pIndices;

	/* �ε������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Ready_Materials(const _tchar* pModelFilePath)
{
	if (nullptr == m_pBin_Scene)
		return E_FAIL;

	for (_uint i = 0; i < (_uint)m_pBin_Scene->iMaterialCount; ++i)
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
			m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, szWideFullPath, 1);
			if (nullptr == m_pTextureCom)
			{
				MSG_BOX(szWideFullPath);
				continue; //return E_FAIL;
			}
		}
	}
	return S_OK;
}


CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix, const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Mesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_Mesh_Instance::Clone(void* pArg)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Mesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Release(m_pTextureCom);
	}
	else
	{
		Safe_Delete_Array(m_fjumpSpeed);
	}
}
