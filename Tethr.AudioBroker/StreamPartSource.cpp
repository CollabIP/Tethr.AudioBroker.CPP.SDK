#include "StreamPartSource.h"


#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/Exception.h"


namespace tethr {

	StreamPartSource::StreamPartSource(const std::istream& stream): _istr(nullptr)
	{
		/*Poco::Path p(path);
		_filename = p.getFileName();
		if (!_istr.good())
			throw OpenFileException(path);*/
	}


	StreamPartSource::StreamPartSource(const std::istream& stream, const std::string& mediaType) : PartSource(mediaType), _istr(nullptr)
	{
		/*Path p(path);
		_filename = p.getFileName();
		if (!_istr.good())
			throw OpenFileException(path);*/
	}

	StreamPartSource::~StreamPartSource()
	{
	}


	std::istream& StreamPartSource::stream()
	{
		return _istr;
	}

	std::streamsize StreamPartSource::getContentLength() const
	{
		//Poco::File p(_path);
		//return p.getSize();

		return 0;
	}

} 
