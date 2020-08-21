#include "PacketHandler.h"

#include "DBConnector.h"
#include "ZoneConnector.h"

PacketHandler::~PacketHandler()
{
}

void PacketHandler::SetZoneConnector(ZoneConnector* _zoneConnector)
{
	m_zoneConnector = _zoneConnector;
}

void PacketHandler::HandleDBConnectorPacket(Packet* _packet)
{
	Packet* packet = _packet;

	(this->*dbPacketFunc[packet->cmd % 100])(packet);

	DBConnector::getSingleton()->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::HandleZoneServerPacket(Packet* _packet)
{
	Packet* packet = _packet;

	(this->*zoneServerPacketFunc[packet->cmd % 300])(packet);

	m_zoneConnector->GetReceiver()->GetRingBuffer()->Read(packet->size);
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

			return;
		}

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

void PacketHandler::OnPacket_HelloFromZone(Packet* _packet)
{
	MYDEBUG("[ Connecting With ZoneServer Success ]\n");
}

void PacketHandler::OnPacket_DisConnectUser(Packet* _packet)
{
	UserNumPacket* userNumPacket = reinterpret_cast<UserNumPacket*>(_packet);

	const std::unordered_map<int, LogInSession*> tempHashMap = m_logInSessionManager.GetSessionHashMap()->GetItemHashMap();

	auto iter = tempHashMap.find(userNumPacket->userIndex);
	if (iter == tempHashMap.end())
	{
		return;
	}

	LogInSession* tempSession;
	tempSession = tempHashMap.find(userNumPacket->userIndex)->second;

	if (tempSession != nullptr)
	{
		tempSession->DisConnect();
		tempSession->m_isAlreadyDisConnected = true;
	}
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