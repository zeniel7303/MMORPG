#pragma once
#include "IOCPClass.h"
#include "ListenClass.h"
#include "HeartBeatThread.h"
#include "SessionManager.h"

#include "ObjectPool.h"

#include "User.h"
#include "ZoneManager.h"

#include "ServerLogicThread.h"

#define USERMAXCOUNT 2000

class RPGServer
{
private:
	IOCPClass				m_IOCPClass;
	ListenClass				m_listenClass;

	ObjectPool<Session>		m_objectPool;
	SessionManager			m_sessionManager;

	ZoneManager				m_zoneManager;

	HeartBeatThread			m_heartBeatThread;

public:
	RPGServer();
	~RPGServer();

	void Init();
};

