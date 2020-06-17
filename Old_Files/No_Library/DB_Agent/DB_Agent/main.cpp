#include "DB_Agent.h"
#include "DBConnectorManager.h"

void main()
{
	DB_Agent			dbAgent;
	DBConnectorManager	connectorManager;

	dbAgent.Init();
	connectorManager.Init(&dbAgent);

	dbAgent.GetConnectorManager(&connectorManager);
}