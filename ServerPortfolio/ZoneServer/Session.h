#pragma once
#include <winsock2.h>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"

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
		state = IO_NONE;
		bytes = 0;
	}
};

class Session
{
protected:
	bool					m_failed;

	SOCKET					m_socket;

	ST_OVERLAPPED			m_overlapped;

private:

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
};