HOME = ../../..
DEFDIR = $(HOME)/lib/m2

SOURCES =	ASCII.def FIFFEF.def MathLib0.def Processes.def \
		RealInOut.def Storage.def Arguments.def Conversion.def \
		random.def Semaphores.def Unix.def RealConver.def \
		Strings.def InOut.def Terminal.def TTY.def \
		Mathlib.def

all:

install:
		-mkdir $(DEFDIR)
		for i in $(SOURCES) ; do cp $$i $(DEFDIR)/$$i ; done
