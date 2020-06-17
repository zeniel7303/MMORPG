#pragma once
#include <queue>
#include "../../HeaderFiles/CriticalSection.h"
#include "../SpinLock.h"

template<typename T>
class DoubleQueue
{
private:
	std::queue<T>			queue_1;
	std::queue<T>			queue_2;

	std::queue<T>*			primaryQueue;
	std::queue<T>*			secondaryQueue;

	CRITICAL_SECTION		m_cs;
	SpinLock				m_spinLock;

public:
	DoubleQueue()
	{
		InitializeCriticalSection(&m_cs);

		primaryQueue = &queue_1;
		secondaryQueue = &queue_2;
	}

	~DoubleQueue()
	{
		while (!queue_1.empty())
		{
			queue_1.pop();
		}

		while (!queue_2.empty())
		{
			queue_2.pop();
		}

		DeleteCriticalSection(&m_cs);
	}

	void SwappingQueue()
	{
		CSLock csLock(m_cs);

		if (primaryQueue == &queue_1)
		{
			primaryQueue = &queue_2;
			secondaryQueue = &queue_1;
		}
		else if (primaryQueue == &queue_2)
		{
			primaryQueue = &queue_1;
			secondaryQueue = &queue_2;
		}
		else
		{
			printf("Swap Queue Error \n");
		}
	}

	//넣는건 첫번째 큐에서만
	void AddObject(T _object)
	{
		m_spinLock.Enter();

		primaryQueue->push(_object);

		m_spinLock.Leave();
	}

	//빼는건 두번째 큐에서만
	T& PopObject()
	{
		T& retVal = secondaryQueue->front();
		secondaryQueue->pop();

		return retVal;
	}

	bool IsEmpty(void)
	{
		return secondaryQueue->empty();
	}

	int GetPrimaryQueueSize()
	{
		return primaryQueue->size();
	}

	int GetSecondaryQueueSize()
	{
		return secondaryQueue->size();
	}
};