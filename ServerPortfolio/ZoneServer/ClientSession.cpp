#include "ClientSession.h"

ClientSession::ClientSession()
{
	m_failed = false;

	m_overlapped.state = IO_STATE::IO_RECV;

	TRYCATCH_CONSTRUCTOR(m_receiver = new Receiver(), m_failed);
	if (m_receiver->IsFailed() || m_failed)
	{
		m_failed = true;
		return;
	}
	TRYCATCH_CONSTRUCTOR(m_sendBuffer = new SendBuffer(10000), m_failed);
	if (m_failed) return;

	m_isTestClient = false;

	m_isConnected = false;
}

ClientSession::~ClientSession()
{
	if (m_receiver != nullptr) delete m_receiver;
	if (m_sendBuffer != nullptr) delete m_sendBuffer;
}

void ClientSession::OnConnect()
{
	m_isConnected = true;
	m_isActived = true;

	GenerateOverlappedIO();
}

void ClientSession::DisConnect()
{
	m_isConnected = false;
}

void ClientSession::Reset()
{
	m_overlapped.Reset(nullptr);
	m_overlapped.state = IO_STATE::IO_RECV;

	m_receiver->Reset();
	m_sendBuffer->Reset();

	m_isTestClient = false;

	m_isConnected = false;
	m_isActived = false;
}

void ClientSession::HandleOverlappedIO(ST_OVERLAPPED* _overlapped)
{
	/*switch (_overlapped->state)
	{
	case IO_RECV:
	{

	}
	break;
	case IO_SEND:
	{

	}
		break;
	default:
		//¿¹¿Ü
		break;
	}*/

	if (_overlapped == &m_overlapped)
	{
		if (m_overlapped.bytes <= 0)
		{
			//printf("[INFO] BYTES <= 0\n");

			//printf("2 \n");
			DisConnect();

			return;
		}

		m_receiver->Write(m_overlapped.bytes);

		OnRecv();

		GenerateOverlappedIO();
	}
}

void ClientSession::GenerateOverlappedIO()
{
	m_receiver->ASyncRecv(m_socket, m_overlapped.wsaBuffer, m_overlapped);
}

void ClientSession::Send(char* _data, DWORD _bytes)
{
	/*m_sendDataBuffer.buf = _data;
	m_sendDataBuffer.len = _bytes;

	if (WSASend(
		m_socket,
		&m_sendDataBuffer,
		1,
		&m_sendBytes,
		0,
		&m_sendOverlapped,
		NULL)
		== SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		int num = WSAGetLastError();
		if (num != WSA_IO_PENDING)
		{
			printf("SOCKET %d : SEND IO PENDING FAILURE %d\n", m_socket, num);

			//printf("6 \n");
			Disconnect();
		}
	}*/

	send(m_socket, _data, _bytes, 0);
}