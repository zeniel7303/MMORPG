#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ObjectPool.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_UnorderedMap.h"

//=====================================================

//ItemManager

//임의의 Item들을 관리하기위한 매니저

//=====================================================

template<typename T>
class ItemManager
{
private:
	ObjectPool<T>			m_objectPool;
	Manager_UnorderedMap<T>	m_userHashMap;
	Manager_List<T>			m_tempUserList;

public:
	ItemManager()
	{

	}
	~ItemManager()
	{
		while (m_objectPool.GetSize() != 0)
		{
			delete m_objectPool.PopObject();
		}
	}

	void AddObject(T* _t)
	{
		m_objectPool.AddObject(_t);
	}

	T* PopUser()
	{
		return m_objectPool.PopObject(); //안에서 lock하는중
	}

	void AddUser(T* _t)
	{
		m_userHashMap.AddItem(_t->GetInfo()->userInfo.userID, _t);
	}

	void ReturnUser(T* _t)
	{
		if (m_userHashMap.DeleteItem(_t->GetInfo()->userInfo.userID))
		{
			_t->Reset();

			m_objectPool.ReturnObject(_t);
		}
	}

	//=============================================================
	void AddTempuser(T* _t)
	{
		m_tempUserList.AddItem(_t);
	}

	void DeleteTempUser(T* _t, bool _returnBool)
	{
		if (m_tempUserList.DeleteItem(_t))
		{
			if (_returnBool)
			{
				m_objectPool.ReturnObject(_t);
			}
		}
	}

	ObjectPool<T>* GetObjectPool() { return &m_objectPool; }
	Manager_UnorderedMap<T>* GetUserHashMap() { return &m_userHashMap; }
	Manager_List<T>* GetUserList() { return &m_tempUserList; }
};