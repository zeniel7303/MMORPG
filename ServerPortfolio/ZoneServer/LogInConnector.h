#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/ClientSession.h"

#include "packet.h"

#include "MainThread.h"

#include <vector>

class LogInConnector : public ClientSession
{
public:
	static LogInConnector* getSingleton()
	{
		static LogInConnector singleton;

		return &singleton;
	}

private:
	IpEndPoint					m_ipEndPoint;

	LogInConnector();
public:	
	~LogInConnector();

	bool Connect(const char* _ip, const unsigned short _portNum);
	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();
};

