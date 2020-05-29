#pragma once
#include "singleton.h"
#include "CriticalSectionClass.h"

template <class T>
class SharedQueue : public singletonBase<SharedQueue<T>>
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
void SharedQueue<T>::AddItem(T _item)
{
	CSLock csLock(m_lock.cs);

	//m_lock.EnterLock();

	m_itemQueue.push(_item);

	//m_lock.LeaveLock();
}

template<class T>
T& SharedQueue<T>::GetItem(void)
{
	CSLock csLock(m_lock.cs);

	T& item = m_itemQueue.front();
	m_itemQueue.pop();

	return item;

	/*m_lock.EnterLock();

	T& item = m_itemQueue.front();
	m_itemQueue.pop();

	m_lock.LeaveLock();

	return item;*/
}

template<class T>
bool SharedQueue<T>::IsEmpty(void)
{
	return m_itemQueue.empty();
}