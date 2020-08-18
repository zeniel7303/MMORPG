#include "../HeaderFiles/TimeSchedular.h"

TimeSchedular::TimeSchedular()
{
}


TimeSchedular::~TimeSchedular()
{
}

void TimeSchedular::SetTimer(int _num)
{
	int time = _num;

	HANDLE				hThread;
	unsigned int		threadID;

	/*auto lambda = [=](int _num)
	{
		while (1)
		{
			WaitingFor
		}
	};

	hThread = (HANDLE)_beginthreadex(NULL, 0, 
		static_cast<unsigned int __stdcall>(lambda(time)),
		(void*)time, 0, &threadID);*/
}