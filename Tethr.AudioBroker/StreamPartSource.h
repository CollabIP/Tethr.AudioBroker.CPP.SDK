#pragma once

#include "Poco/Net/Net.h"
#include "Poco/Net/PartSource.h"
#include "Poco/FileStream.h"
#include "AudioBroker.h"

namespace tethr {

	class AUDIOBROKER_API StreamPartSource : public Poco::Net::PartSource
	{
	public:
		StreamPartSource(const std::istream& stream);

		StreamPartSource(const std::istream& stream, const std::string& mediaType);

		~StreamPartSource();

		std::istream& stream();

		std::streamsize getContentLength() const;

	private:
		std::istream _istr;
	};
} 

