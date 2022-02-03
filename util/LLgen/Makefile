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

DOCS = \
	doc/LLgen.n \
	doc/LLgen_NCER.n \

OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))
DEPS = $(patsubst %.c, $(OBJDIR)/%.d, $(SRCS))
PDFS = $(patsubst doc/%.n, $(OBJDIR)/%.pdf, $(DOCS))

all: LLgen doc

LLgen: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(OBJDIR)/%.pdf: doc/%.n doc/LLgen.refs
	@mkdir -p $(dir $@)
	groff -R -G -e -g -p -ms -Tpdf $< > $@

doc:: $(PDFS)

clean:
	rm -rf $(OBJDIR)

install: LLgen doc/LLgen.1 $(PDFS) $(wildcard lib/*)
	install -D -s LLgen                    -t $(PREFIX)/bin
	install -D    doc/LLgen.1              -t $(PREFIX)/man/man1
	install -D    $(OBJDIR)/LLgen.pdf      -t $(PREFIX)/doc/LLgen
	install -D    $(OBJDIR)/LLgen_NCER.pdf -t $(PREFIX)/doc/LLgen
	install -D    $(wildcard lib/*)        -t $(PREFIX)/share/LLgen
	
-include $(DEPS)

