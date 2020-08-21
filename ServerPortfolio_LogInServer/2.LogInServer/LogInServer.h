#pragma once

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "LogInSession.h"
#include "LogInSessionManager.h"

#include "HeartBeatThread.h"

#include "MainThread.h"
#include "DBConnector.h"

class LogInServer
{
private:
	const int				USERMAXCOUNT = 2000;

	IOCPClass&				m_IOCPClass;

	LogInSessionManager*	m_logInSessionManager;

	HeartBeatThread*		m_heartBeatThread;

public:
	LogInServer(IOCPClass& _iocpClass)
	: m_IOCPClass(_iocpClass)
	{

	};
	~LogInServer();

	bool Start();
};

