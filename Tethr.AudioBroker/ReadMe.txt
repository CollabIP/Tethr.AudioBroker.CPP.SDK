========================================================================
    STATIC LIBRARY : Tethr.AudioBroker Project Overview
========================================================================

AppWizard has created this Tethr.AudioBroker library project for you.

No source files were created as part of your project.


Tethr.AudioBroker.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

Tethr.AudioBroker.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

/////////////////////////////////////////////////////////////////////////////
Other notes:

This is a public SDK and we are using a more explicit syntax in code to ensure backward 
compatibility as well as limiting the newer C++ features and using OpenSSL for cross platform. 

This SDK should remain compatible with older versions of Visual Studio (2008) for the forseeable future.


Todo: Implement/Review Exception Handling and/or status code responses to callers
Todo: Add logging and logger injection.
Todo: Add Async Methods - I'm not sure if POCO has this today.  C++ REST SDK may be the best option, but will required C++11 support
Todo: Unit Tests!

/////////////////////////////////////////////////////////////////////////////
