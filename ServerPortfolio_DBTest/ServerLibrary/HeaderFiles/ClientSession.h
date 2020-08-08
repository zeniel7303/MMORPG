#pragma once
#include "../HeaderFiles/Session.h"
#include "../HeaderFiles/Receiver.h"
#include "../HeaderFiles/SendBuffer.h"

class ClientSession : public Session
{
protected:
	Receiver*			m_receiver;
	SendBuffer*			m_sendBuffer;

	bool				m_isTestClient;
	bool				m_isConnected;
	bool				m_isActived;

	bool				m_ConstructorBool;

public:
	ClientSession();
	virtual ~ClientSession();

	//연결
	void OnConnect();
	//연결 해제
	virtual void DisConnect() override;
	//초기화(재사용위함)
	void Reset();
	virtual void HandleOverlappedIO(ST_OVERLAPPED* _overlapped) override;
	virtual void GenerateOverlappedIO() override;
	//파싱용
	virtual void OnRecv() = 0;

	void Send(char* _data, DWORD _bytes);

	SendBuffer* GetSendBuffer() { return m_sendBuffer; }

	bool IsTestClient() { return m_isTestClient; }

	bool IsConnected() { return m_isConnected; }
	void SetConnected(bool _bConnected) { m_isConnected = _bConnected; }
};

