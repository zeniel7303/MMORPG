#include "../HeaderFiles/WorkerThread.h"
#include "../HeaderFiles/Session.h"


#define CONTAINING_RECORD(address, type, field) ((type *)( \
												  (PCHAR)(address) - \
												  (ULONG_PTR)(&((type *)0)->field)))

/*GetQueuedCompletionStatus(pOverlappedIO);
pObjOverlappedIO = CONTAINING_RECORD(pOverlappedIO, CLOverlappedIO, m_overlapped);

CLOverlappedIO::m_overlapped;
*/

WorkerThread::WorkerThread(HANDLE& _iocpHandle)
{
	m_hIocp = _iocpHandle;

	Thread<WorkerThread>::Start(this);
}

WorkerThread::~WorkerThread()
{
	m_hIocp = NULL;
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

			/*CONTAINING_RECORD(overlapped->session, 
				ST_OVERLAPPED, session)->session->SetBytes(bytes);*/

			InterlockedDecrement((LPLONG)&overlapped->count);

			Session* tempSession = overlapped->session;
			tempSession->SetBytes(bytes);

			//Session의 인터페이스
			tempSession->HandleOverlappedIO(overlapped);
		}
		else
		{
			MYDEBUG("[INFO] GetQueuedCompletionStatus Failure\n");
			LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[INFO] GetQueuedCompletionStatus Failure");

			/*int errorNum = WSAGetLastError();
			if (errorNum != 0)
			{
				MYDEBUG("[ %d Error ]\n", errorNum);
			}*/

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

			if (overlapped->count <= 0)
			{
				overlapped->session->DisConnect();
			}
		}
	}
}