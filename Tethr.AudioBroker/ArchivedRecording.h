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
		ArchivedRecording();
		~ArchivedRecording();

		ArchiveCallResponse SendRecording(tethr::Session *session, tethr::RecordingInfo info, std::string fileName, std::string mediaType);
		SessionStatus GetRecordingStatus(tethr::Session *session, std::string sessionId);
		std::vector<CallStatus> GetRecordingStatuses(tethr::Session *session, std::vector<std::string> sessionIds);
	};
}
