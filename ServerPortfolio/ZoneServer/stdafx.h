#pragma once

#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>
#include <winsock2.h>
#include <process.h>

#include "Vector.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define DBCONNECTOR DBConnector::getSingleton()

#define SAFE_DELETE(p)			{ if (p) { delete (p); (p) = NULL; } }
#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }