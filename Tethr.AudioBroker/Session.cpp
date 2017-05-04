#include "Session.h"

namespace tethr {
	
	Session::Session(): ResetAuthTokenOnUnauthorized(true)
	{
		Poco::Net::initializeSSL(); //Review:: maybe use Poco::Crypto::OpenSSLInitializer::initialize(); here instead
	}

	Session::~Session()
	{
		Poco::Net::uninitializeSSL();
	}

	// <summary>
	/// Instances this instance as a Singleton
	/// </summary>
	/// <returns></returns>
	static Session& instance()
	{
		static Poco::SingletonHolder<Session> singletonHolder;
		return *singletonHolder.get();
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
	/// Gets the specified resource path.
	/// </summary>
	/// <param name="resourcePath">The resource path.  This path should start with a leading slash like /Token or /callCapture/v1/archive </param>
	/// <returns>Poco::DynamicStruct</returns>
	/// <remarks>Throws Poco::ApplicationException on Error</remarks>
	Poco::DynamicStruct Session::Get(std::string resourcePath)
	{
		//This is barely documented anywhere by POCO but you do need an SSL Manager for the request or risk getting odd/difficult to diagnose exceptions.
		//Note that InitalizeSSL is called in the constructor.
		Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrHandler = new Poco::Net::AcceptCertificateHandler(false);
		Poco::Net::Context context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
		Poco::Net::SSLManager::instance().initializeClient(nullptr, ptrHandler, &context);

		//Create the HTTPSClientSession & initialize the request
		// Create the request URI.
		Poco::URI uri(HostUri.toString() + resourcePath);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), &context);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);

		std::string accessToken = GetApiAuthToken();

		//This call creates the bearer token and adds it to the header
		Poco::Net::OAuth20Credentials(accessToken).authenticate(request);

		//Send the HTTP GET Request
		session.sendRequest(request);

		// Receive the response.
		Poco::Net::HTTPResponse response;
		std::istream& responseStream = session.receiveResponse(response);

		//Do some basic response code checks
		EnsureAuthorizedStatusCode(response); //This does a status code check along with some other token maintenance (if required)

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND)
			throw Poco::ApplicationException("The requested resource was not found.");

		//Parse JSON Response
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(responseStream);

		if (_stricmp(response.getContentType().c_str(), "application/json") == 0)
		{
			Poco::JSON::Object::Ptr pJsonObject = result.extract<Poco::JSON::Object::Ptr>();

			//Todo:  Review - Not really sure if I should return the ptr here or if I should copy to a DynamicStruct
			//Todo:  The .NET code serializes the json to an object of type T so a dynamic struct may be more in line with this method, but why not just
			//Todo:  return the json in an object and let the caller do what they will?  DynamicStruct is probably the right answer though
			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
			{
				// copy/convert to Poco::DynamicStruct
				Poco::DynamicStruct ds = *pJsonObject;
				return ds;
			}

			throw Poco::ApplicationException(response.getReason(), response.getStatus());
		}

		std::string error = "Unexpected content type returned from server.  Expected application/json and received " + response.getContentType();
		throw Poco::ApplicationException(error);
	}
	
	/// <summary>
	/// Puts the specified resource path.
	/// </summary>
	/// <param name="resourcePath">The resource path.  This path should start with a leading slash like /Token or /callCapture/v1/archive </param>
	/// <param name="body">The body as a JSON Object</param>
	/// <remarks>Throws Poco::ApplicationException on Error</remarks>
	void Session::Put(std::string resourcePath, Poco::JSON::Object body)
	{
		//This is barely documented anywhere by POCO but you do need an SSL Manager for the request or risk getting odd/difficult to diagnose exceptions.
		//Note that InitalizeSSL is called in the constructor.
		Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrHandler = new Poco::Net::AcceptCertificateHandler(false);
		Poco::Net::Context context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
		Poco::Net::SSLManager::instance().initializeClient(nullptr, ptrHandler, &context);

		//Create the HTTPSClientSession & initialize the request
		// Create the request URI.
		Poco::URI uri(HostUri.toString() + resourcePath);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), &context);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
		std::string accessToken = GetApiAuthToken();

		//This call creates the bearer token and adds it to the header
		Poco::Net::OAuth20Credentials(accessToken).authenticate(request);

		std::stringstream bodyStream;
		body.stringify(bodyStream); //Used to set content length
		request.setKeepAlive(true);
		request.setContentLength(bodyStream.str().size());
		request.setContentType("application/json");  

		std::ostream& requestStream = session.sendRequest(request);
		body.stringify(requestStream); //Write to request stream (Send)

		// Receive the response.
		Poco::Net::HTTPResponse response;
		std::istream& responseStream = session.receiveResponse(response);

		//Parse JSON Response
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(responseStream);

		//Do some basic response code checks
		EnsureAuthorizedStatusCode(response); //This does a status code check along with some other token maintenance (if required)

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND)
			throw Poco::ApplicationException("The requested resource was not found.");

		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

		if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
		{
			throw Poco::ApplicationException(response.getReason(), response.getStatus());
		}
	}
	
	/// <summary>
	/// Posts the specified resource path.
	/// </summary>
	/// <param name="resourcePath">The resource path.  This path should start with a leading slash like /Token or /callCapture/v1/archive </param>
	/// <param name="body">The body as a JSON Object</param>
	/// <returns>Poco::DynamicStruct</returns>
	/// <remarks>Throws Poco::ApplicationException on Error</remarks>
	Poco::DynamicStruct Session::Post(std::string resourcePath, Poco::JSON::Object body)
	{
		//This is barely documented anywhere by POCO but you do need an SSL Manager for the request or risk getting odd/difficult to diagnose exceptions.
		//Note that InitalizeSSL is called in the constructor.
		Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrHandler = new Poco::Net::AcceptCertificateHandler(false);
		Poco::Net::Context context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
		Poco::Net::SSLManager::instance().initializeClient(nullptr, ptrHandler, &context);

		//Create the HTTPSClientSession & initialize the request
		// Create the request URI.
		Poco::URI uri(HostUri.toString() + resourcePath);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), &context);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
		std::string accessToken = GetApiAuthToken();

		//This call creates the bearer token and adds it to the header
		Poco::Net::OAuth20Credentials(accessToken).authenticate(request);

		std::stringstream bodyStream;
		body.stringify(bodyStream); //Used to set content length
		request.setKeepAlive(true);
		request.setContentLength(bodyStream.str().size());
		request.setContentType("application/json");

		std::ostream& requestStream = session.sendRequest(request);
		body.stringify(requestStream); //Write to request stream (Send)

		// Receive the response.
		Poco::Net::HTTPResponse response;
		std::istream& responseStream = session.receiveResponse(response);

		//Do some basic response code checks
		EnsureAuthorizedStatusCode(response); //This does a status code check along with some other token maintenance (if required)

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND)
			throw Poco::ApplicationException("The requested resource was not found.");

		//Parse JSON Response
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(responseStream);

		if (_stricmp(response.getContentType().c_str(), "application/json") == 0)
		{
			Poco::JSON::Object::Ptr pJsonObject = result.extract<Poco::JSON::Object::Ptr>();

			//Todo:  Review - Not really sure if I should return the ptr here or if I should copy to a DynamicStruct
			//Todo:  The .NET code serializes the json to an object of type T so a dynamic struct may be more in line with this method, but why not just
			//Todo:  return the json in an object and let the caller do what they will?  DynamicStruct is probably the right answer though
			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
			{
				// copy/convert to Poco::DynamicStruct
				Poco::DynamicStruct ds = *pJsonObject;
				return ds;
			}

			throw Poco::ApplicationException(response.getReason(), response.getStatus());
		}

		std::string error = "Unexpected content type returned from server.  Expected application/json and received " + response.getContentType();
		throw Poco::ApplicationException(error);
	}

	Poco::DynamicStruct Session::PostMutliPartFormData(std::string resourcePath, Poco::JSON::Object info, std::string filePath, std::string dataPartMediaType)
	{
		//This is barely documented anywhere by POCO but you do need an SSL Manager for the request or risk getting odd/difficult to diagnose exceptions.
		//Note that InitalizeSSL is called in the constructor.
		Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrHandler = new Poco::Net::AcceptCertificateHandler(false);
		Poco::Net::Context context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
		Poco::Net::SSLManager::instance().initializeClient(nullptr, ptrHandler, &context);

		//Create the HTTPSClientSession & initialize the request
		// Create the request URI.
		Poco::URI uri(HostUri.toString() + resourcePath);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), &context);
		
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
		std::string accessToken = GetApiAuthToken();

		//This call creates the bearer token and adds it to the header
		Poco::Net::OAuth20Credentials(accessToken).authenticate(request);

		//Set 5 minute timeout
		session.setTimeout(Poco::Timespan(0, 0, 5, 0, 0));

		request.setKeepAlive(true);
		
		request.setContentType("application/octet-stream");

		//Stringify Info
		std::stringstream infoStream;
		info.stringify(infoStream); 

		//Create Multi-Part Form
		Poco::Net::HTMLForm form;
		form.setEncoding(Poco::Net::HTMLForm::ENCODING_MULTIPART);
		form.set("info", infoStream.str().c_str());
		form.addPart("data", new Poco::Net::FilePartSource(filePath, dataPartMediaType));

		form.prepareSubmit(request);

		//Send the form
		form.write(session.sendRequest(request));


		// Receive the response.
		Poco::Net::HTTPResponse response;
		std::istream& responseStream = session.receiveResponse(response);

		//Do some basic response code checks
		EnsureAuthorizedStatusCode(response); //This does a status code check along with some other token maintenance (if required)

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND)
			throw Poco::ApplicationException("The requested resource was not found.");

		//Parse JSON Response
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(responseStream);

		if (_stricmp(response.getContentType().c_str(), "application/json") == 0)
		{
			Poco::JSON::Object::Ptr pJsonObject = result.extract<Poco::JSON::Object::Ptr>();

			//Todo:  Review - Not really sure if I should return the ptr here or if I should copy to a DynamicStruct
			//Todo:  The .NET code serializes the json to an object of type T so a dynamic struct may be more in line with this method, but why not just
			//Todo:  return the json in an object and let the caller do what they will?  DynamicStruct is probably the right answer though
			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
			{
				// copy/convert to Poco::DynamicStruct
				Poco::DynamicStruct ds = *pJsonObject;
				return ds;
			}

			throw Poco::ApplicationException(response.getReason(), response.getStatus());
		}

		std::string error = "Unexpected content type returned from server.  Expected application/json and received " + response.getContentType();
		throw Poco::ApplicationException(error);
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
			
			TokenResponse token = GetClientCredentials(ApiUser, ApiPassword);
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
		Poco::URI uri(HostUri.toString() + "/Token");
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
	
	/// <summary>
	/// Ensures the authorized status code.
	/// </summary>
	/// <param name="response">The HTTPResponse from the call you made</param> 
	/// <remarks>throws ApplicationException if Unathorized or Forbidden</remarks>
	void Session::EnsureAuthorizedStatusCode(Poco::Net::HTTPResponse &response)
	{
		switch (response.getStatus())
		{
		case Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED:
		{
			if (ResetAuthTokenOnUnauthorized)
				ClearAuthToken();
			throw Poco::ApplicationException("Request returned 401 (Unauthorized)");
		}
		case Poco::Net::HTTPResponse::HTTP_FORBIDDEN:
			throw Poco::ApplicationException("Request returned 403 (Forbidden)");
		default:;
		}
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
#pragma endregion
}

