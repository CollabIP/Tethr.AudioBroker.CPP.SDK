#pragma once
#include "AudioBroker.h"
#include "ArchiveCallResponse.h"
#include "SessionStatus.h"
#include "RecordingInfo.h"

namespace tethr {
	class AUDIOBROKER_API ArchivedRecording
	{
	public:
		explicit ArchivedRecording(const Session &session);
		~ArchivedRecording();

		ArchiveCallResponse SendRecording(RecordingInfo info, std::string fileName, std::string mediaType);
		SessionStatus GetRecordingStatus(std::string sessionId);
		std::vector<CallStatus> GetRecordingStatuses(std::vector<std::string> sessionIds);

	private:
		const Session &_session;
	};
}

