#pragma once
#include <chrono>
#include <algorithm>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

#include "User.h"
#include "UserManager.h"
#include "DBConnector.h"

class HeartBeatThread : public Thread<HeartBeatThread>
{
private:
	int												m_cycle;

	UserManager&									m_userManager;

	std::chrono::high_resolution_clock::time_point	m_end;
	std::chrono::seconds 							m_durationSec;

public:
	HeartBeatThread(UserManager& _userManager, int _cycle);
	~HeartBeatThread();

	void LoopRun();

	void HeartBeat();
};

