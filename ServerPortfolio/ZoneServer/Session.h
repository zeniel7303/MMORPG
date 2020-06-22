#pragma once
#include <winsock2.h>

#include "SendBuffer.h"

#include "Acceptor.h"
#include "Receiver.h"

//세션은 로그인 정보 유지를 위해 사용한다.

//=====================================================

//Session

//User의 기본이 되는 클래스

//=====================================================

//MMORPG 서버구조
//https://www.youtube.com/watch?v=yk-HD8YoyZg

//게임서버디자인 가이드
//https://www.slideshare.net/devcatpublications/ndc2013-19986939

#define MYOVERLAPPED m_acceptor->GetOverlapped()

class Session
{
protected:
	int						m_index;
	SOCKET					m_socket;

	Acceptor*				m_acceptor;
	Receiver*				m_receiver;
	SendBuffer*				m_sendBuffer;

	bool					m_isConnected;

private:

public:
	Session();
	virtual ~Session();

	virtual void Init(SOCKET _listenSocket);
	virtual void OnConnect();
	virtual void Disconnect();
	virtual void Reset();
	//RECV SEND 체크용
	virtual void CheckCompletion(Acceptor::ST_OVERLAPPED* _overlapped) = 0;

	void StartAccept();
	void AssociateIOCP(HANDLE _handle);

	void Recv();
	void Send(char* _data, DWORD _bytes);
	void ReSend();

	SendBuffer* GetSendBuffer() { return m_sendBuffer; }

	Acceptor* GetAcceptor() { return m_acceptor; }

	int GetIndex() { return m_index; }

	SOCKET GetSocket() { return m_socket; }

	bool IsConnected() { return m_isConnected; }
	void SetConnected(bool _bool) { m_isConnected = _bool; }	
};