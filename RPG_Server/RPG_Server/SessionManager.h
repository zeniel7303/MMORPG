#pragma once
#include <algorithm>
#include <set>

#include "Session.h"
#include "ObjectPool.h"
#include "ManagerFrame.h"
#include "CriticalSectionClass.h"

//=====================================================

//SessionManager

//Session들을 관리하기위한 매니저

//=====================================================

class SessionManager : public Manager<Session>
{
private:
	CriticalSectionClass	m_locker;

	ObjectPool<Session>		m_objectPool;

	set<int>				m_idSet;

public:
	SessionManager() {}
	~SessionManager() {}

	void Init(ObjectPool<Session>* _pool);
	Session* CreateSession();
	void DeleteSession(Session* _t);
	void AddSessionList(Session* _t);

	void AddSessionID(int _num);
	void DeleteSessionID(int _num);
	bool FindSessionID(int _num);
};