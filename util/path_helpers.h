// Copyright 2010-2014 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#ifndef UTIL__PATH_HELPERS_H_
#define UTIL__PATH_HELPERS_H_

#ifdef _WIN32
#	include <Windows.h>
#	include <Shlobj.h>
//#	pragma comment(lib,"Shell32.lib")
#else
#	error "not implemented"
#endif

// c++0x workaround
#if _MSC_VER < 1600
#define nullptr NULL
#endif

#if 0
#include <boost/filesystem.hpp>
#endif

#include <string>

namespace fmu
{

namespace
{

enum EShellFolderType
{
	// http://msdn.microsoft.com/en-us/library/bb762494(v=vs.85).aspx
	eShellFolderType_ADMINTOOLS = CSIDL_ADMINTOOLS,
	//eShellFolderType_ALTSTARTUP
	//eShellFolderType_APPDATA
	//eShellFolderType_BITBUCKET
	//eShellFolderType_CDBURN_AREA
	//eShellFolderType_COMMON_ADMINTOOLS
	//eShellFolderType_COMMON_ALTSTARTUP
	eShellFolderType_COMMON_APPDATA = CSIDL_COMMON_APPDATA
									  //eShellFolderType_COMMON_DESKTOPDIRECTORY
									  //eShellFolderType_COMMON_DOCUMENTS
									  //eShellFolderType_COMMON_FAVORITES
									  //eShellFolderType_COMMON_MUSIC
									  //eShellFolderType_COMMON_OEM_LINKS
									  //eShellFolderType_COMMON_PICTURES
									  //eShellFolderType_COMMON_PROGRAMS
									  //eShellFolderType_COMMON_STARTMENU
									  //eShellFolderType_COMMON_STARTUP
									  //eShellFolderType_COMMON_TEMPLATES
									  //eShellFolderType_COMMON_VIDEO
									  //eShellFolderType_COMPUTERSNEARME
									  //eShellFolderType_CONNECTIONS
									  //eShellFolderType_CONTROLS
									  //eShellFolderType_COOKIES
									  //eShellFolderType_DESKTOP
									  //eShellFolderType_DESKTOPDIRECTORY
									  //eShellFolderType_DRIVES
									  //eShellFolderType_FAVORITES
									  //eShellFolderType_FONTS
									  //eShellFolderType_HISTORY
									  //eShellFolderType_INTERNET
									  //eShellFolderType_INTERNET_CACHE
									  //eShellFolderType_LOCAL_APPDATA
									  //eShellFolderType_MYDOCUMENTS
									  //eShellFolderType_MYMUSIC
									  //eShellFolderType_MYPICTURES
									  //eShellFolderType_MYVIDEO
									  //eShellFolderType_NETHOOD
									  //eShellFolderType_NETWORK
									  //eShellFolderType_PERSONAL
									  //eShellFolderType_PRINTERS
									  //eShellFolderType_PRINTHOOD
									  //eShellFolderType_PROFILE
									  //eShellFolderType_PROGRAM_FILES
									  //eShellFolderType_PROGRAM_FILESX86
									  //eShellFolderType_PROGRAM_FILES_COMMON
									  //eShellFolderType_PROGRAM_FILES_COMMONX86
									  //eShellFolderType_PROGRAMS
									  //eShellFolderType_RECENT
									  //eShellFolderType_RESOURCES
									  //eShellFolderType_RESOURCES_LOCALIZED
									  //eShellFolderType_SENDTO
									  //eShellFolderType_STARTMENU
									  //eShellFolderType_STARTUP
									  //eShellFolderType_SYSTEM
									  //eShellFolderType_SYSTEMX86
									  //eShellFolderType_TEMPLATES
									  //eShellFolderType_WINDOWS
};

const std::string GetShellFolder(EShellFolderType what)
{
#ifndef _WIN32
	// TODO: handle other platforms (when needed)
#error "not implemented yet"
#endif
	std::string ret;
	char folder[MAX_PATH + 1] = {0};

	if (S_OK == ::SHGetFolderPathA(0, what, 0, /*SHGFP_TYPE_CURRENT*/0, folder))
		ret.assign(folder);

	return ret;
}

const std::string GetCommonAppData()
{
	return GetShellFolder(eShellFolderType_COMMON_APPDATA);
}

bool IsPortable()
{
	// NOTE: one could also use QCoreApplication::applicationFilePath()
#ifdef _WIN32
	char buf[MAX_PATH] = {0};
	unsigned ret = ::GetModuleFileNameA(NULL, buf, MAX_PATH);

	if (ret != 0)
	{
		const std::string compStr("-portable.exe");
		std::string s(buf);
		return 0 == s.compare(s.size() - compStr.size(), compStr.size(), compStr);
	}

#else
	// TODO: handle other platforms (when needed)
#error "not implemented yet"
#endif
	return false;
}

const std::string GetSettingsFilePath(const char* fileName)
{
	// (1) if portable, return fileName directly
	// (2) use file in common app data
	// (3) create file or error
	std::string filePath(fileName);

	if (!IsPortable())
	{
		filePath.assign(GetCommonAppData() + "\\Ipponboard\\");
		filePath.append(fileName);
#if 0

		// Unfortunately this did not link anymore with vc2010/Qt4.8.2/boost 1.50
		if (!boost::filesystem::exists(filePath))
#else
		// so we need to go plain Win32
#	ifndef _WIN32
		// TODO: handle other platforms (when needed)
#	error "not implemented yet"
#	endif
		const DWORD dwAttrib = ::GetFileAttributesA(filePath.c_str());

		if (dwAttrib == INVALID_FILE_ATTRIBUTES ||
				(dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
#endif
		{
			filePath.assign(fileName);
		}
	}

	return filePath;
}

} // anonymous namespace
} // namespace fmu

#endif  // UTIL__PATH_HELPERS_H_
