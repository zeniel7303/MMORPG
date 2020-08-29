#include "DBConnectorManager.h"

DBConnectorManager::DBConnectorManager()
{
	DBConnector* dbConnector;

	for (int i = 0; i < 20; i++)
	{
		dbConnector = new DBConnector(i);
		dbConnector->Connect("localhost", "root", "135andromeda803", "rpg", 3306, NULL, 0);

		if (!dbConnector->SelectDB("rpg"))
		{
			printf("[ %d Connector - DB Connect Success ]\n", dbConnector->GetNum());
		}
		else
		{
			printf("[ %d Connector - DB Connect Failed ]\n", dbConnector->GetNum());
		}

		m_DBConnectorList.push_back(dbConnector);
	}
}

DBConnectorManager::~DBConnectorManager()
{
}
