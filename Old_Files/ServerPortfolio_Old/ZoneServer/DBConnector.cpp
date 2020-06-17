#include "DBConnector.h"

DBConnector::DBConnector()
{
}

DBConnector::~DBConnector()
{
}

void DBConnector::Init(const char* _ip, const unsigned short _portNum)
{
	Connector::Init(_ip, _portNum);
}

bool DBConnector::Connect()
{
	return Connector::Connect();
}

void DBConnector::OnConnect(SOCKET _socket)
{
	Connector::OnConnect(_socket);

	//왜 바로 못보내고 Sleep 걸어야 보내지?
	Sleep(1);
	//BOOL bVal = TRUE;
	//::setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&bVal, sizeof(BOOL));

	Packet* requestMonsterDataPacket =
		reinterpret_cast<Packet*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
	requestMonsterDataPacket->Init(SendCommand::Zone2DB_REQUEST_MONSTER_DATA, sizeof(Packet));

	Send(reinterpret_cast<char*>(requestMonsterDataPacket), requestMonsterDataPacket->size);
}

void DBConnector::GetMonstersData(Packet* _packet)
{
	MonstersInfoPacket* monstersInfoPacket = static_cast<MonstersInfoPacket*>(_packet);

	for (int i = 0; i < monstersInfoPacket->count; i++)
	{
		monsterDataVec.push_back(monstersInfoPacket->monstersData[i]);
	}
}