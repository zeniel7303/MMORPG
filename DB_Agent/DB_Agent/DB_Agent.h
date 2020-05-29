#pragma once
#include "IOCPClass.h"
#include "ListenClass.h"
#include "Session.h"
#include "DBConnectorManager.h"

#include "SharedQueue.h"

#include "CriticalSectionClass.h"

class DB_Agent : public Session
{
private:
	HANDLE					hEvent;
	HANDLE					hThread[2];
	unsigned int			threadID[2];

	IOCPClass				m_IOCPClass;
	ListenClass				m_listenClass;

	DBConnectorManager*		m_connectorManager;

	CriticalSectionClass	m_locker;

	SharedQueue<Packet*>	m_recvSharedQueue;
	SharedQueue<Packet*>	m_sendSharedQueue;

public:
	DB_Agent();
	~DB_Agent();

	void Init();
	void GetConnectorManager(DBConnectorManager* _connectorManager);

	void Parsing();

	void Thread_1();
	void Thread_2();

	static unsigned int __stdcall ThreadFunc_1(void* pArgs)
	{
		DB_Agent* obj = (DB_Agent*)pArgs;
		obj->Thread_1();
		return 0;
	}

	static unsigned int __stdcall ThreadFunc_2(void* pArgs)
	{
		DB_Agent* obj = (DB_Agent*)pArgs;
		obj->Thread_2();
		return 0;
	}
	
	SharedQueue<Packet*>* GetSendSharedQueue() { return &m_sendSharedQueue; }
};

