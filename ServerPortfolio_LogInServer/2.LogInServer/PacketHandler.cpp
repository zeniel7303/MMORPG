#include "PacketHandler.h"

#include "DBConnector.h"

PacketHandler::~PacketHandler()
{
}

void PacketHandler::HandlePacket(LogInSession* _session, Packet* _packet)
{
	Packet* packet = _packet;
	LogInSession* session = _session;

	(this->*packetFunc[packet->cmd])(session, packet);

	session->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::HandleDBConnectorPacket(Packet* _packet)
{
	Packet* packet = _packet;

	(this->*dbPacketFunc[packet->cmd % 100])(packet);

	DBConnector::getSingleton()->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::OnPacket_Register(LogInSession* _session, Packet* _packet)
{
	RegisterUserPacket* registerUserPacket = reinterpret_cast<RegisterUserPacket*>(_packet);	

	_session->RegisterUser(registerUserPacket);
}

void PacketHandler::OnPacket_LogIn(LogInSession* _session, Packet* _packet)
{
	LogInPacket* logInPacket = reinterpret_cast<LogInPacket*>(_packet);

	_session->LogInUser(logInPacket);
}

void PacketHandler::OnPacket_LogInSuccess(Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(_packet);

	LogInSession* tempSession;
	tempSession = FindSession(logInSuccessPacket->socket);

	if (tempSession != nullptr)
	{
		tempSession->LogInSuccess(logInSuccessPacket->userIndex);
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