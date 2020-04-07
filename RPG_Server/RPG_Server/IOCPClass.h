#pragma once
#include "stdafx.h"

#include "WorkerThread.h"
#include "CriticalSectionClass.h"

//어차피 하나만 쓰는데 싱글톤 써보자
class IOCPClass
{
private:
	HANDLE			m_hIOCP;
	unsigned int	m_threadId;

	int				m_threadCount;
	WorkerThread**	m_workThreadBuffer;

	CriticalSectionClass m_locker;

public:
	IOCPClass();
	~IOCPClass();

	bool Init();
	void Reset();
	void AddSocket(SOCKET _socket, unsigned long long _value);
};