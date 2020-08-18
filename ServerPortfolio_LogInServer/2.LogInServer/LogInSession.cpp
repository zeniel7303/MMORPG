#include "LogInSession.h"

#include "MainThread.h"
#include "DBConnector.h"

LogInSession::LogInSession()
{
}


LogInSession::~LogInSession()
{
}

void LogInSession::OnConnect()
{
	ClientSession::OnConnect();

	IsConnectedPacket* isConnectedPacket =
		reinterpret_cast<IsConnectedPacket*>(m_sendBuffer->
			GetBuffer(sizeof(IsConnectedPacket)));

	isConnectedPacket->Init(SendCommand::LogIn2C_ISCONNECTED, sizeof(IsConnectedPacket));
	isConnectedPacket->isConnected = m_isConnected;

	Send(reinterpret_cast<char*>(isConnectedPacket), isConnectedPacket->size);
}

void LogInSession::DisConnect()
{
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

	MainThread::getSingleton()->AddToDisConnectQueue(this);
}

void LogInSession::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		//상수로 넣는것(상수니까 주소바뀔 수 없다.)
		MainThread::getSingleton()->AddToUserPacketQueue({ this, packet });

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
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

void LogInSession::LogInSuccess(int _num)
{
	LogInSuccessPacket* LogInSuccess = reinterpret_cast<LogInSuccessPacket*>(m_sendBuffer->
		GetBuffer(sizeof(LogInSuccessPacket)));
	LogInSuccess->Init(SendCommand::Zone2C_LOGIN_SUCCESS, sizeof(LogInSuccessPacket));
	LogInSuccess->userIndex = _num;
	//m_sendBuffer->Write(LogInSuccess->size);
	MYDEBUG("[ Login Success ] \n");

	Send(reinterpret_cast<char*>(LogInSuccess), LogInSuccess->size);

	DisConnect();
}

void LogInSession::LogInFailed()
{
	Packet* LogInFailed = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	LogInFailed->Init(SendCommand::Zone2C_LOGIN_FAILED, sizeof(Packet));
	//m_sendBuffer->Write(LogInFailed->size);
	MYDEBUG("[ Login Failed ] \n");

	Send(reinterpret_cast<char*>(LogInFailed), LogInFailed->size);

	DisConnect();
}

void LogInSession::RegisterSuccess()
{
	Packet* RegisterSuccessPacket = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	RegisterSuccessPacket->Init(SendCommand::Zone2C_REGISTER_USER_SUCCESS, sizeof(Packet));
	//m_sendBuffer->Write(RegisterSuccessPacket->size);

	Send(reinterpret_cast<char*>(RegisterSuccessPacket), RegisterSuccessPacket->size);

	DisConnect();
}

void LogInSession::RegisterFailed()
{
	Packet* RegisterFailedPacket = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	RegisterFailedPacket->Init(SendCommand::Zone2C_REGISTER_USER_FAILED, sizeof(Packet));
	//m_sendBuffer->Write(RegisterFailedPacket->size);

	Send(reinterpret_cast<char*>(RegisterFailedPacket), RegisterFailedPacket->size);

	DisConnect();
}