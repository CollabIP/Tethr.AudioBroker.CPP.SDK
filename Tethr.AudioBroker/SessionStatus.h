#pragma once
#include <string>
#include "CallStatus.h"

namespace tethr{
	struct SessionStatus
	{
		CallStatus Status;
		std::string CallId;
		std::string SessionId;
	};
}
