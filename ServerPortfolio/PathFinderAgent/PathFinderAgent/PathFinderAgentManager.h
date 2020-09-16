#pragma once
#define _WINSOCKAPI_

#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"

#include "PathFinderAgent.h"

class PathFinderAgentManager
{
private:
	std::vector<PathFinderAgent*>		m_agentVec;
	ObjectPool<PathFinderAgent>			m_objectPool;
	Manager_List<PathFinderAgent>		m_agentList;

public:
	PathFinderAgentManager();
	~PathFinderAgentManager();

	void AddObject(PathFinderAgent* _t);
	void CopyToObjectPool();

	PathFinderAgent* PopAgent();
	void AddAgent(PathFinderAgent* _t);
	void ReturnAgent(PathFinderAgent* _t);

	ObjectPool<PathFinderAgent>* GetObjectPool() { return &m_objectPool; }
	Manager_List<PathFinderAgent>* GetAgentList() { return &m_agentList; }
};

