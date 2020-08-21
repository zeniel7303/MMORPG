#pragma once
#include <chrono>
#include <algorithm>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

#include "LogInSession.h"
#include "LogInSessionManager.h"
#include "DBConnector.h"

class HeartBeatThread : public Thread<HeartBeatThread>
{
private:
	int												m_cycle;

	LogInSessionManager&							m_sessionManager;

	std::chrono::high_resolution_clock::time_point	m_end;
	std::chrono::seconds 							m_durationSec;

public:
	HeartBeatThread(LogInSessionManager& _sessionManager, int _cycle);
	~HeartBeatThread();

	void LoopRun();

	void HeartBeat();
};

