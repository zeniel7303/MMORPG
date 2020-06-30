#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

#include "IOCPClass.h"
#include "AcceptorSession.h"
#include "HeartBeatThread.h"
#include "SessionManager.h"

#include "User.h"
#include "FieldManager.h"
#include "ServerLogicThread.h"

class ZoneServer
{
private:
	const int				USERMAXCOUNT = 2000;
	const int				ACCEPTORCOUNT = 5;

	WSADATA					m_wsaData;
	SOCKET					m_listenSocket;
	IpEndPoint				m_ipEndPoint;

	IOCPClass*				m_IOCPClass;
	AcceptorSession*		m_acceptorSession;
	//AcceptorSession*		m_acceptorSession[5];
	SessionManager*			m_sessionManager;
	FieldManager*			m_fieldManager;
	HeartBeatThread*		m_heartBeatThread;

public:
	ZoneServer();
	~ZoneServer();

	bool Start();
	bool SetUp();
};

