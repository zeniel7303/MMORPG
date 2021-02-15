#include "../HeaderFiles/ClientSession.h"
#include "../../ZoneServer/packet.h"

ClientSession::ClientSession()
{
	m_failed = false;

	TRYCATCH_CONSTRUCTOR(m_receiver = new Receiver(), m_failed);
	if (m_receiver->IsFailed() || m_failed)
	{
		m_failed = true;
		return;
	}

	TRYCATCH_CONSTRUCTOR(m_sender = new Sender(), m_failed);
	if (m_failed) return;

	m_isTestClient = false;

	m_isConnected = false;
}

ClientSession::~ClientSession()
{
	if (m_sender != nullptr) delete m_sender;
	if (m_receiver != nullptr) delete m_receiver;
}

void ClientSession::OnConnect()
{
	m_isConnected = true;
	m_isActived = true;

	m_recvOverlapped.state = IO_STATE::IO_RECV;
	m_sendOverlapped.state = IO_STATE::IO_SEND;

	GenerateOverlappedIO(&m_recvOverlapped);
}

void ClientSession::DisConnect()
{
	m_isConnected = false;
}

void ClientSession::Reset()
{
	m_isConnected = false;

	memset(&m_recvOverlapped, 0, sizeof(struct ST_OVERLAPPED));
	m_recvOverlapped.session = nullptr;
	m_recvOverlapped.state = IO_STATE::IO_RECV;

	memset(&m_sendOverlapped, 0, sizeof(struct ST_OVERLAPPED));
	m_sendOverlapped.session = nullptr;
	m_sendOverlapped.state = IO_STATE::IO_SEND;

	m_receiver->Reset();
	m_sender->Reset();

	m_isTestClient = false;

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
		//예외
		break;
	}*/

	if (bytes <= 0)
	{
		MYDEBUG("[INFO] BYTES <= 0\n");

		DisConnect();

		/*if (bytes < 0)

			DisConnect();
		}*/

		return;
	}

	/*if (bytes == 0)
	{
		MYDEBUG("[INFO] %d BYTES == 0\n", m_socket);
	}*/

	switch (_overlapped->state)
	{
	case IO_RECV:
	{
		m_receiver->Write(bytes);

		OnRecv();

		GenerateOverlappedIO(_overlapped);
	}
	break;
	case IO_SEND:
	{
		m_sender->m_spinLock.Enter();

		if (!m_sender->m_queue.empty())
		{
			/*char* pChar = m_sender->m_queue.front().GetPointer();
			WORD tempBytes = *(WORD*)pChar;

			if (tempBytes > bytes)
			{
				m_sender->m_queue.front().SetPointer((WORD)bytes);
				m_sender->m_bytes = tempBytes;
			}
			else
			{
				m_sender->m_queue.pop();
				m_sender->m_bytes = 0;
			}*/

			m_sender->m_queue.pop();
			//m_sender->m_bytes = 0;
		}

		GenerateOverlappedIO(_overlapped);

		m_sender->m_spinLock.Leave();
	}
	break;
	default:
		//예외
		break;
	}

	InterlockedIncrement((LPLONG)&_overlapped->count);
}

void ClientSession::GenerateOverlappedIO(ST_OVERLAPPED* _overlapped)
{
	switch (_overlapped->state)
	{
	case IO_RECV:
	{
		if (!m_receiver->ASyncRecv(m_socket, *_overlapped))
		{
			DisConnect();
		}
	}
	break;
	case IO_SEND:
	{
		if (!m_sender->ASyncSend(m_socket, *_overlapped))
		{
			DisConnect();
		}
	}
	break;
	default:
		//예외
		break;
	}
}

//void ClientSession::Send(char* _data, DWORD _bytes)
//{
//	WORD size = *(WORD*)_data;
//	printf("size : %d \n", _bytes);
//
//	int size = send(m_socket, _data, _bytes, 0);
//	printf("size : %d \n", size);
//}

void ClientSession::AddToSendQueue(SharedPointer<char>& sharedptr)
{
	m_sender->m_spinLock.Enter();

	if (!m_isConnected)
	{
		m_sender->m_spinLock.Leave();

		return;
	}

	m_sender->m_queue.emplace(sharedptr);

	if (!m_sender->m_isSend)
	{
		GenerateOverlappedIO(&m_sendOverlapped);
	}

	m_sender->m_spinLock.Leave();
}