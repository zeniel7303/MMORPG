#pragma once
#include <algorithm>

#include "User.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "SessionManager.h"
#include "DBConnector.h"

class HeartBeatThread : public Thread<HeartBeatThread>
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

