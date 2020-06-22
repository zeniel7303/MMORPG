#include "WorkerThread.h"
#include "Session.h"

WorkerThread::WorkerThread(HANDLE& _iocpHandle)
{
	m_hIocp = _iocpHandle;
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
	Acceptor::ST_OVERLAPPED* overlapped = nullptr;
	SOCKET* socket = nullptr;

	while (1)
	{
		bool CheckGQCS;

		if (CheckGQCS = GetQueuedCompletionStatus(
			m_hIocp,
			&bytes,
			reinterpret_cast<PULONG_PTR>(&socket),
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

			Session* tempSession = overlapped->session;
			overlapped->bytes = bytes;

			switch (overlapped->state)
			{
			case Acceptor::IO_ACCEPT:
				tempSession->AssociateIOCP(m_hIocp);
				tempSession->OnConnect();
				break;
			case Acceptor::IO_RECV:
				tempSession->CheckCompletion(overlapped);
				break;
			case Acceptor::IO_SEND:
				break;
			}
		}
		else
		{
			//printf("[INFO] GetQueuedCompletionStatus Failure\n");

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