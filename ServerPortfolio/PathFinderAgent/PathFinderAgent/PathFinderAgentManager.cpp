#include "PathFinderAgentManager.h"

PathFinderAgentManager::PathFinderAgentManager()
{
}

PathFinderAgentManager::~PathFinderAgentManager()
{
	while (m_objectPool.GetSize() != 0)
	{
		delete m_objectPool.PopObject();
	}
}

void PathFinderAgentManager::AddObject(PathFinderAgent* _t)
{
	m_objectPool.AddObject(_t);
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