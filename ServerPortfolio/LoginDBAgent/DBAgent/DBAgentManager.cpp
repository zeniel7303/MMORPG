#include "DBAgentManager.h"

DBAgentManager::DBAgentManager()
{

}

DBAgentManager::~DBAgentManager()
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

void DBAgentManager::AddObject(DBAgent* _t)
{
	m_agentVec.emplace_back(_t);

	//m_objectPool.AddObject(_t);
}

void DBAgentManager::CopyToObjectPool()
{
	for (const auto& element : m_agentVec)
	{
		m_objectPool.AddObject(element);
	}
}

DBAgent* DBAgentManager::PopAgent()
{
	return m_objectPool.PopObject(); //안에서 lock하는중
}

void DBAgentManager::AddAgent(DBAgent* _t)
{
	m_agentList.AddItem(_t);
}

void DBAgentManager::ReturnAgent(DBAgent* _t)
{
	if (m_agentList.DeleteItem(_t))
	{
		m_objectPool.ReturnObject(_t);
	}
}