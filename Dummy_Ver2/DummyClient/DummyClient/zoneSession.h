#pragma once
#include "../../ZoneServer/ServerLibrary/HeaderFiles/IOCPClass.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/ClientSession.h"

#include "stdafx.h"
#include "packet.h"
#include "commandList.h"

#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

struct INFO
{
	UserInfo	userInfo;
	UnitInfo	unitInfo;
};

class loginSession;

class zoneSession : public ClientSession
{
public:
	int m_portNum;

	int m_userIdx;

	INFO m_basicInfo;
	int m_zoneNum;
	int m_fieldNum;

	bool isReady;

private:
	IOCPClass* iocp;

	loginSession* m_loginSession;

	IpEndPoint m_ipEndPoint;

	int tempNum;

public:
	zoneSession(int _num, IOCPClass* _iocp);
	~zoneSession();

	bool Connect();
	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();

	void PacketHandle(Packet* _packet);

	void SetPortNum(int _num) { m_portNum = _num; }
	void SetUserIdx(int _num) { m_userIdx = _num; }

	void SetLoginSession(loginSession* _session) { m_loginSession = _session; }
};

