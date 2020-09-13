#include "PacketHandler.h"

#include "DBConnector.h"
#include "ZoneConnector.h"

PacketHandler::~PacketHandler()
{
}

void PacketHandler::HandleDBConnectorPacket(Packet* _packet)
{
	Packet* packet = _packet;

	(this->*dbPacketFunc[packet->cmd % 100])(packet);

	DBConnector::getSingleton()->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::HandleZoneServerPacket(int _num, Packet* _packet)
{
	Packet* packet = _packet;
	ZoneConnector* zoneConnector = m_zoneServerManager.GetZoneConnector(_num);

	(this->*zoneServerPacketFunc[packet->cmd % 300])(zoneConnector, packet);

	zoneConnector->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::OnPacket_LogInSuccess(Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(_packet);

	LogInSession* tempSession;
	tempSession = FindSession(logInSuccessPacket->socket);

	if (tempSession != nullptr)
	{
		if (m_logInSessionManager.GetSessionHashMap()->IsExist(logInSuccessPacket->userIndex))
		{
			tempSession->LogInDuplicated();

			//접속중인 사람도 종료시키기
			const std::unordered_map<int, LogInSession*> tempHashMap = m_logInSessionManager.GetSessionHashMap()->GetItemHashMap();

			auto iter = tempHashMap.find(logInSuccessPacket->userIndex);
			if (iter == tempHashMap.end())
			{
				return;
			}

			tempSession = iter->second;
			if (tempSession != nullptr)
			{
				tempSession->DisConnect();
			}

			return;
		}

		/*ZoneConnector* zoneConnector = m_zoneServerManager.GetZoneConnector(tempSession->GetZoneNum());
		if (zoneConnector == nullptr)
		{
			MYDEBUG("[ 존재하지 않는 Zone	 ]\n");

			tempSession->LogInFailed();

			return;
		}*/

		tempSession->LogInSuccess(logInSuccessPacket->userIndex);

		MainThread::getSingleton()->AddToHashMapQueue(tempSession);
	}
}

void PacketHandler::OnPacket_LogInFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	LogInSession* tempSession;
	tempSession = FindSession(packetWithSocket->socket);

	if (tempSession != nullptr)
	{
		tempSession->LogInFailed();
	}
}

void PacketHandler::OnPacket_RegisterSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	LogInSession* tempSession;
	tempSession = FindSession(packetWithSocket->socket);

	if (tempSession != nullptr)
	{
		tempSession->RegisterSuccess();
	}
}

void PacketHandler::OnPacket_RegisterFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	LogInSession* tempSession;
	tempSession = FindSession(packetWithSocket->socket);

	if (tempSession != nullptr)
	{
		tempSession->RegisterFailed();
	}
}

void PacketHandler::OnPacket_HelloFromZone(ZoneConnector* _zone, Packet* _packet)
{
	MYDEBUG("[ Connecting With %d ZoneServer Success ]\n", _zone->GetZoneNum());
}

void PacketHandler::OnPacket_DisConnectUser(ZoneConnector* _zone, Packet* _packet)
{
	UserNumPacket* userNumPacket = reinterpret_cast<UserNumPacket*>(_packet);

	const std::unordered_map<int, LogInSession*> tempHashMap = m_logInSessionManager.GetSessionHashMap()->GetItemHashMap();

	auto iter = tempHashMap.find(userNumPacket->userIndex);
	if (iter == tempHashMap.end())
	{
		MYDEBUG("[ non-existent user ]\n");

		return;
	}

	LogInSession* tempSession = iter->second;
	if (tempSession != nullptr)
	{
		tempSession->DisConnect();
	}
}

void PacketHandler::OnPacket_HeartBeat(ZoneConnector* _zone, Packet* _packet)
{
	_zone->HeartBeat();
}

void PacketHandler::OnPacket_AuthenticationUser(ZoneConnector* _zone, Packet* _packet)
{
	AuthenticationPacket* authenticationPacket = reinterpret_cast<AuthenticationPacket*>(_packet);

	const std::unordered_map<int, LogInSession*> tempHashMap = m_logInSessionManager.GetSessionHashMap()->GetItemHashMap();

	auto iter = tempHashMap.find(authenticationPacket->userIndex);
	if (iter == tempHashMap.end())
	{
		_zone->AuthenticationFailed(authenticationPacket);

		return;
	}

	_zone->AuthenticationSuccess(authenticationPacket);
}

void PacketHandler::OnPacket_ReadyToChangeZone(ZoneConnector* _zone, Packet* _packet)
{
	UserNumPacket* ReadyToChangeZone = reinterpret_cast<UserNumPacket*>(_packet);

	const std::unordered_map<int, LogInSession*> tempHashMap = m_logInSessionManager.GetSessionHashMap()->GetItemHashMap();

	auto iter = tempHashMap.find(ReadyToChangeZone->userIndex);
	if (iter == tempHashMap.end())
	{
		return;
	}

	ZoneNumPacket* changeZone = reinterpret_cast<ZoneNumPacket*>(iter->second->GetSendBuffer()->
		GetBuffer(sizeof(ZoneNumPacket)));
	changeZone->Init(SendCommand::Login2C_CHANGE_ZONE, sizeof(ZoneNumPacket));
	changeZone->zoneNum = iter->second->GetZoneNum();
	//m_sendBuffer->Write(changeZone->size);

	iter->second->Send(reinterpret_cast<char*>(changeZone), changeZone->size);
}

LogInSession* PacketHandler::FindSession(SOCKET _socket)
{
	const std::list<LogInSession*> tempList = m_logInSessionManager.GetSessionList()->GetItemList();

	for (const auto& element : tempList)
	{
		if (element->GetSocket() == _socket)
		{
			return element;
		}
	}

	return nullptr;
}