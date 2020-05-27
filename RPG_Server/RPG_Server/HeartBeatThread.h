#pragma once
#include "User.h"
#include "ThreadClass.h"
#include "SessionManager.h"
#include "ConnectorClass.h"

class HeartBeatThread : ThreadClass<HeartBeatThread>
{
private:
	SessionManager*					m_sessionManager;

public:
	HeartBeatThread();
	~HeartBeatThread();

	void Init(SessionManager* _sessionManager);

	void LoopRun();

	void HeartBeat(Packet* _packet);
};

