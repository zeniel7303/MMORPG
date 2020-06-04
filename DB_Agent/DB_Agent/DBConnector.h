#pragma once
#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN

#include <tchar.h>
#include <iostream>
#include <string>

#include <mysql.h>
#include <sql.h>
#include <sqlext.h>

#include "SharedQueue.h"

#include "packet.h"
#include "SendBuffer.h"

#pragma comment(lib, "libmySQL.lib")

#pragma warning(disable:4996)

//#define _DEFINE_ODBC_

enum CONNECTOR_STATE
{
	READY,
	ACTIVE
};

class DBConnector
{
private:
	HANDLE					hEvent;
	HANDLE					hThread;
	unsigned int			threadID;

	CONNECTOR_STATE			m_state;
	bool					m_isConnect;
							
#ifdef _DEFINE_ODBC_
	SQLHENV					m_henv;
	SQLHDBC					m_hdbc;
	SQLHSTMT				m_hstmt;
	SQLRETURN				m_retcode;
#else
	MYSQL					m_connect;
	MYSQL_RES*				m_result;

	MYSQL_ROW				row;
	MYSQL_FIELD*			field;

	//레코드셋의 개수
	my_ulonglong			rowNum;
	//필드의 개수
	unsigned int			fieldNum;
#endif						
	int						m_num;
						
	SharedQueue<Packet*>&	m_packetQueue;
	HANDLE&					m_hEvent;

	SendBuffer*				m_sendBuffer;
	Packet*					m_packet;

public:
	DBConnector(SharedQueue<Packet*>& _queue, HANDLE& _handle) 
		: m_packetQueue(_queue), m_hEvent(_handle)
	{
		
	}

	~DBConnector()
	{
		DisConnect();
	}

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

	void Thread();

	static unsigned int __stdcall ThreadFunc(void* pArgs)
	{
		DBConnector* obj = (DBConnector*)pArgs;
		obj->Thread();
		return 0;
	}

	HANDLE GetEventHandle() { return hEvent; }
	int GetNum() { return m_num; }
	CONNECTOR_STATE GetState() { return m_state; }
	void SetPacket(Packet* _packet) { m_packet = _packet; }
};

