#include "stdafx.h"
#include "RPGServer.h"

int main()
{
	_wsetlocale(LC_ALL, L"korean");      //지역화 설정을 전역적으로 적용
	wcout.imbue(locale("korean"));       //출력시 부분적 적용
	wcin.imbue(locale("korean"));        //입력시 부분적 적용

	//======================================================

	RPGServer* rpgServer = new RPGServer();
	rpgServer->Init();

	WSACleanup();

	return 0;
}