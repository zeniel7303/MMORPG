//라이브러리 추가
#ifdef _DEBUG
#pragma comment (lib, "../../ZoneServer/x64/Debug/_ServerLibrary.lib")
#else
#pragma comment (lib, "../../ZoneServer/x64/Release/_ServerLibrary.lib")
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <memory>

#include "../../ZoneServer/ServerLibrary/HeaderFiles/FileLog.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/Utils.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/IOCPClass.h"

#include "Acceptor.h"
#include "ZoneServerAcceptor.h"
#include "LogInServer.h"

WSADATA m_wsaData;

IOCPClass* iocpClass;
Acceptor* acceptor;
ZoneServerAcceptor* zoneServerAcceptor;

int main()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");

		return 0;
	}

	TRYCATCH(iocpClass = new IOCPClass());
	if (iocpClass->IsFailed()) return false;

	LogInServer* logInServer = new LogInServer(*iocpClass);
	if (!logInServer->Start())
	{
		printf("[ LogInServer Initializing Fail ]\n");

		WSACleanup();

		return 0;
	}

	TRYCATCH(zoneServerAcceptor = new ZoneServerAcceptor("192.168.0.13", 30003,
		iocpClass->GetIOCPHandle(), 0));
	if (zoneServerAcceptor->IsFailed()) return false;
	iocpClass->Associate(zoneServerAcceptor->GetListenSocket(),
		(unsigned long long)zoneServerAcceptor->GetListenSocket());

	//211.221.147.29
	TRYCATCH(acceptor = new Acceptor("192.168.0.13", 30004,
		iocpClass->GetIOCPHandle(), 0));
	if (acceptor->IsFailed()) return false;
	iocpClass->Associate(acceptor->GetListenSocket(),
		(unsigned long long)acceptor->GetListenSocket());

	MYDEBUG("[ Accept 준비 완료 ]\n");

	WaitForSingleObject(MainThread::getSingleton()->GetHandle(), INFINITE);

	delete acceptor;
	delete iocpClass;

	WSACleanup();

	return 0;
}