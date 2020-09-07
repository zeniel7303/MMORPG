#include "DBAgentManager.h"

DBAgentManager::DBAgentManager()
{

}

DBAgentManager::~DBAgentManager()
{
	while (m_objectPool.GetSize() != 0)
	{
		delete m_objectPool.PopObject();
	}
}

void DBAgentManager::AddObject(DBAgent* _t)
{
	m_objectPool.AddObject(_t);
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