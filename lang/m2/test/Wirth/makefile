IFLAGS =
M2FLAGS = 
MOD = m2
SUFFIX = o
LIBS = 

all:	PowersOf2 XREF 

PowersOf2.$(SUFFIX): \
	PowersOf2.mod \
	/proj/em/Work/lib/m2/InOut.def
	$(MOD) -c.$(SUFFIX) $(M2FLAGS) $(IFLAGS) PowersOf2.mod
TableHandl.$(SUFFIX): \
	TableHandl.mod \
	TableHandl.def \
	/proj/em/Work/lib/m2/InOut.def \
	/proj/em/Work/lib/m2/Storage.def
	$(MOD) -c.$(SUFFIX) $(M2FLAGS) $(IFLAGS) TableHandl.mod
XREF.$(SUFFIX): \
	XREF.mod \
	/proj/em/Work/lib/m2/InOut.def \
	TableHandl.def
	$(MOD) -c.$(SUFFIX) $(M2FLAGS) $(IFLAGS) XREF.mod

OBS_PowersOf2 = \
	PowersOf2.$(SUFFIX)

OBS2_PowersOf2 =

o_files:	$(OBS_PowersOf2)

PowersOf2:	$(OBS_PowersOf2) $(OBS2_PowersOf2)
	$(MOD) -o PowersOf2 $(M2FLAGS) $(OBS_PowersOf2) $(OBS2_PowersOf2) $(LIBS)

OBS_XREF = \
	XREF.$(SUFFIX) \
	TableHandl.$(SUFFIX)

OBS2_XREF =

o_files:	$(OBS_XREF)

XREF:	$(OBS_XREF) $(OBS2_XREF)
	$(MOD) -o XREF $(M2FLAGS) $(OBS_XREF) $(OBS2_XREF) $(LIBS)
