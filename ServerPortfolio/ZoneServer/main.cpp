//라이브러리 추가
#pragma comment (lib, "../x64/Debug/ServerLibrary.lib")

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"

#include "IOCPClass.h"
#include "Acceptor.h"
#include "SessionManager.h"

#include "ZoneServer.h"

WSADATA m_wsaData;
SOCKET m_listenSocket;
IpEndPoint m_ipEndPoint;

IOCPClass* iocpClass;
SessionManager* sessionManager;
Acceptor* acceptor;

int main()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");

		return 0;
	}

	TRYCATCH(iocpClass = new IOCPClass());
	if (iocpClass->IsFailed()) return false;

	TRYCATCH(sessionManager = new SessionManager());

	ZoneServer* zoneServer = new ZoneServer(*iocpClass, *sessionManager);
	if (!zoneServer->Start())
	{
		printf("[ Zone Server Initializing Fail ]\n");

		WSACleanup();

		return 0;
	}

	TRYCATCH(acceptor = new Acceptor("192.168.0.13", 30002,
		iocpClass->GetIOCPHandle(), 0));
	if (acceptor->IsFailed()) return false;
	iocpClass->AddSocket(acceptor->GetListenSocket(),
		(unsigned long long)acceptor->GetListenSocket());

	WaitForSingleObject(MainThread::getSingleton()->GetHandle(), INFINITE);

	delete acceptor;
	delete zoneServer;
	delete sessionManager;
	delete iocpClass;

	WSACleanup();

	return 0;
}