#include "WorkerThread.h"
#include "Session.h"

WorkerThread::WorkerThread(HANDLE& _iocpHandle)
{
	m_hIocpHandle = _iocpHandle;
}

WorkerThread::~WorkerThread()
{

}

bool WorkerThread::Init()
{
	ThreadClass<WorkerThread>::Start(this);

	return true;
}

void WorkerThread::LoopRun()
{
	DWORD bytes;
	Session* pSession = nullptr;
	ST_OVERLAPPED* overlapped;

	while (1)
	{
		bool CheckGQCS;

		if (CheckGQCS = GetQueuedCompletionStatus(
			m_hIocpHandle,
			&bytes,
			reinterpret_cast<PULONG_PTR>(&pSession),
			reinterpret_cast<LPOVERLAPPED*>(&overlapped),
			INFINITE))
		{
			if (overlapped->session == nullptr) printf("[INFO] SESSION IS NULL\n");

			overlapped->session->CheckCompletion(CheckGQCS, overlapped, bytes);
		}
	}
}