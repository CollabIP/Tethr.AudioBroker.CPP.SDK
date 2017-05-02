#pragma once
#include <vector>
#include <Poco/DateTime.h>
#include "CallDirection.h"
#include "Contact.h"
#include "Audio.h"
#include "Metadata.h"

namespace tethr{
	class RecordingInfo
	{
	public:
		RecordingInfo();
		~RecordingInfo();
		
		/// <summary>
		/// The session identifier (Required).
		/// </summary>
		/// <remarks>
		/// Session ID needs to be unique to any and all recordings uploaded to a given instance of Tethr
		///
		/// When not used in conjunction with a MasterCallId, Session ID should be something
		/// that can be used by other systems to connect a call in Tethr to the same call in other systems.
		///
		/// When a MasterCallId is provided, the Master call ID can be used link a Tethr call to other systems.
		/// But the SessionID should still be something that the originating call recorder can use to lookup
		/// a given recording with.  This allows for the recorder to validate that Tethr did in fact get a call
		/// as part of a regular integrity or verification process.
		/// </remarks>
		std::string SessionId;

		/// <summary>
		/// (Optional) A unique identify that may be shared between one or more calls, that together represent one complete calls.
		/// </summary>
		/// <remarks>
		/// Some call records will split a call into multiple calls when the caller is transferred or another participant is added.
		/// This master call ID can be used to tell Tethr that a set of call are really one call.
		///
		/// In these cases, it allows Tethr to treated each part of the call as own call, but still be able
		/// to link what happened in prior call to an event in this call, or even a future call.
		/// </remarks>
		std::string MasterCallId;

		/// <summary>
		/// (Optional) The direction the call was originated
		/// </summary>
		/// <remarks>
		/// The definitions are a lose outline and can be adapted to the customers needs, but should be consistent.
		/// </remarks>
		CallDirection Direction;

		/// <summary>
		/// The start time of the call, this value should be in UTC.
		/// </summary>
		/// <remarks>
		/// it is important that the audio length and the reported call length are the same.
		/// If this is not true, Tethr may have harder time accurately detecting what actually happened on a call
		/// By default, Tethr will quarantine calls where the reported time and audio length do not match, and wait for
		/// human interaction to fix any miss configurations.  (Contact Support if you think you this will happen often)
		/// </remarks>
		Poco::DateTime StartTime() const;  // Getter
		void StartTime(Poco::DateTime dt); // Setter

		/// <summary>
		/// The end time of the call, this value should be in UTC.
		/// </summary>
		Poco::DateTime EndTime() const;  //Getter
		void EndTime(Poco::DateTime dt); //Setter

		/// <summary>
		/// List of contacts or participants on the call
		/// </summary>
		std::vector<Contact> Contacts;

		/// <summary>
		/// (Optional) The number that was dialed that initiated this call.
		/// </summary>
		/// <remarks>
		/// This is primarily used with inbound calls, where the number dialed is a trunk line and any number
		/// of internal devices could pick up the call.  It's particularly useful when combined with special
		/// promotions where each promotion is given it's own phone number.
		///
		/// Where the number dialed is a Direct Inward Dialing of a single station or an outbound number
		/// this value is best left null or empty.
		/// </remarks>
		std::string NumberDialed;

		/// <summary>
		/// Information about the type of Audio that is being uploaded.
		/// </summary>
		/// <remarks>
		/// In a future release, when sending Archived Recordings the media type of the audio part is used
		/// and this property is ignored.
		///
		/// This property will still be used in Live Call processing, and with some out of band audio feeds.
		/// </remarks>
		Audio Audio;

		/// <summary>
		/// Any extra metadata related to the call that could be used by Tethr.
		/// </summary>
		/// <remarks>
		/// The metadata is read by Tethr and used classify calls, Examples:
		/// Contact Center the call was answered in, the disposition code entered by the agent,
		/// the skill group / or product the call was related to, the call routing group from the IVR,
		/// an identifier for phone the agent used to make the call.
		///
		/// There is really no limit to what can be put in there, as long as it can be converted to JSON.
		/// </remarks>
		Metadata Metadata;
	private:
		Poco::DateTime _startTime;
		Poco::DateTime _endTime;
		
	};
}


