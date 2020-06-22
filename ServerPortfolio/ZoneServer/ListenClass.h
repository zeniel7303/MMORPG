#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

#include "Session.h"
#include "SessionManager.h"

class ListenClass
{
private:
	IpEndPoint					m_ipEndPoint;

	SOCKET						m_listenSocket;

	int							m_portNum;

public:
	ListenClass();
	~ListenClass();

	bool Init(const char* _ip, const unsigned short _portNum);

	SOCKET GetListenSocket() { return m_listenSocket; }
};