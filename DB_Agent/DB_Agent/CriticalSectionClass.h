#pragma once
#include <Windows.h>

class CriticalSectionClass
{
public:
	CRITICAL_SECTION cs;

public:
	CriticalSectionClass();
	~CriticalSectionClass();

	void EnterLock();
	void LeaveLock();
};

class CSLock
{
private:
	CRITICAL_SECTION criticalSection;

public :
	CSLock(CRITICAL_SECTION& cs)
	{
		criticalSection = cs;

		EnterCriticalSection(&criticalSection);
	}

	~CSLock()
	{
		LeaveCriticalSection(&criticalSection);
	}
};
