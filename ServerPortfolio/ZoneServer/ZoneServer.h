#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

#include "IOCPClass.h"
#include "SessionManager.h"
#include "HeartBeatThread.h"

#include "User.h"
#include "FieldManager.h"
#include "MainThread.h"

class ZoneServer
{
private:
	const int				USERMAXCOUNT = 2000;
	const int				ACCEPTORCOUNT = 5;

	IOCPClass&				m_IOCPClass;
	SessionManager&			m_sessionManager;

	FieldManager*			m_fieldManager;
	HeartBeatThread*		m_heartBeatThread;

public:
	ZoneServer(IOCPClass& _iocpClass, SessionManager& _sessionManager) 
		: m_IOCPClass(_iocpClass), m_sessionManager(_sessionManager)
	{

	};

	~ZoneServer();

	bool Start();
};

