#pragma once
#include <queue>

#include "../../HeaderFiles/CriticalSection.h"

using namespace std;

//=====================================================

//ObjectPool

//=====================================================
template<typename T>
class ObjectPool
{
private:
	queue<T*> objects;

	CRITICAL_SECTION		m_cs;

public:
	int GetSize()
	{
		return int(objects.size());
	}

	ObjectPool() {
		InitializeCriticalSection(&m_cs);
	}

	~ObjectPool()
	{
		DeleteCriticalSection(&m_cs);

		while (!objects.empty())
		{
			objects.pop();
		}
	}

	void AddObject(T* _object)
	{
		//CSLock csLock(m_cs);

		objects.emplace(_object);
	}

	// 오브젝트를 꺼낸다.
	T* PopObject()
	{
		CSLock csLock(m_cs);

		// 오브젝트가 없다
		if (objects.empty())
		{
			return nullptr;
		}

		T* retVal = objects.front();
		objects.pop();

		//Critical Sections and return values in C++
		//https://stackoverflow.com/questions/11653062/critical-sections-and-return-values-in-c

		return retVal;
	}

	// 현재 크기만큼 새로운 오브젝트를 넣어 확장한다.
	/*void Expand()
	{
		locker.EnterLock();

		// 늘린 크기만큼 새로운 오브젝트를 넣어준다.
		for (int i = 0; i < maxSize; ++i)
		{
			T* newObject = new T();
			objects.push(newObject);
		}

		// 최대 크기를 늘린다.
		maxSize += maxSize;

		locker.LeaveLock();
	}*/

	// 오브젝트를 수거한다.
	void ReturnObject(T* _object)
	{
		//CSLock csLock(m_cs);

		objects.push(_object);
	}
};