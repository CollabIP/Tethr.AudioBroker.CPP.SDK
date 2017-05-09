#pragma once

#include "CallStatus.h"
#include "SessionStatus.h"
#include "ArchiveCallResponse.h"
#include "Session.h"

namespace tethr
{
	class AUDIOBROKER_API ArchivedRecording
	{
	public:
		ArchivedRecording(Session * session);
		~ArchivedRecording();

		Session * _session;

		ArchiveCallResponse SendRecording(std::string jsonFileName, std::string audioFileName, std::string mediaType);
		SessionStatus GetRecordingStatus(std::string sessionId);
		std::vector<SessionStatus> GetRecordingStatus(std::vector<std::string> sessionIds);

	private:
		Poco::JSON::Object::Ptr SetAudioFormat(std::string jsonFileName, std::string mediaType);
	};
}
