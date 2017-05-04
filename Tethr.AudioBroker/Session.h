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
#include "Poco/Net/FilePartSource.h"
#include "Poco/SingletonHolder.h"
#include "RecordingInfo.h"

namespace tethr {

	class AUDIOBROKER_API Session
	{
	public:
		Session();
		~Session();
		
		Poco::URI HostUri;
		std::string ApiUser;
		std::string ApiPassword;

		bool ResetAuthTokenOnUnauthorized; //When set this will automatically reauthorize the token on the next request if 401 Unauthorized is received

		void ClearAuthToken();
		
		Poco::DynamicStruct Get(std::string resourcePath);
		void Put(std::string resourcePath, Poco::JSON::Object body); //In the original SDK this is a POST, but cant overload in the same way here.  
																	 //Using Put as the function name it does not provide a response so might be close to Put equivalent
		Poco::DynamicStruct Post(std::string resourcePath, Poco::JSON::Object body);
		Poco::DynamicStruct PostMutliPartFormData(std::string resourcePath, Poco::JSON::Object info, std::string filePath, std::string dataPartMediaType = "application/octet-stream");
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

		TokenResponse _apiToken;

		std::string GetApiAuthToken(bool force = false);
		TokenResponse GetClientCredentials(std::string clientId, std::string clientSecret) const;

		void EnsureAuthorizedStatusCode(Poco::Net::HTTPResponse &response);
	};
}
