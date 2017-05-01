#include "Configuration.h"
#include "ConnectionString.h"

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
	connectionString.hostUri = pConf->getString("Uri");
	connectionString.apiUser = pConf->getString("ApiUser");
	connectionString.password = pConf->getString("Password");

	return connectionString;
}
