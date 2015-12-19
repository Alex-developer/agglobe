# Microsoft Developer Studio Generated NMAKE File, Based on xearth.dsp
!IF "$(CFG)" == ""
CFG=xearth - Win32 Debug
!MESSAGE No configuration specified. Defaulting to xearth - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "xearth - Win32 Release" && "$(CFG)" != "xearth - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xearth.mak" CFG="xearth - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xearth - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xearth - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "xearth - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\xearth.dll"


CLEAN :
	-@erase "$(INTDIR)\bmp.obj"
	-@erase "$(INTDIR)\dither.obj"
	-@erase "$(INTDIR)\extarr.obj"
	-@erase "$(INTDIR)\mapdata.obj"
	-@erase "$(INTDIR)\markers.obj"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\scan.obj"
	-@erase "$(INTDIR)\settings.obj"
	-@erase "$(INTDIR)\sunpos.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\winearth.res"
	-@erase "$(INTDIR)\xearth.obj"
	-@erase "$(INTDIR)\xearthwin.obj"
	-@erase "$(OUTDIR)\xearth.dll"
	-@erase "$(OUTDIR)\xearth.exp"
	-@erase "$(OUTDIR)\xearth.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_AFXDLL" /Fp"$(INTDIR)\xearth.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\winearth.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xearth.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\xearth.pdb" /machine:I386 /def:".\AgGlobe.def" /out:"$(OUTDIR)\xearth.dll" /implib:"$(OUTDIR)\xearth.lib" 
DEF_FILE= \
	".\AgGlobe.def"
LINK32_OBJS= \
	"$(INTDIR)\bmp.obj" \
	"$(INTDIR)\dither.obj" \
	"$(INTDIR)\extarr.obj" \
	"$(INTDIR)\mapdata.obj" \
	"$(INTDIR)\markers.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\scan.obj" \
	"$(INTDIR)\settings.obj" \
	"$(INTDIR)\sunpos.obj" \
	"$(INTDIR)\xearth.obj" \
	"$(INTDIR)\xearthwin.obj" \
	"$(INTDIR)\winearth.res"

"$(OUTDIR)\xearth.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\AgGlobe.dll" "$(OUTDIR)\xearth.bsc"


CLEAN :
	-@erase "$(INTDIR)\bmp.obj"
	-@erase "$(INTDIR)\bmp.sbr"
	-@erase "$(INTDIR)\dither.obj"
	-@erase "$(INTDIR)\dither.sbr"
	-@erase "$(INTDIR)\extarr.obj"
	-@erase "$(INTDIR)\extarr.sbr"
	-@erase "$(INTDIR)\mapdata.obj"
	-@erase "$(INTDIR)\mapdata.sbr"
	-@erase "$(INTDIR)\markers.obj"
	-@erase "$(INTDIR)\markers.sbr"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\render.sbr"
	-@erase "$(INTDIR)\scan.obj"
	-@erase "$(INTDIR)\scan.sbr"
	-@erase "$(INTDIR)\settings.obj"
	-@erase "$(INTDIR)\settings.sbr"
	-@erase "$(INTDIR)\sunpos.obj"
	-@erase "$(INTDIR)\sunpos.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\winearth.res"
	-@erase "$(INTDIR)\xearth.obj"
	-@erase "$(INTDIR)\xearth.sbr"
	-@erase "$(INTDIR)\xearthwin.obj"
	-@erase "$(INTDIR)\xearthwin.sbr"
	-@erase "$(OUTDIR)\AgGlobe.dll"
	-@erase "$(OUTDIR)\AgGlobe.exp"
	-@erase "$(OUTDIR)\AgGlobe.ilk"
	-@erase "$(OUTDIR)\AgGlobe.lib"
	-@erase "$(OUTDIR)\AgGlobe.pdb"
	-@erase "$(OUTDIR)\xearth.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\xearth.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\winearth.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xearth.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bmp.sbr" \
	"$(INTDIR)\dither.sbr" \
	"$(INTDIR)\extarr.sbr" \
	"$(INTDIR)\mapdata.sbr" \
	"$(INTDIR)\markers.sbr" \
	"$(INTDIR)\render.sbr" \
	"$(INTDIR)\scan.sbr" \
	"$(INTDIR)\settings.sbr" \
	"$(INTDIR)\sunpos.sbr" \
	"$(INTDIR)\xearth.sbr" \
	"$(INTDIR)\xearthwin.sbr"

"$(OUTDIR)\xearth.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\AgGlobe.pdb" /debug /machine:I386 /def:".\AgGlobe.def" /out:"$(OUTDIR)\AgGlobe.dll" /implib:"$(OUTDIR)\AgGlobe.lib" /pdbtype:sept 
DEF_FILE= \
	".\AgGlobe.def"
LINK32_OBJS= \
	"$(INTDIR)\bmp.obj" \
	"$(INTDIR)\dither.obj" \
	"$(INTDIR)\extarr.obj" \
	"$(INTDIR)\mapdata.obj" \
	"$(INTDIR)\markers.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\scan.obj" \
	"$(INTDIR)\settings.obj" \
	"$(INTDIR)\sunpos.obj" \
	"$(INTDIR)\xearth.obj" \
	"$(INTDIR)\xearthwin.obj" \
	"$(INTDIR)\winearth.res"

"$(OUTDIR)\AgGlobe.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("xearth.dep")
!INCLUDE "xearth.dep"
!ELSE 
!MESSAGE Warning: cannot find "xearth.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "xearth - Win32 Release" || "$(CFG)" == "xearth - Win32 Debug"
SOURCE=.\bmp.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\bmp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\bmp.obj"	"$(INTDIR)\bmp.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dither.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\dither.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\dither.obj"	"$(INTDIR)\dither.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\extarr.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\extarr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\extarr.obj"	"$(INTDIR)\extarr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mapdata.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\mapdata.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\mapdata.obj"	"$(INTDIR)\mapdata.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\markers.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\markers.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\markers.obj"	"$(INTDIR)\markers.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\render.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\render.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\render.obj"	"$(INTDIR)\render.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\scan.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\scan.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\scan.obj"	"$(INTDIR)\scan.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\settings.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\settings.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\settings.obj"	"$(INTDIR)\settings.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sunpos.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\sunpos.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\sunpos.obj"	"$(INTDIR)\sunpos.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\xearth.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\xearth.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\xearth.obj"	"$(INTDIR)\xearth.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\xearthwin.cpp

!IF  "$(CFG)" == "xearth - Win32 Release"


"$(INTDIR)\xearthwin.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xearth - Win32 Debug"


"$(INTDIR)\xearthwin.obj"	"$(INTDIR)\xearthwin.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\winearth.rc

"$(INTDIR)\winearth.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

