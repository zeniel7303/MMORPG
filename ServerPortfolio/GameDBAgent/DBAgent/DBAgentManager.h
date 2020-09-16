#pragma once
#include "DBAgent.h"

#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"

class DBAgentManager
{
private:
	std::vector<DBAgent*>		m_agentVec;
	ObjectPool<DBAgent>			m_objectPool;
	Manager_List<DBAgent>		m_agentList;

public:
	DBAgentManager();
	~DBAgentManager();

	void AddObject(DBAgent* _t);
	void CopyToObjectPool();

	DBAgent* PopAgent();
	void AddAgent(DBAgent* _t);
	void ReturnAgent(DBAgent* _t);

	ObjectPool<DBAgent>* GetObjectPool() { return &m_objectPool; }
	Manager_List<DBAgent>* GetAgentList() { return &m_agentList; }
};

