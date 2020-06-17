#pragma once
#include <Windows.h>

//200601
//왜 쓰는가?
//일반 lock과 spin lock의 차이
//Interlocked 계열 함수에 대해 좀 더 공부

class SpinLock
{
private:
	int m_use;
public:
	SpinLock() {
		m_use = 0;
	}
	~SpinLock() {}

	void Enter()
	{
		//대기 상태
		while (true)
		{
			if (InterlockedExchange((LPLONG)&m_use, 1) == 0) break;
			Sleep(0);
		}
	}

	void Leave()
	{
		InterlockedExchange((LPLONG)&m_use, 0);
	}
};