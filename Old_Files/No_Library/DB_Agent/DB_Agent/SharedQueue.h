#pragma once
#include "singleton.h"
#include "SpinLock.h"

template <class T>
class SharedQueue : public SpinLock
{
private:
	std::queue<T>				m_itemQueue;
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
	SpinLock::Enter();

	m_itemQueue.push(_item);

	SpinLock::Leave();
}

template<class T>
T& SharedQueue<T>::GetItem(void)
{
	SpinLock::Enter();

	T& item = m_itemQueue.front();
	m_itemQueue.pop();

	SpinLock::Leave();

	return item;
}

template<class T>
bool SharedQueue<T>::IsEmpty(void)
{
	return m_itemQueue.empty();
}