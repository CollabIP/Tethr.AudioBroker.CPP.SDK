#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TETHRTEST_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TETHRTEST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef AUDIOBROKER_EXPORTS
#define AUDIOBROKER_API __declspec(dllexport)
#else
#define AUDIOBROKER_API __declspec(dllimport)
#endif

// This class is exported from the Tethr.Test.dll
class AUDIOBROKER_API CTethrAudioBroker {
public:
	CAudioBroker(void);
	// TODO: add your methods here.
};

//Include Additional Headers
#include <string>
#include "Poco/AutoPtr.h"
#include "Poco/Util/PropertyFileConfiguration.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"

#include "Configuration.h"
#include "ConnectionString.h"
#include "SendFileResult.h"
#include "Session.h"

//Todo: Make XPlatform