#pragma once
#include <list>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

class ThreadClass : public Thread<ThreadClass>
{
private:
	int m_time;
	HANDLE m_handle;

public:
	ThreadClass(HANDLE _handle, int _time)
	{
		m_handle = _handle;

		m_time = _time;
	}

	~ThreadClass() {}

	void LoopRun()
	{
		while (1)
		{
			//에러 체크
			WaitForSingleObject(this->GetHandle(), m_time);

			SetEvent(m_handle);

			//MYDEBUG("Test \n");
		}
	}

	HANDLE GetHandle() { return m_handle; }
};

class ThreadSchedular
{
private:
	std::list<ThreadClass*> threadList;

public:
	ThreadSchedular();
	~ThreadSchedular();

	void CreateSchedule(HANDLE _handle, int _time);
};

