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
	}

	void AddObject(T* _object)
	{
		//CSLock csLock(m_cs);

		objects.push(_object);
	}

	// ������Ʈ�� ������.
	T* PopObject()
	{
		CSLock csLock(m_cs);

		// ������Ʈ�� ����
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

	// ���� ũ�⸸ŭ ���ο� ������Ʈ�� �־� Ȯ���Ѵ�.
	/*void Expand()
	{
		locker.EnterLock();

		// �ø� ũ�⸸ŭ ���ο� ������Ʈ�� �־��ش�.
		for (int i = 0; i < maxSize; ++i)
		{
			T* newObject = new T();
			objects.push(newObject);
		}

		// �ִ� ũ�⸦ �ø���.
		maxSize += maxSize;

		locker.LeaveLock();
	}*/

	// ������Ʈ�� �����Ѵ�.
	void ReturnObject(T* _object)
	{
		//CSLock csLock(m_cs);

		objects.push(_object);
	}
};