#pragma once
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

//요놈은 sockaddr_in을 상속
struct IpEndPoint : SOCKADDR_IN
{
	char* ip;
	unsigned short port;

	IpEndPoint() {}

	IpEndPoint(const char* _ip, const unsigned short _port)
	{
		ip = new char[15];
		memset(ip, 0, sizeof(char) * 15);
		memcpy(ip, _ip, strlen(_ip));
		port = _port;

		memset(this, 0, sizeof(SOCKADDR_IN));
		sin_family = AF_INET;
		inet_pton(AF_INET, ip, &sin_addr);

		sin_port = htons(port);
	}
};