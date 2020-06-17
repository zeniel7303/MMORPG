#include "ZoneServer.h"

ZoneServer::ZoneServer()
{

}

ZoneServer::~ZoneServer()
{
	
}

void ZoneServer::Start()
{
	ServerLogicThread::getSingleton()->Init();

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ]\n");
	}

	m_IOCPClass.Init();
	m_sessionManager.Init();
	m_IOCPClass.SetSessionManager(&m_sessionManager);

	m_listenClass.Init(&m_IOCPClass, &m_sessionManager, "192.168.0.13", 30002);

	User* user;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		user = new User(*ServerLogicThread::getSingleton()->GetSessionDoubleQueue());
		user->Init(m_listenClass.GetListenSocket(), 
			ServerLogicThread::getSingleton()->GetEventHandle(0));

		m_sessionManager.AddObject(user);
	}

	printf("[ User Max Count : %d ]\n", m_sessionManager.GetObjectPool()->GetSize());

	m_sessionManager.CheckingAccept();

	m_IOCPClass.AddSocket(m_listenClass.GetListenSocket(), 
		(unsigned long long)m_listenClass.GetListenSocket());

	DBCONNECTOR->Init("211.221.147.29", 30003, 
		ServerLogicThread::getSingleton()->GetEventHandle(1));
	DBCONNECTOR->Connect();
	m_IOCPClass.AddSocket(DBCONNECTOR->GetSocket(),
		(unsigned long long)DBCONNECTOR);
	DBCONNECTOR->OnConnect();

	m_fieldManager.Init();
	m_heartBeatThread.Init(&m_sessionManager);
	ServerLogicThread::getSingleton()->
		GetManagers(&m_sessionManager, &m_fieldManager);

	WaitForSingleObject(ServerLogicThread::getSingleton()->GetHandle(), INFINITE);
}