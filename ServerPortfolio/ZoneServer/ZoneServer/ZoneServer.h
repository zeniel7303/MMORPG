#pragma once
#include <memory>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/IOCPClass.h"

#include "User.h"
#include "UserManager.h"
#include "FieldManager.h"

#include "DBConnector.h"
#include "LogInConnector.h"
#include "PathFinderAgent.h"

#include "MainThread.h"

//https://chipmaker.tistory.com/entry/dd-1

//unique_ptr
//받는쪽(함수)로 완전히 자원을 넘겨야 할 경우
//함수파라미터를 값이나 rvalue로 선언한다.
//rvalue로 선언할 경우, 받는쪽에서 move를 통해서 자원을 가져가야 자원이동이 완료된다.
//
//전달한 쪽에서도 계속 자원을 사용해야 할 경우
//함수파라미터를 참조로 선언해야 한다.

class ZoneServer
{
private:
	const int				USERMAXCOUNT = 2000;

	IOCPClass&				m_IOCPClass;

	UserManager*			m_userManager;
	FieldManager*			m_fieldManager;

public:
	ZoneServer(IOCPClass& _iocpClass)
		: m_IOCPClass(_iocpClass)
	{

	};

	~ZoneServer();

	bool Start(int _num);
};

