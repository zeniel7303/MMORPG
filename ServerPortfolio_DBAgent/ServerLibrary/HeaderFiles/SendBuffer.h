#pragma once

#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

//CircBuffer를 아직 송신용으로 안써봐서 만든 임시 sendbuffer
class SendBuffer
{
private:
	char*	sendBuffer;
	char*	sendBufferEnd;
	char*	writePoint;

	int		bufferLength;

public:
	SendBuffer(int _size)
	{
		bufferLength = _size;

		sendBuffer = new char[bufferLength];

		writePoint = sendBuffer;
		sendBufferEnd = sendBuffer + bufferLength;
	}

	~SendBuffer()
	{
		SAFE_DELETE_ARRAY(sendBuffer);
	}

	void Reset()
	{
		//SAFE_DELETE_ARRAY(sendBuffer);
	}

	char* GetBuffer(int _size)
	{
		if (_size > sendBufferEnd - writePoint)
		{
			writePoint = sendBuffer;
		}

		return writePoint;
	}

	char* GetWritePoint()
	{
		return writePoint;
	}

	void Write(int _size)
	{
		writePoint += _size;

		if (writePoint >= sendBufferEnd)
		{
			writePoint = sendBuffer;
		}
	}
};

