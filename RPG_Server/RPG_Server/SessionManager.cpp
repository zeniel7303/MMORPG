#include "SessionManager.h"

void SessionManager::Init(ObjectPool<Session>* _pool)
{
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
	m_locker.EnterLock();

	if (DeleteItem(_t))
	{
		m_objectPool.ReturnObject(_t);
	}

	m_locker.LeaveLock();
}

void SessionManager::AddSessionList(Session* _t)
{
	m_locker.EnterLock();

	AddItem(_t);

	m_locker.LeaveLock();
}

void SessionManager::AddSessionID(int _num)
{
	m_locker.EnterLock();

	m_idSet.insert(_num); 

	m_locker.LeaveLock();
}

void SessionManager::DeleteSessionID(int _num)
{
	m_locker.EnterLock();

	if (FindSessionID(_num))
	{
		m_idSet.erase(_num);
	}

	m_locker.LeaveLock();
}

bool SessionManager::FindSessionID(int _num)
{
	m_locker.EnterLock(); 

	set<int>::iterator FindIter = m_idSet.find(_num);

	if (FindIter != m_idSet.end())
	{
		m_locker.LeaveLock();

		return true;
	}

	m_locker.LeaveLock();

	return false;
}