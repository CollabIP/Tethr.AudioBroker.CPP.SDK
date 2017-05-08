#pragma once
#include <string>
#include "AudioBroker.h"
#include "Session.h"
#include "RecordingSettingSummary.h"

namespace tethr
{
	class AUDIOBROKER_API RecordingSettings
	{
	public:
		RecordingSettings(Session *session);
		~RecordingSettings();

		std::vector<RecordingSettingSummary> GetRecordingSessionSummaries();		
	private:
		Session * _session;
	};
}

