# Microsoft Developer Studio Project File - Name="Game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Game - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak" CFG="Game - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Game - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Game - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /Zd /O1 /I "..\Engine" /I "..\Engine\Common" /I "..\Engine\Debug" /I "..\Engine\Files" /I "..\Engine\Input" /I "..\Engine\Sound" /I "..\Engine\App" /I "..\Engine\Render" /I "..\Engine\Gs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"!Out\Release/Sab.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 engine.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib strmiids.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib msimg32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"..\Engine"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy !Out\Release\game.exe ..\..\Bin\game.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Engine" /I "..\Engine\Common" /I "..\Engine\Debug" /I "..\Engine\Files" /I "..\Engine\Input" /I "..\Engine\Sound" /I "..\Engine\App" /I "..\Engine\Render" /I "..\Engine\Gs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Fp"!Out\Debug/Sab.pch" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 engine_d.lib dxguid.lib d3d8.lib d3dx8.lib dinput8.lib dsound.lib strmiids.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib msimg32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\Engine"
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy !Out\Debug\game.exe ..\..\Bin\game_d.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Game - Win32 Release"
# Name "Game - Win32 Debug"
# Begin Source File

SOURCE=.\DizApp.cpp
# End Source File
# Begin Source File

SOURCE=.\DizApp.h
# End Source File
# Begin Source File

SOURCE=.\DizCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\DizCfg.h
# End Source File
# Begin Source File

SOURCE=.\DizDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\DizDebug.h
# End Source File
# Begin Source File

SOURCE=.\DizDef.h
# End Source File
# Begin Source File

SOURCE=.\DizGame.cpp
# End Source File
# Begin Source File

SOURCE=.\DizGame.h
# End Source File
# Begin Source File

SOURCE=.\DizMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DizMap.h
# End Source File
# Begin Source File

SOURCE=.\DizPaint.cpp
# End Source File
# Begin Source File

SOURCE=.\DizPaint.h
# End Source File
# Begin Source File

SOURCE=.\DizPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\DizPlayer.h
# End Source File
# Begin Source File

SOURCE=.\DizScript.cpp
# End Source File
# Begin Source File

SOURCE=.\DizScript.h
# End Source File
# Begin Source File

SOURCE=.\DizSound.cpp
# End Source File
# Begin Source File

SOURCE=.\DizSound.h
# End Source File
# Begin Source File

SOURCE=.\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\Game.ico
# End Source File
# Begin Source File

SOURCE=.\Game.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Game - Win32 Release"

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# ADD CPP /Yc

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Target
# End Project
