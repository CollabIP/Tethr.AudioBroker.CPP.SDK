#pragma once
#include "AudioBroker.h"
#include <string>
#include <map>
#include "Poco/Mutex.h"
#include "Poco/DateTime.h"
#include "Poco/LocalDateTime.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/OAuth20Credentials.h"
#include "Poco/Util/JSONConfiguration.h"
#include "Poco/JSON/Parser.h"
#include "Poco/URI.h"

namespace tethr {
	class AUDIOBROKER_API Session
	{
	public:
		//Todo: Not sure if we need to template this or overload for wstring given that this is a customer facing SDK
		//Todo: that may be used internationally.  String should be fine, but doesn't hurt to refresh platform knowledge
		Session(std::string hostUri, std::string apiUser, std::string apiPassword);
		Session(std::string configurationFile);
		~Session();

		void ClearAuthToken();
	private:

		class TokenResponse
		{
		public:
			TokenResponse();
			~TokenResponse();

			// leaving access token as a string as there is no security gained from anything else and only 
			// slows down the calls.  In normal use cases this is used often for the lifetime of the Token, 
			// meaning that string is in clear text the entire time it's valid anyway.
			std::string AccessToken;
			std::string TokenType;
			long ExpiresInSeconds;
			Poco::Timestamp CreatedTimeStamp;

			bool IsValid() const;
		};

		Poco::FastMutex  _mutex;

		Poco::URI _hostUri;
		std::string _apiUser;
		std::string _apiPassword;

		TokenResponse _apiToken;

		std::string GetApiAuthToken(bool force = false);
		TokenResponse GetClientCredentials(std::string clientId, std::string clientSecret) const;
	};
}
