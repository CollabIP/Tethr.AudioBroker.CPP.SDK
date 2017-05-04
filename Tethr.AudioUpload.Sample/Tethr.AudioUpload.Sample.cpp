// Tethr.AudioUpload.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


tethr::SendFileResult SendFile(tethr::Session *session, std::string fileName)
{
	// Create an Instance of the Archive Recording provider used to send archived audio to Tethr
	tethr::ArchivedRecording tethrConnection;

	tethr::RecordingInfo recordingInfo; // = jsonStream.JsonDeserialize<tethr::RecordingInfo>();
	//Todo: Deserialze json

	// NOTE: Session ID should be something that can be used by other systems to connect a call in Tethr
	// to the same call in other systems.  The sessionID must be unique per recording.
	//
	// For this example, we are appending the time so we can test with the same file multiple times.
	// but should be a more meaningful value in production.
	Poco::Timestamp now;
	std::stringstream id;
	id << recordingInfo.SessionId << now.epochMicroseconds();

	recordingInfo.SessionId = id.str();
	
	// NOTE: it is important that the audio length and the reported call length are the same.
	// If this is not true, Tethr may have harder time accurately detecting what actually happened on a call
	// By default, Tethr will quarantine calls where the reported time and audio length do not match, and wait for
	// human interaction to fix any miss configurations.  (Contact Support if you think you need this changed)
	//
	// for this example, we are Update the call times, so the calls don't always have the same time.
	// and if you change out the audio file in the sample, you will have to re-compute the end time in the sample JSON file.
	Poco::Timespan audioLength = recordingInfo.EndTime() - recordingInfo.StartTime();
	recordingInfo.StartTime() = Poco::DateTime(); //Now
	recordingInfo.EndTime() = recordingInfo.StartTime() + audioLength;

	Poco::Path audioFilePath(fileName);
	audioFilePath.setExtension("wav");
	
	tethr::ArchiveCallResponse result = tethrConnection.SendRecording(session, recordingInfo, audioFilePath.getFileName(), "audio/wav");
	
	return tethr::SendFileResult();
}

int main()
{
	
	//Create the Session
	// The Session object should be a singleton, and reused on subsequent sends so that
	// the oauth bearer token can be reused and refreshed only when it expires
	tethr::Configuration config;
	tethr::ConnectionString cs = config.LoadConfiguration("Configuration.Properties");  //You can also initialize these directly if you wish

	Poco::SingletonHolder<tethr::Session> _session;

	//The session object required that you set HostUri, ApiUser, and Password.  These can be loaded from a properties file or
	//you can also initialize these directly.
	_session.get()->HostUri = cs.HostUri;
	_session.get()->ApiUser = cs.ApiUser;
	_session.get()->ApiPassword = cs.Password;

	//Send the file
	std::string fileName = "SampleRecording.json";
	tethr::SendFileResult result = SendFile(_session.get(), fileName);
	
	std::string localTime(Poco::DateTimeFormatter::format(result.StartTime, "%e %b %Y %H:%M"));

	std::cout << "Sent recording:" << std::endl;
	std::cout << "Session Id      : " << result.SessionId << std::endl;
	std::cout << "Call Start Time : " << localTime << std::endl;
	std::cout << "Tethr Call Id   : " << result.CallId << std::endl;

	std::cout << "Press Enter to Exit";
	std::cin.ignore();
	
    return 0;
}



