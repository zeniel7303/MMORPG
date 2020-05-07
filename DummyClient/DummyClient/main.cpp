#include "stdafx.h"
#include "App.h"

int main()
{
	App app;

	IpEndPoint ipEndPoint;
	ipEndPoint = IpEndPoint("192.168.0.13", 30002);

	app.Init(5);

	app.Begin(ipEndPoint);

	WaitForSingleObject(app.GetHandle(), INFINITE);
}