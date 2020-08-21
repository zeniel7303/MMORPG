#include "LogInSessionManager.h"

LogInSessionManager::LogInSessionManager()
{
}

LogInSessionManager::~LogInSessionManager()
{
	while (m_objectPool.GetSize() != 0)
	{
		delete m_objectPool.PopObject();
	}
}

void LogInSessionManager::AddObject(LogInSession* _t)
{
	m_objectPool.AddObject(_t);
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