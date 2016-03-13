define reset
	$(eval q :=)
	$(eval o :=)
	$(eval s :=)
	$(eval cflags :=)
	$(eval ackflags :=)
	$(eval ldflags :=)
	$(eval objdir :=)
endef

# --- Host compiler

define cfile-rule
$o: $s
	@echo CC $o
	@mkdir -p $(dir $o)
	@$(CC) $(CFLAGS) $(cflags) -MM -MQ $o -o $d $s
	$(hide) $(CC) $(CFLAGS) $(cflags) -c -o $o $s
endef

define cfile
	$(eval s := $1)
	$(eval o := $(OBJDIR)/$(objdir)/$(1:.c=.o))
	$(eval d := $(OBJDIR)/$(objdir)/$(1:.c=.d))
	$(eval DEPENDS += $d)
	$(eval CLEANABLES += $o $d)
	$(eval q += $o)
	$(eval $(cfile-rule))
endef

# --- ACK compiler

define ackfile-rule
$o: $s $(ACK) \
		$(CCOMPILER) \
		$(PLATFORM_$(PLATFORM)) \
		$(ACK_CORE_TOOLS)
	@echo ACK $o
	@mkdir -p $(dir $o)
	$(hide) ACKDIR=$(INSDIR) $(ACK) $(ACKFLAGS) $(ackflags) -m$(PLATFORM) -c -o $o $s
endef

define ackfile
	$(eval s := $1)
	$(eval o := $(OBJDIR)/$(objdir)/$(basename $1).o)
	$(eval d := $(OBJDIR)/$(objdir)/$(basename $1).d)
	$(eval DEPENDS += $d)
	$(eval CLEANABLES += $o $d)
	$(eval q += $o)
	$(eval $(ackfile-rule))
endef

# --- Add a raw to the queue

define rawfile
	$(eval q += $1)
endef

# --- Host compiler linking

define cprogram-rule
$o: $s
	@echo CPROGRAM $o
	@mkdir -p $(dir $o)
	$(hide) $(CC) $(CFLAGS) $(cflags) $(LDFLAGS) $(ldflags) -o $o $s
endef

define cprogram
	$(eval o := $1)
	$(eval s := $q)
	$(eval CLEANABLES += $o)
	$(eval q := $o)
	$(eval $(cprogram-rule))
endef

# --- Host library linking

define clibrary-rule
$o: $s
	@echo CLIBRARY $o
	@mkdir -p $(dir $o)
	@$(RM) $o
	$(hide) $(AR) qsc $o $s
endef

define clibrary
	$(eval o := $1)
	$(eval s := $q)
	$(eval CLEANABLES += $o)
	$(eval q := $o)
	$(eval $(clibrary-rule))
endef

# --- ACK library linking

define acklibrary-rule
$o: $s $(AAL)
	@echo ACKLIBRARY $o
	@mkdir -p $(dir $o)
	@$(RM) $o
	$(hide) $(AAL) q $o $s
endef

define acklibrary
	$(eval o := $1)
	$(eval s := $q)
	$(eval CLEANABLES += $o)
	$(eval q := $o)
	$(eval $(acklibrary-rule))
endef

# --- Copies a file.

define copyto-rule
$o: $s
	@echo CP $o
	@mkdir -p $(dir $o)
	$(hide) $(CP) $s $o
endef

define copyto
	$(eval o := $1)
	$(eval s := $q)
	$(eval CLEANABLES += $o)
	$(eval q := $o)
	$(eval $(copyto-rule))
endef

# --- Installs a file (copies it and adds it to INSTALLABLES).

define installto-rule
$o: $s
	@echo INSTALL $o
	@mkdir -p $(dir $o)
	$(hide) $(CP) $s $o
endef

define installto
	$(eval o := $1)
	$(eval s := $q)
	$(eval CLEANABLES += $o)
	$(eval INSTALLABLES += $o)
	$(eval q := $o)
	$(eval $(installto-rule))
endef

# --- Adds a dependency for the last object file

define dependson
	$(eval $o: $1)
endef

# --- Runs yacc.

#  $1: directory to put output files
#  $2: input files
#
# Output files are compiled via cfile and queued.

define yacc-impl
	$(eval o := $1/y.tab.c)
	$(eval CLEANABLES += $o $1/y.tab.h)

$o: $2
	@echo YACC $o
	@mkdir -p $(dir $o)
	$(hide) yacc -t -b $1/y -d $2

	$(call cfile, $o)

$1/y.tab.h: $o

endef

yacc = $(eval $(call yacc-impl,$1,$2))

# --- Runs flex.

#  $1: directory to put output files
#  $2: input files
#
# Output files are compiled via cfile and queued.

define flex-impl

$(eval o := $1/lex.yy.c)
$(eval CLEANABLES += $o)

$o: $2
	@echo FLEX $o
	@mkdir -p $(dir $o)
	$(hide) flex -s -t $2 > $o

$(call cfile, $o)

endef

flex = $(eval $(call flex-impl,$1,$2))

