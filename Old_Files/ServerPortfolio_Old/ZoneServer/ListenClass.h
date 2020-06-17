#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

#include "IOCPClass.h"

#include "Session.h"
#include "SessionManager.h"

class ListenClass : public Thread<ListenClass>
{
private:
	IpEndPoint					m_ipEndPoint;

	//WSADATA						m_wsaData;
	SOCKET						m_listenSocket;
	SOCKADDR_IN					m_servAddr;

	int							m_portNum;

	IOCPClass*					m_IOCPClass;
	SessionManager*				m_sessionManager;

public:
	ListenClass();
	~ListenClass();

	bool Init(IOCPClass* _IOCPClass, SessionManager* _sessionManager, const char* _ip, const unsigned short _portNum);

	void LoopRun();
};