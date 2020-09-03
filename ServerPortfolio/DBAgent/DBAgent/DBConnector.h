#pragma once
#define WIN32_LEAN_AND_MEAN

#define _DEFINE_ODBC_

#include <tchar.h>
#include <iostream>
#include <string>

#include <mysql.h>
#include <sql.h>
#include <sqlext.h>

#include "packet.h"
#include "DBAgent.h"

#include "../../ZoneServer/ServerLibrary/HeaderFiles/SendBuffer.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#pragma comment(lib, "libmySQL.lib")

#pragma warning(disable:4996)

enum CONNECTOR_STATE
{
	READY,
	ACTIVE
};

class DBConnector : public Thread<DBConnector>
{
public:
	DBAgent*				m_dbAgent;
	Packet*					m_packet;

private:
	HANDLE					m_hEvent;

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

	SendBuffer*				m_sendBuffer;

public:
	DBConnector(int _num);
	~DBConnector();

	bool Connect(const char* host,
		const char* username,
		const char* password,
		const char* database,
		const unsigned int port,
		const char* unix_socket,
		const unsigned int client_flag);
	void DisConnect();

#ifdef _DEFINE_ODBC_
	bool Query(const char* query);
#else
	bool SelectDB(const char* DBname);
	MYSQL_RES* Query(const char* query);
#endif	

	void Login(LogInPacket_DBAgent* _packet);
	void Register(RegisterPacket_DBAgent* _packet);
	void GetUserInfo(RequireUserInfoPacket_DBAgent* _packet);
	void UpdateUser(UpdateUserPacket* _packet);
	void GetMonsterInfo();
	void DBTest(RegisterPacket_DBAgent* _packet);

	void SetDBAgent(DBAgent* _agent);
	void SetPacket(Packet* _packet);

	void LoopRun();

	int GetNum() { return m_num; }
	CONNECTOR_STATE GetState() { return m_state; }
};