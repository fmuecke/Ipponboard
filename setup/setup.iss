; Copyright 2018 Florian Muecke. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE.txt file.

#include "ISPPBuiltins.iss"
#include "defines.def"

#define MyFullAppVersion GetVersionNumbersString('..\_bin\Ipponboard-Release\Ipponboard.exe')
#define MyAppVersion GetFileProductVersion('..\_bin\Ipponboard-Release\Ipponboard.exe')
;#define MyAppVersion GetEnv("IPPONBOARD_VERSION")
#if len(MyAppVersion) < 1
  #pragma warning "IPPONBOARD_VERSION not defined"
  #define MyAppVersion "0.0.0.0"
#endif

#define MySimpleAppVersion SimpleVersion(MyAppVersion)

#pragma message "*** Version info ***
#pragma message "Detailed version info: " + MyAppVersion
#pragma message "Simple version info:   " + MySimpleAppVersion

#define MyAppName "Ipponboard"
#define MyAppCopyright "2010-2024 Florian Mücke "
#define MyAppAuthor "Florian Mücke"


; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
#define MyAppID "{1CE3EDF1-161E-414B-ACFA-1FA01B6686A7}"

[Setup]
AppID={{#MyAppID}
AppName={#MyAppName}
AppVersion={#MySimpleAppVersion}
AppVerName={#MyAppName} {#MySimpleAppVersion}
AppPublisher={#MyAppAuthor}
AppPublisherURL=https://github.com/fmuecke/Ipponboard/
AppSupportURL=https://github.com/fmuecke/Ipponboard/issues
AppUpdatesURL=https://github.com/fmuecke/Ipponboard/releases
AppCopyright=Copyright (C) {#MyAppCopyright}
DefaultDirName={localappdata}\{#MyAppName}
DisableProgramGroupPage=yes
;OutputBaseFilename={#MyAppName}-{#MySimpleAppVersion}-Setup
OutputBaseFilename={#MyAppName}-{#MyAppVersion}-setup
Compression=lzma2/ultra64
SolidCompression=true
InternalCompressLevel=ultra64
;MinVersion=0,6.0.6001sp2 ;; not recommended anymore. 
ShowLanguageDialog=no
;WizardImageFile=compiler:wizmodernimage-IS.bmp
WizardImageFile=images\install_bg.bmp
WizardSmallImageFile=images\logo_small.bmp
WizardStyle=modern
VersionInfoVersion={#MyFullAppVersion}
VersionInfoProductName={#MyAppName}
VersionInfoProductVersion={#MyFullAppVersion}
VersionInfoProductTextVersion={#MyAppVersion}
VersionInfoCopyright={#MyAppCopyright}
VersionInfoCompany={#MyAppAuthor}
AlwaysShowDirOnReadyPage=true
AlwaysShowGroupOnReadyPage=true
UninstallDisplayIcon={app}\Ipponboard.exe
Uninstallable=True
PrivilegesRequired=lowest
DisableDirPage=yes
AllowUNCPath=False
DisableReadyPage=True
DisableReadyMemo=True
TimeStampsInUTC=True
UninstallFilesDir={localappdata}

[Languages]
;Name: "en"; MessagesFile: "compiler:Default.isl,compiler:MyMessages.isl"
Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "License_en.txt"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"; LicenseFile: "License_de.txt"

[CustomMessages]
de.ViewProgram=%1 öffnen
en.ViewProgram=Open %1
;de.Survey=Meinungsumfrage
;en.Survey=Feedback/Survey
de.UninstallKeepSettings=Möchten Sie die Programmeinstellungen für eine spätere Installation aufheben?
en.UninstallKeepSettings=Do you want to keep your settings for a later installation?
;en.VCmsg=Installing Microsoft Visual C++ Minimum Runtime...
;de.VCmsg=Installiere Microsoft Visual C++ Minimum Runtime...

[Tasks]
;Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "..\_bin\Ipponboard-Release\Ipponboard.exe"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\_bin\GamepadDemo-Release\GamepadDemo.exe"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\_bin\Ipponboard-Release\categories.json"; DestDir: "{app}"; Flags: promptifolder
Source: "..\_bin\Ipponboard-Release\clubs.json"; DestDir: "{app}"; Flags: promptifolder
Source: "..\_bin\Ipponboard-Release\Anleitung.html"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion; Languages: de
Source: "..\_bin\Ipponboard-Release\User-Manual.html"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion; Languages: en
Source: "..\_bin\Ipponboard-Release\CHANGELOG.html"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion
Source: "..\_bin\Ipponboard-Release\TournamentModes.ini"; DestDir: "{app}"; Flags: promptifolder
Source: "..\_bin\Ipponboard-Release\clubs\*.*"; DestDir: "{app}\clubs\"; Flags: ignoreversion promptifolder
Source: "..\_bin\Ipponboard-Release\sounds\*.*"; DestDir: "{app}\sounds\"; Flags: ignoreversion promptifolder
Source: "..\_bin\Ipponboard-Release\templates\*.*"; DestDir: "{app}\templates\"; Flags: ignoreversion promptifolder
Source: "..\_bin\Ipponboard-Release\lang\*.*"; DestDir: "{app}\lang\"; Flags: IgnoreVersion promptifolder
Source: "..\_bin\Ipponboard-Release\licenses\*.*"; DestDir: "{app}\licenses"; Flags: IgnoreVersion recursesubdirs
Source: "..\_bin\Ipponboard-Release\plugins\*.*"; DestDir: "{app}\plugins"; Flags: IgnoreVersion recursesubdirs
Source: "..\_bin\Ipponboard-Release\Qt5*.dll"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\_bin\Ipponboard-Release\concrt140.dll"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion
Source: "..\_bin\Ipponboard-Release\msvcp140*.dll"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion
;Source: "vcredist_x86.exe"; DestDir: {tmp}; Flags: deleteafterinstall

[Dirs]
;Name: {commonappdata}\Ipponboard; Permissions: users-full
Name: "{app}\sounds"
Name: "{app}\lang"
Name: "{app}\clubs"; Permissions: users-full
Name: "{app}\templates"; Permissions: users-full
Name: "{app}\licenses"

[Icons]
Name: "{userprograms}\{#MyAppName}\{#MyAppName}"; Filename: "{app}\Ipponboard.exe"; WorkingDir: "{app}"
Name: "{userprograms}\{#MyAppName}\{#MyAppName} Anleitung"; Filename: "{app}\Anleitung.html"; WorkingDir: "{app}"; Languages: de
Name: "{userprograms}\{#MyAppName}\{#MyAppName} User manual"; Filename: "{app}\User-Manual.html"; WorkingDir: "{app}"; Languages: en
Name: "{userprograms}\{#MyAppName}\{#MyAppName} Changelog"; Filename: "{app}\CHANGELOG.html"; WorkingDir: "{app}"
Name: "{userprograms}\{#MyAppName}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "https://github.com/fmuecke/Ipponboard"
Name: "{userprograms}\{#MyAppName}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\{#MyAppName}"; Filename: "{app}\Ipponboard.exe"; WorkingDir: "{app}"; IconFilename: "{app}\Ipponboard.exe"; IconIndex: 0

[Run]
Filename: {app}\Ipponboard.exe; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent unchecked; WorkingDir: {app}
;Filename: "{app}\Anleitung.pdf"; Description: {cm:ViewProgram,Anleitung}; Flags: shellexec nowait postinstall skipifsilent; WorkingDir: {app}; Languages: de
;Filename: "{app}\Manual.pdf"; Description: {cm:ViewProgram,manual}; Flags: shellexec nowait postinstall skipifsilent; WorkingDir: {app}; Languages: en
;Filename: "{tmp}\vcredist_x86.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "{#VCmsg}"; Check: VCRedistNeedsInstall; WorkingDir: {tmp};

[Registry]
Root: "HKCU"; Subkey: "Software\{#MyAppName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: deletekey
Root: "HKCU"; Subkey: "Software\{#MyAppName}"; ValueType: string; ValueName: "InstalledVersion"; ValueData: "{#MySimpleAppVersion}"; Flags: deletekey

[INI]
Filename: {app}\Ipponboard.ini; Section: Main; Key: Language; String: de; Languages: de;
Filename: {app}\Ipponboard.ini; Section: Main; Key: Language; String: en; Languages: en;

[UnInstallDelete]
Type: filesandordirs; Name: "{localappdata}\{#MyAppName}"
Type: files; Name: "{userdesktop}\{#MyAppName}.lnk"

[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
  if RegKeyExists(HKEY_CURRENT_USER, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppID}_is1') then
  begin
    case MsgBox('Ipponboard is alread installed.'#13#10'Do you want to continue and overwrite older files?', mbConfirmation, MB_YESNO) of 
      IDYES: ; // how to call the uninstall command -->  https://stackoverflow.com/questions/2000296/how-to-automatically-uninstall-previous-installed-version-in-inno-setup
      IDNO: Result := False; 
    end;
  end;
end;
