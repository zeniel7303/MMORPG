#ifdef _DEBUG
#pragma comment (lib, "../../ZoneServer/x64/Debug/_ServerLibrary.lib")
#else
#pragma comment (lib, "../../ZoneServer/x64/Release/_ServerLibrary.lib")
#endif

#include <iostream>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include "../../ZoneServer/ServerLibrary/HeaderFiles/FileLog.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/Utils.h"

#include "App.h"

WSADATA m_wsaData;

int num;

void main()
{
	std::cout << "<< 몇 번째 Zone 입장(0부터) >>" << endl;
	std::cin >> num;

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("[ Failed WSAStartup() ] \n");

		return;
	}

	App* app = new App(num);
	app->Begin();

	WaitForSingleObject(app->GetHandle(), INFINITE);

	return;
}