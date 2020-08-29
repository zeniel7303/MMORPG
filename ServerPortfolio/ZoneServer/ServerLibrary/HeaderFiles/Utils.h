#pragma once
#include <time.h>
#include <iostream>

#define TRYCATCH(expr)\
{\
	try\
	{\
		expr;\
	}\
	catch (const std::bad_alloc& error)\
	{\
		MYDEBUG("bad alloc : %s\n", error.what());\
		return false;\
	}\
}

#define TRYCATCH_CONSTRUCTOR(expr, _bool)\
{\
	try\
	{\
		expr;\
	}\
	catch (const std::bad_alloc& error)\
	{\
		_bool = true;\
		MYDEBUG("bad alloc : %s\n", error.what());\
	}\
}

namespace AHN_UTILS
{
	void GetLocalTime();
}