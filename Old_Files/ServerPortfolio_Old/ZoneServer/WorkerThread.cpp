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
	Thread<WorkerThread>::Start(this);

	return true;
}

void WorkerThread::LoopRun()
{
	DWORD bytes = NULL;
	Session* pSession = nullptr;
	ST_OVERLAPPED* overlapped = nullptr;

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
			if (overlapped == nullptr)
			{
				printf("[INFO] OVERLAPPED IS NULL\n");
				continue;
			}

			if (overlapped->session == nullptr)
			{
				printf("[INFO] SESSION IS NULL\n");
				continue;
			}
			
			switch (overlapped->flag)
			{
			case IOCP_ACCEPT:
				break;
			case IOCP_RECV:
				break;
			case IOCP_SEND:
				break;
			}

			overlapped->session->CheckCompletion(overlapped, bytes);
		}
		else
		{
			printf("[INFO] GetQueuedCompletionStatus Failure\n");

			if (overlapped == nullptr)
			{
				printf("[INFO] OVERLAPPED IS NULL\n");
				continue;
			}

			if (overlapped->session == nullptr)
			{
				printf("[INFO] SESSION IS NULL\n");
				continue;
			}

			overlapped->session->Disconnect();
		}
	}
}