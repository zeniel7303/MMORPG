#include "stdafx.h"
#include "App.h"

int main()
{
	App app;

	IpEndPoint ipEndPoint;
	//ipEndPoint = IpEndPoint("192.168.0.13", 30005);
	//ipEndPoint = IpEndPoint("211.221.147.29", 30005);
	ipEndPoint = IpEndPoint("211.221.147.29", 30002);

	app.Init(1);

	app.Begin(ipEndPoint);

	WaitForSingleObject(app.GetHandle(), INFINITE);

	return 1;
}