#pragma once
#include <winsock2.h>

#include "WorkerThread.h"
#include "SessionManager.h"

class IOCPClass
{
private:
	HANDLE			m_hIOCP;
	unsigned int	m_threadId;

	int				m_threadCount;
	WorkerThread**	m_workThreadBuffer;

	int				m_conCurrency;

public:
	IOCPClass();
	~IOCPClass();

	bool Init();
	void Reset();
	void AddSocket(SOCKET _socket, unsigned long long _value);

	void SetSessionManager(SessionManager* _sessionManager);
};