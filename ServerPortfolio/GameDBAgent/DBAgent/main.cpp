//라이브러리 추가

#ifdef _DEBUG
#pragma comment (lib, "../../ZoneServer/x64/Debug/_ServerLibrary.lib")
#else
#pragma comment (lib, "../../ZoneServer/x64/Release/_ServerLibrary.lib")
#endif

#include "../../ZoneServer/ServerLibrary/HeaderFiles/FileLog.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/Utils.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/IOCPClass.h"

#include "Acceptor.h"
#include "DBAgent.h"
#include "DBAgentManager.h"
#include "MainThread.h"

WSADATA				m_wsaData;

IOCPClass*			iocpClass;
Acceptor*			acceptor;

DBAgentManager*		dbAgentManager;

int main()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");

		return 0;
	}

	TRYCATCH(iocpClass = new IOCPClass());
	if (iocpClass->IsFailed()) return false;

	TRYCATCH(acceptor = new Acceptor("192.168.0.13", 30003,
		iocpClass->GetIOCPHandle(), 0));
	if (acceptor->IsFailed()) return false;
	iocpClass->Associate(acceptor->GetListenSocket(),
		(unsigned long long)acceptor->GetListenSocket());

	if (!MainThread::getSingleton()->Init())
	{
		return false;
	}

	TRYCATCH(dbAgentManager = new DBAgentManager());

	DBAgent* dbAgent;
	for (int i = 0; i < 100; i++)
	{
		TRYCATCH(dbAgent = new DBAgent());
		dbAgentManager->AddObject(dbAgent);
	}

	MYDEBUG("[ Max Count : %d ]\n", dbAgentManager->GetObjectPool()->GetSize());

	MainThread::getSingleton()->SetManagers(dbAgentManager);

	WaitForSingleObject(MainThread::getSingleton()->GetHandle(), INFINITE);

	delete acceptor;
	delete iocpClass;

	return 0;
}