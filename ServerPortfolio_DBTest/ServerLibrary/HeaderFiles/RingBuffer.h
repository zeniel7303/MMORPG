#pragma once
#include <Windows.h>
#include <iostream>

#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

//참고 (확실한 이해가 안되서 크게 참고는 안됨)
//https://github.com/jacking75/NetworkProg_Projects/tree/87e98bc00a5c3383281e3ca6178c0f6b4ce51a69
//nhn->iocpnetlib->circularBuffer.h

//=====================================================

//RingBuffer

//원형버퍼이다. 총 버퍼 크기는 Init에서 정해준다. 그리고 데이터 이어붙이기용 TempBuffer를 둔다.
//총 버퍼의 크기와 별개로 실제로 사용중인 버퍼의 크기는 다르다.
//버퍼를 계속 이어붙여가며 받다가 총 버퍼 크기를 넘어가게되면 맨 앞부분으로 돌아가서 다시 받는다.
//패킷이 링버퍼의 맨 앞부분과 맨 뒷부분으로 짤려셔 들어오게되면 ringBuffer의 앞부분으로 복사해 붙여준다.
//(패킷이 잘린걸 확인되면 무조건 붙이므로 몇바이트가 잘린지는 중요하지 않다.)

/*
						  readPoint        writePoint
							 ㅣ			       ㅣ
				 ◆◆◆◆◆◆◆◇◇◇◇◇★★★★★				★ = ringBuffer의 크기를 넘어서는 패킷의 잘리는 부분
							↓
	readPoint        writePoint
	   ㅣ				 ㅣ
		★★★★★◆◆◆◆◆◆◆◆◆◆◆◆
*/

//=====================================================

class RingBuffer
{
private:
	//버퍼 사이즈
	int			bufferMaxSize;
	//임시 저장 버퍼 사이즈
	int			tempBufferSize;
	//버퍼 주소
	char*		buffer;
	//읽는 지점
	char*		readPoint;
	//파싱 지점
	char*		parsingPoint;
	//쓰는 지점
	char*		writePoint;
	//버퍼의 시작 지점
	char*		bufferStartPoint;
	//버퍼의 끝 지점
	char*		bufferEndPoint;
	//버퍼 안 데이터 크기
	int			dataInBuffer;

public:
	//RingBuffer(const RingBuffer&) = delete;
	RingBuffer(int _size, int _tempSize);
	~RingBuffer();

	//초기화
	void Reset();
	//_size만큼 쓰기,읽기
	void Write(int _size);
	void MoveParsingPoint(int _size);
	void Read(int _size);
	//파싱 가능한지 여부 체크
	char* CanParsing();
	//파싱
	char* Parsing();

	//쓸 수 있는, 읽을 수 있는 버퍼의 크기 계산
	DWORD GetWriteableSize();
	DWORD GetReadableSize();
	//링버퍼 안에 있는 데이터의 총 크기
	DWORD GetDataInBuffer();
	//패킷 사이즈 
	WORD GetPacketSize();

	char* GetWritePoint() { return writePoint; }
	char* GetReadPoint() { return readPoint; }
	int GetBufferSize() { return bufferMaxSize; }
};

