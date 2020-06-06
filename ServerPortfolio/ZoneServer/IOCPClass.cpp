#include "IOCPClass.h"

IOCPClass::IOCPClass()
{

}

IOCPClass::~IOCPClass()
{

}

bool IOCPClass::Init()
{
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	m_threadCount = (systemInfo.dwNumberOfProcessors * 2) + 1;
	printf("[ Thread Count : %d ,", m_threadCount);

	m_workThreadBuffer = new WorkerThread*[m_threadCount];
	//thread 积己
	for (int i = 0; i < m_threadCount; i++)
	{
		WorkerThread* workerThread = new WorkerThread(m_hIOCP);
		if (!workerThread->Init())
		{
			printf("IOCP WORKER THREAD 积己 角菩 ]\n");
			return false;
		}

		m_workThreadBuffer[i] = workerThread;
	}

	printf("IOCP WORKER THREAD 积己 己傍 ]\n");

	return true;
}

void IOCPClass::Reset()
{
	for (int i = 0; i < m_threadCount; i++)
	{
		WorkerThread* workerThread = m_workThreadBuffer[i];
	}
}

void IOCPClass::AddSocket(SOCKET _socket, unsigned long long _value)
{
	m_hIOCP = CreateIoCompletionPort(
		reinterpret_cast <HANDLE>(_socket),
		m_hIOCP,
		_value,
		0);
}