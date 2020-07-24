#pragma once
#include <set>

#include "Session.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"
#include "../ServerLibrary/HeaderFiles/CriticalSection.h"

//=====================================================

//SessionManager

//Session들을 관리하기위한 매니저

//=====================================================

class SessionManager : public Manager_List<Session>
{
private:
	CRITICAL_SECTION		m_cs;

	ObjectPool<Session>		m_objectPool;
	//DB 프로시저 사용시 빼주자(현재 로그인 중복 체크를 위해서만 사용중)
	//Unordered Mapp 사용할 수도 있음.
	set<int>				m_idSet;

public:
	SessionManager();
	~SessionManager();

	void AddObject(Session* _t);

	Session* PopSession();
	void AddSessionList(Session* _t);
	void ReturnSessionList(Session* _t);
	void AddSessionID(int _num);
	void DeleteSessionID(int _num);
	bool FindSessionID(int _num);

	ObjectPool<Session>* GetObjectPool() { return &m_objectPool; }
};