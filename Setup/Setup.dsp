# Microsoft Developer Studio Project File - Name="Setup" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Setup - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Setup.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Setup.mak" CFG="Setup - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Setup - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Setup - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Setup - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "!Out\Release"
# PROP Intermediate_Dir "!Out\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "." /I "..\Engine" /I "..\Engine\Common" /I "..\Engine\Debug" /I "..\Engine\Files" /I "..\Engine\Math" /I "..\Engine\Gs" /I "..\Engine\Input" /I "..\Engine\Render" /I "..\Engine\Sound" /I "..\Engine\App" /I "..\Engine\Wui" /I "..\Libs\zlib" /I "..\Libs\lpng" /I "..\Libs\ljpeg" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 engine.lib dxguid.lib d3d8.lib d3dx8.lib dinput8.lib strmiids.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib msimg32.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\Engine"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy !Out\Release\setup.exe ..\..\Bin\setup.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Setup - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "!Out\Debug"
# PROP Intermediate_Dir "!Out\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\Engine" /I "..\Engine\Common" /I "..\Engine\Debug" /I "..\Engine\Files" /I "..\Engine\Math" /I "..\Engine\Gs" /I "..\Engine\Input" /I "..\Engine\Render" /I "..\Engine\Sound" /I "..\Engine\App" /I "..\Engine\Wui" /I "..\Libs\zlib" /I "..\Libs\lpng" /I "..\Libs\ljpeg" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 engine_d.lib dxguid.lib d3d8.lib d3dx8.lib dinput8.lib strmiids.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib msimg32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\Engine"
# SUBTRACT LINK32 /incremental:no
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy !Out\Debug\setup.exe ..\..\Bin\setup_d.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Setup - Win32 Release"
# Name "Setup - Win32 Debug"
# Begin Group "Res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Res\background.png
# End Source File
# Begin Source File

SOURCE=.\Res\browse.png
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_finger.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor_hand.cur
# End Source File
# Begin Source File

SOURCE=.\Res\dizzyage.png
# End Source File
# Begin Source File

SOURCE=.\Res\icon.ico
# End Source File
# Begin Source File

SOURCE=.\Res\icon_advanced.png
# End Source File
# Begin Source File

SOURCE=.\Res\icon_audio.png
# End Source File
# Begin Source File

SOURCE=.\Res\icon_disk.png
# End Source File
# Begin Source File

SOURCE=.\Res\icon_input.png
# End Source File
# Begin Source File

SOURCE=.\Res\icon_pc.png
# End Source File
# Begin Source File

SOURCE=.\Res\icon_video.png
# End Source File
# Begin Source File

SOURCE=.\Res\setup.gs
# End Source File
# Begin Source File

SOURCE=.\Res\user.gs
# End Source File
# End Group
# Begin Source File

SOURCE=.\replicators.cpp
# End Source File
# Begin Source File

SOURCE=.\replicators.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Setup.cpp
# End Source File
# Begin Source File

SOURCE=.\Res\setup.ico
# End Source File
# Begin Source File

SOURCE=.\Setup.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WUIApp.cpp
# End Source File
# Begin Source File

SOURCE=.\WUIApp.h
# End Source File
# End Target
# End Project
