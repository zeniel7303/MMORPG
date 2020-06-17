#pragma once
#include <winsock2.h>

#include "../ServerLibrary/HeaderFiles/RingBuffer.h"
#include "SendBuffer.h"

//세션은 로그인 정보 유지를 위해 사용한다.

//=====================================================

//Session

//User의 기본이 되는 클래스

//=====================================================

//MMORPG 서버구조
//https://www.youtube.com/watch?v=yk-HD8YoyZg

//게임서버디자인 가이드
//https://www.slideshare.net/devcatpublications/ndc2013-19986939

enum FLAGCOMMAND : WORD
{
	IOCP_ACCEPT	=	1000,
	IOCP_RECV	=	2000,
	IOCP_SEND	=	3000
};

//구조체 확장
//overlapped
struct ST_OVERLAPPED : public WSAOVERLAPPED
{
	class Session*	session;
	FLAGCOMMAND		flag;
};

class Session
{
protected:
	SOCKET					m_socket;

	DWORD					m_recvBytes;
	DWORD					m_flags;
	DWORD					m_sendBytes;

	ST_OVERLAPPED			m_recvOverlapped;
	WSABUF					m_recvDataBuffer;

	ST_OVERLAPPED			m_sendOverlapped;
	WSABUF					m_sendDataBuffer;

	RingBuffer*				m_recvBuffer;
	SendBuffer*				m_sendBuffer;

	bool					m_isConnected;
	bool					m_isCheckingHeartBeat;
	bool					m_startCheckingHeartBeat;
	int						m_heartBeatCheckedCount;

private:

public:
	Session();
	~Session();

	virtual void Init();
	virtual void OnConnect(SOCKET _socket);
	virtual void Disconnect();
	virtual void Reset();
	//RECV SEND 체크용
	virtual void CheckCompletion(ST_OVERLAPPED* _overlapped, DWORD _bytes) = 0;

	void Recv();
	void Send(char* _data, DWORD _bytes);
	void ReSend();

	//ST_OVERLAPPED* GetRecvOverlapped() { return &m_recvOverlapped; }
	//ST_OVERLAPPED* GetSendOverlapped() { return &m_sendOverlapped; }

	RingBuffer* GetRecvBuffer() { return m_recvBuffer; }
	SendBuffer* GetSendBuffer() { return m_sendBuffer; }

	bool GetIsConnected() { return m_isConnected; }
	void SetIsConnected(bool _bool) { m_isConnected = _bool; }

	SOCKET GetSocket() { return m_socket; }

	bool GetIsChecking() { return m_isCheckingHeartBeat; }
	void SetIsChecking(bool _bool) { m_isCheckingHeartBeat = _bool; }

	void StartCheckingHeartBeat() { m_startCheckingHeartBeat = true; }
	bool GetStartCheckingHeartBeat() { return m_startCheckingHeartBeat; }
};