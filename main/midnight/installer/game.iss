; Inno Setup installer for all three games (replaces the per-game Visual Studio .vdproj projects).
;
;   iscc /DApp=midnight|revenge|citadel /DAppVersion=2.0.12 /DBuildDir=<Release folder> game.iss
;
; scripts/build_installer.ps1 wraps this and takes the version from ../VERSION.

#ifndef App
  #error Pass /DApp=midnight, revenge or citadel
#endif
#ifndef AppVersion
  #error Pass /DAppVersion=x.y.z
#endif
#ifndef BuildDir
  #error Pass /DBuildDir=<folder holding <app>.exe and Content>
#endif

#define Publisher "Chilli Hugger Software Ltd"

; AppId must never change once released: it is how later installers find and upgrade this one.
; UpgradeCode is the old Windows Installer (.vdproj) UpgradeCode, used to find and remove the
; MSI-installed version the first time someone upgrades.
#if App == "midnight"
  #define AppName "Lords of Midnight"
  #define ShortcutName "Lords of Midnight"
  #define AppId "{{9EB14222-2A32-417A-82F1-636C2270D7AD}"
  #define OldUpgradeCode "{50C10DF9-F125-49C5-948D-A546D0DB5861}"
  #define OutputName "lords_of_midnight"
#elif App == "revenge"
  #define AppName "Doomdark's Revenge"
  #define ShortcutName "Doomdark's Revenge"
  #define AppId "{{60F66945-1E75-4867-B90E-865CA8438CAE}"
  #define OldUpgradeCode "{775E658D-8EC9-46F1-A4DE-A554CDE21F51}"
  #define OutputName "doomdarks_revenge"
#elif App == "citadel"
  #define AppName "Lords of Midnight - The Citadel"
  #define ShortcutName "The Citadel"
  #define AppId "{{FA1AEA11-5B93-4FB7-926F-4D01706D9E5D}"
  #define OldUpgradeCode "{9A4A35D0-9D5A-46FC-8E6A-FCE01E7E9B59}"
  #define OutputName "the_citadel"
#else
  #error Unknown App
#endif

[Setup]
AppId={#AppId}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#Publisher}
AppPublisherURL=https://www.chillihugger.com/
; same location as the old MSI: [ProgramFilesFolder]\[Manufacturer]\[ProductName]
DefaultDirName={autopf}\{#Publisher}\{#AppName}
DefaultGroupName={#Publisher}
DisableProgramGroupPage=yes
PrivilegesRequired=admin
; the games are 32-bit
ArchitecturesInstallIn64BitMode=
SetupIconFile=..\proj.win32\{#App}\game.ico
UninstallDisplayIcon={app}\{#App}.exe
OutputDir=Output
OutputBaseFilename={#OutputName}_setup_{#AppVersion}
Compression=lzma2/ultra
SolidCompression=yes
WizardStyle=modern
CloseApplications=yes

[Files]
Source: "{#BuildDir}\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs ignoreversion; Excludes: "*.pdb,*.ilk,*.exp,*.lib"
; the Visual C++ runtime the games link against
Source: "..\build.win32\dlls\*.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#ShortcutName}"; Filename: "{app}\{#App}.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\{#ShortcutName}"; Filename: "{app}\{#App}.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; Flags: unchecked

[Run]
Filename: "{app}\{#App}.exe"; Description: "Launch {#AppName}"; WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent

[Code]
// Windows Installer keeps a product's UpgradeCode "packed" (see PackGuid) under
// Installer\UpgradeCodes; the value names beneath it are the packed ProductCodes of every
// installed product with that UpgradeCode.

function SwapPairs(S: String): String;
var
  I: Integer;
begin
  Result := '';
  for I := 0 to (Length(S) div 2) - 1 do
    Result := Result + S[2 * I + 2] + S[2 * I + 1];
end;

function Reverse(S: String): String;
var
  I: Integer;
begin
  Result := '';
  for I := Length(S) downto 1 do
    Result := Result + S[I];
end;

// {AABBCCDD-EEFF-GGHH-IIJJ-KKLLMMNNOOPP} -> DDCCBBAA FFEE HHGG JJII LLKKNNMMPPOO
function PackGuid(Guid: String): String;
var
  G: String;
begin
  G := Copy(Guid, 2, Length(Guid) - 2);
  Result := Reverse(Copy(G, 1, 8)) + Reverse(Copy(G, 10, 4)) + Reverse(Copy(G, 15, 4)) +
            SwapPairs(Copy(G, 20, 4)) + SwapPairs(Copy(G, 25, 12));
end;

function UnpackGuid(P: String): String;
begin
  Result := '{' + Reverse(Copy(P, 1, 8)) + '-' + Reverse(Copy(P, 9, 4)) + '-' +
            Reverse(Copy(P, 13, 4)) + '-' + SwapPairs(Copy(P, 17, 4)) + '-' +
            SwapPairs(Copy(P, 21, 12)) + '}';
end;

// Uninstalls any MSI-installed (pre-Inno) copy so the install below replaces it rather than
// sitting beside it. User data lives outside the install folder, so it is untouched.
procedure RemoveOldMsiVersions(RootKey: Integer);
var
  Names: TArrayOfString;
  I, ResultCode: Integer;
begin
  if RegGetValueNames(RootKey, 'SOFTWARE\Classes\Installer\UpgradeCodes\' + PackGuid('{#OldUpgradeCode}'), Names) then
    for I := 0 to GetArrayLength(Names) - 1 do
      Exec('msiexec.exe', '/x ' + UnpackGuid(Names[I]) + ' /qn /norestart', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssInstall then
  begin
    RemoveOldMsiVersions(HKLM64);
    if IsWin64 then
      RemoveOldMsiVersions(HKLM32);
  end;
end;
