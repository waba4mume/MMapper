; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=MMapper
AppVerName=MMapper 2.0.0.0 beta
AppPublisher=Caligor & Alve
AppPublisherURL=http://sourceforge.net/projects/mmapper
AppSupportURL=http://sourceforge.net/projects/mmapper
AppUpdatesURL=http://sourceforge.net/projects/mmapper
DefaultDirName={pf}\MMapper2
DefaultGroupName=MMapper
AllowNoIcons=yes
LicenseFile=D:\SourceForge\mmapper2\COPYING
InfoBeforeFile=D:\SourceForge\mmapper2\INSTALL
InfoAfterFile=D:\SourceForge\mmapper2\doc\mapping_advice
OutputDir=D:\SourceForge
OutputBaseFilename=MMapper2_0_0_0_Beta
Compression=lzma
SolidCompression=yes

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: czech; MessagesFile: compiler:Languages\Czech.isl
Name: german; MessagesFile: compiler:Languages\German.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
Source: ..\..\bin\release\mmapper2.exe; DestDir: {app}; Flags: ignoreversion overwritereadonly; Components: app
Source: D:\QT\4.1.1\bin\QtCore4.dll; DestDir: {app}; Flags: ignoreversion overwritereadonly; Components: app
Source: D:\QT\4.1.1\bin\QtGui4.dll; DestDir: {app}; Flags: ignoreversion overwritereadonly; Components: app
Source: D:\QT\4.1.1\bin\QtNetwork4.dll; DestDir: {app}; Flags: ignoreversion overwritereadonly; Components: app
Source: D:\QT\4.1.1\bin\QtOpenGL4.dll; DestDir: {app}; Flags: ignoreversion overwritereadonly; Components: app
Source: C:\WINDOWS\WinSxS\x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.42_x-ww_0de06acd\msvcr80.dll; DestDir: {app}; Flags: ignoreversion overwritereadonly; Components: app
Source: C:\WINDOWS\WinSxS\x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.42_x-ww_0de06acd\msvcp80.dll; DestDir: {app}; Flags: ignoreversion overwritereadonly; Components: app
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: ..\..\bin\release\mmapper2.exe.manifest; DestDir: {app}; Flags: overwritereadonly ignoreversion; Components: app
Source: ..\..\doc\pathmachine; DestDir: {app}\Doc; Flags: overwritereadonly ignoreversion; Components: doc
Source: ..\..\doc\mapping_advice; DestDir: {app}\Doc; Flags: overwritereadonly ignoreversion; Components: doc
Source: ..\..\AUTHORS; DestDir: {app}; Flags: overwritereadonly ignoreversion; DestName: Authors.txt
Source: ..\..\COPYING; DestDir: {app}; Flags: overwritereadonly ignoreversion; DestName: Copying.txt
Source: ..\..\map\basemap.mm2; DestDir: {app}; Flags: uninsneveruninstall onlyifdoesntexist; Components: map

[Icons]
Name: {group}\MMapper; Filename: {app}\mmapper2.exe
Name: {group}\{cm:UninstallProgram,MMapper}; Filename: {uninstallexe}
Name: {userdesktop}\MMapper; Filename: {app}\mmapper2.exe; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\MMapper; Filename: {app}\mmapper2.exe; Tasks: quicklaunchicon

[Run]
Filename: {app}\mmapper2.exe; Description: {cm:LaunchProgram,MMapper}; Flags: nowait postinstall skipifsilent

[Dirs]
Name: {app}\Doc; Components: doc
[Components]
Name: app; Description: The mapper application; Flags: fixed; Types: custom compact full
Name: doc; Description: Advice, tips and tricks; Types: custom full
Name: map; Description: Basic map skeleton; Types: custom full