// Tethr.AudioUpload.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

Poco::SingletonHolder<tethr::Session> _session;


std::vector<tethr::RecordingSettingSummary> GetRecordingSummaries()
{
	tethr::RecordingSettings tethrConnection(_session.get());

	return tethrConnection.GetRecordingSessionSummaries();

}

tethr::SessionStatus GetRecordingStatus(std::string sessionId)
{
	tethr::ArchivedRecording tethrConnection(_session.get());

	return tethrConnection.GetRecordingStatus(sessionId);
}

std::vector<tethr::SessionStatus> GetRecordingStatus(std::vector<std::string> sessionIds)
{
	tethr::ArchivedRecording tethrConnection(_session.get());

	return tethrConnection.GetRecordingStatus(sessionIds);
}

tethr::SendFileResult SendFile(std::string jsonFileName)
{
	// Create an Instance of the Archive Recording provider used to send archived audio to Tethr
	tethr::ArchivedRecording tethrConnection(_session.get());

	// NOTE: We have to do some additional parsing of the JSON to make the sample unique and reusable in this example.

	// NOTE: Session ID should be something that can be used by other systems to connect a call in Tethr
	// to the same call in other systems.  The sessionID must be unique per recording.
	//
	// For this example, we are appending the time so we can test with the same file multiple times.
	// but should be a more meaningful value in production.
	Poco::Util::JSONConfiguration json(jsonFileName);
	std::stringstream jsonStream;

	Poco::Timestamp now;
	std::stringstream sessionId;
	sessionId << now.utcTime();

	json.setString("sessionId", sessionId.str());
	json.save(jsonStream);  //Save the json to a stringstream

	Poco::JSON::Parser parser;
	Poco::Dynamic::Var jsonParseResult = parser.parse(jsonStream.str());

	//Create JSON object Pointer
	Poco::JSON::Object::Ptr jsonObject = jsonParseResult.extract<Poco::JSON::Object::Ptr>();

	Poco::FileOutputStream sampleRecording("SampleRecording.json");
	jsonObject->stringify(sampleRecording);
	sampleRecording.close();

	Poco::Path audioFilePath(jsonFileName);
	audioFilePath.setExtension("wav");
	
	tethr::ArchiveCallResponse result = tethrConnection.SendRecording(jsonFileName, audioFilePath.getFileName(), "audio/wav");

	tethr::SendFileResult fileResult;
	fileResult.CallId = result.CallId;
	fileResult.SessionId = sessionId.str();

	return fileResult;
}

int main()
{
	//Create the Session
	// The Session object should be a singleton, and reused on subsequent sends so that
	// the oauth bearer token can be reused and refreshed only when it expires
	tethr::Configuration config;
	tethr::ConnectionString cs = config.LoadConfiguration("Configuration.Properties");  //You can also initialize these directly if you wish

	Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> m_pCertificateHandler = new Poco::Net::AcceptCertificateHandler(true);
	Poco::Net::Context::Ptr m_pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
	Poco::Net::SSLManager::instance().initializeClient(nullptr, m_pCertificateHandler, m_pContext);

	//The session object required that you set HostUri, ApiUser, and Password.  These can be loaded from a properties file or
	//you can also initialize these directly.
	_session.get()->HostUri = cs.HostUri;
	_session.get()->ApiUser = cs.ApiUser;
	_session.get()->ApiPassword = cs.Password;

	//API Examples - Uncomment to execute
	//1. Get RecordingSummaries Example
	//std::vector<tethr::RecordingSettingSummary> recordingSettingSummaries = GetRecordingSummaries();

	//2. Get Recording Status Example 
	//std::string sessionId = "95d8dd76-f975-482e-9cd3-29b3ffeff09a";
	//tethr::SessionStatus sessionStatus = GetRecordingStatus(sessionId);

	//3. Get Recording Statuses Example
	//std::vector<std::string> sessionIds;
	//sessionIds.push_back("95d8dd76-f975-482e-9cd3-29b3ffeff09a");
	//sessionIds.push_back("8c7287d9-9098-44c5-9662-4650c12d02cb");
	//std::vector<tethr::SessionStatus> sessionStatuses = GetRecordingStatus(sessionIds);

	//Send Recording example
	std::string fileName = "SampleRecording.json";
	tethr::SendFileResult result = SendFile(fileName);
	
	std::string localTime(Poco::DateTimeFormatter::format(result.StartTime, "%e %b %Y %H:%M"));

	std::cout << "Sent recording:" << std::endl;
	std::cout << "Session Id      : " << result.SessionId << std::endl;
	std::cout << "Call Start Time : " << localTime << std::endl;
	std::cout << "Tethr Call Id   : " << result.CallId << std::endl;

	std::cout << "Press Enter to Exit";
	std::cin.ignore();
	
    return 0;
}



