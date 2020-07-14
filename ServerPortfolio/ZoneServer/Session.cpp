#include "Session.h"

Session::Session()
{
	m_failed = false;

	m_overlapped.Reset(this);

	m_socket = 0;
}

Session::~Session()
{

}