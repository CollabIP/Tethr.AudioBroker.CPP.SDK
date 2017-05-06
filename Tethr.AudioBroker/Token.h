#pragma once
#include <Poco/Timestamp.h>
#include "AudioBroker.h"

namespace tethr
{
	class AUDIOBROKER_API Token
	{
	public:
		Token();
		~Token();

		std::string AccessToken;
		std::string TokenType;
		long ExpiresInSeconds;
		Poco::Timestamp CreatedTimeStamp;

		bool IsValid();
	};
}
