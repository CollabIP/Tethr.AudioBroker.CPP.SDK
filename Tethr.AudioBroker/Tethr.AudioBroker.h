#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TETHRTEST_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TETHRTEST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TETHRAUDIOBROKER_EXPORTS
#define TETHRAUDIOBROKER_API __declspec(dllexport)
#else
#define TETHRAUDIOBROKER_API __declspec(dllimport)
#endif

// This class is exported from the Tethr.Test.dll
class TETHRAUDIOBROKER_API CTethrAudioBroker {
public:
	CTethrAudioBroker(void);
	// TODO: add your methods here.
};

extern TETHRAUDIOBROKER_API int nTethrTest;

TETHRAUDIOBROKER_API int fnTethrAudioBroker(void);

//Include Additional Headers

#include <string>
#include "Poco/AutoPtr.h"
#include "Poco/Util/PropertyFileConfiguration.h"

#include "Configuration.h"
#include "ConnectionString.h"
#include "Session.h"

//Todo: Make XPlatform