#pragma once
#include <Poco/Util/PropertyFileConfiguration.h>
#include "ConnectionString.h"

namespace tethr{
	class Configuration
	{
	public:
		Configuration();
		~Configuration();

		static ConnectionString LoadConfiguration(std::string configurationFile);
	};

}

