#include "../HeaderFiles/CriticalSection.h"

CriticalSection::CriticalSection()
{
	InitializeCriticalSection(&cs);
}

CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(&cs);
}

void CriticalSection::EnterLock()
{
	EnterCriticalSection(&cs);
}

void CriticalSection::LeaveLock()
{
	LeaveCriticalSection(&cs);
}