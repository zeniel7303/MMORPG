#include "LogInSessionManager.h"

LogInSessionManager::LogInSessionManager()
{
}

LogInSessionManager::~LogInSessionManager()
{
	for (const auto& element : m_sessionList.GetItemList())
	{
		delete element;
	}

	for (const auto& element : m_sessionHashMap.GetItemHashMap())
	{
		delete element.second;
	}

	m_objectPool.~ObjectPool();

	for (const auto& element : m_sessionVec)
	{
		delete element;
	}

	m_sessionVec.clear();
	m_sessionVec.resize(0);
}

void LogInSessionManager::AddObject(LogInSession* _t)
{
	m_sessionVec.emplace_back(_t);

	//m_objectPool.AddObject(_t);
}

void LogInSessionManager::CopyToObjectPool()
{
	for (const auto& element : m_sessionVec)
	{
		m_objectPool.AddObject(element);
	}
}

LogInSession* LogInSessionManager::PopSession()
{
	return m_objectPool.PopObject(); //안에서 lock하는중
}

void LogInSessionManager::AddSession(LogInSession* _t)
{
	m_sessionHashMap.AddItem(_t->GetIdx(), _t);
}

void LogInSessionManager::ReturnSession(LogInSession* _t)
{
	if (m_sessionHashMap.DeleteItem(_t->GetIdx()))
	{
		_t->Reset();

		m_objectPool.ReturnObject(_t);
	}
}

void LogInSessionManager::AddTempSession(LogInSession* _t)
{
	m_sessionList.AddItem(_t);
}

void LogInSessionManager::DeleteTempSession(LogInSession* _t, bool _returnBool)
{
	if (m_sessionList.DeleteItem(_t))
	{
		if (_returnBool)
		{
			_t->Reset();

			m_objectPool.ReturnObject(_t);
		}
	}
}