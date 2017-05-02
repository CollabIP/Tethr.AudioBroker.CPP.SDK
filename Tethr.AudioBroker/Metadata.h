#pragma once
#include <string>
#include <map>

//Todo:  This should be templated for storing anything that can be converted to json.  Keeping simple with strings right now.
namespace tethr {
	struct Metadata
	{
		std::map<std::string, std::string> Data;
	};
}
