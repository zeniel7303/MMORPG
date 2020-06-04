#include "ZoneServer.h"

ZoneServer::ZoneServer()
{

}

ZoneServer::~ZoneServer()
{
	
}

void ZoneServer::Init()
{
	User* user;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		user = new User();
		user->Init();

		m_objectPool.AddObject(user);
	}

	printf("[ User Max Count : %d ]\n", USERMAXCOUNT);

	m_IOCPClass.Init();

	DBCONNECTOR->Init("211.221.147.29", 30003);
	DBCONNECTOR->Connect();
	m_IOCPClass.AddSocket(DBCONNECTOR->GetSocket(),
		(unsigned long long)DBCONNECTOR);
	DBCONNECTOR->OnConnect(DBCONNECTOR->GetSocket());

	m_sessionManager.Init(&m_objectPool);
	m_fieldManager.Init();
	ServerLogicThread::getSingleton()->Init(&m_sessionManager,
		&m_fieldManager);

	m_heartBeatThread.Init(&m_sessionManager);
	m_listenClass.Init(&m_IOCPClass, &m_sessionManager, "192.168.0.13", 30002);

	WaitForSingleObject(m_listenClass.GetHandle(), INFINITE);
}