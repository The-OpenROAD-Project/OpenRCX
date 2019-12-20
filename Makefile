#
#
#
include ../Makefile.defs

CXXFLAGS+=-I../tmg $(STAINC)

LIBNAME= ext

TCLIF=ext.ti
SRCS=  \
        ext_pkg.c \
        extmain.cpp \
        ext.cpp \
	zroute_ext.cpp \
        ext_Tcl.cpp \
	extRCap.cpp \
	ext_test_wire.cpp \
	extCoords.cpp \
	extSpef.cpp \
	extSpefIn.cpp \
	netRC.cpp \
	extCC.cpp \
	ZguiExt.cpp \
	extRCmodel.cpp \
	extprocess.cpp \
	exttree.cpp \
	extmeasure.cpp \
	extBench.cpp \
	extplanes.cpp \
	extFlow.cpp \
	gseq.cpp \
	hierSpef.cpp \
	powerConn.cpp

CMAKE_INCLUDES=\
                ../sta/constraints \
                ../sta/dcalc \
                ../sta/graph \
                ../sta/liberty \
                ../sta/network \
                ../sta/parasitics \
                ../sta/sdf \
                ../sta/search \
                ../sta/sta_z \
                ../sta/util \

##############################################
# Add custom targets below the following line.
include ../Makefile.rules
