#pragma once

#include "CallStatus.h"
#include "SessionStatus.h"
#include "ArchiveCallResponse.h"
#include "Session.h"
#include "RecordingInfo.h"

namespace tethr
{

	class AUDIOBROKER_API ArchivedRecording
	{
	public:
		ArchivedRecording(Session * session);
		~ArchivedRecording();

		Session * _session;

		ArchiveCallResponse SendRecording(RecordingInfo recordingInfo, std::string audioFileName, std::string mediaType);
		SessionStatus GetRecordingStatus(std::string sessionId);
		std::vector<SessionStatus> GetRecordingStatus(std::vector<std::string> sessionIds);

	private:
		Poco::JSON::Object::Ptr ConvertRecordingInfoToJson(RecordingInfo recordingInfo, std::string mediaType);

		Poco::DateTime m_startTime;
		Poco::DateTime m_endTime;
	};
}
