#pragma once
#include <winsock2.h>
#include <MSWSock.h>

#pragma comment(lib, "mswsock.lib")

#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

class Acceptor
{
public:
	enum IO_STATE : WORD
	{
		IO_ACCEPT = 1000,
		IO_RECV = 2000,
		IO_SEND = 3000
	};

	//구조체 확장
	//overlapped
	struct ST_OVERLAPPED : public WSAOVERLAPPED
	{
		class Session*	session;
		WSABUF			wsaBuffer;
		IO_STATE		state;
		DWORD			bytes;

		void Reset(Session* _session)
		{
			hEvent = 0;
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
			Pointer = 0;

			session = _session;
			state = IO_STATE::IO_ACCEPT;
			bytes = 0;
		}
	};

private:
	ST_OVERLAPPED			m_overlapped;
	SOCKET					m_listenSocket;

	char*					m_byteBuffer;

	int						m_locallen;
	int						m_remotelen;
	sockaddr_in*			m_plocal;
	sockaddr_in*			m_premote;

public:
	Acceptor();
	~Acceptor();

	void Init(Session* _session);
	void SetListenSocket(SOCKET _listenSocket);
	void Reset();
	bool Start(SOCKET _socket);
	void GetAccept(SOCKET _socket);

	void SetOverlappedState(IO_STATE _state) { m_overlapped.state = _state; }
	ST_OVERLAPPED* GetOverlapped() { return &m_overlapped; }
};

