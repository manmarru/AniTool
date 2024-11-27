#include "..\Public\VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect & Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; //동적버퍼로 생성할거임
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOSTEX*			pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort*		pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Rect::Set_Scale(_float _fScale)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(SubResource.pData);

	pVertices[0].vPosition = { -_fScale, _fScale, 0.f };
	pVertices[1].vPosition = { _fScale, _fScale, 0.f };
	pVertices[2].vPosition = { _fScale, -_fScale, 0.f };
	pVertices[3].vPosition = { -_fScale, -_fScale, 0.f };
	
	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Rect * CVIBuffer_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Rect*		pInstance = new CVIBuffer_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent * CVIBuffer_Rect::Clone(void * pArg)
{
	CVIBuffer_Rect*		pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();

}
