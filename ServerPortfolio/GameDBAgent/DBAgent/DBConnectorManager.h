#pragma once
#include <list>

#include "DBConnector.h"

class DBConnectorManager
{
private:
	std::list<DBConnector*> m_DBConnectorList;

public:
	DBConnectorManager();
	~DBConnectorManager();

	std::list<DBConnector*>& GetConnectorList() { return m_DBConnectorList; }
};

