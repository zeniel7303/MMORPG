#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/singletonBase.h"
#include "Connector.h"
#include "packet.h"

#include <vector>

class DBConnector : public Connector, public SingletonBase<DBConnector>
{
private:
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

