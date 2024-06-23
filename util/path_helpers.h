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

#include <QFile>
#include <QDir>
#include <string>

namespace fm
{

namespace
{

#ifdef _WIN32

struct KnownFolders
{
	// reference: https://learn.microsoft.com/en-us/windows/win32/shell/knownfolderid

	static QString get_AppDataDesktop() { return get_folder(FOLDERID_AppDataDesktop); }
	static QString get_AppDataDocuments() { return get_folder(FOLDERID_AppDataDocuments); }
	static QString get_AppDataProgramData() { return get_folder(FOLDERID_AppDataProgramData); }
	static QString get_ApplicationShortcuts() { return get_folder(FOLDERID_ApplicationShortcuts); }
	static QString get_Desktop() { return get_folder(FOLDERID_Desktop); }
	static QString get_Documents() { return get_folder(FOLDERID_Documents); }
	static QString get_DocumentsLibrary() { return get_folder(FOLDERID_DocumentsLibrary); }
	static QString get_Downloads() { return get_folder(FOLDERID_Downloads); }
	static QString get_LocalAppData() { return get_folder(FOLDERID_LocalAppData); }
	static QString get_LocalAppDataLow() { return get_folder(FOLDERID_LocalAppDataLow); }
	static QString get_Profile() { return get_folder(FOLDERID_Profile); }
	static QString get_ProgramData() { return get_folder(FOLDERID_ProgramData); }
	static QString get_ProgramFiles() { return get_folder(FOLDERID_ProgramFiles); }
#ifdef _WIN64
static QString get_ProgramFilesX64() { return get_folder(FOLDERID_ProgramFilesX64); }
#endif
	static QString get_ProgramFilesX86() { return get_folder(FOLDERID_ProgramFilesX86); }
	static QString get_RoamingAppData() { return get_folder(FOLDERID_RoamingAppData); }
	static QString get_ProgramFilesCommon() { return get_folder(FOLDERID_ProgramFilesCommon); }

private:
	static QString get_folder(GUID folderId)
	{
		PWSTR folder{ nullptr };
		QString result;
		if (SUCCEEDED(::SHGetKnownFolderPath(folderId, 0, nullptr, &folder)))
		{
			result = QDir::toNativeSeparators(QString::fromWCharArray(folder));
		}
		::CoTaskMemFree(folder);
		return result;
	}
};
#endif

const QString GetSettingsFilePath(const char* fileName)
{
#ifdef _WIN32
	// (1) use file in common app data
	// (2) create file or error

	QString basePath = KnownFolders::get_LocalAppData();
	QDir baseDir(basePath);
	if (!baseDir.exists())
	{
		return fileName;
	}

	QString configPath = baseDir.filePath("Ipponboard");
	if (!QDir(configPath).exists() && !baseDir.mkpath(configPath))
	{
		return fileName;
	}

	return QDir::toNativeSeparators(QDir(configPath).filePath(fileName));
#else
	//TODO: implement proper file path handling for linux
	return fileName;
#endif
}

} // anonymous namespace
} // namespace fmu

#endif  // UTIL__PATH_HELPERS_H_
