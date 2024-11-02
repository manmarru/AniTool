#include "stdafx.h"
#include "..\Public\Pooling_Manager.h"

#include "GameInstance.h"
#include <fstream>
#include "GameObject.h"


IMPLEMENT_SINGLETON(CPooling_Manager)

CPooling_Manager::CPooling_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	m_pObjects = new POOLINGOJECTS[LEVEL_END];
}

void CPooling_Manager::Add_Pooling_Object(LEVELID iLevelIndex, const _tchar* pLayerTag, CGameObject* pGameObject)
{
	list< class CGameObject*>* pLayer = Find_PoolingObjects(iLevelIndex, pLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = new LAYERS;
		pLayer->push_back(pGameObject);
		m_pObjects[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		m_pObjects[iLevelIndex].find(pLayerTag)->second->push_back(pGameObject);
	}

}

void CPooling_Manager::Add_Pooling_Layer(LEVELID iLevelIndex, const _tchar* pLayerTag)
{
	list< class CGameObject*>* pLayerList = m_pGameInstance->Get_ObjectList(iLevelIndex, pLayerTag);
	if (pLayerList == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	list< class CGameObject*>* pLayer = Find_PoolingObjects(iLevelIndex, pLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = new LAYERS;
	}

	for (auto& iter : *pLayerList)
		pLayer->push_back(iter);

	m_pObjects[iLevelIndex].emplace(pLayerTag, pLayer);
	m_pGameInstance->Out_GameObjectList(iLevelIndex, pLayerTag);
}

_bool CPooling_Manager::Reuse_Pooling_Layer(LEVELID iLevelIndex, const _tchar* pLayerTag, void* pArg)
{
	list< class CGameObject*>* pLayer = Find_PoolingObjects(iLevelIndex, pLayerTag);
	if (pLayer == nullptr)
	{
		return false;
	}

	for (auto& iter = pLayer->begin(); iter != pLayer->end();)
	{
		if (pArg != nullptr)
			(*iter)->Re_Initialize(pArg);

		m_pGameInstance->ReAdd_GameObject(iLevelIndex, pLayerTag, *iter);

		iter = pLayer->erase(iter);
	}

	return true;
}

_bool CPooling_Manager::Reuse_Pooling_Object(LEVELID iLevelIndex, const _tchar* pLayerTag, void* pArg)
{
	list< class CGameObject*>* pLayer = Find_PoolingObjects(iLevelIndex, pLayerTag);
	if (pLayer == nullptr || pLayer->size() == 0)
	{
		return false;
	}

	for (auto& iter = pLayer->begin(); iter != pLayer->end();)
	{
		if (pArg != nullptr)
			(*iter)->Re_Initialize(pArg);

		m_pGameInstance->ReAdd_GameObject(iLevelIndex, pLayerTag, *iter);

		iter = pLayer->erase(iter);
		break;
	}

	return true;
}

_bool CPooling_Manager::Clear_AllPooling_Layer()
{
	for (int i = 0; i < LEVEL_END; ++i)
	{
		for (auto& iter : m_pObjects[i])
		{
			for (auto& vec = iter.second->begin(); vec != iter.second->end();)
			{
				Safe_Release(*vec);
				vec = iter.second->erase(vec);
			}
			iter.second->clear();
			Safe_Delete(iter.second);
		}
		m_pObjects[i].clear();
	}
	Safe_Delete_Array(m_pObjects);

	return true;
}


list< class CGameObject*>* CPooling_Manager::Find_PoolingObjects(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (iLevelIndex >= LEVEL_END)
		return nullptr;

	auto	iter = find_if(m_pObjects[iLevelIndex].begin(), m_pObjects[iLevelIndex].end(), CTag_Finder(pLayerTag));
	if (iter == m_pObjects[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

_bool CPooling_Manager::Find_LayerTag(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (iLevelIndex >= LEVEL_END)
		return false;

	auto	iter = find_if(m_pObjects[iLevelIndex].begin(), m_pObjects[iLevelIndex].end(), CTag_Finder(pLayerTag));
	if (iter == m_pObjects[iLevelIndex].end())
		return nullptr;

	return true;
}

void CPooling_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
