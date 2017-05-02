// Tethr.AudioUpload.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

SendFileResult SendFile(std::string fileName)
{
	// Create an Instance of the Archive Recording provider used to send archived audio to Tethr
	/*var tethrConnection = new TethrArchivedRecording(_tethrSession);

	// Figure out the file name of the sample file we are going to send to Tethr and read it in.
	using (var jsonStream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read))
	{
		var recording = jsonStream.JsonDeserialize<RecordingInfo>();

		// NOTE: Session ID should be something that can be used by other systems to connect a call in Tethr
		// to the same call in other systems.  The sessionID must be unique per recording.
		//
		// For this example, we are appending the time so we can test with the same file multiple times.
		// but should be a more meaningful value in production.
		recording.SessionId += DateTime.UtcNow.ToString("o");

		// NOTE: it is important that the audio length and the reported call length are the same.
		// If this is not true, Tethr may have harder time accurately detecting what actually happened on a call
		// By default, Tethr will quarantine calls where the reported time and audio length do not match, and wait for
		// human interaction to fix any miss configurations.  (Contact Support if you think you need this changed)
		//
		// for this example, we are Update the call times, so the calls don't always have the same time.
		// and if you change out the audio file in the sample, you will have to re-compute the end time in the sample JSON file.
		var audioLength = recording.EndTime - recording.StartTime;
		recording.StartTime = DateTime.UtcNow;
		recording.EndTime = recording.StartTime + audioLength;

		// Open the sample wave file.
		using (var wavStream = new FileStream(Path.ChangeExtension(fileName, ".wav"), FileMode.Open,
			FileAccess.Read, FileShare.Read))
		{
			// Send the audio and metadata to Tethr.
			var result = await tethrConnection.SendRecordingAsync(recording, wavStream, AudioMediaTypes.Wave);
			return new SendFileResult
			{
				CallId = result.CallId,
				SessionId = recording.SessionId,
				StartTime = recording.StartTime
			};
		}
	}*/
	return SendFileResult();
}

int main()
{
	
	//Create the Session
	// The Session object should be a singleton, and reused on subsequent sends so that
	// the oauth bearer token can be reused and refreshed only when it expires
	tethr::Session session("Configuration.Properties");

	//Send the file
	std::string fileName = "SampleRecording.json";
	SendFileResult result = SendFile(fileName);

	//Todo: Convert to LocalTime
	std::string localTime(Poco::DateTimeFormatter::format(result.startTime, "%e %b %Y %H:%M"));

	std::cout << "Sent recording:" << std::endl;
	std::cout << "Session Id      : " << result.sessionId << std::endl;
	std::cout << "Call Start Time : " << localTime << std::endl;
	std::cout << "Tethr Call Id   : " << result.callId << std::endl;

	std::cout << "Press Enter to Exit";
	std::cin.ignore();
	
    return 0;
}



