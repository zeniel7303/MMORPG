//#pragma once
//#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
//#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
//#include "../ServerLibrary/HeaderFiles/ClientSession.h"
//#include "../ServerLibrary/HeaderFiles/IOCPClass.h"
//
//#include "packet.h"
//
//#include "MainThread.h"
//
//class PathFinderAgent : public ClientSession
//{
//public:
//	static PathFinderAgent* getSingleton()
//	{
//		static PathFinderAgent singleton;
//
//		return &singleton;
//	}
//
//private:
//	IpEndPoint					m_ipEndPoint;
//
//	IOCPClass*					m_IOCPClass;
//
//	PathFinderAgent();
//public:
//	~PathFinderAgent();
//
//	bool Connect(const char* _ip, const unsigned short _portNum, IOCPClass* _iocpClass);
//	void OnConnect();
//	void DisConnect();
//	void Reset();
//
//	void OnRecv();
//
//	void SendPathFindPacket(int _index, int _fieldNum, unsigned short _state,
//		int _nowTileX, int _nowTileY, int _targetTileX, int _targetTileY);
//};