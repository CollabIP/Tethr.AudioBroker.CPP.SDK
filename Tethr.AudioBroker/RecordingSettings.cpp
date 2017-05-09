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
		try
		{
			std::string json = _session->Get("/sources/v1/recordingSettings");
			//Todo This json is always blank?  
		}
		catch (Poco::ApplicationException e)
		{
			e.rethrow();
		}

		return std::vector<RecordingSettingSummary>();
		
	}
}