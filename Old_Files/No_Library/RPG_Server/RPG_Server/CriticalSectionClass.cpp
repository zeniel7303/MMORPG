#include "CriticalSectionClass.h"

CriticalSectionClass::CriticalSectionClass()
{
	InitializeCriticalSection(&cs);
}

CriticalSectionClass::~CriticalSectionClass()
{
	DeleteCriticalSection(&cs);
}

void CriticalSectionClass::EnterLock()
{
	EnterCriticalSection(&cs);
}

void CriticalSectionClass::LeaveLock()
{
	LeaveCriticalSection(&cs);
}