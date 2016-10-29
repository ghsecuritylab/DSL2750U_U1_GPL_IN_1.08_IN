# Auto-generated by EclipseNSIS Script Wizard

Name "DSL CPE API Driver V${VERSION_DRV}${PATCH_DRV} for ${DEVICE_LONG}"
SetCompressor /SOLID lzma

# Defines
!define REGKEY "SOFTWARE\$(^Name)"
!define FILENAME_DRV drv_dsl_cpe_api_${DEVICE_FILE}-${VERSION_DRV}${PATCH_DRV}.tar.gz
!define FILENAME_CTRL dsl_cpe_control_${DEVICE_FILE}-${VERSION_CTRL}${PATCH_CTRL}.tar.gz
!define COMPANY "Infineon Technologies"
!define URL "http://www.infineon.com/dsl"
!define PUBLISHER "www.infineon.com"

# MUI defines
!define MUI_WELCOMEFINISHPAGE_BITMAP "win_inst_ifx.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "win_uninst_ifx.bmp"
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\box-install.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\$(^Name)"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULT_FOLDER "$(^Name)"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\box-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI.nsh

# Reserved Files

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE License.rtf
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages

# Please copy the file EnglishIfx.nsh to the directory
# "C:\Programme\NSIS\Contrib\Modern UI\Language files\"
# if you get errors here!
!insertmacro MUI_LANGUAGE EnglishIfx

# Installer attributes
OutFile setup_dsl_cpe_api_${DEVICE_FILE}-${VERSION_DRV}${PATCH_DRV}.exe
InstallDir "$PROGRAMFILES\Infineon\$(^Name)"
CRCCheck on
XPStyle on
ShowInstDetails show

BrandingText "${PUBLISHER}"

VIProductVersion ${VERSION_DRV}.0
VIAddVersionKey ProductName "DSL CPE API Driver for ${DEVICE_LONG}"
VIAddVersionKey ProductVersion "${VERSION_DRV}${PATCH_DRV}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION_DRV}${PATCH_DRV}"
VIAddVersionKey FileDescription "DSL CPE API Driver for ${DEVICE_LONG}"
VIAddVersionKey LegalCopyright "Copyright @ 2006 ${PUBLISHER}"
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show

# Installer sections
Section "!Source Code" SEC0000
    SetOutPath $INSTDIR
    SetOverwrite on
    File ..\build_${DEVICE_FILE}\${FILENAME_DRV}
    File ..\..\application\dsl_cpe_control\build_${DEVICE_FILE}\${FILENAME_CTRL}
    File License.pdf
    File ..\doc\drv_dsl_cpe_api_${DEVICE_FILE}_${VERSION_DRV}${PATCH_DRV}.chm
    WriteRegStr HKLM "${REGKEY}\Components" "Source Code" 1
SectionEnd

Section -post SEC0001
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortCut "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION_DRV}${PATCH_DRV}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    Goto done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o "un.Source Code" UNSEC0000
    Delete /REBOOTOK $INSTDIR\License.pdf
    Delete /REBOOTOK $INSTDIR\${FILENAME_DRV}
    Delete /REBOOTOK $INSTDIR\${FILENAME_CTRL}
    Delete /REBOOTOK $INSTDIR\drv_dsl_cpe_api_${DEVICE_FILE}_${VERSION_DRV}${PATCH_DRV}.chm
    DeleteRegValue HKLM "${REGKEY}\Components" "Source Code"
SectionEnd

Section un.post UNSEC0001
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /ifempty HKLM "${REGKEY}\Components"
    DeleteRegKey /ifempty HKLM "${REGKEY}"
    RMDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RMDir /REBOOTOK $INSTDIR
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    ReadRegStr $StartMenuGroup HKLM "${REGKEY}" StartMenuGroup
    !insertmacro SELECT_UNSECTION "Source Code" ${UNSEC0000}
FunctionEnd
