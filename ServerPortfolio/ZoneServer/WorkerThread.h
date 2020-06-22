#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

class WorkerThread : public Thread<WorkerThread>
{
private:
	HANDLE			m_hIocp;

public:
	WorkerThread(HANDLE& _iocpHandle);
	~WorkerThread();

	bool Init();
	void LoopRun();
};