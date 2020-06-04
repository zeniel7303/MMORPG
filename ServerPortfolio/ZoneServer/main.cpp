#pragma comment (lib, "../x64/Debug/ServerLibrary.lib")

#include "ZoneServer.h"

int main()
{
	ZoneServer* zoneServer = new ZoneServer();
	zoneServer->Init();

	WSACleanup();

	return 0;
}