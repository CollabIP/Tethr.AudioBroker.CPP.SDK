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
	ArchiveCallResponse ArchivedRecording::SendRecording(RecordingInfo recordingInfo, std::string audioFileName, std::string mediaType)
	{
		try
		{
			Poco::JSON::Object::Ptr jsonObject = ConvertRecordingInfoToJson(recordingInfo, mediaType);
			
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

	Poco::JSON::Object::Ptr ArchivedRecording::ConvertRecordingInfoToJson(RecordingInfo recordingInfo, std::string mediaType)
	{
		// Setting the Audio Format to make sure it matches the media type, RecordingInfo.Audio will be obsoleted at some point in favor of only looking at the media type.
		Poco::JSON::Object::Ptr base = new Poco::JSON::Object(true);
		
		//Set SessionId
		if (recordingInfo.SessionId.empty())
		{
			throw new std::exception("The sessionId cannot be empty and should be unique.");
		}
		
		base->set("sessionId", recordingInfo.SessionId);
		
		//Set the MasterCallId
		base->set("masterCallId", recordingInfo.MasterCallId);

		//Set the number dialed
		base->set("numberDialed", recordingInfo.NumberDialed);
		

		//Check local and convert to utc
		Poco::Timestamp ts_start(Poco::Timestamp::fromEpochTime(recordingInfo.StartTime));
		Poco::Timestamp ts_end(Poco::Timestamp::fromEpochTime(recordingInfo.EndTime));

		Poco::DateTime dt_startTime(ts_start);
		Poco::DateTime dt_endTime(ts_end);

		std::string utcStartTime(Poco::DateTimeFormatter::format(dt_startTime, "%Y-%m-%dT%H:%M:%sZ"));
		std::string utcEndTime(Poco::DateTimeFormatter::format(dt_startTime, "%Y-%m-%dT%H:%M:%sZ"));

		base->set("startTime", utcStartTime);
		base->set("endTime", utcEndTime);
		
		//Set the call direction
		switch(recordingInfo.Direction)
		{
		case Inbound:
			base->set("direction", "Inbound");
			break;
		case Outbound:
			base->set("direction", "Outbound");
			break;
		case Internal:
			base->set("direction", "Internal");
			break;
		case Unknown:
			base->set("direction", "Unknown");
			break;
		default:
			base->set("direction", "Unknown");
			break;
		}
		
		//Set Contacts Array
		Poco::JSON::Array::Ptr contactsArray = new Poco::JSON::Array();
		std::vector<tethr::Contact>::iterator contacts_iter;
		
		int contactsCounter = 0;
		for (contacts_iter= recordingInfo.Contacts.begin(); contacts_iter != recordingInfo.Contacts.end(); ++contacts_iter)
		{
		
			Poco::JSON::Object::Ptr contact = new Poco::JSON::Object(true);

			contact->set("channel", contacts_iter->Channel);
			contact->set("firstName", contacts_iter->FirstName);
			contact->set("lastName", contacts_iter->LastName);
			contact->set("phoneNumber", contacts_iter->PhoneNumber);
			contact->set("referenceId", contacts_iter->ReferenceId);
			contact->set("type", contacts_iter->Type);
			
			contactsArray->set(contactsCounter, contact);

			contactsCounter++;
		}

		//Add Contacts Array to the base object
		base->set("contacts", contactsArray);

		//Set Metadata Object
		Poco::JSON::Object::Ptr metadataObject= new Poco::JSON::Object(true);

		std::map<std::string, std::string>::iterator metadata_iter;
		
		for (metadata_iter = recordingInfo.Metadata.begin(); metadata_iter != recordingInfo.Metadata.end(); ++metadata_iter)
		{
			metadataObject->set(metadata_iter->first, metadata_iter->second);
		}

		base->set("metadata", metadataObject);
		
		//Set Audio Type (Should be deprecated)
		Poco::JSON::Object::Ptr audioFormatObj = new Poco::JSON::Object();
		if (_stricmp(mediaType.c_str(), "audio/x-wav") == 0 ||
			_stricmp(mediaType.c_str(), "audio/wave") == 0 ||
			_stricmp(mediaType.c_str(), "audio/vnd.wav") == 0 ||
			_stricmp(mediaType.c_str(), "audio/x-wave") == 0 ||
			_stricmp(mediaType.c_str(), "audio/wav") == 0)
		{
			// Setting the audio value as some instance of Tethr may still be looking for this.
			// Will be removed from SDK, once it is fully removed from Tethr servers.
			audioFormatObj->set("Format", "wav");
			base->set("Audio", audioFormatObj);

			// Set the media type to the one used by default in Tethr.
			mediaType = "audio/wav";
		}
		else if (_stricmp(mediaType.c_str(), "audio/mp3") == 0)
		{
			audioFormatObj->set("Format", "mp3");
			base->set("Audio", audioFormatObj);
		}
		else if (_stricmp(mediaType.c_str(), "audio/ogg") == 0)
		{
			audioFormatObj->set("Format", "opus");
			base->set("Audio", audioFormatObj);
		}
		else
		{
			//Check the file type is wav, If they are not attaching a file, we only support Wav files.
			throw std::exception("Only Wav, MP3, or OPUS files are supported files.");
		}

		/*std::stringstream infoStream;
		base->stringify(infoStream);

		std::string info = infoStream.str();*/
		return base;

	}
}
