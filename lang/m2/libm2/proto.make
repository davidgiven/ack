# $Id$

#PARAMS		do not remove this line!

DEF_DIR = $(TARGET_HOME)/lib/m2
SRC_DIR = $(SRC_HOME)/lang/m2/libm2

SOURCES =	ASCII.def EM.def MathLib0.def Processes.def \
		RealInOut.def Storage.def Arguments.def Conversion.def \
		random.def Semaphores.def Unix.def RealConver.def \
		Strings.def InOut.def Terminal.def \
		Mathlib.def PascalIO.def Traps.def CSP.def \
		Epilogue.def Streams.def ArraySort.def StripUnix.def \
		Termcap.def XXTermcap.def

all:
clean:
cmp:

pr:
		@pr $(SRC_DIR)/proto.make
		for i in $(SOURCES) ; do pr $(SRC_DIR)/$$i ; done

opr:
		make pr | opr

install:
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then if [ -d $(DEF_DIR) ] ; then : ; else mkdir $(DEF_DIR) ; fi ; \
		for i in $(SOURCES) ; do cp $(SRC_DIR)/$$i $(DEF_DIR)/$$i ; done ; \
		fi
