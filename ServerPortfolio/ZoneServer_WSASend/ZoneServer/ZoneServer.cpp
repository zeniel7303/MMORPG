#include "ZoneServer.h"

ZoneServer::~ZoneServer()
{
	if (m_fieldManager != nullptr) delete m_fieldManager;
	if (m_userManager != nullptr) delete m_userManager;
	DBConnector::getSingleton()->~DBConnector();

	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "존 서버 구동 종료");
}

bool ZoneServer::Start(const int _num)
{
	MYDEBUG("[ %d 존 서버 구동 시작 ]\n", _num);
	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "존 서버 구동 시작");

	if (!MainThread::getSingleton()->Init())
	{
		return false;
	}

	TRYCATCH(m_userManager = new UserManager());

	User* user;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		TRYCATCH(user = new User());
		m_userManager->AddObject(user);
	}

	m_userManager->CopyToObjectPool();

	MYDEBUG("[ User Max Count : %d ]\n", m_userManager->GetObjectPool()->GetSize());

	if (!DBConnector::getSingleton()->Connect("211.221.147.29", 30003, &m_IOCPClass))
	{
		return false;
	}	
	DBConnector::getSingleton()->OnConnect();

	if (!LogInConnector::getSingleton()->Connect("211.221.147.29", 30004, &m_IOCPClass))
	{
		return false;
	}
	LogInConnector::getSingleton()->OnConnect();

	if (!PathFinderAgent::getSingleton()->Connect("211.221.147.29", _num + 30016, &m_IOCPClass))
	{
		return false;
	}
	PathFinderAgent::getSingleton()->OnConnect();

	TRYCATCH(m_fieldManager = new FieldManager());
	if (m_fieldManager->IsFailed()) return false;

	MainThread::getSingleton()->
		SetManagers(m_userManager, m_fieldManager);

	return true;
}