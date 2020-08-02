#pragma once
#include <set>

#include "User.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_UnorderedMap.h"
#include "../ServerLibrary/HeaderFiles/CriticalSection.h"

//=====================================================

//UserManager

//User들을 관리하기위한 매니저

//=====================================================

class UserManager : public Manager_UnorderedMap<User>, public Manager_List<User>
{
private:
	CRITICAL_SECTION		m_cs;

	ObjectPool<User>		m_objectPool;

public:
	UserManager();
	~UserManager();

	void AddObject(User* _t);

	User* PopUser();
	void AddUser(User* _t);
	void ReturnUser(User* _t);

	void AddTempuser(User* _t);
	void DeleteTempUser(User* _t, bool _returnBool);

	ObjectPool<User>* GetObjectPool() { return &m_objectPool; }
};