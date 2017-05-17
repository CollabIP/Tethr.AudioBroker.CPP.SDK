// Tethr.AudioUpload.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

tethr::Session * _session;

std::vector<tethr::RecordingSettingSummary> GetRecordingSummaries()
{
	tethr::RecordingSettings tethrConnection(_session);

	return tethrConnection.GetRecordingSessionSummaries();

}

tethr::SessionStatus GetRecordingStatus(std::string sessionId)
{
	tethr::ArchivedRecording tethrConnection(_session);

	return tethrConnection.GetRecordingStatus(sessionId);
}

std::vector<tethr::SessionStatus> GetRecordingStatus(std::vector<std::string> sessionIds)
{
	tethr::ArchivedRecording tethrConnection(_session);

	return tethrConnection.GetRecordingStatus(sessionIds);
}

tethr::SendFileResult SendFile(tethr::RecordingInfo recordingInfo, std::string audioFilePath, std::string mediaType)
{
	// Create an Instance of the Archive Recording provider used to send archived audio to Tethr
	tethr::ArchivedRecording tethrConnection(_session);

	tethr::ArchiveCallResponse result = tethrConnection.SendRecording(recordingInfo, audioFilePath, "audio/wav");

	tethr::SendFileResult fileResult;
	fileResult.CallId = result.CallId;
	fileResult.SessionId = recordingInfo.SessionId;

	return fileResult;
}


int main()
{
	try
	{
		//Create the Session
		// The Session object should be a singleton, and reused on subsequent sends so that
		// the oauth bearer token can be reused and refreshed only when it expires
		tethr::Configuration config;
		tethr::ConnectionString cs = config.LoadConfiguration("Configuration.Properties");  //You can also initialize these directly if you wish

		_session = tethr::Session::GetInstance(cs.HostUri, cs.ApiUser, cs.Password);
		
		//Send Recording example
		tethr::Contact contact1;
		contact1.Channel = 0;
		contact1.FirstName = "Johny";
		contact1.LastName = "Doe";
		contact1.PhoneNumber = "1001";
		contact1.ReferenceId = "12445";
		contact1.Type = "agent";

		tethr::Contact contact2;
		contact2.Channel = 1;
		contact2.FirstName = "Jane";
		contact2.LastName = "Doe";
		contact2.PhoneNumber = "1002";
		contact2.ReferenceId = "12446";
		contact2.Type = "customer";

		//The sessionId should be unique to the call.  You can use the sessionId to perform other api functions.
		//For this sample we are just setting it to a UTC time string.
		Poco::Timestamp nownow;
		std::stringstream sessionId;
		sessionId << nownow.utcTime();

		tethr::RecordingInfo recordingInfo;
		recordingInfo.SessionId = sessionId.str();
		recordingInfo.MasterCallId = "3bef3ef1495c49e19bfd21951afcf043";
		recordingInfo.Direction = tethr::Inbound;
		recordingInfo.NumberDialed = "1001";

		
		recordingInfo.Contacts.push_back(contact1);
		recordingInfo.Contacts.push_back(contact2);

		recordingInfo.Metadata.insert(std::make_pair("key1", "value2"));
		recordingInfo.Metadata.insert(std::make_pair("key2", "value2"));

		// NOTE: it is important that the audio length and the reported call length are the same.
		// If this is not true, Tethr may have harder time accurately detecting what actually happened on a call
		// By default, Tethr will quarantine calls where the reported time and audio length do not match, and wait for
		// human interaction to fix any miss configurations.  (Contact Support if you think you need this changed)
		//This is just a sample.  Times should be in UTC

		//Internally we convert the utc epoch time to a POCO::Timestamp and then format it with the DateTimeFormatter so that the JSON has the correct datetime format 
		//for Tethr.
		time_t now = time(0);
		tm *gmtm = gmtime(&now);
		time_t utcNow = mktime(gmtm);

		recordingInfo.StartTime = utcNow;  //This should be the UTC start time 
		recordingInfo.EndTime = utcNow; //UtC end time

		tethr::SendFileResult result = SendFile(recordingInfo, "SampleRecording.wav", "audio/wav");

		std::string localTime(Poco::DateTimeFormatter::format(result.StartTime, "%e %b %Y %H:%M"));

		std::cout << "Sent recording:" << std::endl;
		std::cout << "Session Id      : " << result.SessionId << std::endl;
		std::cout << "Call Start Time : " << localTime << std::endl;
		std::cout << "Tethr Call Id   : " << result.CallId << std::endl;

		std::cout << "Press Enter to Exit";
		std::cin.ignore();

		return 0;
	}
	catch (Poco::ApplicationException e)
	{
		e.what();
	}

	
}



