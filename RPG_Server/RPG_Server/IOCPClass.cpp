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
	//thread 생성
	for (int i = 0; i < m_threadCount; i++)
	{
		WorkerThread* workerThread = new WorkerThread(m_hIOCP);
		if (!workerThread->Init())
		{
			printf("IOCP WORKER THREAD 생성 실패 ]\n");
			return false;
		}

		m_workThreadBuffer[i] = workerThread;
	}

	printf("IOCP WORKER THREAD 생성 성공 ]\n");

	return true;
}

void IOCPClass::Reset()
{
	for (int i = 0; i < m_threadCount; i++)
	{
		WorkerThread* workerThread = m_workThreadBuffer[i];
	}
}

//동기화 해야하나? 사방에서 접근할듯
//현재 Accept가 동기이므로 필요없을것 같다.
void IOCPClass::AddSocket(SOCKET _socket, unsigned long long _value)
{
	m_hIOCP = CreateIoCompletionPort(
		(HANDLE)_socket,
		m_hIOCP,
		_value,
		0);
}