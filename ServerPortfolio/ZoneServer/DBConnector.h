#pragma once
#include "../ServerLibrary/HeaderFiles/SubHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/SubHeaders/singletonBase.h"
#include "packet.h"
#include "Session.h"

#include <vector>

class DBConnector : public Session, public SingletonBase<DBConnector>
{
private:
	IpEndPoint m_ipEndPoint;

	std::vector<MonsterData> monsterDataVec;

public:
	DBConnector();
	~DBConnector();

	void Init(const char* _ip, const unsigned short _portNum);
	bool Connect();
	void OnConnect(SOCKET _socket);

	void GetMonstersData(Packet* _packet);

	MonsterData* GetMonsterData(int _num) { return &monsterDataVec[_num]; }
};

