#pragma once
#include <Poco/Util/PropertyFileConfiguration.h>
#include "ConnectionString.h"
#include "AudioBroker.h"

namespace tethr {
	class AUDIOBROKER_API Configuration
	{
	public:
		Configuration();
		~Configuration();

		static ConnectionString LoadConfiguration(std::string configurationFile);
	};
}

