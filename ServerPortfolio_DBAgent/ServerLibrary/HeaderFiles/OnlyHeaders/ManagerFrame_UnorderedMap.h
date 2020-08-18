#pragma once
#include <unordered_map>

template<typename T>
class Manager_UnorderedMap
{
protected:
	std::unordered_map<int, T*> m_itemHashMap;

public:
	Manager_UnorderedMap() {}
	virtual ~Manager_UnorderedMap()
	{
		m_itemHashMap.clear();
	}

	void AddItem(int _num, T* _t);
	bool DeleteItem(int _num);
	std::unordered_map<int, T*>& GetItemHashMap();

	bool IsExist(int _num);
	T* FindItem(int _num);
};

template<class T>
void Manager_UnorderedMap<T>::AddItem(int _num, T * _t)
{
	m_itemHashMap.emplace(_num, _t);
}

template<class T>
bool Manager_UnorderedMap<T>::DeleteItem(int _num)
{
	return m_itemHashMap.erase(_num);
}

template<class T>
std::unordered_map<int, T*>& Manager_UnorderedMap<T>::GetItemHashMap()
{
	return m_itemHashMap;
}

template<class T>
bool Manager_UnorderedMap<T>::IsExist(int _num)
{
	auto item = m_itemHashMap.find(_num);
	if (item == m_itemHashMap.end())
	{
		return false;
	}

	return true;
}

template<class T>
T* Manager_UnorderedMap<T>::FindItem(int _num)
{
	auto item = m_itemHashMap.find(_num);
	if (item != m_itemHashMap.end())
	{
		return item->second;
	}
	else
	{
		return nullptr;
	}
}