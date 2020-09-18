#pragma once
#pragma warning(disable:4005)

#include <winsock2.h>
#include "../HeaderFiles/FileLog.h"
#include "../HeaderFiles/Utils.h"

enum IO_STATE : WORD
{
	IO_NONE = 0,
	IO_ACCEPT = 1000,
	IO_RECV = 2000,
	IO_SEND = 3000
};

//구조체 확장
//overlapped
struct ST_OVERLAPPED : public WSAOVERLAPPED
{
	class Session*	session;
	IO_STATE		state;
	int				count;
};

class Session
{
protected:
	//생성자에서 사용되는 bool값
	bool					m_failed;

	SOCKET					m_socket;

	ST_OVERLAPPED			m_overlapped;

	DWORD					bytes;

public:
	Session();
	virtual ~Session();

	//RECV SEND 체크용
	virtual void HandleOverlappedIO(ST_OVERLAPPED* _overlapped) = 0;
	virtual void GenerateOverlappedIO() = 0;
	virtual void DisConnect() = 0;

	bool IsFailed() { return m_failed; }

	void SetSocket(SOCKET _socket) { m_socket = _socket; }
	SOCKET GetSocket() { return m_socket; }
	void SetBytes(DWORD _bytes) { bytes = _bytes; }
};

