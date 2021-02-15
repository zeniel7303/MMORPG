#pragma once
#include <chrono>

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/ClientSession.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "packet.h"

#include "MainThread.h"

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

	IOCPClass*					m_IOCPClass;

	std::chrono::high_resolution_clock::time_point	m_start;

	LogInConnector();
public:	
	~LogInConnector();

	bool Connect(const char* _ip, const unsigned short _portNum, IOCPClass* _iocpClass);
	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();

	void SendDisConnectUser(int _num);
	void HeartBeat();

	void SetStartTime() { m_start = std::chrono::high_resolution_clock::now(); }
};

