#pragma once
#include <string>
#include <Poco/DateTime.h>

namespace tethr {
	struct SendFileResult
	{
		std::string CallId;
		std::string SessionId;
		Poco::DateTime StartTime;
	};
}

