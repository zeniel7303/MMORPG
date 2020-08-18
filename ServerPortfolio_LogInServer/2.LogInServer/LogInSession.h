#pragma once
#include "../ServerLibrary/HeaderFiles/ClientSession.h"

#include "packet.h"

class LogInSession : public ClientSession
{
public:
	LogInSession();
	~LogInSession();

	void OnConnect();
	void DisConnect();
	void OnRecv();

	void LogInUser(LogInPacket* _packet);
	void RegisterUser(RegisterUserPacket* _packet);

	void LogInSuccess(int _num);
	void LogInFailed();

	void RegisterSuccess();
	void RegisterFailed();
};

