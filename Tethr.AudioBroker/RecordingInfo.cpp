#include "RecordingInfo.h"

namespace tethr {
	RecordingInfo::RecordingInfo()
	{
	}


	RecordingInfo::~RecordingInfo()
	{
	}

	Poco::DateTime RecordingInfo::StartTime() const
	{
		return _startTime;
	}

	void RecordingInfo::StartTime(Poco::DateTime dt)
	{
		/*if (value.Kind == DateTimeKind.Local)
			_startTime = value.ToUniversalTime();
		else
		{
			Debug.Assert(value.Kind == DateTimeKind.Utc, "Start time kind is Unspecified, this could result in incorrect times showing up in Tethr");
			_startTime = value;
		}*/
	}
	Poco::DateTime RecordingInfo::EndTime() const
	{
		return Poco::DateTime();
	}
	void RecordingInfo::EndTime(Poco::DateTime dt)
	{
	}
}

