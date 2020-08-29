#pragma once
#include <chrono>
#include <vector>

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/ClientSession.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "packet.h"

#include "MainThread.h"

class DBConnector : public ClientSession
{
public:
	static DBConnector* getSingleton()
	{
		static DBConnector singleton;

		return &singleton;
	}

private:
	IpEndPoint					m_ipEndPoint;

	IOCPClass*					m_IOCPClass;

	std::chrono::high_resolution_clock::time_point	m_start;

	std::vector<MonsterData>	m_monsterDataVec;

	DBConnector();
public:
	~DBConnector();

	bool Connect(const char* _ip, const unsigned short _portNum, IOCPClass* _iocpClass);
	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();

	void HeartBeat();

	void GetMonstersData(Packet* _packet);

	void SetStartTime() { m_start = std::chrono::high_resolution_clock::now(); }

	MonsterData* GetMonsterData(int _num) { return &m_monsterDataVec[_num]; }
};