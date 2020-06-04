#include "SessionManager.h"

void SessionManager::Init(ObjectPool<Session>* _pool)
{
	InitializeCriticalSection(&m_cs);

	m_objectPool = *_pool;

	if (m_objectPool.GetCount() <= 0)
	{
		printf("[ SessionManager Init Failed ] \n");
	}
	else
	{
		printf("[ SessionManager Init Success ] \n");
	}
}

Session* SessionManager::CreateSession()
{
	return m_objectPool.PopObject(); //안에서 lock하는중
}

void SessionManager::DeleteSession(Session* _t)
{
	CSLock csLock(m_cs);

	if (DeleteItem(_t))
	{
		m_objectPool.ReturnObject(_t);
	}
}

void SessionManager::AddSessionList(Session* _t)
{
	CSLock csLock(m_cs);

	AddItem(_t);
}

void SessionManager::AddSessionID(int _num)
{
	CSLock csLock(m_cs);

	m_idSet.insert(_num); 
}

void SessionManager::DeleteSessionID(int _num)
{
	CSLock csLock(m_cs);

	if (FindSessionID(_num))
	{
		m_idSet.erase(_num);
	}
}

bool SessionManager::FindSessionID(int _num)
{
	CSLock csLock(m_cs);

	set<int>::iterator FindIter = m_idSet.find(_num);

	if (FindIter != m_idSet.end())
	{
		return true;
	}

	return false;
}