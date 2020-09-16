#pragma once
#define _WINSOCKAPI_
#include "User.h"

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_UnorderedMap.h"

//=====================================================

//UserManager

//User들을 관리하기위한 매니저

//cpp파일까지쓰면 에러 폭발한다. 이유가 뭐지?

//=====================================================

class UserManager
{
private:
	//유저 데이터 관리를 위해 유저 전부 저장해두는 벡터
	std::vector<User*>			m_userVec;
	ObjectPool<User>			m_objectPool;
	Manager_UnorderedMap<User>	m_userHashMap;
	Manager_List<User>			m_tempUserList;

public:
	UserManager();
	~UserManager();

	void AddObject(User* _t);
	void CopyToObjectPool();

	User* PopUser();
	void AddUser(User* _t);
	void ReturnUser(User* _t);

	//=============================================================
	void AddTempuser(User* _t);
	void DeleteTempUser(User* _t, bool _returnBool);

	ObjectPool<User>* GetObjectPool() { return &m_objectPool; }
	Manager_UnorderedMap<User>* GetUserHashMap() { return &m_userHashMap; }
	Manager_List<User>* GetUserList() { return &m_tempUserList; }
};