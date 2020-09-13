#pragma once

//CircBuffer를 아직 송신용으로 안써봐서 만든 임시 sendbuffer
class SendBuffer
{
private:
	char*	sendBuffer;
	char*	sendBufferEnd;
	char*	writePoint;

	int		bufferLength;

public:
	SendBuffer()
	{

	}

	~SendBuffer()
	{

	}

	void Init(int _size)
	{
		bufferLength = _size;

		sendBuffer = new char[bufferLength];

		writePoint = sendBuffer;
		sendBufferEnd = sendBuffer + bufferLength;
	}

	void Reset()
	{
		//SAFE_DELETE_ARRAY(sendBuffer);
	}

	char* GetBuffer(int _size)
	{
		//m_locker.EnterLock();

		if (_size > sendBufferEnd - writePoint)
		{
			writePoint = sendBuffer;
		}

		//m_locker.LeaveLock();

		return writePoint;
	}

	char* GetWritePoint()
	{
		return writePoint;
	}

	void Write(int _size)
	{
		//m_locker.EnterLock();

		writePoint += _size;

		//m_locker.LeaveLock();
	}
};

