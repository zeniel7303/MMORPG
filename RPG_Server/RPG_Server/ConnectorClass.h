#pragma once

#include "singletonBase.h"
#include "IpEndPoint.h"
#include "Session.h"

class ConnectorClass : public Session, public singletonBase<ConnectorClass>
{
private:
	IpEndPoint m_ipEndPoint;

	std::vector<MonsterData> monsterDataVec;

public:
	ConnectorClass();
	~ConnectorClass();

	void Init();
	bool Connect();
	void OnConnect(SOCKET _socket);

	void GetMonstersData(Packet* _packet);

	MonsterData* GetMonsterData(int _num) { return &monsterDataVec[_num]; }
};