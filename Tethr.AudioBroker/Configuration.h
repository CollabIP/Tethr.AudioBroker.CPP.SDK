#pragma once
#include <Poco/Util/PropertyFileConfiguration.h>
#include "ConnectionString.h"

class Configuration
{
public:
	Configuration();
	~Configuration();

	static ConnectionString LoadConfiguration(std::string configurationFile);
};

