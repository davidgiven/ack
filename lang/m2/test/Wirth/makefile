IFLAGS =
M2FLAGS = 
MOD = ack
SUFFIX = o

all:	PowersOf2 XREF 

PowersOf2.$(SUFFIX):	PowersOf2.mod /proj/em/Work/lib/m2/InOut.def
	$(MOD) -c $(M2FLAGS) $(IFLAGS) PowersOf2.mod
TableHandl.$(SUFFIX):	TableHandl.mod TableHandl.def /proj/em/Work/lib/m2/InOut.def /proj/em/Work/lib/m2/Storage.def
	$(MOD) -c $(M2FLAGS) $(IFLAGS) TableHandl.mod
XREF.$(SUFFIX):	XREF.mod /proj/em/Work/lib/m2/InOut.def TableHandl.def
	$(MOD) -c $(M2FLAGS) $(IFLAGS) XREF.mod

OBS_PowersOf2 = \
	PowersOf2.$(SUFFIX)

PowersOf2:	$(OBS_PowersOf2)
	$(MOD) -.mod -o PowersOf2 $(M2FLAGS) $(OBS_PowersOf2)

OBS_XREF = \
	XREF.$(SUFFIX)\
	TableHandl.$(SUFFIX)

XREF:	$(OBS_XREF)
	$(MOD) -.mod -o XREF $(M2FLAGS) $(OBS_XREF)
