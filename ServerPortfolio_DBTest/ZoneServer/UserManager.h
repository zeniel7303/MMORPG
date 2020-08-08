#pragma once
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
	ObjectPool<User>			m_objectPool;
	Manager_UnorderedMap<User>	m_userHashMap;
	Manager_List<User>			m_tempUserList;

public:
	UserManager()
	{

	}
	~UserManager()
	{
		while (m_objectPool.GetSize() != 0)
		{
			delete m_objectPool.PopObject();
		}
	}

	void AddObject(User* _t)
	{
		m_objectPool.AddObject(_t);
	}

	User* PopUser()
	{
		return m_objectPool.PopObject(); //안에서 lock하는중
	}

	void AddUser(User* _t)
	{
		m_userHashMap.AddItem(_t->GetInfo()->userInfo.userID, _t);
	}

	void ReturnUser(User* _t)
	{
		if (m_userHashMap.DeleteItem(_t->GetInfo()->userInfo.userID))
		{
			_t->Reset();

			m_objectPool.ReturnObject(_t);
		}
	}

	//=============================================================
	void AddTempuser(User* _t)
	{
		m_tempUserList.AddItem(_t);
	}

	void DeleteTempUser(User* _t, bool _returnBool)
	{
		if (m_tempUserList.DeleteItem(_t))
		{
			if (_returnBool)
			{
				m_objectPool.ReturnObject(_t);
			}
		}
	}

	ObjectPool<User>* GetObjectPool() { return &m_objectPool; }
	Manager_UnorderedMap<User>* GetUserHashMap() { return &m_userHashMap; }
	Manager_List<User>* GetUserList() { return &m_tempUserList; }
};