#pragma once
#include <Windows.h>

class CriticalSectionClass
{
private:
	CRITICAL_SECTION criticalSection;

public:
	CriticalSectionClass();
	~CriticalSectionClass();

	void EnterLock();
	void LeaveLock();
};

