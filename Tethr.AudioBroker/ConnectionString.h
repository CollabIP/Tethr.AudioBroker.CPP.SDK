#pragma once
#include <string>

namespace tethr {
	struct ConnectionString
	{
		std::string HostUri;
		std::string ApiUser;
		std::string Password;
	};
}

