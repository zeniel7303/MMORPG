#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "SessionManager.h"
#include "Acceptor.h"

class WorkerThread : public Thread<WorkerThread>
{
private:
	HANDLE			m_hIocp;
	SessionManager* m_sessionManager;

public:
	WorkerThread(HANDLE& _iocpHandle);
	~WorkerThread();

	bool Init();
	void LoopRun();

	void SetSessionManager(SessionManager* _sessionManager)
						{ m_sessionManager = _sessionManager; };
};