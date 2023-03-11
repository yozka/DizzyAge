# Microsoft Developer Studio Project File - Name="Engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak" CFG="Engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Engine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Engine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "!Out\Release"
# PROP Intermediate_Dir "!Out\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /Op /I ".\\" /I "Common" /I "Debug" /I "Files" /I "Sound" /I "Render" /I "..\Libs\zlib" /I "..\Libs\lpng" /I "..\Libs\ljpeg" /I "..\Libs\dumb\include" /I "..\Libs\ogg\include" /I "..\Libs\vorbis\include" /I "..\Libs\stsound" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy !Out\Release\engine.lib engine.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Engine___Win32_Debug"
# PROP BASE Intermediate_Dir "Engine___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "!Out\Debug"
# PROP Intermediate_Dir "!Out\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "Common" /I "Debug" /I "Files" /I "Sound" /I "Render" /I "..\Libs\zlib" /I "..\Libs\lpng" /I "..\Libs\ljpeg" /I "..\Libs\dumb\include" /I "..\Libs\ogg\include" /I "..\Libs\vorbis\include" /I "..\Libs\stsound" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy !Out\Debug\engine.lib engine_d.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Engine - Win32 Release"
# Name "Engine - Win32 Debug"
# Begin Group "App"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\App\E9App.cpp
# End Source File
# Begin Source File

SOURCE=.\App\E9App.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Common\E9Array.h
# End Source File
# Begin Source File

SOURCE=.\Common\E9Config.h
# End Source File
# Begin Source File

SOURCE=.\Common\E9Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\E9Engine.h
# End Source File
# Begin Source File

SOURCE=.\Common\E9Hash.h
# End Source File
# Begin Source File

SOURCE=.\Common\E9List.h
# End Source File
# Begin Source File

SOURCE=.\Common\E9Math.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\E9Math.h
# End Source File
# Begin Source File

SOURCE=.\Common\E9String.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\E9String.h
# End Source File
# Begin Source File

SOURCE=.\Common\E9System.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\E9System.h
# End Source File
# End Group
# Begin Group "Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Debug\D9Breakpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Breakpoint.h
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Debug.h
# End Source File
# Begin Source File

SOURCE=.\Debug\D9DH.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug\D9DH.h
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Guard.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Guard.h
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Log.h
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Memory.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Memory.h
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Shutdown.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug\D9Shutdown.h
# End Source File
# End Group
# Begin Group "Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Files\F9Archive.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9Archive.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9ArchivePak.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9ArchivePak.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9ArchiveZip.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9ArchiveZip.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9File.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9File.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9FileDisk.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9FileDisk.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9FileMem.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9FileMem.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9FilePak.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9FilePak.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9FilePakZ.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9FilePakZ.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9FileRes.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9FileRes.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9Files.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9Files.h
# End Source File
# Begin Source File

SOURCE=.\Files\F9FileZip.cpp
# End Source File
# Begin Source File

SOURCE=.\Files\F9FileZip.h
# End Source File
# End Group
# Begin Group "Gs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Gs\gs.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gs.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gsasm.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gsasm.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gscfg.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gscfg.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gscompiler.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gscompiler.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gsenv.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gsenv.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gserror.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gserror.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gslexer.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gslexer.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gslexerdef.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gslib_default.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gslib_default.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gslib_file.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gslib_file.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gslib_math.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gslib_math.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gslib_win.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gslib_win.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gsnode.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gsnode.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gsparser.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gsparser.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gsutil.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gsutil.h
# End Source File
# Begin Source File

SOURCE=.\Gs\gsvm.cpp
# End Source File
# Begin Source File

SOURCE=.\Gs\gsvm.h
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Input\I9Def.h
# End Source File
# Begin Source File

SOURCE=.\Input\I9DeviceDX.cpp
# End Source File
# Begin Source File

SOURCE=.\Input\I9DeviceDX.h
# End Source File
# Begin Source File

SOURCE=.\Input\I9FFDX.cpp
# End Source File
# Begin Source File

SOURCE=.\Input\I9FFDX.h
# End Source File
# Begin Source File

SOURCE=.\Input\I9Input.cpp
# End Source File
# Begin Source File

SOURCE=.\Input\I9Input.h
# End Source File
# Begin Source File

SOURCE=.\Input\I9InputDX.cpp
# End Source File
# Begin Source File

SOURCE=.\Input\I9InputDX.h
# End Source File
# End Group
# Begin Group "Render"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Render\R9Font.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\R9Font.h
# End Source File
# Begin Source File

SOURCE=.\Render\R9Img.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\R9Img.h
# End Source File
# Begin Source File

SOURCE=.\Render\R9ImgLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\R9ImgLoader.h
# End Source File
# Begin Source File

SOURCE=.\Render\R9Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\R9Render.h
# End Source File
# Begin Source File

SOURCE=.\Render\R9RenderDX.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\R9RenderDX.h
# End Source File
# Begin Source File

SOURCE=.\Render\R9RenderGL.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\R9RenderGL.h
# End Source File
# Begin Source File

SOURCE=.\Render\R9Resources.h
# End Source File
# Begin Source File

SOURCE=.\Render\R9TexturePool.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\R9TexturePool.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sound\A9Audio.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\A9Audio.h
# End Source File
# Begin Source File

SOURCE=.\Sound\A9AudioDX.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\A9AudioDX.h
# End Source File
# Begin Source File

SOURCE=.\Sound\A9Codec.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\a9codec.h
# End Source File
# Begin Source File

SOURCE=.\Sound\A9Codec_dumb.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\a9codec_dumb.h
# End Source File
# Begin Source File

SOURCE=.\Sound\A9Codec_ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\a9codec_ogg.h
# End Source File
# Begin Source File

SOURCE=.\Sound\A9Codec_wav.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\a9codec_wav.h
# End Source File
# Begin Source File

SOURCE=.\Sound\A9Codec_ym.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\a9codec_ym.h
# End Source File
# Begin Source File

SOURCE=.\Sound\A9Def.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\a9def.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# ADD CPP /Yc

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Target
# End Project
