#pragma once
#include <string>

class AudioMediaTypes
{
public:
	static const std::string Wave;
	static const std::string Mp3;
	static const std::string Opus;

	AudioMediaTypes()
	{
		const std::string Wave = "audio/wav";
		const std::string Mp3 = "audio/mp3";
		const std::string Opus = "audio/ogg";
	}

	~AudioMediaTypes()
	{
	}
};

