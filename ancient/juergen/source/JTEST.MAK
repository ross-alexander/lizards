# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Java Virtual Machine Java Workspace" 0x0809

!IF "$(CFG)" == ""
CFG=jtest - Java Virtual Machine Debug
!MESSAGE No configuration specified.  Defaulting to jtest - Java Virtual\
 Machine Debug.
!ENDIF 

!IF "$(CFG)" != "jtest - Java Virtual Machine Release" && "$(CFG)" !=\
 "jtest - Java Virtual Machine Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "jtest.mak" CFG="jtest - Java Virtual Machine Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jtest - Java Virtual Machine Release" (based on\
 "Java Virtual Machine Java Workspace")
!MESSAGE "jtest - Java Virtual Machine Debug" (based on\
 "Java Virtual Machine Java Workspace")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "jtest - Java Virtual Machine Debug"
JAVA=jvc.exe

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\LizCanvas.class" "$(OUTDIR)\Images.class"\
 "$(OUTDIR)\jtest.class" "$(OUTDIR)\ControlPanel.class"\
 "$(OUTDIR)\CtrlFrame.class" "$(OUTDIR)\ButtonCanvas.class"\
 "$(OUTDIR)\ImageButton.class"

CLEAN : 
	-@erase "$(INTDIR)\ButtonCanvas.class"
	-@erase "$(INTDIR)\ControlPanel.class"
	-@erase "$(INTDIR)\CtrlFrame.class"
	-@erase "$(INTDIR)\ImageButton.class"
	-@erase "$(INTDIR)\Images.class"
	-@erase "$(INTDIR)\jtest.class"
	-@erase "$(INTDIR)\LizCanvas.class"

# ADD BASE JAVA /O
# ADD JAVA /O

!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\LizCanvas.class" "$(OUTDIR)\Images.class"\
 "$(OUTDIR)\jtest.class" "$(OUTDIR)\ControlPanel.class"\
 "$(OUTDIR)\CtrlFrame.class" "$(OUTDIR)\ButtonCanvas.class"\
 "$(OUTDIR)\ImageButton.class"

CLEAN : 
	-@erase "$(INTDIR)\ButtonCanvas.class"
	-@erase "$(INTDIR)\ControlPanel.class"
	-@erase "$(INTDIR)\CtrlFrame.class"
	-@erase "$(INTDIR)\ImageButton.class"
	-@erase "$(INTDIR)\Images.class"
	-@erase "$(INTDIR)\jtest.class"
	-@erase "$(INTDIR)\LizCanvas.class"

# ADD BASE JAVA /g
# ADD JAVA /g

!ENDIF 

################################################################################
# Begin Target

# Name "jtest - Java Virtual Machine Release"
# Name "jtest - Java Virtual Machine Debug"

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\LizCanvas.java

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"


"$(INTDIR)\LizCanvas.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\LizCanvas.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\images.java

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"


"$(INTDIR)\Images.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\Images.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jtest.java

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"


"$(INTDIR)\jtest.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\ControlPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\CtrlFrame.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\jtest.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\ControlPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\CtrlFrame.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jtest.html

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ButtonCanvas.java

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"


"$(INTDIR)\ButtonCanvas.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\ImageButton.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\ButtonCanvas.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\ImageButton.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
