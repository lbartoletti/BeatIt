#define Version Trim(FileRead(FileOpen("..\VERSION")))
#define ProjectName GetEnv('PROJECT_NAME')
#define ProductName GetEnv('PRODUCT_NAME')
#define Publisher GetEnv('COMPANY_NAME')
#define Year GetDateTimeString("yyyy","","")

; 'Types': What get displayed during the setup
[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

; Components are used inside the script and can be composed of a set of 'Types'
[Components]
Name: "standalone"; Description: "Standalone application"; Types: full custom
Name: "vst3"; Description: "VST3 plugin"; Types: full custom
Name: "lv2"; Description: "LV2 plugin"; Types: full custom

[Setup]
ArchitecturesInstallIn64BitMode=x64compatible
ArchitecturesAllowed=x64compatible
AppName={#ProductName}
OutputBaseFilename={#ProductName}-{#Version}-Windows
AppCopyright=Copyright (C) {#Year} {#Publisher}
AppPublisher={#Publisher}
AppVersion={#Version}
DefaultDirName="{commoncf64}\VST3\{#ProductName}.vst3"
DisableDirPage=yes
LicenseFile="EULA"
UninstallFilesDir="{commonappdata}\{#ProductName}\uninstall"

[UninstallDelete]
Type: filesandordirs; Name: "{commoncf64}\VST3\{#ProductName}Data"
Type: filesandordirs; Name: "{commoncf64}\LV2\{#ProductName}Data"

[Files]
; VST3 Plugin
Source: "..\Builds\{#ProjectName}_artefacts\Release\VST3\{#ProductName}.vst3\*"; DestDir: "{commoncf64}\VST3\{#ProductName}.vst3\"; Excludes: *.ilk; Flags: ignoreversion recursesubdirs; Components: vst3

; Standalone Application
Source: "..\Builds\{#ProjectName}_artefacts\Release\Standalone\{#ProductName}.exe"; DestDir: "{commonpf64}\{#Publisher}\{#ProductName}"; Flags: ignoreversion; Components: standalone

; LV2 Plugin
Source: "..\Builds\{#ProjectName}_artefacts\Release\LV2\{#ProductName}.lv2\*"; DestDir: "{commoncf64}\LV2\{#ProductName}.lv2\"; Excludes: *.ilk; Flags: ignoreversion recursesubdirs; Components: lv2

[Icons]
Name: "{autoprograms}\{#ProductName}"; Filename: "{commonpf64}\{#Publisher}\{#ProductName}\{#ProductName}.exe"; Components: standalone 
Name: "{autoprograms}\Uninstall {#ProductName}"; Filename: "{uninstallexe}"

[Run]
; Create symlink for VST3 data
Filename: "{cmd}"; \
    WorkingDir: "{commoncf64}\VST3"; \
    Parameters: "/C mklink /D ""{commoncf64}\VST3\{#ProductName}Data"" ""{commonappdata}\{#ProductName}"""; \
    Flags: runascurrentuser; Components: vst3

; Create symlink for LV2 data
Filename: "{cmd}"; \
    WorkingDir: "{commoncf64}\LV2"; \
    Parameters: "/C mklink /D ""{commoncf64}\LV2\{#ProductName}Data"" ""{commonappdata}\{#ProductName}"""; \
    Flags: runascurrentuser; Components: lv2