#include "Session.h"

//Todo: Review Need to catch JSON & SSL exceptions in these methods and rethrow ApplciationException or pass through and notify the caller of exceptions that can be thrown
namespace tethr {
	
	Session::Session(): ResetAuthTokenOnUnauthorized(true)
	{
		Poco::Net::initializeSSL();
		m_pConsoleHandler = new Poco::Net::KeyConsoleHandler(false); //This is kind of silly, but required for proper initialization (otherwise warning - prompts for password when using client side ssl).
		m_pCertificateHandler = new Poco::Net::AcceptCertificateHandler(true);
		m_pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"); //Todo: Review (Verify NONE is not doing any checking.  Ok for now) need cert details

		Poco::Net::SSLManager::instance().initializeClient(m_pConsoleHandler, m_pCertificateHandler, m_pContext);
	}

	Session::~Session()
	{
		Poco::Net::uninitializeSSL();
	}

	Session * Session::GetInstance(std::string hostUri, std::string apiUser, std::string apiPassword)
	{
		static Poco::SingletonHolder<Session> singletonHolder;
		Session *session = singletonHolder.get();

		//Set Host Uri
		if (session->HostUri.empty())
		{
			Poco::URI uri(hostUri);
			session->HostUri = uri;
		}

		//Set Api User
		if (session->ApiUser.empty()) {
			session->ApiUser = apiUser;
		}

		//Set Api Password
		if (session->ApiPassword.empty()) {
			session->ApiPassword = apiPassword;
		}

		return session;
	}

	/// <summary>
	/// Clears the authentication token.
	/// </summary>
	void Session::ClearAuthToken()
	{
		//Todo: Not really sure why this is needed?  Just reassign?  Ask Adam
		Poco::FastMutex::ScopedLock lock(m_mutex);
		m_Token.AccessToken = nullptr;
		m_Token.CreatedTimeStamp = NULL;
		m_Token.ExpiresInSeconds = 0;
	}

	/// <summary>
	/// Gets the API authentication token.
	/// </summary>
	/// <param name="force">if set to <c>true</c> [force].</param>
	/// <returns></returns>
	std::string Session::GetApiAuthToken(bool force)
	{
		bool isValid = m_Token.IsValid();
		if (force == true || isValid == false)
		{
			Poco::FastMutex::ScopedLock lock(m_mutex);

			m_Token = GetClientCredentials();
		}

		return m_Token.AccessToken;
	}
		
	/// <summary>
	/// Gets the specified resource path.
	/// </summary>
	/// <param name="resourcePath">The resource path.  This path should start with a leading slash like /Token or /callCapture/v1/archive </param>
	/// <returns>Poco::DynamicStruct</returns>
	/// <remarks>Throws Poco::ApplicationException on Error</remarks>
	//Todo: Test Get Method
	std::string Session::Get(std::string resourcePath)
	{
		std::string accessToken = GetApiAuthToken();

		//Create the HTTPSClientSession & initialize the request
		Poco::URI uri(HostUri.toString() + resourcePath);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);

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

