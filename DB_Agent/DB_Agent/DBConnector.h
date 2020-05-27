#pragma once
#include "EventClass.h"
#include "stdafx.h"

#include <iostream>
#include <string>
#include <mysql.h>

#include "SharedQueue.h"

#include "packet.h"
#include "SendBuffer.h"

#pragma comment(lib, "libmySQL.lib")

#pragma warning(disable:4996)

enum CONNECTOR_STATE
{
	READY,
	ACTIVE
};

class DBConnector : public EventClass<DBConnector>
{
private:
	CONNECTOR_STATE					m_state;
	bool					m_isConnect;
							
	MYSQL					m_connect;
	MYSQL_RES*				m_result;

	MYSQL_ROW				row;
	MYSQL_FIELD*			field;

	//레코드셋의 개수
	my_ulonglong			rowNum;
	//필드의 개수
	unsigned int			fieldNum;
							
	int						m_num;
						
	SharedQueue<Packet*>&	m_packetQueue;
	SendBuffer*				m_sendBuffer;
	Packet*					m_packet;

public:
	DBConnector(SharedQueue<Packet*>& _queue) : m_packetQueue(_queue)
	{
		
	}

	~DBConnector();

	void Init(int _num);
	bool Connect(const char* host,
		const char* username,
		const char* password,
		const char* database,
		const unsigned int port,
		const char* unix_socket,
		const unsigned int client_flag);
	void DisConnect();
	bool SelectDB(const char* DBname);
	MYSQL_RES* Query(const char* query);

	void Login(LogInPacket_DBAgent* _packet);
	void Register(RegisterPacket_DBAgent* _packet);
	void GetUserInfo(RequireUserInfoPacket_DBAgent* _packet);
	void UpdateUser(UpdateUserPacket* _packet);
	void GetMonsterInfo();

	bool Test();
	void EventFunc();

	int GetNum() { return m_num; }
	CONNECTOR_STATE GetState() { return m_state; }
	void SetPacket(Packet* _packet) { m_packet = _packet; }
};

