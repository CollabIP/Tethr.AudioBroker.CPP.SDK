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
		Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> pConf;
		pConf = new Poco::Util::PropertyFileConfiguration(configurationFile);

		ConnectionString connectionString;
		connectionString.HostUri = pConf->getString("Uri");
		connectionString.ApiUser = pConf->getString("ApiUser");
		connectionString.Password = pConf->getString("Password");

		return connectionString;
	}
}

