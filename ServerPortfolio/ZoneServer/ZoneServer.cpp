#include "ZoneServer.h"

ZoneServer::~ZoneServer()
{
	if(m_fieldManager != nullptr) delete m_fieldManager;
	if(m_heartBeatThread != nullptr) delete m_heartBeatThread;

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

	User* user;
	for (int i = 0; i < USERMAXCOUNT; i++)
	{
		TRYCATCH(user = new User());
		m_sessionManager.AddObject(user);
	}

	MYDEBUG("[ User Max Count : %d ]\n", m_sessionManager.GetObjectPool()->GetSize());

	if (!DBCONNECTOR->Connect("211.221.147.29", 30003))
	{
		return false;
	}
	m_IOCPClass.AddSocket(DBCONNECTOR->GetSocket(),
		(unsigned long long)DBCONNECTOR);
	DBCONNECTOR->OnConnect();

	TRYCATCH(m_fieldManager = new FieldManager());
	if (m_fieldManager->IsFailed()) return false;

	TRYCATCH(m_heartBeatThread = new HeartBeatThread(m_sessionManager));

	MainThread::getSingleton()->
		SetManagers(&m_sessionManager, m_fieldManager);

	return true;
}