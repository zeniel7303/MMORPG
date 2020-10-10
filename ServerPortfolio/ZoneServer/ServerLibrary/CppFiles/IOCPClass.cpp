#include "../HeaderFiles/IOCPClass.h"

IOCPClass::IOCPClass()
{
	m_failed = false;

	m_conCurrency = 0;

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, m_conCurrency);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	m_threadCount = (systemInfo.dwNumberOfProcessors * 2) + 1;
	MYDEBUG("[ Thread Count : %d ]\n", m_threadCount);

	m_workThreadBuffer = new WorkerThread*[m_threadCount];
	//thread 积己
	for (int i = 0; i < m_threadCount; i++)
	{
		WorkerThread* workerThread;
		TRYCATCH_CONSTRUCTOR(workerThread = new WorkerThread(m_hIOCP), m_failed);
		if (m_failed)
		{
			MYDEBUG("IOCP WORKER THREAD 积己 角菩 ]\n");

			return;
		}
		else
		{
			m_workThreadBuffer[i] = workerThread;
		}
	}

	MYDEBUG("IOCP WORKER THREAD 积己 己傍 ]\n");
}

IOCPClass::~IOCPClass()
{
	for (int i = m_threadCount - 1; i > 0; i--)
	{
		delete m_workThreadBuffer[i];
	}

	if (m_hIOCP) { CloseHandle(m_hIOCP); m_hIOCP = 0; }
}

void IOCPClass::Reset()
{
	for (int i = 0; i < m_threadCount; i++)
	{
		WorkerThread* workerThread = m_workThreadBuffer[i];
	}
}

void IOCPClass::Associate(SOCKET _socket, unsigned long long _value)
{
	m_hIOCP = CreateIoCompletionPort(
		reinterpret_cast <HANDLE>(_socket),
		m_hIOCP,
		_value,
		0);

	/*m_hIOCP = CreateIoCompletionPort(
		reinterpret_cast <HANDLE>(_socket),
		m_hIOCP,
		0,
		m_conCurrency);*/
}