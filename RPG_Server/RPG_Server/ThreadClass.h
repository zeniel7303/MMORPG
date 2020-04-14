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
	HANDLE				hThread;
	unsigned int		threadID;

public:
	ThreadClass()
	{
		hThread = 0;
	}

	virtual ~ThreadClass()
	{
		if (hThread)
		{
			CloseHandle(hThread);
			hThread = 0;
		}
	}

	void Start(_T* _obj)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc, (void*)_obj, 0, &threadID);

		if (hThread <= 0)
		{
			throw std::runtime_error("failed_beginthreadex()");
		}

		//WaitForSingleObject(hThread, INFINITE);
	}

	//thread Loop가 실행될 함수
	virtual void LoopRun(void) = 0;

	//_beginthreadex()에 들어갈 static 함수. threadLoop 함수 호출
	static unsigned int __stdcall ThreadFunc(void* pArgs)
	{
		_T* obj = (_T*)pArgs;
		obj->LoopRun();
		return 0;
	}

	HANDLE GetHandle() { return hThread; }
};

