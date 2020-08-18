#include "LogInServer.h"

LogInServer::~LogInServer()
{
	if (m_logInSessionManager != nullptr) delete m_logInSessionManager;

	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "로그인 서버 구동 종료");
}

bool LogInServer::Start()
{
	MYDEBUG("[ 로그인 서버 구동 시작 ]\n");
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

	MYDEBUG("[ Max Count : %d ]\n", m_logInSessionManager->GetObjectPool()->GetSize());

	if (!DBConnector::getSingleton()->Connect("211.221.147.29", 30004))
	{
		return false;
	}
	m_IOCPClass.Associate(DBConnector::getSingleton()->GetSocket(),
		(unsigned long long)DBConnector::getSingleton());
	DBConnector::getSingleton()->OnConnect();

	MainThread::getSingleton()->SetManagers(m_logInSessionManager);
}