#pragma once

#pragma comment (lib, "../x64/Debug/ServerLibrary.lib")
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <WS2tcpip.h>
#include <winsock2.h>
#include <process.h>

#include "Vector.h"

#pragma warning(disable:4996)

#define DBCONNECTOR DBConnector::getSingleton()

#define SAFE_DELETE(p)			{ if (p) { delete (p); (p) = NULL; } }
#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }