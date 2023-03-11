# Microsoft Developer Studio Project File - Name="Editor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Editor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Editor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Editor.mak" CFG="Editor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Editor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Editor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Editor - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O1 /I "." /I "..\Engine" /I "..\Engine\Common" /I "..\Engine\Debug" /I "..\Engine\Files" /I "..\Engine\Math" /I "..\Engine\Gs" /I "..\Engine\Input" /I "..\Engine\Render" /I "..\Engine\Sound" /I "..\Engine\App" /I "..\Libs\zlib" /I "..\Libs\lpng" /I "..\Libs\ljpeg" /I "..\Libs\fmod" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"!Out\Release/Sab.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo"!Out\Release/Game.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 engine.lib dxguid.lib d3d8.lib d3dx8.lib dinput8.lib strmiids.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib msimg32.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\Engine"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy !Out\Release\editor.exe ..\..\Bin\editor.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\Engine" /I "..\Engine\Common" /I "..\Engine\Debug" /I "..\Engine\Files" /I "..\Engine\Math" /I "..\Engine\Gs" /I "..\Engine\Input" /I "..\Engine\Render" /I "..\Engine\Sound" /I "..\Engine\App" /I "..\Libs\zlib" /I "..\Libs\lpng" /I "..\Libs\ljpeg" /I "..\Libs\fmod" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Fp"!Out\Debug/Sab.pch" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"!Out\Debug/Game.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 engine_d.lib dxguid.lib d3d8.lib d3dx8.lib dinput8.lib strmiids.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib msimg32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\Engine"
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy !Out\Debug\editor.exe ..\..\Bin\editor_d.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Editor - Win32 Release"
# Name "Editor - Win32 Debug"
# Begin Source File

SOURCE=.\EdiApp.cpp
# End Source File
# Begin Source File

SOURCE=.\EdiApp.h
# End Source File
# Begin Source File

SOURCE=.\EdiDef.h
# End Source File
# Begin Source File

SOURCE=.\EdiMap.cpp
# End Source File
# Begin Source File

SOURCE=.\EdiMap.h
# End Source File
# Begin Source File

SOURCE=.\EdiPaint.cpp
# End Source File
# Begin Source File

SOURCE=.\EdiPaint.h
# End Source File
# Begin Source File

SOURCE=.\EdiTool.cpp
# End Source File
# Begin Source File

SOURCE=.\EdiTool.h
# End Source File
# Begin Source File

SOURCE=.\Editor.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor.ico
# End Source File
# Begin Source File

SOURCE=.\Editor.rc
# End Source File
# Begin Source File

SOURCE=.\GUI.cpp
# End Source File
# Begin Source File

SOURCE=.\GUI.h
# End Source File
# Begin Source File

SOURCE=.\GUIButton.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIButton.h
# End Source File
# Begin Source File

SOURCE=.\GUIColor.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIColor.h
# End Source File
# Begin Source File

SOURCE=.\GUIDef.h
# End Source File
# Begin Source File

SOURCE=.\GUIDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIDlg.h
# End Source File
# Begin Source File

SOURCE=.\GUIEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIEdit.h
# End Source File
# Begin Source File

SOURCE=.\GUIItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIItem.h
# End Source File
# Begin Source File

SOURCE=.\GUITile.cpp
# End Source File
# Begin Source File

SOURCE=.\GUITile.h
# End Source File
# Begin Source File

SOURCE=.\GUIUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIUtil.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Editor - Win32 Release"

!ELSEIF  "$(CFG)" == "Editor - Win32 Debug"

# ADD CPP /Yc

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Target
# End Project
