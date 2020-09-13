#pragma once
#include "../../ZoneServer/ServerLibrary/HeaderFiles/IOCPClass.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/ClientSession.h"

#pragma warning(disable:4996)

#include "stdafx.h"
#include "packet.h"
#include "commandList.h"

#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

class zoneSession;

class loginSession : public ClientSession
{
private:
	IOCPClass* iocp;

	zoneSession* m_zoneSession;

	IpEndPoint m_ipEndPoint;

	char id[15] = "testUser";
	char password[15] = "testUser";

public:
	loginSession(int _num, IOCPClass* _iocp);
	~loginSession();

	bool Connect();
	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();

	void PacketHandle(Packet* _packet);

	void SendZoneNumPacket();
	void SendHeartBeat();

	void SetZoneSession(zoneSession* _session) { m_zoneSession = _session; }
};

