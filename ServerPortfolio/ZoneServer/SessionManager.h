#pragma once
#include <set>

#include "Session.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"
#include "../ServerLibrary/HeaderFiles/CriticalSection.h"

//=====================================================

//SessionManager

//Session들을 관리하기위한 매니저

//=====================================================

class SessionManager : public Manager_List<Session>, public Thread<SessionManager>
{
private:
	CRITICAL_SECTION		m_cs;

	ObjectPool<Session>		m_objectPool;
	set<int>				m_idSet;

	LONG					m_checkingSessionsNum;

	HANDLE					m_hEvent;

public:
	SessionManager();
	~SessionManager();

	void Init();

	void AddObject(Session* _t);

	Session* PopSession();
	void AddSessionList(Session* _t);
	void ReturnSessionList(Session* _t);
	void AddSessionID(int _num);
	void DeleteSessionID(int _num);
	bool FindSessionID(int _num);

	void CheckingAccept();

	void LoopRun();

	ObjectPool<Session>* GetObjectPool() { return &m_objectPool; }
};