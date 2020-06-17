#pragma once
#include <winsock2.h>

#include "../ServerLibrary/HeaderFiles/RingBuffer.h"
#include "SendBuffer.h"
#include "Acceptor.h"

//세션은 로그인 정보 유지를 위해 사용한다.

//=====================================================

//Session

//User의 기본이 되는 클래스

//=====================================================

//MMORPG 서버구조
//https://www.youtube.com/watch?v=yk-HD8YoyZg

//게임서버디자인 가이드
//https://www.slideshare.net/devcatpublications/ndc2013-19986939

enum IO_STATE : WORD
{
	IO_ACCEPT	=	1000,
	IO_RECV		=	2000,
	IO_SEND		=	3000
};

//구조체 확장
//overlapped
struct ST_OVERLAPPED : public WSAOVERLAPPED
{
	class Session*	session;
	WSABUF			wsaBuffer;
	IO_STATE		state;
	DWORD			bytes;
};

class Session
{
protected:
	SOCKET					m_socket;

	DWORD					m_recvBytes;
	DWORD					m_flags;
	DWORD					m_sendBytes;

	ST_OVERLAPPED			m_overlapped;

	RingBuffer*				m_recvBuffer;
	SendBuffer*				m_sendBuffer;

	Acceptor*				m_acceptor;

	HANDLE					m_hEvent;

	int						m_index;

	bool					m_isConnected;

	bool					m_isCheckingHeartBeat;
	bool					m_startCheckingHeartBeat;
	int						m_heartBeatCheckedCount;

private:

public:
	Session();
	virtual ~Session();

	virtual void Init(SOCKET _listenSocket, HANDLE _handle);
	virtual void OnConnect();
	virtual void Disconnect();
	virtual void Reset();
	//RECV SEND 체크용
	virtual void CheckCompletion(ST_OVERLAPPED* _overlapped) {};

	void StartAccept();
	void AssociateIOCP(HANDLE _handle);

	void Recv();
	void Send(char* _data, DWORD _bytes);
	void ReSend();

	RingBuffer* GetRecvBuffer() { return m_recvBuffer; }
	SendBuffer* GetSendBuffer() { return m_sendBuffer; }

	Acceptor* GetAcceptor() { return m_acceptor; }

	int GetIndex() { return m_index; }
	ST_OVERLAPPED* GetOverlapped() { return &m_overlapped; }

	SOCKET GetSocket() { return m_socket; }

	bool IsConnected() { return m_isConnected; }
	void SetConnected(bool _bool) { m_isConnected = _bool; }

	bool IsChecking() { return m_isCheckingHeartBeat; }
	void SetChecking(bool _bool) { m_isCheckingHeartBeat = _bool; }

	bool GetStartCheckingHeartBeat() { return m_startCheckingHeartBeat; }
	void StartCheckingHeartBeat() { m_startCheckingHeartBeat = true; }
};