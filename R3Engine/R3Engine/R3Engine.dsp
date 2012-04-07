# Microsoft Developer Studio Project File - Name="R3Engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=R3Engine - Win32 Tool Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "R3Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "R3Engine.mak" CFG="R3Engine - Win32 Tool Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "R3Engine - Win32 Client Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "R3Engine - Win32 S Debug NoSound" (based on "Win32 (x86) Static Library")
!MESSAGE "R3Engine - Win32 S Release NoSound" (based on "Win32 (x86) Static Library")
!MESSAGE "R3Engine - Win32 RTMovie Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "R3Engine - Win32 RTMovie Release" (based on "Win32 (x86) Static Library")
!MESSAGE "R3Engine - Win32 Tool Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/R3Technology/Core/R3Engine", GAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "R3Engine - Win32 Client Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Client Debug"
# PROP BASE Intermediate_Dir "Client Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Client Debug"
# PROP Intermediate_Dir "Client Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I ".\Extension" /I ".\1stClass" /I ".\2ndClass" /I ".\Common" /I ".\D3d\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTMOVIE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"R3EngineDMT.lib"

!ELSEIF  "$(CFG)" == "R3Engine - Win32 S Debug NoSound"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "S Debug NoSound"
# PROP BASE Intermediate_Dir "S Debug NoSound"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "S Debug NoSound"
# PROP Intermediate_Dir "S Debug NoSound"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\Extension" /I ".\1stClass" /I ".\2ndClass" /I ".\Common" /I ".\D3d\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_SERVER_DEBUG_NO_SOUND" /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"R3EngineDMTDLL_NoSound.lib"

!ELSEIF  "$(CFG)" == "R3Engine - Win32 S Release NoSound"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "S Release NoSound"
# PROP BASE Intermediate_Dir "S Release NoSound"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "S Release NoSound"
# PROP Intermediate_Dir "S Release NoSound"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\Extension" /I ".\1stClass" /I ".\2ndClass" /I ".\Common" /I ".\D3d\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_SERVER_DEBUG_NO_SOUND" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"R3EngineMTDLL_NoSound.lib"

!ELSEIF  "$(CFG)" == "R3Engine - Win32 RTMovie Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RTMovie Debug"
# PROP BASE Intermediate_Dir "RTMovie Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "RTMovie Debug"
# PROP Intermediate_Dir "RTMovie Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\Extension" /I ".\1stClass" /I ".\2ndClass" /I ".\Common" /I ".\D3d\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"R3EngineD.lib"

!ELSEIF  "$(CFG)" == "R3Engine - Win32 RTMovie Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RTMovie Release"
# PROP BASE Intermediate_Dir "RTMovie Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RTMovie Release"
# PROP Intermediate_Dir "RTMovie Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\Extension" /I ".\1stClass" /I ".\2ndClass" /I ".\Common" /I ".\D3d\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"R3Engine.lib"

!ELSEIF  "$(CFG)" == "R3Engine - Win32 Tool Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "R3Engine___Win32_Tool_Debug"
# PROP BASE Intermediate_Dir "R3Engine___Win32_Tool_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "R3Engine___Win32_Tool_Debug"
# PROP Intermediate_Dir "R3Engine___Win32_Tool_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\Extension" /I ".\1stClass" /I ".\2ndClass" /I ".\Common" /I ".\D3d\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_SERVER_DEBUG_NO_SOUND" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\Extension" /I ".\1stClass" /I ".\2ndClass" /I ".\Common" /I ".\D3d\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTMOVIE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"R3EngineDMTDLL_NoSound.lib"
# ADD LIB32 /nologo /out:"R3EngineDMTDLL_NoSound.lib"

!ENDIF 

# Begin Target

# Name "R3Engine - Win32 Client Debug"
# Name "R3Engine - Win32 S Debug NoSound"
# Name "R3Engine - Win32 S Release NoSound"
# Name "R3Engine - Win32 RTMovie Debug"
# Name "R3Engine - Win32 RTMovie Release"
# Name "R3Engine - Win32 Tool Debug"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "Header (Common)"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\common\B_sort.h
# End Source File
# Begin Source File

SOURCE=.\common\C_Vector.h
# End Source File
# Begin Source File

SOURCE=.\common\CJcache.h
# End Source File
# Begin Source File

SOURCE=.\common\CommonUtil.h
# End Source File
# Begin Source File

SOURCE=.\common\JCONFIG.H
# End Source File
# Begin Source File

SOURCE=.\common\JError.h
# End Source File
# Begin Source File

SOURCE=.\common\Jmalloc.h
# End Source File
# Begin Source File

SOURCE=.\common\JMORECFG.H
# End Source File
# Begin Source File

