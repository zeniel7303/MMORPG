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

	SpinLock				m_spinLock;

public:
	DoubleQueue()
	{
		primaryQueue = &queue_1;
		secondaryQueue = &queue_2;
	}

	~DoubleQueue()
	{
		//자동으로 메모리 정리를 해주는가?
		//혹은 이렇게 직접 해줘야하나?
		//알아서 지워주더라
		/*while (!queue_1.empty())
		{
			queue_1.pop();
		}

		while (!queue_2.empty())
		{
			queue_2.pop();
		}*/
	}

	void Swap()
	{
		m_spinLock.Enter();

		std::queue<T>* temp;

		temp = primaryQueue;
		primaryQueue = secondaryQueue;
		secondaryQueue = temp;

		m_spinLock.Leave();
	}

	//넣는건 첫번째 큐에서만
	void AddObject(const T& _object)
	{
		m_spinLock.Enter();

		primaryQueue->push(_object);

		m_spinLock.Leave();
	}

	//빼는건 두번째 큐에서만
	T& PopObject()
	{
		T& retVal = secondaryQueue->front();
		//해당 부분은 T를 처리한 이후에 해주는 것이 안전하다.
		secondaryQueue->pop();

		return retVal;
	}

	bool IsEmpty(void)
	{
		return secondaryQueue->empty();
	}

	std::queue<T>& GetPrimaryQueue()
	{
		return *primaryQueue;
	}

	std::queue<T>& GetSecondaryQueue()
	{
		return *secondaryQueue;
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