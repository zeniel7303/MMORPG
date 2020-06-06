#pragma once
#include "../ServerLibrary/HeaderFiles/SubHeaders/IpEndPoint.h"

#include "Session.h"

class Connector : public Session
{
private:
	IpEndPoint m_ipEndPoint;

public:
	Connector();
	~Connector();

	void Init(const char* _ip, const unsigned short _portNum);
	bool Connect();
	void OnConnect(SOCKET _socket);
};

