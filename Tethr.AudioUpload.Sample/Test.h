#pragma once
#include <Poco/SingletonHolder.h>
#include "Session.h"

class Test
{
public:
	//Test(Poco::SingletonHolder<tethr::Session> &sh);
	Test(tethr::Session *session);
	~Test();

	tethr::Session *_session;
	std::string GetApiUser();
};

