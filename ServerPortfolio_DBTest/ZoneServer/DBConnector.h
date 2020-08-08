#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/ClientSession.h"

#include "packet.h"

#include "MainThread.h"

#include <vector>

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

	std::vector<MonsterData>	m_monsterDataVec;

public:
	DBConnector();
	~DBConnector();

	bool Connect(const char* _ip, const unsigned short _portNum);
	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();

	void GetMonstersData(Packet* _packet);

	MonsterData* GetMonsterData(int _num) { return &m_monsterDataVec[_num]; }
};