#pragma once

#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <process.h>

#include <map>
#include <list>
#include <queue>
//#include <vector>
#include <algorithm>

#include "Vector.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define CONNECTOR ConnectorClass::getSingleton()

#define SAFE_DELETE(p)			{ if (p) { delete (p); (p) = NULL; } }
#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }