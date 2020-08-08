//#include "UserManager.h"
//
//UserManager::UserManager()
//{
//	
//}
//
//UserManager::~UserManager()
//{
//	while (m_objectPool.GetSize() != 0)
//	{
//		delete m_objectPool.PopObject();
//	}
//}
//
//void UserManager::AddObject(User* _t)
//{
//	m_objectPool.AddObject(_t);
//}
//
//User* UserManager::PopUser()
//{
//	return m_objectPool.PopObject(); //안에서 lock하는중
//}
//
//void UserManager::AddUser(User* _t)
//{
//	m_userHashMap.AddItem(_t->GetInfo()->userInfo.userID, _t);
//}
//
//void UserManager::ReturnUser(User* _t)
//{
//	if (m_userHashMap.DeleteItem(_t->GetInfo()->userInfo.userID))
//	{
//		_t->Reset();
//
//		m_objectPool.ReturnObject(_t);
//	}
//}
//
//void UserManager::AddTempuser(User* _t)
//{
//	m_tempUserList.AddItem(_t);
//}
//
//void UserManager::DeleteTempUser(User* _t, bool _returnBool)
//{
//	if (m_tempUserList.DeleteItem(_t))
//	{
//		if (_returnBool)
//		{
//			m_objectPool.ReturnObject(_t);
//		}
//	}
//}