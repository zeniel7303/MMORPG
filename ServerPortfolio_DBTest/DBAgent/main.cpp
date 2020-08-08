//라이브러리 추가
#pragma comment (lib, "../x64/Debug/1.ServerLibrary.lib")

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "Acceptor.h"
#include "DBAgent.h"
#include "MainThread.h"

WSADATA				m_wsaData;

IOCPClass*			iocpClass;
Acceptor*			acceptor;

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

	MainThread::getSingleton()->SetManagers();

	WaitForSingleObject(MainThread::getSingleton()->GetHandle(), INFINITE);

	delete acceptor;
	delete iocpClass;

	return 0;
}