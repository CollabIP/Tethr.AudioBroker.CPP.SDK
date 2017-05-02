#pragma once
#include "AudioBroker.h"
#include "ArchiveCallResponse.h"

namespace tethr {
	class AUDIOBROKER_API ArchivedRecording
	{
	public:
		ArchivedRecording(Session tethrSession);
		~ArchivedRecording();

		ArchiveCallResponse SendRecording(RecordingInfo info, Stream waveStream, std::string mediaType);
		SessionStatus GetRecordingStatus(std::string sessionId);
		SessionStatuses GetRecordingStatus(IEnumerable<std::string> sessionIds);
	};
}

