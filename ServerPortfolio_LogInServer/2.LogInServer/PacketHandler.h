#pragma once
#include "LogInSession.h"
#include "LogInSessionManager.h"

#include "packet.h"

class PacketHandler
{
	LogInSessionManager&	m_logInSessionManager;

public:
	PacketHandler(LogInSessionManager&	_logInSessionManager)
		: m_logInSessionManager(_logInSessionManager)
	{
		packetFunc[0] = nullptr;
		packetFunc[1] = nullptr;
		packetFunc[2] = nullptr;
		packetFunc[3] = nullptr;
		packetFunc[4] = nullptr;
		packetFunc[5] = nullptr;
		packetFunc[6] = nullptr;
		packetFunc[7] = nullptr;
		packetFunc[8] = nullptr;
		packetFunc[9] = &PacketHandler::OnPacket_Register;
		packetFunc[10] = &PacketHandler::OnPacket_LogIn;

		dbPacketFunc[0] = &PacketHandler::OnPacket_LogInSuccess;
		dbPacketFunc[1] = &PacketHandler::OnPacket_LogInFailed;
		dbPacketFunc[2] = &PacketHandler::OnPacket_LogInFailed;
		dbPacketFunc[3] = &PacketHandler::OnPacket_RegisterSuccess;
		dbPacketFunc[4] = &PacketHandler::OnPacket_RegisterFailed;
	};
	~PacketHandler();

	void HandlePacket(LogInSession* _session, Packet* _packet);
	void HandleDBConnectorPacket(Packet* _packet);

	using PacketFunction = void (PacketHandler::*)(LogInSession* _session, Packet* _packet);
	PacketFunction packetFunc[100];

	using DBConnectorPacketFunction = void (PacketHandler::*)(Packet* _packet);
	DBConnectorPacketFunction dbPacketFunc[100];

	void OnPacket_Register(LogInSession* _session, Packet* _packet);
	void OnPacket_LogIn(LogInSession* _session, Packet* _packet);

	void OnPacket_LogInSuccess(Packet* _packet);
	void OnPacket_LogInFailed(Packet* _packet);
	void OnPacket_RegisterSuccess(Packet* _packet);
	void OnPacket_RegisterFailed(Packet* _packet);

	LogInSession* FindSession(SOCKET _socket);
};

