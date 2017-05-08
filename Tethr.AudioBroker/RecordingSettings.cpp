#include "RecordingSettings.h"

namespace tethr
{
	RecordingSettings::RecordingSettings(Session *session)
	{
		_session = session;
	}

	RecordingSettings::~RecordingSettings()
	{
	}

	std::vector<RecordingSettingSummary> RecordingSettings::GetRecordingSessionSummaries()
	{
		std::string json = _session->Get("/sources/v1/recordingSettings");
		
		return std::vector<RecordingSettingSummary>();
	}
}