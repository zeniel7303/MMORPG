#pragma once
#include "stdafx.h"

//=====================================================

//MangerFrame

//매니저들의 기본적인 기능들을 가지고있는 탬플릿 클래스

//=====================================================

template<typename T>
class Manager_List
{
protected:
	list<T*> m_itemList;

public:
	Manager_List() {}
	~Manager_List() {
		m_itemList.clear();
		m_itemList.resize(0);
	}

	void AddItem(T* _t);
	bool DeleteItem(T* _t);

	list<T*>* GetItemList();

	bool FindItem(T* _t);
};

template<class T>
void Manager_List<T>::AddItem(T * _t)
{
	m_itemList.push_back(_t);
}

template<class T>
bool Manager_List<T>::DeleteItem(T * _t)
{
	typename list<T*>::iterator deleteiter =
		find(m_itemList.begin(), m_itemList.end(), _t);

	if (deleteiter == m_itemList.end()) return false;

	m_itemList.erase(deleteiter);

	return true;
}

template<class T>
list<T*>* Manager_List<T>::GetItemList()
{
	return &m_itemList;
}

template<class T>
bool Manager_List<T>::FindItem(T* _t)
{
	typename list<T*>::iterator iter =
		find(m_itemList.begin(), m_itemList.end(), _t);

	if (iter == m_itemList.end()) return false;

	return true;
}