#include "UserManager.h"

UserManager::UserManager()
{

}

UserManager::~UserManager()
{
	for (const auto& element : m_tempUserList.GetItemList())
	{
		delete element;
	}

	for (const auto& element : m_userHashMap.GetItemHashMap())
	{
		delete element.second;
	}

	m_objectPool.~ObjectPool();

	for (const auto& element : m_userVec)
	{
		delete element;
	}

	m_userVec.clear();
	m_userVec.resize(0);
}

void UserManager::AddObject(User* _t)
{
	m_userVec.emplace_back(_t);

	//m_objectPool.AddObject(_t);
}

void UserManager::CopyToObjectPool()
{
	for (const auto& element : m_userVec)
	{
		m_objectPool.AddObject(element);
	}
}

User* UserManager::PopUser()
{
	return m_objectPool.PopObject(); //안에서 lock하는중
}

void UserManager::AddUser(User* _t)
{
	m_userHashMap.AddItem(_t->GetInfo()->userInfo.userID, _t);
}

void UserManager::ReturnUser(User* _t)
{
	if (m_userHashMap.DeleteItem(_t->GetInfo()->userInfo.userID))
	{
		_t->Reset();

		m_objectPool.ReturnObject(_t);
	}
}

void UserManager::AddTempuser(User* _t)
{
	m_tempUserList.AddItem(_t);
}

void UserManager::DeleteTempUser(User* _t, bool _returnBool)
{
	if (m_tempUserList.DeleteItem(_t))
	{
		if (_returnBool)
		{
			_t->Reset();

			m_objectPool.ReturnObject(_t);
		}
	}
}