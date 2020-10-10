#pragma once
#pragma warning(disable:4005)

#include <winsock2.h>

#include "../HeaderFiles/FileLog.h"
#include "../HeaderFiles/OnlyHeaders/Thread.h"

class WorkerThread : public Thread<WorkerThread>
{
private:
	HANDLE			m_hIocp;

public:
	WorkerThread(HANDLE& _iocpHandle);
	~WorkerThread();

	void LoopRun();
};