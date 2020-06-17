#pragma once
#include "IOCPClass.h"
#include "ListenClass.h"
#include "HeartBeatThread.h"
#include "SessionManager.h"

#include "ObjectPool.h"

#include "User.h"
#include "FieldManager.h"
#include "ConnectorClass.h"

#include "ServerLogicThread.h"

#define USERMAXCOUNT 2000

class ZoneServer
{
private:
	IOCPClass				m_IOCPClass;
	ListenClass				m_listenClass;

	ObjectPool<Session>		m_objectPool;
	SessionManager			m_sessionManager;

	FieldManager			m_fieldManager;

	HeartBeatThread			m_heartBeatThread;

public:
	ZoneServer();
	~ZoneServer();

	void Init();
};

