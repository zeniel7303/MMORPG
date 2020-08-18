#pragma once
#include "LogInSession.h"

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"

class LogInSessionManager
{
private:
	ObjectPool<LogInSession>		m_objectPool;
	Manager_List<LogInSession>		m_sessionList;

public:
	LogInSessionManager();
	~LogInSessionManager();

	void AddObject(LogInSession* _t);

	LogInSession* PopSession();
	void AddSession(LogInSession* _t);
	void ReturnSession(LogInSession* _t);

	ObjectPool<LogInSession>* GetObjectPool() { return &m_objectPool; }
	Manager_List<LogInSession>* GetSessionList() { return &m_sessionList; }
};

