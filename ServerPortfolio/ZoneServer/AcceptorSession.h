#pragma once
#include "Session.h"

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"

#include <winsock2.h>
#include <MSWSock.h>

#pragma comment(lib, "mswsock.lib")

#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

class AcceptorSession : public Session
{
private:
	int						m_tempNum;

	HANDLE					m_hIOCP;
	IpEndPoint				m_ipEndPoint;
	//밖에서 생성해서 가져오기
	SOCKET					m_listenSocket;

	char*					m_byteBuffer;
	int						m_locallen;
	int						m_remotelen;
	sockaddr_in*			m_plocal;
	sockaddr_in*			m_premote;

public:
	AcceptorSession(const char* _ip, const unsigned short _port, HANDLE _handle, int _num);
	virtual ~AcceptorSession();

	void Reset();

	virtual void HandleOverlappedIO(ST_OVERLAPPED* _overlapped) override;
	virtual void GenerateOverlappedIO() override;
	virtual void DisConnect() override;
	virtual void OnAccept() = 0;

	SOCKET GetListenSocket() { return m_listenSocket; }
};

