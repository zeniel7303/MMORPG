#pragma once
#include <list>

#include "DBConnector.h"

class DB_Agent;

class DBConnectorManager
{
private:
	std::list<DBConnector*> m_DBConnectorList;
	DB_Agent* m_DBAgent;

public:
	DBConnectorManager();
	~DBConnectorManager();

	void Init(DB_Agent* _agent);

	std::list<DBConnector*> GetConnectorList() { return m_DBConnectorList; }
};

