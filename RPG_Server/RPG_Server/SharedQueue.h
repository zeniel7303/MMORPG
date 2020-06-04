#pragma once
#include "SpinLock.h"
#include "CriticalSectionClass.h"

template <class T>
class SharedQueue : public SpinLock
{
private:
	std::queue<T>				m_itemQueue;
public:
	SharedQueue() {}
	~SharedQueue() {
		while (!m_itemQueue.empty())
		{
			T* item = m_itemQueue.top();
			m_itemQueue.pop();
			delete item;
		}
	}

public:
	void			AddItem(T _item);
	T&				GetItem(void);

	bool			IsEmpty(void);

	int				GetSize(void) { return m_itemQueue.size(); }
};

template<class T>
inline void SharedQueue<T>::AddItem(T _item)
{
	SpinLock::Enter();

	m_itemQueue.push(_item);

	SpinLock::Leave();
}

template<class T>
inline T& SharedQueue<T>::GetItem(void)
{
	SpinLock::Enter();

	T& item = m_itemQueue.front();
	m_itemQueue.pop();

	SpinLock::Leave();

	return item;
}

template<class T>
inline bool SharedQueue<T>::IsEmpty(void)
{
	return m_itemQueue.empty();
}