// Tethr.AudioUpload.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{
	
	//Create Session
	tethr::Session session("Configuration.Properties");

	std::string authToken = session.GetApiAuthToken(false);

    return 0;
}

