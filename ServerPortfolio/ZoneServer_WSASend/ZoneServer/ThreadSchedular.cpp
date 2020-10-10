#include "ThreadSchedular.h"


ThreadSchedular::ThreadSchedular()
{
}

ThreadSchedular::~ThreadSchedular()
{
}

void ThreadSchedular::CreateSchedule(HANDLE _handle, int _time)
{
	ThreadClass* thread = new ThreadClass(_handle, _time);
	threadList.emplace_back(thread);

	thread->Start(thread);
}