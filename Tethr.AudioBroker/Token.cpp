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
		return false;
	}
}
