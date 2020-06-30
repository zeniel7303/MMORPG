#include "SessionManager.h"

SessionManager::SessionManager()
{
	InitializeCriticalSection(&m_cs);
}

SessionManager::~SessionManager()
{
	DeleteCriticalSection(&m_cs);

	while (m_objectPool.GetSize() != 0)
	{
		delete m_objectPool.PopObject();
	}

	m_idSet.clear();
}

void SessionManager::AddObject(Session* _t)
{
	m_objectPool.AddObject(_t);
}

Session* SessionManager::PopSession()
{
	return m_objectPool.PopObject(); //안에서 lock하는중
}

void SessionManager::AddSessionList(Session* _t)
{
	AddItem(_t);
}

void SessionManager::ReturnSessionList(Session* _t)
{
	if (DeleteItem(_t))
	{
		m_objectPool.ReturnObject(_t);
	}
}

void SessionManager::AddSessionID(int _num)
{
	m_idSet.insert(_num);
}

void SessionManager::DeleteSessionID(int _num)
{
	if (FindSessionID(_num))
	{
		m_idSet.erase(_num);
	}
}

bool SessionManager::FindSessionID(int _num)
{
	set<int>::iterator FindIter = m_idSet.find(_num);

	if (FindIter != m_idSet.end())
	{
		return true;
	}

	return false;
}