#ifndef HELPERS_H
#define HELPERS_H

#ifdef _WIN32
#	include <Windows.h>
#	include <Shlobj.h>
#	pragma comment(lib,"Shell32.lib")
#endif

#include <string>
#include <boost/filesystem.hpp>

namespace FMU
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

static const std::string GetShellFolder(EShellFolderType what)
{
	std::string ret;
#ifdef _WIN32
	char folder[MAX_PATH+1] = {0};
	if( S_OK == ::SHGetFolderPathA(0, what, 0, SHGFP_TYPE_CURRENT, folder ) )
		ret.assign(folder);
#else
	// TODO: handle other platforms (when needed)
	#error "not implemented yet"
#endif
	return ret;
}
static const std::string GetCommonAppData()
{
	return GetShellFolder(eShellFolderType_COMMON_APPDATA);
}

static bool IsPortable()
{
	// NOTE: one could also use QCoreApplication::applicationFilePath()
#ifdef _WIN32
	char buf[MAX_PATH] = {0};
	unsigned ret = ::GetModuleFileNameA( NULL, buf, MAX_PATH );
	if( ret != 0 )
	{
		const std::string comp("-portable.exe");
		std::string s(buf);
		return 0 == s.compare(s.size()-comp.size(), comp.size(), comp);
	}
#else
	// TODO: handle other platforms (when needed)
	#error "not implemented yet"
	return false;
#endif
}

static const std::string GetSettingsFilePath(const char* fileName)
{
	// (1) if portable, return fileName directly
	// (2) use file in common app data
	// (3) create file or error
	std::string filePath(fileName);
	if( !IsPortable() )
	{
		filePath.assign(GetCommonAppData()+"\\Ipponboard\\");
		filePath.append(fileName);
		if( !boost::filesystem::exists(filePath) )
			filePath.assign(fileName);
	}
	return filePath;
}


} // namespace

#endif // HELPERS_H

