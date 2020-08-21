#pragma once
#include "LogInSession.h"
#include "LogInSessionManager.h"

#include "packet.h"

class ZoneConnector;

class PacketHandler
{
	LogInSessionManager&	m_logInSessionManager;
	ZoneConnector*			m_zoneConnector;

public:
	PacketHandler(LogInSessionManager&	_logInSessionManager)
		: m_logInSessionManager(_logInSessionManager)
	{
		dbPacketFunc[0] = &PacketHandler::OnPacket_LogInSuccess;
		dbPacketFunc[1] = &PacketHandler::OnPacket_LogInFailed;
		dbPacketFunc[2] = &PacketHandler::OnPacket_LogInFailed;
		dbPacketFunc[3] = &PacketHandler::OnPacket_RegisterSuccess;
		dbPacketFunc[4] = &PacketHandler::OnPacket_RegisterFailed;

		zoneServerPacketFunc[0] = &PacketHandler::OnPacket_HelloFromZone;
		zoneServerPacketFunc[1] = &PacketHandler::OnPacket_HelloFromZone;
	};
	~PacketHandler();

	void SetZoneConnector(ZoneConnector* _zoneConnector);

	void HandleDBConnectorPacket(Packet* _packet);
	void HandleZoneServerPacket(Packet* _packet);

	using DBConnectorPacketFunction = void (PacketHandler::*)(Packet* _packet);
	DBConnectorPacketFunction dbPacketFunc[100];

	using ZoneServerPacketFunction = void (PacketHandler::*)(Packet* _packet);
	ZoneServerPacketFunction zoneServerPacketFunc[100];

	//============================================================
	void OnPacket_LogInSuccess(Packet* _packet);
	void OnPacket_LogInFailed(Packet* _packet);
	void OnPacket_RegisterSuccess(Packet* _packet);
	void OnPacket_RegisterFailed(Packet* _packet);

	//============================================================
	void OnPacket_HelloFromZone(Packet* _packet);
	void OnPacket_DisConnectUser(Packet* _packet);

	LogInSession* FindSession(SOCKET _socket);
};

