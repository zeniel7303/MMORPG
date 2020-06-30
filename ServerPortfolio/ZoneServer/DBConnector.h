#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/singletonBase.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include"../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "UserSession.h"
#include "packet.h"

#include "ServerLogicThread.h"

#include <vector>

class DBConnector : public UserSession, public SingletonBase<DBConnector>
{
private:
	IpEndPoint					m_ipEndPoint;

	std::vector<MonsterData>	m_monsterDataVec;

public:
	DBConnector();
	~DBConnector();

	bool Init(const char* _ip, const unsigned short _portNum);
	bool Connect();
	void OnConnect();
	void DisConnect();
	void Reset();
	void HandleOverlappedIO(ST_OVERLAPPED* _overlapped);

	void Parsing();

	void GetMonstersData(Packet* _packet);

	MonsterData* GetMonsterData(int _num) { return &m_monsterDataVec[_num]; }
};

