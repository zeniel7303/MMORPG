#pragma once
#include "LogInSession.h"
#include "LogInSessionManager.h"

#include "ZoneServerManager.h"

#include "packet.h"

class PacketHandler
{
	LogInSessionManager&	m_logInSessionManager;
	ZoneServerManager&		m_zoneServerManager;

public:
	PacketHandler(LogInSessionManager&	_logInSessionManager, ZoneServerManager& _zoneServerManager)
		: m_logInSessionManager(_logInSessionManager), m_zoneServerManager(_zoneServerManager)
	{
		dbPacketFunc[0] = &PacketHandler::OnPacket_LogInSuccess;
		dbPacketFunc[1] = &PacketHandler::OnPacket_LogInFailed;
		dbPacketFunc[2] = &PacketHandler::OnPacket_LogInFailed;
		dbPacketFunc[3] = &PacketHandler::OnPacket_RegisterSuccess;
		dbPacketFunc[4] = &PacketHandler::OnPacket_RegisterFailed;

		zoneServerPacketFunc[0] = &PacketHandler::OnPacket_HelloFromZone;
		zoneServerPacketFunc[1] = &PacketHandler::OnPacket_DisConnectUser;
		zoneServerPacketFunc[2] = &PacketHandler::OnPacket_HeartBeat;
		zoneServerPacketFunc[3] = &PacketHandler::OnPacket_AuthenticationUser;
	};
	~PacketHandler();

	void HandleDBConnectorPacket(Packet* _packet);
	void HandleZoneServerPacket(int _num, Packet* _packet);

	using DBConnectorPacketFunction = void (PacketHandler::*)(Packet* _packet);
	DBConnectorPacketFunction dbPacketFunc[100];

	using ZoneServerPacketFunction = void (PacketHandler::*)(ZoneConnector* _zone, Packet* _packet);
	ZoneServerPacketFunction zoneServerPacketFunc[100];

	//============================================================
	void OnPacket_LogInSuccess(Packet* _packet);
	void OnPacket_LogInFailed(Packet* _packet);
	void OnPacket_RegisterSuccess(Packet* _packet);
	void OnPacket_RegisterFailed(Packet* _packet);

	//============================================================
	void OnPacket_HelloFromZone(ZoneConnector* _zone, Packet* _packet);
	void OnPacket_DisConnectUser(ZoneConnector* _zone, Packet* _packet);
	void OnPacket_HeartBeat(ZoneConnector* _zone, Packet* _packet);
	void OnPacket_AuthenticationUser(ZoneConnector* _zone, Packet* _packet);

	LogInSession* FindSession(SOCKET _socket);
};

