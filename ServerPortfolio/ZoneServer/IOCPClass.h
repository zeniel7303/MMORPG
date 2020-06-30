#pragma once
#include <winsock2.h>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "WorkerThread.h"
#include "SessionManager.h"

class IOCPClass
{
private:
	bool			m_failed;

	HANDLE			m_hIOCP;

	int				m_threadCount;
	WorkerThread**	m_workThreadBuffer;

	int				m_conCurrency;

public:
	IOCPClass();
	~IOCPClass();

	void Reset();
	void AddSocket(SOCKET _socket, unsigned long long _value);

	bool IsFailed() { return m_failed; }

	HANDLE GetIOCPHandle() { return m_hIOCP; }
};