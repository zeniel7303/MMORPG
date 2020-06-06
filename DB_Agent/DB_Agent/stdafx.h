#pragma once

#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

#include <map>
#include <list>
#include <queue>
#include <algorithm>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define SAFE_DELETE(p)			{ if (p) { delete (p); (p) = NULL; } }
#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }