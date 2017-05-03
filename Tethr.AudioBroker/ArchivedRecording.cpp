#include "ArchivedRecording.h"

namespace tethr {
	
	ArchivedRecording::ArchivedRecording(const Session& session): _session(session)
	{
	}

	ArchivedRecording::~ArchivedRecording()
	{
	}

	ArchiveCallResponse ArchivedRecording::SendRecording(RecordingInfo info, std::string fileName, std::string mediaType)
	{
		// Setting the Audio Format to make sure it matches the media type, RecordingInfo.Audio will be obsoleted at some point in favor of only looking at the media type.
		if (_stricmp(mediaType.c_str(), "audio/x-wav") == 0 ||
			_stricmp(mediaType.c_str(), "audio/wave") == 0 ||
			_stricmp(mediaType.c_str(), "audio/vnd.wav") == 0 ||
			_stricmp(mediaType.c_str(), "audio/x-wave") == 0 ||
			_stricmp(mediaType.c_str(), "audio/wav") == 0)
		{
			// Setting the audio value as some instance of Tethr may still be looking for this.
			// Will be removed from SDK, once it is fully removed from Tethr servers.
			info.Audio.Format = "wav";

			// Set the media type to the one used by default in Tethr.
			mediaType = "audio/wav";
		}
		else if (_stricmp(mediaType.c_str(), "audio/mp3") == 0)
		{
			info.Audio.Format = "mp3";
		}
		else if (_stricmp(mediaType.c_str(), "audio/ogg") == 0)
		{
			info.Audio.Format = "opus";
		}
		else
		{
			//Check the file type is wav, If they are not attaching a file, we only support Wav files.
			throw Poco::ApplicationException("Only Wav, MP3, or OPUS files are supported files.");
		}

		Poco::JSON::Object recordingInfo;
		
		//recordingInfo->set(info., "file");
		//menuObj->set("value", "File");
		//Poco::DynamicStruct result = _session.PostMutliPartFormData("/callCapture/v1/archive", recordingInfo, fileName, mediaType);

		
		return ArchiveCallResponse();
	}

	SessionStatus ArchivedRecording::GetRecordingStatus(std::string sessionId)
	{
		return SessionStatus();
	}

	std::vector<CallStatus> ArchivedRecording::GetRecordingStatuses(std::vector<std::string> sessionIds)
	{
		return std::vector<CallStatus>();
	}
}
