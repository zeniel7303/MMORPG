#pragma once
#include <iostream>

#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = NULL;} }

//우용이가 예전에 말했던 방식으로 링버퍼 새로 짜봄
//파싱 부분이 좀 더 내가 쓰던 것보다 탄탄함

//Init에서 총 버퍼 길이와 임시 버퍼 사이즈를 정함
//버퍼를 받다가 범위를 넘어가게되면 앞으로 당겨 사용
//패킷이 잘렸을 경우에 앞 부분 패킷만 memcpy해줌(1바이트만 들어오건뭐건 얼마나 잘리든간에 잘렸으면 그냥 memcpy하므로 문제 발생 x)

//20191118
//링버퍼가 제대로 짜여지지 않은 탓에 클라,서버가 전부 불안정했다. 고치니까 잘됨

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
	//쓰는 지점
	char*		writePoint;
	//버퍼의 시작 지점
	char*		bufferStartPoint;
	//버퍼의 끝 지점
	char*		bufferEndPoint;
	//버퍼 안 데이터 크기
	int			dataInBuffer;

public:
	RingBuffer();
	~RingBuffer();

	//Initializing
	void Init(int _size, int _tempSize);
	//초기화
	void Reset();
	//_size만큼 읽기
	void Write(int _size);
	//파싱 가능한지 여부 체크
	bool CanParsing();
	//파싱
	char* Parsing();

	//쓸 수 있는 버퍼의 크기 계산
	int GetWriteableSize();
	//링버퍼 안에 있는 데이터의 총 크기
	int GetDataInBuffer();
	//패킷 사이즈 
	unsigned short GetPacketSize();

	char* GetWritePoint() { return writePoint; }
	char* GetReadPoint() { return readPoint; }
	int GetBufferSize() { return bufferMaxSize; }
};

