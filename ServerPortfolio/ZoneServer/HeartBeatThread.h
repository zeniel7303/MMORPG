#pragma once
#include <chrono>
#include <algorithm>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

#include "User.h"
#include "SessionManager.h"
#include "DBConnector.h"

class HeartBeatThread : public Thread<HeartBeatThread>
{
private:
	SessionManager&	m_sessionManager;

	std::chrono::system_clock::time_point			m_end;
	std::chrono::duration<double>					m_duration;

public:
	HeartBeatThread(SessionManager& _sessionManager);
	~HeartBeatThread();

	void LoopRun();

	void HeartBeat(Packet* _packet);
};

