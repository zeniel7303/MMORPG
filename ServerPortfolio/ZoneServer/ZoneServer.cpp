#include "ZoneServer.h"

ZoneServer::ZoneServer()
{

}

ZoneServer::~ZoneServer()
{
	delete m_IOCPClass;
	delete m_sessionManager;
	delete m_listenClass;
	delete m_fieldManager;
	delete m_heartBeatThread;

	ServerLogicThread::getSingleton()->releaseSingleton();
	DBCONNECTOR->releaseSingleton();
}

bool ZoneServer::Init()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ]\n");
	}

	if (!ServerLogicThread::getSingleton()->Init())
	{
		return false;
	}

	try
	{
		m_IOCPClass = new IOCPClass();
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return false;
	}
	if (!m_IOCPClass->Init())
	{
		return false;
	}

	try
	{
		m_sessionManager = new SessionManager();
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return false;
	}
	m_sessionManager->Init();

	try
	{
		m_listenClass = new ListenClass();
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return false;
	}
	if (!m_listenClass->Init("192.168.0.13", 30002))
	{
		return false;
	}

	User* user;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		try
		{
			user = new User();
		}
		catch (const std::bad_alloc& error)
		{
			printf("bad alloc : %s\n", error.what());
			return false;
		}
		
		user->Init(m_listenClass->GetListenSocket());
		m_sessionManager->AddObject(user);
	}

	printf("[ User Max Count : %d ]\n", m_sessionManager->GetObjectPool()->GetSize());

	m_IOCPClass->AddSocket(m_listenClass->GetListenSocket(), 
		(unsigned long long)m_listenClass->GetListenSocket());

	m_sessionManager->Start(m_sessionManager);
	m_sessionManager->CheckingAccept();

	DBCONNECTOR->Init("211.221.147.29", 30003);
	DBCONNECTOR->Connect();
	m_IOCPClass->AddSocket(DBCONNECTOR->GetSocket(),
		(unsigned long long)DBCONNECTOR);
	DBCONNECTOR->OnConnect();

	try
	{
		m_fieldManager = new FieldManager();
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return false;
	}
	if (!m_fieldManager->Init())
	{
		return false;
	}

	try
	{
		m_heartBeatThread = new HeartBeatThread();
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return false;
	}	
	m_heartBeatThread->Init(m_sessionManager);

	ServerLogicThread::getSingleton()->
		GetManagers(m_sessionManager, m_fieldManager);

	WaitForSingleObject(ServerLogicThread::getSingleton()->GetHandle(), INFINITE);

	return true;
}