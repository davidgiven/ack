OBJDIR = .obj
PREFIX = /usr/local
CFLAGS = \
	-Os -g \
	-DNON_CORRECTING \
	-DLIBDIR=\"$(PREFIX)/share/LLgen/\"

SRCS = \
	src/alloc.c \
	src/cclass.c \
	src/check.c \
	src/compute.c \
	src/gencode.c \
	src/global.c \
	src/LLgen.c \
	src/Lpars.c \
	src/machdep.c \
	src/main.c \
	src/name.c \
	src/reach.c \
	src/savegram.c \
	src/sets.c \
	src/tokens.c

OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))
DEPS = $(patsubst %.c, $(OBJDIR)/%.d, $(SRCS))

LLgen: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(OBJDIR)/%.pdf: doc/%.n doc/LLgen.refs
	refer -p doc/LLgen.refs $^ | groff -G -e -g -p -ms -Tpdf > $@

clean:
	rm -rf $(OBJDIR)

install: LLgen doc/LLgen.1 $(OBJDIR)/LLgen.pdf $(OBJDIR)/LLgen_NCER.pdf $(wildcard lib/*)
	install -D -s LLgen                    -T $(PREFIX)/bin/LLgen
	install -D    doc/LLgen.1              -T $(PREFIX)/man/man1/LLgen.1
	install -D    $(OBJDIR)/LLgen.pdf      -T $(PREFIX)/doc/LLgen
	install -D    $(OBJDIR)/LLgen_NCER.pdf -T $(PREFIX)/doc/LLgen
	install -D    $(wildcard lib/*)        -t $(PREFIX)/share/LLgen
	
-include $(DEPS)

