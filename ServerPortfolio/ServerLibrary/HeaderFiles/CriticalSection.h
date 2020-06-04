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
	CSLock(CRITICAL_SECTION& cs) : criticalSection(cs)
	{
		EnterCriticalSection(&criticalSection);
	}

	~CSLock()
	{
		LeaveCriticalSection(&criticalSection);
	}
};