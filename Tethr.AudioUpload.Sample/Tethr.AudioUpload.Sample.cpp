// Tethr.AudioUpload.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

Poco::SingletonHolder<tethr::Session> _session;

tethr::RecordingInfo ParseJson(std::string fileName)
{
	//We need to load the data into a recordingInfo object
	//Todo: Review.  Why dont we just set the audio format in the metadata that is passed to tethr? 
	//Todo: It would save a lot of parsing.

	//Load Json from the file using JsonConfiguration
	Poco::Util::JSONConfiguration json(fileName);
	std::stringstream jsonStream;
	json.save(jsonStream);  //Save the json to a stringstream

	//Setup JSON Parsing
	Poco::JSON::Parser parser;
	Poco::Dynamic::Var jsonParseResult = parser.parse(jsonStream.str());

	//Create JSON object Pointer
	Poco::JSON::Object::Ptr jsonObject = jsonParseResult.extract<Poco::JSON::Object::Ptr>();

	//Create RecordingInfo
	tethr::RecordingInfo recordingInfo;
	recordingInfo.SessionId = jsonObject->get("sessionId").toString();
	recordingInfo.MasterCallId = jsonObject->get("masterCallId").toString();
	recordingInfo.NumberDialed = jsonObject->get("numberDialed").toString();

	std::string callDirection = jsonObject->get("direction").toString();
	if (_stricmp(callDirection.c_str(), "Outbound") == 0)
	{
		recordingInfo.Direction = tethr::Outbound;
	}
	else if (_stricmp(callDirection.c_str(), "Inbound") == 0)
	{
		recordingInfo.Direction = tethr::Inbound;
	}
	else if (_stricmp(callDirection.c_str(), "Internal") == 0)
	{
		recordingInfo.Direction = tethr::Internal;
	}
	else
	{
		recordingInfo.Direction = tethr::Unknown;
	}

	//Get Metadata Object
	Poco::Dynamic::Var metadata = jsonObject->get("metadata");
	Poco::JSON::Object::Ptr metadataObject = metadata.extract<Poco::JSON::Object::Ptr>();

	/*for (Poco::JSON::Object::ConstIterator it = metadataObject->begin(), end = metadataObject->end(); it != end; ++it)
	{
		std::string key = it->first;
		std::string value = it->second;
		recordingInfo.Metadata.Data.insert(key, value);
	}*/


	std::string mdata = metadataObject->get("CallIndex").toString();

	return recordingInfo;
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
	/*Poco::Timestamp now;
	std::stringstream id;
	id << recordingInfo.SessionId << now.epochMicroseconds();*/

	//recordingInfo.SessionId = id.str();
	
	// NOTE: it is important that the audio length and the reported call length are the same.
	// If this is not true, Tethr may have harder time accurately detecting what actually happened on a call
	// By default, Tethr will quarantine calls where the reported time and audio length do not match, and wait for
	// human interaction to fix any miss configurations.  (Contact Support if you think you need this changed)
	//
	// for this example, we are Update the call times, so the calls don't always have the same time.
	// and if you change out the audio file in the sample, you will have to re-compute the end time in the sample JSON file.
	
	//"startTime" : "2016-08-30T18:43:34.5565965Z",
	//"endTime" : "2016-08-30T18:44:32.62859Z"

	/*Poco::Timespan audioLength = recordingInfo.EndTime() - recordingInfo.StartTime();
	recordingInfo.StartTime() = Poco::DateTime(); //Now
	recordingInfo.EndTime() = recordingInfo.StartTime() + audioLength;*/

	Poco::Path audioFilePath(jsonFileName);
	audioFilePath.setExtension("wav");
	
	tethr::ArchiveCallResponse result = tethrConnection.SendRecording(recordingInfo, audioFilePath.getFileName(), "audio/wav");
	
	return tethr::SendFileResult();
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

	//Get Recording Status Example 
	std::string sessionId = "95d8dd76-f975-482e-9cd3-29b3ffeff09a";
	tethr::SessionStatus sessionStatus = GetRecordingStatus(sessionId);

	//Get Recording Statuses Example
	std::vector<std::string> sessionIds;
	sessionIds.push_back("95d8dd76-f975-482e-9cd3-29b3ffeff09a");
	sessionIds.push_back("8c7287d9-9098-44c5-9662-4650c12d02cb");

	//Todo: shouldn't we be able to lookup by call id as well?
	std::vector<tethr::SessionStatus> sessionStatuses = GetRecordingStatus(sessionIds);

	//Send Recording example
	std::string fileName = "SampleRecording.json";
	tethr::SendFileResult result = SendFile(fileName);
	
	std::string localTime(Poco::DateTimeFormatter::format(result.StartTime, "%e %b %Y %H:%M"));

	std::cout << "Sent recording:" << std::endl;
	std::cout << "Session Id      : " << result.SessionId << std::endl;
	std::cout << "Call Start Time : " << localTime << std::endl;
	std::cout << "Tethr Call Id   : " << result.CallId << std::endl;

	std::cout << "Press Enter to Exit";
	std::cin.ignore();*/
	
    return 0;
}



