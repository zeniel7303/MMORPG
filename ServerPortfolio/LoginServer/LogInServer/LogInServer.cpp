#include "LogInServer.h"

LogInServer::~LogInServer()
{
	if (m_logInSessionManager != nullptr) delete m_logInSessionManager;
	if (m_heartBeatThread != nullptr) delete m_heartBeatThread;

	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "로그인 서버 구동 종료");
}

bool LogInServer::Start(int _num)
{
	MYDEBUG("[ %d 로그인 서버 구동 시작 ]\n", _num);
	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "로그인 서버 구동 시작");

	if (!MainThread::getSingleton()->Init())
	{
		return false;
	}

	TRYCATCH(m_logInSessionManager = new LogInSessionManager());

	LogInSession* logInSession;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		TRYCATCH(logInSession = new LogInSession());
		m_logInSessionManager->AddObject(logInSession);
	}

	m_logInSessionManager->CopyToObjectPool();

	MYDEBUG("[ Max Count : %d ]\n", m_logInSessionManager->GetObjectPool()->GetSize());

	if (!DBConnector::getSingleton()->Connect("211.221.147.29", 30002))
	{
		return false;
	}
	m_IOCPClass.Associate(DBConnector::getSingleton()->GetSocket(),
		(unsigned long long)DBConnector::getSingleton());
	DBConnector::getSingleton()->OnConnect();

	TRYCATCH(m_heartBeatThread = new HeartBeatThread(*m_logInSessionManager, 10));

	MainThread::getSingleton()->SetManagers(m_logInSessionManager, m_heartBeatThread);

	return true;
}