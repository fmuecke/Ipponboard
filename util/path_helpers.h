// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef UTIL__PATH_HELPERS_H_
#define UTIL__PATH_HELPERS_H_

#ifdef _WIN32
#	include <Windows.h>
#	include <Shlobj.h>
//#	pragma comment(lib,"Shell32.lib")
#endif

#include <string>
#include <filesystem>

namespace fm
{

namespace
{

#ifdef _WIN32

struct KnownFolders
{
	// reference: https://learn.microsoft.com/en-us/windows/win32/shell/knownfolderid

	static const std::string get_AppDataDesktop() { return get_folder(FOLDERID_AppDataDesktop); }
	static const std::string get_AppDataDocuments() { return get_folder(FOLDERID_AppDataDocuments); }
	static const std::string get_AppDataProgramData() { return get_folder(FOLDERID_AppDataProgramData); }
	static const std::string get_ApplicationShortcuts() { return get_folder(FOLDERID_ApplicationShortcuts); }
	static const std::string get_Desktop() { return get_folder(FOLDERID_Desktop); }
	static const std::string get_Documents() { return get_folder(FOLDERID_Documents); }
	static const std::string get_DocumentsLibrary() { return get_folder(FOLDERID_DocumentsLibrary); }
	static const std::string get_Downloads() { return get_folder(FOLDERID_Downloads); }
	static const std::string get_LocalAppData() { return get_folder(FOLDERID_LocalAppData); }
	static const std::string get_LocalAppDataLow() { return get_folder(FOLDERID_LocalAppDataLow); }
	static const std::string get_Profile() { return get_folder(FOLDERID_Profile); }
	static const std::string get_ProgramData() { return get_folder(FOLDERID_ProgramData); }
	static const std::string get_ProgramFiles() { return get_folder(FOLDERID_ProgramFiles); }
#ifdef _WIN64
	static const std::string get_ProgramFilesX64() { return get_folder(FOLDERID_ProgramFilesX64); }
#endif
	static const std::string get_ProgramFilesX86() { return get_folder(FOLDERID_ProgramFilesX86); }
	static const std::string get_RoamingAppData() { return get_folder(FOLDERID_RoamingAppData); }
	static const std::string get_ProgramFilesCommon() { return get_folder(FOLDERID_ProgramFilesCommon); }

private:
	static std::string get_folder(GUID folderId)
	{
		PWSTR folder{ nullptr };
		std::string result;
		if (SUCCEEDED(::SHGetKnownFolderPath(folderId, 0, nullptr, &folder)))
		{
			result = std::filesystem::path(folder).string();
		}
		::CoTaskMemFree(folder);
		return result;
	}
};
#endif

const std::string GetSettingsFilePath(const char* fileName)
{
#ifdef _WIN32
	// (1) use file in common app data
	// (2) create file or error

	std::filesystem::path basePath{ KnownFolders::get_LocalAppData() };
	if (!std::filesystem::exists(basePath))
	{
		return fileName;
	}

	auto configPath = basePath.append("Ipponboard");
	if (!std::filesystem::exists(configPath) && !std::filesystem::create_directory(configPath))
	{
		return fileName;
	}

	return configPath.append(fileName).string();

#else
    //TODO: implement proper file path handling for linux
    return fileName;
#endif
}

} // anonymous namespace
} // namespace fmu

#endif  // UTIL__PATH_HELPERS_H_
