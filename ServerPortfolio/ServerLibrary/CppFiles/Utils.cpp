#include "../HeaderFiles/Utils.h"

namespace AHN_UTILS
{
	void GetLocalTime()
	{
		time_t curTime = time(NULL);
		struct tm pLocal;
		localtime_s(&pLocal, &curTime);

		printf("[ %04d-%02d-%02d %02d:%02d:%02d ] ",
			pLocal.tm_year + 1900, pLocal.tm_mon + 1, pLocal.tm_mday,
			pLocal.tm_hour, pLocal.tm_min, pLocal.tm_sec);
	}
}