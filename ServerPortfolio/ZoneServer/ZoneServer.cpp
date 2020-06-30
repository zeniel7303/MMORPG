#include "ZoneServer.h"

ZoneServer::ZoneServer()
{
}

ZoneServer::~ZoneServer()
{
	if(m_IOCPClass != nullptr) delete m_IOCPClass;
	if(m_sessionManager != nullptr) delete m_sessionManager;
	/*for (int i = 0; i < ACCEPTORCOUNT; i++)
	{
		if(m_acceptorSession[i] != nullptr) delete m_acceptorSession[i];
	}*/
	if(m_acceptorSession != nullptr) delete m_acceptorSession;
	if(m_fieldManager != nullptr) delete m_fieldManager;
	if(m_heartBeatThread != nullptr) delete m_heartBeatThread;

	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "서버 구동 종료");
}

bool ZoneServer::Start()
{
	MYDEBUG("서버 구동 시작\n");
	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "서버 구동 시작");

	if (!SetUp()) return false;

	if (!ServerLogicThread::getSingleton()->Init())
	{
		return false;
	}

	TRYCATCH(m_IOCPClass = new IOCPClass());
	if (m_IOCPClass->IsFailed()) return false;

	TRYCATCH(m_sessionManager = new SessionManager());

	User* user;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		TRYCATCH(user = new User());
		if (user->IsFailed()) return false;
		m_sessionManager->AddObject(user);
	}

	MYDEBUG("[ User Max Count : %d ]\n", m_sessionManager->GetObjectPool()->GetSize());

	/*for (int i = 0; i < ACCEPTORCOUNT; i++)
	{
		TRYCATCH(m_acceptorSession[i] = new AcceptorSession(m_listenSocket, &m_ipEndPoint,
			m_IOCPClass->GetIOCPHandle(), i));
		if (m_acceptorSession[i]->IsFailed()) return false;

		m_IOCPClass->AddSocket(m_acceptorSession[i]->GetListenSocket(),
			(unsigned long long)m_acceptorSession[i]->GetListenSocket());
	}*/

	TRYCATCH(m_acceptorSession = new AcceptorSession(m_listenSocket, &m_ipEndPoint, 
		m_IOCPClass->GetIOCPHandle(), 0));
	if (m_acceptorSession->IsFailed()) return false;

	m_IOCPClass->AddSocket(m_acceptorSession->GetListenSocket(),
		(unsigned long long)m_acceptorSession->GetListenSocket());

	//싱글톤은 어떻게 Init 없애지?
	DBCONNECTOR->Init("211.221.147.29", 30003);
	DBCONNECTOR->Connect();
	m_IOCPClass->AddSocket(DBCONNECTOR->GetSocket(),
		(unsigned long long)DBCONNECTOR);
	DBCONNECTOR->OnConnect();

	TRYCATCH(m_fieldManager = new FieldManager());
	if (m_fieldManager->IsFailed()) return false;

	TRYCATCH(m_heartBeatThread = new HeartBeatThread(m_sessionManager));

	ServerLogicThread::getSingleton()->
		GetManagers(m_sessionManager, m_fieldManager);

	WaitForSingleObject(ServerLogicThread::getSingleton()->GetHandle(), INFINITE);

	return true;
}

bool ZoneServer::SetUp()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");
	}

	m_ipEndPoint = IpEndPoint("192.168.0.13", 30002);

	m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		WSACleanup();
		MYDEBUG("[ Failed socket() ]\n");
		LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[ Failed socket() ]");
		return false;
	}

	//앞에 :: 붙인 이유
	//https://stackoverflow.com/questions/44861571/operator-error
	if (bind(m_listenSocket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MYDEBUG("[ Binding Error ]\n");
		LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[ Binding Error ]");
		closesocket(m_listenSocket);
		WSACleanup();

		return false;
	}

	if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		MYDEBUG("[ Listening Error ]\n");
		LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[ Listening Error ]");
		closesocket(m_listenSocket);
		WSACleanup();

		return false;
	}

	return true;
}