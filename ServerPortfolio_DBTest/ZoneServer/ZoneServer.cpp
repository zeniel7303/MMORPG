#include "ZoneServer.h"

ZoneServer::~ZoneServer()
{
	if (m_userManager != nullptr) delete m_userManager;
	if (m_fieldManager != nullptr) delete m_fieldManager;
	if (m_heartBeatThread != nullptr) delete m_heartBeatThread;

	LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "존 서버 구동 종료");
}

bool ZoneServer::Start()
{
	MYDEBUG("[ 존 서버 구동 시작 ]\n");
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

	MYDEBUG("[ User Max Count : %d ]\n", m_userManager->GetObjectPool()->GetSize());

	if (!DBCONNECTOR->Connect("211.221.147.29", 30003))
	{
		return false;
	}
	m_IOCPClass.Associate(DBCONNECTOR->GetSocket(),
		(unsigned long long)DBCONNECTOR);
	DBCONNECTOR->OnConnect();

	TRYCATCH(m_fieldManager = new FieldManager());
	if (m_fieldManager->IsFailed()) return false;

	TRYCATCH(m_heartBeatThread = new HeartBeatThread(*m_userManager, 10));

	MainThread::getSingleton()->
		SetManagers(m_userManager, m_fieldManager, m_heartBeatThread);

	return true;
}