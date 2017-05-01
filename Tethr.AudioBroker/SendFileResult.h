#pragma once
#include <string>
#include <Poco/DateTime.h>

struct SendFileResult
{
	std::string callId;
	std::string sessionId;
	Poco::DateTime startTime;
};
