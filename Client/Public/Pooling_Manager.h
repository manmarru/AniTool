#pragma once
#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)
class CGameInstance;
class CModel;
class CGameObject;

END

BEGIN(Client)
class CPooling_Manager final : public CBase
{
	DECLARE_SINGLETON(CPooling_Manager)

public:
	CPooling_Manager();
	virtual ~CPooling_Manager() = default;

public:
	void Add_Pooling_Object(LEVELID iLevelIndex, const _tchar* pLayerTag, CGameObject* pgameObject);
	void Add_Pooling_Layer(LEVELID iLevelIndex, const _tchar* pLayerTag);

	_bool Reuse_Pooling_Layer(LEVELID iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	_bool Reuse_Pooling_Object(LEVELID iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	_bool Clear_AllPooling_Layer();

private:
	typedef list< class CGameObject*> LAYERS;

	map<const _tchar*, LAYERS*>* m_pObjects = nullptr;
	typedef map<const _tchar*, LAYERS*>		POOLINGOJECTS;

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	list< class CGameObject*>* Find_PoolingObjects(_uint iLevelIndex, const _tchar* pLayerTag);
	_bool Find_LayerTag(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END