SOURCE=.\common\Jpegfile.h
# End Source File
# Begin Source File

SOURCE=.\common\JPEGLIB.H
# End Source File
# Begin Source File

SOURCE=.\common\R3DInput.h
# End Source File
# Begin Source File

SOURCE=.\common\R3Input.h
# End Source File
# Begin Source File

SOURCE=.\common\R3Math.h
# End Source File
# Begin Source File

SOURCE=.\common\R3Profiler.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\common\B_sort.cpp
# End Source File
# Begin Source File

SOURCE=.\common\CJcache.cpp
# End Source File
# Begin Source File

SOURCE=.\common\CommonUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\common\JError.cpp
# End Source File
# Begin Source File

SOURCE=.\common\Jmalloc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\Jpegfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\R3DInput.cpp
# End Source File
# Begin Source File

SOURCE=.\common\R3Input.cpp
# End Source File
# Begin Source File

SOURCE=.\common\R3Math.cpp
# End Source File
# Begin Source File

SOURCE=.\common\R3Profiler.cpp
# End Source File
# End Group
# Begin Group "1st Class"

# PROP Default_Filter ""
# Begin Group "Header (1st Class)"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\1stClass\Animation.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\Collision.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\Core.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\CPathFinder.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\entity.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\MultiLayer.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\Procedural.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Alpha.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Bsp.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3D3d8.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3D3dTex.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3LightMap.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Material.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Particle.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Render.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3VertexBuffer.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3VertexShader.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\RenderFlag.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\RenderParticle.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\Skybox.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\SoundEntity.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\SpecialParticle.h
# End Source File
# Begin Source File

SOURCE=.\1stClass\ToolCollision.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\1stClass\Animation.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\Bsp.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\Collision.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\Core.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\CPathFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\entity.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\MultiLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\Procedural.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Alpha.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3D3d8.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3D3dTex.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3LightMap.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Material.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3Render.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3VertexBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\R3VertexShader.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\RenderEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\RenderFlag.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\RenderParticle.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\Skybox.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\SoundEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\SpecialParticle.cpp
# End Source File
# Begin Source File

SOURCE=.\1stClass\ToolCollision.cpp
# End Source File
# End Group
# Begin Group "2nd Class"

# PROP Default_Filter ""
# Begin Group "Header (2nd Class)"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\2ndClass\2DSprite.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\AniCamera.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\Console.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\ExtDummy.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\Level.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\MoveCamera.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3D3DAPP.H
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3d3dwrapper.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3EngineKernel.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Font.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Move.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Sound.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Text.h
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Util.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\2ndClass\2DSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\AniCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\Console.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\ExtDummy.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\Level.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\MoveCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3D3DAPP.CPP
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3D3DWrapper.CPP
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3EngineKernel.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Font.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Move.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Text.cpp
# End Source File
# Begin Source File

SOURCE=.\2ndClass\R3Util.cpp
# End Source File
# End Group
# Begin Group "Extension"

# PROP Default_Filter ""
# Begin Group "Header (Extension)"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\extension\CharacterObj.h
# End Source File
# Begin Source File

SOURCE=.\extension\CMagicScript.h
# End Source File
# Begin Source File

SOURCE=.\extension\CollisionUtil.h
# End Source File
# Begin Source File

SOURCE=.\extension\CSwordTrace.h
# End Source File
# Begin Source File

SOURCE=.\extension\EffectCore.h
# End Source File
# Begin Source File

SOURCE=.\extension\EffectEntityList.h
# End Source File
# Begin Source File

SOURCE=.\extension\effectutil.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\extension\ParticleList.h
# End Source File
# Begin Source File

SOURCE=.\extension\RTMovie.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\extension\CharacterObj.cpp
# End Source File
# Begin Source File

SOURCE=.\extension\CMagicScript.cpp
# End Source File
# Begin Source File

SOURCE=.\extension\CollisionUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\extension\CSwordTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\extension\EffectCore.cpp
# End Source File
# Begin Source File

SOURCE=.\extension\EffectEntityList.cpp
# End Source File
# Begin Source File

SOURCE=.\extension\ParticleList.cpp
# End Source File
# Begin Source File

SOURCE=.\extension\RTMovie.cpp
# End Source File
# End Group
# Begin Group "D3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\d3d\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d\include\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\d3d\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d\include\dxutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Logo_32.ico
# End Source File
# Begin Source File

SOURCE=.\MSS.H
# End Source File
# Begin Source File

SOURCE=.\winmain.rc
# End Source File
# Begin Source File

SOURCE=.\mss32.lib
# End Source File
# End Target
# End Project
