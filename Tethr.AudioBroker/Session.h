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
#include "Poco/Net/SSLException.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/OAuth20Credentials.h"
#include "Poco/Util/JSONConfiguration.h"
#include "Poco/JSON/Parser.h"
#include "Poco/URI.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/StringPartSource.h"
#include "Poco/SingletonHolder.h"
#include "Poco/UUIDGenerator.h"
#include "Token.h"

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
		std::string GetApiAuthToken(bool force = true);
		
		std::string Get(std::string resourcePath);
		void Put(std::string resourcePath, Poco::JSON::Object::Ptr body); //In the original SDK this is a POST, but cant overload in the same way here.  								 //Using Put as the function name it does not provide a response so might be close to Put equivalent
		std::string Post(std::string resourcePath, Poco::JSON::Object::Ptr body);
		std::string PostMutliPartFormData(std::string resourcePath, Poco::JSON::Object::Ptr recordingInfo, std::string filePath, std::string dataPartMediaType = "application/octet-stream");
	private:

		Poco::SharedPtr<Poco::Net::PrivateKeyPassphraseHandler> pConsoleHandler;
		Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> m_pCertificateHandler;
		Poco::Net::Context::Ptr m_pContext;

		std::string m_apiToken;

		Poco::FastMutex  _mutex;

		Token _apiToken;
		Token GetClientCredentials();
		void EnsureAuthorizedStatusCode(Poco::Net::HTTPResponse &response);
	};
}
