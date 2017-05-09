#include "ArchivedRecording.h"

//Todo:: Fix exception handling.  Right now just rethrowing to the caller.  Maybe throw std::exception instead and change return path structure.
namespace tethr
{
	ArchivedRecording::ArchivedRecording(Session * session)
	{
		_session = session;
	}

	ArchivedRecording::~ArchivedRecording()
	{
	}
	
	/// <summary>
	/// Sends the recording.
	/// </summary>
	/// <param name="jsonFileName">Name of the json file.</param>
	/// <param name="audioFileName">Name of the audio file.</param>
	/// <param name="mediaType">Type of the media.</param>
	/// <returns>ArchiveC</returns>
	/// <remarks>Throws Poco::ApplicationException</remarks>
	ArchiveCallResponse ArchivedRecording::SendRecording(std::string jsonFileName, std::string audioFileName, std::string mediaType)
	{
		try
		{
			// Setting the Audio Format to make sure it matches the media type, RecordingInfo.Audio will be obsoleted at some point in favor of only looking at the media type.
			Poco::JSON::Object::Ptr jsonObject = SetAudioFormat(jsonFileName, mediaType);

			std::string result = _session->PostMutliPartFormData("/callCapture/v1/archive", jsonObject, audioFileName, mediaType);

			//Parse JSON
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var jsonParseResult = parser.parse(result);

			//Create JSON object Pointer
			Poco::JSON::Object::Ptr archiveResult = jsonParseResult.extract<Poco::JSON::Object::Ptr>();

			//Get callSessions Object
			Poco::Dynamic::Var callId = archiveResult->get("callId");

			ArchiveCallResponse callResponse;
			callResponse.CallId = callId.convert<std::string>();

			return callResponse;
		}
		catch(Poco::ApplicationException e)
		{
			e.rethrow();
		}
		
		throw Poco::ApplicationException("An unexpected error occured while sending the recording.");
	}
	
	/// <summary>
	/// Gets the recording status.
	/// </summary>
	/// <param name="sessionId">The session identifier.</param>
	/// <returns>Session Status</returns>
	/// <remarks>Throws Poco::ApplicationException</remarks>
	SessionStatus ArchivedRecording::GetRecordingStatus(std::string sessionId)
	{
		try
		{
			//Create json array structure like so from vector
			Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

			//Create the SessionId Array
			Poco::JSON::Array::Ptr sessionIdArray = new Poco::JSON::Array();

			sessionIdArray->add(sessionId);

			//Set the name for the array
			root->set("CallSessionIds", sessionIdArray);

			//Poco::DynamicStruct statuses = _session->Post("/callCapture/v1/status", root);
			std::string json = _session->Post("/callCapture/v1/status", root);

			//Parse JSON
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var jsonParseResult = parser.parse(json);

			//Create JSON object Pointer
			Poco::JSON::Object::Ptr jsonObject = jsonParseResult.extract<Poco::JSON::Object::Ptr>();

			//Get callSessions Object
			Poco::Dynamic::Var callSessions = jsonObject->get("callSessions");
			Poco::JSON::Array::Ptr callSessionsObject = callSessions.extract<Poco::JSON::Array::Ptr>();

			std::vector<SessionStatus> callSessionStatuses;
			for (int i = 0; i < callSessionsObject->size(); ++i)
			{
				SessionStatus sessionStatus;
				Poco::JSON::Object::Ptr callSession = callSessionsObject->getObject(i);
				for (Poco::JSON::Object::ConstIterator it = callSession->begin(), end = callSession->end(); it != end; ++it)
				{
					if (_stricmp(it->first.c_str(), "callId") == 0)
					{
						sessionStatus.CallId = it->second.convert<std::string>();
					}
					else if (_stricmp(it->first.c_str(), "sessionId") == 0)
					{
						sessionStatus.SessionId = it->second.convert<std::string>();;
					}
					else if (_stricmp(it->first.c_str(), "status") == 0)
					{
						if (_stricmp(it->second.convert<std::string>().c_str(), "InProgress") == 0)
						{
							sessionStatus.Status = InProgress;
						}
						else if (_stricmp(it->second.convert<std::string>().c_str(), "Complete") == 0)
						{
							sessionStatus.Status = Complete;
						}
						else if (_stricmp(it->second.convert<std::string>().c_str(), "Concluded") == 0)
						{
							sessionStatus.Status = Concluded;
						}
						else if (_stricmp(it->second.convert<std::string>().c_str(), "NotFound") == 0)
						{
							sessionStatus.Status = NotFound;
						}
						else if (_stricmp(it->second.convert<std::string>().c_str(), "Error") == 0)
						{
							sessionStatus.Status = Error;
						}
					}
				}

				callSessionStatuses.push_back(sessionStatus);
			}

			return callSessionStatuses.front();
		}
		catch (Poco::ApplicationException e)
		{
			e.rethrow();
		}

		throw Poco::ApplicationException("An unexpected error occured while getting the recording status.");
	}
	
