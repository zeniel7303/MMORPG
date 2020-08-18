#include "../HeaderFiles/FileLog.h"

namespace LOG
{
	void ClearFileLog(const char* _fileName)
	{
		remove(_fileName);
	}

	void FileLog(const char* _txtfileName, const char* _fileName, int _line, const char*_log, ...)
	{
		char tempBuffer[1024];
		time_t curTime = time(NULL);
		struct tm pLocal;
		localtime_s(&pLocal, &curTime);

		sprintf(tempBuffer, "[ %04d-%02d-%02d %02d:%02d:%02d -  %s : %d ] %s",
			pLocal.tm_year + 1900, pLocal.tm_mon + 1, pLocal.tm_mday,
			pLocal.tm_hour, pLocal.tm_min, pLocal.tm_sec, _fileName, _line, _log);

		//현재 로그 작성할때마다 txt파일 열고 닫고있음.
		std::fstream _streamOut;
		_streamOut.open(_txtfileName, std::ios::out | std::ios::app);

		va_list argList;
		char cbuffer[1024];
		va_start(argList, tempBuffer);
		vsnprintf(cbuffer, 1024, tempBuffer, argList);
		va_end(argList);

		_streamOut << cbuffer << std::endl;
		_streamOut.close();
	}
}