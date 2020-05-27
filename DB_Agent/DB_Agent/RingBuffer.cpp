#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
	buffer = nullptr;
	readPoint = nullptr;
	writePoint = nullptr;
	bufferStartPoint = nullptr;
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

	bufferStartPoint = readPoint = writePoint;
	dataInBuffer = 0;
}

void RingBuffer::Write(int _size)
{
	writePoint += _size;

	if (writePoint >= (bufferEndPoint))
	{
		writePoint = bufferStartPoint;
	}
}

void RingBuffer::Read(int _size)
{
	readPoint += _size;

	if (readPoint >= bufferEndPoint)
	{
		readPoint = bufferStartPoint;
	}
}

char* RingBuffer::CanParsing()
{
	dataInBuffer = GetDataInBuffer();

	//링버퍼 안에 데이터가 2이상 있는가? (패킷 사이즈 읽을 수 있는 최소한의 크기 = 2)
	if (dataInBuffer >= 2)
	{
		//링버퍼 안에 PacketSize를 읽어올 수 있으므로 그 이상만큼 들어와있는 데이터가 존재한다면 Parsing();
		if (dataInBuffer >= GetPacketSize())
		{
			return Parsing();
		}

		//데이터가 2이상있지만 패킷 사이즈만큼 충분히 받지 못할 경우
		//return nullptr;
	}

	//데이터가 없거나 1들어와서 크기조차 알 수 없을 경우
	return nullptr;
}

char* RingBuffer::Parsing()
{
	//버퍼의 끝지점부터 읽는 지점까지의 사이즈
	int remainedSize = (int)(bufferEndPoint - readPoint);
	//파싱할 데이터는 읽는 지점부터 읽어온다.
	char* parsingData = readPoint;

	//버퍼의 끝지점부터 읽는 지점까지 사이즈가 1밖에 없는 경우 -> TempBuffer쪽으로 복사함
	if (remainedSize == 1)
	{
		char* tempBuffer = bufferStartPoint - 1;
		*tempBuffer = *readPoint;
		parsingData = readPoint = tempBuffer;
		//이후 readPoint는 패킷 사이즈만큼 더해준다.
		readPoint += GetPacketSize();

		return parsingData;
	}
	//버퍼의 끝지점부터 읽는 지점이 같다 -> 버퍼의 첫지점으로 읽는 지점을 옮겨야한다.
	else if (remainedSize == 0)
	{
		readPoint = bufferStartPoint;
	}

	WORD packetSize = GetPacketSize();

	//패킷 사이즈가 남아있는 크기보다 크다. -> 끝이 짤렸다.
	if (packetSize > remainedSize)
	{
		//맨 앞으로 복사한다.
		memcpy(bufferStartPoint - remainedSize, readPoint, remainedSize);
		parsingData = readPoint = (bufferStartPoint - remainedSize);
	}

	readPoint += packetSize;

	//테스트용
	/*if (this->GetDataInBuffer() > 0)
	{
		printf("%d \n", this->GetDataInBuffer());
	}*/

	return parsingData;
}

DWORD RingBuffer::GetWriteableSize()
{
	//평범한 경우
	if (readPoint < writePoint)
	{
		return bufferEndPoint - writePoint;
	}
	else if (readPoint == writePoint)
	{
		//readPoint와 writePoint가 같은데
		//데이터가 있는 경우 == 한바퀴 돌아서 같아짐
		if (this->GetDataInBuffer() > 0)
		{
			return 0;
		}
		//readPoint와 writePoint가 같은데
		//데이터가 없는 경우 == ringBuffer가 데이터를 받는 시점이거나 처리할게 없음(문제X)
		else
		{
			return bufferEndPoint - writePoint;
		}
	}
	else
	{
		return readPoint - writePoint;
	}
}

DWORD RingBuffer::GetReadableSize()
{
	if (readPoint <= writePoint)
	{
		return writePoint - readPoint;
	}
	else
	{
		return bufferEndPoint - readPoint;
	}

}

DWORD RingBuffer::GetDataInBuffer()
{
	if (readPoint > writePoint)
	{
		return bufferEndPoint - readPoint + (writePoint - bufferStartPoint);
	}
	else
	{
		return writePoint - readPoint;
	}
}

WORD RingBuffer::GetPacketSize()
{
	return *(WORD*)readPoint;
}