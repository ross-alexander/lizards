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

ALL : "$(OUTDIR)\Images.class" "$(OUTDIR)\jtest.class"\
 "$(OUTDIR)\CtrlPanel.class" "$(OUTDIR)\AccountPanel.class"\
 "$(OUTDIR)\HelpPanel.class" "$(OUTDIR)\BorderRightTopJoinPanel.class"\
 "$(OUTDIR)\BorderLeftTopJoinPanel.class" "$(OUTDIR)\BorderTopJoinPanel.class"\
 "$(OUTDIR)\BorderRightBottomJoinPanel.class"\
 "$(OUTDIR)\BorderLeftBottomJoinPanel.class"\
 "$(OUTDIR)\BorderBottomJoinPanel.class" "$(OUTDIR)\BorderPanel.class"\
 "$(OUTDIR)\Border3D.class" "$(OUTDIR)\ButtonCanvas.class"\
 "$(OUTDIR)\ImageButton.class" "$(OUTDIR)\LizSetupWorld.class"\
 "$(OUTDIR)\LizGameWorld.class" "$(OUTDIR)\LizWorld.class"\
 "$(OUTDIR)\LizHex.class" "$(OUTDIR)\LizHexConstants.class"\
 "$(OUTDIR)\ButtonConstants.class" "$(OUTDIR)\LizConstants.class"\
 "$(OUTDIR)\EntryPanel.class"

CLEAN : 
	-@erase "$(INTDIR)\AccountPanel.class"
	-@erase "$(INTDIR)\Border3D.class"
	-@erase "$(INTDIR)\BorderBottomJoinPanel.class"
	-@erase "$(INTDIR)\BorderLeftBottomJoinPanel.class"
	-@erase "$(INTDIR)\BorderLeftTopJoinPanel.class"
	-@erase "$(INTDIR)\BorderPanel.class"
	-@erase "$(INTDIR)\BorderRightBottomJoinPanel.class"
	-@erase "$(INTDIR)\BorderRightTopJoinPanel.class"
	-@erase "$(INTDIR)\BorderTopJoinPanel.class"
	-@erase "$(INTDIR)\ButtonCanvas.class"
	-@erase "$(INTDIR)\ButtonConstants.class"
	-@erase "$(INTDIR)\CtrlPanel.class"
	-@erase "$(INTDIR)\EntryPanel.class"
	-@erase "$(INTDIR)\HelpPanel.class"
	-@erase "$(INTDIR)\ImageButton.class"
	-@erase "$(INTDIR)\Images.class"
	-@erase "$(INTDIR)\jtest.class"
	-@erase "$(INTDIR)\LizConstants.class"
	-@erase "$(INTDIR)\LizGameWorld.class"
	-@erase "$(INTDIR)\LizHex.class"
	-@erase "$(INTDIR)\LizHexConstants.class"
	-@erase "$(INTDIR)\LizSetupWorld.class"
	-@erase "$(INTDIR)\LizWorld.class"

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

ALL : "$(OUTDIR)\Images.class" "$(OUTDIR)\jtest.class"\
 "$(OUTDIR)\CtrlPanel.class" "$(OUTDIR)\AccountPanel.class"\
 "$(OUTDIR)\HelpPanel.class" "$(OUTDIR)\BorderRightTopJoinPanel.class"\
 "$(OUTDIR)\BorderLeftTopJoinPanel.class" "$(OUTDIR)\BorderTopJoinPanel.class"\
 "$(OUTDIR)\BorderRightBottomJoinPanel.class"\
 "$(OUTDIR)\BorderLeftBottomJoinPanel.class"\
 "$(OUTDIR)\BorderBottomJoinPanel.class" "$(OUTDIR)\BorderPanel.class"\
 "$(OUTDIR)\Border3D.class" "$(OUTDIR)\ButtonCanvas.class"\
 "$(OUTDIR)\ImageButton.class" "$(OUTDIR)\LizWorld.class"\
 "$(OUTDIR)\LizHex.class" "$(OUTDIR)\LizHexConstants.class"\
 "$(OUTDIR)\ButtonConstants.class" "$(OUTDIR)\LizConstants.class"\
 "$(OUTDIR)\EntryPanel.class" "$(OUTDIR)\LizSetupWorld.class"\
 "$(OUTDIR)\LizGameWorld.class"

CLEAN : 
	-@erase "$(INTDIR)\AccountPanel.class"
	-@erase "$(INTDIR)\Border3D.class"
	-@erase "$(INTDIR)\BorderBottomJoinPanel.class"
	-@erase "$(INTDIR)\BorderLeftBottomJoinPanel.class"
	-@erase "$(INTDIR)\BorderLeftTopJoinPanel.class"
	-@erase "$(INTDIR)\BorderPanel.class"
	-@erase "$(INTDIR)\BorderRightBottomJoinPanel.class"
	-@erase "$(INTDIR)\BorderRightTopJoinPanel.class"
	-@erase "$(INTDIR)\BorderTopJoinPanel.class"
	-@erase "$(INTDIR)\ButtonCanvas.class"
	-@erase "$(INTDIR)\ButtonConstants.class"
	-@erase "$(INTDIR)\CtrlPanel.class"
	-@erase "$(INTDIR)\EntryPanel.class"
	-@erase "$(INTDIR)\HelpPanel.class"
	-@erase "$(INTDIR)\ImageButton.class"
	-@erase "$(INTDIR)\Images.class"
	-@erase "$(INTDIR)\jtest.class"
	-@erase "$(INTDIR)\LizConstants.class"
	-@erase "$(INTDIR)\LizGameWorld.class"
	-@erase "$(INTDIR)\LizHex.class"
	-@erase "$(INTDIR)\LizHexConstants.class"
	-@erase "$(INTDIR)\LizSetupWorld.class"
	-@erase "$(INTDIR)\LizWorld.class"

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

"$(INTDIR)\CtrlPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\AccountPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\HelpPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderRightTopJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderLeftTopJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderTopJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderRightBottomJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderLeftBottomJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderBottomJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\Border3D.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\jtest.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\CtrlPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\AccountPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\HelpPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderRightTopJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderLeftTopJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderTopJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderRightBottomJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderLeftBottomJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderBottomJoinPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\BorderPanel.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\Border3D.class" : $(SOURCE) "$(INTDIR)"


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
################################################################################
# Begin Source File

SOURCE=.\LizLoader.java

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"


"$(INTDIR)\LizSetupWorld.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\LizGameWorld.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\LizWorld.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\LizHex.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\LizHexConstants.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\LizSetupWorld.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\LizGameWorld.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\LizWorld.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\LizHex.class" : $(SOURCE) "$(INTDIR)"

"$(INTDIR)\LizHexConstants.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ButtonConstants.java

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"


"$(INTDIR)\ButtonConstants.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\ButtonConstants.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LizConstants.java

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"


"$(INTDIR)\LizConstants.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\LizConstants.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\entrypnl.java

!IF  "$(CFG)" == "jtest - Java Virtual Machine Release"


"$(INTDIR)\EntryPanel.class" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jtest - Java Virtual Machine Debug"


"$(INTDIR)\EntryPanel.class" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
