#include "CriticalSectionClass.h"

CriticalSectionClass::CriticalSectionClass()
{
	InitializeCriticalSection(&criticalSection);
}

CriticalSectionClass::~CriticalSectionClass()
{
	DeleteCriticalSection(&criticalSection);
}

void CriticalSectionClass::EnterLock()
{
	EnterCriticalSection(&criticalSection);
}

void CriticalSectionClass::LeaveLock()
{
	LeaveCriticalSection(&criticalSection);
}