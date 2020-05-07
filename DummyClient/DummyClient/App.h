#pragma once
#include "stdafx.h"
#include "IOCPClass.h"
#include "Dummy.h"

#include "FieldTilesData.h"

#define ACT_DELAY 3000

class App : public ThreadClass<App>
{
private:
	IOCPClass iocp;

	std::vector<Dummy*> dummyVec;

	FieldTilesData villageTilesData;
	FieldTilesData fieldTilesData;
	FieldTilesData testFieldTilesData;

public:
	App();
	~App();

	void Init(int _num);
	void Begin(IpEndPoint& _ipEndPoint);

	void LoopRun();
};

