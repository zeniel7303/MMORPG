#pragma once
#include "CriticalSectionClass.h"

template <class T>
class SharedQueue
{
private:
	std::queue<T>				m_itemQueue;
	CriticalSectionClass		m_lock;
public:
	SharedQueue() {}
	~SharedQueue() {}

public:
	void			AddItem(T _item);
	T&				GetItem(void);

	bool			IsEmpty(void);

	int				GetSize(void) { return m_itemQueue.size(); }
};

template<class T>
inline void SharedQueue<T>::AddItem(T _item)
{
	CSLock csLock(m_lock.cs);

	m_itemQueue.push(_item);
}

template<class T>
inline T& SharedQueue<T>::GetItem(void)
{
	CSLock csLock(m_lock.cs);

	T& item = m_itemQueue.front();
	m_itemQueue.pop();

	return item;
}

template<class T>
inline bool SharedQueue<T>::IsEmpty(void)
{
	return m_itemQueue.empty();
}