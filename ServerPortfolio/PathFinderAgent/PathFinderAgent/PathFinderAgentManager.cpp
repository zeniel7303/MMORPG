#include "PathFinderAgentManager.h"

PathFinderAgentManager::PathFinderAgentManager()
{
}

PathFinderAgentManager::~PathFinderAgentManager()
{
	for (const auto& element : m_agentList.GetItemList())
	{
		delete element;
	}

	m_objectPool.~ObjectPool();

	for (const auto& element : m_agentVec)
	{
		delete element;
	}

	m_agentVec.clear();
	m_agentVec.resize(0);
}

void PathFinderAgentManager::AddObject(PathFinderAgent* _t)
{
	m_agentVec.emplace_back(_t);

	//m_objectPool.AddObject(_t);
}

void PathFinderAgentManager::CopyToObjectPool()
{
	for (const auto& element : m_agentVec)
	{
		m_objectPool.AddObject(element);
	}
}

PathFinderAgent* PathFinderAgentManager::PopAgent()
{
	return m_objectPool.PopObject(); //안에서 lock하는중
}

void PathFinderAgentManager::AddAgent(PathFinderAgent* _t)
{
	m_agentList.AddItem(_t);
}

void PathFinderAgentManager::ReturnAgent(PathFinderAgent* _t)
{
	if (m_agentList.DeleteItem(_t))
	{
		m_objectPool.ReturnObject(_t);
	}
}