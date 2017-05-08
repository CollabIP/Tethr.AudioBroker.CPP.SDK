#include "Token.h"


namespace tethr
{
	Token::Token(): ExpiresInSeconds(0)
	{
	}

	Token::~Token()
	{
	}

	bool Token::IsValid()
	{
		//Calculate Expiration Buffer.  If we are within 45 seconds we need to refresh anyway.
		INT64 expiresInMicroSeconds = (ExpiresInSeconds * 1000000) - 45000000;

		Poco::Timestamp now; // the current date and time UTC
		Poco::Timestamp created = CreatedTimeStamp;

		Poco::Timestamp::TimeDiff expirationBuffer(expiresInMicroSeconds);
		Poco::Timestamp expires = created + expirationBuffer;

		if (expires > now)
		{
			return true;
		}

		return false;
	}
}
