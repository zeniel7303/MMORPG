#pragma once
#include <vector>
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

#include "../../ZoneServer/ServerLibrary/HeaderFiles/IOCPClass.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

#include "FieldTilesData.h"

#include "Dummy.h"

class App : public Thread<App>
{
private:
	IOCPClass* iocp;

	FieldTilesData villageTilesData;
	FieldTilesData fieldTilesData;
	FieldTilesData testFieldTilesData;

	std::vector<Dummy*> dummyVec;

public:
	App(int _num);
	~App();

	void Begin();

	void LoopRun();
};

