#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/ClientSession.h"

#include "packet.h"

#include "MainThread.h"

class DBConnector : public ClientSession
{
public:
	static DBConnector* getSingleton()
	{
		static DBConnector singleton;

		return &singleton;
	}

private:
	IpEndPoint					m_ipEndPoint;

	DBConnector()
	{

	};
public:	
	~DBConnector();

	bool Connect(const char* _ip, const unsigned short _portNum);
	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();
};