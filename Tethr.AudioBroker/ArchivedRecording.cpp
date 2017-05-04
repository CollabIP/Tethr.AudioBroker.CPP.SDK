#include "ArchivedRecording.h"


namespace tethr
{
	ArchivedRecording::ArchivedRecording()
	{
	}

	ArchivedRecording::~ArchivedRecording()
	{
	}

	ArchiveCallResponse ArchivedRecording::SendRecording(tethr::Session * session, tethr::RecordingInfo info, std::string fileName, std::string mediaType)
	{
		return ArchiveCallResponse();
	}

	SessionStatus ArchivedRecording::GetRecordingStatus(tethr::Session * session, std::string sessionId)
	{
		return SessionStatus();
	}

	std::vector<tethr::CallStatus> ArchivedRecording::GetRecordingStatuses(tethr::Session *session, std::vector<std::string> sessionIds)
	{
		return std::vector<CallStatus>();
	}
}
