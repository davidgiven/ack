HOME = ../../..
DEFDIR = $(HOME)/lib/m2

SOURCES =	ASCII.def EM.def MathLib0.def Processes.def \
		RealInOut.def Storage.def Arguments.def Conversion.def \
		random.def Semaphores.def Unix.def RealConver.def \
		Strings.def InOut.def Terminal.def \
		Mathlib.def PascalIO.def Traps.def CSP.def \
		Epilogue.def Streams.def ArraySort.def StripUnix.def

all:
clean:
cmp:

pr:
		@pr Makefile $(SOURCES)
opr:
		make pr | opr

install:
		-mkdir $(DEFDIR)
		cp $(SOURCES) $(DEFDIR)
