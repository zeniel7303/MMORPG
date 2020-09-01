#pragma once
#include <Windows.h>

//unsigned short
enum class RecvCommand : WORD
{
	Zone2Path_PATHFIND = 400,
	Zone2Path_HEARTBEAT = 401,
};

enum class SendCommand : WORD
{
	Path2Zone_PATHFIND_SUCCESS = 500,
	Path2Zone_PATHFIND_FAILED = 501,
	Path2Zone_ALIVE = 502
};