#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/singletonBase.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include"../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "Session.h"
#include "packet.h"

#include "ServerLogicThread.h"

#include <vector>

class DBConnector : public Session, public SingletonBase<DBConnector>
{
private:
	IpEndPoint					m_ipEndPoint;

	std::vector<MonsterData>	m_monsterDataVec;

public:
	DBConnector();
	~DBConnector();

	void Init(const char* _ip, const unsigned short _portNum);
	bool Connect();
	void OnConnect();
	void Disconnect();
	void Reset();
	void CheckCompletion(Acceptor::ST_OVERLAPPED* _overlapped);

	void Parsing();

	void GetMonstersData(Packet* _packet);

	MonsterData* GetMonsterData(int _num) { return &m_monsterDataVec[_num]; }
};

