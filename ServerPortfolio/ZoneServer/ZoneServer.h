#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

#include "IOCPClass.h"
#include "UserManager.h"
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

	UserManager*			m_userManager;
	FieldManager*			m_fieldManager;
	HeartBeatThread*		m_heartBeatThread;

public:
	ZoneServer(IOCPClass& _iocpClass)
		: m_IOCPClass(_iocpClass)
	{

	};

	~ZoneServer();

	bool Start();
};