		if (_stricmp(response.getContentType().c_str(), "application/json; charset=utf-8") == 0)
		{
			//Todo: Review - Just returning the json string here.  I could return a more complex object, but seems cleaner to
			//Todo: just to return the string as is and let the caller parse the data.
			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
			{
				return result.toString();
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
	void Session::Put(std::string resourcePath, Poco::JSON::Object::Ptr body)
	{
		std::string accessToken = GetApiAuthToken();

		//Create the HTTPSClientSession & initialize the request
		Poco::URI uri(HostUri.toString() + resourcePath);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);

		//This call creates the bearer token and adds it to the header
		Poco::Net::OAuth20Credentials(accessToken).authenticate(request);

		std::stringstream bodyStream;
		body->stringify(bodyStream); //Used to set content length
		request.setKeepAlive(true);
		request.setContentLength(bodyStream.str().size());
		request.setContentType("application/json");

		std::ostream& requestStream = session.sendRequest(request);
		body->stringify(requestStream); //Write to request stream (Send)

		Poco::Net::HTTPResponse response;
		
		//Do some basic response code checks
		EnsureAuthorizedStatusCode(response); //This does a status code check along with some other token maintenance (if required)

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND)
			throw Poco::ApplicationException("The requested resource was not found.");

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
	std::string Session::Post(std::string resourcePath, Poco::JSON::Object::Ptr body)
	{
		std::string accessToken = GetApiAuthToken();
		
		//Create the HTTPSClientSession & initialize the request
		Poco::URI uri(HostUri.toString() + resourcePath);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
		
		//This call creates the bearer token and adds it to the header
		Poco::Net::OAuth20Credentials(accessToken).authenticate(request);

		std::stringstream bodyStream;
		body->stringify(bodyStream); //Used to set content length
		request.setKeepAlive(true);
		request.setContentLength(bodyStream.str().size());
		request.setContentType("application/json");

		std::ostream& requestStream = session.sendRequest(request);
		body->stringify(requestStream); //Write to request stream (Send)

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
		
		if (_stricmp(response.getContentType().c_str(), "application/json; charset=utf-8") == 0)
		{
			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
			{
				return result.toString();
			}

			throw Poco::ApplicationException(response.getReason(), response.getStatus());
		}

		std::string error = "Unexpected content type returned from server.  Expected (application/json; charset=utf-8) and received " + response.getContentType();
		throw Poco::ApplicationException(error);
	}
	
	/// <summary>
	/// Posts the mutli part form data.
	/// </summary>
	/// <param name="resourcePath">The resource path.</param>
	/// <param name="recordingInfo">The recording information.</param>
	/// <param name="filePath">The file path.</param>
	/// <param name="dataPartMediaType">Type of the data part media.</param>
	/// <returns>json string</returns>
	std::string Session::PostMutliPartFormData(std::string resourcePath, Poco::JSON::Object::Ptr recordingInfo, std::string filePath, std::string dataPartMediaType)
	{
		std::string accessToken = GetApiAuthToken();

		//Create the HTTPSClientSession & initialize the request
		Poco::URI uri(HostUri.toString() + resourcePath);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
		session.setTimeout(Poco::Timespan(0, 0, 5, 0, 0)); //Set 5 minute timeout - Todo: - Review is this long enough?
		//session.setProxy("localhost", 8888);  //Enable this to send to local fiddler proxy

		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
		request.setTransferEncoding("IDENTITY_TRANSFER_CODING");
		
		//This call creates the bearer token and adds it to the header
		Poco::Net::OAuth20Credentials(accessToken).authenticate(request);

		//Create Multi-Part Form
		std::stringstream infoStream;
		recordingInfo->stringify(infoStream);

		Poco::Net::HTMLForm form;
		form.setEncoding(Poco::Net::HTMLForm::ENCODING_MULTIPART);
		form.addPart("info", new Poco::Net::StringPartSource(infoStream.str(), "application/json; charset=utf-8"));
		form.addPart("data", new Poco::Net::FilePartSource(filePath, "audio/wav"));
		
		form.prepareSubmit(request);

		std::streamsize contentLength = form.calculateContentLength();
		request.setContentLength(contentLength);

		//Send the form
		std::ostream& requestStream = session.sendRequest(request);
		form.write(requestStream);
		
		//form.write(session.sendRequest(request));

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

		if (_stricmp(response.getContentType().c_str(), "application/json; charset=utf-8") == 0)
		{
			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
			{
				return result.toString();
			}

			throw Poco::ApplicationException(response.getReason(), response.getStatus());
		}

		std::string error = "Unexpected content type returned from server.  Expected application/json and received " + response.getContentType();
		throw Poco::ApplicationException(error);
	}

	/// <summary>
	/// Gets the client credentials (token).
	/// </summary>
	/// <param name="clientId">The client identifier.</param>
	/// <param name="clientSecret">The client secret.</param>
	/// <returns>Token Response</returns> 
	/// <remarks>Throws Poco::ApplicationException on Error</remarks>
	Token Session::GetClientCredentials()
	{
		//Create the HTTPSClientSession & initialize the request
		std::string tokenUri = HostUri.getScheme() + "://" + HostUri.getHost() + "/Token";

		Poco::URI uri(tokenUri);
		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);

		//Create the form.  Default is FormUrlEncoded so no need to set anything else here.
		Poco::Net::HTMLForm form;
		form.set("grant_type", "client_credentials");
		form.set("client_id", ApiUser);
		form.set("client_secret", ApiPassword);

		form.prepareSubmit(request);
		std::ostream& requestStream = session.sendRequest(request);

		//Send the request
		form.write(requestStream);

		// Receive the response.
		Poco::Net::HTTPResponse response;
		std::istream& responseStream = session.receiveResponse(response);

		//Parse JSON Response
		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
		{
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var result = parser.parse(responseStream);

			Poco::JSON::Object::Ptr jsonObject = result.extract<Poco::JSON::Object::Ptr>();
			
			Token token;
			Poco::Dynamic::Var tokenType = jsonObject->get("token_type");
			token.TokenType = tokenType.convert<std::string>();

			Poco::Dynamic::Var accessToken = jsonObject->get("access_token");
			token.AccessToken = accessToken.convert<std::string>();

			Poco::Dynamic::Var expiresIn = jsonObject->get("expires_in");
			token.ExpiresInSeconds = expiresIn.convert<long>();

			token.CreatedTimeStamp = Poco::Timestamp(); //Now.  Monotonic time value (uses Microseconds)

			if (token.TokenType.compare("bearer") != 0)
			{
				throw Poco::ApplicationException("InvalidOperationException: Tethr only supports Bearer tokens");
			}
			
			return token;
		}
		
		throw Poco::ApplicationException("An unexpected response was returned while trying to get an access token.", response.getStatus());
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
}

