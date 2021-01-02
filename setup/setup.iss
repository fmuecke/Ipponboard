; Copyright 2018 Florian Muecke. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE.txt file.

#include "ISPPBuiltins.iss"
#include "defines.def"

#define MyFullAppVersion GetFileVersion('..\bin\Ipponboard.exe')
#define MyAppVersion GetFileProductVersion('..\bin\Ipponboard.exe')
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
#define MyAppCopyright "2010-2017 Florian Mücke"
#define MyAppAuthor "Florian Mücke"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppID={{1CE3EDF1-161E-414B-ACFA-1FA01B6686A7}
AppName={#MyAppName}
AppVersion={#MySimpleAppVersion}
AppVerName={#MyAppName} {#MySimpleAppVersion}
AppPublisher={#MyAppAuthor}
AppPublisherURL=https://ipponboard.koe-judo.de/
AppSupportURL=https://github.com/fmuecke/Ipponboard/issues
AppUpdatesURL=https://ipponboard.koe-judo.de/
AppCopyright=Copyright (C) {#MyAppCopyright}
DefaultDirName={localappdata}\{#MyAppName}
DisableProgramGroupPage=yes
;OutputBaseFilename={#MyAppName}-{#MySimpleAppVersion}-Setup
OutputBaseFilename={#MyAppName}-{#MyAppVersion}-setup
Compression=lzma2/ultra64
SolidCompression=true
InternalCompressLevel=ultra64
MinVersion=0,6.0.6001sp2
ShowLanguageDialog=no
;WizardImageFile=compiler:wizmodernimage-IS.bmp
WizardImageFile=images\install_bg.bmp
WizardSmallImageFile=images\logo_small.bmp
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
Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "License_en.rtf"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"; LicenseFile: "License_de.rtf"

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
Source: "..\bin\Ipponboard.exe"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\bin\GamepadDemo.exe"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\bin\categories.json"; DestDir: "{app}"; Flags: promptifolder
Source: "..\bin\clubs.json"; DestDir: "{app}"; Flags: promptifolder
Source: "..\bin\Anleitung.html"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion
Source: "..\bin\CHANGELOG.html"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion
Source: "..\bin\TournamentModes.ini"; DestDir: "{app}"; Flags: promptifolder
Source: "..\bin\Clubs\*.*"; DestDir: "{app}\Clubs\"; Flags: ignoreversion promptifolder
Source: "..\bin\Sounds\*.*"; DestDir: "{app}\Sounds\"; Flags: ignoreversion promptifolder
Source: "..\bin\Templates\*.*"; DestDir: "{app}\Templates\"; Flags: ignoreversion promptifolder
Source: "..\bin\Lang\*.*"; DestDir: "{app}\lang\"; Flags: IgnoreVersion promptifolder
Source: "..\bin\Licenses\*.*"; DestDir: "{app}\Licenses"; Flags: IgnoreVersion recursesubdirs
;Source: ..\bin\msvcp100.dll; DestDir: {app}; Flags: IgnoreVersion promptifolder;
;Source: ..\bin\msvcr100.dll; DestDir: {app}; Flags: IgnoreVersion promptifolder;
Source: "..\bin\QtCore4.dll"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\bin\QtGui4.dll"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\bin\QtNetwork4.dll"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\bin\QtXmlPatterns4.dll"; DestDir: "{app}"; Flags: IgnoreVersion promptifolder
Source: "..\bin\vcruntime140.dll"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion
Source: "..\bin\msvcp140.dll"; DestDir: "{app}"; Flags: IgnoreVersion replacesameversion
;Source: "vcredist_x86.exe"; DestDir: {tmp}; Flags: deleteafterinstall

[Dirs]
;Name: {commonappdata}\Ipponboard; Permissions: users-full
Name: "{app}\Sounds"
Name: "{app}\Lang"
Name: "{app}\Clubs"; Permissions: users-full
Name: "{app}\Templates"; Permissions: users-full
Name: "{app}\Licenses"

[Icons]
Name: "{userprograms}\{#MyAppName}\{#MyAppName}"; Filename: "{app}\Ipponboard.exe"; WorkingDir: "{app}"
Name: "{userprograms}\{#MyAppName}\{#MyAppName} Anleitung"; Filename: "{app}\Anleitung.html"; WorkingDir: "{app}"
Name: "{userprograms}\{#MyAppName}\{#MyAppName} Changelog"; Filename: "{app}\CHANGELOG.html"; WorkingDir: "{app}"
;Name: "{group}\Manual"; Filename: {app}\manual.pdf; WorkingDir: {app}; Languages: en
Name: "{userprograms}\{#MyAppName}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "https://ipponboard.koe-judo.de/"
Name: "{userprograms}\{#MyAppName}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
;Name: "{userstartmenu}\Umfrage"; Filename: "http://flo.mueckeimnetz.de/ipponboard/survey_de/"; Languages: de
;Name: "{userstartmenu}\Online-Survey (Feedback)"; Filename: "http://flo.mueckeimnetz.de/ipponboard/survey_en/"; Languages: en
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
// function ShouldSkipPage(PageID: Integer): Boolean;
// begin
//   { Skip pages that shouldn't be shown }
//   if (PageID = wpSelectProgramGroup) then 
//   begin
//     WizardForm.NoIconsCheck.Checked := False;
//     Result := False;
//   end else
//   begin
//     Result := False;
//   end
// end;

// function UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo,
//   MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: String): String;
// var
//   S: String;
// begin
//   { Fill the 'Ready Memo' with the normal settings and the custom settings }
//   S := '';
//   S := S + ExpandConstant('{cm:InstallType_InfoTitle}') + ':' + NewLine + Space;
//   S := S + NewLine;
//   S := S + NewLine + MemoDirInfo + NewLine;
//   S := S + NewLine + MemoGroupInfo + NewLine;
//   S := S + NewLine + MemoTasksInfo + NewLine;
//   Result := S;
// end;

// procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
// begin
//   case CurUninstallStep of
//     usUninstall:
//       begin
//         //if (MsgBox(ExpandConstant('{cm:UninstallKeepSettings}'), mbConfirmation, mb_YesNo) = idNo) then
//         begin
//           //DelTree(ExpandConstant('{localappdata}\{#MyAppName}'),True,True,True);
//           DelTree(ExpandConstant('{app}'),True,True,True);
//           DeleteFile(ExpandConstant('{userdesktop}\{#MyAppName}.lnk'));
//         end;
//       end;
//     usPostUninstall:
//       begin
//         // ...insert code to perform pre-uninstall tasks here...
//       end;
//   end;
// end;

function GetFileVersion(filename : String): String;
begin
    GetVersionNumbersString(filename, Result);
end;

#IFDEF UNICODE
  #DEFINE AW "W"
#ELSE
  #DEFINE AW "A"
#ENDIF
type
  INSTALLSTATE = Longint;
const
  INSTALLSTATE_INVALIDARG = -2;  { An invalid parameter was passed to the function. }
  INSTALLSTATE_UNKNOWN = -1;     { The product is neither advertised or installed. }
  INSTALLSTATE_ADVERTISED = 1;   { The product is advertised but not installed. }
  INSTALLSTATE_ABSENT = 2;       { The product is installed for a different user. }
  INSTALLSTATE_DEFAULT = 5;      { The product is installed for the current user. }

  { Visual C++ 2015 Redistributable 14.0.23026 }
  VC_2015_REDIST_X86_MIN = '{A2563E55-3BEC-3828-8D67-E5E8B9E8B675}';
  VC_2015_REDIST_X64_MIN = '{0D3E9E15-DE7A-300B-96F1-B4AF12B96488}';

  VC_2015_REDIST_X86_ADD = '{BE960C1C-7BAD-3DE6-8B1A-2616FE532845}';
  VC_2015_REDIST_X64_ADD = '{BC958BD2-5DAC-3862-BB1A-C1BE0790438D}';

  { Visual C++ 2015 Redistributable 14.0.24210 }
  VC_2015_REDIST_X86 = '{8FD71E98-EE44-3844-9DAD-9CB0BBBC603C}';
  VC_2015_U3_REDIST_X86 = '{37B55901-995A-3650-80B1-BBFD047E2911}'; { 14.0.24212 }
  VC_2015_REDIST_X64 = '{C0B2C673-ECAA-372D-94E5-E89440D087AD}';

function MsiQueryProductState(szProduct: string): INSTALLSTATE; 
  external 'MsiQueryProductState{#AW}@msi.dll stdcall';

function VCVersionInstalled(const ProductID: string): Boolean;
begin
  Result := MsiQueryProductState(ProductID) = INSTALLSTATE_DEFAULT;
end;

function VCRedistNeedsInstall: Boolean;
begin
  { here the Result must be True when you need to install your VCRedist }
  { or False when you don't need to, so now it's upon you how you build }
  { this statement, the following won't install your VC redist only when }
  { the Visual C++ 2010 Redist (x86) and Visual C++ 2010 SP1 Redist(x86) }
  { are installed for the current user }
  Result := not (VCVersionInstalled(VC_2015_REDIST_X86) and 
    VCVersionInstalled(VC_2015_U3_REDIST_X86));
end;