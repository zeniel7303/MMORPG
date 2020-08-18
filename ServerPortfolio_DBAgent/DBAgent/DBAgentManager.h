#pragma once
#include "DBAgent.h"

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"

class DBAgentManager
{
private:
	ObjectPool<DBAgent>			m_objectPool;
	Manager_List<DBAgent>		m_agentList;

public:
	DBAgentManager();
	~DBAgentManager();

	void AddObject(DBAgent* _t);

	DBAgent* PopAgent();
	void AddAgent(DBAgent* _t);
	void ReturnAgent(DBAgent* _t);

	ObjectPool<DBAgent>* GetObjectPool() { return &m_objectPool; }
	Manager_List<DBAgent>* GetAgentList() { return &m_agentList; }
};

