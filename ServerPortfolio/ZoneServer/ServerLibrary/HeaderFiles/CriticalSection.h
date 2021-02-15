#pragma once
#include <Windows.h>

class CriticalSection
{
private:
	CRITICAL_SECTION cs;

public:
	CriticalSection();
	~CriticalSection();

	void EnterLock();
	void LeaveLock();
};

class CSLock
{
private:
	CRITICAL_SECTION& criticalSection;

public:
	//									  ↓     참조자여서 썼지만 빠른 초기화를 위해서도 썼다.
	CSLock(CRITICAL_SECTION& cs) : criticalSection(cs)
	{
		EnterCriticalSection(&criticalSection);
	}

	~CSLock()
	{
		LeaveCriticalSection(&criticalSection);
	}
};