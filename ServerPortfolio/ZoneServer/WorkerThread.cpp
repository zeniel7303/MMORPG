#include "WorkerThread.h"
#include "Session.h"

WorkerThread::WorkerThread(HANDLE& _iocpHandle)
{
	m_hIocp = _iocpHandle;

	Thread<WorkerThread>::Start(this);
}

WorkerThread::~WorkerThread()
{

}

void WorkerThread::LoopRun()
{
	DWORD bytes = NULL;
	ST_OVERLAPPED* overlapped = nullptr;
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
				MYDEBUG("[INFO] OVERLAPPED IS NULL\n");
				LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[INFO] OVERLAPPED IS NULL");
				continue;
			}

			if (overlapped->session == nullptr)
			{
				MYDEBUG("[INFO] SESSION IS NULL\n");
				LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[INFO] SESSION IS NULL");
				continue;
			}

			Session* tempSession = overlapped->session;
			overlapped->bytes = bytes;

			//Session의 인터페이스
			tempSession->HandleOverlappedIO(overlapped);
		}
		else
		{
			MYDEBUG("[INFO] GetQueuedCompletionStatus Failure\n");
			LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[INFO] GetQueuedCompletionStatus Failure");

			if (overlapped == nullptr)
			{
				MYDEBUG("[INFO] OVERLAPPED IS NULL\n");
				LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[INFO] OVERLAPPED IS NULL");
				continue;
			}

			if (overlapped->session == nullptr)
			{
				MYDEBUG("[INFO] SESSION IS NULL\n");
				LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[INFO] SESSION IS NULL");
				//FILELOG("[INFO] SESSION IS NULL");
				continue;
			}

			overlapped->session->DisConnect();
		}
	}
}