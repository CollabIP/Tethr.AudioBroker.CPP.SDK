// Tethr.AudioUpload.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

SendFileResult SendFile(std::string fileName)
{

	return SendFileResult();
}

int main()
{
	
	//Create the Session
	// The Session object should be a singleton, and reused on subsequent sends so that
	// the oauth bearer token can be reused and refreshed only when it expires
	tethr::Session session("Configuration.Properties");

	//Send the file
	std::string fileName = "SampleRecording.json";
	SendFileResult result = SendFile(fileName);

	//Todo: Convert to LocalTime
	std::string localTime(Poco::DateTimeFormatter::format(result.startTime, "%e %b %Y %H:%M"));

	std::cout << "Sent recording:" << std::endl;
	std::cout << "Session Id      : " << result.sessionId << std::endl;
	std::cout << "Call Start Time : " << localTime << std::endl;
	std::cout << "Tethr Call Id   : " << result.callId << std::endl;

	std::cout << "Press Enter to Exit";
	std::cin.ignore();
	
    return 0;
}



