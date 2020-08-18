#pragma once
#include <fstream>
#include <stdarg.h>
#include <direct.h>
#include <iostream>
#include <time.h>

//#define MYDEBUG(fmt, ...) { printf("[ %s : %d ] %s " fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }
//#define MYDEBUG(fmt, ...) { printf("[ %s : %d / %s] - " fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }

#define __FILENAME__ (strrchr(__FILE__,'\\')+1)
#define MYDEBUG(fmt, ...) { printf("[ %s : %d ] - " fmt, __FILENAME__, __LINE__,  ##__VA_ARGS__); }
#define FILELOG(fmt) { LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, fmt); }

namespace LOG
{
	void ClearFileLog(const char* _fileName);

	void FileLog(const char* _txtfileName, const char* _fileName, int _line, const char*_log, ...);
}