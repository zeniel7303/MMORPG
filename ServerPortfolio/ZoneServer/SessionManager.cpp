#include "SessionManager.h"

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
	DeleteCriticalSection(&m_cs);

	if (m_hEvent) { CloseHandle(m_hEvent); m_hEvent = 0; }

	while (m_objectPool.GetSize() != 0)
	{
		delete m_objectPool.PopObject();
	}

	m_idSet.clear();
}

void SessionManager::Init()
{
	InitializeCriticalSection(&m_cs);

	m_checkingSessionsNum = 0;

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
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

	m_checkingSessionsNum -= 1;
	SetEvent(m_hEvent);
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

void SessionManager::CheckingAccept()
{
	Session* tempSession;

	while (m_checkingSessionsNum < 10)
	{
		tempSession = m_objectPool.PopObject();

		//오브젝트 풀에 더이상 남아있는 Session이 없다.
		if (tempSession == nullptr)
		{
			printf("[ 접속 대기 세션 생성 추가 불가능 - 남은 Session이 없음 ]");

			break;
		}

		tempSession->StartAccept();
		m_checkingSessionsNum += 1;
		//InterlockedIncrement(&m_checkingSessionsNum);
	}

	if (m_checkingSessionsNum > 0)
	{
		printf("[ 접속 대기 세션의 수 : %d ] \n", m_checkingSessionsNum);
	}
	else
	{
		printf("[ 포화 - 접속 대기 세션의 수 : 0 ]\n");
	}
}

void SessionManager::LoopRun()
{
	while (1)
	{
		WaitForSingleObject(m_hEvent, INFINITE);

		CheckingAccept();
	}
}