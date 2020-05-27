#pragma once
#include "ThreadClass.h"
#include "IpEndPoint.h"

#include "IOCPClass.h"

#include "Session.h"

class ListenClass : public ThreadClass<ListenClass>
{
private:
	IpEndPoint					m_ipEndPoint;

	WSADATA						m_wsaData;
	SOCKET						m_listenSocket;
	SOCKADDR_IN					m_servAddr;

	int							m_portNum;

	IOCPClass*					m_IOCPClass;
	Session*					m_session;

public:
	ListenClass();
	~ListenClass();

	bool Init(IOCPClass* _IOCPClass, Session* _session, const char* _ip, const unsigned short _portNum);

	void LoopRun();
};