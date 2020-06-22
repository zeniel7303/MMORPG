#pragma once
#include "IOCPClass.h"
#include "ListenClass.h"
#include "HeartBeatThread.h"
#include "SessionManager.h"

#include "User.h"
#include "FieldManager.h"
#include "ServerLogicThread.h"

//#define USERMAXCOUNT 1000

class ZoneServer
{
private:
	WSADATA					m_wsaData;

	IOCPClass*				m_IOCPClass;
	ListenClass*			m_listenClass;
	SessionManager*			m_sessionManager;
	FieldManager*			m_fieldManager;
	HeartBeatThread*		m_heartBeatThread;

	const int				USERMAXCOUNT = 2000;

public:
	ZoneServer();
	~ZoneServer();

	bool Init();
};

