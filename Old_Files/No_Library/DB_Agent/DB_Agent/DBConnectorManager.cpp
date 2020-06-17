#include "DBConnectorManager.h"

#include "DB_Agent.h"

DBConnectorManager::DBConnectorManager()
{
}


DBConnectorManager::~DBConnectorManager()
{
}

void DBConnectorManager::Init(DB_Agent* _agent)
{
	m_DBAgent = _agent;
	SharedQueue<Packet*>* packetQueue = m_DBAgent->GetSendSharedQueue();
	HANDLE* hEvent = m_DBAgent->GetEventHandle(1);

	DBConnector* dbConnector;
	for (int i = 0; i < 10; i++)
	{
		dbConnector = new DBConnector(*packetQueue, *hEvent);

		dbConnector->Init(i);
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