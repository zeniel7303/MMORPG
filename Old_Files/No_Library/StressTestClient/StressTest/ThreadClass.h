#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

template<typename _T>
class ThreadClass
{
private:
	HANDLE				Handle;
	unsigned int		ThreadID;

public:
	ThreadClass()
	{
		Handle = 0;
	}

	virtual ~ThreadClass()
	{
		CloseHandle(Handle);
		Handle = 0;
	}

	void Start(_T* _obj)
	{
		Handle = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc, (void*)_obj, 0, &ThreadID);

		if (Handle <= 0)
		{
			throw std::runtime_error("failed_beginthreadex()");
		}

		//WaitForMultipleObjects(2, Handle, TRUE, INFINITE);
	}

	virtual void LoopRun(void) = 0;

	static unsigned int __stdcall ThreadFunc(void* pArgs)
	{
		_T* obj = (_T*)pArgs;
		obj->LoopRun();
		return 0;
	}

	HANDLE GetHandle() { return Handle; }
};