	/// <summary>
	/// Gets the recording status.
	/// </summary>
	/// <param name="sessionIds">The session ids.</param>
	/// <returns>vector of SessionStatus</returns>
	/// <remarks>Throws Poco::ApplicationException</remarks>
	std::vector<SessionStatus> ArchivedRecording::GetRecordingStatus(std::vector<std::string> sessionIds)
	{
		try
		{
			//Create json array structure like so from vector
			Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

			//Create the SessionId Array
			Poco::JSON::Array::Ptr sessionIdArray = new Poco::JSON::Array();

			for (std::vector<std::string>::iterator it = sessionIds.begin(); it != sessionIds.end(); ++it)
			{
				sessionIdArray->add(*it);
			}

			//Set the name for the array
			root->set("CallSessionIds", sessionIdArray);

			//Poco::DynamicStruct statuses = _session->Post("/callCapture/v1/status", root);
			std::string json = _session->Post("/callCapture/v1/status", root);

			//Parse JSON
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var jsonParseResult = parser.parse(json);

			//Create JSON object Pointer
			Poco::JSON::Object::Ptr jsonObject = jsonParseResult.extract<Poco::JSON::Object::Ptr>();

			//Get callSessions Object
			Poco::Dynamic::Var callSessions = jsonObject->get("callSessions");
			Poco::JSON::Array::Ptr callSessionsObject = callSessions.extract<Poco::JSON::Array::Ptr>();

			std::vector<SessionStatus> callSessionStatuses;
			for (int i = 0; i < callSessionsObject->size(); ++i)
			{
				SessionStatus sessionStatus;
				Poco::JSON::Object::Ptr callSession = callSessionsObject->getObject(i);
				for (Poco::JSON::Object::ConstIterator it = callSession->begin(), end = callSession->end(); it != end; ++it)
				{
					if (_stricmp(it->first.c_str(), "callId") == 0)
					{
						sessionStatus.CallId = it->second.convert<std::string>();
					}
					else if (_stricmp(it->first.c_str(), "sessionId") == 0)
					{
						sessionStatus.SessionId = it->second.convert<std::string>();;
					}
					else if (_stricmp(it->first.c_str(), "status") == 0)
					{
						if (_stricmp(it->second.convert<std::string>().c_str(), "InProgress") == 0)
						{
							sessionStatus.Status = InProgress;
						}
						else if (_stricmp(it->second.convert<std::string>().c_str(), "Complete") == 0)
						{
							sessionStatus.Status = Complete;
						}
						else if (_stricmp(it->second.convert<std::string>().c_str(), "Concluded") == 0)
						{
							sessionStatus.Status = Concluded;
						}
						else if (_stricmp(it->second.convert<std::string>().c_str(), "NotFound") == 0)
						{
							sessionStatus.Status = NotFound;
						}
						else if (_stricmp(it->second.convert<std::string>().c_str(), "Error") == 0)
						{
							sessionStatus.Status = Error;
						}
					}
				}

				callSessionStatuses.push_back(sessionStatus);
			}

			return callSessionStatuses;
		}
		catch (Poco::ApplicationException e)
		{
			e.rethrow();
		}

		throw Poco::ApplicationException("An unexpected error occured while getting the recording status.");
	}
	
	/// <summary>
	/// Sets the audio format. (Helper)
	/// </summary>
	/// <param name="jsonFileName">Name of the json file.</param>
	/// <param name="mediaType">Type of the media.</param>
	/// <returns></returns>
	Poco::JSON::Object::Ptr ArchivedRecording::SetAudioFormat(std::string jsonFileName, std::string mediaType)
	{
		Poco::Util::JSONConfiguration json(jsonFileName);
		std::stringstream jsonStream;
		json.save(jsonStream);  //Save the json to a stringstream

		//Setup JSON Parsing
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var jsonParseResult = parser.parse(jsonStream.str());

		Poco::JSON::Object::Ptr audioFormatObj = new Poco::JSON::Object();
		
		//Create JSON object Pointer
		Poco::JSON::Object::Ptr jsonObject = jsonParseResult.extract<Poco::JSON::Object::Ptr>();
		
		if (_stricmp(mediaType.c_str(), "audio/x-wav") == 0 ||
			_stricmp(mediaType.c_str(), "audio/wave") == 0 ||
			_stricmp(mediaType.c_str(), "audio/vnd.wav") == 0 ||
			_stricmp(mediaType.c_str(), "audio/x-wave") == 0 ||
			_stricmp(mediaType.c_str(), "audio/wav") == 0)
		{
			// Setting the audio value as some instance of Tethr may still be looking for this.
			// Will be removed from SDK, once it is fully removed from Tethr servers.
			audioFormatObj->set("Format", "wav");
			jsonObject->set("Audio", audioFormatObj);

			// Set the media type to the one used by default in Tethr.
			mediaType = "audio/wav";
		}
		else if (_stricmp(mediaType.c_str(), "audio/mp3") == 0)
		{
			audioFormatObj->set("Format", "mp3");
			jsonObject->set("Audio", audioFormatObj);
		}
		else if (_stricmp(mediaType.c_str(), "audio/ogg") == 0)
		{
			audioFormatObj->set("Format", "opus");
			jsonObject->set("Audio", audioFormatObj);
		}
		else
		{
			//Check the file type is wav, If they are not attaching a file, we only support Wav files.
			throw Poco::ApplicationException("Only Wav, MP3, or OPUS files are supported files.");
		}

		return jsonObject;
	}
}
