#include "LogInSession.h"

#include "MainThread.h"
#include "DBConnector.h"

#include "ZoneConnector.h"

LogInSession::LogInSession()
{
	m_idx = 0;
	m_zoneNum = 0;

	m_heartBeatCheckedCount = 0;

	m_isInHashMap = false;
	m_isAlreadyDisConnected = false;
}

LogInSession::~LogInSession()
{
	DisConnect();
}

void LogInSession::OnConnect()
{
	m_isAlreadyDisConnected = false;

	ClientSession::OnConnect();

	m_start = std::chrono::high_resolution_clock::now();

	IsConnectedPacket* isConnectedPacket =
		reinterpret_cast<IsConnectedPacket*>(m_sendBuffer->
			GetBuffer(sizeof(IsConnectedPacket)));

	isConnectedPacket->Init(SendCommand::LogIn2C_ISCONNECTED, sizeof(IsConnectedPacket));
	isConnectedPacket->isConnected = m_isConnected;

	Send(reinterpret_cast<char*>(isConnectedPacket), isConnectedPacket->size);
}

void LogInSession::DisConnect()
{
	if (m_isAlreadyDisConnected) return;

	m_isAlreadyDisConnected = true;

	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	printf("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown 이후 close
	closesocket(m_socket); //순서상 다른 곳에서 해도된다.

	m_heartBeatCheckedCount = 0;

	if (m_isInHashMap)
	{
		UserNumPacket* DisConnectUserPacket = reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
		DisConnectUserPacket->Init(SendCommand::LogIn2Zone_DISCONNECT_USER, sizeof(UserNumPacket));
		DisConnectUserPacket->userIndex = m_idx;
		//m_sendBuffer->Write(DisConnectUserPacket->size);

		MainThread::getSingleton()->GetZoneServerManager()->GetZoneConnector(m_zoneNum)
			->Send(reinterpret_cast<char*>(DisConnectUserPacket), DisConnectUserPacket->size);
	}

	MainThread::getSingleton()->AddToDisConnectQueue(this);
}

void LogInSession::Reset()
{
	ClientSession::Reset();

	m_idx = 0;
	m_zoneNum = 0;

	m_isInHashMap = false;
}

void LogInSession::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		PacketHandle(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void LogInSession::PacketHandle(Packet* _packet)
{
	Packet* packet = _packet;

	switch (packet->cmd)
	{
	case 9:
	{
		RegisterUserPacket* registerUserPacket = reinterpret_cast<RegisterUserPacket*>(packet);

		RegisterUser(registerUserPacket);
	}		
		break;
	case 10:
	{
		LogInPacket* logInPacket = reinterpret_cast<LogInPacket*>(packet);

		LogInUser(logInPacket);
	}	
		break;
	case 15:
	{
		HeartBeatChecked();
	}
		break;
	case 16:
	{
		TestClientEnterPacket* testClientEnterPacket = 
			reinterpret_cast<TestClientEnterPacket*>(packet);

		m_idx = testClientEnterPacket->userNum;
		m_isTestClient = true;

		MainThread::getSingleton()->AddToHashMapQueue(this);
	}
		break;
	case 18:
	{
		ChangeZonePacket* changeZonePacket = reinterpret_cast<ChangeZonePacket*>(packet);
		
		ChangeZone(changeZonePacket->zoneNum);
	}
		break;
	case 20:
	{
		ZoneNumPacket* zoneNumPacket = reinterpret_cast<ZoneNumPacket*>(packet);

		m_zoneNum = zoneNumPacket->zoneNum;
		//MYDEBUG("%d \n", m_zoneNum);
	}
		break;
	}

	m_receiver->GetRingBuffer()->Read(packet->size);
}

void LogInSession::LogInUser(LogInPacket* _packet)
{
	LogInPacket_DBAgent* logInPacket_DBAgent =
		reinterpret_cast<LogInPacket_DBAgent*>(m_sendBuffer->
			GetBuffer(sizeof(LogInPacket_DBAgent)));
	logInPacket_DBAgent->Init(SendCommand::LogIn2DB_LOGIN, sizeof(LogInPacket_DBAgent));
	//m_sendBuffer->Write(logInPacket_DBAgent->size);
	logInPacket_DBAgent->socket = m_socket;
	strcpy_s(logInPacket_DBAgent->id, _packet->id);
	strcpy_s(logInPacket_DBAgent->password, _packet->password);
	m_zoneNum = _packet->zoneNum;

	DBConnector::getSingleton()->Send(reinterpret_cast<char*>(logInPacket_DBAgent), logInPacket_DBAgent->size);
}

void LogInSession::RegisterUser(RegisterUserPacket* _packet)
{
	RegisterPacket_DBAgent* registerPacket_DBAgent =
		reinterpret_cast<RegisterPacket_DBAgent*>(m_sendBuffer->
			GetBuffer(sizeof(RegisterPacket_DBAgent)));
	registerPacket_DBAgent->Init(SendCommand::LogIn2DB_REGISTER, sizeof(RegisterPacket_DBAgent));
	//m_sendBuffer->Write(registerPacket_DBAgent->size);
	registerPacket_DBAgent->socket = m_socket;
	strcpy_s(registerPacket_DBAgent->id, _packet->id);
	strcpy_s(registerPacket_DBAgent->password, _packet->password);

	DBConnector::getSingleton()->Send(reinterpret_cast<char*>(registerPacket_DBAgent), registerPacket_DBAgent->size);
}

//살아있는지 체크
void LogInSession::HeartBeatChecked()
{
	m_start = std::chrono::high_resolution_clock::now();

	if (m_heartBeatCheckedCount >= 30)
	{
		if (!m_isTestClient)
		{
			Packet* UpdateInfoPacket = reinterpret_cast<Packet*>(m_sendBuffer->
				GetBuffer(sizeof(Packet)));
			UpdateInfoPacket->Init(SendCommand::LogIn2C_UPDATE_INFO, sizeof(Packet));
			//m_sendBuffer->Write(UpdateInfoPacket->size);

			Send(reinterpret_cast<char*>(UpdateInfoPacket), UpdateInfoPacket->size);
		}

		m_heartBeatCheckedCount = 0;

		MYDEBUG("[ HeartBeat Checking & Update : user %d ]\n", m_idx);
	}
	else
	{
		m_heartBeatCheckedCount++;

		//printf("[ HeartBeat Checking Success : %d ]\n", m_heartBeatCheckedCount);
	}

	//MYDEBUG("[ Check %d ]\n", m_heartBeatCheckedCount);
}

void LogInSession::LogInDuplicated()
{
	Packet* LogInFailed = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	LogInFailed->Init(SendCommand::Zone2C_LOGIN_FAILED_DUPLICATED, sizeof(Packet));
	//m_sendBuffer->Write(LogInFailed->size);

	Send(reinterpret_cast<char*>(LogInFailed), LogInFailed->size);

	//DisConnect();
}

void LogInSession::LogInSuccess(int _num)
{
	m_idx = _num;

	LogInSuccessPacket_PortNum* LogInSuccess = reinterpret_cast<LogInSuccessPacket_PortNum*>(m_sendBuffer->
		GetBuffer(sizeof(LogInSuccessPacket_PortNum)));
	LogInSuccess->Init(SendCommand::Zone2C_LOGIN_SUCCESS, sizeof(LogInSuccessPacket_PortNum));
	LogInSuccess->userIndex = m_idx;
	LogInSuccess->portNum = 30005;
	//m_sendBuffer->Write(LogInSuccess->size);
	MYDEBUG("[ Login Success ] \n");

	Send(reinterpret_cast<char*>(LogInSuccess), LogInSuccess->size);
}

void LogInSession::LogInFailed()
{
	Packet* LogInFailed = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	LogInFailed->Init(SendCommand::Zone2C_LOGIN_FAILED, sizeof(Packet));
	//m_sendBuffer->Write(LogInFailed->size);
	MYDEBUG("[ Login Failed ] \n");

	Send(reinterpret_cast<char*>(LogInFailed), LogInFailed->size);

	//DisConnect();
}

void LogInSession::RegisterSuccess()
{
	Packet* RegisterSuccessPacket = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	RegisterSuccessPacket->Init(SendCommand::Zone2C_REGISTER_USER_SUCCESS, sizeof(Packet));
	//m_sendBuffer->Write(RegisterSuccessPacket->size);

	Send(reinterpret_cast<char*>(RegisterSuccessPacket), RegisterSuccessPacket->size);

	//DisConnect();
}

void LogInSession::RegisterFailed()
{
	Packet* RegisterFailedPacket = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	RegisterFailedPacket->Init(SendCommand::Zone2C_REGISTER_USER_FAILED, sizeof(Packet));
	//m_sendBuffer->Write(RegisterFailedPacket->size);

	Send(reinterpret_cast<char*>(RegisterFailedPacket), RegisterFailedPacket->size);

	//DisConnect();
}

void LogInSession::ChangeZone(int _num)
{
	ZoneConnector* zoneConnector = MainThread::getSingleton()->GetZoneServerManager()->GetZoneConnector(_num);

	if (zoneConnector == nullptr)
	{
		//없는 존

		return;
	}

	ZoneConnector* prevZoneConnector = MainThread::getSingleton()->GetZoneServerManager()->GetZoneConnector(m_zoneNum);
	
	ChangeZonePacket* changeZonePacket = reinterpret_cast<ChangeZonePacket*>(m_sendBuffer->
		GetBuffer(sizeof(ChangeZonePacket)));
	changeZonePacket->Init(SendCommand::Login2Zone_DISCONNECT_USER_CHANGE_ZONE, sizeof(ChangeZonePacket));
	changeZonePacket->zoneNum = _num;
	changeZonePacket->userIndex = m_idx;
	//m_sendBuffer->Write(DisConnectUserPacket->size);

	prevZoneConnector->Send(reinterpret_cast<char*>(changeZonePacket), changeZonePacket->size);

	m_zoneNum = _num;
}