#include "RPGServer.h"

RPGServer::RPGServer()
{

}

RPGServer::~RPGServer()
{

}

void RPGServer::Init()
{
	User* user;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		user = new User();
		user->Init();

		m_objectPool.AddObject(user);
	}

	printf("[ User Max Count : %d ]\n", USERMAXCOUNT);

	MYSQLCLASS->Init();

	if (MYSQLCLASS->Connect("localhost", "root", "135andromeda803", "rpg", 3306, NULL, 0))
	{
		if (!MYSQLCLASS->SelectDB("rpg"))
		{
			printf("[ DB Connect Success ]\n");
		}
		else
		{
			printf("[ DB Connect Failed ]\n");
		}
	}

	m_IOCPClass.Init();
	m_sessionManager.Init(&m_objectPool);
	m_zoneManager.Init();
	ServerLogicThread::getSingleton()->Init(&m_sessionManager, &m_zoneManager);
	m_heartBeatThread.Init(&m_sessionManager);
	//m_listenClass.Init(&m_IOCPClass, &m_sessionManager, "192.168.0.13", 30002);
	m_listenClass.Init(&m_IOCPClass, &m_sessionManager, "127.0.0.1", 30002);

	WaitForSingleObject(m_listenClass.GetHandle(), INFINITE);
}