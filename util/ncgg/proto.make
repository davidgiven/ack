# Header: Makefile,v 0.8 87/11/16 10:35:32 ceriel Exp $

#PARAMS		do not remove this line!

CFILES=	cgg.c $(SRC_DIR)/subr.c $(SRC_DIR)/main.c $(SRC_DIR)/coerc.c \
	enterkeyw.c $(SRC_DIR)/error.c $(SRC_DIR)/emlookup.c $(SRC_DIR)/expr.c \
	$(SRC_DIR)/instruct.c $(SRC_DIR)/iocc.c $(SRC_DIR)/lookup.c \
	$(SRC_DIR)/output.c $(SRC_DIR)/set.c $(SRC_DIR)/strlookup.c \
	$(SRC_DIR)/var.c $(SRC_DIR)/hall.c
OFILES= cgg.$(SUF) subr.$(SUF) main.$(SUF) coerc.$(SUF) enterkeyw.$(SUF) \
	error.$(SUF) emlookup.$(SUF) expr.$(SUF) instruct.$(SUF) iocc.$(SUF) \
	lookup.$(SUF) set.$(SUF) strlookup.$(SUF) var.$(SUF) hall.$(SUF)
SOURCES=$(SRC_DIR)/*.h $(SRC_DIR)/cgg.y $(SRC_DIR)/scan.l \
	$(SRC_DIR)/cvtkeywords $(SRC_DIR)/keywords $(SRC_DIR)/coerc.c \
	$(SRC_DIR)/emlookup.c $(SRC_DIR)/error.c $(SRC_DIR)/expr.c \
	$(SRC_DIR)/hall.c $(SRC_DIR)/instruct.c $(SRC_DIR)/iocc.c \
	$(SRC_DIR)/lookup.c $(SRC_DIR)/main.c $(SRC_DIR)/output.c \
	$(SRC_DIR)/set.c $(SRC_DIR)/strlookup.c $(SRC_DIR)/subr.c \
	$(SRC_DIR)/var.c

SRC_DIR=$(SRC_HOME)/util/ncgg
EMH=$(TARGET_HOME)/h
MANDIR=$(TARGET_HOME)/man
INCLUDES=-I$(EMH) -I$(TARGET_HOME)/config -I$(SRC_DIR) -I.
CFLAGS=$(INCLUDES) $(COPTIONS)
YFLAGS=-d
LDFLAGS=$(LDOPTIONS)
LINTFLAGS=$(INCLUDES) $(LINTOPTIONS)


cgg:	cgg.$(SUF) $(OFILES) output.$(SUF)
	$(CC) $(LDFLAGS) $(OFILES) output.$(SUF) $(TARGET_HOME)/lib.bin/em_data.$(LIBSUF) -o cgg

install:	cgg
	rm -f $(TARGET_HOME)/lib.bin/ncgg
	cp cgg $(TARGET_HOME)/lib.bin/ncgg
	rm -f $(MANDIR)/ncgg.6
	cp $(SRC_DIR)/ncgg.6 $(MANDIR)/ncgg.6

cmp:	cgg
	-cmp cgg $(TARGET_HOME)/lib.bin/ncgg
	-cmp $(SRC_DIR)/ncgg.6 $(MANDIR)/ncgg.6

debugcgg:	cgg.$(SUF) $(OFILES) debugoutput.$(SUF)
	$(CC) $(LDFLAGS) $(OFILES) debugoutput.$(SUF) $(TARGET_HOME)/lib.bin/em_data.$(LIBSUF) -o cgg

cgg.c:	$(SRC_DIR)/cgg.y
	yacc $(YFLAGS) $(SRC_DIR)/cgg.y && mv y.tab.c cgg.c

enterkeyw.c:	$(SRC_DIR)/cvtkeywords $(SRC_DIR)/keywords y.tab.h
	$(SRC_DIR)/cvtkeywords $(SRC_DIR)/keywords

debugoutput.$(SUF):	debugoutput.c
	$(CC) $(CFLAGS) -DCODEDEBUG -c debugoutput.c

debugoutput.c:	$(SRC_DIR)/output.c
	cp $(SRC_DIR)/output.c debugoutput.c

lint:	$(CFILES)
	$(LINT) $(LINTFLAGS) $(CFILES)

clean:
	rm -f cgg.c scan.c y.output y.tab.h enterkeyw.c debugoutput.c
	rm -f $(OFILES) output.$(SUF) debugoutput.$(SUF) cgg lint

scan.c:	$(SRC_DIR)/scan.l
	flex -st $(SRC_DIR)/scan.l > scan.c

pr:
	@pr $(SOURCES)

opr:
	-make pr|opr

depend: $(CFILES) scan.c
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(INCLUDES) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

#DEPENDENCIES
cgg.$(SUF):	cgg.c
	$(CC) -c $(CFLAGS) cgg.c
cgg.$(SUF):	scan.c
cgg.$(SUF):	$(TARGET_HOME)/h/em_reg.h
cgg.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
cgg.$(SUF):	$(SRC_DIR)/extern.h
cgg.$(SUF):	$(SRC_DIR)/expr.h
cgg.$(SUF):	$(SRC_DIR)/cost.h
cgg.$(SUF):	$(SRC_DIR)/instruct.h
cgg.$(SUF):	$(SRC_DIR)/iocc.h
cgg.$(SUF):	$(SRC_DIR)/set.h
cgg.$(SUF):	$(SRC_DIR)/lookup.h
cgg.$(SUF):	$(SRC_DIR)/varinfo.h
cgg.$(SUF):	$(TARGET_HOME)/h/em_spec.h
cgg.$(SUF):	$(TARGET_HOME)/config/local.h
cgg.$(SUF):	$(SRC_DIR)/param.h
subr.$(SUF):	$(SRC_DIR)/subr.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/subr.c
subr.$(SUF):	$(SRC_DIR)/extern.h
subr.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
subr.$(SUF):	$(SRC_DIR)/regvar.h
subr.$(SUF):	$(SRC_DIR)/token.h
subr.$(SUF):	$(SRC_DIR)/cost.h
subr.$(SUF):	$(SRC_DIR)/instruct.h
subr.$(SUF):	$(SRC_DIR)/varinfo.h
subr.$(SUF):	$(SRC_DIR)/set.h
subr.$(SUF):	$(SRC_DIR)/expr.h
subr.$(SUF):	$(SRC_DIR)/property.h
subr.$(SUF):	$(SRC_DIR)/lookup.h
subr.$(SUF):	$(SRC_DIR)/reg.h
subr.$(SUF):	$(TARGET_HOME)/h/em_spec.h
subr.$(SUF):	$(TARGET_HOME)/config/local.h
subr.$(SUF):	$(SRC_DIR)/param.h
main.$(SUF):	$(SRC_DIR)/main.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
coerc.$(SUF):	$(SRC_DIR)/coerc.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/coerc.c
coerc.$(SUF):	$(SRC_DIR)/extern.h
coerc.$(SUF):	$(SRC_DIR)/pseudo.h
coerc.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
coerc.$(SUF):	$(SRC_DIR)/iocc.h
coerc.$(SUF):	$(SRC_DIR)/varinfo.h
coerc.$(SUF):	$(SRC_DIR)/cost.h
coerc.$(SUF):	$(SRC_DIR)/token.h
coerc.$(SUF):	$(SRC_DIR)/reg.h
coerc.$(SUF):	$(SRC_DIR)/property.h
coerc.$(SUF):	$(SRC_DIR)/set.h
coerc.$(SUF):	$(TARGET_HOME)/h/em_spec.h
coerc.$(SUF):	$(TARGET_HOME)/config/local.h
coerc.$(SUF):	$(SRC_DIR)/param.h
coerc.$(SUF):	$(SRC_DIR)/assert.h
enterkeyw.$(SUF):	enterkeyw.c
	$(CC) -c $(CFLAGS) enterkeyw.c
enterkeyw.$(SUF):	$(SRC_DIR)/lookup.h
error.$(SUF):	$(SRC_DIR)/error.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/error.c
emlookup.$(SUF):	$(SRC_DIR)/emlookup.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/emlookup.c
emlookup.$(SUF):	$(TARGET_HOME)/h/em_flag.h
emlookup.$(SUF):	$(SRC_DIR)/expr.h
emlookup.$(SUF):	$(TARGET_HOME)/h/em_spec.h
emlookup.$(SUF):	$(TARGET_HOME)/config/local.h
emlookup.$(SUF):	$(SRC_DIR)/param.h
expr.$(SUF):	$(SRC_DIR)/expr.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/expr.c
expr.$(SUF):	$(SRC_DIR)/extern.h
expr.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
expr.$(SUF):	$(SRC_DIR)/regvar.h
expr.$(SUF):	$(SRC_DIR)/expr.h
expr.$(SUF):	$(SRC_DIR)/property.h
expr.$(SUF):	$(SRC_DIR)/cost.h
expr.$(SUF):	$(SRC_DIR)/token.h
expr.$(SUF):	$(SRC_DIR)/lookup.h
expr.$(SUF):	$(SRC_DIR)/reg.h
expr.$(SUF):	$(SRC_DIR)/set.h
expr.$(SUF):	$(TARGET_HOME)/h/em_spec.h
expr.$(SUF):	$(TARGET_HOME)/config/local.h
expr.$(SUF):	$(SRC_DIR)/param.h
expr.$(SUF):	$(SRC_DIR)/assert.h
instruct.$(SUF):	$(SRC_DIR)/instruct.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/instruct.c
instruct.$(SUF):	$(SRC_DIR)/extern.h
instruct.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
instruct.$(SUF):	$(SRC_DIR)/iocc.h
instruct.$(SUF):	$(SRC_DIR)/expr.h
instruct.$(SUF):	$(SRC_DIR)/set.h
instruct.$(SUF):	$(SRC_DIR)/varinfo.h
instruct.$(SUF):	$(SRC_DIR)/pseudo.h
instruct.$(SUF):	$(SRC_DIR)/cost.h
instruct.$(SUF):	$(SRC_DIR)/instruct.h
instruct.$(SUF):	$(TARGET_HOME)/h/em_spec.h
instruct.$(SUF):	$(TARGET_HOME)/config/local.h
instruct.$(SUF):	$(SRC_DIR)/param.h
iocc.$(SUF):	$(SRC_DIR)/iocc.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/iocc.c
iocc.$(SUF):	$(SRC_DIR)/extern.h
iocc.$(SUF):	$(SRC_DIR)/regvar.h
iocc.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
iocc.$(SUF):	$(SRC_DIR)/iocc.h
iocc.$(SUF):	$(SRC_DIR)/property.h
iocc.$(SUF):	$(SRC_DIR)/cost.h
iocc.$(SUF):	$(SRC_DIR)/token.h
iocc.$(SUF):	$(SRC_DIR)/lookup.h
iocc.$(SUF):	$(SRC_DIR)/expr.h
iocc.$(SUF):	$(SRC_DIR)/set.h
iocc.$(SUF):	$(TARGET_HOME)/h/em_spec.h
iocc.$(SUF):	$(TARGET_HOME)/config/local.h
iocc.$(SUF):	$(SRC_DIR)/param.h
iocc.$(SUF):	$(SRC_DIR)/assert.h
lookup.$(SUF):	$(SRC_DIR)/lookup.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/lookup.c
lookup.$(SUF):	$(SRC_DIR)/lookup.h
lookup.$(SUF):	$(TARGET_HOME)/h/em_spec.h
lookup.$(SUF):	$(TARGET_HOME)/config/local.h
lookup.$(SUF):	$(SRC_DIR)/param.h
lookup.$(SUF):	$(SRC_DIR)/assert.h
output.$(SUF):	$(SRC_DIR)/output.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/output.c
output.$(SUF):	$(SRC_DIR)/extern.h
output.$(SUF):	$(SRC_DIR)/regvar.h
output.$(SUF):	$(SRC_DIR)/pseudo.h
output.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
output.$(SUF):	$(SRC_DIR)/lookup.h
output.$(SUF):	$(SRC_DIR)/instruct.h
output.$(SUF):	$(SRC_DIR)/set.h
output.$(SUF):	$(SRC_DIR)/cost.h
output.$(SUF):	$(SRC_DIR)/token.h
output.$(SUF):	$(SRC_DIR)/property.h
output.$(SUF):	$(SRC_DIR)/reg.h
output.$(SUF):	$(TARGET_HOME)/h/em_spec.h
output.$(SUF):	$(TARGET_HOME)/config/local.h
output.$(SUF):	$(SRC_DIR)/param.h
output.$(SUF):	$(SRC_DIR)/varinfo.h
output.$(SUF):	$(SRC_DIR)/assert.h
set.$(SUF):	$(SRC_DIR)/set.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/set.c
set.$(SUF):	$(SRC_DIR)/extern.h
set.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
set.$(SUF):	$(SRC_DIR)/reg.h
set.$(SUF):	$(SRC_DIR)/lookup.h
set.$(SUF):	$(SRC_DIR)/cost.h
set.$(SUF):	$(SRC_DIR)/token.h
set.$(SUF):	$(SRC_DIR)/set.h
set.$(SUF):	$(SRC_DIR)/property.h
set.$(SUF):	$(TARGET_HOME)/h/em_spec.h
set.$(SUF):	$(TARGET_HOME)/config/local.h
set.$(SUF):	$(SRC_DIR)/param.h
strlookup.$(SUF):	$(SRC_DIR)/strlookup.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/strlookup.c
strlookup.$(SUF):	$(TARGET_HOME)/h/em_spec.h
strlookup.$(SUF):	$(TARGET_HOME)/config/local.h
strlookup.$(SUF):	$(SRC_DIR)/param.h
var.$(SUF):	$(SRC_DIR)/var.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/var.c
var.$(SUF):	$(TARGET_HOME)/h/cgg_cg.h
var.$(SUF):	$(SRC_DIR)/lookup.h
var.$(SUF):	$(SRC_DIR)/instruct.h
var.$(SUF):	$(SRC_DIR)/set.h
var.$(SUF):	$(SRC_DIR)/cost.h
var.$(SUF):	$(SRC_DIR)/token.h
var.$(SUF):	$(SRC_DIR)/property.h
var.$(SUF):	$(SRC_DIR)/reg.h
var.$(SUF):	$(TARGET_HOME)/h/em_spec.h
var.$(SUF):	$(TARGET_HOME)/config/local.h
var.$(SUF):	$(SRC_DIR)/param.h
hall.$(SUF):	$(SRC_DIR)/hall.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/hall.c
hall.$(SUF):	$(SRC_DIR)/set.h
hall.$(SUF):	$(TARGET_HOME)/h/em_spec.h
hall.$(SUF):	$(TARGET_HOME)/config/local.h
hall.$(SUF):	$(SRC_DIR)/param.h
hall.$(SUF):	$(SRC_DIR)/assert.h
