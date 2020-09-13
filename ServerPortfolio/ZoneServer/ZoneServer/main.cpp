//라이브러리 추가
#ifdef _DEBUG
#pragma comment (lib, "../x64/Debug/_ServerLibrary.lib")
#else
#pragma comment (lib, "../x64/Release/_ServerLibrary.lib")
#endif

#include <memory>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "Acceptor.h"

#include "ZoneServer.h"

WSADATA m_wsaData;

IOCPClass* iocpClass;
Acceptor* acceptor;

int num;
int portNum;

int main()
{
	std::cout << "<< 몇 번째 Zone인지 입력(0부터) >>" << endl;
	std::cin >> num;

	switch (num)
	{
	case 0:
		portNum = 30006;
		break;
	case 1:
		portNum = 30007;
		break;
	case 2:
		portNum = 30008;
		break;
	case 3:
		portNum = 30009;
		break;
	case 4:
		portNum = 30010;
		break;
	case 5:
		portNum = 30011;
		break;
	case 6:
		portNum = 30012;
		break;
	case 7:
		portNum = 30013;
		break;
	case 8:
		portNum = 30014;
		break;
	case 9:
		portNum = 30015;
		break;
	default:
		return 0;
		break;
	}

	//port는 30006부터

	system("cls");

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");

		return 0;
	}

	TRYCATCH(iocpClass = new IOCPClass());
	if (iocpClass->IsFailed()) return false;

	ZoneServer* zoneServer = new ZoneServer(*iocpClass);
	if (!zoneServer->Start(num))
	{
		MYDEBUG("[ Zone Server Initializing Fail ]\n");

		WSACleanup();

		return 0;
	}

	TRYCATCH(acceptor = new Acceptor("192.168.0.13", portNum,
		iocpClass->GetIOCPHandle(), 0));
	if (acceptor->IsFailed()) return false;
	iocpClass->Associate(acceptor->GetListenSocket(),
		(unsigned long long)acceptor->GetListenSocket());

	MYDEBUG("[ 존 서버 Port : %d ]\n", portNum);

	WaitForSingleObject(MainThread::getSingleton()->GetHandle(), INFINITE);

	delete acceptor;
	delete zoneServer;
	delete iocpClass;

	WSACleanup();

	return 0;
}