//라이브러리 추가
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#pragma comment (lib, "../x64/Debug/_ServerLibrary.lib")
#else
#pragma comment (lib, "../x64/Release/_ServerLibrary.lib")
#endif

#include <memory>
#include <fstream>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "Acceptor.h"

#include "ZoneServer.h"

WSADATA m_wsaData;

IOCPClass* iocpClass;
ZoneServer* zoneServer;
Acceptor* acceptor;

int num;
int portNum;

int main()
{
	char tmp[256];
	ifstream readFile;
	readFile.open("zoneNum.txt");

	if (readFile.is_open())
	{
		readFile.getline(tmp, 256);
	}

	int num = atoi(tmp);
	portNum = num + 30006;

	readFile.close();

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");

		return 0;
	}

	TRYCATCH(iocpClass = new IOCPClass());
	if (iocpClass->IsFailed()) return false;

	TRYCATCH(zoneServer = new ZoneServer(*iocpClass));
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

	//_CrtDumpMemoryLeaks();

	return 0;
}