#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TETHRTEST_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TETHRTEST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//
// Ensure that AUDIOBROKER_DLL is default unless AUDIOBROKER_STATIC is defined
//
#if defined(_WIN32) && defined(_DLL)
	#if !defined(AUDIOBROKER_DLL) && !defined(AUDIOBROKER_STATIC)
		#define AUDIOBROKER_DLL
	#endif
#endif


#if defined(_WIN32) && defined(AUDIOBROKER_DLL)
	#if defined(AUDIOBROKER_EXPORTS)
		#define AUDIOBROKER_API __declspec(dllexport)
	#else
		#define AUDIOBROKER_API __declspec(dllimport)
	#endif
#endif

#if !defined(AUDIOBROKER_API)
	#if !defined(AUDIOBROKER_NO_GCC_API_ATTRIBUTE) && defined (__GNUC__) && (__GNUC__ >= 4)
		#define AUDIOBROKER_API __attribute__ ((visibility ("default")))
	#else
		#define AUDIOBROKER_API
	#endif
#endif

//Include Additional Headers
#include <string>
#include "Poco/AutoPtr.h"
#include "Poco/Util/PropertyFileConfiguration.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Path.h"
#include "Poco/Foundation.h"

//Todo: Make XPlatform