#pragma once
#include "IOCPClass.h"
#include "ListenClass.h"
#include "HeartBeatThread.h"
#include "SessionManager.h"

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"

#include "User.h"
#include "FieldManager.h"
#include "ServerLogicThread.h"
#include "DBConnector.h"

#define USERMAXCOUNT 2000

class ZoneServer
{
private:
	WSADATA					m_wsaData;

	IOCPClass				m_IOCPClass;
	ListenClass				m_listenClass;

	ObjectPool<Session>		m_objectPool;
	SessionManager			m_sessionManager;

	FieldManager			m_fieldManager;

	HeartBeatThread			m_heartBeatThread;

public:
	ZoneServer();
	~ZoneServer();

	void Start();
};

