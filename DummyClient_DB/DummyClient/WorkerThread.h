#pragma once
#include "ThreadClass.h"
#include "Session.h"

class WorkerThread : public ThreadClass<WorkerThread>
{
private:
	HANDLE			m_hIocpHandle;

public:
	WorkerThread(HANDLE& _iocpHandle);
	~WorkerThread();

	bool Init();
	void LoopRun();
};

