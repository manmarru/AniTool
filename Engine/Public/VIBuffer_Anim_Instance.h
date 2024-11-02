#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Anim_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Anim_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Anim_Instance(const CVIBuffer_Anim_Instance& Prototype);
	virtual ~CVIBuffer_Anim_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Spread(_float fTimeDelta);
	void SpreadGravity(_float fTimeDelta);
	virtual void Fountain(_float fTimeDelta, _float fgravity);
	virtual void Drop(_float fTimeDelta);

public:
	HRESULT Bind_Texture(class CShader* pShader, const _char* pConstantName);

private:
	HRESULT Read_File(const _tchar* pModelFilePath);
	HRESULT	Safe_Release_Scene();
	HRESULT Ready_VertexBuffer_NonAnim(const DATA_BINMESH* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Materials(const _tchar* pModelFilePath);

private:
	DATA_BINSCENE* m_pBin_Scene = nullptr;
	CTexture* m_pTextureCom = nullptr;

private:
	_float* m_fjumpSpeed = {nullptr};

public:
	static CVIBuffer_Anim_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix, const CVIBuffer_Instancing::INSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END