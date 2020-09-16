#include <fstream>

#include "stdafx.h"
#include "App.h"

int num;

int main()
{
	char tmp[256];
	ifstream readFile;
	readFile.open("zoneNum.txt");

	if (readFile.is_open())
	{
		readFile.getline(tmp, 256);
	}

	num = atoi(tmp);

	readFile.close();

	printf("%d zone ¿‘¿Â \n", num);

	App app;

	IpEndPoint ipEndPoint;
	ipEndPoint = IpEndPoint("211.221.147.29", 30006 + num);

	app.Init(num);

	app.Begin(ipEndPoint);

	WaitForSingleObject(app.GetHandle(), INFINITE);

	return 1;
}