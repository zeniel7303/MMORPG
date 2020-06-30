#pragma once

#pragma pack(push, 1)

struct UserInfo
{
	int userID;
	char userName[20];

	void Reset()
	{
		userID = 0;
		userName[0] = { 0, };
	}
};

#pragma pack(pop)