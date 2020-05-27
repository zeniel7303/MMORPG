#pragma once
#include "EventClass.h"
#include "ThreadClass.h"

#include "IOCPClass.h"
#include "ListenClass.h"
#include "Session.h"
#include "DBConnectorManager.h"

#include "SharedQueue.h"

#include "CriticalSectionClass.h"

class DB_Agent : public EventClass<DB_Agent>, public ThreadClass<DB_Agent>, 
	public Session
{
private:
	IOCPClass				m_IOCPClass;
	ListenClass				m_listenClass;

	DBConnectorManager*		m_connectorManager;

	CriticalSectionClass	m_locker;

	SharedQueue<Packet*>		m_recvSharedQueue;
	SharedQueue<Packet*>		m_sendSharedQueue;

public:
	DB_Agent();
	~DB_Agent();

	void Init();
	void GetConnectorManager(DBConnectorManager* _connectorManager);

	void Parsing();

	void EventFunc();
	void LoopRun();
	
	SharedQueue<Packet*>* GetSendSharedQueue() { return &m_sendSharedQueue; }
};

