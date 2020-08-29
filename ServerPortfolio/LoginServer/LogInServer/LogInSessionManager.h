#pragma once
#include "LogInSession.h"

#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_UnorderedMap.h"

class LogInSessionManager
{
private:
	ObjectPool<LogInSession>		m_objectPool;
	Manager_UnorderedMap<LogInSession>	m_sessionHashMap;
	Manager_List<LogInSession>		m_sessionList;

public:
	LogInSessionManager();
	~LogInSessionManager();

	void AddObject(LogInSession* _t);

	LogInSession* PopSession();
	void AddSession(LogInSession* _t);
	void ReturnSession(LogInSession* _t);

	void AddTempSession(LogInSession* _t);
	void DeleteTempSession(LogInSession* _t, bool _returnBool);

	ObjectPool<LogInSession>* GetObjectPool() { return &m_objectPool; }
	Manager_UnorderedMap<LogInSession>* GetSessionHashMap() { return &m_sessionHashMap; }
	Manager_List<LogInSession>* GetSessionList() { return &m_sessionList; }
};

