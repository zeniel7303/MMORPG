#pragma once
#include <chrono>
#include <ctime> 
#include <algorithm>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

#include "User.h"
#include "UserManager.h"
#include "DBConnector.h"

class HeartBeatThread : public Thread<HeartBeatThread>
{
private:
	UserManager&								m_userManager;

	std::chrono::system_clock::time_point		m_end;
	std::chrono::seconds 						m_durationSec;

public:
	HeartBeatThread(UserManager& _userManager);
	~HeartBeatThread();

	void LoopRun();

	void HeartBeat();
};

