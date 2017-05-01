#include "Session.h"
#include "Configuration.h"

namespace tethr {

	
	Session::Session(std::string hostUri, std::string apiUser, std::string apiPassword)
	{
		_hostUri = hostUri;
		_apiUser = apiUser;
		_apiPassword = apiPassword;
	}

	Session::Session(std::string configurationFile)
	{
		ConnectionString connectionString = Configuration::LoadConfiguration(configurationFile);

		_hostUri = connectionString.hostUri;
		_apiUser = connectionString.apiUser;
		_apiPassword = connectionString.password;
	}

	Session::~Session()
	{
	}

	void Session::ClearAuthToken()
	{
		//Todo: Not really sure why this is needed?  Just reassign?  Ask Adam
		Poco::FastMutex::ScopedLock lock(_mutex);
		_apiToken.AccessToken = nullptr;
		_apiToken.CreatedTimeStamp = NULL;
		_apiToken.ExpiresInSeconds = 0;
	}

	std::string Session::GetApiAuthToken(bool force)
	{
		bool isValid = _apiToken.IsValid();
		if (force == true || isValid == false)
		{
			Poco::FastMutex::ScopedLock lock(_mutex);
			
			TokenResponse token = GetClientCredentialsAsync(_apiUser, _apiPassword);
			_apiToken = token;
		}

		return _apiToken.AccessToken;
	}

	Session::TokenResponse Session::GetClientCredentialsAsync(std::string clientId, std::string clientSecret)
	{
		//Create the form.  Default is FormUrlEncoded
		Poco::Net::HTMLForm form;
		form.set("grant_type", "client_credentials");
		form.set("client_secret", clientSecret);
		form.set("client_id", clientId);
		
		// Create the request URI.
		Poco::URI uri(_hostUri.toString() + "/Token");

		Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);

		// Send the request.
		form.prepareSubmit(request);
		std::ostream& ostr = session.sendRequest(request);
		form.write(ostr);

		// Receive the response.
		Poco::Net::HTTPResponse response;
		std::istream& rs = session.receiveResponse(response);

		Poco::AutoPtr<Poco::Util::JSONConfiguration> pResult = new Poco::Util::JSONConfiguration(rs);

		// If everything went fine, return the JSON document.
		// Otherwise throw an exception.
		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
		{
			//return pResult;
		}
		else
		{
			//throw Poco::ApplicationException("Twitter Error", pResult->getString("errors[0].message", ""));
		}
		
		return TokenResponse();
	}

#pragma region Nested Classes
	Session::TokenResponse::TokenResponse(): ExpiresInSeconds(0)
	{
	}

	Session::TokenResponse::~TokenResponse()
	{
	}

	bool Session::TokenResponse::IsValid() const
	{
		//Calculate Expiration Buffer.  If we are within 45 seconds we need to refresh anyway.
		INT64 expiresInMicroSeconds = (ExpiresInSeconds * 1000000) - 45000000;

		Poco::Timestamp now; // the current date and time UTC
		Poco::Timestamp created = CreatedTimeStamp.timestamp();

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

