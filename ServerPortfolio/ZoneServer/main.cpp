//라이브러리 추가
#pragma comment (lib, "../x64/Debug/ServerLibrary.lib")

#include "ZoneServer.h"

int main()
{
	ZoneServer* zoneServer = new ZoneServer();
	zoneServer->Start();

	WSACleanup();

	return 0;
}