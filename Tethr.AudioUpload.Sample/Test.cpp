#include "Test.h"



Test::Test(tethr::Session *session)
{
	_session = session;
}

Test::~Test()
{
}

std::string Test::GetApiUser()
{
	std::string u = _session->ApiUser;
	return std::string();
}
