// Tethr.AudioUpload.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{
	
	
	Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> pConf;
	pConf = new Poco::Util::PropertyFileConfiguration("Configuration.Properties");
	
	std::string uri = pConf->getString("Uri");
	std::string apiUser = pConf->getString("ApiUser");
	std::string password = pConf->getString("Password");

    return 0;
}

