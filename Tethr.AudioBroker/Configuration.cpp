#include "Configuration.h"
#include "ConnectionString.h"

namespace tethr {
	Configuration::Configuration()
	{
	}


	Configuration::~Configuration()
	{
	}

	ConnectionString Configuration::LoadConfiguration(std::string configurationFile)
	{
		Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> pConfiguration = new Poco::Util::PropertyFileConfiguration(configurationFile);

		ConnectionString connectionString;
		connectionString.HostUri = pConfiguration->getString("Uri");
		connectionString.ApiUser = pConfiguration->getString("ApiUser");
		connectionString.Password = pConfiguration->getString("Password");

		return connectionString;
	}
}

