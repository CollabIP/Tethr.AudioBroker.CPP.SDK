#include "Session.h"
#include "Configuration.h"

namespace tethr {
	
	/// <summary>
	/// Initializes a new instance of the <see cref="Session"/> class.
	/// </summary>
	/// <param name="hostUri">https://yourcompanyname.audio.labs.Tethr.io</param>
	/// <param name="apiUser">UserName</param>
	/// <param name="password">Password</param>
	Session::Session(std::string hostUri, std::string apiUser, std::string apiPassword)
	{
		_hostUri = hostUri;
		_apiUser = apiUser;
		_apiPassword = apiPassword;

		Poco::Net::initializeSSL();  //Review:: maybe use Poco::Crypto::OpenSSLInitializer::initialize(); here instead
	}
	
	/// <summary>
	/// Initializes a new instance of the <see cref="Session"/> class.
	/// </summary>
	/// <param name="configurationFile">The configuration file.</param>
	Session::Session(std::string configurationFile)
	{
		ConnectionString connectionString = Configuration::LoadConfiguration(configurationFile);

		_hostUri = connectionString.HostUri;
		_apiUser = connectionString.ApiUser;
		_apiPassword = connectionString.Password;

		Poco::Net::initializeSSL();  //Review:: maybe use Poco::Crypto::OpenSSLInitializer::initialize(); here instead
	}

	Session::~Session()
	{
	}
	
	/// <summary>
	/// Clears the authentication token.
	/// </summary>
	void Session::ClearAuthToken()
	{
		//Todo: Not really sure why this is needed?  Just reassign?  Ask Adam
		Poco::FastMutex::ScopedLock lock(_mutex);
		_apiToken.AccessToken = nullptr;
		_apiToken.CreatedTimeStamp = NULL;
		_apiToken.ExpiresInSeconds = 0;
	}
	
	/// <summary>
	/// Gets the API authentication token.
	/// </summary>
	/// <param name="force">if set to <c>true</c> [force].</param>
	/// <returns></returns>
	std::string Session::GetApiAuthToken(bool force)
	{
		bool isValid = _apiToken.IsValid();
		if (force == true || isValid == false)
		{
			Poco::FastMutex::ScopedLock lock(_mutex);
			
			TokenResponse token = GetClientCredentials(_apiUser, _apiPassword);
			_apiToken = token;
		}

		return _apiToken.AccessToken;
	}
	
	/// <summary>
	/// Gets the client credentials (token).
	/// </summary>
	/// <param name="clientId">The client identifier.</param>
	/// <param name="clientSecret">The client secret.</param>
	/// <returns>Token Response</returns> 
	/// <remarks>Throws Poco::ApplicationException on Error</remarks>
	Session::TokenResponse Session::GetClientCredentials(std::string clientId, std::string clientSecret) const
	{	
		//This is barely documented anywhere by POCO but you do need an SSL Manager for the request or risk getting odd/difficult to diagnose exceptions.
		//Note that InitalizeSSL is called in the constructor.
		Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrHandler = new Poco::Net::AcceptCertificateHandler(false);
		Poco::Net::Context context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
		Poco::Net::SSLManager::instance().initializeClient(nullptr, ptrHandler, &context);

		//Create the HTTPSClientSession & initialize the request
		// Create the request URI.
		Poco::URI uri(_hostUri.toString() + "/Token");
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), &context);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);

		//Create the form.  Default is FormUrlEncoded so no need to set anything else here.
		Poco::Net::HTMLForm form;
		form.set("grant_type", "client_credentials");
		form.set("client_secret", clientSecret);
		form.set("client_id", clientId);

		form.prepareSubmit(request);
		std::ostream& requestStream = session.sendRequest(request);

		//Send the request
		form.write(requestStream);

		// Receive the response.
		Poco::Net::HTTPResponse response;
		std::istream& responseStream = session.receiveResponse(response);
		
		//Parse JSON Response
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(responseStream);

		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
		
		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
		{
			//If everything is ok then parse the JSON response to a TokenResponse and return the result
			//Otherwise you are getting an exception.
			std::string token_type = object->getValue<std::string>("token_type");
			if (token_type.compare("bearer") != 0)
			{
				throw Poco::ApplicationException("InvalidOperationException: Tethr only supports Bearer tokens");
			}

			TokenResponse tokenResponse;
			tokenResponse.TokenType = token_type;
			tokenResponse.AccessToken = object->getValue<std::string>("access_token");
			tokenResponse.CreatedTimeStamp = Poco::Timestamp(); //Now.  Monotonic time value (uses Microseconds)
			tokenResponse.ExpiresInSeconds = object->getValue<int>("expires_in");

			return tokenResponse;
		}
		
		std::string error = object->getValue<std::string>("error");
		throw Poco::ApplicationException(error, response.getStatus());
	}

#pragma region Nested Classes
	Session::TokenResponse::TokenResponse(): ExpiresInSeconds(0)
	{
	}

	Session::TokenResponse::~TokenResponse()
	{
	}
	
	/// <summary>
	/// Returns true if ... is valid.
	/// </summary>
	/// <returns>
	///   <c>true</c> if this instance is valid; otherwise, <c>false</c>.
	/// </returns>
	bool Session::TokenResponse::IsValid() const
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
	std::string Session::TokenResponse::Get(std::string resourcePath)
	{
		return std::string();
	}
#pragma endregion
}

