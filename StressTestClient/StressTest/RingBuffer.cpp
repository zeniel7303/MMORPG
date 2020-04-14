#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
	buffer = nullptr;
	readPoint = nullptr;
	writePoint = nullptr;
	bufferStartPoint = nullptr;
	bufferEndPoint = nullptr;
	bufferMaxSize = 0;
	tempBufferSize = 0;
	dataInBuffer = 0;
}

RingBuffer::~RingBuffer()
{

}

void RingBuffer::Init(int _size, int _tempSize)
{
	bufferMaxSize = _size;
	tempBufferSize = _tempSize;

	buffer = new char[_size];
	if (buffer == nullptr) return;

	bufferStartPoint = buffer + tempBufferSize;
	bufferEndPoint = buffer + bufferMaxSize;
	writePoint = readPoint = bufferStartPoint;
}

void RingBuffer::Reset()
{
	SAFE_DELETE_ARRAY(buffer);

	readPoint = nullptr;
	writePoint = nullptr;
	bufferStartPoint = nullptr;
	bufferEndPoint = nullptr;
	bufferMaxSize = 0;
	tempBufferSize = 0;
	dataInBuffer = 0;
}

void RingBuffer::Write(int _size)
{
	writePoint += _size;

	if (writePoint >= bufferEndPoint)
	{
		writePoint = bufferStartPoint;
	}
}

bool RingBuffer::CanParsing()
{
	dataInBuffer = GetDataInBuffer();

	//링버퍼 안에 데이터가 2이상 있는가? (패킷 사이즈 읽을 수 있는 최소한의 크기 = 2)
	if (dataInBuffer >= 2)
	{
		return true;
	}

	//데이터가 없거나 1들어와서 크기조차 알 수 없을 경우
	return false;
}

char* RingBuffer::Parsing()
{
	//버퍼의 끝지점부터 읽는 지점까지의 사이즈
	int remainedSize = (int)(bufferEndPoint - readPoint);
	//파싱할 데이터는 읽는 지점부터 읽어온다.
	char* parsingData = readPoint;

	//버퍼의 끝지점부터 읽는 지점까지 사이즈가 1밖에 없는 경우 -> 이어줘야한다.
	if (remainedSize == 1)
	{
		char* tempBuffer = bufferStartPoint - 1;
		*tempBuffer = *readPoint;
		parsingData = readPoint = tempBuffer;

		readPoint += GetPacketSize();

		return parsingData;
	}
	//버퍼의 끝지점부터 읽는 지점이 같다 = 버퍼의 첫지점으로 읽는 지점을 옮겨야한다.
	else if (remainedSize == 0)
	{
		readPoint = bufferStartPoint;
	}

	unsigned short packetSize = GetPacketSize();

	if (packetSize > remainedSize)
	{
		memcpy(bufferStartPoint - remainedSize, readPoint, remainedSize);
		parsingData = readPoint = (bufferStartPoint - remainedSize);
	}

	readPoint += packetSize;
	return parsingData;
}

int RingBuffer::GetWriteableSize()
{
	if (readPoint > writePoint)
	{
		return (int)(readPoint - writePoint);
	}
	else
	{
		return (int)(bufferEndPoint - writePoint);
	}
}

int RingBuffer::GetDataInBuffer()
{
	if (readPoint > writePoint)
	{
		return (bufferEndPoint - readPoint) + (writePoint - bufferStartPoint);
	}
	else
	{
		return (int)(writePoint - readPoint);
	}
}

unsigned short RingBuffer::GetPacketSize()
{
	return *(unsigned short*)readPoint;
}